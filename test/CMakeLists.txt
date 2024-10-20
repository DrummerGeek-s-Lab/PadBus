cmake_minimum_required(VERSION 3.29)
project(PadBus_Tests)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(
        PadBus_tests
        TestRunner.cpp
        CRC_tests.cpp
        Event_tests.cpp
        StateMachine_tests.cpp)

target_link_libraries(PadBus_tests PadBus gtest_main)