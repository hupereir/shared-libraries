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
find_package( PkgConfig REQUIRED )
pkg_check_modules(LIBSSH2 libssh2>=1.2.8)
if(${LIBSSH2_FOUND})
   add_definitions(-DHAVE_SSH=1)
  include_directories(${LIBSSH2_INCLUDE_DIRS})
else()
   add_definitions(-DHAVE_SSH=0)
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
  SshChannelThread.cpp
  SshConnection.cpp
  SshConnectionAttributes.cpp
  SshListeningThread.cpp
  SshLoginDialog.cpp
  SshSingleton.cpp
  SshSocket.cpp
  SshTunnel.cpp
  SshUtil.cpp
)

if(ENABLE_SHARED)

  add_library(base-ssh SHARED ${base_ssh_SOURCES})
  set_target_properties(base-ssh PROPERTIES VERSION ${BASE_SSH_VERSION} SOVERSION ${BASE_SSH_SOVERSION})
  install(TARGETS base-ssh DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

else()

  add_library(base-ssh STATIC ${base_ssh_SOURCES} ${base_ssh_RESOURCES_RCC})

endif()

target_link_libraries(base-ssh ${QT_LIBRARIES} base base-qt)

if(WIN32)
  # add needed libraries for sockets and inet on windows
  target_link_libraries(base-ssh ws2_32)
endif()

if(${LIBSSH2_FOUND})
target_link_libraries(base-ssh ${LIBSSH2_LIBRARIES})
endif()

if(USE_QT5)

  qt5_use_modules(base-ssh Widgets Network)

endif()