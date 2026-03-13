cmake_minimum_required(VERSION 3.23)

project(NDS-DeviceAccess)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

cmake_policy(SET CMP0167 OLD)

find_program(CPPCHECK_PATH cppcheck)
if(CPPCHECK_PATH)
    set(CMAKE_CXX_CPPCHECK
        ${CPPCHECK_PATH}
        --enable=all
        --inline-suppr
        --std=c++20
        --template=gcc
        --check-level=exhaustive
        --suppress=missingIncludeSystem
        --suppress=missingInclude
        --suppress=unusedFunction
        --suppress=unmatchedSuppression
        --suppress=checkersReport
        -I/usr/local/include
        --suppress=*:*usr/local/*
        --suppress=*:*_deps/*
        --quiet
    )
endif()

find_package(nds-da REQUIRED)

# ============================== NDS-TMPE =====================================
add_library(nds-tmpe SHARED
    "src/main/c++/nds-tmpe/nds-tmpe.cpp"
    "src/main/c++/nds-tmpe/nds-tmpeFirmware.cpp"
)
target_include_directories(nds-tmpe PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/c++/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(nds-tmpe PUBLIC nds3 nds-da)
target_compile_options(nds-tmpe PRIVATE -Wall -Wextra -Wpedantic)
set(CMAKE_CXX_CLANG_TIDY clang-tidy)

# ============================== Google Tests =====================================
set(INSTALL_GTEST "OFF")
cmake_policy(SET CMP0135 NEW)
include(FetchContent)
FetchContent_Declare( googletest
  URL https://github.com/google/googletest/releases/download/v1.17.0/googletest-1.17.0.tar.gz
)
FetchContent_MakeAvailable(googletest)
enable_testing()

add_executable(nds-tmpe-test
    "src/test/c++/nds-tmpe-test/nds-tmpe-test.cpp"
    "src/test/c++/nds-tmpe-test/nds-tmpe-basictests.cpp"
)
target_compile_options(nds-tmpe-test PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(nds-tmpe-test PRIVATE
    nds-tmpe
    GTest::gtest_main
    atomic
)
target_include_directories(nds-tmpe-test PRIVATE
    "src/test/c++/include"
)
set_target_properties(nds-tmpe-test PROPERTIES
    BUILD_RPATH "/usr/local/lib64/"
)

include(GoogleTest)
gtest_discover_tests(nds-tmpe-test)

# ============================== NDS3 =====================================
find_library(nds3
    NAMES nds3
    PATHS /usr/local/lib64
    REQUIRED
)
find_library(nds3-testcontrolsystem
    NAMES nds3-testcontrolsystem
    PATHS /usr/local/lib64
    REQUIRED
)

target_link_libraries(nds-tmpe PUBLIC nds3)
target_include_directories(nds-tmpe PRIVATE /usr/local/include)

target_link_libraries(nds-tmpe-test PRIVATE nds3-testcontrolsystem)
target_include_directories(nds-tmpe-test PRIVATE /usr/local/include)