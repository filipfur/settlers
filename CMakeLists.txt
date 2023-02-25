cmake_minimum_required(VERSION 3.22) 

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(settlers)

include_directories(
    include
)

file(GLOB SOURCES
    src/*.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
target_link_libraries(${CMAKE_PROJECT_NAME} lithium)

add_executable(testecs test/testecs.cpp lithium/src/ecs.cpp)
target_include_directories(testecs PUBLIC lithium/include/lithium/ecs)

add_subdirectory(lithium)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin)
install(TARGETS testecs DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin)