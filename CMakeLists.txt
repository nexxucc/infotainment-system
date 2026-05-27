cmake_minimum_required(VERSION 3.22)
project(infotainment-simulator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Export compile commands for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find dependencies
find_package(Qt6 REQUIRED COMPONENTS Widgets Network)
find_package(GTest REQUIRED)
find_package(nlohmann_json REQUIRED)

# Add subdirectories
add_subdirectory(app)
add_subdirectory(tests/cpp)
