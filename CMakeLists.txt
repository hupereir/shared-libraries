# $Id$
PROJECT( BASE_SPELLCHECK )
SET( BASE_SPELLCHECK_VERSION_MAJOR 1 )
SET( BASE_SPELLCHECK_VERSION_MINOR 0 )
SET( BASE_SPELLCHECK_VERSION_PATCH 0 )
SET( BASE_SPELLCHECK_VERSION ${BASE_SPELLCHECK_VERSION_MAJOR}.${BASE_SPELLCHECK_VERSION_MINOR}.${BASE_SPELLCHECK_VERSION_PATCH} )
SET( BASE_SPELLCHECK_SOVERSION ${BASE_SPELLCHECK_VERSION_MAJOR}.${BASE_SPELLCHECK_VERSION_MINOR} )

########### Qt4 configuration #########
FIND_PACKAGE( Qt4 REQUIRED )

INCLUDE( ${QT_USE_FILE} )
ADD_DEFINITIONS( ${QT_DEFINITIONS} )

########### aspell ###############
FIND_PACKAGE( ASPELL REQUIRED )
INCLUDE_DIRECTORIES(${ASPELL_INCLUDE_DIR})

########### includes ###############
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/base)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/base-qt)

########### links #########
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
LINK_DIRECTORIES( ${CMAKE_INSTALL_PREFIX}/lib )

########### next target ###############
SET( base_spellcheck_SOURCES
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

SET( base_spellcheck_HEADERS
  DictionaryMenu.h
  DictionarySelectionButton.h
  FilterMenu.h
  FilterSelectionButton.h
  SpellCheckConfiguration.h
  SpellDialog.h
  SpellItemDialog.h
  SuggestionMenu.h
)

QT4_WRAP_CPP( base_spellcheck_HEADERS_MOC ${base_spellcheck_HEADERS} )

IF( ENABLE_SHARED )

  ADD_LIBRARY( base-spellcheck SHARED
    ${base_spellcheck_SOURCES}
    ${base_spellcheck_HEADERS_MOC}
  )

  SET_TARGET_PROPERTIES( base-spellcheck PROPERTIES VERSION ${BASE_SPELLCHECK_VERSION} SOVERSION ${BASE_SPELLCHECK_SOVERSION} )
  TARGET_LINK_LIBRARIES( base-spellcheck ${QT_LIBRARIES} ${ASPELL_LIBRARIES} base base-qt )
  INSTALL( TARGETS base-spellcheck DESTINATION ${CMAKE_INSTALL_PREFIX}/lib )

ELSE()

 ADD_LIBRARY( base-spellcheck STATIC
    ${base_spellcheck_SOURCES}
    ${base_spellcheck_HEADERS_MOC}
  )

  TARGET_LINK_LIBRARIES( base-spellcheck ${QT_LIBRARIES} ${ASPELL_LIBRARIES} base base-qt )

ENDIF()
