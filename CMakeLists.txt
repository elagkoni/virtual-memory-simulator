cmake_minimum_required(VERSION 3.10)
project(VirtualMemorySimulator)

set(CMAKE_CXX_STANDARD 11)

add_executable(vm_simulator
    main.cpp
    hashtable.cpp
    lru.cpp
    secondchance.cpp
    simulator.cpp
)
