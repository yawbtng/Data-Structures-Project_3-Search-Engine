/**
 * @file test_avltree.cpp
 * @author <YourName>
 * @brief Simple tests for AVL tree implementation
 * @version 1.0
 * @date 2024-03-15
 */

#include <iostream>
#include <string>
#include <cassert>
#include <vector>
#include <algorithm>
#include "../include/AVLTree.h"

// A simple test function to avoid Boost dependency
void test_avl_tree() {
    AVLTree<std::string, int> tree;
    
    // Test insertion
    tree.insert("apple", "doc1", 1.0);
    tree.insert("banana", "doc1", 2.0);
    tree.insert("orange", "doc2", 3.0);
    
    // Test search
    auto results = tree.search("apple");
    assert(results.size() == 1);
    assert(results[0].first == "doc1");
    assert(results[0].second == 1.0);
    
    // Test nonexistent key
    results = tree.search("nonexistent");
    assert(results.size() == 0);
    
    // Test multiple documents for same key
    tree.insert("common", "doc1", 1.0);
    tree.insert("common", "doc2", 2.0);
    tree.insert("common", "doc3", 3.0);
    
    results = tree.search("common");
    assert(results.size() == 3);
    
    // Check for all three documents
    std::vector<std::string> docIDs;
    for (const auto& [docID, _] : results) {
        docIDs.push_back(docID);
    }
    
    assert(std::find(docIDs.begin(), docIDs.end(), "doc1") != docIDs.end());
    assert(std::find(docIDs.begin(), docIDs.end(), "doc2") != docIDs.end());
    assert(std::find(docIDs.begin(), docIDs.end(), "doc3") != docIDs.end());
    
    std::cout << "All AVL tree tests passed!" << std::endl;
}

int main() {
    std::cout << "Running AVL tree tests..." << std::endl;
    test_avl_tree();
    return 0;
}