CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

project(voxelizer)

set(BASEPATH "${CMAKE_SOURCE_DIR}")
set(TCLAP_INCLUDE "./vendor/tclap-1.2.1/include")
FILE(GLOB SOURCES "*.cu" "*.cpp" "*.c" "*.h")

find_package(CUDA REQUIRED)

set(CMAKE_MODULE_PATH
  "${CMAKE_SOURCE_DIR}/CMake"
  ${CMAKE_MODULE_PATH}
)

# Put all the runtime stuff in the same directory.  By default, CMake puts each targets'
# output into their own directory.  We want all the targets to be put in the same
# directory, and we can do this by setting these variables.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# set up include directories
include_directories(/usr/local/cuda/include )
include_directories("${BASEPATH}")
include_directories(BEFORE "${TCLAP_INCLUDE}")

# add executable
CUDA_ADD_EXECUTABLE(voxelizer ${SOURCES})

# set compiler and NVCC flags
list(APPEND CMAKE_CXX_FLAGS "-std=c++0x -std=c++11 -O3 -ffast-math -Wall")
list(APPEND CUDA_NVCC_FLAGS --compiler-options -fno-strict-aliasing -lineinfo -use_fast_math -Xptxas -dlcm=cg)
list(APPEND CUDA_NVCC_FLAGS -gencode arch=compute_20,code=sm_20)
list(APPEND CUDA_NVCC_FLAGS -gencode arch=compute_30,code=sm_30)
list(APPEND CUDA_NVCC_FLAGS -gencode arch=compute_35,code=sm_35)

target_link_libraries(voxelizer)