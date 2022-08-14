# $Id$
project(BASE_FILESYSTEM)
set(BASE_FILESYSTEM_VERSION_MAJOR 1)
set(BASE_FILESYSTEM_VERSION_MINOR 0)
set(BASE_FILESYSTEM_VERSION_PATCH 0)
set(BASE_FILESYSTEM_VERSION ${BASE_FILESYSTEM_VERSION_MAJOR}.${BASE_FILESYSTEM_VERSION_MINOR}.${BASE_FILESYSTEM_VERSION_PATCH})
set(BASE_FILESYSTEM_SOVERSION ${BASE_FILESYSTEM_VERSION_MAJOR}.${BASE_FILESYSTEM_VERSION_MINOR})

########### Qt configuration #########
if(USE_QT6)
find_package(Qt6 COMPONENTS Widgets Xml REQUIRED)
else()
find_package(Qt5 COMPONENTS Widgets Xml REQUIRED)
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

if(USE_SHARED_LIBS)
  add_library(base-filesystem SHARED ${base_filesystem_SOURCES} ${base_filesystem_RESOURCES})
  set_target_properties(base-filesystem PROPERTIES VERSION ${BASE_FILESYSTEM_VERSION} SOVERSION ${BASE_FILESYSTEM_SOVERSION})
  install(TARGETS base-filesystem DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
else()
  add_library(base-filesystem STATIC ${base_filesystem_SOURCES} ${base_filesystem_RESOURCES})
endif()

target_link_libraries(base-filesystem base-qt)
target_link_libraries(base-filesystem Qt::Widgets Qt::Xml)
