cmake_minimum_required(VERSION 2.8)
project(CuckooSniffer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTINS_STATIC")
set(CMAKE_CXX_LINKER_FLAGS "-static")

set(INCLUDE_DIRS
        ${PROJECT_SOURCE_DIR}/src)

set(LIB_DIRS)

set(LIBS tins)

set(BOOST_LIBS
    boost_log
    boost_log_setup
    boost_thread
    boost_system
    boost_date_time
)

if (WIN32)

    set(INCLUDE_DIRS    ${INCLUDE_DIRS}
        C:/libs/WpdPack/Include
        C:/libs/libtins64-debug/include
        C:/libs/boost64
        C:/libs/OpenSSL-Win64/include
    )
    set(LIB_DIRS        ${LIB_DIRS}
        C:/libs/WpdPack/Lib/x64
        C:/libs/libtins64-debug/lib
        C:/libs/boost64/lib64-msvc-14.0
        C:/libs/OpenSSL-Win64/lib
    )

    set(LIBS ${LIBS}
        wpcap
        Ws2_32
        Iphlpapi
        libeay32  #libcrypto on windows
    )

elseif (APPLE)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_LOG_DYN_LINK")

    set(INCLUDE_DIRS    ${INCLUDE_DIRS}
        /usr/local/include
        /usr/local/boost/include
        /usr/local/opt/openssl/include
    )
    set(LIB_DIRS        ${LIB_DIRS}
        /usr/local/lib
        /usr/local/boost/lib
        /usr/local/opt/openssl/lib
    )

    set(LIBS ${LIBS} ${BOOST_LIBS} pcap crypto)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")

elseif (UNIX)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_LOG_DYN_LINK")

    set(INCLUDE_DIRS    ${INCLUDE_DIRS}
          /usr/local/include
          /usr/local/libtins/include
        )
    set(LIB_DIRS        ${LIB_DIRS}
          /usr/local/lib
          /usr/local/libtins/lib
        )

    set(LIBS ${LIBS} ${BOOST_LIBS} pcap pthread crypto)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11")
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
        src/threads/thread.cpp
        src/threads/data_queue.cpp
        src/base/data_processor.cpp src/list_devs.cpp)

add_library(libcuckoo_sniffer STATIC ${SRC_FILES})

add_executable(CuckooSniffer src/main.cpp)
target_link_libraries(CuckooSniffer libcuckoo_sniffer ${LIBS})

add_executable(Test src/test.cpp)
target_link_libraries(Test libcuckoo_sniffer ${LIBS})

add_executable(ListDevs src/list_devs.cpp)
target_link_libraries(ListDevs libcuckoo_sniffer ${LIBS})
