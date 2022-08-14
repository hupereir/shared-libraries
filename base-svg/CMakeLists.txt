# $Id$
project(BASE_SVG)
set(BASE_SVG_VERSION_MAJOR 1)
set(BASE_SVG_VERSION_MINOR 0)
set(BASE_SVG_VERSION_PATCH 0)
set(BASE_SVG_VERSION ${BASE_SVG_VERSION_MAJOR}.${BASE_SVG_VERSION_MINOR}.${BASE_SVG_VERSION_PATCH})
set(BASE_SVG_SOVERSION ${BASE_SVG_VERSION_MAJOR}.${BASE_SVG_VERSION_MINOR})

########### Qt configuration #########
if(USE_QT6)
find_package(Qt6 COMPONENTS Widgets Network Svg Xml REQUIRED)
else()
find_package(Qt5 COMPONENTS Widgets Network Svg Xml REQUIRED)
endif()

########### kde config ###############
find_program(KDE_CONFIG NAMES kf5-config kde4-config)

########### zlib ###############
if(NOT WIN32)
  find_package(ZLIB)
endif()

if(ZLIB_FOUND)
  include_directories(${ZLIB_INCLUDE_DIRS})
  add_definitions(-DWITH_ZLIB=1)
else()
  add_definitions(-DWITH_ZLIB=0)
endif()

########### configuration files ###############
configure_file(SvgSystemOptions.h.cmake SvgSystemOptions.h)

########### links #########
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
link_directories(${CMAKE_INSTALL_PREFIX}/lib)

########### includes ###############
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/base)
include_directories(${CMAKE_SOURCE_DIR}/base-qt)
include_directories(${CMAKE_SOURCE_DIR}/base-transparency)

########### next target ###############
set(base_svg_SOURCES
  BaseSvgRenderer.cpp
  SvgConfiguration.cpp
  SvgEngine.cpp
  SvgPlasmaInterface.cpp
  SvgRenderer.cpp
  SvgThread.cpp
)

set(base_svg_RESOURCES baseSvg.qrc)

if(USE_SHARED_LIBS)
  add_library(base-svg SHARED ${base_svg_SOURCES} ${base_svg_RESOURCES})
  set_target_properties(base-svg PROPERTIES VERSION ${BASE_SVG_VERSION} SOVERSION ${BASE_SVG_SOVERSION})
  install(TARGETS base-svg DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
else()
  add_library(base-svg STATIC ${base_svg_SOURCES} ${base_svg_RESOURCES})
endif()

target_link_libraries(base-svg base-transparency)

if( ZLIB_FOUND )
  target_link_libraries(base-svg ${ZLIB_LIBRARIES})
endif()
target_link_libraries(base-svg Qt::Widgets Qt::Svg Qt::Xml)
