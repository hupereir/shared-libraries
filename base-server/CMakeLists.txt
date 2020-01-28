# $Id$
project(BASE_SERVER)
set(BASE_SERVER_VERSION_MAJOR 1)
set(BASE_SERVER_VERSION_MINOR 0)
set(BASE_SERVER_VERSION_PATCH 0)
set(BASE_SERVER_VERSION ${BASE_SERVER_VERSION_MAJOR}.${BASE_SERVER_VERSION_MINOR}.${BASE_SERVER_VERSION_PATCH})
set(BASE_SERVER_SOVERSION ${BASE_SERVER_VERSION_MAJOR}.${BASE_SERVER_VERSION_MINOR})

########### Qt configuration #########
find_package(Qt5 COMPONENTS Widgets Network Xml REQUIRED )

########### kde config ###############
find_program(KDE_CONFIG NAMES kf5-config kde4-config)

########### configuration files ###############
configure_file(ServerSystemOptions.h.cmake ServerSystemOptions.h)

########### includes ###############
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/base)
include_directories(${CMAKE_SOURCE_DIR}/base-qt)

########### links #########
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
link_directories(${CMAKE_INSTALL_PREFIX}/lib)

########### next target ###############
set(base_server_SOURCES
  ApplicationId.cpp
  ApplicationManager.cpp
  BaseApplication.cpp
  BaseCoreApplication.cpp
  Client.cpp
  ServerCommand.cpp
  ServerConfiguration.cpp
)

if(ENABLE_SHARED)

  add_library(base-server SHARED ${base_server_SOURCES})
  set_target_properties(base-server PROPERTIES VERSION ${BASE_SERVER_VERSION} SOVERSION ${BASE_SERVER_SOVERSION})
  install(TARGETS base-server DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

else()

  add_library(base-server STATIC ${base_server_SOURCES})

endif()

target_link_libraries(base-server base base-qt)
target_link_libraries(base-server Qt5::Widgets Qt5::Network Qt5::Xml)
if(Qt5DBus_FOUND )
  target_link_libraries(base-server Qt5::DBus)
else()
  add_definitions("-DQT_NO_DBUS")
endif()

