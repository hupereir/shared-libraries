# $Id$
MACRO( GET_BUILD_TIMESTAMP RESULT )

  IF( WIN32 )

    EXECUTE_PROCESS( COMMAND "cmd" " /C date /T" OUTPUT_VARIABLE DATE )
    STRING( REGEX REPLACE ".*(..)[/.](..)[/.](....).*" "\\3/\\2/\\1" DATE ${DATE} )
    SET( ${RESULT} "${DATE}" )

  ELSEIF( UNIX )

    EXECUTE_PROCESS( COMMAND "date" "+%Y/%m/%d" OUTPUT_VARIABLE DATE )
    STRING( REGEX REPLACE "\n+$" "" ${RESULT} ${DATE} )

  ENDIF()

ENDMACRO()

###################### Install Win32 application #########################
MACRO( ADD_WIN32_EXECUTABLE target version )

  ### executable
  ADD_EXECUTABLE( ${target} WIN32 ${ARGN} )
  SET( BIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/bin" )

  ### use static gcc linking
  IF( ${CMAKE_COMPILER_IS_GNUCXX} )
    SET_TARGET_PROPERTIES( ${target} PROPERTIES LINK_FLAGS " -static-libgcc -s" )
  ENDIF()

  ### get target location
  GET_TARGET_PROPERTY( TARGET_PATH ${target} LOCATION )

  ### Compress target
  FIND_PROGRAM( UPX upx )
  IF( UPX )

    ADD_CUSTOM_COMMAND( TARGET ${target} POST_BUILD COMMAND ${UPX} ${TARGET_PATH} )

  ELSE()

    MESSAGE( "-- Program 'upx' not found" )

  ENDIF()

  ### copy to version release
  IF( RELEASE )

    SET( TARGET_RELEASE ${target}-${version} )
    ADD_CUSTOM_COMMAND(
      TARGET ${target} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${TARGET_PATH} ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_RELEASE}.exe
    )

    INSTALL(
      PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_RELEASE}.exe
      DESTINATION ${CMAKE_INSTALL_PREFIX}/release )

  ENDIF()

ENDMACRO( ADD_WIN32_EXECUTABLE )

###################### Install Apple application #########################
MACRO( ADD_APPLE_EXECUTABLE target version )

  ### version
  SET( MACOSX_BUNDLE_SHORT_VERSION_STRING ${version} )
  SET( MACOSX_BUNDLE_VERSION ${version})
  SET( MACOSX_BUNDLE_LONG_VERSION_STRING "Version ${version}" )

  ### installation directory
  SET( BIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}" )
  ADD_EXECUTABLE( ${target} MACOSX_BUNDLE ${ARGN} )

ENDMACRO( ADD_APPLE_EXECUTABLE )

###################### Install unix application #########################
MACRO( ADD_UNIX_EXECUTABLE target )

  ### executable
  ADD_EXECUTABLE( ${target} ${ARGN} )
  SET( BIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/bin" )

  ### unix specific links
  TARGET_LINK_LIBRARIES( ${target} ${X11_X11_LIB} )

ENDMACRO( ADD_UNIX_EXECUTABLE )

###################### Install application #########################
MACRO( ADD_PLATFORM_EXECUTABLE target version )

  IF( WIN32 )

    ADD_WIN32_EXECUTABLE( ${target} ${version} ${ARGN} )

  ELSEIF( APPLE )

    ADD_APPLE_EXECUTABLE( ${target} ${version} ${ARGN} )

  ELSE()

    ADD_UNIX_EXECUTABLE( ${target} ${ARGN} )

  ENDIF()

ENDMACRO( ADD_PLATFORM_EXECUTABLE )


###################### add application icon for Apple and Windows #########################
MACRO( ADD_APPLICATION_ICON  sources icon )

  IF( WIN32 )

    SET( _icon ${icon}.ico )
    IF( EXISTS "${_icon}" )

      MESSAGE( "-- Using application icon: ${_icon}" )
      STRING( REPLACE _SOURCES "" appname ${sources} )
      SET( _resource ${CMAKE_CURRENT_BINARY_DIR}/${appname}_win32.rc )
      FILE( WRITE ${_resource} "IDI_ICON1        ICON        DISCARDABLE    \"${_icon}\"\n")
      LIST( APPEND ${sources} ${_resource} )

    ELSE()

      MESSAGE("-- Unable to find icon ${_icon}" )

    ENDIF()

  ENDIF()

  IF( APPLE )

    SET( _icon ${icon}.icns )
    IF( EXISTS "${_icon}" )

      MESSAGE( "-- Using application icon: ${_icon}" )
      SET( MACOSX_BUNDLE_ICON_FILE icon.icns )
      SET_SOURCE_FILES_PROPERTIES( ${_icon} PROPERTIES MACOSX_PACKAGE_LOCATION Resources )
      LIST( APPEND ${sources} ${_icon} )

    ELSE()

      MESSAGE("-- Unable to find icon ${_icon}" )

    ENDIF()

  ENDIF()

ENDMACRO( ADD_APPLICATION_ICON )
