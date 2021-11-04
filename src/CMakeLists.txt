include(PackableLibrary)

set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

add_packable_library(
    FlatCombining
    NAMESPACE
    FlatCombining
    ROOT_INCLUDE_DIR
    ${INCLUDE_DIR}
    DEPENDENCIES_CMAKE
    Dependencies.cmake
    INTERFACE_INCLUDE_DIRS
    FlatCombining)

add_library(FlatCombining flatcombining.cpp)
add_library(FlatCombining::FlatCombining ALIAS FlatCombining)
set_target_properties(FlatCombining PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(FlatCombining PRIVATE ${INCLUDE_DIR})
target_compile_features(FlatCombining PUBLIC cxx_std_20)

add_warning_flags(FlatCombining)
add_sanitizer_flags(FlatCombining)

include(CPack)
