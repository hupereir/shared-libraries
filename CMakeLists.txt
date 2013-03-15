# $Id$
PROJECT( BASE_SERVER )
SET( BASE_SERVER_VERSION_MAJOR 1 )
SET( BASE_SERVER_VERSION_MINOR 0 )
SET( BASE_SERVER_VERSION_PATCH 0 )
SET( BASE_SERVER_VERSION ${BASE_SERVER_VERSION_MAJOR}.${BASE_SERVER_VERSION_MINOR}.${BASE_SERVER_VERSION_PATCH} )
SET( BASE_SERVER_SOVERSION ${BASE_SERVER_VERSION_MAJOR}.${BASE_SERVER_VERSION_MINOR} )

########### Qt4 configuration #########
IF( USE_QT5 )

  FIND_PACKAGE( Qt5Widgets REQUIRED )

ELSE()

  FIND_PACKAGE( Qt4 REQUIRED )
  SET( QT_USE_QTXML TRUE )
  SET( QT_USE_QTNETWORK TRUE )

  IF( UNIX AND NOT APPLE )

    SET( QT_USE_QTDBUS TRUE )

  ELSE()

    ADD_DEFINITIONS( "-DQT_NO_DBUS" )

  ENDIF()

  INCLUDE( ${QT_USE_FILE} )
  ADD_DEFINITIONS( ${QT_DEFINITIONS} )

ENDIF()

# DBUS: is forced to false on APPLE

########### includes ###############
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/base)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/base-qt)

########### links #########
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
LINK_DIRECTORIES( ${CMAKE_INSTALL_PREFIX}/lib )

########### next target ###############
SET( base_server_SOURCES
  ApplicationId.cpp
  ApplicationManager.cpp
  BaseApplication.cpp
  BaseCoreApplication.cpp
  Client.cpp
  ServerCommand.cpp
  ServerConfiguration.cpp
)

SET( base_server_HEADERS
  ApplicationManager.h
  BaseApplication.h
  BaseCoreApplication.h
  Client.h
  ServerConfiguration.h
)

IF( USE_QT5 )

  QT5_WRAP_CPP( base_server_HEADERS_MOC ${base_server_HEADERS} )

ELSE()

  QT4_WRAP_CPP( base_server_HEADERS_MOC ${base_server_HEADERS} )

ENDIF()

IF( ENABLE_SHARED )

  ADD_LIBRARY( base-server SHARED
    ${base_server_SOURCES}
    ${base_server_HEADERS_MOC}
  )

  SET_TARGET_PROPERTIES( base-server PROPERTIES VERSION ${BASE_SERVER_VERSION} SOVERSION ${BASE_SERVER_SOVERSION} )
  TARGET_LINK_LIBRARIES( base-server ${QT_LIBRARIES} base base-qt )
  INSTALL( TARGETS base-server DESTINATION ${CMAKE_INSTALL_PREFIX}/lib )

ELSE()

  ADD_LIBRARY( base-server STATIC
    ${base_server_SOURCES}
    ${base_server_HEADERS_MOC}
  )
  TARGET_LINK_LIBRARIES( base-server ${QT_LIBRARIES} base base-qt )

ENDIF()

IF( USE_QT5 )

  QT5_USE_MODULES( base-server Widgets Network Xml DBus )

ENDIF()
