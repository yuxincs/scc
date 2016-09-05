cmake_minimum_required(VERSION 2.8)

project(scc)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

include_directories(include)

# bison and flex
find_package(BISON)
find_package(FLEX)
flex_target(SCANNER source/scc.l ${CMAKE_CURRENT_BINARY_DIR}/scc_lex.cpp)
bison_target(PARSER source/scc.y ${CMAKE_CURRENT_BINARY_DIR}/scc_yacc.cpp COMPILE_FLAGS -d)
ADD_FLEX_BISON_DEPENDENCY(SCANNER PARSER)

file(GLOB_RECURSE source_files "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(scc ${SOURCE_FILES}
                   ${BISON_PARSER_OUTPUTS}
                   ${FLEX_SCANNER_OUTPUTS})