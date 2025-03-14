package consensus

import (
    "crypto/sha256"
    "encoding/hex"
    "errors"
    "math/rand"
    "time"
)

// ProofOfPlay implements a custom consensus mechanism for the Nexus Legends blockchain
// It rewards active players and validates transactions based on game participation
type ProofOfPlay struct {
    // Minimum number of validators required for consensus
    MinValidators int
    
    // Percentage of validators required for finality (e.g., 67 for 2/3)
    FinalityThreshold int
    
    // List of active validators
    Validators []Validator
    
    // Map of validator votes for current block
    Votes map[string]bool
}

// Validator represents a node that can validate transactions and create blocks
type Validator struct {
    Address      string  // Wallet address of the validator
    Stake        float64 // Amount of ILYZ tokens staked
    PlayScore    float64 // Score based on game participation
    LastActivity int64   // Timestamp of last activity
    IsGameNode   bool    // Whether this is a game server node
}

// NewProofOfPlay creates a new Proof of Play consensus mechanism
func NewProofOfPlay() *ProofOfPlay {
    return &ProofOfPlay{
        MinValidators:     3,
        FinalityThreshold: 67,
        Validators:        []Validator{},
        Votes:             make(map[string]bool),
    }
}

// RegisterValidator adds a new validator to the consensus mechanism
func (pop *ProofOfPlay) RegisterValidator(address string, stake float64, isGameNode bool) {
    validator := Validator{
        Address:      address,
        Stake:        stake,
        PlayScore:    0,
        LastActivity: time.Now().Unix(),
        IsGameNode:   isGameNode,
    }
    
    pop.Validators = append(pop.Validators, validator)
}

// UpdatePlayScore updates a validator's play score based on game activity
func (pop *ProofOfPlay) UpdatePlayScore(address string, activityValue float64) error {
    for i, validator := range pop.Validators {
        if validator.Address == address {
            pop.Validators[i].PlayScore += activityValue
            pop.Validators[i].LastActivity = time.Now().Unix()
            return nil
        }
    }
    
    return errors.New("validator not found")
}

// SelectBlockProducer selects a validator to produce the next block
// Selection is weighted by stake and play score
func (pop *ProofOfPlay) SelectBlockProducer() (string, error) {
    if len(pop.Validators) < pop.MinValidators {
        return "", errors.New("not enough validators")
    }
    
    // Calculate total weight (stake * play score)
    totalWeight := 0.0
    weights := make([]float64, len(pop.Validators))
    
    for i, validator := range pop.Validators {
        // Inactive validators (no activity in last 24 hours) have zero weight
        if time.Now().Unix()-validator.LastActivity > 86400 {
            weights[i] = 0
            continue
        }
        
        // Game nodes have higher base weight
        baseWeight := 1.0
        if validator.IsGameNode {
            baseWeight = 2.0
        }
        
        // Weight = base * stake * (1 + play score)
        weight := baseWeight * validator.Stake * (1 + validator.PlayScore)
        weights[i] = weight
        totalWeight += weight
    }
    
    if totalWeight <= 0 {
        return "", errors.New("no active validators with positive weight")
    }
    
    // Select validator based on weight
    rand.Seed(time.Now().UnixNano())
    selection := rand.Float64() * totalWeight
    
    cumulativeWeight := 0.0
    for i, weight := range weights {
        cumulativeWeight += weight
        if cumulativeWeight >= selection {
            return pop.Validators[i].Address, nil
        }
    }
    
    // Fallback to first validator (should never happen)
    return pop.Validators[0].Address, nil
}

// ValidateBlock checks if a block is valid according to consensus rules
func (pop *ProofOfPlay) ValidateBlock(blockData []byte, producerAddress string, signature string) bool {
    // Verify the block producer is a registered validator
    isRegistered := false
    for _, validator := range pop.Validators {
        if validator.Address == producerAddress {
            isRegistered = true
            break
        }
    }
    
    if !isRegistered {
        return false
    }
    
    // TODO: Implement signature verification
    // For now, we'll use a simple hash check as placeholder
    expectedHash := sha256.Sum256(blockData)
    expectedHashStr := hex.EncodeToString(expectedHash[:])
    
    return signature == expectedHashStr
}

// VoteForBlock records a validator's vote for a block
func (pop *ProofOfPlay) VoteForBlock(blockHash string, validatorAddress string, approve bool) error {
    // Check if validator exists
    isRegistered := false
    for _, validator := range pop.Validators {
        if validator.Address == validatorAddress {
            isRegistered = true
            break
        }
    }
    
    if !isRegistered {
        return errors.New("validator not registered")
    }
    
    // Record vote
    voteKey := blockHash + ":" + validatorAddress
    pop.Votes[voteKey] = approve
    
    return nil
}

// HasConsensus checks if a block has reached consensus
func (pop *ProofOfPlay) HasConsensus(blockHash string) (bool, float64) {
    approvalCount := 0
    totalVotes := 0
    
    // Count votes for this block
    for key, approved := range pop.Votes {
        if key[:len(blockHash)] == blockHash {
            totalVotes++
            if approved {
                approvalCount++
            }
        }
    }
    
    // Calculate approval percentage
    if totalVotes == 0 {
        return false, 0
    }
    
    approvalPercentage := float64(approvalCount) * 100 / float64(totalVotes)
    
    // Check if approval percentage meets finality threshold
    return approvalPercentage >= float64(pop.FinalityThreshold), approvalPercentage
}

// ResetVotes clears all votes (used after block is finalized)
func (pop *ProofOfPlay) ResetVotes() {
    pop.Votes = make(map[string]bool)
}

// PruneInactiveValidators removes validators that have been inactive for too long
func (pop *ProofOfPlay) PruneInactiveValidators(maxInactivityPeriod int64) {
    currentTime := time.Now().Unix()
    activeValidators := []Validator{}
    
    for _, validator := range pop.Validators {
        if currentTime-validator.LastActivity <= maxInactivityPeriod {
            activeValidators = append(activeValidators, validator)
        }
    }
    
    pop.Validators = activeValidators
}
