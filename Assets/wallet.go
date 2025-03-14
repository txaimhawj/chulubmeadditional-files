package wallet

import (
    "encoding/json"
    "errors"
    "time"

    "../crypto"
)

// Wallet represents a user's blockchain wallet
type Wallet struct {
    Address    string `json:"address"`
    PublicKey  string `json:"publicKey"`
    PrivateKey string `json:"privateKey,omitempty"` // Only stored locally, never transmitted
    Balance    struct {
        ILYZ float64 `json:"ilyz"`
    } `json:"balance"`
    NFTs        []NFT      `json:"nfts"`
    Transactions []string   `json:"transactions"` // Transaction IDs
    CreatedAt   int64      `json:"createdAt"`
    LastUpdated int64      `json:"lastUpdated"`
}

// NFT represents a non-fungible token in the wallet
type NFT struct {
    ID          string                 `json:"id"`
    Type        string                 `json:"type"` // "champion_skin", "yield_generator", etc.
    Metadata    map[string]interface{} `json:"metadata"`
    AcquiredAt  int64                  `json:"acquiredAt"`
    YieldRate   float64                `json:"yieldRate,omitempty"` // Only for yield generators
    LastYield   int64                  `json:"lastYield,omitempty"` // Only for yield generators
}

// CreateWallet generates a new wallet with a key pair
func CreateWallet() (*Wallet, error) {
    // Generate key pair
    keyPair, err := crypto.GenerateKeyPair()
    if err != nil {
        return nil, err
    }
    
    // Get address from public key
    address := crypto.GetAddressFromPublicKey(keyPair.PublicKey)
    
    // Create wallet
    wallet := &Wallet{
        Address:    address,
        PublicKey:  crypto.PublicKeyToHex(keyPair.PublicKey),
        PrivateKey: crypto.PrivateKeyToHex(keyPair.PrivateKey),
        NFTs:       []NFT{},
        Transactions: []string{},
        CreatedAt:  time.Now().Unix(),
        LastUpdated: time.Now().Unix(),
    }
    
    wallet.Balance.ILYZ = 0.0
    
    return wallet, nil
}

// LoadWallet loads a wallet from a JSON string
func LoadWallet(jsonData string) (*Wallet, error) {
    var wallet Wallet
    err := json.Unmarshal([]byte(jsonData), &wallet)
    if err != nil {
        return nil, err
    }
    
    return &wallet, nil
}

// SaveWallet saves a wallet to a JSON string
func SaveWallet(wallet *Wallet, includePrivateKey bool) (string, error) {
    // Create a copy of the wallet to avoid modifying the original
    walletCopy := *wallet
    
    // Remove private key if not including it
    if !includePrivateKey {
        walletCopy.PrivateKey = ""
    }
    
    // Update last updated timestamp
    walletCopy.LastUpdated = time.Now().Unix()
    
    // Convert to JSON
    jsonData, err := json.MarshalIndent(walletCopy, "", "  ")
    if err != nil {
        return "", err
    }
    
    return string(jsonData), nil
}

// SignTransaction signs a transaction with the wallet's private key
func (w *Wallet) SignTransaction(transactionData []byte) (string, error) {
    if w.PrivateKey == "" {
        return "", errors.New("private key not available")
    }
    
    // Convert hex private key to ed25519 private key
    privateKey, err := crypto.HexToPrivateKey(w.PrivateKey)
    if err != nil {
        return "", err
    }
    
    // Create key pair with private key
    keyPair := &crypto.KeyPair{
        PrivateKey: privateKey,
    }
    
    // Sign transaction
    signature, err := keyPair.Sign(transactionData)
    if err != nil {
        return "", err
    }
    
    return signature, nil
}

// AddNFT adds an NFT to the wallet
func (w *Wallet) AddNFT(nft NFT) {
    w.NFTs = append(w.NFTs, nft)
    w.LastUpdated = time.Now().Unix()
}

// RemoveNFT removes an NFT from the wallet
func (w *Wallet) RemoveNFT(nftID string) error {
    for i, nft := range w.NFTs {
        if nft.ID == nftID {
            // Remove NFT from slice
            w.NFTs = append(w.NFTs[:i], w.NFTs[i+1:]...)
            w.LastUpdated = time.Now().Unix()
            return nil
        }
    }
    
    return errors.New("NFT not found in wallet")
}

// AddTransaction adds a transaction ID to the wallet's transaction history
func (w *Wallet) AddTransaction(transactionID string) {
    w.Transactions = append(w.Transactions, transactionID)
    w.LastUpdated = time.Now().Unix()
}

// UpdateBalance updates the wallet's ILYZ balance
func (w *Wallet) UpdateBalance(amount float64) {
    w.Balance.ILYZ = amount
    w.LastUpdated = time.Now().Unix()
}

// CalculateYield calculates and updates yield for yield-generating NFTs
func (w *Wallet) CalculateYield() float64 {
    currentTime := time.Now().Unix()
    totalYield := 0.0
    
    for i, nft := range w.NFTs {
        if nft.Type == "yield_generator" && nft.YieldRate > 0 {
            // Calculate time since last yield in seconds
            timeSinceLastYield := currentTime - nft.LastYield
            
            // Convert to days (86400 seconds in a day)
            daysSinceLastYield := float64(timeSinceLastYield) / 86400.0
            
            // Calculate yield based on rate (e.g., 7% APY = 0.07 / 365 per day)
            dailyRate := nft.YieldRate / 365.0
            yield := w.Balance.ILYZ * dailyRate * daysSinceLastYield
            
            // Update last yield time
            w.NFTs[i].LastYield = currentTime
            
            totalYield += yield
        }
    }
    
    // Add yield to balance
    w.Balance.ILYZ += totalYield
    w.LastUpdated = currentTime
    
    return totalYield
}
