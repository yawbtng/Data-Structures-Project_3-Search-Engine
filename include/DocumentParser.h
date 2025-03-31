/**
 * @file DocumentParser.h
 * @author <YourName>
 * @brief Parser for JSON news articles
 * @version 1.0
 * @date 2024-03-15
 * 
 * History:
 * - 2024-03-15: Initial implementation
 * 
 * References:
 * - RapidJSON documentation (https://rapidjson.org/)
 * - Porter2 Stemmer
 */

#pragma once
#include <string>
#include <vector>
#include <unordered_set>
#include <fstream>
#include "IndexHandler.h"
#include "../thirdparty/rapidjson/include/rapidjson/document.h"

class DocumentParser {
private:
    IndexHandler& indexHandler;
    std::unordered_set<std::string> stopwords;
    
    /**
     * @brief Process article content with stemming and stopword removal
     * @param content Article text
     * @param docID Article UUID
     */
    void processContent(const std::string& content, const std::string& docID);
    
    /**
     * @brief Extract entities from article metadata
     * @param metadata JSON metadata object
     * @param docID Article UUID
     */
    void processEntities(const rapidjson::Value& metadata, const std::string& docID);
    
    /**
     * @brief Load stopwords from file
     * @param filename Path to stopwords file
     */
    void loadStopwords(const std::string& filename);
    
public:
    /**
     * @brief Constructor
     * @param handler Reference to index handler
     * @param stopwordsFile Path to stopwords file
     */
    DocumentParser(IndexHandler& handler, const std::string& stopwordsFile);
    
    /**
     * @brief Parse a JSON news article
     * @param filename Path to JSON file
     */
    void parseJSON(const std::string& filename);
    
    /**
     * @brief Parse a directory of JSON files
     * @param directory Path to directory
     */
    void parseDirectory(const std::string& directory);
    
    /**
     * @brief Calculate TF-IDF scores for indexed documents
     */
    void calculateTFIDF();
};