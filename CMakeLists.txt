cmake_minimum_required(VERSION 3.3)
project(Demoniac)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++1z")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D DC_DEBUG")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")


set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#set(LIBS tins pcap)

LINK_DIRECTORIES(/usr/local/lib)

set(INCLUDES /usr/local/include)

include_directories(${INCLUDES})

# The version number.
set (CuckooSniffer_VERSION_MAJOR 0)
set (CuckooSniffer_VERSION_MINOR 1)


add_executable(CuckooSniffer src/cuckoo_sniffer.cpp)

target_link_libraries(CuckooSniffer tins)
