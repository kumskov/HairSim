cmake_minimum_required (VERSION 2.6)

project (superbible7)

INCLUDE_DIRECTORIES(/usr/X11/include)
LINK_DIRECTORIES(/usr/X11/lib)

LINK_DIRECTORIES( ${CMAKE_SOURCE_DIR}/lib )

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_CXX_STANDARD 11) # C++11...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11

# Second, for multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/bin )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/lib )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/lib )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

find_package(OpenGL)
find_package(glfw3)
set (CMAKE_DEBUG_POSTFIX "_d")

if(WIN32)
set(COMMON_LIBS sb7 optimized glfw3 debug glfw3_d ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES})
elseif (UNIX)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)
set(COMMON_LIBS sb7 glfw X11 Xrandr Xinerama Xi Xxf86vm Xcursor GL dl)
else()
set(COMMON_LIBS sb7)
endif()

IF(APPLE)
   FIND_LIBRARY(COCOA_LIBRARY Cocoa)
   FIND_LIBRARY(OPENGL_LIBRARY OpenGL)
   FIND_LIBRARY(IOKIT_LIBRARY IOKit)
   FIND_LIBRARY(COREVID_LIBRARY CoreVideo)
   SET(EXTRA_LIBS ${COCOA_LIBRARY} ${COREVID_LIBRARY} ${IOKIT_LIBRARY} ${OPENGL_LIBRARY})
ENDIF (APPLE)

set(COMMON_LIBS ${COMMON_LIBS} ${EXTRA_LIBS})

add_library(sb7
            src/sb7/sb7.cpp
            src/sb7/sb7color.cpp
            src/sb7/sb7ktx.cpp
            src/sb7/sb7object.cpp
            src/sb7/sb7shader.cpp
            src/sb7/sb7textoverlay.cpp
            src/sb7/gl3w.c
)

set(RUN_DIR ${PROJECT_SOURCE_DIR}/bin)

set(EXAMPLES
  hair
)



foreach(EXAMPLE ${EXAMPLES})
  add_executable(${EXAMPLE} WIN32 src/${EXAMPLE}/${EXAMPLE}.cpp)
  set_property(TARGET ${EXAMPLE} PROPERTY DEBUG_POSTFIX _d)
  find_package(PNG REQUIRED)
  include_directories(${PNG_INCLUDE_DIR})
  target_link_libraries(${EXAMPLE} ${COMMON_LIBS} ${PNG_LIBRARY} ${EXTRA_LIBS})
  if(MSVC)
    configure_file(${PROJECT_SOURCE_DIR}/build/templates/vs2013.vcxproj.user.in ${CMAKE_CURRENT_BINARY_DIR}/${EXAMPLE}.vcxproj.user @ONLY)
  endif(MSVC)
endforeach(EXAMPLE)

IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_LINUX -std=c++0x")
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

include_directories( include )
include_directories(extern/glfw-3.0.4/include)

ADD_CUSTOM_TARGET(debug ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE:STRING=Debug ${PROJECT_SOURCE_DIR})
ADD_CUSTOM_TARGET(release ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE:STRING=Release ${PROJECT_SOURCE_DIR})
