# $Id$
project(BASE_FILESYSTEM)
set(BASE_FILESYSTEM_VERSION_MAJOR 1)
set(BASE_FILESYSTEM_VERSION_MINOR 0)
set(BASE_FILESYSTEM_VERSION_PATCH 0)
set(BASE_FILESYSTEM_VERSION ${BASE_FILESYSTEM_VERSION_MAJOR}.${BASE_FILESYSTEM_VERSION_MINOR}.${BASE_FILESYSTEM_VERSION_PATCH})
set(BASE_FILESYSTEM_SOVERSION ${BASE_FILESYSTEM_VERSION_MAJOR}.${BASE_FILESYSTEM_VERSION_MINOR})

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

########### next target ###############
set(base_filesystem_SOURCES
  FileIconProvider.cpp
  FileInformationDialog.cpp
  BaseFileSystemWidget.cpp
  FileSystemModel.cpp
  RemoveFilesDialog.cpp
  RenameFileDialog.cpp
)

set(base_filesystem_RESOURCES baseFileSystem.qrc)

if(USE_QT5)

  qt5_add_resources(base_filesystem_RESOURCES_RCC ${base_filesystem_RESOURCES})

else()

  qt4_add_resources(base_filesystem_RESOURCES_RCC ${base_filesystem_RESOURCES})

endif()

if(ENABLE_SHARED)

  add_library(base-filesystem SHARED ${base_filesystem_SOURCES} ${base_filesystem_RESOURCES_RCC})
  set_target_properties(base-filesystem PROPERTIES VERSION ${BASE_FILESYSTEM_VERSION} SOVERSION ${BASE_FILESYSTEM_SOVERSION})
  install(TARGETS base-filesystem DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

else()

  add_library(base-filesystem STATIC ${base_filesystem_SOURCES} ${base_filesystem_RESOURCES_RCC})

endif()

target_link_libraries(base-filesystem base-qt)

if(USE_QT5)

  target_link_libraries(base-filesystem Qt5::Widgets Qt5::Xml)

else()

  target_link_libraries(base-filesystem ${QT_LIBRARIES})

endif()
