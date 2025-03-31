/**
 * @file IndexHandler.h
 * @author <YourName>
 * @brief Manages AVL tree indices for words and entities
 * @version 1.0
 * @date 2024-03-15
 * 
 * History:
 * - 2024-03-15: Initial implementation
 */

#pragma once
#include "AVLTree.h"
#include <unordered_set>
#include <string>
#include <vector>
#include <unordered_map>
#include "../thirdparty/rapidjson/include/rapidjson/document.h"

class IndexHandler {
private:
    AVLTree<std::string, std::string> wordIndex;
    AVLTree<std::string, std::string> organizationIndex;
    AVLTree<std::string, std::string> personIndex;
    std::unordered_set<std::string> documentIDs;
    std::unordered_map<std::string, std::string> documentMetadata; // uuid -> title, date, etc.
    
public:
    IndexHandler() = default;
    
    /**
     * @brief Get total number of indexed documents
     * @return Document count
     */
    size_t getTotalDocuments() const;
    
    /**
     * @brief Get number of documents containing term
     * @param term Search term
     * @return Document frequency
     */
    size_t getDocumentFrequency(const std::string& term) const;
    
    /**
     * @brief Add term to word index
     * @param term Stemmed word
     * @param docID Document ID
     * @param score Initial term frequency
     */
    void addTerm(const std::string& term, const std::string& docID, double score = 1.0);
    
    /**
     * @brief Add organization entity
     * @param org Organization name
     * @param docID Document ID
     */
    void addOrganization(const std::string& org, const std::string& docID);
    
    /**
     * @brief Add person entity
     * @param person Person name
     * @param docID Document ID
     */
    void addPerson(const std::string& person, const std::string& docID);
    
    /**
     * @brief Add document metadata
     * @param docID Document ID
     * @param title Article title
     * @param date Publication date
     * @param source Publication source
     */
    void addDocumentMetadata(const std::string& docID, const std::string& title, 
                            const std::string& date, const std::string& source);
    
    /**
     * @brief Get document metadata
     * @param docID Document ID
     * @return Metadata as JSON string
     */
    std::string getDocumentMetadata(const std::string& docID) const;
    
    /**
     * @brief Save all indices to files
     * @param basePath Base path for index files
     */
    void saveIndices(const std::string& basePath) const;
    
    /**
     * @brief Load all indices from files
     * @param basePath Base path for index files
     */
    void loadIndices(const std::string& basePath);
    
    /**
     * @brief Search for term in word index
     * @param term Search term
     * @return Vector of document IDs with scores
     */
    std::vector<std::pair<std::string, double>> searchWord(const std::string& term) const;
    
    /**
     * @brief Search for organization entity
     * @param org Organization name
     * @return Vector of document IDs with scores
     */
    std::vector<std::pair<std::string, double>> searchOrganization(const std::string& org) const;
    
    /**
     * @brief Search for person entity
     * @param person Person name
     * @return Vector of document IDs with scores
     */
    std::vector<std::pair<std::string, double>> searchPerson(const std::string& person) const;
    
    /**
     * @brief Register document in index
     * @param docID Document ID
     */
    void registerDocument(const std::string& docID);
};