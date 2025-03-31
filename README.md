# Financial News Search Engine

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

## Usage
1. Index documents:
   ```bash
   ./supersearch index ../data/
   ```

2. Search:
   ```bash
   ./supersearch query "search terms"
   ```

3. Interactive mode:
   ```bash
   ./supersearch ui
   ```