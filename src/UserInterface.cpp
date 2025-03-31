/**
 * @file UserInterface.cpp
 * @author <YourName>
 * @brief Implementation of command-line interface
 */

#include "../include/UserInterface.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <algorithm>
#include <filesystem>

UserInterface::UserInterface(const std::string& stopwordsFile)
    : documentParser(indexHandler, stopwordsFile),
      queryProcessor(indexHandler) {
}

int UserInterface::run(int argc, char* argv[]) {
    if (argc < 2) {
        displayHelp();
        return 1;
    }
    
    std::string command = argv[1];
    
    try {
        if (command == "index") {
            std::vector<std::string> args(argv + 2, argv + argc);
            handleIndexCommand(args);
        }
        else if (command == "query") {
            std::vector<std::string> args(argv + 2, argv + argc);
            handleQueryCommand(args);
        }
        else if (command == "ui") {
            handleUICommand();
        }
        else {
            std::cerr << "Unknown command: " << command << std::endl;
            displayHelp();
            return 1;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

void UserInterface::displayHelp() const {
    std::cout << "Financial News Search Engine" << std::endl;
    std::cout << "Usage: supersearch [command] [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  index <path> [output]  - Index JSON documents in directory" << std::endl;
    std::cout << "  query <search terms>   - Search the index" << std::endl;
    std::cout << "  ui                     - Start interactive UI" << std::endl;
    std::cout << std::endl;
    std::cout << "Query syntax:" << std::endl;
    std::cout << "  word1 word2            - Search for documents containing all terms" << std::endl;
    std::cout << "  ORG:Google            - Search for organization" << std::endl;
    std::cout << "  PERSON:Musk           - Search for person" << std::endl;
    std::cout << "  -excludeword          - Exclude documents with this term" << std::endl;
}

void UserInterface::handleIndexCommand(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Error: No path specified for indexing" << std::endl;
        return;
    }
    
    std::string path = args[0];
    std::string outputBase = args.size() > 1 ? args[1] : "financial_index";
    
    std::cout << "Indexing documents in " << path << "..." << std::endl;
    
    if (std::filesystem::is_directory(path)) {
        documentParser.parseDirectory(path);
    }
    else {
        documentParser.parseJSON(path);
    }
    
    std::cout << "Saving index to " << outputBase << "..." << std::endl;
    indexHandler.saveIndices(outputBase);
    
    std::cout << "Indexed " << indexHandler.getTotalDocuments() << " documents." << std::endl;
}

void UserInterface::handleQueryCommand(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Error: No search terms specified" << std::endl;
        return;
    }
    
    // Check if index is loaded
    if (indexHandler.getTotalDocuments() == 0) {
        // Try to load default index
        try {
            indexHandler.loadIndices("financial_index");
        }
        catch (const std::exception& e) {
            std::cerr << "Error loading index: " << e.what() << std::endl;
            std::cerr << "Please create an index first using the 'index' command." << std::endl;
            return;
        }
    }
    
    // Combine all arguments into a query string
    std::ostringstream query;
    std::copy(args.begin(), args.end(), std::ostream_iterator<std::string>(query, " "));
    
    std::cout << "Searching for: " << query.str() << std::endl;
    
    auto results = queryProcessor.processQuery(query.str());
    displayResults(results);
}

void UserInterface::displayResults(const std::vector<QueryResult>& results) const {
    if (results.empty()) {
        std::cout << "No results found." << std::endl;
        return;
    }
    
    std::cout << "Found " << results.size() << " results:" << std::endl;
    std::cout << std::string(80, '-') << std::endl;
    
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& result = results[i];
        
        std::cout << i + 1 << ". " << result.title << std::endl;
        std::cout << "   Source: " << result.source << " | Date: " << result.date << std::endl;
        std::cout << "   Score: " << std::fixed << std::setprecision(4) << result.score << std::endl;
        std::cout << std::string(80, '-') << std::endl;
    }
}

void UserInterface::displayArticle(const std::string& docID) const {
    std::string content = queryProcessor.getFullArticle(docID);
    
    std::cout << std::string(80, '=') << std::endl;
    std::cout << content << std::endl;
    std::cout << std::string(80, '=') << std::endl;
}

void UserInterface::handleUICommand() {
    std::cout << "Financial News Search Engine - Interactive Mode" << std::endl;
    std::cout << "Type 'help' for commands, 'exit' to quit" << std::endl;
    
    std::string command;
    std::vector<QueryResult> lastResults;
    
    while (true) {
        std::cout << "\n> ";
        std::getline(std::cin, command);
        
        if (command == "exit" || command == "quit") {
            break;
        }
        else if (command == "help") {
            std::cout << "Commands:" << std::endl;
            std::cout << "  load <path>     - Load index from path" << std::endl;
            std::cout << "  index <path>    - Index documents in directory" << std::endl;
            std::cout << "  save <path>     - Save index to path" << std::endl;
            std::cout << "  view <number>   - View full article from last search" << std::endl;
            std::cout << "  exit/quit       - Exit program" << std::endl;
            std::cout << "  Any other input will be treated as a search query" << std::endl;
        }
        else if (command.substr(0, 5) == "load ") {
            std::string path = command.substr(5);
            std::cout << "Loading index from " << path << "..." << std::endl;
            try {
                indexHandler.loadIndices(path);
                std::cout << "Loaded " << indexHandler.getTotalDocuments() << " documents." << std::endl;
            }
            catch (const std::exception& e) {
                std::cerr << "Error loading index: " << e.what() << std::endl;
            }
        }
        else if (command.substr(0, 6) == "index ") {
            std::string path = command.substr(6);
            std::cout << "Indexing documents in " << path << "..." << std::endl;
            try {
                if (std::filesystem::is_directory(path)) {
                    documentParser.parseDirectory(path);
                }
                else {
                    documentParser.parseJSON(path);
                }
                std::cout << "Indexed " << indexHandler.getTotalDocuments() << " documents." << std::endl;
            }
            catch (const std::exception& e) {
                std::cerr << "Error indexing documents: " << e.what() << std::endl;
            }
        }
        else if (command.substr(0, 5) == "save ") {
            std::string path = command.substr(5);
            std::cout << "Saving index to " << path << "..." << std::endl;
            try {
                indexHandler.saveIndices(path);
                std::cout << "Index saved successfully." << std::endl;
            }
            catch (const std::exception& e) {
                std::cerr << "Error saving index: " << e.what() << std::endl;
            }
        }
        else if (command.substr(0, 5) == "view ") {
            try {
                int resultNum = std::stoi(command.substr(5));
                if (resultNum <= 0 || resultNum > static_cast<int>(lastResults.size())) {
                    std::cerr << "Invalid result number. Please specify a number between 1 and " 
                              << lastResults.size() << std::endl;
                }
                else {
                    displayArticle(lastResults[resultNum - 1].docID);
                }
            }
            catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }
        else if (!command.empty()) {
            // Process as search query
            if (indexHandler.getTotalDocuments() == 0) {
                std::cerr << "No index loaded. Use 'load <path>' to load an index." << std::endl;
                continue;
            }
            
            auto results = queryProcessor.processQuery(command);
            lastResults = results;
            displayResults(results);
        }
    }
}