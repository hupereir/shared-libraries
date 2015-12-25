# $Id$
project(BASE_NOTIFICATIONS)
set(BASE_NOTIFICATIONS_VERSION_MAJOR 1)
set(BASE_NOTIFICATIONS_VERSION_MINOR 0)
set(BASE_NOTIFICATIONS_VERSION_PATCH 0)
set(BASE_NOTIFICATIONS_VERSION ${BASE_NOTIFICATIONS_VERSION_MAJOR}.${BASE_NOTIFICATIONS_VERSION_MINOR}.${BASE_NOTIFICATIONS_VERSION_PATCH})
set(BASE_NOTIFICATIONS_SOVERSION ${BASE_NOTIFICATIONS_VERSION_MAJOR}.${BASE_NOTIFICATIONS_VERSION_MINOR})

########### Qt4 configuration #########
if(USE_QT5)

  find_package(Qt5Widgets REQUIRED)
  find_package(Qt5DBus)

else()

  find_package(Qt4 REQUIRED)

  if(UNIX AND NOT APPLE)

    set(QT_USE_QTDBUS TRUE)

  else()

    add_definitions("-DQT_NO_DBUS")

  endif()

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
set(base_notifications_SOURCES
  ImageData.cpp
  SystemNotifications.cpp
)

if(ENABLE_SHARED)

  add_library(base-notifications SHARED ${base_notifications_SOURCES})
  set_target_properties(base-notifications PROPERTIES VERSION ${BASE_NOTIFICATIONS_VERSION} SOVERSION ${BASE_NOTIFICATIONS_VERSION})
  install(TARGETS base-notifications DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

else()

  add_library(base-notifications STATIC ${base_notifications_SOURCES})

endif()

target_link_libraries(base-notifications ${QT_LIBRARIES} base base-qt)

if(USE_QT5)

  qt5_use_modules(base-notifications Widgets)
  if(Qt5DBus_FOUND )

    qt5_use_modules(base-notifications DBus)

  else()

    add_definitions("-DQT_NO_DBUS")

  endif()

endif()
