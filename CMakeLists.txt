cmake_minimum_required(VERSION 3.16)
project(MB_groep_22_23)

set(CMAKE_CXX_STANDARD 14)

AUX_SOURCE_DIRECTORY(lib/doctest DOCTEST_FILES)

AUX_SOURCE_DIRECTORY(lib/nlohmann-json NM_JSON_FILES)

AUX_SOURCE_DIRECTORY(src/objects OBJECT_FILES)

AUX_SOURCE_DIRECTORY(src/objects/EarleyParser OBJECT_FILES_EARLEY_PARSER)

AUX_SOURCE_DIRECTORY(src/objects/FileConverter OBJECT_FILES_FILE_CONVERTER)

AUX_SOURCE_DIRECTORY(src/utilities UTILITY_FILES)

AUX_SOURCE_DIRECTORY(tests T_SOURCE_FILES)

# The source files without tests
set(RELEASE_SOURCE_FILES
        src/main.cpp
        ${OBJECT_FILES}
        ${OBJECT_FILES_EARLEY_PARSER}
        ${OBJECT_FILES_FILE_CONVERTER}
        ${NM_JSON_FILES}
        ${UTILITY_FILES}
        src/objects/LL1Parser.cpp src/objects/LL1Parser.h src/objects/EMLGrammarGenerator.cpp src/objects/EMLGrammarGenerator.h src/objects/EMLTokenizer.cpp src/objects/EMLTokenizer.h)

# The source files with tests
set(TEST_SOURCE_FILES
        ${OBJECT_FILES}
        ${OBJECT_FILES_EARLEY_PARSER}
        ${OBJECT_FILES_FILE_CONVERTER}
        ${NM_JSON_FILES}
        ${UTILITY_FILES}
        ${DOCTEST_FILES}
        ${T_SOURCE_FILES}
        )

add_executable(MB_groep_22_23 ${RELEASE_SOURCE_FILES})
add_executable(MB_groep_22_23_test ${TEST_SOURCE_FILES})
