PROJECT( BASE_SERVER )
CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

set(BASE_SERVER_VERSION_MAJOR 1)
set(BASE_SERVER_VERSION_MINOR 0)
set(BASE_SERVER_VERSION_PATCH 0)
set(BASE_SERVER_VERSION ${BASE_SERVER_VERSION_MAJOR}.${BASE_SERVER_VERSION_MINOR}.${BASE_SERVER_VERSION_PATCH} )
set(BASE_SERVER_SOVERSION ${BASE_SERVER_VERSION_MAJOR}.${BASE_SERVER_VERSION_MINOR} )

########### Qt4 configuration #########
FIND_PACKAGE( Qt4 REQUIRED )
SET( QT_USE_QTXML TRUE )
SET( QT_USE_QTDBUS TRUE )

INCLUDE( ${QT_USE_FILE} )
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
ADD_DEFINITIONS( ${QT_DEFINITIONS} )

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/base)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/base-qt)

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
)

QT4_WRAP_CPP( base_server_HEADERS_MOC ${base_server_HEADERS} )

ADD_LIBRARY( base-server SHARED
  ${base_server_SOURCES}
  ${base_server_HEADERS_MOC}
)

SET_TARGET_PROPERTIES( base-server PROPERTIES VERSION ${BASE_SERVER_VERSION} SOVERSION ${BASE_SERVER_SOVERSION} )
TARGET_LINK_LIBRARIES( base-server ${QT_LIBRARIES} base base-qt )
INSTALL( TARGETS base-server DESTINATION ${CMAKE_INSTALL_PREFIX}/lib )

########### next target ###############
ADD_CUSTOM_COMMAND( OUTPUT "testexternals.cpp" COMMAND echo \"int main() {} \" > testexternals.cpp )
SET( testexternals_SOURCES testexternals.cpp )

ADD_EXECUTABLE( testexternals-base-server ${testexternals_SOURCES} )
TARGET_LINK_LIBRARIES( testexternals-base-server ${QT_LIBRARIES} base-server )
