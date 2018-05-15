cmake_minimum_required(VERSION 2.8)
project(CuckooSniffer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTINS_STATIC")
set(CMAKE_CXX_LINKER_FLAGS "-static")

set(INCLUDE_DIRS            ${PROJECT_SOURCE_DIR}/src)

set(LIBRARY_OUTPUT_PATH     ${PROJECT_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(LIB_DIRS)

set(LIBS tins)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++1z")

if (WIN32)

    set(INCLUDE_DIRS    ${INCLUDE_DIRS}
            C:/libs/WpdPack/Include
            C:/libs/libtins64-debug/include
#            C:/libs/boost64
#            C:/libs/OpenSSL-Win64/include
#            C:/libs/curl-7.46.0-win64/include
            )
    set(LIB_DIRS        ${LIB_DIRS}
            C:/libs/WpdPack/Lib/x64
            C:/libs/libtins64-debug/lib
#            C:/libs/boost64/lib64-msvc-14.0
#            C:/libs/OpenSSL-Win64/lib
#            C:/libs/curl-7.46.0-win64/lib
            )

    set(LIBS ${LIBS}
            wpcap
            Ws2_32
            Iphlpapi
            libcurl
            libeay32  #libcrypto on windows
            )

elseif (UNIX)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_LOG_DYN_LINK")

    set(INCLUDE_DIRS    ${INCLUDE_DIRS}
            /usr/local/include
            $ENV{TINS_INCLUDE}
            )
    set(LIB_DIRS        ${LIB_DIRS}
            /usr/local/lib
            $ENV{TINS_LIB}
            )

    set(LIBS ${LIBS} ${BOOST_LIBS}
        boost_log_setup-mt
        boost_log-mt
        boost_thread-mt
        boost_system-mt
        boost_filesystem-mt
        boost_date_time-mt
        boost_program_options-mt
        pcap
        crypto
        curl
        pthread
    )

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")

endif()

find_package(Boost REQUIRED COMPONENTS context coroutine)
find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)


link_directories(${Boost_LIBRARY_DIRS})
set(${LIBS} ${LIBS} ${CURL_LIBRARIES} ${OPENSSL_LIBRARIES})

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CURL_INCLUDE_DIRS})
include_directories(${OPENSSL_INCLUDE_DIR})


include_directories(${INCLUDE_DIRS})
link_directories(${LIB_DIRS})


set(SRC_FILES
        src/cuckoo_sniffer.cpp
        src/sniffer_manager.cpp
        src/base/sniffer.cpp
        src/util/base64.cpp
        src/util/buffer.cpp
        src/util/file.cpp
        src/util/function.cpp
        src/util/mail_process.cpp
        src/util/option_parser.cpp
#        src/smtp/sniffer.cpp
#        src/imap/sniffer.cpp
#        src/ftp/data_sniffer.cpp
#        src/ftp/command_sniffer.cpp
        src/http/sniffer.cpp
        src/http/data.cpp
        src/http/parser.cpp
        src/http/parser/http_parser.c
        src/http/multipart_parser.cpp
#        src/samba/sniffer.cpp
        src/threads/thread.cpp
        src/threads/data_queue.cpp
        src/mime/Mime.cpp
        src/mime/MimeChar.cpp
        src/mime/MimeCode.cpp
        src/mime/MimeType.cpp
        )

add_library(libcuckoo_sniffer STATIC ${SRC_FILES})

add_executable(CuckooSniffer src/main.cpp)
target_link_libraries(CuckooSniffer libcuckoo_sniffer ${LIBS})

add_executable(Test test/test.cpp)
target_link_libraries(Test libcuckoo_sniffer ${LIBS})

add_executable(TestHTTP test/test_http.cpp)
target_link_libraries(TestHTTP libcuckoo_sniffer ${LIBS} boost_regex-mt)

add_executable(TestMultipart test/test_multipart_data.cpp)
target_link_libraries(TestMultipart libcuckoo_sniffer ${LIBS})


add_executable(ListDevs src/list_devs.cpp)
target_link_libraries(ListDevs libcuckoo_sniffer ${LIBS})
