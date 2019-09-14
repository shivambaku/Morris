cmake_minimum_required(VERSION 3.12)

set(CMAKE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
file(GLOB_RECURSE HEADER_FILES src/*.hpp)

project(morris)

if (MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT /Zi")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()

add_library(morris STATIC ${SOURCE_FILES} ${HEADER_FILES})
set_target_properties(morris PROPERTIES CXX_STANDARD 17)

add_executable(app src/main.cpp)
set_target_properties(app PROPERTIES CXX_STANDARD 17)

target_link_libraries(app morris)

# mkdir build/
# cd build/

# Visual studio generator
# cmake -G "Visual Studio 15 2017 Win64" ..