cmake_minimum_required(VERSION 3.29)
project(PadBus)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
include_directories(include/PadBus)

add_executable(PadBus
        include/PadBus/Event.h
        include/PadBus/IBusIODriver.h
        include/PadBus/IObservable.h
        src/Event.cpp
        src/FunctionObserver.cpp
        src/main.cpp
    test/MockClock.cpp)
