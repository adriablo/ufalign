# Minimum cmake version
# cmake_minimum_required (VERSION 3.0.0)
cmake_minimum_required (VERSION 2.8.0)

# ----------------------------------------------------------------------------
# read target properties utility
# https://stackoverflow.com/questions/46303348/print-complete-cflags-cxxflags-of-cmake-project
# ----------------------------------------------------------------------------

function(print_target_properties tgt)
    if(NOT TARGET ${tgt})
        message("There is no target named '${tgt}'")
        return()
    endif()

    # this list of properties can be extended as needed
    set(CMAKE_PROPERTY_LIST SOURCE_DIR BINARY_DIR COMPILE_DEFINITIONS
             COMPILE_OPTIONS INCLUDE_DIRECTORIES LINK_LIBRARIES)

    message("Configuration for target ${tgt}")

    foreach (prop ${CMAKE_PROPERTY_LIST})
        get_property(propval TARGET ${tgt} PROPERTY ${prop} SET)
        if (propval)
            get_target_property(propval ${tgt} ${prop})
            message (STATUS "${prop} = ${propval}")
        endif()
    endforeach(prop)

endfunction(print_target_properties)


# ----------------------------------------------------------------------------
# Project
# ----------------------------------------------------------------------------

# Name of project and that it is C++ only.
project (aligner CXX)

set(SRC_ROOT ~/source)
set(CMAKE_PREFIX_PATH ${SRC_ROOT}/seqan/util/cmake)
set(SEQAN_INCLUDE_PATH ${SRC_ROOT}/seqan/include)


# ----------------------------------------------------------------------------
# Dependencies
# ----------------------------------------------------------------------------

# Search for zlib as a dependency for SeqAn.
# find_package (ZLIB)

# Load the SeqAn module and fail if not found.
find_package (SeqAn REQUIRED)

# find_package (ZLIB REQUIRED)
# find_package (OpenMP REQUIRED)

find_package (Threads)


# ----------------------------------------------------------------------------
# Build Setup
# ----------------------------------------------------------------------------

# Add include directories.
include_directories (${SEQAN_INCLUDE_DIRS})

# Add definitions set by find_package (SeqAn).
add_definitions (${SEQAN_DEFINITIONS})

# Add CXX flags found by find_package (SeqAn).
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SEQAN_CXX_FLAGS}")

# set (CMAKE_EXE_LINKER_FLAGS "-pthread")
# set (CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static -pthread")

# Add executable and link against SeqAn dependencies.
add_executable (aligner aligner.cpp  cl_opt.cpp  handler.cpp  ../common/logger.cpp ../common/print_algn.cpp)
target_include_directories(aligner PUBLIC ${SRC_ROOT}/edlib/edlib/include ${SRC_ROOT}/spdlog/include ${SRC_ROOT}/shared-samples/ufalign/common)
target_link_libraries(aligner  ${CMAKE_THREAD_LIBS_INIT}  ${SEQAN_LIBRARIES}  ${SRC_ROOT}/edlib/build/lib/libedlib_static.a)


# ----------------------------------------------------------------------------
# Custom Build Types
# ----------------------------------------------------------------------------

message("Available BUILD TYPEs: debug, release, profile")
message(STATUS "CMAKE_BUILD_TYPE = " ${CMAKE_BUILD_TYPE})

if (${CMAKE_BUILD_TYPE} STREQUAL "debug")
    message(STATUS "==> BUILD: DEBUG")
    set (SEQAN_ENABLE_DEBUG 1)
    target_compile_options(aligner PUBLIC -g -ggdb)

elseif (${CMAKE_BUILD_TYPE} STREQUAL "release")
    message(STATUS "==> BUILD: RELEASE")
    set (SEQAN_ENABLE_DEBUG 0)
    set (NDEBUG 1)
    target_compile_options(aligner PUBLIC -O3 -DNDEBUG)

elseif (${CMAKE_BUILD_TYPE} STREQUAL "profile")
    message(STATUS "==> BUILD: PROFILE")
    # FOR PROFILING - include symbols & disable template inlining - same as DEBUG options
    target_compile_options(aligner PUBLIC -g -ggdb)

else ()
    message(FATAL_ERROR "==> BUILD: UNKNOWN")

endif ()

message(STATUS "SEQAN_ENABLE_DEBUG = " ${SEQAN_ENABLE_DEBUG})
message(STATUS "NDEBUG = " ${NDEBUG})

print_target_properties("aligner")
