# $Id$
project(BASE_SPELLCHECK)
set(BASE_SPELLCHECK_VERSION_MAJOR 1)
set(BASE_SPELLCHECK_VERSION_MINOR 0)
set(BASE_SPELLCHECK_VERSION_PATCH 0)
set(BASE_SPELLCHECK_VERSION ${BASE_SPELLCHECK_VERSION_MAJOR}.${BASE_SPELLCHECK_VERSION_MINOR}.${BASE_SPELLCHECK_VERSION_PATCH})
set(BASE_SPELLCHECK_SOVERSION ${BASE_SPELLCHECK_VERSION_MAJOR}.${BASE_SPELLCHECK_VERSION_MINOR})

########### Qt configuration #########
find_package(Qt5 COMPONENTS Widgets REQUIRED)

########### aspell ###############
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
  SpellCheckConfigurationDialog.cpp
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

target_link_libraries(base-spellcheck ${ASPELL_LIBRARIES} base base-qt)
target_link_libraries(base-spellcheck Qt5::Widgets)
