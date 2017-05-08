# $Id$
project(BASE_HELP)
set(BASE_HELP_VERSION_MAJOR 1)
set(BASE_HELP_VERSION_MINOR 0)
set(BASE_HELP_VERSION_PATCH 0)
set(BASE_HELP_VERSION ${BASE_HELP_VERSION_MAJOR}.${BASE_HELP_VERSION_MINOR}.${BASE_HELP_VERSION_PATCH})
set(BASE_HELP_SOVERSION ${BASE_HELP_VERSION_MAJOR}.${BASE_HELP_VERSION_MINOR})

########### Qt4 configuration #########
if(USE_QT5)

  find_package(Qt5Widgets REQUIRED)
  find_package(Qt5Xml REQUIRED)

else()

  find_package(Qt4 REQUIRED)
  set(QT_USE_QTXML TRUE)
  include(${QT_USE_FILE})
  add_definitions(${QT_DEFINITIONS})

endif()

########### includes ###############
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/base)
include_directories(${CMAKE_SOURCE_DIR}/base-qt)

########### links #########
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
link_directories(${CMAKE_INSTALL_PREFIX}/lib)

########### next target ###############
set(base_help_SOURCES
  HelpDialog.cpp
  HelpItem.cpp
  HelpManager.cpp
  HelpModel.cpp
 )

if(ENABLE_SHARED)

  add_library(base-help SHARED ${base_help_SOURCES})
  set_target_properties(base-help PROPERTIES VERSION ${BASE_HELP_VERSION} SOVERSION ${BASE_HELP_SOVERSION})
  install(TARGETS base-help DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

else()

  add_library(base-help STATIC ${base_help_SOURCES})

endif()

target_link_libraries(base-help ${QT_LIBRARIES} base base-qt)

if(USE_QT5)

  target_link_libraries(base-help Qt5::Widgets Qt5::Xml)

endif()
