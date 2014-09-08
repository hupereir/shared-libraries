# $Id$
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.6)
if(COMMAND CMAKE_POLICY)
  cmake_policy(SET CMP0003 NEW)
endif()

set(CMAKE_AUTOMOC TRUE)

project(SHARED_LIBRARIES)
set(SHARED_LIBRARIES_VERSION_MAJOR 1)
set(SHARED_LIBRARIES_VERSION_MINOR 0)
set(SHARED_LIBRARIES_VERSION_PATCH 4)
set(SHARED_LIBRARIES_VERSION ${SHARED_LIBRARIES_VERSION_MAJOR}.${SHARED_LIBRARIES_VERSION_MINOR}.${SHARED_LIBRARIES_VERSION_PATCH})

########### modules #################
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/base-cmake")
include(Macros)

if(WIN32)

  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--enable-auto-import")
  set(CMAKE_SHARED_LIBRARY_CXX_FLAGS  "${CMAKE_SHARED_LIBRARY_CXX_FLAGS} -Wl,--enable-auto-import ")

endif()

########### aspell ###############
find_package(ASPELL)

########### links #########
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

########### make shared libraries #########
set(ENABLE_SHARED 1)

########### sub directories #########
set(CMAKE_CXX_FLAGS "-g -Wall -Wundef")

add_subdirectory(base)
add_subdirectory(base-qt)
add_subdirectory(base-server)
add_subdirectory(base-help)

if(ASPELL_FOUND)

  add_subdirectory(base-spellcheck)

endif()

add_subdirectory(base-transparency)
add_subdirectory(base-svg)
add_subdirectory(base-notifications)
