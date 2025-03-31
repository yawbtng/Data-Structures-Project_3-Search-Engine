# Search Engine UML Class Diagram

```mermaid
classDiagram
    %% Styling
    classDef coreStructure fill:#f9f,stroke:#333,stroke-width:2px
    classDef indexing fill:#bbf,stroke:#333,stroke-width:1px
    classDef processing fill:#bfb,stroke:#333,stroke-width:1px
    classDef querying fill:#fbb,stroke:#333,stroke-width:1px
    classDef ui fill:#fcb,stroke:#333,stroke-width:1px

    %% Core Data Structures
    class AVLTree~KeyType, ValueType~ {
        -Node* root
        -int height(Node*)
        -int getBalanceFactor(Node*)
        -Node* rotateRight(Node*)
        -Node* rotateLeft(Node*)
        +void insert(KeyType, string, double)
        +vector~pair~string, double~~ search(KeyType)
        +void serialize(string)
        +void deserialize(string)
        +void traverse(Function)
        +bool isEmpty()
    }

    class Node {
        +KeyType key
        +ValueType value
        +map~string, double~ docScores
        +Node* left
        +Node* right
        +int height
    }

    %% Index Management
    class IndexHandler {
        -AVLTree~string, string~ wordIndex
        -AVLTree~string, string~ organizationIndex
        -AVLTree~string, string~ personIndex
        -unordered_set~string~ documentIDs
        -unordered_map~string, string~ documentMetadata
        +size_t getTotalDocuments()
        +size_t getDocumentFrequency(string)
        +void addTerm(string, string, double)
        +void addOrganization(string, string)
        +void addPerson(string, string)
        +void registerDocument(string)
        +void addDocumentMetadata(string, string, string, string)
        +string getDocumentMetadata(string)
        +void saveIndices(string)
        +void loadIndices(string)
        +vector~pair~string, double~~ searchWord(string)
        +vector~pair~string, double~~ searchOrganization(string)
        +vector~pair~string, double~~ searchPerson(string)
    }

    %% Document Processing
    class DocumentParser {
        -IndexHandler& indexHandler
        -unordered_set~string~ stopwords
        +DocumentParser(IndexHandler&, string)
        +void parseJSON(string)
        +void parseDirectory(string)
        +void calculateTFIDF()
        -void loadStopwords(string)
        -void processContent(string, string)
        -void processEntities(rapidjson::Value&, string)
    }

    %% Query Processing
    class QueryProcessor {
        -IndexHandler& indexHandler
        +QueryProcessor(IndexHandler&)
        +vector~QueryResult~ processQuery(string)
        +vector~QueryResult~ rankResults(unordered_map~string, double~, size_t)
        +string getFullArticle(string)
        -tuple parseQuery(string)
        -void applyExclusions(unordered_map~string, double~, vector~string~)
    }

    class QueryResult {
        +string docID
        +double score
        +string title
        +string date
        +string source
        +QueryResult(string, double)
    }

    %% User Interface
    class UserInterface {
        -IndexHandler indexHandler
        -DocumentParser documentParser
        -QueryProcessor queryProcessor
        +UserInterface(string)
        +int run(int, char*[])
        +void displayHelp()
        -void displayResults(vector~QueryResult~)
        -void displayArticle(string)
        -void handleIndexCommand(vector~string~)
        -void handleQueryCommand(vector~string~)
        -void handleUICommand()
    }

    %% Relationships
    AVLTree *-- Node : contains
    IndexHandler *-- AVLTree : contains 3
    DocumentParser o-- IndexHandler : uses
    QueryProcessor o-- IndexHandler : uses
    QueryProcessor ..> QueryResult : creates
    UserInterface *-- IndexHandler : owns
    UserInterface *-- DocumentParser : owns
    UserInterface *-- QueryProcessor : owns
    
    %% Apply styles
    AVLTree::coreStructure
    Node::coreStructure
    IndexHandler::indexing
    DocumentParser::processing
    QueryProcessor::querying
    QueryResult::querying
    UserInterface::ui
```

## System Components

This diagram shows the core components of the Financial News Search Engine:

1. **Core Data Structure** (pink): AVL Tree implementation for efficient word indexing
2. **Index Management** (blue): Handles three indices (words, organizations, persons)
3. **Document Processing** (green): Parses JSON, removes stopwords, applies stemming
4. **Query Processing** (red): Implements boolean operations and relevancy ranking
5. **User Interface** (orange): Command-line interface with search and indexing functions

## Design Patterns

- **Composition**: UserInterface aggregates the other components
- **Dependency Injection**: DocumentParser and QueryProcessor receive IndexHandler
- **Factory Method**: QueryProcessor creates QueryResult objects
- **Visitor Pattern**: AVLTree's traverse method accepts function objects

## Key Features

- Self-balancing AVL tree for efficient search
- TF-IDF scoring for relevancy ranking
- Entity-based search with special operators
- Boolean AND/exclusion operations
- Persistence through serialization


## 2. Functional Description (High-Level Design)

### Core Modules

#### A. AVL Tree Module
- **Purpose**: Self-balancing binary search tree for efficient indexing
- **Key Functions**:
  - `insert()`: Adds terms with document references and TF-IDF scores
  - `search()`: Retrieves documents containing specific terms
  - `serialize()/deserialize()`: Persists index to disk
- **Interfaces**: Template-based implementation for flexibility
- **Data Flow**: Core data structure used by IndexHandler

#### B. Index Management Module
- **Purpose**: Manages three separate indices (words, organizations, persons)
- **Key Functions**:
  - `addTerm()`: Adds words to main index
  - `addOrganization()/addPerson()`: Adds entities to respective indices
  - `searchWord()/searchOrganization()/searchPerson()`: Searches specific indices
- **Data Flow**: Interfaces between AVL trees and higher-level modules

#### C. Document Processing Module
- **Purpose**: Parses JSON articles and extracts searchable content
- **Key Functions**:
  - `parseJSON()`: Processes individual articles
  - `parseDirectory()`: Batch processes multiple articles
  - `calculateTFIDF()`: Computes relevancy scores
- **Data Location**: Reads from /data directory containing JSON files
- **Dependencies**: RapidJSON for parsing, Porter2 Stemmer for word normalization

#### D. Query Processing Module
- **Purpose**: Handles user queries and ranks results
- **Key Functions**:
  - `processQuery()`: Main query handling
  - `rankResults()`: Orders results by relevance
  - `parseQuery()`: Interprets special operators
- **Query Operators**: AND (default), ORG:, PERSON:, exclusion (-)

#### E. User Interface Module
- **Purpose**: Command-line interface for user interaction
- **Commands**:
  - `index`: Creates/updates search indices
  - `query`: Executes searches
  - `ui`: Interactive mode

### Data Flow
1. Document Processing → Index Management → AVL Trees
2. Query Processing → Index Management → AVL Trees → Results
3. User Interface → Query Processing → Ranked Results

## 3. Detailed Design Description

### A. Core Data Structures

#### AVLTree<KeyType, ValueType>
```cpp
template <typename KeyType, typename ValueType>
class AVLTree {
    struct Node {
        KeyType key;                              // Search term
        ValueType value;                          // Unused in current implementation
        unordered_map<string, double> docScores;  // Document ID -> TF-IDF score
        shared_ptr<Node> left, right;
        int height;
    };
    
    // Implementation details...
}
```

### B. Main Function Flow
```cpp
int main(int argc, char* argv[]) {
    // 1. Initialize components
    UserInterface ui("stopwords.txt");
    
    // 2. Process command-line arguments
    if (argc < 2) {
        ui.displayHelp();
        return 1;
    }
    
    // 3. Execute appropriate command
    return ui.run(argc, argv);
}
```

### C. Function Specifications

#### Document Processing
```cpp
void DocumentParser::parseJSON(const string& filename) {
    // USES: RapidJSON, Porter2Stemmer
    // USERS: UserInterface::handleIndexCommand
    
    Parameters:
    - filename: Path to JSON article file
    
    Process:
    1. Read and parse JSON
    2. Extract content and metadata
    3. Tokenize and stem words
    4. Update indices via IndexHandler
}
```

#### Query Processing
```cpp
vector<QueryResult> QueryProcessor::processQuery(const string& query) {
    // USES: IndexHandler, Porter2Stemmer
    // USERS: UserInterface::handleQueryCommand
    
    Parameters:
    - query: User search string (e.g., "apple ORG:Google -microsoft")
    
    Returns:
    - Vector of QueryResult objects, sorted by relevance
    
    Process:
    1. Parse query into components
    2. Search appropriate indices
    3. Apply boolean operations
    4. Rank results
    5. Return top 15 matches
}
```

### D. Data Structures Used

1. **Indices**
   - Three AVL trees (words, organizations, persons)
   - Each node contains document references and TF-IDF scores

2. **Document Storage**
   ```cpp
   struct DocumentMetadata {
       string title;
       string date;
       string source;
       // Stored as JSON string in IndexHandler
   };
   ```

3. **Query Results**
   ```cpp
   struct QueryResult {
       string docID;
       double score;
       string title;
       string date;
       string source;
   };
   ```

### E. File Organization
Project_3/
├── include/ # Header files
├── src/ # Implementation files
├── test/ # Unit tests
├── thirdparty/ # External libraries
├── data/ # JSON articles
└── stopwords.txt # Common words to ignore

Project_3/
├── include/ # Header files
├── src/ # Implementation files
├── test/ # Unit tests
├── thirdparty/ # External libraries
├── data/ # JSON articles
└── stopwords.txt # Common words to ignore