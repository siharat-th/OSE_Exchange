# CMakeList.txt : CMake project for MDHandler, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_COMPILER_LAUNCHER ccache)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable Hot Reload for MSVC compilers if supported.
# if (POLICY CMP0141)
#   cmake_policy(SET CMP0141 NEW)
#   set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
# endif()

project ("ExchangeHandler")

# ============ BUILD SPEED OPTIMIZATION ============

# Enable ccache if available (massive speedup for incremental builds)
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    message(STATUS "Using ccache: ${CCACHE_PROGRAM}")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb3")
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -flto")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}  -O2 -march=x86-64-v3 -w -flto")

# # Check the build type and define the _DEBUG macro if it's a debug build
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-D_DEBUG)
endif()

# add include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/src
)

include_directories(
    "/app/dev/onload/src/include"
    "/app/dev/tcpdirect/src/include"
    ${CMAKE_SOURCE_DIR}/deps/omnet/include
    "/usr/local/mysql/connector-c++-9.1.0/include/jdbc"
    "/app/dev/mysql-connector-cpp/build/include/jdb"
    "/app/dev/mysql-connector-cpp/jdbc"
)

link_directories(
    "/app/dev/onload/build/gnu_x86_64/lib/ciul"
    "/app/dev/onload/build/gnu_x86_64/lib/onload_ext"
    "/app/dev/tcpdirect/build/gnu_x86_64/lib"
    ${CMAKE_SOURCE_DIR}/deps/omnet/lib
    "/usr/lib64/mysql"
    "/app/dev/mysql-connector-cpp/build/lib"
)

if (${AKL_TEST_MODE})
  message(STATUS "AKL_TEST_MODE is set to ${AKL_TEST_MODE} so set predefined processor variable.")
  add_compile_definitions(AKL_TEST_MODE=1)
endif()

# 🔹 Collect all source files recursively and store them in SOURCE
file(GLOB_RECURSE SOURCES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/**/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.cc"
    "${CMAKE_SOURCE_DIR}/src/*.c"
)


#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/build/dev-linux-release)
# Dynamically set output dir based on build type
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/build/${CMAKE_BUILD_TYPE})
# if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#     set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/build/dev-linux-debug)
# elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
#     set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/build/dev-linux-release)
# else()
#     set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/build/${CMAKE_BUILD_TYPE})
# endif()

set(LIBS
	stdc++
	crypto
	z
	ciul
	onload_ext
	onload_zf
	m
	curl
	tbb
	ssl
	dl
	rt
	pthread
	zmq
	cryptopp
	readline
	mysqlclient
)

# 🔹 Create executable
if (${AKL_TEST_MODE})
	list(FILTER SOURCES EXCLUDE REGEX ".*Main\\.cpp$")
	add_library(occamlib STATIC ${SOURCES})
	set(LIBS ${LIBS} occamlib)
	add_executable(ExchangeHandler ${CMAKE_SOURCE_DIR}/src/Main.cpp)
else()
	add_executable(ExchangeHandler ${SOURCES})
endif()

# # Option 1: Set default precision with option for override: 9 is CME, 0 is CFE
# option(PRICE_PRECISION "Decimal places for Price type" 0)

# # Pass the PRICE_PRECISION to the compiler as a preprocessor definition
# target_compile_definitions(ExchangeHandler PRIVATE PRICE_PRECISION=${PRICE_PRECISION})

# Add the link libraries
target_link_libraries(ExchangeHandler PRIVATE ${LIBS})
target_link_libraries(ExchangeHandler PRIVATE ${CMAKE_SOURCE_DIR}/deps/lime/lib/limeTradingApi.a)
target_link_libraries(ExchangeHandler PRIVATE ${CMAKE_SOURCE_DIR}/deps/omnet/lib/liboapi.a)
# OMnet API depends on OpenSSL 1.1 + zlib (bundled in deps/omnet/lib)
target_link_libraries(ExchangeHandler PRIVATE
    ${CMAKE_SOURCE_DIR}/deps/omnet/lib/libssl.so
    ${CMAKE_SOURCE_DIR}/deps/omnet/lib/libcrypto.so
    ${CMAKE_SOURCE_DIR}/deps/omnet/lib/libz.a
)
# Set RPATH so binary finds bundled OpenSSL at runtime
set_target_properties(ExchangeHandler PROPERTIES
    BUILD_RPATH "${CMAKE_SOURCE_DIR}/deps/omnet/lib"
    INSTALL_RPATH "$ORIGIN/deps/omnet/lib"
)

add_subdirectory(test)

message(STATUS "CMAKE_CXX_FLAGS is: \"${CMAKE_CXX_FLAGS}\"")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG is: \"${CMAKE_CXX_FLAGS_DEBUG}\"")
message(STATUS "CMAKE_CXX_FLAGS_RELEASE is: \"${CMAKE_CXX_FLAGS_RELEASE}\"")
