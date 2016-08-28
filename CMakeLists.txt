cmake_minimum_required(VERSION 3.6)
project(OpenGL_template)

# 32bit or 64bit compiler
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    MESSAGE(WARNING "64 bits compiler detected")
    SET(EX_PLATFORM 64)
    SET(EX_PLATFORM_NAME "x64")
else (CMAKE_SIZEOF_VOID_P EQUAL 8)
    MESSAGE(WARNING "32 bits compiler detected")
    SET(EX_PLATFORM 32)
    SET(EX_PLATFORM_NAME "x86")
endif (CMAKE_SIZEOF_VOID_P EQUAL 8)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -DGL_GLEXT_PROTOTYPES")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -O2")

if (APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGLFW_INCLUDE_GLCOREARB")
endif (APPLE)

## Define if in debug mode or not (comment out line below to disable logging -> faster)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMY_DEBUG")

## CMake stuff
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(PROJECT_BINARY_DIR ${PROJECT_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
set(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build)

## Set paths
set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(PROJECT_CPP_DIR ${PROJECT_SOURCE_DIR}/src)
set(PROJECT_EXT_DIR ${PROJECT_SOURCE_DIR}/external)


if (CMAKE_BUILD_TYPE EQUAL "DEBUG")
message(WARNING "Debug mode")
elseif (CMAKE_BUILD_TYPE EQUAL "DEBUG")
message(WARNING "Release mode")
endif (CMAKE_BUILD_TYPE EQUAL "DEBUG")

##########################
### External libraries ###
##########################

### OpenGL ###
find_package(OpenGL REQUIRED)
set(ALL_LIBRARIES ${ALL_LIBRARIES} ${OPENGL_gl_LIBRARY})

### GLFW ###
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw-3.2.1)
set(ALL_LIBRARIES ${ALL_LIBRARIES} glfw)


### GLM ###
set(EXTERNAL_INCLUDE_DIRS ${EXTERNAL_INCLUDE_DIRS} ${PROJECT_EXT_DIR}/glm)

### GLEW (on Windows) ###
if (MINGW)
set(GLEW_DIR ${PROJECT_EXT_DIR}/glew)

set(GLEW_INCLUDE_DIR ${GLEW_DIR}/include)
#set(GLEW_LIBRARIES ${GLEW_DIR}/lib/Release/x64/glew32s.lib)
#set(GLEW_LIBRARIES "C:/Windows/System32/glew32.dll")
set(GLEW_LIBRARIES ${GLEW_DIR}/bin/Release/x64/glew32.dll)

set(EXTERNAL_INCLUDE_DIRS ${EXTERNAL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIR})
set(ALL_LIBRARIES ${ALL_LIBRARIES} ${GLEW_LIBRARIES})
endif (MINGW)

# Lump all external- and project includes into one variable
set(ALL_INCLUDES ${EXTERNAL_INCLUDE_DIRS} ${PROJECT_INCLUDE_DIR})

# Set the include directories so that #include statements work
include_directories(${ALL_INCLUDES})
message(WARNING "All include dirs: ${ALL_INCLUDES}")

# Get all source files by traversing the source directory recursively
file(GLOB_RECURSE PROJECT_CPP_FILES ${PROJECT_CPP_DIR}/*.cpp)

set(SOURCE_FILES main.cpp ${PROJECT_CPP_FILES})
add_executable(OpenGL_template ${SOURCE_FILES})

target_link_libraries(OpenGL_template ${ALL_LIBRARIES})
message(WARNING "All libraries: ${ALL_LIBRARIES}")

