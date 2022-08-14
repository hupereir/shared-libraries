# $Id$
project(BASE_NOTIFICATIONS)
set(BASE_NOTIFICATIONS_VERSION_MAJOR 1)
set(BASE_NOTIFICATIONS_VERSION_MINOR 0)
set(BASE_NOTIFICATIONS_VERSION_PATCH 0)
set(BASE_NOTIFICATIONS_VERSION ${BASE_NOTIFICATIONS_VERSION_MAJOR}.${BASE_NOTIFICATIONS_VERSION_MINOR}.${BASE_NOTIFICATIONS_VERSION_PATCH})
set(BASE_NOTIFICATIONS_SOVERSION ${BASE_NOTIFICATIONS_VERSION_MAJOR}.${BASE_NOTIFICATIONS_VERSION_MINOR})

########### Qt configuration #########
if(USE_QT6)
find_package(Qt6 COMPONENTS Widgets OPTIONAL_COMPONENTS DBus REQUIRED)
else()
find_package(Qt5 COMPONENTS Widgets OPTIONAL_COMPONENTS DBus REQUIRED)
endif()
########### includes ###############
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/base)
include_directories(${CMAKE_SOURCE_DIR}/base-qt)

########### links #########
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
link_directories(${CMAKE_INSTALL_PREFIX}/lib)

########### next target ###############
set(base_notifications_SOURCES
  ImageData.cpp
  SystemNotifications.cpp
  SystemNotifications_p.cpp
  TaskBarProgressNotifications.cpp
)

if(USE_SHARED_LIBS)
  add_library(base-notifications SHARED ${base_notifications_SOURCES})
  set_target_properties(base-notifications PROPERTIES VERSION ${BASE_NOTIFICATIONS_VERSION} SOVERSION ${BASE_NOTIFICATIONS_VERSION})
  install(TARGETS base-notifications DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
else()
  add_library(base-notifications STATIC ${base_notifications_SOURCES})
endif()

target_link_libraries(base-notifications base base-qt)
target_link_libraries(base-notifications Qt::Widgets)
if(Qt5DBus_FOUND )
  target_link_libraries(base-notifications Qt::DBus)
else()
  add_definitions("-DQT_NO_DBUS")
endif()
