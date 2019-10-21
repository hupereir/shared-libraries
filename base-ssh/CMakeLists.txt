# $Id$
project(BASE_SSH)
set(BASE_SSH_VERSION_MAJOR 1)
set(BASE_SSH_VERSION_MINOR 0)
set(BASE_SSH_VERSION_PATCH 0)
set(BASE_SSH_VERSION ${BASE_SSH_VERSION_MAJOR}.${BASE_SSH_VERSION_MINOR}.${BASE_SSH_VERSION_PATCH})
set(BASE_SSH_SOVERSION ${BASE_SSH_VERSION_MAJOR}.${BASE_SSH_VERSION_MINOR})

########### Qt configuration #########
if(USE_QT5)

  find_package(Qt5Widgets REQUIRED)
  find_package(Qt5Network REQUIRED)

else()

  find_package(Qt4 REQUIRED)
  set(QT_USE_QTNETWORK TRUE)
  include(${QT_USE_FILE})
  add_definitions(${QT_DEFINITIONS})

endif()

########### ssh #########
find_package(LibSSH)
if(LibSSH_FOUND)

  add_definitions(-DWITH_SSH=1)
  include_directories(${LIBSSH_INCLUDE_DIR})

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
  SshLoginDialog.cpp
  SshReadFileSocket.cpp
  SshSocket.cpp
  SshTunnel.cpp
  SshWriteFileSocket.cpp
)

if(ENABLE_SHARED)

  add_library(base-ssh SHARED ${base_ssh_SOURCES})
  set_target_properties(base-ssh PROPERTIES VERSION ${BASE_SSH_VERSION} SOVERSION ${BASE_SSH_SOVERSION})
  install(TARGETS base-ssh DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

else()

  add_library(base-ssh STATIC ${base_ssh_SOURCES})

endif()

target_link_libraries(base-ssh base-qt)

if(WIN32)

  # add needed libraries for sockets and inet on windows
  target_link_libraries(base-ssh ws2_32)

endif()

if(LibSSH_FOUND)

  target_link_libraries(base-ssh ${LIBSSH_LIBRARIES})

endif()

if(USE_QT5)

  target_link_libraries(base-ssh Qt5::Widgets Qt5::Network)

else()

  target_link_libraries(base-ssh ${QT_LIBRARIES} base base-qt)

endif()
