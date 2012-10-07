# $Id$
MACRO( GET_BUILD_TIMESTAMP RESULT )

  IF( WIN32 AND ${RELEASE} )

    EXECUTE_PROCESS( COMMAND "cmd" " /C ${DATE_PROGRAM} /T" OUTPUT_VARIABLE DATE )
    STRING( REGEX REPLACE "(..)[/.](..)[/.](....).*" "\\3/\\2/\\1" DATE ${DATE} )
    SET( ${RESULT} "${DATE}" )
    MESSAGE( "-- Build timeStamp: ${${RESULT}}" )

  ELSEIF( UNIX )

    EXECUTE_PROCESS( COMMAND "date" "+%Y/%m/%d" OUTPUT_VARIABLE DATE )
    STRING( REGEX REPLACE "\n+$" "" ${RESULT} ${DATE} )
    MESSAGE( "-- Build timeStamp: ${${RESULT}}" )

  ENDIF()

ENDMACRO()
