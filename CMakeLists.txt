
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(HEADERS_WAY ${CMAKE_SOURCE_DIR}/search_engine/headers)
set(SOURCE_WAY ${CMAKE_SOURCE_DIR}/search_engine/sources)

include(FetchContent)

add_subdirectory("search_engine")
add_subdirectory("tests")

