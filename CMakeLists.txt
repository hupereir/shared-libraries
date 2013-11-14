# $Id$
PROJECT( BASE_SVG )
SET( BASE_SVG_VERSION_MAJOR 1 )
SET( BASE_SVG_VERSION_MINOR 0 )
SET( BASE_SVG_VERSION_PATCH 0 )
SET( BASE_SVG_VERSION ${BASE_SVG_VERSION_MAJOR}.${BASE_SVG_VERSION_MINOR}.${BASE_SVG_VERSION_PATCH} )
SET( BASE_SVG_SOVERSION ${BASE_SVG_VERSION_MAJOR}.${BASE_SVG_VERSION_MINOR} )

########### Qt4 configuration #########
IF( USE_QT5 )

  FIND_PACKAGE( Qt5Widgets REQUIRED )

ELSE()

  FIND_PACKAGE( Qt4 REQUIRED )
  SET( QT_USE_QTSVG TRUE )
  INCLUDE( ${QT_USE_FILE} )
  ADD_DEFINITIONS( ${QT_DEFINITIONS} )

ENDIF()

########### aspell ###############
FIND_PROGRAM( KDE4_CONFIG kde4-config )

########### configuration files ###############
CONFIGURE_FILE( SvgSystemOptions.h.cmake SvgSystemOptions.h )

########### links #########
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
LINK_DIRECTORIES( ${CMAKE_INSTALL_PREFIX}/lib )

########### includes ###############
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/base)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/base-qt)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/base-transparency)

########### next target ###############
SET( base_svg_SOURCES
  SvgConfiguration.cpp
  SvgEngine.cpp
  SvgPlasmaInterface.cpp
  SvgRenderer.cpp
  SvgThread.cpp
)

SET( base_svg_RESOURCES baseSvg.qrc)

IF( USE_QT5 )

  QT5_ADD_RESOURCES( base_svg_RESOURCES_RCC ${base_svg_RESOURCES})

ELSE()

  QT4_ADD_RESOURCES( base_svg_RESOURCES_RCC ${base_svg_RESOURCES})

ENDIF()

IF( ENABLE_SHARED )

  ADD_SHARED_LIBRARY( base-svg ${base_svg_SOURCES} ${base_svg_RESOURCES_RCC} )
  SET_TARGET_PROPERTIES( base-svg PROPERTIES VERSION ${BASE_SVG_VERSION} SOVERSION ${BASE_SVG_SOVERSION} )
  TARGET_LINK_LIBRARIES( base-svg ${QT_LIBRARIES} base-transparency)
  INSTALL( TARGETS base-svg DESTINATION ${CMAKE_INSTALL_PREFIX}/lib )

ELSE()

  ADD_STATIC_LIBRARY( base-svg ${base_svg_SOURCES} ${base_svg_RESOURCES_RCC} )
  TARGET_LINK_LIBRARIES( base-svg ${QT_LIBRARIES} base-transparency)

ENDIF()

IF( USE_QT5 )

  QT5_USE_MODULES( base-svg Widgets Svg )

ENDIF()
