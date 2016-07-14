cmake_minimum_required(VERSION 3.3)
project(CuckooSniffer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTINS_STATIC")


set(INCLUDE_DIRS
        ${PROJECT_SOURCE_DIR}/src)

set(LIB_DIRS)

set(LIBS tins)

if (WIN32)

    set(WINPCAP_ROOT D:/libs/WpdPack)
    set(LIBTINS_ROOT D:/libs/libtins)


    set(WINPCAP_INCLUDE ${WINPCAP_ROOT}/Include)
    set(WINPCAP_LIB     ${WINPCAP_ROOT}/Lib)
    set(LIBTINS_INCLUDE ${LIBTINS_ROOT}/include)
    set(LIBTINS_LIB     ${LIBTINS_ROOT}/lib)

    set(INCLUDE_DIRS    ${INCLUDE_DIRS} ${WINPCAP_INCLUDE} ${LIBTINS_INCLUDE})
    set(LIB_DIRS        ${LIB_DIRS} ${WINPCAP_LIB} ${LIBTINS_LIB})

    set(LIBS ${LIBS} wpcap)

elseif (UNIX)

    set(INCLUDE_DIRS    ${INCLUDE_DIRS} /usr/local/include)
    set(LIB_DIRS        ${LIB_DIRS} /usr/local/lib)

    set(LIBS ${LIBS} pcap)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++1z")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")

endif()

include_directories(${INCLUDE_DIRS})
link_directories(${LIB_DIRS})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

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
