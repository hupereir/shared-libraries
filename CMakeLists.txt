PROJECT( SHARED_LIBRARIES )
CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

########### aspell ###############
FIND_PACKAGE( ASPELL )

################
# Subdirectories
################
ADD_SUBDIRECTORY( base )
ADD_SUBDIRECTORY( base-qt )
ADD_SUBDIRECTORY( base-server )
ADD_SUBDIRECTORY( base-help )

IF( ASPELL_FOUND )
  ADD_SUBDIRECTORY( base-spellcheck )
ENDIF()

ADD_SUBDIRECTORY( base-transparency )
ADD_SUBDIRECTORY( base-svg )
