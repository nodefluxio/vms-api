cmake_minimum_required(VERSION 3.0.0)
project(vmsapi VERSION 0.1.0)

# ----------------------------------------------------------------------------
# Detect dependencies
# ----------------------------------------------------------------------------
# THREADS
find_package(Threads)

if(WIN32) 
  # Even FindThreads.cmake found, CMAKE_THREAD_LIBS_INIT still not initialized in win
  option(CMAKE_THREAD_LIBS_INIT "thread library" ON)
  option(CMAKE_USE_WIN32_THREADS_INIT "using WIN32 threads" ON)
endif()

if(NOT CMAKE_THREAD_LIBS_INIT)
    message(SEND_ERROR "Cannot find a thread library.")
endif()

# BOOST
if(WIN32)
  find_package(Boost)
else()
  find_package(Boost COMPONENTS system)
endif()

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
  $ENV{IVS_SDK_PATH}/inc
)

add_executable(vmsapi ${hwivs_SRC})

target_include_directories(vmsapi PRIVATE ${vmsapi_INCLUDE_DIR})

target_link_libraries(vmsapi
    ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_LIBRARIES}
    hwivs
)

if(CMAKE_COMPILER_IS_GNUCXX)
  target_compile_options(vmsapi PRIVATE -Wall -Wextra -Wunused-parameter)
else() # CMake Microsoft Compiler (cl)
  target_compile_options(vmsapi PRIVATE -Wall)
endif()

set_property(TARGET vmsapi PROPERTY CXX_STANDARD 14)

set(vmsapi_INCLUDE_DIR ${hwivs_INCLUDE_DIR} PARENT_SCOPE)
