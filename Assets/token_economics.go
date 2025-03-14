package token

import (
    "errors"
    "math"
    "sync"
    "time"
)

// TokenEconomics manages the ILYZ token economics
type TokenEconomics struct {
    // Total supply caps by year
    YearlySupplyCaps []float64
    
    // Current year (1-indexed)
    CurrentYear int
    
    // Current total supply
    CurrentSupply float64
    
    // Tokens minted this year
    YearlyMinted float64
    
    // Year start timestamp
    YearStartTime int64
    
    // Master wallet address
    MasterWalletAddress string
    
    // Transaction fee percentage (0.5% = 0.005)
    TransactionFeeRate float64
    
    // Yield rate for yield-generating NFTs (7% = 0.07)
    YieldRate float64
    
    // Mutex for thread safety
    mutex sync.Mutex
}

// NewTokenEconomics creates a new token economics manager
func NewTokenEconomics(masterWalletAddress string) *TokenEconomics {
    // Initialize with the 5B → 4B → 3B → 2B → 1B schedule
    yearlySupplyCaps := []float64{
        5_000_000_000, // Year 1: 5 billion
        4_000_000_000, // Year 2: 4 billion
        3_000_000_000, // Year 3: 3 billion
        2_000_000_000, // Year 4: 2 billion
        1_000_000_000, // Year 5+: 1 billion
    }
    
    return &TokenEconomics{
        YearlySupplyCaps:     yearlySupplyCaps,
        CurrentYear:          1,
        CurrentSupply:        0,
        YearlyMinted:         0,
        YearStartTime:        time.Now().Unix(),
        MasterWalletAddress:  masterWalletAddress,
        TransactionFeeRate:   0.005, // 0.5%
        YieldRate:            0.07,  // 7%
        mutex:                sync.Mutex{},
    }
}

// CalculateGameReward calculates the reward for winning a game
// based on match duration, player rank, and performance score
func (te *TokenEconomics) CalculateGameReward(
    matchDuration int64,
    playerRank int,
    performanceScore float64,
    activePlayerCount int,
) (float64, error) {
    te.mutex.Lock()
    defer te.mutex.Unlock()
    
    // Check if we've reached the yearly cap
    if te.YearlyMinted >= te.GetYearlySupplyCap() {
        return 0, errors.New("yearly token supply cap reached")
    }
    
    // Base reward calculation
    // Longer matches, higher ranks, and better performance = more rewards
    
    // Base reward (10 ILYZ)
    baseReward := 10.0
    
    // Adjust for match duration (longer matches = more rewards, up to a cap)
    // Convert seconds to minutes
    matchMinutes := float64(matchDuration) / 60.0
    
    // Duration factor (80% to 130% based on duration)
    durationFactor := 0.0
    if matchMinutes < 15 {
        durationFactor = 0.8
    } else if matchMinutes < 30 {
        durationFactor = 1.0
    } else if matchMinutes < 45 {
        durationFactor = 1.2
    } else {
        durationFactor = 1.3
    }
    
    // Adjust for player rank (higher rank = more rewards)
    // Rank factor (80% to 150% based on rank)
    rankFactor := 0.8 + (float64(playerRank) * 0.05)
    if rankFactor > 1.5 {
        rankFactor = 1.5
    }
    
    // Adjust for performance score (50% to 100% based on score)
    // Performance score should be between 0 and 100
    if performanceScore < 0 {
        performanceScore = 0
    } else if performanceScore > 100 {
        performanceScore = 100
    }
    
    performanceFactor := 0.5 + (performanceScore / 200.0)
    
    // Calculate reward
    reward := baseReward * durationFactor * rankFactor * performanceFactor
    
    // Adjust for active player count to ensure we don't exceed yearly cap
    // This helps distribute tokens more evenly throughout the year
    // Estimate total players per year (10 million monthly = 120 million yearly)
    estimatedYearlyPlayers := 120_000_000.0
    
    // Adjust reward based on active player count vs. estimated
    playerAdjustment := math.Sqrt(float64(activePlayerCount) / estimatedYearlyPlayers)
    reward *= playerAdjustment
    
    // Ensure we don't exceed yearly cap
    remainingYearlyCap := te.GetYearlySupplyCap() - te.YearlyMinted
    if reward > remainingYearlyCap {
        reward = remainingYearlyCap
    }
    
    // Update yearly minted amount
    te.YearlyMinted += reward
    
    return reward, nil
}

// CalculateTransactionFee calculates the fee for a transaction
func (te *TokenEconomics) CalculateTransactionFee(amount float64) float64 {
    return amount * te.TransactionFeeRate
}

// CalculateYield calculates the yield for a yield-generating NFT
func (te *TokenEconomics) CalculateYield(stakedAmount float64, daysSinceLastClaim float64) float64 {
    // Daily yield rate (7% APY / 365 days)
    dailyYieldRate := te.YieldRate / 365.0
    
    // Calculate yield
    yield := stakedAmount * dailyYieldRate * daysSinceLastClaim
    
    return yield
}

// GetYearlySupplyCap returns the supply cap for the current year
func (te *TokenEconomics) GetYearlySupplyCap() float64 {
    // If we're beyond the defined years, use the last year's cap
    if te.CurrentYear > len(te.YearlySupplyCaps) {
        return te.YearlySupplyCaps[len(te.YearlySupplyCaps)-1]
    }
    
    return te.YearlySupplyCaps[te.CurrentYear-1]
}

// CheckYearTransition checks if we've moved to a new year and updates state
func (te *TokenEconomics) CheckYearTransition() bool {
    te.mutex.Lock()
    defer te.mutex.Unlock()
    
    currentTime := time.Now().Unix()
    yearDuration := int64(365 * 24 * 60 * 60) // 365 days in seconds
    
    // Check if a year has passed since the start time
    if currentTime - te.YearStartTime >= yearDuration {
        // Increment year
        te.CurrentYear++
        
        // Reset yearly minted amount
        te.YearlyMinted = 0
        
        // Update year start time
        te.YearStartTime = currentTime
        
        return true
    }
    
    return false
}

// GetRemainingYearlySupply returns the remaining supply that can be minted this year
func (te *TokenEconomics) GetRemainingYearlySupply() float64 {
    te.mutex.Lock()
    defer te.mutex.Unlock()
    
    return te.GetYearlySupplyCap() - te.YearlyMinted
}

// GetTotalSupply returns the current total supply of tokens
func (te *TokenEconomics) GetTotalSupply() float64 {
    te.mutex.Lock()
    defer te.mutex.Unlock()
    
    return te.CurrentSupply
}

// UpdateTotalSupply updates the total supply (called when tokens are minted or burned)
func (te *TokenEconomics) UpdateTotalSupply(amount float64) {
    te.mutex.Lock()
    defer te.mutex.Unlock()
    
    te.CurrentSupply += amount
}
