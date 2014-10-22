# $Id$
project(BASE_TRANSPARENCY)
set(BASE_TRANSPARENCY_VERSION_MAJOR 1)
set(BASE_TRANSPARENCY_VERSION_MINOR 0)
set(BASE_TRANSPARENCY_VERSION_PATCH 0)
set(BASE_TRANSPARENCY_VERSION ${BASE_TRANSPARENCY_VERSION_MAJOR}.${BASE_TRANSPARENCY_VERSION_MINOR}.${BASE_TRANSPARENCY_VERSION_PATCH})
set(BASE_TRANSPARENCY_SOVERSION ${BASE_TRANSPARENCY_VERSION_MAJOR}.${BASE_TRANSPARENCY_VERSION_MINOR})

########### Qt configuration #########
if(USE_QT5)

  find_package(Qt5Widgets REQUIRED)
  find_package(Qt5Network REQUIRED)

else()

  find_package(Qt4 REQUIRED)
  include(${QT_USE_FILE})
  add_definitions(${QT_DEFINITIONS})

endif()

# ########### X11 #######################
if(UNIX  AND NOT APPLE)

  find_package(X11)
  if(X11_FOUND)
    find_package(XCB REQUIRED COMPONENTS XCB SHAPE)
    add_definitions(-DHAVE_XCB=${X11_FOUND})
  endif()

else()

  add_definitions(-DHAVE_XCB=0)

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

target_link_libraries(base-transparency ${XCB_LIBRARIES})
target_link_libraries(base-transparency ${QT_LIBRARIES} base-qt)

if(USE_QT5)

  qt5_use_modules(base-transparency Widgets Network)

endif()
