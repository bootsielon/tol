message(STATUS HELLO)
if( CMAKE_SYSTEM_NAME MATCHES "Linux" )
  execute_process(COMMAND uname "-m"
    OUTPUT_VARIABLE SPECIFIC_SYSTEM_ARCHITECTURE
    OUTPUT_STRIP_TRAILING_WHITESPACE )
  execute_process(COMMAND lsb_release "-i"
    OUTPUT_VARIABLE LSB_RELEASE_ID
    OUTPUT_STRIP_TRAILING_WHITESPACE )
  string(REGEX MATCH "Distributor ID:(.+)" LSB_Distributor_ID "${LSB_RELEASE_ID}")
  string(STRIP ${CMAKE_MATCH_1} SPECIFIC_SYSTEM_NAME)
  execute_process(COMMAND lsb_release "-r"
    OUTPUT_VARIABLE LSB_RELEASE_R
    OUTPUT_STRIP_TRAILING_WHITESPACE )
  string(REGEX MATCH "Release:(.+)" LSB_Release "${LSB_RELEASE_R}")
  string(STRIP ${CMAKE_MATCH_1} SPECIFIC_SYSTEM_RELEASE)
  if(SPECIFIC_SYSTEM_NAME MATCHES "CentOS")
    set( SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR "RPM")
  elseif(SPECIFIC_SYSTEM_NAME MATCHES "Fedora")
    set( SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR "RPM")
  elseif(SPECIFIC_SYSTEM_NAME MATCHES "SUSE")
    set( SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR "RPM")
  elseif(SPECIFIC_SYSTEM_NAME MATCHES "Ubuntu")
    set(SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR "DEB")
  else(SPECIFIC_SYSTEM_NAME MATCHES "CentOS")
    set( SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR "TGZ" )
  endif(SPECIFIC_SYSTEM_NAME MATCHES "CentOS")
  
  message("SPECIFIC_SYSTEM_ARCHITECTURE=${SPECIFIC_SYSTEM_ARCHITECTURE}")
  message("SPECIFIC_SYSTEM_NAME=${SPECIFIC_SYSTEM_NAME}")
  message("SPECIFIC_SYSTEM_RELEASE=${SPECIFIC_SYSTEM_RELEASE}")
endif( CMAKE_SYSTEM_NAME MATCHES "Linux" )

