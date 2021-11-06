set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

include(FetchContent)
FetchContent_Declare(
    lockfree-queues
    GIT_REPOSITORY https://github.com/harikrishnan94/lockfree-queues
    GIT_TAG v1.0
    GIT_SUBMODULES "")
FetchContent_MakeAvailable(lockfree-queues)

add_library(FlatCombining flatcombining.cpp)
add_library(FlatCombining::FlatCombining ALIAS FlatCombining)
set_target_properties(FlatCombining PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(FlatCombining PUBLIC ${INCLUDE_DIR})
target_compile_features(FlatCombining PUBLIC cxx_std_20)
target_link_libraries(FlatCombining PRIVATE lockfree-queue::LockfreeQueue)

add_warning_flags(FlatCombining)
add_sanitizer_flags(FlatCombining)

include(CPack)
