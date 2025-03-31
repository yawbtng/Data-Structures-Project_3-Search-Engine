cmake_minimum_required(VERSION 3.16)
project(FinancialSearchEngine)

set(CMAKE_CXX_STANDARD 20)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/rapidjson/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/porter2_stemmer/thirdparty/porter2_stemmer
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/porter2_stemmer/thirdparty
)

# Create Porter stemmer library
add_library(porter_stemmer 
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/porter2_stemmer/thirdparty/porter2_stemmer/porter2_stemmer.cpp
)

# Main executable
add_executable(supersearch
    src/main.cpp
    src/DocumentParser.cpp
    src/IndexHandler.cpp
    src/QueryProcessor.cpp
    src/UserInterface.cpp
)

# Link libraries
target_link_libraries(supersearch PRIVATE 
    porter_stemmer
)

# Test executable
add_executable(test_search
    test/test_avltree.cpp
)

# Link test libraries
target_link_libraries(test_search PRIVATE
    porter_stemmer
)