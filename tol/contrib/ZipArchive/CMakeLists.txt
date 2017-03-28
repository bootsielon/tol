include(ExternalProject)

ExternalProject_Add(prj_zipa
  URL "${CMAKE_CURRENT_SOURCE_DIR}/ZipArchive_Bayes.tar.gz"
  INSTALL_COMMAND "")

ExternalProject_Get_Property(prj_zipa BINARY_DIR SOURCE_DIR)

message(STATUS "BINARY_DIR = ${BINARY_DIR}")
message(STATUS "SOURCE_DIR = ${SOURCE_DIR}")

add_library(ziparchive STATIC IMPORTED GLOBAL)
set_target_properties(ziparchive PROPERTIES IMPORTED_LOCATION "${BINARY_DIR}/libZipArchive.a")

set(ZIPARCHIVE_INCLUDE_DIR "${SOURCE_DIR}/ZipArchive" CACHE INTERNAL "" FORCE)
set(ZIPARCHIVE_LIBRARY "ziparchive" CACHE INTERNAL "" FORCE)
