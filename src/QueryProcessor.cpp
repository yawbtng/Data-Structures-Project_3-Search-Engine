/**
 * @file QueryProcessor.cpp
 * @author <YourName>
 * @brief Implementation of query processing
 */

#include "../include/QueryProcessor.h"
#include <sstream>
#include <algorithm>
#include <cmath>
#include "../thirdparty/porter2_stemmer/thirdparty/porter2_stemmer/porter2_stemmer.h"
#include "../thirdparty/rapidjson/include/rapidjson/document.h"

std::tuple<std::vector<std::string>, std::vector<std::string>, 
          std::vector<std::string>, std::vector<std::string>> 
QueryProcessor::parseQuery(const std::string& query) {
    std::vector<std::string> terms, orgs, persons, exclusions;
    std::istringstream iss(query);
    std::string token;
    
    while (iss >> token) {
        // Process special operator prefixes
        if (token.size() >= 5 && token.substr(0, 4) == "ORG:") {
            orgs.push_back(token.substr(4));
        } 
        else if (token.size() >= 8 && token.substr(0, 7) == "PERSON:") {
            persons.push_back(token.substr(7));
        }
        else if (!token.empty() && token[0] == '-' && token.size() > 1) {
            // Exclusion terms
            std::string term = token.substr(1);
            std::transform(term.begin(), term.end(), term.begin(), 
                          [](unsigned char c) { return std::tolower(c); });
            Porter2Stemmer::stem(term);
            exclusions.push_back(term);
        }
        else {
            // Regular search terms
            std::transform(token.begin(), token.end(), token.begin(), 
                          [](unsigned char c) { return std::tolower(c); });
            Porter2Stemmer::stem(token);
            terms.push_back(token);
        }
    }
    
    return {terms, orgs, persons, exclusions};
}

void QueryProcessor::applyExclusions(std::unordered_map<std::string, double>& results, 
                                   const std::vector<std::string>& exclusions) {
    for (const auto& term : exclusions) {
        auto excludeDocs = indexHandler.searchWord(term);
        for (const auto& [docID, _] : excludeDocs) {
            results.erase(docID);
        }
    }
}

std::vector<QueryResult> QueryProcessor::processQuery(const std::string& query) {
    auto [terms, orgs, persons, exclusions] = parseQuery(query);
    
    std::unordered_map<std::string, double> scores;
    
    // Process regular terms (using AND semantics)
    if (!terms.empty()) {
        auto firstResults = indexHandler.searchWord(terms[0]);
        
        // Start with the first term's documents
        for (const auto& [docID, score] : firstResults) {
            scores[docID] = score;
        }
        
        // Apply AND semantics for additional terms
        for (size_t i = 1; i < terms.size(); ++i) {
            auto termResults = indexHandler.searchWord(terms[i]);
            std::unordered_map<std::string, double> tempScores;
            
            for (const auto& [docID, score] : termResults) {
                if (scores.find(docID) != scores.end()) {
                    tempScores[docID] = scores[docID] + score;
                }
            }
            
            scores = tempScores;
        }
    }
    
    // Add organization matches
    for (const auto& org : orgs) {
        auto orgResults = indexHandler.searchOrganization(org);
        for (const auto& [docID, score] : orgResults) {
            scores[docID] = scores.count(docID) ? scores[docID] + score * 1.5 : score * 1.5;
        }
    }
    
    // Add person matches
    for (const auto& person : persons) {
        auto personResults = indexHandler.searchPerson(person);
        for (const auto& [docID, score] : personResults) {
            scores[docID] = scores.count(docID) ? scores[docID] + score * 1.5 : score * 1.5;
        }
    }
    
    // Apply exclusions
    applyExclusions(scores, exclusions);
    
    // Rank and return results
    return rankResults(scores);
}

std::vector<QueryResult> QueryProcessor::rankResults(
    const std::unordered_map<std::string, double>& rawScores, size_t limit) {
    
    std::vector<QueryResult> results;
    
    for (const auto& [docID, score] : rawScores) {
        auto meta = indexHandler.getDocumentMetadata(docID);
        rapidjson::Document doc;
        doc.Parse(meta.c_str());
        
        QueryResult result(docID, score);
        
        if (doc.HasMember("title") && doc["title"].IsString()) {
            result.title = doc["title"].GetString();
        }
        
        if (doc.HasMember("date") && doc["date"].IsString()) {
            result.date = doc["date"].GetString();
        }
        
        if (doc.HasMember("source") && doc["source"].IsString()) {
            result.source = doc["source"].GetString();
        }
        
        results.push_back(result);
    }
    
    // Sort by score (descending order)
    std::sort(results.begin(), results.end(), 
             [](const QueryResult& a, const QueryResult& b) {
                 return a.score > b.score;
             });
    
    // Limit results
    if (results.size() > limit) {
        results.resize(limit);
    }
    
    return results;
}

std::string QueryProcessor::getFullArticle(const std::string& docID) const {
    // In a real implementation, we would load the article from the original file
    // This is a simplified placeholder
    
    // Return a placeholder message
    return "Article content for document ID: " + docID + "\n"
           "This functionality requires loading the original JSON files.";
}