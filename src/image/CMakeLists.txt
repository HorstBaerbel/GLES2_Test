cmake_minimum_required(VERSION 2.6)

project(Image)

#-------------------------------------------------------------------------------
#finding necessary packages
find_package(FreeImage REQUIRED)

#-------------------------------------------------------------------------------
#set up compiler flags and excutable names
if(DEFINED BCMHOST)
    add_definitions(-DBCMHOST)
endif()

if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP") #multi-processor compilation
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP") #multi-processor compilation
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    #check for G++ 4.7+
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE G++_VERSION)
    if (G++_VERSION VERSION_LESS 4.7)
            message(SEND_ERROR "You need at least G++ 4.7 to compile!")
    endif()
    #set up compiler flags for GCC
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11") #support C++11, debug
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")        #optimize
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")  #strip binary
endif()

#-------------------------------------------------------------------------------
#add include directories
LIST(APPEND IMAGE_INCLUDE_DIRS
    ${FreeImage_INCLUDE_DIR}
)

#-------------------------------------------------------------------------------
#define basic sources and headers
set(IMAGE_LIB_HEADERS
    ../Base.h
    Image.h
    ImageResample.h
    PixelFormat.h
    PixelInfo.h
    ResampleKernel.h
)

set(IMAGE_LIB_SOURCES
    ../Base.cpp
    Image.cpp
    ImageResample.cpp
    PixelFormat.cpp
    ResampleKernel.cpp
)

set(IMAGE_TEST_SOURCES
    ImageTest.cpp
)

#-------------------------------------------------------------------------------
#define libraries and directories
LIST(APPEND IMAGE_LIBRARIES
    ${FreeImage_LIBRARY}
)

#-------------------------------------------------------------------------------
#set up build directories
set(dir ${CMAKE_CURRENT_SOURCE_DIR}/../../build)
set(EXECUTABLE_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)
set(LIBRARY_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)

#-------------------------------------------------------------------------------
#define targets
include_directories(${IMAGE_INCLUDE_DIRS})
add_library(Image ${IMAGE_LIB_SOURCES} ${IMAGE_LIB_HEADERS})
target_link_libraries(Image ${IMAGE_LIBRARIES})
add_executable(ImageTest ${IMAGE_TEST_SOURCES})
target_link_libraries(ImageTest Image)
