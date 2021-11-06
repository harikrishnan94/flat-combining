find_package(doctest CONFIG REQUIRED)

# Add sources here
add_executable(TestFlatCombining testbase.cpp test_function_ptr.cpp test_flat_combining.cpp)
target_link_libraries(TestFlatCombining PRIVATE FlatCombining::FlatCombining doctest::doctest)

add_warning_flags(TestFlatCombining)
add_sanitizer_flags(TestFlatCombining)

enable_testing()
add_test(NAME TestFlatCombining COMMAND TestFlatCombining -d yes)
