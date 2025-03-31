/**
 * @file IndexHandler.cpp
 * @author <YourName>
 * @brief Implementation of index management
 */

#include "../include/IndexHandler.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include "../thirdparty/rapidjson/include/rapidjson/writer.h"
#include "../thirdparty/rapidjson/include/rapidjson/stringbuffer.h"

size_t IndexHandler::getTotalDocuments() const {
    return documentIDs.size();
}

size_t IndexHandler::getDocumentFrequency(const std::string& term) const {
    auto results = wordIndex.search(term);
    return results.size();
}

void IndexHandler::addTerm(const std::string& term, const std::string& docID, double score) {
    wordIndex.insert(term, docID, score);
}

void IndexHandler::addOrganization(const std::string& org, const std::string& docID) {
    organizationIndex.insert(org, docID, 1.0);
}

void IndexHandler::addPerson(const std::string& person, const std::string& docID) {
    personIndex.insert(person, docID, 1.0);
}

void IndexHandler::registerDocument(const std::string& docID) {
    documentIDs.insert(docID);
}

void IndexHandler::addDocumentMetadata(const std::string& docID, const std::string& title, 
                                     const std::string& date, const std::string& source) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    
    writer.StartObject();
    writer.Key("title");
    writer.String(title.c_str());
    writer.Key("date");
    writer.String(date.c_str());
    writer.Key("source");
    writer.String(source.c_str());
    writer.EndObject();
    
    documentMetadata[docID] = buffer.GetString();
}

std::string IndexHandler::getDocumentMetadata(const std::string& docID) const {
    auto it = documentMetadata.find(docID);
    if (it != documentMetadata.end()) {
        return it->second;
    }
    return "{}";
}

void IndexHandler::saveIndices(const std::string& basePath) const {
    std::cout << "Saving indices to " << basePath << "..." << std::endl;
    
    try {
        // Create directory if it doesn't exist
        std::filesystem::path dirPath = std::filesystem::path(basePath).parent_path();
        if (!dirPath.empty() && !std::filesystem::exists(dirPath)) {
            std::filesystem::create_directories(dirPath);
        }
        
        // Save word index
        wordIndex.serialize(basePath + ".words");
        
        // Save organization index
        organizationIndex.serialize(basePath + ".orgs");
        
        // Save person index
        personIndex.serialize(basePath + ".persons");
        
        // Save document metadata
        std::ofstream metaFile(basePath + ".meta", std::ios::binary);
        if (!metaFile) {
            throw std::runtime_error("Failed to open metadata file for writing");
        }
        
        size_t docCount = documentIDs.size();
        metaFile.write(reinterpret_cast<const char*>(&docCount), sizeof(docCount));
        
        for (const auto& docID : documentIDs) {
            size_t idSize = docID.size();
            metaFile.write(reinterpret_cast<const char*>(&idSize), sizeof(idSize));
            metaFile.write(docID.c_str(), idSize);
            
            auto metadata = documentMetadata.find(docID);
            std::string metaStr = (metadata != documentMetadata.end()) ? 
                                metadata->second : "{}";
            
            size_t metaSize = metaStr.size();
            metaFile.write(reinterpret_cast<const char*>(&metaSize), sizeof(metaSize));
            metaFile.write(metaStr.c_str(), metaSize);
        }
        
        std::cout << "Indices saved successfully." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving indices: " << e.what() << std::endl;
        throw;
    }
}

void IndexHandler::loadIndices(const std::string& basePath) {
    std::cout << "Loading indices from " << basePath << "..." << std::endl;
    
    try {
        // Load word index
        wordIndex.deserialize(basePath + ".words");
        
        // Load organization index
        organizationIndex.deserialize(basePath + ".orgs");
        
        // Load person index
        personIndex.deserialize(basePath + ".persons");
        
        // Load document metadata
        std::ifstream metaFile(basePath + ".meta", std::ios::binary);
        if (!metaFile) {
            throw std::runtime_error("Failed to open metadata file for reading");
        }
        
        size_t docCount;
        metaFile.read(reinterpret_cast<char*>(&docCount), sizeof(docCount));
        
        documentIDs.clear();
        documentMetadata.clear();
        
        for (size_t i = 0; i < docCount; ++i) {
            size_t idSize;
            metaFile.read(reinterpret_cast<char*>(&idSize), sizeof(idSize));
            
            std::string docID(idSize, ' ');
            metaFile.read(&docID[0], idSize);
            
            size_t metaSize;
            metaFile.read(reinterpret_cast<char*>(&metaSize), sizeof(metaSize));
            
            std::string metaStr(metaSize, ' ');
            metaFile.read(&metaStr[0], metaSize);
            
            documentIDs.insert(docID);
            documentMetadata[docID] = metaStr;
        }
        
        std::cout << "Loaded " << documentIDs.size() << " documents." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading indices: " << e.what() << std::endl;
        throw;
    }
}

std::vector<std::pair<std::string, double>> IndexHandler::searchWord(const std::string& term) const {
    return wordIndex.search(term);
}

std::vector<std::pair<std::string, double>> IndexHandler::searchOrganization(const std::string& org) const {
    return organizationIndex.search(org);
}

std::vector<std::pair<std::string, double>> IndexHandler::searchPerson(const std::string& person) const {
    return personIndex.search(person);
}