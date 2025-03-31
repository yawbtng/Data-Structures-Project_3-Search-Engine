# Financial News Search Engine

<div align="center">
  <img src="https://img.shields.io/badge/C++-17-blue.svg?style=flat&logo=c%2B%2B" alt="C++ 17"/>
  <img src="https://img.shields.io/badge/NLP-Document%20Search-orange.svg?style=flat" alt="NLP"/>
  <img src="https://img.shields.io/badge/Data%20Structures-AVL%20Trees-green.svg?style=flat" alt="Data Structures"/>
  <img src="https://img.shields.io/badge/Custom%20Implementation-red.svg?style=flat" alt="Custom Implementation"/>
  <img src="https://img.shields.io/badge/Memory-Dynamic%20Allocation-purple.svg?style=flat" alt="Memory"/>
  <img src="https://img.shields.io/badge/ML-Lexicon%20Based-blueviolet.svg?style=flat" alt="ML"/>
</div>

<p align="center">
  <a href="#project-overview">Overview</a> •
  <a href="#key-features">Features</a> •
  <a href="#architecture">Architecture</a> •
  <a href="#build-instructions">Build</a> •
  <a href="#usage">Usage</a> •
  <a href="#query-syntax">Query Syntax</a> •
  <a href="#implementation-details">Implementation</a>
</p>

<div align="center">
  <img src="https://user-images.githubusercontent.com/74038190/212750155-3ceddfbd-19d3-40a3-87af-8d329c8323c4.gif" width="700">
</div>

## Project Overview

This project implements a robust search engine specifically designed for financial news articles, featuring AVL tree-based indexing and advanced text processing capabilities. The search engine processes JSON-formatted news articles, indexes terms and entities, and enables efficient querying with relevancy ranking.

<div align="center">
  <img src="https://user-images.githubusercontent.com/74038190/213910845-af37a709-8995-40d6-be59-724526e3c3d7.gif" width="900">
</div>

## Key Features

- **Custom AVL Tree Implementation**: Self-balancing binary search trees for O(log n) insertion, deletion, and search operations
- **Multi-Index System**: Separate indices for words, organizations, and persons
- **Text Processing**: Stopword removal and Porter stemming for efficient indexing
- **Entity Extraction**: Organization and person entity recognition
- **Advanced Query Capabilities**: AND operations, entity-specific searches, exclusion operators
- **Relevancy Ranking**: TF-IDF based relevancy scoring for search results
- **Persistent Storage**: Serialization and deserialization of indices

## Build Requirements
- C++20 compiler
- CMake 3.16 or higher
- Boost library
- RapidJSON (included)
- Porter2 Stemmer (included)

## Build Instructions
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Architecture

<div align="center">
  <img src="https://user-images.githubusercontent.com/74038190/212749447-bfb7e725-6987-49d9-ae85-2015e3e7cc41.gif" width="500">
</div>

### 1. Document Parser
Processes JSON news articles using RapidJSON, normalizes text with Porter stemming, removes stopwords, and extracts metadata.

### 2. Index Handler
Maintains three AVL trees for different types of information:
- Words index: Maps stemmed words to document references
- Organizations index: Maps organization names to document references
- Persons index: Maps person names to document references

### 3. Query Processor
Processes user queries with boolean operations, entity-specific searches, and term exclusion. Results are ranked by relevance using TF-IDF scoring.

### 4. User Interface
Provides a command-line interface with three main commands:
- `index`: Creates an index from financial news articles
- `query`: Searches the index with advanced query syntax
- `ui`: Launches an interactive user interface

## Usage

### Indexing Documents
```bash
./supersearch index /path/to/financial/news/data
```

### Searching
```bash
# Basic search
./supersearch query "apple quarterly results"

# Organization search
./supersearch query "ORG:Amazon"

# Person search
./supersearch query "PERSON:Bezos"

# Exclusion
./supersearch query "finance -crypto"
```

### Interactive Mode
```bash
./supersearch ui
```

## Query Syntax

- `word1 word2`: Search for documents containing all terms (AND operation)
- `ORG:Google`: Search for documents mentioning the organization "Google"
- `PERSON:Musk`: Search for documents mentioning the person "Musk"
- `-excludeword`: Exclude documents containing this term

## Interactive UI Commands
The interactive mode supports additional commands:
- `load <path>`: Load an existing index
- `save <path>`: Save the current index
- `view <number>`: View full article from search results
- `exit/quit`: Exit the program

## Implementation Details

### AVL Tree
The project implements a custom AVL tree data structure that provides efficient O(log n) operations while maintaining balance through automatic rotations.

<div align="center">
  <img src="https://user-images.githubusercontent.com/74038190/238200431-3c16d4f0-9acd-4ffa-9dc9-38d3e9196d46.gif" width="400">
</div>

### Text Processing
- **Stopword Removal**: Common words like "the", "and", "of" are filtered out
- **Porter Stemming**: Normalizes words to their root form (e.g., "running" → "run")
- **Entity Recognition**: Extracts organizations and persons from article metadata

### Relevancy Ranking
The TF-IDF (Term Frequency-Inverse Document Frequency) algorithm is used to calculate the relevance of documents to a query:
- **Term Frequency**: How often a term appears in a document
- **Inverse Document Frequency**: How rare a term is across all documents
- **Final Score**: Combination of TF-IDF scores for all query terms

<div align="center">
  <img src="https://user-images.githubusercontent.com/74038190/212284158-e840e285-664b-44d7-b79b-e264b5e54825.gif" width="400">
</div>