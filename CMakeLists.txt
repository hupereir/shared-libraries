# $Id$
cmake_minimum_required(VERSION 2.8.12)

project(SHARED_LIBRARIES)
SET(SHARED_LIBRARIES_VERSION_MAJOR 2)
SET(SHARED_LIBRARIES_VERSION_MINOR 0)
SET(SHARED_LIBRARIES_VERSION_PATCH 0)
set(SHARED_LIBRARIES_VERSION ${SHARED_LIBRARIES_VERSION_MAJOR}.${SHARED_LIBRARIES_VERSION_MINOR}.${SHARED_LIBRARIES_VERSION_PATCH})

########### modules #################
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/base-cmake")
include(Macros)
setup_cmake_settings()
setup_compiler_flags()

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

add_subdirectory(base)
add_subdirectory(base-qt)
add_subdirectory(base-ssh)
add_subdirectory(base-server)
add_subdirectory(base-help)

if(ASPELL_FOUND)

  add_subdirectory(base-spellcheck)

endif()

add_subdirectory(base-transparency)
add_subdirectory(base-svg)
add_subdirectory(base-notifications)
