cmake_minimum_required(VERSION 3.0.0)
project(hwivs VERSION 0.1.0)

# ----------------------------------------------------------------------------
# Detect dependencies
# ----------------------------------------------------------------------------
find_library(IVS_SDK_LIBRARY NAMES IVS_SDK HINTS $ENV{IVS_SDK_PATH}/sdklib)
if(NOT IVS_SDK_LIBRARY)
    message(SEND_ERROR "Cannot find IVS SDK. Try to point it in IVS_SDK_PATH environment variable.")
endif()

# ----------------------------------------------------------------------------
# Target definition
# ----------------------------------------------------------------------------

file(GLOB hwivs_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

set(hwivs_INCLUDE_DIR
    ${CMAKE_SOURCE_DIR}/external/spdlog/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../base
    $ENV{IVS_SDK_PATH}/inc
)

add_library(hwivs ${hwivs_SRC})

target_include_directories(hwivs PRIVATE ${hwivs_INCLUDE_DIR})

target_link_libraries(hwivs
    ${IVS_SDK_LIBRARY}
)

target_compile_options(hwivs PRIVATE -Wall)
set_property(TARGET hwivs PROPERTY CXX_STANDARD 14)

set(hwivs_INCLUDE_DIR ${hwivs_INCLUDE_DIR} PARENT_SCOPE)
