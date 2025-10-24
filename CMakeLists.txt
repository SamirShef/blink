cmake_minimum_required(VERSION 3.10)

project(cc)

find_package(LLVM REQUIRED CONFIG)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_COMPILER clang)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_BUILD_TYPE Release)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(cc ${SOURCES})

include_directories(${CMAKE_SOURCE_DIR}/include)

target_include_directories(сс PRIVATE ${LLVM_INCLUDE_DIRS})
target_compile_definitions(сс PRIVATE ${LLVM_DEFINITIONS})

if (TARGET LLVM)
    target_link_libraries(сс PRIVATE LLVM)
else()
    llvm_map_components_to_libnames(LLVM_LIBS all)
    target_link_libraries(сс PRIVATE ${LLVM_LIBS})
endif()