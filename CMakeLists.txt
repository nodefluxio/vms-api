# ----------------------------------------------------------------------------
#  Root CMake file for vms
#
#    From the off-tree build directory, invoke:
#      $ cmake <PATH_TO_VMS_ROOT>
#
# ----------------------------------------------------------------------------

# Disable in-source builds to prevent source tree corruption.
if(" ${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "
FATAL: In-source builds are not allowed.
       You should create a separate directory for build files.
")
endif()

cmake_minimum_required(VERSION 3.0.0)
project(vms VERSION 0.1.0)

# ----------------------------------------------------------------------------
#  Build & install layouts
# ----------------------------------------------------------------------------

set(EXECUTABLE_OUTPUT_PATH    "${CMAKE_BINARY_DIR}/bin" CACHE PATH "Output directory for applications")
set(LIBRARY_OUTPUT_PATH       "${CMAKE_BINARY_DIR}/lib" CACHE PATH "Output directory for libraries")

# In case of Makefiles if the user does not setup CMAKE_BUILD_TYPE, assume it's Release:
if(CMAKE_GENERATOR MATCHES "Makefiles|Ninja" AND "${CMAKE_BUILD_TYPE}" STREQUAL "")
  set(CMAKE_BUILD_TYPE Release)
endif()

# ----------------------------------------------------------------------------
# Add modules
# ----------------------------------------------------------------------------

add_subdirectory(modules/huawei)
add_subdirectory(modules/api)
