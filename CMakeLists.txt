cmake_minimum_required(VERSION 3.21)

project(
    FlatCombining
    VERSION 0.1
    LANGUAGES C CXX
    DESCRIPTION "")

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

option(ENABLE_CLANG_TIDY "Enable clang-tidy analysis." OFF)
if(ENABLE_CLANG_TIDY)
    set(CMAKE_CXX_CLANG_TIDY clang-tidy;)
endif(ENABLE_CLANG_TIDY)

option(ENABLE_IWYU "Enable Include What You Use analysis." OFF)
if(ENABLE_IWYU)
    set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE
        include-what-you-use;-Xiwyu;cxx17ns;-Xiwyu;transitive_includes_only;)
endif(ENABLE_IWYU)

option(ENABLE_CPPCHECK "Enable cppcheck analysis." OFF)
if(ENABLE_CPPCHECK)
    file(WRITE ${CMAKE_BINARY_DIR}/cppcheck-supression-list.txt
         "missingIncludeSystem\npreprocessorErrorDirective\n")
    set(CMAKE_CXX_CPPCHECK "cppcheck" ; --quiet;--enable=all;--inline-suppr;
                           --suppressions-list=${CMAKE_BINARY_DIR}/cppcheck-supression-list.txt)
endif(ENABLE_CPPCHECK)

option(GITHUB_PERSONAL_ACCESS_TOKEN "Personal Access Token for accessing the github repositories"
       "")

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

include(SanitizerFlags)
include(WarningFlags)
include(GoldLinker)

add_subdirectory(src)

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(test)
endif()
