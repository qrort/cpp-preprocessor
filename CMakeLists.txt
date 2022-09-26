cmake_minimum_required(VERSION 3.13)
project(preprocessor)

cmake_policy(SET CMP0054 NEW)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
# required if linking to static library
add_definitions(-DANTLR4CPP_STATIC)


# set variable pointing to the antlr tool that supports C++
# this is not required if the jar file can be found under PATH environment
set(ANTLR_EXECUTABLE ${PROJECT_SOURCE_DIR}/antlr/antlr-4.10.1-complete.jar)

# add external build for antlrcpp
include(ExternalAntlr4Cpp)

# add macros to generate ANTLR Cpp code from grammar
find_package(ANTLR REQUIRED)

#dirty hack
set(ANTLR4CPP_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/cmake-build-debug/antlr4_runtime/src/antlr4_runtime/runtime/Cpp/runtime/src)

# Include the runtime to compile against
include_directories( ${ANTLR4CPP_INCLUDE_DIRS} )

include_directories(${PROJECT_SOURCE_DIR}/gen)

add_library(genlib
        gen/Cpp14Lexer.cpp
        gen/Cpp14Parser.cpp
        gen/Cpp14ParserVisitor.cpp
        gen/Cpp14ParserBaseVisitor.cpp
        gen/DefineLexer.cpp
        gen/DefineParser.cpp
        gen/DefineParserBaseVisitor.cpp
        gen/DefineParserVisitor.cpp)

target_include_directories(genlib PRIVATE  ${ANTLR4CPP_INCLUDE_DIRS})
target_link_libraries(genlib antlr4_static)
add_executable(preprocessor main.cpp DefineCollector.cpp DefineCollector.h Preprocessor.cpp Preprocessor.h DefineMacro.cpp DefineMacro.h PreprocessorVisitor.cpp PreprocessorVisitor.h)
target_link_libraries(preprocessor genlib antlr4_static)