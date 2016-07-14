cmake_minimum_required(VERSION 3.3)
project(CuckooSniffer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTINS_STATIC")

if (WIN32)

    set(WINPCAP_ROOT D:/libs/WpdPack)
    set(LIBTINS_ROOT D:/libs/libtins)


    set(WINPCAP_INCLUDE ${WINPCAP_ROOT}/Include)
    set(WINPCAP_LIB     ${WINPCAP_ROOT}/Lib)
    set(LIBTINS_INCLUDE ${LIBTINS_ROOT}/include)
    set(LIBTINS_LIB     ${LIBTINS_ROOT}/lib)

    include_directories(WINPCAP_INCLUDE LIBTINS_INCLUDE)
    link_libraries(WINPCAP_LIB LIBTINS_LIB)

elseif (UNIX)

    set(INCLUDES
            ${PROJECT_SOURCE_DIR}/src
            /usr/local/include)

    include_directories(${INCLUDES})
    link_directories(/usr/local/lib)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++1z")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")

endif()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(LIBS tins pcap)

set(SRC_FILES
        src/main.cpp
        src/sniffer_manager.cpp
        src/smtp/smtp_sniffer.cpp
        src/smtp/smtp_sniffer_data.cpp
        src/smtp/smtp_data_processor.cpp
        src/util/base64.cpp
        src/util/file.cpp
        )

add_executable(CuckooSniffer ${SRC_FILES})

target_link_libraries(CuckooSniffer ${LIBS})
