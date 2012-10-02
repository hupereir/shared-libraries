MACRO( ADD_WIN32_EXECUTABLE target version )

	### executable
	ADD_EXECUTABLE( ${target} WIN32 ${ARGN} ${target}_win32.rc )

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
