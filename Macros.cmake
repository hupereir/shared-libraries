# $Id$
MACRO( GET_BUILD_TIMESTAMP RESULT )

  # find date
  FIND_PROGRAM(DATE_PROGRAM date)
  IF( DATE_PROGRAM )

    IF( WIN32 )

      EXECUTE_PROCESS( COMMAND "cmd" " /C ${DATE_PROGRAM} /T" OUTPUT_VARIABLE DATE )
      STRING( REGEX REPLACE "(..)[/.](..)[/.](....).*" "\\3/\\2/\\1" DATE ${DATE} )
      SET( ${RESULT} "${DATE}" )
      MESSAGE( "-- Build timeStamp: ${${RESULT}}" )

    ELSEIF(UNIX)

      EXECUTE_PROCESS( COMMAND "${DATE_PROGRAM}" "+%Y/%m/%d" OUTPUT_VARIABLE DATE )
      STRING( REGEX REPLACE "\n+$" "" ${RESULT} ${DATE} )
      MESSAGE( "-- Build timeStamp: ${${RESULT}}" )

    ENDIF()

  ENDIF()

ENDMACRO()
