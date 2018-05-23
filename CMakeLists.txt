cmake_minimum_required(VERSION 2.8)
project(CuckooSniffer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTINS_STATIC")
set(CMAKE_CXX_LINKER_FLAGS "-static")

set(INCLUDE_DIRS            ${PROJECT_SOURCE_DIR}/src)

set(LIBRARY_OUTPUT_PATH     ${PROJECT_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(LIB_DIRS)

set(LIBS Packet++ Pcap++ Common++)

set(INCLUDE_DIRS    ${INCLUDE_DIRS}
        ${PROJECT_SOURCE_DIR}/deps/pcapplusplus/Packet++/header
        ${PROJECT_SOURCE_DIR}/deps/pcapplusplus/Pcap++/header
        ${PROJECT_SOURCE_DIR}/deps/pcapplusplus/Common++/header
        )

set(LIB_DIRS        ${LIB_DIRS}
        ${PROJECT_SOURCE_DIR}/deps/pcapplusplus/Packet++/Lib
        ${PROJECT_SOURCE_DIR}/deps/pcapplusplus/Pcap++/Lib
        ${PROJECT_SOURCE_DIR}/deps/pcapplusplus/Common++/Lib/Debug
        )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++1z")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

if (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_LOG_DYN_LINK")

    set(INCLUDE_DIRS    ${INCLUDE_DIRS}
            /usr/local/include
            )
    set(LIB_DIRS        ${LIB_DIRS}
            /usr/local/lib
            )

    set(LIBS ${LIBS} ${BOOST_LIBS}
            boost_log_setup
            boost_regex
            boost_log
            boost_thread
            boost_system
            boost_filesystem
            boost_date_time
            boost_program_options
            pcap
            crypto
            curl
            pthread
            )

ELSEIF (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_LOG_DYN_LINK")

    set(INCLUDE_DIRS    ${INCLUDE_DIRS}
            /usr/local/include
            )
    set(LIB_DIRS        ${LIB_DIRS}
            /usr/local/lib
            )

    set(LIBS ${LIBS} ${BOOST_LIBS}
            boost_log_setup-mt
            boost_regex-mt
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
            "-framework Foundation" "-framework SystemConfiguration"

            )
    find_library(CORE_FOUNDATION NAMES CoreFoundation)
    find_library(APPLICATION_SERVICES NAMES ApplicationServices)
    find_library(OPEN_GL NAMES OpenGL)
    find_library(IO_KIT Names IOKit)
    find_library(CORE_SERVICES Names CoreServices)
    find_library(COCOA Names Cocoa)
    find_library(CARBON Names Carbon)
    #    set(CMAKE_SHARED_LINKER_FLAGS  "{CMAKE_SHARED_LINKER_FLAGS} -framework CoreAudio -framework OpenAL -framework CoreFoundation -framework ApplicationServices -framework OpenGL -framework IOKit -framework CoreServices -framework Cocoa -framework Carbon  -lglew -ljpeg -framework sndfile -framework freetype")


ELSEIF (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Windows")
    set(INCLUDE_DIRS    ${INCLUDE_DIRS}
            C:/libs/WpdPack/Include
            #            C:/libs/libtins64-debug/include
            #            C:/libs/boost64
            #            C:/libs/OpenSSL-Win64/include
            #            C:/libs/curl-7.46.0-win64/include
            )
    set(LIB_DIRS        ${LIB_DIRS}
            C:/libs/WpdPack/Lib/x64
            #            C:/libs/libtins64-debug/lib
            #            C:/libs/boost64/lib64-msvc-14.0
            #            C:/libs/OpenSSL-Win64/lib
            #            C:/libs/curl-7.46.0-win64/lib
            )
    set(LIBS ${LIBS}
            libcurl
            libeay32  #libcrypto on windows
            )
endif ()

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
        src/pcpp_capturer.cpp
#        src/sniffer_manager.cpp
        src/base/sniffer.cpp
        src/util/base64.cpp
        src/util/buffer.cpp
        src/util/file.cpp
        src/util/function.cpp
        src/util/mail_process.cpp
        src/util/option_parser.cpp
        #        src/smtp/sniffer.cpp
        #        src/imap/sniffer.cpp
        src/ftp/data_sniffer.cpp
        src/ftp/command_sniffer.cpp
        src/http/sniffer.cpp
        src/http/data.cpp
        src/http/parser.cpp
        src/http/parser/http_parser.c
        src/http/multipart_parser.cpp
        src/samba/sniffer.cpp
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
target_link_libraries(TestHTTP libcuckoo_sniffer ${LIBS})

add_executable(TestPcapplusplus test/test_pcapplusplus.cpp)
target_link_libraries(TestPcapplusplus ${LIBS} )

add_executable(TestMultipart test/test_multipart_data.cpp)
target_link_libraries(TestMultipart libcuckoo_sniffer ${LIBS})


add_executable(ListDevs test/list_devs.cpp)
target_link_libraries(ListDevs libcuckoo_sniffer ${LIBS})
