set(CMAKE_SYSTEM_NAME Windows)

set( CMAKE_CXX_FLAGS           "${CMAKE_CXX_FLAGS} -m64 -mno-align-double"                 CACHE STRING "c++ flags" )
set( CMAKE_C_FLAGS             "${CMAKE_C_FLAGS} -m64 -mno-align-double"                   CACHE STRING "c flags" )
set( CMAKE_CXX_FLAGS_DEBUG     "${CMAKE_CXX_FLAGS_DEBUG} -m64 -O0 -ggdb -mno-align-double" CACHE STRING "c++ flags in Debug" )
set( CMAKE_C_FLAGS_DEBUG       "${CMAKE_C_FLAGS_DEBUG} -m64 -O0 -ggdb -mno-align-double"   CACHE STRING "c flags in Debug" )
set( CMAKE_CXX_FLAGS_RELEASE   "${CMAKE_CXX_FLAGS_RELEASE} -m64 -O2 -mno-align-double -DNDEBUG"     CACHE STRING "c++ flags in Release" )
set( CMAKE_C_FLAGS_RELEASE     "${CMAKE_C_FLAGS_RELEASE} -m64 -O2 -mno-align-double -DNDEBUG"       CACHE STRING "c flags in Release" )
set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -m64"                         CACHE STRING "linker flags" )