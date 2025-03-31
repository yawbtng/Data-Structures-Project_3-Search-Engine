/**
 * @file main.cpp
 * @author <YourName>
 * @brief Main entry point for financial news search engine
 * @version 1.0
 * @date 2024-03-15
 * 
 * History:
 * - 2024-03-15: Initial implementation
 * 
 * References:
 * - RapidJSON (https://rapidjson.org/)
 * - Porter2 Stemmer (https://github.com/smassung/porter2_stemmer)
 */

#include <iostream>
#include <string>
#include "../include/UserInterface.h"

int main(int argc, char* argv[]) {
    try {
        // Initialize with default stopwords file
        const std::string stopwordsFile = "stopwords.txt";
        UserInterface ui(stopwordsFile);
        
        return ui.run(argc, argv);
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}