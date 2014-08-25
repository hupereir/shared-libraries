# $Id$
project( BASE_NOTIFICATIONS )
set( BASE_NOTIFICATIONS_VERSION_MAJOR 1 )
set( BASE_NOTIFICATIONS_VERSION_MINOR 0 )
set( BASE_NOTIFICATIONS_VERSION_PATCH 0 )
set( BASE_NOTIFICATIONS_VERSION ${BASE_NOTIFICATIONS_VERSION_MAJOR}.${BASE_NOTIFICATIONS_VERSION_MINOR}.${BASE_NOTIFICATIONS_VERSION_PATCH} )
set( BASE_NOTIFICATIONS_SOVERSION ${BASE_NOTIFICATIONS_VERSION_MAJOR}.${BASE_NOTIFICATIONS_VERSION_MINOR} )

########### Qt4 configuration #########
if( USE_QT5 )

  find_package( Qt5Widgets REQUIRED )

else()

  find_package( Qt4 REQUIRED )
  include( ${QT_USE_FILE} )
  add_definitions( ${QT_DEFINITIONS} )

endif()

############ Notifications
find_package( PkgConfig )
if( PKG_CONFIG_FOUND )

  PKG_CHECK_MODULES( LIBNOTIFY libnotify )
  if( LIBNOTIFY_FOUND )
    include_directories( ${LIBNOTIFY_INCLUDE_DIRS} )
    add_definitions( -DWITH_LIBNOTIFY=${LIBNOTIFY_FOUND} )

  else()

    add_definitions( -DWITH_LIBNOTIFY=FALSE )

  endif()

else()

  add_definitions( -DWITH_LIBNOTIFY=FALSE )

endif()


########### includes ###############
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/base)
include_directories(${CMAKE_SOURCE_DIR}/base-qt)

########### links #########
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
link_directories( ${CMAKE_INSTALL_PREFIX}/lib )

########### next target ###############
set( base_notifications_SOURCES
  SystemNotifications.cpp
  SystemNotifications_p.cpp
)

if( ENABLE_SHARED )

  add_library( base-notifications SHARED ${base_notifications_SOURCES} )
  set_target_properties( base-notifications PROPERTIES VERSION ${BASE_NOTIFICATIONS_VERSION} SOVERSION ${BASE_NOTIFICATIONS_VERSION} )
  install( TARGETS base-notifications DESTINATION ${CMAKE_INSTALL_PREFIX}/lib )

else()

  add_library( base-notifications STATIC ${base_notifications_SOURCES} )

endif()

target_link_libraries( base-notifications ${QT_LIBRARIES} base base-qt )

if( LIBNOTIFY_FOUND )
  target_link_libraries( base-notifications ${LIBNOTIFY_LIBRARIES} )
endif()
