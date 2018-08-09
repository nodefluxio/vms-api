cmake_minimum_required(VERSION 3.0.0)
project(vmsapi VERSION 0.1.0)

# Set default compile flags for GCC
if(CMAKE_COMPILER_IS_GNUCXX)
    message(STATUS "GCC detected, adding compile flags")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++98 -pedantic -Wall -Wextra")
endif(CMAKE_COMPILER_IS_GNUCXX)

# ----------------------------------------------------------------------------
# Detect dependencies
# ----------------------------------------------------------------------------
option(CMAKE_THREAD_LIBS_INIT "thread library" ON)
option(CMAKE_USE_WIN32_THREADS_INIT "using WIN32 threads" _WIN32)

find_package(Threads)
if(NOT CMAKE_THREAD_LIBS_INIT)
    message(SEND_ERROR "Cannot find a thread library.")
endif()

find_package(Boost)
if(NOT Boost_FOUND)
    message(SEND_ERROR "Cannot find boost library.")
endif()

# ----------------------------------------------------------------------------
# Target definition
# ----------------------------------------------------------------------------

file(GLOB hwivs_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

set(vmsapi_INCLUDE_DIR
  ${CMAKE_SOURCE_DIR}/external/spdlog/include
  ${CMAKE_CURRENT_SOURCE_DIR}/external
  ${CMAKE_CURRENT_SOURCE_DIR}/../base
  ${CMAKE_CURRENT_SOURCE_DIR}/../huawei/include
)

add_executable(vmsapi ${hwivs_SRC})

target_include_directories(vmsapi PRIVATE ${vmsapi_INCLUDE_DIR})

target_link_libraries(vmsapi
    ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_LIBRARIES}
    hwivs
)

target_compile_options(vmsapi PRIVATE -Wall)
set_property(TARGET vmsapi PROPERTY CXX_STANDARD 14)

set(vmsapi_INCLUDE_DIR ${hwivs_INCLUDE_DIR} PARENT_SCOPE)
