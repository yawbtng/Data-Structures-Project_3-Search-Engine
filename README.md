# Financial News Search Engine

## Project Overview

This project implements a robust search engine specifically designed for financial news articles, featuring AVL tree-based indexing and advanced text processing capabilities. The search engine processes JSON-formatted news articles, indexes terms and entities, and enables efficient querying with relevancy ranking.

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

### Text Processing
- **Stopword Removal**: Common words like "the", "and", "of" are filtered out
- **Porter Stemming**: Normalizes words to their root form (e.g., "running" → "run")
- **Entity Recognition**: Extracts organizations and persons from article metadata

### Relevancy Ranking
The TF-IDF (Term Frequency-Inverse Document Frequency) algorithm is used to calculate the relevance of documents to a query:
- **Term Frequency**: How often a term appears in a document
- **Inverse Document Frequency**: How rare a term is across all documents
- **Final Score**: Combination of TF-IDF scores for all query terms