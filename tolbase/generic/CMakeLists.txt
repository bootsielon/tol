add_definitions( -DTCL_THREADS -DTOLBASE_VERSION=\"${TOLBASE_VERSION_MAJOR}.${TOLBASE_VERSION_MINOR}\")
include_directories( ${TCL_INCLUDE_PATH} )
 
message( STATUS "Using TCL_INCLUDE_PATH = ${TCL_INCLUDE_PATH}" )
message( STATUS "Using TCL_LIBRARY = ${TCL_LIBRARY}" )
message( STATUS "Using TK_LIBRARY = ${TK_LIBRARY}" )

if ( WIN32 )
  set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -mwindows" )
endif( WIN32 )

set(RES_FILES "")
if(MINGW)
 set(RES_FILES "tolbase-gnu.rc")
 set(CMAKE_RC_COMPILER_INIT windres)
 ENABLE_LANGUAGE(RC)
 SET(CMAKE_RC_COMPILE_OBJECT
 "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
endif(MINGW)

add_executable( tolbase tolbase.c ${RES_FILES})

target_link_libraries( tolbase ${TCL_LIBRARY} ${TK_LIBRARY} )

message( STATUS "TOLBASE_DEST_DIR = ${TOLBASE_DEST_DIR}" )
install( TARGETS tolbase RUNTIME DESTINATION "bin" )
