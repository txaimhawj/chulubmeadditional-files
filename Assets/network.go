package network

import (
    "encoding/json"
    "errors"
    "fmt"
    "net"
    "sync"
    "time"
)

// Node represents a node in the blockchain network
type Node struct {
    ID            string
    Address       string
    Type          string // "full", "game", "light", "master"
    IsValidator   bool
    Peers         map[string]*Peer
    MessageQueue  chan Message
    BlockQueue    chan []byte
    TxQueue       chan []byte
    IsRunning     bool
    mutex         sync.Mutex
    listener      net.Listener
    peerDiscovery *PeerDiscovery
}

// Peer represents a connection to another node
type Peer struct {
    ID        string
    Address   string
    Type      string
    Conn      net.Conn
    LastSeen  int64
    IsActive  bool
}

// Message represents a network message
type Message struct {
    Type    string      `json:"type"`
    Sender  string      `json:"sender"`
    Content interface{} `json:"content"`
    Time    int64       `json:"time"`
}

// PeerDiscovery handles finding and connecting to peers
type PeerDiscovery struct {
    BootstrapNodes []string
    DiscoveryPort  int
    HeartbeatInterval int
    node          *Node
}

// NewNode creates a new network node
func NewNode(id string, address string, nodeType string, isValidator bool) *Node {
    return &Node{
        ID:           id,
        Address:      address,
        Type:         nodeType,
        IsValidator:  isValidator,
        Peers:        make(map[string]*Peer),
        MessageQueue: make(chan Message, 100),
        BlockQueue:   make(chan []byte, 10),
        TxQueue:      make(chan []byte, 100),
        IsRunning:    false,
    }
}

// Start starts the node's network services
func (n *Node) Start(port int) error {
    n.mutex.Lock()
    defer n.mutex.Unlock()
    
    if n.IsRunning {
        return errors.New("node is already running")
    }
    
    // Start listening for connections
    listener, err := net.Listen("tcp", fmt.Sprintf(":%d", port))
    if err != nil {
        return err
    }
    
    n.listener = listener
    n.IsRunning = true
    
    // Start accepting connections
    go n.acceptConnections()
    
    // Start processing messages
    go n.processMessages()
    
    // Initialize peer discovery
    n.peerDiscovery = &PeerDiscovery{
        BootstrapNodes:    []string{},
        DiscoveryPort:     port + 1,
        HeartbeatInterval: 30, // seconds
        node:              n,
    }
    
    // Start peer discovery
    go n.peerDiscovery.Start()
    
    return nil
}

// Stop stops the node's network services
func (n *Node) Stop() error {
    n.mutex.Lock()
    defer n.mutex.Unlock()
    
    if !n.IsRunning {
        return errors.New("node is not running")
    }
    
    // Close listener
    if n.listener != nil {
        n.listener.Close()
    }
    
    // Close all peer connections
    for _, peer := range n.Peers {
        if peer.Conn != nil {
            peer.Conn.Close()
        }
    }
    
    n.IsRunning = false
    
    return nil
}

// Connect connects to a peer
func (n *Node) Connect(address string) error {
    // Check if already connected
    for _, peer := range n.Peers {
        if peer.Address == address && peer.IsActive {
            return errors.New("already connected to this peer")
        }
    }
    
    // Connect to peer
    conn, err := net.Dial("tcp", address)
    if err != nil {
        return err
    }
    
    // Send handshake
    handshake := Message{
        Type:    "handshake",
        Sender:  n.ID,
        Content: map[string]string{
            "address": n.Address,
            "type":    n.Type,
        },
        Time:    time.Now().Unix(),
    }
    
    handshakeData, err := json.Marshal(handshake)
    if err != nil {
        conn.Close()
        return err
    }
    
    _, err = conn.Write(handshakeData)
    if err != nil {
        conn.Close()
        return err
    }
    
    // Wait for handshake response
    buffer := make([]byte, 1024)
    n, err := conn.Read(buffer)
    if err != nil {
        conn.Close()
        return err
    }
    
    var response Message
    err = json.Unmarshal(buffer[:n], &response)
    if err != nil {
        conn.Close()
        return err
    }
    
    if response.Type != "handshake_ack" {
        conn.Close()
        return errors.New("invalid handshake response")
    }
    
    // Extract peer info from response
    content, ok := response.Content.(map[string]interface{})
    if !ok {
        conn.Close()
        return errors.New("invalid handshake content")
    }
    
    peerID, ok := content["id"].(string)
    if !ok {
        conn.Close()
        return errors.New("invalid peer ID")
    }
    
    peerType, ok := content["type"].(string)
    if !ok {
        conn.Close()
        return errors.New("invalid peer type")
    }
    
    // Add peer
    peer := &Peer{
        ID:       peerID,
        Address:  address,
        Type:     peerType,
        Conn:     conn,
        LastSeen: time.Now().Unix(),
        IsActive: true,
    }
    
    n.Peers[peerID] = peer
    
    // Start handling messages from this peer
    go n.handlePeerMessages(peer)
    
    return nil
}

// Broadcast sends a message to all connected peers
func (n *Node) Broadcast(messageType string, content interface{}) error {
    message := Message{
        Type:    messageType,
        Sender:  n.ID,
        Content: content,
        Time:    time.Now().Unix(),
    }
    
    messageData, err := json.Marshal(message)
    if err != nil {
        return err
    }
    
    for _, peer := range n.Peers {
        if peer.IsActive && peer.Conn != nil {
            _, err := peer.Conn.Write(messageData)
            if err != nil {
                // Mark peer as inactive
                peer.IsActive = false
                continue
            }
        }
    }
    
    return nil
}

// SendToPeer sends a message to a specific peer
func (n *Node) SendToPeer(peerID string, messageType string, content interface{}) error {
    peer, exists := n.Peers[peerID]
    if !exists || !peer.IsActive {
        return errors.New("peer not found or inactive")
    }
    
    message := Message{
        Type:    messageType,
        Sender:  n.ID,
        Content: content,
        Time:    time.Now().Unix(),
    }
    
    messageData, err := json.Marshal(message)
    if err != nil {
        return err
    }
    
    _, err = peer.Conn.Write(messageData)
    if err != nil {
        peer.IsActive = false
        return err
    }
    
    return nil
}

// acceptConnections accepts incoming connections
func (n *Node) acceptConnections() {
    for n.IsRunning {
        conn, err := n.listener.Accept()
        if err != nil {
            // Check if node is still running
            if !n.IsRunning {
                return
            }
            
            fmt.Printf("Error accepting connection: %v\n", err)
            continue
        }
        
        // Handle connection in a new goroutine
        go n.handleConnection(conn)
    }
}

// handleConnection handles a new connection
func (n *Node) handleConnection(conn net.Conn) {
    // Read handshake
    buffer := make([]byte, 1024)
    bytesRead, err := conn.Read(buffer)
    if err != nil {
        conn.Close()
        return
    }
    
    var message Message
    err = json.Unmarshal(buffer[:bytesRead], &message)
    if err != nil {
        conn.Close()
        return
    }
    
    if message.Type != "handshake" {
        conn.Close()
        return
    }
    
    // Extract peer info from handshake
    content, ok := message.Content.(map[string]interface{})
    if !ok {
        conn.Close()
        return
    }
    
    peerAddress, ok := content["address"].(string)
    if !ok {
        conn.Close()
        return
    }
    
    peerType, ok := content["type"].(string)
    if !ok {
        conn.Close()
        return
    }
    
    // Send handshake acknowledgement
    response := Message{
        Type:    "handshake_ack",
        Sender:  n.ID,
        Content: map[string]string{
            "id":   n.ID,
            "type": n.Type,
        },
        Time:    time.Now().Unix(),
    }
    
    responseData, err := json.Marshal(response)
    if err != nil {
        conn.Close()
        return
    }
    
    _, err = conn.Write(responseData)
    if err != nil {
        conn.Close()
        return
    }
    
    // Add peer
    peer := &Peer{
        ID:       message.Sender,
        Address:  peerAddress,
        Type:     peerType,
        Conn:     conn,
        LastSeen: time.Now().Unix(),
        IsActive: true,
    }
    
    n.Peers[peer.ID] = peer
    
    // Start handling messages from this peer
    go n.handlePeerMessages(peer)
}

// handlePeerMessages handles messages from a peer
func (n *Node) handlePeerMessages(peer *Peer) {
    buffer := make([]byte, 4096)
    
    for {
        bytesRead, err := peer.Conn.Read(buffer)
        if err != nil {
            // Mark peer as inactive
            peer.IsActive = false
            return
        }
        
        // Update last seen
        peer.LastSeen = time.Now().Unix()
        
        var message Message
        err = json.Unmarshal(buffer[:bytesRead], &message)
        if err != nil {
            continue
        }
        
        // Add message to queue
        n.MessageQueue <- message
    }
}

// processMessages processes messages from the message queue
func (n *Node) processMessages() {
    for n.IsRunning {
        select {
        case message := <-n.MessageQueue:
            // Process message based on type
            switch message.Type {
            case "block":
                // Convert content to bytes and add to block queue
                blockData, err := json.Marshal(message.Content)
                if err != nil {
                    continue
                }
                n.BlockQueue <- blockData
                
            case "transaction":
                // Convert content to bytes and add to transaction queue
                txData, err := json.Marshal(message.Content)
                if err != nil {
                    continue
                }
                n.TxQueue <- txData
                
            case "peer_discovery":
                // Handle peer discovery
                n.handlePeerDiscovery(message)
                
            case "heartbeat":
                // Update peer last seen
                if peer, exists := n.Peers[message.Sender]; exists {
                    peer.LastSeen = time.Now().Unix()
                }
            }
        }
    }
}

// handlePeerDiscovery handles peer discovery messages
func (n *Node) handlePeerDiscovery(message Message) {
    content, ok := message.Content.(map[string]interface{})
    if !ok {
        return
    }
    
    peersData, ok := content["peers"].([]interface{})
    if !ok {
        return
    }
    
    for _, peerData := range peersData {
        peerMap, ok := peerData.(map[string]interface{})
        if !ok {
            continue
        }
        
        peerAddress, ok := peerMap["address"].(string)
        if !ok {
            continue
        }
        
        // Check if we're already connected to this peer
        alreadyConnected := false
        for _, peer := range n.Peers {
            if peer.Address == peerAddress {
                alreadyConnected = true
                break
            }
        }
        
        // Connect to new peer
        if !alreadyConnected && peerAddress != n.Address {
            go n.Connect(peerAddress)
        }
    }
}

// Start starts the peer discovery service
func (p *PeerDiscovery) Start() {
    // Start heartbeat
    go p.sendHeartbeats()
    
    // Connect to bootstrap nodes
    for _, address := range p.BootstrapNodes {
        go p.node.Connect(address)
    }
    
    // Start discovery server
    listener, err := net.Listen("udp", fmt.Sprintf(":%d", p.DiscoveryPort))
    if err != nil {
        fmt.Printf("Error starting peer discovery: %v\n", err)
        return
    }
    
    defer listener.Close()
    
    // Handle discovery requests
    buffer := make([]byte, 1024)
    for {
        n, addr, err := listener.(*net.UDPConn).ReadFromUDP(buffer)
        if err != nil {
            continue
        }
        
        var message Message
        err = json.Unmarshal(buffer[:n], &message)
        if err != nil {
            continue
        }
        
        if message.Type == "discovery_request" {
            // Send peer list
            peerList := []map[string]string{}
            for _, peer := range p.node.Peers {
                if peer.IsActive {
                    peerList = append(peerList, map[string]string{
                        "id":      peer.ID,
                        "address": peer.Address,
                        "type":    peer.Type,
                    })
                }
            }
            
            // Add self
            peerList = append(peerList, map[string]string{
                "id":      p.node.ID,
                "address": p.node.Address,
                "type":    p.node.Type,
            })
            
            response := Message{
                Type:    "discovery_response",
                Sender:  p.node.ID,
                Content: map[string]interface{}{
                    "peers": peerList,
                },
                Time:    time.Now().Unix(),
            }
            
            responseData, err := json.Marshal(response)
            if err != nil {
                continue
            }
            
            listener.(*net.UDPConn).WriteToUDP(responseData, addr)
        }
    }
}

// sendHeartbeats sends periodic heartbeats to peers
func (p *PeerDiscovery) sendHeartbeats() {
    ticker := time.NewTicker(time.Duration(p.HeartbeatInterval) * time.Second)
    defer ticker.Stop()
    
    for range ticker.C {
        // Send heartbeat to all peers
        p.node.Broadcast("heartbeat", nil)
        
        // Check for inactive peers
        for id, peer := range p.node.Peers {
            if time.Now().Unix()-peer.LastSeen > int64(p.HeartbeatInterval*2) {
                peer.IsActive = false
                if peer.Conn != nil {
                    peer.Conn.Close()
                }
                delete(p.node.Peers, id)
            }
        }
        
        // If we have few peers, try to discover more
        if len(p.node.Peers) < 3 {
            p.discoverPeers()
        }
    }
}

// discoverPeers attempts to discover new peers
func (p *PeerDiscovery) discoverPeers() {
    // Send discovery request to known peers
    for _, peer := range p.node.Peers {
        if peer.IsActive {
            p.node.SendToPeer(peer.ID, "peer_discovery", map[string]interface{}{
                "request": true,
            })
        }
    }
}
