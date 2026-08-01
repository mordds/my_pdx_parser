cmake_minimum_required(VERSION 3.10)


project(PDXParser VERSION 0.4.1)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Werror -Wall -Wextra -O2 -Wno-unused-function -Wno-stringop-truncation -Wno-unused-parameter -Wno-sign-compare)

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)





BISON_TARGET(MyParser test.y ${CMAKE_CURRENT_BINARY_DIR}/y.tab.cpp
             DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/y.tab.h
             COMPILE_FLAGS "-vdty")

FLEX_TARGET(MyLexer pdx.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)


add_flex_bison_dependency(MyLexer MyParser)

file(GLOB COMMON_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/**/*.cpp")

list(REMOVE_ITEM COMMON_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/test.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/console.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/b.cpp"
)
add_executable(console ${COMMON_SRC} console.cpp ${BISON_MyParser_OUTPUTS} ${FLEX_MyLexer_OUTPUTS} )
add_executable(print_modifier ${COMMON_SRC} test.cpp ${BISON_MyParser_OUTPUTS} ${FLEX_MyLexer_OUTPUTS} )
add_executable(print_trigger  ${COMMON_SRC} b.cpp ${BISON_MyParser_OUTPUTS} ${FLEX_MyLexer_OUTPUTS} )

target_include_directories(console PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(print_modifier PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(print_trigger PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})