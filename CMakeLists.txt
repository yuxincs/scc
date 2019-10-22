cmake_minimum_required(VERSION 2.8)

project(scc)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_INSTALL_PREFIX /usr/local)

include_directories(include ${CMAKE_CURRENT_BINARY_DIR})

# bison and flex
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
flex_target(SCANNER source/scc.l ${CMAKE_CURRENT_BINARY_DIR}/scc_lex.c)
bison_target(PARSER source/scc.y ${CMAKE_CURRENT_BINARY_DIR}/scc_yacc.c)
ADD_FLEX_BISON_DEPENDENCY(SCANNER PARSER)

file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/source/*.c")

add_executable(scc ${SOURCE_FILES}
                   ${BISON_PARSER_OUTPUTS}
                   ${FLEX_SCANNER_OUTPUTS})
target_compile_options(scc PUBLIC -W -Wall -Wextra)

install(TARGETS scc RUNTIME DESTINATION bin)