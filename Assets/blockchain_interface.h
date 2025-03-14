#pragma once

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <unordered_map>

#include "../core/ecs.h"
#include "../../blockchain/custom/core/blockchain.go" // Include via wrapper

namespace CHULUBME {

// Forward declarations
class Wallet;
class NFTSystem;
class TokenEconomics;
class BlockchainInterface;

/**
 * @brief Interface to the blockchain system
 */
class BlockchainInterface {
public:
    // Singleton instance
    static BlockchainInterface& Instance();

    // Initialize the blockchain interface
    bool Initialize();

    // Shutdown the blockchain interface
    void Shutdown();

    // Update the blockchain interface
    void Update(float deltaTime);

    // Connect to blockchain network
    bool Connect(const std::string& nodeAddress);

    // Disconnect from blockchain network
    void Disconnect();

    // Check if connected to blockchain
    bool IsConnected() const;

    // Get the blockchain instance
    Blockchain* GetBlockchain() const;

    // Get the wallet instance
    Wallet* GetWallet() const;

    // Get the NFT system instance
    NFTSystem* GetNFTSystem() const;

    // Get the token economics instance
    TokenEconomics* GetTokenEconomics() const;

    // Create a new wallet
    bool CreateWallet(const std::string& password);

    // Load a wallet from file
    bool LoadWallet(const std::string& filename, const std::string& password);

    // Save wallet to file
    bool SaveWallet(const std::string& filename, const std::string& password);

    // Get wallet balance
    float GetBalance() const;

    // Send ILYZ tokens
    bool SendTokens(const std::string& recipient, float amount);

    // Get owned NFTs
    std::vector<NFT> GetOwnedNFTs() const;

    // Get NFT marketplace listings
    std::vector<NFT> GetMarketplaceListings() const;

    // Buy NFT from marketplace
    bool BuyNFT(const std::string& nftId);

    // Sell NFT on marketplace
    bool SellNFT(const std::string& nftId, float price);

    // Calculate game rewards
    float CalculateGameReward(int matchDuration, int playerRank, float performanceScore);

    // Register transaction callback
    void RegisterTransactionCallback(std::function<void(const Transaction&)> callback);

    // Register balance change callback
    void RegisterBalanceChangeCallback(std::function<void(float)> callback);

    // Register NFT acquisition callback
    void RegisterNFTAcquisitionCallback(std::function<void(const NFT&)> callback);

private:
    // Private constructor for singleton
    BlockchainInterface();
    ~BlockchainInterface();

    // Deleted copy constructor and assignment operator
    BlockchainInterface(const BlockchainInterface&) = delete;
    BlockchainInterface& operator=(const BlockchainInterface&) = delete;

    // Blockchain instance
    std::unique_ptr<Blockchain> m_blockchain;

    // Wallet instance
    std::unique_ptr<Wallet> m_wallet;

    // NFT system instance
    std::unique_ptr<NFTSystem> m_nftSystem;

    // Token economics instance
    std::unique_ptr<TokenEconomics> m_tokenEconomics;

    // Connection status
    bool m_connected;

    // Callbacks
    std::vector<std::function<void(const Transaction&)>> m_transactionCallbacks;
    std::vector<std::function<void(float)>> m_balanceChangeCallbacks;
    std::vector<std::function<void(const NFT&)>> m_nftAcquisitionCallbacks;
};

/**
 * @brief Wallet component for entity blockchain interaction
 */
class WalletComponent : public Component {
public:
    WalletComponent();
    ~WalletComponent() = default;

    // Initialize the wallet component
    void Initialize() override;

    // Finalize the wallet component
    void Finalize() override;

    // Get wallet address
    std::string GetAddress() const;

    // Get wallet balance
    float GetBalance() const;

    // Send tokens
    bool SendTokens(const std::string& recipient, float amount);

    // Get owned NFTs
    std::vector<NFT> GetOwnedNFTs() const;

    // Register balance change callback
    void RegisterBalanceChangeCallback(std::function<void(float)> callback);

    // Register NFT acquisition callback
    void RegisterNFTAcquisitionCallback(std::function<void(const NFT&)> callback);

private:
    // Blockchain interface reference
    BlockchainInterface& m_blockchainInterface;

    // Callback handles
    int m_balanceChangeCallbackHandle;
    int m_nftAcquisitionCallbackHandle;
};

/**
 * @brief NFT component for entity NFT representation
 */
class NFTComponent : public Component {
public:
    NFTComponent(const std::string& nftId = "");
    ~NFTComponent() = default;

    // Initialize the NFT component
    void Initialize() override;

    // Finalize the NFT component
    void Finalize() override;

    // Set NFT ID
    void SetNFTID(const std::string& nftId);

    // Get NFT ID
    std::string GetNFTID() const;

    // Get NFT data
    NFT GetNFTData() const;

    // Check if NFT is owned
    bool IsOwned() const;

    // Apply NFT skin to entity
    bool ApplySkin();

private:
    // NFT ID
    std::string m_nftId;

    // Blockchain interface reference
    BlockchainInterface& m_blockchainInterface;
};

/**
 * @brief Blockchain system for processing blockchain interactions
 */
class BlockchainSystem : public System {
public:
    BlockchainSystem(EntityManager* manager);
    ~BlockchainSystem() = default;

    // Initialize the blockchain system
    void Initialize() override;

    // Update the blockchain system
    void Update(float deltaTime) override;

    // Called when an entity is added to this system
    void OnEntityAdded(Entity entity) override;

    // Called when an entity is removed from this system
    void OnEntityRemoved(Entity entity) override;

private:
    // Blockchain interface reference
    BlockchainInterface& m_blockchainInterface;
};

/**
 * @brief Game reward system for calculating and distributing rewards
 */
class GameRewardSystem : public System {
public:
    GameRewardSystem(EntityManager* manager);
    ~GameRewardSystem() = default;

    // Initialize the game reward system
    void Initialize() override;

    // Update the game reward system
    void Update(float deltaTime) override;

    // Calculate and distribute rewards for a match
    void CalculateRewards(const std::vector<Entity>& players, int matchDuration);

private:
    // Blockchain interface reference
    BlockchainInterface& m_blockchainInterface;
};

} // namespace CHULUBME
