# $Id$
MACRO( GET_BUILD_TIMESTAMP RESULT )

  IF( WIN32 )

    EXECUTE_PROCESS( COMMAND "cmd" " /C date /T" OUTPUT_VARIABLE DATE )
    STRING( REGEX REPLACE "(..)[/.](..)[/.](....).*" "\\3/\\2/\\1" DATE ${DATE} )
    SET( ${RESULT} "${DATE}" )

  ELSEIF(UNIX)

    EXECUTE_PROCESS( COMMAND "date" "+%Y/%m/%d" OUTPUT_VARIABLE DATE )
    STRING( REGEX REPLACE "\n+$" "" ${RESULT} ${DATE} )

  ELSE()

    MESSAGE( SEND_ERROR "date not implemented" )
    SET( ${RESULT} 000000000000 )

  ENDIF()

ENDMACRO()
