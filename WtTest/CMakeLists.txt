cmake_minimum_required (VERSION 3.5)
project (wttest)

set (WT_INCLUDE_DIR "/usr/local/include")
set (WT_LIB_DIR "/usr/local/lib")

# https://stackoverflow.com/questions/42834844/how-to-get-cmake-to-pass-either-std-c14-c1y-or-c17-c1z-based-on-gcc-vers
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# https://github.com/Kitware/CMake/blob/v3.1.0/Modules/FindThreads.cmake#L207
# https://stackoverflow.com/questions/1620918/cmake-and-libpthread
find_package (Threads)
set (THREADS_PREFER_PTHREAD_FLAG TRUE)

find_package(OpenSSL REQUIRED)

# https://github.com/Kitware/CMake/blob/master/Modules/FindBoost.cmake
# set (BOOST_ROOT "/usr/home/farid/Devel/boost_1_66_0")
# set (Boost_NO_SYSTEM_PATHS ON)

# https://stackoverflow.com/questions/6646405/how-do-you-add-boost-libraries-in-cmakelists-txt
find_package (Boost REQUIRED COMPONENTS system serialization)

if (Boost_FOUND AND OPENSSL_FOUND)
    include_directories (${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR} ${WT_INCLUDE_DIR})
    add_executable (wttest.wt
		encdecapplication.cpp hexdumpmodel.cpp
		main.cpp) 
    add_library (wt SHARED IMPORTED)
    set_target_properties (wt PROPERTIES
                           IMPORTED_LOCATION "${WT_LIB_DIR}/libwt.so")
    add_library (wthttp SHARED IMPORTED)
    set_target_properties (wthttp PROPERTIES
                           IMPORTED_LOCATION "${WT_LIB_DIR}/libwthttp.so")
    target_link_libraries (wttest.wt PRIVATE 
            wt wthttp 
            OpenSSL::SSL OpenSSL::Crypto
            ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
endif()

# https://stackoverflow.com/questions/28887680/linking-boost-library-with-boost-use-static-lib-off-on-windows
if (WIN32)
    # disable autolinking in boost
    add_definitions( -DBOOST_ALL_NO_LIB )

    # force all boost libraries to dynamic link (we already disabled
    # autolinking, so I don't know why we need this, but we do!)
    add_definitions( -DBOOST_ALL_DYN_LINK )
endif()

# https://stackoverflow.com/questions/28597351/how-do-i-add-a-library-path-in-cmake
# add_library (boost_system SHARED IMPORTED)
# set_target_properties (boost_system PROPERTIES
#                        IMPORTED_LOCATION "/usr/local/lib/libboost_system.so")


# https://stackoverflow.com/questions/33222569/best-way-to-portably-set-compiler-options-like-wall-and-pedantic-in-cmake
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
