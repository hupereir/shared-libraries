# $Id$

PROJECT( BASE_TRANSPARENCY )
CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

set(BASE_TRANSPARENCY_VERSION_MAJOR 1)
set(BASE_TRANSPARENCY_VERSION_MINOR 0)
set(BASE_TRANSPARENCY_VERSION_PATCH 0)
set(BASE_TRANSPARENCY_VERSION ${BASE_TRANSPARENCY_VERSION_MAJOR}.${BASE_TRANSPARENCY_VERSION_MINOR}.${BASE_TRANSPARENCY_VERSION_PATCH} )
set(BASE_TRANSPARENCY_SOVERSION ${BASE_TRANSPARENCY_VERSION_MAJOR}.${BASE_TRANSPARENCY_VERSION_MINOR} )

# ########### XRender #######################
IF( UNIX )

  FIND_PACKAGE(PkgConfig REQUIRED )
  PKG_CHECK_MODULES( XRENDER REQUIRED xrender )

ENDIF()

########### Qt4 configuration #########
FIND_PACKAGE( Qt4 REQUIRED )

INCLUDE( ${QT_USE_FILE} )
ADD_DEFINITIONS( ${QT_DEFINITIONS} )

########### links #########
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
LINK_DIRECTORIES( ${CMAKE_INSTALL_PREFIX}/lib )

########### includes ###############
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/base)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/base-qt)

########### next target ###############
SET( base_transparency_SOURCES
  BackgroundPixmap.cpp
  CompositeEngine.cpp
  ShadowLabel.cpp
  TransparencyConfiguration.cpp
  TransparentWidget.cpp
)

SET( base_transparency_HEADERS
  BackgroundPixmap.h
  TransparentWidget.h
)

QT4_WRAP_CPP( base_transparency_HEADERS_MOC ${base_transparency_HEADERS} )

IF( ENABLE_SHARED )

  ADD_LIBRARY( base-transparency SHARED
    ${base_transparency_SOURCES}
    ${base_transparency_HEADERS_MOC}
  )

  SET_TARGET_PROPERTIES( base-transparency PROPERTIES VERSION ${BASE_TRANSPARENCY_VERSION} SOVERSION ${BASE_TRANSPARENCY_SOVERSION} )
  TARGET_LINK_LIBRARIES( base-transparency ${XRENDER_LIBRARIES} ${QT_LIBRARIES} base-qt)
  INSTALL( TARGETS base-transparency DESTINATION ${CMAKE_INSTALL_PREFIX}/lib )

ELSE()

  ADD_LIBRARY( base-transparency STATIC
    ${base_transparency_SOURCES}
    ${base_transparency_HEADERS_MOC}
  )

  TARGET_LINK_LIBRARIES( base-transparency ${XRENDER_LIBRARIES} ${QT_LIBRARIES} base-qt)

ENDIF()
