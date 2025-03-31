/**
 * @file AVLTree.h
 * @author <YourName>
 * @brief AVL Tree implementation for financial news search engine
 * @version 1.0
 * @date 2024-03-15
 * 
 * History:
 * - 2024-03-15: Initial implementation
 * 
 * References:
 * - Introduction to Algorithms, 3rd Edition (Cormen et al.)
 */

#pragma once
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <queue>
#include <algorithm>
#include <stdexcept>
#include <iostream>

/**
 * @brief AVL Tree template class implementing a self-balancing binary search tree
 * @tparam KeyType Type of the key (usually std::string for word/entity)
 * @tparam ValueType Type of the value (not directly used, as document info is stored)
 */
template <typename KeyType, typename ValueType>
class AVLTree {
private:
    struct Node {
        KeyType key;
        ValueType value;
        std::unordered_map<std::string, double> docScores; // docID -> TF-IDF score
        std::shared_ptr<Node> left;
        std::shared_ptr<Node> right;
        int height;
        
        Node(const KeyType& k, const ValueType& v) 
            : key(k), value(v), left(nullptr), right(nullptr), height(1) {}
    };
    
    std::shared_ptr<Node> root;

    // Helper methods
    int height(std::shared_ptr<Node> node) const {
        return node ? node->height : 0;
    }
    
    int getBalanceFactor(std::shared_ptr<Node> node) const {
        return node ? height(node->left) - height(node->right) : 0;
    }
    
    std::shared_ptr<Node> rotateRight(std::shared_ptr<Node> y) {
        auto x = y->left;
        auto T2 = x->right;
        
        // Perform rotation
        x->right = y;
        y->left = T2;
        
        // Update heights
        y->height = std::max(height(y->left), height(y->right)) + 1;
        x->height = std::max(height(x->left), height(x->right)) + 1;
        
        return x;
    }
    
    std::shared_ptr<Node> rotateLeft(std::shared_ptr<Node> x) {
        auto y = x->right;
        auto T2 = y->left;
        
        // Perform rotation
        y->left = x;
        x->right = T2;
        
        // Update heights
        x->height = std::max(height(x->left), height(x->right)) + 1;
        y->height = std::max(height(y->left), height(y->right)) + 1;
        
        return y;
    }
    
    std::shared_ptr<Node> insert(std::shared_ptr<Node> node, const KeyType& key, 
                                const std::string& docID, double score) {
        // Normal BST insertion
        if (!node)
            return std::make_shared<Node>(key, ValueType());
            
        if (key < node->key)
            node->left = insert(node->left, key, docID, score);
        else if (key > node->key)
            node->right = insert(node->right, key, docID, score);
        else {
            // Key exists, update document scores
            node->docScores[docID] = score;
            return node; // No structural change
        }
        
        // Update height
        node->height = 1 + std::max(height(node->left), height(node->right));
        
        // Get balance factor
        int balance = getBalanceFactor(node);
        
        // Left Left Case
        if (balance > 1 && key < node->left->key)
            return rotateRight(node);
            
        // Right Right Case
        if (balance < -1 && key > node->right->key)
            return rotateLeft(node);
            
        // Left Right Case
        if (balance > 1 && key > node->left->key) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        
        // Right Left Case
        if (balance < -1 && key < node->right->key) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
        
        return node;
    }
    
    std::shared_ptr<Node> search(std::shared_ptr<Node> node, const KeyType& key) const {
        if (!node || node->key == key)
            return node;
            
        if (key < node->key)
            return search(node->left, key);
        else
            return search(node->right, key);
    }
    
    void serializeHelper(std::ofstream& out, std::shared_ptr<Node> node) const {
        if (!node) return;
        
        // Write node key
        size_t keySize = node->key.size();
        out.write(reinterpret_cast<const char*>(&keySize), sizeof(keySize));
        out.write(node->key.c_str(), keySize);
        
        // Write document scores
        size_t docCount = node->docScores.size();
        out.write(reinterpret_cast<const char*>(&docCount), sizeof(docCount));
        
        for (const auto& [docID, score] : node->docScores) {
            size_t idSize = docID.size();
            out.write(reinterpret_cast<const char*>(&idSize), sizeof(idSize));
            out.write(docID.c_str(), idSize);
            out.write(reinterpret_cast<const char*>(&score), sizeof(score));
        }
        
        // Recursively serialize left and right
        bool hasLeft = node->left != nullptr;
        bool hasRight = node->right != nullptr;
        
        out.write(reinterpret_cast<const char*>(&hasLeft), sizeof(hasLeft));
        if (hasLeft) serializeHelper(out, node->left);
        
        out.write(reinterpret_cast<const char*>(&hasRight), sizeof(hasRight));
        if (hasRight) serializeHelper(out, node->right);
    }
    
    std::shared_ptr<Node> deserializeHelper(std::ifstream& in) {
        // Read key
        size_t keySize;
        in.read(reinterpret_cast<char*>(&keySize), sizeof(keySize));
        
        std::string key(keySize, ' ');
        in.read(&key[0], keySize);
        
        auto node = std::make_shared<Node>(key, ValueType());
        
        // Read document scores
        size_t docCount;
        in.read(reinterpret_cast<char*>(&docCount), sizeof(docCount));
        
        for (size_t i = 0; i < docCount; ++i) {
            size_t idSize;
            in.read(reinterpret_cast<char*>(&idSize), sizeof(idSize));
            
            std::string docID(idSize, ' ');
            in.read(&docID[0], idSize);
            
            double score;
            in.read(reinterpret_cast<char*>(&score), sizeof(score));
            
            node->docScores[docID] = score;
        }
        
        // Deserialize left and right
        bool hasLeft, hasRight;
        
        in.read(reinterpret_cast<char*>(&hasLeft), sizeof(hasLeft));
        if (hasLeft) node->left = deserializeHelper(in);
        
        in.read(reinterpret_cast<char*>(&hasRight), sizeof(hasRight));
        if (hasRight) node->right = deserializeHelper(in);
        
        // Update height
        node->height = 1 + std::max(height(node->left), height(node->right));
        
        return node;
    }
    
    template <typename Func>
    void traverseInOrder(std::shared_ptr<Node> node, Func func) const {
        if (!node) return;
        
        traverseInOrder(node->left, func);
        func(node->key, node->docScores);
        traverseInOrder(node->right, func);
    }
    
public:
    AVLTree() : root(nullptr) {}
    
    /**
     * @brief Inserts a key with document ID and score
     * @param key Word or entity
     * @param docID Document ID where key appears
     * @param score TF-IDF score or initial term frequency
     */
    void insert(const KeyType& key, const std::string& docID, double score) {
        root = insert(root, key, docID, score);
    }
    
    /**
     * @brief Searches for documents containing key
     * @param key Word or entity to search for
     * @return Vector of pairs <docID, score>
     */
    std::vector<std::pair<std::string, double>> search(const KeyType& key) const {
        auto node = search(root, key);
        std::vector<std::pair<std::string, double>> results;
        
        if (node) {
            for (const auto& [docID, score] : node->docScores) {
                results.emplace_back(docID, score);
            }
        }
        
        return results;
    }
    
    /**
     * @brief Serializes the tree to a binary file
     * @param filename Path to output file
     */
    void serialize(const std::string& filename) const {
        std::ofstream out(filename, std::ios::binary);
        if (!out) {
            throw std::runtime_error("Failed to open file for writing: " + filename);
        }
        
        serializeHelper(out, root);
    }
    
    /**
     * @brief Deserializes the tree from a binary file
     * @param filename Path to input file
     */
    void deserialize(const std::string& filename) {
        std::ifstream in(filename, std::ios::binary);
        if (!in) {
            throw std::runtime_error("Failed to open file for reading: " + filename);
        }
        
        root = deserializeHelper(in);
    }
    
    /**
     * @brief Traverse the tree and apply function to each node
     * @param func Function to apply to each node
     */
    template <typename Func>
    void traverse(Func func) const {
        traverseInOrder(root, func);
    }
    
    /**
     * @brief Check if tree is empty
     * @return true if empty, false otherwise
     */
    bool isEmpty() const {
        return root == nullptr;
    }
};