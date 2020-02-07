# $Id$
project(BASE_SSH)
set(BASE_SSH_VERSION_MAJOR 1)
set(BASE_SSH_VERSION_MINOR 0)
set(BASE_SSH_VERSION_PATCH 0)
set(BASE_SSH_VERSION ${BASE_SSH_VERSION_MAJOR}.${BASE_SSH_VERSION_MINOR}.${BASE_SSH_VERSION_PATCH})
set(BASE_SSH_SOVERSION ${BASE_SSH_VERSION_MAJOR}.${BASE_SSH_VERSION_MINOR})

########### Qt configuration #########
find_package(Qt5 COMPONENTS Widgets Network REQUIRED)

########### ssh #########
find_package(libssh)
if(libssh_FOUND)
  add_definitions(-DWITH_SSH=1)
  if(NOT TARGET ssh)
    add_library(ssh SHARED IMPORTED)
    set_target_properties(ssh
      PROPERTIES
      IMPORTED_LOCATION "${LIBSSH_LIBRARIES}"
      INTERFACE_INCLUDE_DIRECTORIES "${LIBSSH_INCLUDE_DIR}")
  endif()
else()
  add_definitions(-DWITH_SSH=0)
endif()

########### includes #########
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/base)
include_directories(${CMAKE_SOURCE_DIR}/base-qt)

########### links #########
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
link_directories(${CMAKE_INSTALL_PREFIX}/lib)

########### next target ###############
set(base_ssh_SOURCES
  SshConnection.cpp
  SshConnectionAttributes.cpp
  SshFileTransferObject.cpp
  SshLoginDialog.cpp
  SshReadFileSocket.cpp
  SshSocket.cpp
  SshTunnel.cpp
  SshUtil.cpp
  SshWriteFileSocket.cpp
)

if(USE_SHARED_LIBS)
  add_library(base-ssh SHARED ${base_ssh_SOURCES})
  set_target_properties(base-ssh PROPERTIES VERSION ${BASE_SSH_VERSION} SOVERSION ${BASE_SSH_SOVERSION})
  install(TARGETS base-ssh DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
else()
  add_definitions(-DBASE_SSH_STATIC_DEFINE)
  add_library(base-ssh STATIC ${base_ssh_SOURCES})
endif()

target_link_libraries(base-ssh base-qt)
target_link_libraries(base-ssh Qt5::Widgets Qt5::Network)

if(libssh_FOUND)
  target_link_libraries(base-ssh ssh)
endif()
