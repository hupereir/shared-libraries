# $Id$
project(BASE_SPELLCHECK)
set(BASE_SPELLCHECK_VERSION_MAJOR 1)
set(BASE_SPELLCHECK_VERSION_MINOR 0)
set(BASE_SPELLCHECK_VERSION_PATCH 0)
set(BASE_SPELLCHECK_VERSION ${BASE_SPELLCHECK_VERSION_MAJOR}.${BASE_SPELLCHECK_VERSION_MINOR}.${BASE_SPELLCHECK_VERSION_PATCH})
set(BASE_SPELLCHECK_SOVERSION ${BASE_SPELLCHECK_VERSION_MAJOR}.${BASE_SPELLCHECK_VERSION_MINOR})

########### Qt4 configuration #########
if(USE_QT5)

  find_package(Qt5Widgets REQUIRED)
  find_package(Qt5Network REQUIRED)

else()

  find_package(Qt4 REQUIRED)
  include(${QT_USE_FILE})
  add_definitions(${QT_DEFINITIONS})

endif()

########### aspell ###############
find_package(ASPELL REQUIRED)
include_directories(${ASPELL_INCLUDE_DIR})

########### includes ###############
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/base)
include_directories(${CMAKE_SOURCE_DIR}/base-qt)

########### links #########
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
link_directories(${CMAKE_INSTALL_PREFIX}/lib)

########### next target ###############
set(base_spellcheck_SOURCES
  AutoSpellConfiguration.cpp
  DictionaryMenu.cpp
  DictionarySelectionButton.cpp
  FilterMenu.cpp
  FilterSelectionButton.cpp
  SpellCheckConfiguration.cpp
  SpellDialog.cpp
  SpellInterface.cpp
  SpellItemDialog.cpp
  SpellItemModel.cpp
  SpellParser.cpp
  SuggestionMenu.cpp
  TexString.cpp
 )

if(ENABLE_SHARED)

  add_library(base-spellcheck SHARED ${base_spellcheck_SOURCES})
  set_target_properties(base-spellcheck PROPERTIES VERSION ${BASE_SPELLCHECK_VERSION} SOVERSION ${BASE_SPELLCHECK_SOVERSION})
  install(TARGETS base-spellcheck DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

else()

  add_library(base-spellcheck STATIC ${base_spellcheck_SOURCES})

endif()

target_link_libraries(base-spellcheck ${QT_LIBRARIES} ${ASPELL_LIBRARIES} base base-qt)

if(USE_QT5)

  qt5_use_modules(base-spellcheck Widgets Network)

endif()