cmake_minimum_required(VERSION 3.0.0)
project(flussonic VERSION 0.1.0)

add_subdirectory(external/cpr)

# ----------------------------------------------------------------------------
# Target definition
# ----------------------------------------------------------------------------

file(GLOB flussonic_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

set(flussonic_INCLUDE_DIR
    ${CPR_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/external/spdlog/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../base
)

add_library(flussonic ${flussonic_SRC})

target_include_directories(flussonic PRIVATE ${flussonic_INCLUDE_DIR})

target_link_libraries(flussonic 
    ${CPR_LIBRARIES}  
)

target_compile_options(flussonic PRIVATE -Wall -Wextra -Wunused-parameter)
set_property(TARGET flussonic PROPERTY CXX_STANDARD 14)
