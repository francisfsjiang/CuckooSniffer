cmake_minimum_required(VERSION 3.3)
project(CuckooSniffer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTINS_STATIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_ALL_DYN_LINK")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_LOG_DYN_LINK")
set(CMAKE_CXX_LINKER_FLAGS "-static")

set(INCLUDE_DIRS
        ${PROJECT_SOURCE_DIR}/src)

set(LIB_DIRS)

set(LIBS tins
        boost_log
        boost_log_setup
        boost_thread
        boost_system
        boost_date_time
        )

if (WIN32)

    set(WINPCAP_ROOT D:/libs/WpdPack)
    set(LIBTINS_ROOT D:/libs/libtins)

    set(WINPCAP_INCLUDE ${WINPCAP_ROOT}/Include)
    set(WINPCAP_LIB     ${WINPCAP_ROOT}/Lib)
    set(LIBTINS_INCLUDE ${LIBTINS_ROOT}/include)
    set(LIBTINS_LIB     ${LIBTINS_ROOT}/lib)

    set(INCLUDE_DIRS    ${INCLUDE_DIRS} ${WINPCAP_INCLUDE} ${LIBTINS_INCLUDE})
    set(LIB_DIRS        ${LIB_DIRS} ${WINPCAP_LIB} ${LIBTINS_LIB})

    set(LIBS ${LIBS} wpcap Ws2_32 Iphlpapi)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")

elseif (APPLE)

    set(INCLUDE_DIRS    ${INCLUDE_DIRS} /usr/local/include /usr/local/boost/include)
    set(LIB_DIRS        ${LIB_DIRS} /usr/local/lib /usr/local/boost/lib)

    set(LIBS ${LIBS} pcap)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++1z")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")

elseif (UNIX)
    set(CMAKE_CXX_COMPILER g++-6)

    set(INCLUDE_DIRS    ${INCLUDE_DIRS}
        /usr/local/include
        /usr/local/libtins/include
        )
    set(LIB_DIRS        ${LIB_DIRS}
        /usr/local/lib
        /usr/local/libtins/lib
        )

    set(LIBS ${LIBS} pcap pthread)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++17")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")

endif()

include_directories(${INCLUDE_DIRS})
link_directories(${LIB_DIRS})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(SRC_FILES
        src/cuckoo_sniffer.cpp
        src/sniffer_manager.cpp
        src/base/sniffer.cpp
        src/util/base64.cpp
        src/util/file.cpp
        src/util/function.cpp
        src/util/mail_process.cpp
        src/smtp/sniffer.cpp
        src/smtp/collected_data.cpp
        src/smtp/data_processor.cpp
        src/imap/sniffer.cpp
        src/imap/collected_data.cpp
        src/imap/data_processor.cpp
        src/ftp/data_sniffer.cpp
        src/ftp/collected_data.cpp
        src/ftp/data_processor.cpp
        src/ftp/command_sniffer.cpp
        src/http/sniffer.cpp
        src/http/collected_data.cpp
        src/http/data_processor.cpp
        src/samba/sniffer.cpp
        src/samba/collected_data.cpp
        src/samba/data_processor.cpp
        )

add_library(libcuckoo_sniffer STATIC ${SRC_FILES})

add_executable(CuckooSniffer src/main.cpp)

target_link_libraries(CuckooSniffer libcuckoo_sniffer ${LIBS})

add_executable(Test src/test.cpp)

target_link_libraries(Test libcuckoo_sniffer ${LIBS})
