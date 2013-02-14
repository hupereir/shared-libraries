# $Id$
PROJECT( BASE )
SET( BASE_VERSION_MAJOR 1 )
SET( BASE_VERSION_MINOR 0 )
SET( BASE_VERSION_PATCH 0 )
SET( BASE_VERSION ${BASE_VERSION_MAJOR}.${BASE_VERSION_MINOR}.${BASE_VERSION_PATCH} )
SET( BASE_SOVERSION ${BASE_VERSION_MAJOR}.${BASE_VERSION_MINOR} )

########### Qt4 configuration #########
IF( USE_QT5 )

  SET( CMAKE_AUTOMOC ON )
  SET( CMAKE_INCLUDE_CURRENT_DIR ON )
  FIND_PACKAGE( Qt5Widgets )

ELSE()

  FIND_PACKAGE( Qt4 REQUIRED )
  SET( QT_USE_QTNETWORK TRUE )
  INCLUDE( ${QT_USE_FILE} )
  ADD_DEFINITIONS( ${QT_DEFINITIONS} )

ENDIF()

########### includes ###############
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

########### links #########
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
LINK_DIRECTORIES( ${CMAKE_INSTALL_PREFIX}/lib )

########### next target ###############
SET( base_SOURCES
  Command.cpp
  CommandLineArguments.cpp
  CommandLineParser.cpp
  Counter.cpp
  CounterMap.cpp
  CustomProcess.cpp
  Debug.cpp
  File.cpp
  FileThread.cpp
  FileRecord.cpp
  IOString.cpp
  Key.cpp
  Option.cpp
  Options.cpp
  Singleton.cpp
  TimeStamp.cpp
  Util.cpp
)

SET( base_HEADERS FileThread.h )

IF( NOT USE_QT5 )

  QT4_WRAP_CPP( base_HEADERS_MOC ${base_HEADERS} )

ENDIF()

IF( ENABLE_SHARED )

  ADD_LIBRARY( base SHARED ${base_SOURCES} ${base_HEADERS_MOC} )
  SET_TARGET_PROPERTIES( base PROPERTIES VERSION ${BASE_VERSION} SOVERSION ${BASE_SOVERSION} )
  TARGET_LINK_LIBRARIES( base ${QT_LIBRARIES} )
  INSTALL( TARGETS base DESTINATION ${CMAKE_INSTALL_PREFIX}/lib )

ELSE()

  ADD_LIBRARY( base STATIC ${base_SOURCES} ${base_HEADERS_MOC} )
  TARGET_LINK_LIBRARIES( base ${QT_LIBRARIES} )

ENDIF()

IF( USE_QT5 )

  QT5_USE_MODULES( base Widgets Network )

ENDIF()
