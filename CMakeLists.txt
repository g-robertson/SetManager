# I expect at bare minimum 3 year to date software (2021->2018)
cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Adds compile command definitions for vscode
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set default cmake build type to release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(SET_MANAGER_VERSION 0.0.1)

# Project name
project(SetManager
    VERSION ${SET_MANAGER_VERSION}
    HOMEPAGE_URL https://github.com/g-robertson/set-manager/
    LANGUAGES C CXX
)


# Add executable
add_executable(SetManager src/main.cpp)
# Add src
add_subdirectory(src)
target_include_directories(SetManager PRIVATE src)