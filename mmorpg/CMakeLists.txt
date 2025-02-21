cmake_minimum_required(VERSION 3.15)
project(mmorpg)

if (DEFINED ENV{VCPKG_DEFAULT_TRIPLET} AND NOT DEFINED VCPKG_TARGET_TRIPLET)
    set(VCPKG_TARGET_TRIPLET "$ENV{VCPKG_DEFAULT_TRIPLET}" CACHE STRING "The vcpkg triplet")
endif()

find_package(Boost REQUIRED COMPONENTS system)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(mmorpg
    "mmorpg.cpp"
    "server.cpp"
    "server.h"
    "scheduler.h"
    "scheduler.cpp"
    "networkManager.h"
    "networkManager.cpp"
)

target_link_libraries(mmorpg PRIVATE Boost::system)

