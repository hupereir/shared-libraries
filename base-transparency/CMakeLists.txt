# $Id$
project(BASE_TRANSPARENCY)
set(BASE_TRANSPARENCY_VERSION_MAJOR 1)
set(BASE_TRANSPARENCY_VERSION_MINOR 0)
set(BASE_TRANSPARENCY_VERSION_PATCH 0)
set(BASE_TRANSPARENCY_VERSION ${BASE_TRANSPARENCY_VERSION_MAJOR}.${BASE_TRANSPARENCY_VERSION_MINOR}.${BASE_TRANSPARENCY_VERSION_PATCH})
set(BASE_TRANSPARENCY_SOVERSION ${BASE_TRANSPARENCY_VERSION_MAJOR}.${BASE_TRANSPARENCY_VERSION_MINOR})

########### Qt configuration #########
find_package(Qt5 COMPONENTS Widgets REQUIRED)

# ########### X11 #######################
if(UNIX AND NOT APPLE)
  find_package(XCB COMPONENTS XCB SHAPE)
  if( XCB_XCB_FOUND AND XCB_SHAPE_FOUND )
    add_definitions(-DWITH_XCB=1)
  else()
    add_definitions(-DWITH_XCB=0)
  endif()
else()
  add_definitions(-DWITH_XCB=0)
endif()

########### links #########
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
link_directories(${CMAKE_INSTALL_PREFIX}/lib)

########### includes ###############
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/base)
include_directories(${CMAKE_SOURCE_DIR}/base-qt)

########### next target ###############
set(base_transparency_SOURCES
  CompositeEngine.cpp
  ShadowLabel.cpp
  TransparencyConfiguration.cpp
  TransparentWidget.cpp
)

if(ENABLE_SHARED)
  add_library(base-transparency SHARED ${base_transparency_SOURCES})
  set_target_properties(base-transparency PROPERTIES VERSION ${BASE_TRANSPARENCY_VERSION} SOVERSION ${BASE_TRANSPARENCY_SOVERSION})
  install(TARGETS base-transparency DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
else()
  add_library(base-transparency STATIC ${base_transparency_SOURCES})
endif()

target_link_libraries(base-transparency base-qt)
target_link_libraries(base-transparency ${XCB_LIBRARIES})
target_link_libraries(base-transparency Qt5::Widgets)
