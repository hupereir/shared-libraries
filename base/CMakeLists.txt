# $Id$
project(BASE)
set(BASE_VERSION_MAJOR 1)
set(BASE_VERSION_MINOR 0)
set(BASE_VERSION_PATCH 0)
set(BASE_VERSION ${BASE_VERSION_MAJOR}.${BASE_VERSION_MINOR}.${BASE_VERSION_PATCH})
set(BASE_SOVERSION ${BASE_VERSION_MAJOR}.${BASE_VERSION_MINOR})

########### Qt configuration #########
if(USE_QT6)
find_package(Qt6 COMPONENTS Widgets Network REQUIRED)
else()
find_package(Qt5 COMPONENTS Widgets Network REQUIRED)
endif()

########### includes ###############
include_directories(${CMAKE_CURRENT_BINARY_DIR})

########### links #########
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
link_directories(${CMAKE_INSTALL_PREFIX}/lib)

########### next target ###############
set(base_SOURCES
  Color.cpp
  Command.cpp
  CommandLineArguments.cpp
  CommandLineParser.cpp
  CounterMap.cpp
  CustomProcess.cpp
  Debug.cpp
  File.cpp
  FileThread.cpp
  FileRecord.cpp
  Key.cpp
  Option.cpp
  Options.cpp
  Singleton.cpp
  TimeStamp.cpp
  Util.cpp
)

if(USE_SHARED_LIBS)
  add_library(base SHARED ${base_SOURCES})
  set_target_properties(base PROPERTIES VERSION ${BASE_VERSION} SOVERSION ${BASE_SOVERSION})
  install(TARGETS base DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
else()
  add_library(base STATIC ${base_SOURCES})
endif()

target_link_libraries(base Qt::Widgets Qt::Network)
