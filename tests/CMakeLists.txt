include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.17.0
)

FetchContent_MakeAvailable(googletest)

find_package(Qt6 REQUIRED COMPONENTS Core)
# Collect c++ source files
file(GLOB CXX_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_executable(unit_tests ${CXX_FILES})

# Link GoogleTest and core library
target_link_libraries(unit_tests
    PRIVATE
    gtest_main
    bass_core
    Qt6::Core
)

# include directories (including where GoogleTest is built)
target_include_directories(unit_tests PRIVATE ${gtest_SOURCE_DIR}/include)

# Enable testing and discover tests
# Discover and run tests
include(GoogleTest)
gtest_discover_tests(unit_tests)
