add_definitions( -DTCL_THREADS -DTOLBASE_VERSION=\"${TOLBASE_VERSION_MAJOR}.${TOLBASE_VERSION_MINOR}\")
include_directories( ${TCL_INCLUDE_PATH} )
 
message( STATUS "Using TCL_INCLUDE_PATH = ${TCL_INCLUDE_PATH}" )
message( STATUS "Using TCL_LIBRARY = ${TCL_LIBRARY}" )
message( STATUS "Using TK_LIBRARY = ${TK_LIBRARY}" )

if ( WIN32 )
  set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -mwindows" )
else ( WIN32 )
	# use, i.e. don't skip the full RPATH for the build tree
  SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

  # when building, don't use the install RPATH already
  # (but later on when installing)
  SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 

  SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

  # add the automatically determined parts of the RPATH
  # which point to directories outside the build tree to the install RPATH
  SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

  # the RPATH to be used when installing, but only if it's not a system directory
  LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
  IF("${isSystemDir}" STREQUAL "-1")
     SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
  ENDIF("${isSystemDir}" STREQUAL "-1")
endif( WIN32 )

add_executable( tolbase tolbase.c )
target_link_libraries( tolbase ${TCL_LIBRARY} ${TK_LIBRARY} )

message( STATUS "TOLBASE_DEST_DIR = ${TOLBASE_DEST_DIR}" )
install( TARGETS tolbase RUNTIME DESTINATION "bin" )
