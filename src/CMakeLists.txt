cmake_minimum_required(VERSION 2.6)

project(GLES2_test)

#-------------------------------------------------------------------------------
#set up build directories
set(dir ${CMAKE_CURRENT_SOURCE_DIR}/../build)
set(EXECUTABLE_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)
set(LIBRARY_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)

#-------------------------------------------------------------------------------
#add local find scripts to CMAKE path
LIST(APPEND CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Utils
    ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Packages
)

#-------------------------------------------------------------------------------
#add our custom image library
add_subdirectory(image)

#-------------------------------------------------------------------------------
#set up OpenGL system variable
set(GLSystem "Desktop OpenGL" CACHE STRING "The OpenGL system to be used")
set_property(CACHE GLSystem PROPERTY STRINGS "Desktop OpenGL" "OpenGL ES 2.0")
#set up OpenGL window sytem variable
set(GLWindowSystem "" CACHE STRING "The OpenGL window system to be used")
set_property(CACHE GLWindowSystem PROPERTY STRINGS "WGL" "EGL" "GLX")

#-------------------------------------------------------------------------------
#check if we're running on Raspberry Pi
MESSAGE("Looking for bcm_host.h")
if(EXISTS "/opt/vc/include/bcm_host.h")
    MESSAGE("bcm_host.h found")
    set(BCMHOST found)
else()
    MESSAGE("bcm_host.h not found")
endif()

#-------------------------------------------------------------------------------
#Set OpenGL variables and compiler flags depending on system
if(DEFINED BCMHOST)
    set(GLSystem "OpenGL ES 2.0")
    set(GLWindowSystem "EGL")
    add_definitions(-DUSE_OPENGL_EGL)
    add_definitions(-DUSE_OPENGL_ES)
else()
    if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        set(GLSystem "Desktop OpenGL")
        set(GLWindowSystem "WGL")
        add_definitions(-DUSE_OPENGL_WGL)
        add_definitions(-DUSE_OPENGL_DESKTOP)
    endif()
    if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        set(GLSystem "Desktop OpenGL")
        set(GLWindowSystem "GLX")
        add_definitions(-DUSE_OPENGL_GLX)
        add_definitions(-DUSE_OPENGL_DESKTOP)
    endif()
endif()

#-------------------------------------------------------------------------------
#finding necessary packages
if(${GLSystem} MATCHES "Desktop OpenGL")
    find_package(OpenGL REQUIRED)
else()
    find_package(OpenGLES2 REQUIRED)
endif()
find_package(Eigen3 REQUIRED)

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
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g3") #support C++11, debug
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")        #optimize
    #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")  #strip binary
endif()

#-------------------------------------------------------------------------------
#add include directories
if(DEFINED BCMHOST)
    LIST(APPEND EXTRA_INCLUDE_DIRS
        "/opt/vc/include"
        "/opt/vc/include/interface/vcos"
        "/opt/vc/include/interface/vmcs_host/linux"
        "/opt/vc/include/interface/vcos/pthreads"
    )
else()
    if(${GLSystem} MATCHES "Desktop OpenGL")
        LIST(APPEND EXTRA_INCLUDE_DIRS
            ${OPENGL_INCLUDE_DIR}
        )
    else()
        LIST(APPEND EXTRA_INCLUDE_DIRS
            ${OPENGLES2_INCLUDE_DIR}
        )
    endif()
endif()

LIST(APPEND EXTRA_INCLUDE_DIRS
    ${EIGEN3_INCLUDE_DIR}
)

#-------------------------------------------------------------------------------
#define basic sources and headers
set(TARGET_HEADERS
    Base.h
    Timing.h
    components/Box2D.h
    components/Group2D.h
    components/Object2D.h
    gl/GLBase.h
    gl/ContextBase.h
    gl/GLFramebuffer.h
    gl/GLIncludes.h
    gl/GLTexture2D.h
    gl/GLVertexAttribute.h
    gl/GLVertexBase.h
    gl/GLVertexBuffer.h
    gl/WindowBase.h
    math/DataProxy.h
    math/Math.h
    math/half.h
    math/half2.h
    math/half3.h
    math/half4.h
    math/Transform2.h
#	model/Material.h
#	model/Mesh.h
#	model/Model.h
#	model/ModelObj.h
)
set(TARGET_SOURCES
    Base.cpp
    Timing.cpp
    components/Box2D.cpp
    components/Group2D.cpp
    components/Object2D.cpp
    gl/GLBase.cpp
    gl/ContextBase.cpp
    gl/GLFramebuffer.cpp
    gl/GLTexture2D.cpp
    gl/GLVertexBase.cpp
    gl/GLVertexBuffer.cpp
    gl/WindowBase.cpp
    math/Math.cpp
    math/half.cpp
    math/Transform2.cpp
#	model/Material.cpp
#	model/Mesh.cpp
#	model/ModelObj.cpp
)

#-------------------------------------------------------------------------------
#define OS specific sources and headers
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    LIST(APPEND TARGET_SOURCES
        main_windows.cpp
    )
endif()
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    LIST(APPEND TARGET_SOURCES
        main_linux.cpp
    )
endif()

#add open gl specific sources
if(${GLSystem} MATCHES "Desktop OpenGL")
    LIST(APPEND TARGET_HEADERS
        gl/GLContext.h
        gl/GLWindow.h
    )
    LIST(APPEND TARGET_SOURCES
        gl/GLContext.cpp
        gl/GLWindow.cpp
    )
else()
    LIST(APPEND TARGET_HEADERS
        gl/ESContext.h
        gl/ESWindow.h
    )
    LIST(APPEND TARGET_SOURCES
        gl/ESContext.cpp
        gl/ESWindow.cpp
    )
endif()

#-------------------------------------------------------------------------------
#define groups for source (mostly for VS)
source_group("gl" REGULAR_EXPRESSION gl\\/.*)
source_group("components" REGULAR_EXPRESSION components\\/.*)
source_group("math" REGULAR_EXPRESSION math\\/.*)
source_group("image" REGULAR_EXPRESSION image\\/.*)
source_group("model" REGULAR_EXPRESSION model\\/.*)
source_group("main" REGULAR_EXPRESSION ^[^\\/]*$) #dunno why this doesn't work

#-------------------------------------------------------------------------------
#define libraries and directories
if(DEFINED BCMHOST)
    LIST(APPEND TARGET_LIBRARIES
        /opt/vc/lib/libbcm_host.so
        /opt/vc/lib/libGLESv2.so
        /opt/vc/lib/libEGL.so
    )
else()
    if(MSVC)
        LIST(APPEND TARGET_LIBRARIES
            winmm
        )
    else()
        LIST(APPEND TARGET_LIBRARIES
            dl
            Xxf86vm
        )	
    endif()
    if(${GLSystem} MATCHES "Desktop OpenGL")
        LIST(APPEND TARGET_LIBRARIES
            ${OPENGL_LIBRARIES}
        )
    else()
        LIST(APPEND TARGET_LIBRARIES
            ${OPENGLES2_LIBRARIES}
        )
    endif()
endif()

LIST(APPEND TARGET_LIBRARIES
    Image
)

#-------------------------------------------------------------------------------
#define target
include_directories(${EXTRA_INCLUDE_DIRS})
add_executable(GLES2_test ${TARGET_SOURCES} ${TARGET_HEADERS})
target_link_libraries(GLES2_test ${TARGET_LIBRARIES})

#special properties for windows builds
if(MSVC)
    #show console in debug builds, but not in proper release builds
    #Note that up to CMake 2.8.11.2 this feature is broken: http://public.kitware.com/Bug/view.php?id=12566
    set_target_properties(GLES2_test PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
    set_target_properties(GLES2_test PROPERTIES COMPILE_DEFINITIONS_DEBUG "_CONSOLE")
    set_target_properties(GLES2_test PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
    set_target_properties(GLES2_test PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "_CONSOLE")
    set_target_properties(GLES2_test PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
    set_target_properties(GLES2_test PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS")
endif()
