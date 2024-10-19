cmake_minimum_required(VERSION 3.29)
project(PadBus)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
include_directories(include/PadBus)
include_directories(include/PadBus/Client)
include_directories(include/PadBus/Host)

add_library(PadBus
        src/CRC.cpp
        src/Event.cpp
        src/IObserver.cpp
        src/StateMachine.cpp
        src/Subject.cpp)

add_executable(
        PadBus_ScratchConsole
        scratch/main.cpp
)

target_link_libraries(PadBus_ScratchConsole PadBus)

# GoogleTest requires threading support
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Add GoogleTest
enable_testing()
add_subdirectory(test)