cmake_minimum_required(VERSION 3.15)
project(CortexI LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB UTILS_SRC utils/*.cpp)
add_executable(CortexI main.cpp ${UTILS_SRC})

find_package(glog REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(GTest REQUIRED)
target_link_libraries(CortexI glog::glog nlohmann_json::nlohmann_json)

enable_testing()

add_executable(logger_test tests/logger_test.cpp ${UTILS_SRC})
target_include_directories(logger_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(logger_test glog::glog nlohmann_json::nlohmann_json GTest::gtest GTest::gtest_main)

add_test(NAME LoggerTest COMMAND logger_test)
