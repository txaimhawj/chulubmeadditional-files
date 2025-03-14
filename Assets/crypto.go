package crypto

import (
    "crypto/ed25519"
    "crypto/rand"
    "crypto/sha256"
    "encoding/hex"
    "errors"
)

// KeyPair represents a public/private key pair for the blockchain
type KeyPair struct {
    PrivateKey ed25519.PrivateKey
    PublicKey  ed25519.PublicKey
}

// GenerateKeyPair creates a new Ed25519 key pair
func GenerateKeyPair() (*KeyPair, error) {
    publicKey, privateKey, err := ed25519.GenerateKey(rand.Reader)
    if err != nil {
        return nil, err
    }
    
    return &KeyPair{
        PrivateKey: privateKey,
        PublicKey:  publicKey,
    }, nil
}

// Sign creates a digital signature for the given data using the private key
func (kp *KeyPair) Sign(data []byte) (string, error) {
    if kp.PrivateKey == nil {
        return "", errors.New("private key is not available")
    }
    
    signature := ed25519.Sign(kp.PrivateKey, data)
    return hex.EncodeToString(signature), nil
}

// Verify checks if a signature is valid for the given data and public key
func Verify(data []byte, signature string, publicKey ed25519.PublicKey) (bool, error) {
    signatureBytes, err := hex.DecodeString(signature)
    if err != nil {
        return false, err
    }
    
    return ed25519.Verify(publicKey, data, signatureBytes), nil
}

// GetAddressFromPublicKey derives a blockchain address from a public key
func GetAddressFromPublicKey(publicKey ed25519.PublicKey) string {
    hash := sha256.Sum256(publicKey)
    return hex.EncodeToString(hash[:])
}

// PublicKeyToHex converts a public key to a hex string
func PublicKeyToHex(publicKey ed25519.PublicKey) string {
    return hex.EncodeToString(publicKey)
}

// HexToPublicKey converts a hex string to a public key
func HexToPublicKey(hexKey string) (ed25519.PublicKey, error) {
    bytes, err := hex.DecodeString(hexKey)
    if err != nil {
        return nil, err
    }
    
    if len(bytes) != ed25519.PublicKeySize {
        return nil, errors.New("invalid public key size")
    }
    
    return ed25519.PublicKey(bytes), nil
}

// PrivateKeyToHex converts a private key to a hex string
func PrivateKeyToHex(privateKey ed25519.PrivateKey) string {
    return hex.EncodeToString(privateKey)
}

// HexToPrivateKey converts a hex string to a private key
func HexToPrivateKey(hexKey string) (ed25519.PrivateKey, error) {
    bytes, err := hex.DecodeString(hexKey)
    if err != nil {
        return nil, err
    }
    
    if len(bytes) != ed25519.PrivateKeySize {
        return nil, errors.New("invalid private key size")
    }
    
    return ed25519.PrivateKey(bytes), nil
}

// HashData creates a SHA-256 hash of the given data
func HashData(data []byte) string {
    hash := sha256.Sum256(data)
    return hex.EncodeToString(hash[:])
}
