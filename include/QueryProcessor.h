/**
 * @file QueryProcessor.h
 * @author <YourName>
 * @brief Processes user queries with boolean operations and ranking
 * @version 1.0
 * @date 2024-03-15
 * 
 * History:
 * - 2024-03-15: Initial implementation
 */

#pragma once
#include "IndexHandler.h"
#include <string>
#include <vector>
#include <unordered_map>

struct QueryResult {
    std::string docID;
    double score;
    std::string title;
    std::string date;
    std::string source;
    
    QueryResult() : score(0.0) {}
    QueryResult(const std::string& id, double s) : docID(id), score(s) {}
};

class QueryProcessor {
private:
    IndexHandler& indexHandler;
    
    /**
     * @brief Parse query string into components
     * @param query User query string
     * @return Tuple of terms, organizations, persons, exclusions
     */
    std::tuple<std::vector<std::string>, std::vector<std::string>, 
              std::vector<std::string>, std::vector<std::string>> 
    parseQuery(const std::string& query);
    
    /**
     * @brief Apply exclusion filters to results
     * @param results Current results
     * @param exclusions Terms to exclude
     */
    void applyExclusions(std::unordered_map<std::string, double>& results, 
                        const std::vector<std::string>& exclusions);
                        
public:
    /**
     * @brief Constructor
     * @param handler Reference to index handler
     */
    QueryProcessor(IndexHandler& handler) : indexHandler(handler) {}
    
    /**
     * @brief Process search query
     * @param query User query string
     * @return Vector of ranked query results
     */
    std::vector<QueryResult> processQuery(const std::string& query);
    
    /**
     * @brief Rank search results by relevance
     * @param rawScores Map of document IDs to scores
     * @param limit Maximum number of results to return
     * @return Sorted vector of query results
     */
    std::vector<QueryResult> rankResults(
        const std::unordered_map<std::string, double>& rawScores, size_t limit = 15);
        
    /**
     * @brief Get full article text
     * @param docID Document ID
     * @return Full article text
     */
    std::string getFullArticle(const std::string& docID) const;
};