package main

import (
    "crypto/sha256"
    "encoding/hex"
    "encoding/json"
    "fmt"
    "time"
)

// Block represents a single block in the blockchain
type Block struct {
    Index        int64         `json:"index"`
    Timestamp    int64         `json:"timestamp"`
    Transactions []Transaction `json:"transactions"`
    Hash         string        `json:"hash"`
    PrevHash     string        `json:"prevHash"`
    Validator    string        `json:"validator"`
    Signature    string        `json:"signature"`
}

// Transaction represents a transaction in the blockchain
type Transaction struct {
    ID        string      `json:"id"`
    Type      string      `json:"type"`
    Sender    string      `json:"sender"`
    Recipient string      `json:"recipient"`
    Amount    float64     `json:"amount"`
    Data      interface{} `json:"data"`
    Timestamp int64       `json:"timestamp"`
    Signature string      `json:"signature"`
}

// Blockchain represents the entire blockchain
type Blockchain struct {
    Chain               []Block `json:"chain"`
    PendingTransactions []Transaction
    Difficulty          int
    MiningReward        float64
    Nodes               []string
}

// NewBlockchain creates a new blockchain with a genesis block
func NewBlockchain() *Blockchain {
    blockchain := &Blockchain{
        Chain:               []Block{},
        PendingTransactions: []Transaction{},
        Difficulty:          4,
        MiningReward:        5.0,
        Nodes:               []string{},
    }

    // Create genesis block
    genesisBlock := Block{
        Index:        0,
        Timestamp:    time.Now().Unix(),
        Transactions: []Transaction{},
        Hash:         "",
        PrevHash:     "0",
        Validator:    "genesis",
        Signature:    "",
    }
    genesisBlock.Hash = blockchain.CalculateHash(genesisBlock)

    blockchain.Chain = append(blockchain.Chain, genesisBlock)
    return blockchain
}

// CalculateHash calculates the hash of a block
func (bc *Blockchain) CalculateHash(block Block) string {
    blockData, _ := json.Marshal(struct {
        Index        int64         `json:"index"`
        Timestamp    int64         `json:"timestamp"`
        Transactions []Transaction `json:"transactions"`
        PrevHash     string        `json:"prevHash"`
        Validator    string        `json:"validator"`
    }{
        Index:        block.Index,
        Timestamp:    block.Timestamp,
        Transactions: block.Transactions,
        PrevHash:     block.PrevHash,
        Validator:    block.Validator,
    })

    hash := sha256.Sum256(blockData)
    return hex.EncodeToString(hash[:])
}

// GetLatestBlock returns the latest block in the blockchain
func (bc *Blockchain) GetLatestBlock() Block {
    return bc.Chain[len(bc.Chain)-1]
}

// CreateTransaction creates a new transaction
func (bc *Blockchain) CreateTransaction(transaction Transaction) {
    bc.PendingTransactions = append(bc.PendingTransactions, transaction)
}

// CreateBlock creates a new block with pending transactions
func (bc *Blockchain) CreateBlock(validator string, signature string) Block {
    latestBlock := bc.GetLatestBlock()
    newBlock := Block{
        Index:        latestBlock.Index + 1,
        Timestamp:    time.Now().Unix(),
        Transactions: bc.PendingTransactions,
        PrevHash:     latestBlock.Hash,
        Validator:    validator,
        Signature:    signature,
    }

    newBlock.Hash = bc.CalculateHash(newBlock)
    bc.Chain = append(bc.Chain, newBlock)
    bc.PendingTransactions = []Transaction{}

    return newBlock
}

// IsChainValid checks if the blockchain is valid
func (bc *Blockchain) IsChainValid() bool {
    for i := 1; i < len(bc.Chain); i++ {
        currentBlock := bc.Chain[i]
        prevBlock := bc.Chain[i-1]

        // Check if the hash is correct
        if currentBlock.Hash != bc.CalculateHash(currentBlock) {
            return false
        }

        // Check if the previous hash reference is correct
        if currentBlock.PrevHash != prevBlock.Hash {
            return false
        }
    }
    return true
}

// RegisterNode registers a new node in the network
func (bc *Blockchain) RegisterNode(address string) {
    bc.Nodes = append(bc.Nodes, address)
}

// Main function for testing
func main() {
    // Create a new blockchain
    nexusChain := NewBlockchain()

    // Create some test transactions
    transaction1 := Transaction{
        ID:        "tx1",
        Type:      "token_transfer",
        Sender:    "player1",
        Recipient: "player2",
        Amount:    10.0,
        Data:      nil,
        Timestamp: time.Now().Unix(),
        Signature: "sig1",
    }

    transaction2 := Transaction{
        ID:        "tx2",
        Type:      "nft_transfer",
        Sender:    "player2",
        Recipient: "player3",
        Amount:    0.0,
        Data: map[string]interface{}{
            "nftId":   "skin123",
            "nftType": "champion_skin",
        },
        Timestamp: time.Now().Unix(),
        Signature: "sig2",
    }

    // Add transactions to the blockchain
    nexusChain.CreateTransaction(transaction1)
    nexusChain.CreateTransaction(transaction2)

    // Create a new block
    nexusChain.CreateBlock("validator1", "block_signature")

    // Print the blockchain
    blockchainJSON, _ := json.MarshalIndent(nexusChain, "", "  ")
    fmt.Println(string(blockchainJSON))

    // Validate the blockchain
    fmt.Println("Is blockchain valid:", nexusChain.IsChainValid())
}
