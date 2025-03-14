package nft

import (
    "encoding/json"
    "errors"
    "sync"
    "time"
)

// NFTSystem manages the NFT functionality in the blockchain
type NFTSystem struct {
    // Map of NFT ID to NFT
    NFTs map[string]*NFT
    
    // Next NFT ID
    NextID int
    
    // Mutex for thread safety
    mutex sync.Mutex
    
    // Master wallet address for fees
    MasterWalletAddress string
    
    // Transaction fee percentage
    TransactionFeeRate float64
}

// NFT represents a non-fungible token
type NFT struct {
    ID          string                 `json:"id"`
    Type        string                 `json:"type"` // "champion_skin", "yield_generator", etc.
    Owner       string                 `json:"owner"`
    Creator     string                 `json:"creator"`
    Metadata    map[string]interface{} `json:"metadata"`
    CreatedAt   int64                  `json:"createdAt"`
    YieldRate   float64                `json:"yieldRate,omitempty"` // Only for yield generators
    LastYield   int64                  `json:"lastYield,omitempty"` // Only for yield generators
    IsListed    bool                   `json:"isListed"`
    ListPrice   float64                `json:"listPrice,omitempty"`
    ListedAt    int64                  `json:"listedAt,omitempty"`
    TransferLog []TransferRecord       `json:"transferLog"`
}

// TransferRecord represents a record of an NFT transfer
type TransferRecord struct {
    FromAddress string  `json:"fromAddress"`
    ToAddress   string  `json:"toAddress"`
    Price       float64 `json:"price,omitempty"`
    Timestamp   int64   `json:"timestamp"`
}

// NewNFTSystem creates a new NFT system
func NewNFTSystem(masterWalletAddress string) *NFTSystem {
    return &NFTSystem{
        NFTs:                make(map[string]*NFT),
        NextID:              1,
        mutex:               sync.Mutex{},
        MasterWalletAddress: masterWalletAddress,
        TransactionFeeRate:  0.005, // 0.5%
    }
}

// CreateNFT creates a new NFT
func (ns *NFTSystem) CreateNFT(
    nftType string,
    owner string,
    creator string,
    metadata map[string]interface{},
    yieldRate float64,
) (*NFT, error) {
    ns.mutex.Lock()
    defer ns.mutex.Unlock()
    
    // Generate NFT ID
    id := generateNFTID(ns.NextID)
    ns.NextID++
    
    // Create NFT
    nft := &NFT{
        ID:          id,
        Type:        nftType,
        Owner:       owner,
        Creator:     creator,
        Metadata:    metadata,
        CreatedAt:   time.Now().Unix(),
        YieldRate:   yieldRate,
        LastYield:   time.Now().Unix(),
        IsListed:    false,
        TransferLog: []TransferRecord{},
    }
    
    // Add initial transfer record (minting)
    nft.TransferLog = append(nft.TransferLog, TransferRecord{
        FromAddress: "0x0", // Minting address
        ToAddress:   owner,
        Timestamp:   time.Now().Unix(),
    })
    
    // Store NFT
    ns.NFTs[id] = nft
    
    return nft, nil
}

// GetNFT gets an NFT by ID
func (ns *NFTSystem) GetNFT(id string) (*NFT, error) {
    ns.mutex.Lock()
    defer ns.mutex.Unlock()
    
    nft, exists := ns.NFTs[id]
    if !exists {
        return nil, errors.New("NFT not found")
    }
    
    return nft, nil
}

// TransferNFT transfers an NFT to a new owner
func (ns *NFTSystem) TransferNFT(id string, fromAddress string, toAddress string, price float64) error {
    ns.mutex.Lock()
    defer ns.mutex.Unlock()
    
    nft, exists := ns.NFTs[id]
    if !exists {
        return errors.New("NFT not found")
    }
    
    // Check ownership
    if nft.Owner != fromAddress {
        return errors.New("sender is not the owner of this NFT")
    }
    
    // Update owner
    nft.Owner = toAddress
    
    // Add transfer record
    nft.TransferLog = append(nft.TransferLog, TransferRecord{
        FromAddress: fromAddress,
        ToAddress:   toAddress,
        Price:       price,
        Timestamp:   time.Now().Unix(),
    })
    
    // If NFT was listed, unlist it
    if nft.IsListed {
        nft.IsListed = false
        nft.ListPrice = 0
        nft.ListedAt = 0
    }
    
    return nil
}

// ListNFT lists an NFT for sale
func (ns *NFTSystem) ListNFT(id string, owner string, price float64) error {
    ns.mutex.Lock()
    defer ns.mutex.Unlock()
    
    nft, exists := ns.NFTs[id]
    if !exists {
        return errors.New("NFT not found")
    }
    
    // Check ownership
    if nft.Owner != owner {
        return errors.New("sender is not the owner of this NFT")
    }
    
    // Update listing status
    nft.IsListed = true
    nft.ListPrice = price
    nft.ListedAt = time.Now().Unix()
    
    return nil
}

// UnlistNFT removes an NFT from sale
func (ns *NFTSystem) UnlistNFT(id string, owner string) error {
    ns.mutex.Lock()
    defer ns.mutex.Unlock()
    
    nft, exists := ns.NFTs[id]
    if !exists {
        return errors.New("NFT not found")
    }
    
    // Check ownership
    if nft.Owner != owner {
        return errors.New("sender is not the owner of this NFT")
    }
    
    // Update listing status
    nft.IsListed = false
    nft.ListPrice = 0
    nft.ListedAt = 0
    
    return nil
}

// BuyNFT buys a listed NFT
func (ns *NFTSystem) BuyNFT(id string, buyer string) (float64, error) {
    ns.mutex.Lock()
    defer ns.mutex.Unlock()
    
    nft, exists := ns.NFTs[id]
    if !exists {
        return 0, errors.New("NFT not found")
    }
    
    // Check if NFT is listed
    if !nft.IsListed {
        return 0, errors.New("NFT is not listed for sale")
    }
    
    // Check if buyer is not already the owner
    if nft.Owner == buyer {
        return 0, errors.New("buyer is already the owner")
    }
    
    // Calculate fee
    fee := nft.ListPrice * ns.TransactionFeeRate
    sellerAmount := nft.ListPrice - fee
    
    // Store current owner for transfer record
    currentOwner := nft.Owner
    
    // Update owner
    nft.Owner = buyer
    
    // Add transfer record
    nft.TransferLog = append(nft.TransferLog, TransferRecord{
        FromAddress: currentOwner,
        ToAddress:   buyer,
        Price:       nft.ListPrice,
        Timestamp:   time.Now().Unix(),
    })
    
    // Unlist NFT
    nft.IsListed = false
    nft.ListPrice = 0
    nft.ListedAt = 0
    
    return sellerAmount, nil
}

// CalculateYield calculates the yield for a yield-generating NFT
func (ns *NFTSystem) CalculateYield(id string, stakedAmount float64) (float64, error) {
    ns.mutex.Lock()
    defer ns.mutex.Unlock()
    
    nft, exists := ns.NFTs[id]
    if !exists {
        return 0, errors.New("NFT not found")
    }
    
    // Check if NFT is a yield generator
    if nft.Type != "yield_generator" || nft.YieldRate <= 0 {
        return 0, errors.New("NFT is not a yield generator")
    }
    
    // Calculate time since last yield in seconds
    currentTime := time.Now().Unix()
    timeSinceLastYield := currentTime - nft.LastYield
    
    // Convert to days (86400 seconds in a day)
    daysSinceLastYield := float64(timeSinceLastYield) / 86400.0
    
    // Calculate yield based on rate (e.g., 7% APY = 0.07 / 365 per day)
    dailyRate := nft.YieldRate / 365.0
    yield := stakedAmount * dailyRate * daysSinceLastYield
    
    // Update last yield time
    nft.LastYield = currentTime
    
    return yield, nil
}

// GetListedNFTs returns all NFTs that are listed for sale
func (ns *NFTSystem) GetListedNFTs() []*NFT {
    ns.mutex.Lock()
    defer ns.mutex.Unlock()
    
    listedNFTs := []*NFT{}
    
    for _, nft := range ns.NFTs {
        if nft.IsListed {
            listedNFTs = append(listedNFTs, nft)
        }
    }
    
    return listedNFTs
}

// GetNFTsByOwner returns all NFTs owned by a specific address
func (ns *NFTSystem) GetNFTsByOwner(owner string) []*NFT {
    ns.mutex.Lock()
    defer ns.mutex.Unlock()
    
    ownedNFTs := []*NFT{}
    
    for _, nft := range ns.NFTs {
        if nft.Owner == owner {
            ownedNFTs = append(ownedNFTs, nft)
        }
    }
    
    return ownedNFTs
}

// GetNFTsByType returns all NFTs of a specific type
func (ns *NFTSystem) GetNFTsByType(nftType string) []*NFT {
    ns.mutex.Lock()
    defer ns.mutex.Unlock()
    
    typedNFTs := []*NFT{}
    
    for _, nft := range ns.NFTs {
        if nft.Type == nftType {
            typedNFTs = append(typedNFTs, nft)
        }
    }
    
    return typedNFTs
}

// SerializeNFT converts an NFT to JSON
func SerializeNFT(nft *NFT) (string, error) {
    data, err := json.MarshalIndent(nft, "", "  ")
    if err != nil {
        return "", err
    }
    
    return string(data), nil
}

// DeserializeNFT converts JSON to an NFT
func DeserializeNFT(jsonData string) (*NFT, error) {
    var nft NFT
    err := json.Unmarshal([]byte(jsonData), &nft)
    if err != nil {
        return nil, err
    }
    
    return &nft, nil
}

// Helper function to generate NFT ID
func generateNFTID(id int) string {
    return "nft_" + time.Now().Format("20060102") + "_" + string(rune(id))
}
