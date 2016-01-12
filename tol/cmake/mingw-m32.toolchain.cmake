set(CMAKE_SYSTEM_NAME Windows)

set(CMAKE_CXX_FLAGS           "${CMAKE_CXX_FLAGS} -m32 -mno-align-double" CACHE STRING "c++ flags")
set(CMAKE_C_FLAGS             "${CMAKE_C_FLAGS} -m32 -mno-align-double"   CACHE STRING "c flags")
set(CMAKE_CXX_FLAGS_DEBUG     "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb"        CACHE STRING "c++ flags in Debug")
set(CMAKE_C_FLAGS_DEBUG       "${CMAKE_C_FLAGS_DEBUG} -O0 -ggdb"          CACHE STRING "c flags in Debug")
set(CMAKE_CXX_FLAGS_RELEASE   "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG -O2"   CACHE STRING "c++ flags in Release")
set(CMAKE_C_FLAGS_RELEASE     "${CMAKE_C_FLAGS_RELEASE} -DNDEBUG -O2"     CACHE STRING "c flags in Release")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -m32 -Wl,--add-stdcall-alias -Wl,--enable-stdcall-fixup" CACHE STRING "linker flags")