# CMakeLists.txt -- CMake Makefile for WtCrypto
# Copyright (C) 2018 Farid Hajji <farid@hajji.name>

# ISC License
# 
# Copyright 2018 Farid Hajji <farid@hajji.name>
# 
# Permission to use, copy, modify, and/or distribute this software
# for any purpose with or without fee is hereby granted, provided
# that the above copyright notice and this permission notice appear
# in all copies.
# 
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
# WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS.IN NO EVENT SHALL THE
# AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
# DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA
# OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
# TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.

cmake_minimum_required (VERSION 3.5)
project (wtcrypto)

set (WT_INCLUDE_DIR "/usr/local/include")
set (WT_LIB_DIR "/usr/local/lib")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package (Threads)
set (THREADS_PREFER_PTHREAD_FLAG TRUE)

find_package(OpenSSL REQUIRED)

# set (BOOST_ROOT "/usr/local/boost_1_66_0")
# set (Boost_NO_SYSTEM_PATHS ON)
find_package (Boost REQUIRED COMPONENTS system serialization)

if (Boost_FOUND AND OPENSSL_FOUND)
    include_directories (${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR} ${WT_INCLUDE_DIR})
    add_executable (wtcrypto.wt
		encdecapplication.cpp hexdumpmodel.cpp
		main.cpp) 

    add_library (wt SHARED IMPORTED)
    set_target_properties (wt PROPERTIES
                           IMPORTED_LOCATION "${WT_LIB_DIR}/libwt.so")

    add_library (wthttp SHARED IMPORTED)
    set_target_properties (wthttp PROPERTIES
                           IMPORTED_LOCATION "${WT_LIB_DIR}/libwthttp.so")

    target_link_libraries (wtcrypto.wt PRIVATE 
            wt wthttp 
            OpenSSL::SSL OpenSSL::Crypto
            ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
endif()

if (WIN32)
    # disable autolinking in boost
    add_definitions( -DBOOST_ALL_NO_LIB )

    # force all boost libraries to dynamic link (we already disabled
    # autolinking, so I don't know why we need this, but we do!)
    add_definitions( -DBOOST_ALL_DYN_LINK )
endif()

# add_library (boost_system SHARED IMPORTED)
# set_target_properties (boost_system PROPERTIES
#                        IMPORTED_LOCATION "/usr/local/lib/libboost_system.so")

include(CheckCXXCompilerFlag)

function(enable_cxx_compiler_flag_if_supported flag)
    string(FIND "${CMAKE_CXX_FLAGS}" "${flag}" flag_already_set)
        if(flag_already_set EQUAL -1)
	    check_cxx_compiler_flag("${flag}" flag_supported)
            if(flag_supported)
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
            endif()
            unset(flag_supported CACHE)
        endif()
endfunction()

# example usage
enable_cxx_compiler_flag_if_supported("-Wall")
enable_cxx_compiler_flag_if_supported("-Wextra")
enable_cxx_compiler_flag_if_supported("-pedantic")

# if using vcpkg's integration, try this (not tested, recommanded by wt port):
# find_package(Wt REQUIRED)
# target_link_libraries(wtcrypto.wt PRIVATE Wt::Dbo Wt::DboMSSQLServer Wt::DboPostgres Wt::DboSqlite3)
