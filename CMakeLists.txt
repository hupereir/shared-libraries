# $Id$
PROJECT( BASE_TRANSPARENCY )
SET( BASE_TRANSPARENCY_VERSION_MAJOR 1 )
SET( BASE_TRANSPARENCY_VERSION_MINOR 0 )
SET( BASE_TRANSPARENCY_VERSION_PATCH 0 )
SET( BASE_TRANSPARENCY_VERSION ${BASE_TRANSPARENCY_VERSION_MAJOR}.${BASE_TRANSPARENCY_VERSION_MINOR}.${BASE_TRANSPARENCY_VERSION_PATCH} )
SET( BASE_TRANSPARENCY_SOVERSION ${BASE_TRANSPARENCY_VERSION_MAJOR}.${BASE_TRANSPARENCY_VERSION_MINOR} )

# ########### XRender #######################
IF( UNIX  AND NOT APPLE )

  FIND_PACKAGE(  PkgConfig REQUIRED )
  PKG_CHECK_MODULES( XRENDER REQUIRED xrender )

ENDIF()

########### Qt4 configuration #########
IF( USE_QT5 )

  SET( CMAKE_AUTOMOC ON )
  SET( CMAKE_INCLUDE_CURRENT_DIR ON )
  FIND_PACKAGE( Qt5Widgets )

ELSE()

  FIND_PACKAGE( Qt4 REQUIRED )
  INCLUDE( ${QT_USE_FILE} )
  ADD_DEFINITIONS( ${QT_DEFINITIONS} )

ENDIF()

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

IF( NOT USE_QT5 )

  QT4_WRAP_CPP( base_transparency_HEADERS_MOC ${base_transparency_HEADERS} )

ENDIF()

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

IF( USE_QT5 )

  QT5_USE_MODULES( base-transparency Widgets Network )

ENDIF()
