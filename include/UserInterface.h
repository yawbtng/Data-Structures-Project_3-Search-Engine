/**
 * @file UserInterface.h
 * @author <YourName>
 * @brief Command-line interface for the search engine
 * @version 1.0
 * @date 2024-03-15
 * 
 * History:
 * - 2024-03-15: Initial implementation
 */

#pragma once
#include "IndexHandler.h"
#include "DocumentParser.h"
#include "QueryProcessor.h"
#include <string>

class UserInterface {
private:
    IndexHandler indexHandler;
    DocumentParser documentParser;
    QueryProcessor queryProcessor;
    
    /**
     * @brief Display search results
     * @param results Vector of query results
     */
    void displayResults(const std::vector<QueryResult>& results) const;
    
    /**
     * @brief Display full article
     * @param docID Document ID
     */
    void displayArticle(const std::string& docID) const;
    
    /**
     * @brief Run indexing command
     * @param args Command arguments
     */
    void handleIndexCommand(const std::vector<std::string>& args);
    
    /**
     * @brief Run query command
     * @param args Command arguments
     */
    void handleQueryCommand(const std::vector<std::string>& args);
    
    /**
     * @brief Run interactive UI
     */
    void handleUICommand();
    
public:
    /**
     * @brief Constructor
     * @param stopwordsFile Path to stopwords file
     */
    UserInterface(const std::string& stopwordsFile);
    
    /**
     * @brief Process command line arguments
     * @param argc Argument count
     * @param argv Argument values
     * @return Exit code
     */
    int run(int argc, char* argv[]);
    
    /**
     * @brief Display help message
     */
    void displayHelp() const;
};