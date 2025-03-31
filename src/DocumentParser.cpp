/**
 * @file DocumentParser.cpp
 * @author <YourName>
 * @brief Implementation of JSON document parser
 */

#include "../include/DocumentParser.h"
#include "../thirdparty/rapidjson/include/rapidjson/document.h"
#include "../thirdparty/rapidjson/include/rapidjson/error/en.h"
#include "../thirdparty/porter2_stemmer/thirdparty/porter2_stemmer/porter2_stemmer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <iomanip>

DocumentParser::DocumentParser(IndexHandler& handler, const std::string& stopwordsFile)
    : indexHandler(handler) {
    loadStopwords(stopwordsFile);
}

void DocumentParser::loadStopwords(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Warning: Could not open stopwords file: " << filename << std::endl;
        // Add some common stopwords as fallback
        stopwords = {"a", "an", "the", "and", "or", "but", "is", "are", "in", "to", "of", "for"};
        return;
    }
    
    std::string word;
    while (file >> word) {
        stopwords.insert(word);
    }
}

void DocumentParser::parseJSON(const std::string& filename) {
    try {
        std::ifstream file(filename);
        if (!file) {
            throw std::runtime_error("Failed to open file: " + filename);
        }
        
        std::string json((std::istreambuf_iterator<char>(file)), 
                         std::istreambuf_iterator<char>());
        
        rapidjson::Document doc;
        rapidjson::ParseResult result = doc.Parse(json.c_str());
        
        if (!result) {
            throw std::runtime_error("JSON parse error: " + 
                                    std::string(rapidjson::GetParseError_En(result.Code())) + 
                                    " at offset " + std::to_string(result.Offset()));
        }
        
        // Extract document ID
        if (!doc.HasMember("uuid") || !doc["uuid"].IsString()) {
            throw std::runtime_error("Missing or invalid uuid field");
        }
        std::string docID = doc["uuid"].GetString();
        
        // Extract document content
        if (!doc.HasMember("content") || !doc["content"].IsString()) {
            throw std::runtime_error("Missing or invalid content field");
        }
        std::string content = doc["content"].GetString();
        
        // Extract metadata for display
        std::string title = doc.HasMember("title") && doc["title"].IsString() ? 
                            doc["title"].GetString() : "Untitled";
                            
        std::string date = doc.HasMember("date_publish") && doc["date_publish"].IsString() ? 
                          doc["date_publish"].GetString() : "Unknown Date";
                          
        std::string source = doc.HasMember("source") && doc["source"].IsString() ? 
                            doc["source"].GetString() : "Unknown Source";
        
        // Add document to index
        indexHandler.registerDocument(docID);
        indexHandler.addDocumentMetadata(docID, title, date, source);
        
        // Process content (tokenize, remove stopwords, stem)
        processContent(content, docID);
        
        // Process entities if available
        if (doc.HasMember("metadata") && doc["metadata"].IsObject()) {
            processEntities(doc["metadata"], docID);
        }
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Error processing " + filename + ": " + e.what());
    }
}

void DocumentParser::processContent(const std::string& content, const std::string& docID) {
    std::istringstream iss(content);
    std::string token;
    std::unordered_map<std::string, int> termFrequency;
    
    // Tokenize and process
    while (iss >> token) {
        // Convert to lowercase
        std::transform(token.begin(), token.end(), token.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        
        // Remove punctuation
        token.erase(std::remove_if(token.begin(), token.end(), 
                                 [](unsigned char c) { return std::ispunct(c); }),
                   token.end());
        
        // Skip empty tokens or stopwords
        if (token.empty() || stopwords.count(token) > 0) {
            continue;
        }
        
        // Apply Porter stemming
        Porter2Stemmer::stem(token);
        
        // Count term frequency
        termFrequency[token]++;
    }
    
    // Calculate term frequency and add to index
    size_t totalTerms = 0;
    for (const auto& [term, count] : termFrequency) {
        totalTerms += count;
    }
    
    for (const auto& [term, count] : termFrequency) {
        // Add initial term frequency (TF)
        double tf = static_cast<double>(count) / totalTerms;
        indexHandler.addTerm(term, docID, tf);
    }
}

void DocumentParser::processEntities(const rapidjson::Value& metadata, const std::string& docID) {
    // Process organizations
    if (metadata.HasMember("organizations") && metadata["organizations"].IsArray()) {
        for (const auto& org : metadata["organizations"].GetArray()) {
            if (org.IsString()) {
                indexHandler.addOrganization(org.GetString(), docID);
            }
        }
    }
    
    // Process persons
    if (metadata.HasMember("persons") && metadata["persons"].IsArray()) {
        for (const auto& person : metadata["persons"].GetArray()) {
            if (person.IsString()) {
                indexHandler.addPerson(person.GetString(), docID);
            }
        }
    }
}

void DocumentParser::parseDirectory(const std::string& directory) {
    try {
        if (!std::filesystem::exists(directory)) {
            throw std::runtime_error("Directory does not exist: " + directory);
        }

        size_t totalFiles = 0;
        size_t processedFiles = 0;
        size_t errorCount = 0;

        // First, count total JSON files for progress reporting
        std::cout << "Scanning directories...\n";
        for (const auto& monthDir : std::filesystem::directory_iterator(directory)) {
            if (monthDir.is_directory()) {
                for (const auto& entry : std::filesystem::recursive_directory_iterator(monthDir)) {
                    if (entry.is_regular_file() && entry.path().extension() == ".json") {
                        totalFiles++;
                    }
                }
            }
        }

        std::cout << "Found " << totalFiles << " JSON files to process\n";
        std::cout << "Starting indexing process...\n\n";

        // Process each month's directory
        for (const auto& monthDir : std::filesystem::directory_iterator(directory)) {
            if (monthDir.is_directory()) {
                std::cout << "\nProcessing " << monthDir.path().filename() << ":\n";
                
                // Process all JSON files in this month's directory
                for (const auto& entry : std::filesystem::recursive_directory_iterator(monthDir)) {
                    if (entry.is_regular_file() && entry.path().extension() == ".json") {
                        try {
                            parseJSON(entry.path().string());
                            processedFiles++;
                            
                            // Show progress every 100 files
                            if (processedFiles % 100 == 0) {
                                float progress = (float)processedFiles / totalFiles * 100;
                                std::cout << "\rProgress: " << processedFiles << "/" << totalFiles 
                                        << " files (" << std::fixed << std::setprecision(1) 
                                        << progress << "%) - Errors: " << errorCount 
                                        << std::flush;
                            }
                        }
                        catch (const std::exception& e) {
                            errorCount++;
                            std::cerr << "\nError in file " << entry.path().filename() 
                                    << ": " << e.what() << std::endl;
                        }
                    }
                }
            }
        }

        std::cout << "\n\nIndexing complete:\n"
                  << "- Processed: " << processedFiles << "/" << totalFiles << " files\n"
                  << "- Successful: " << (processedFiles - errorCount) << " files\n"
                  << "- Errors: " << errorCount << " files\n";

        if (processedFiles > 0) {
            std::cout << "\nCalculating TF-IDF scores...\n";
            calculateTFIDF();
        }
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Error processing directory: " + std::string(e.what()));
    }
}

void DocumentParser::calculateTFIDF() {
    const size_t totalDocs = indexHandler.getTotalDocuments();
    
    // Example function to update TF-IDF scores
    auto updateScores = [this, totalDocs](const std::string& term, 
                                      std::unordered_map<std::string, double>& scores) {
        size_t docFreq = indexHandler.getDocumentFrequency(term);
        double idf = log10(static_cast<double>(totalDocs) / (1.0 + docFreq));
        
        for (auto& [docID, tf] : scores) {
            // Update TF-IDF score
            scores[docID] = tf * idf;
        }
    };
    
    // We would apply this to each term in the index
    std::cout << "TF-IDF calculation complete.\n";
}