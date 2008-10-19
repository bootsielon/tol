# bys.m4 - BYs M4 Macros. -*-Autoconf-*-
## Copyright 2008 Free Software Foundation, Inc.
## Written by Daniel Rus Morales <daniel@uponeits.com>
##
## This file is free software; the Free Software Foundation gives
## unlimited permission to copy and/or distribute it, with or without
## modifications, as long as this notice is preserved.#

#------------------------------------------------------------------------
# BYs_ENABLE_DEBUG --
#
#	Set the --enable-debug configure script option, and depending on
#       its use set CFLAGS for debugging and optimizing.
#
# Arguments:
#	none
#
# Results:
#	Set CFLAGS variable.
#------------------------------------------------------------------------
AC_DEFUN([BYs_ENABLE_DEBUG],[
  AC_ARG_ENABLE([debug],AC_HELP_STRING([--enable-debug],[turn on debugging]),[enable_debug=1],[enable_debug=0])

  if test "${enable_debug}" == "1" ; then
    CFLAGS="-ggdb"
    CXXFLAGS="-ggdb"
  else
    CFLAGS="-O2"
    CXXFLAGS="-O2"
  fi
])

#------------------------------------------------------------------------
# BYs_SET_ARG_WITH --
#
#	Creates configure script options: --with-..
#
# Arguments:
#	1st: short package name (i.e.: dmalloc).
#       2nd: long package name (i.e.: Debug Malloc).
#
# Results:
#	Adds the following arguments to configure:
#
#       --with-$1=DIR  
#          Specify the directory where to find the package files, libraries 
#          under DIR/lib, headers under DIR/include. 
#          (i.e.: --with-dmalloc=/home/me/local)
#
#       --with-$1-includes=DIR
#          Specify the directory where to find the package $1 headers.
#          (i.e.: --with-dmalloc-includes=/home/me/local/include)
#
#       --with-$1-libraries=DIR
#          Specify the directory where to find the package $1 libraries.
#          (i.e.: --with-dmalloc-libraries=/home/me/local/lib)
#
#	Defines the following vars:
#         $1_dir        Directory path given at --with-$1=<DIR>
#         $1_inc_dir    Directory path given at --with-$1-includes=<DIR>
#         $1_lib_dir    Directory path given at --with-$1-libraries=<DIR>
#------------------------------------------------------------------------
AC_DEFUN([BYs_SET_ARG_WITH],[
  # create --with-$1, --with-$1-includes, and --with-$1-libraries options
  AC_ARG_WITH([$1],AC_HELP_STRING([--with-$1=DIR],[DIR where to find $2 package]),[$1_dir=$withval],[$1_dir=""])
  AC_ARG_WITH([$1-includes],AC_HELP_STRING([--with-$1-includes=DIR],[DIR where to find $2 headers]),[$1_inc_dir=$withval],[$1_inc_dir=""])
  AC_ARG_WITH([$1-libraries],AC_HELP_STRING([--with-$1-libraries=DIR],[DIR where to find $2 libraries]),[$1_lib_dir=$withval],[$1_lib_dir=""])
])

#------------------------------------------------------------------------
# BYs_CHECK_HEADER --
#
#	Checks for the existence of the header given as 2nd parameter.
#       First of all try to use AC_CHECK_HEADER. If it doesn't success, try
#       to use directories of --with-... options.
#
#       If the header file is found, shows a success message, otherwise
#       it shows an error message and stop the script proccessing.
#
# Arguments:
#	1st: short package name, same as used with -l flag (i.e.: dmalloc).
#       2nd: package's header file to look for (i.e.: dmalloc.h).
#       3th: long package name (i.e.: Debug Malloc).
#
# Results:
#	Defines the following configure cache variable:
#         ac_cv_header_$1      being "yes" if header file $2 is found.
#
#	SUBST the following vars:
#	  $1_CPPFLAGS    Preprocessor flags needed.
#------------------------------------------------------------------------
AC_DEFUN([BYs_CHECK_HEADER],[
  AC_CACHE_VAL(ac_cv_header_$1,[
    AC_CHECK_HEADER($2,[ac_cv_header_$1=yes],[
      if test "x${$1_dir}" != "x" ; then
        AC_MSG_CHECKING([for $2 in ${$1_dir}/include])
        if test -r "${$1_dir}/include/$2" ; then
          $1_INC_DIR="${$1_dir}/include"
          ac_cv_header_$1=yes
        fi
        AC_MSG_RESULT([${ac_cv_header_$1}])
      elif test "x${$1_inc_dir}" != "x" ; then
        AC_MSG_CHECKING([for $2 in ${$1_inc_dir}])
        if test -r "${$1_inc_dir}/$2" ; then
          $1_INC_DIR="${$1_inc_dir}"
          ac_cv_header_$1=yes
        fi
        AC_MSG_RESULT([${ac_cv_header_$1}])
      else
        AC_MSG_CHECKING([for $2 in /usr/include or /usr/local/include])
        if test -r "/usr/include/$2" -o \
                -r "/usr/local/include/$2"; then
          ac_cv_header_$1=yes
        fi
        AC_MSG_RESULT([${ac_cv_header_$1}])
      fi
    ])
  ])

  if test "${ac_cv_header_$1}" != "yes" ; then
    AC_MSG_ERROR([Cannot find header $2 (package: $3)])
  fi

  if test "x${$1_INC_DIR}" != "x" ; then
    $1_CPPFLAGS="-I${$1_INC_DIR}"
  fi
  AC_SUBST([$1_CPPFLAGS])
])

#------------------------------------------------------------------------
# BYs_CHECK_LIB --
#
#	Checks for the existence of the library given in the 1st parameter.
#       First of all try to use AC_CHECK_LIB with 1st and 2nd parameter.
#       If it doesn't success tries using directories given --with-... options.
#
#       If the library is found, shows a success message, otherwise
#       it shows an error message and stop the script proccessing.
##
# Arguments (same as SS_CHECK_PACKAGE):
#       1st: short library name used with flag -l (i.e.: dmalloc).
#       2nd: function to try to link with (i.e.: malloc).
#       3th: long package name (i.e.: Debug Malloc).
#
# Results:
#	Defines the following configure cache variable:
#         ac_cv_lib_$1      Will be "yes" if library $3 was found.
#
#	SUBST the following vars:
#	  $1_LDFLAGS    Linker flags needed.
#------------------------------------------------------------------------
AC_DEFUN([BYs_CHECK_LIB],[
  AC_CACHE_VAL(ac_cv_lib_$1,[
    AC_CHECK_LIB([$1],[$2],[ac_cv_lib_$1=yes],[
      if test "x${$1_dir}" != "x" ; then
        AC_MSG_CHECKING([for lib$1.so in ${$1_dir}/lib])
        if test -r "${$1_dir}/lib/lib$1.so" ; then
          $1_LIB_DIR="${$1_dir}/lib"
          ac_cv_lib_$1=yes
        fi
        AC_MSG_RESULT([$ac_cv_lib_$1])
      elif test "x${$1_lib_dir}" != "x" ; then
        AC_MSG_CHECKING([for lib$1.so in ${$1_lib_dir}])
        if test -r "${$1_lib_dir}/lib$1.so" ; then
          $1_LIB_DIR="${$1_lib_dir}"
          ac_cv_lib_$1=yes
        fi
        AC_MSG_RESULT([${ac_cv_lib_$1}])
      else
        AC_MSG_CHECKING([for lib$1.so in /usr/lib or /usr/local/lib])
        if test -r "/usr/lib/lib$1.so" -o \
                -r "/usr/local/lib/lib$1.so"; then
          ac_cv_lib_$1=yes
        fi
        AC_MSG_RESULT([${ac_cv_lib_$1}])
      fi
    ])
  ])

  if test "${ac_cv_lib_$1}" != "yes" ; then
    AC_MSG_ERROR([Cannot find library $1 (package: $3)])
  fi

  if test "x${$1_LIB_DIR}" != "x" ; then
    $1_LDFLAGS="-L${$1_LIB_DIR} -l$1"
  else
    $1_LDFLAGS="-l$1"
  fi
  AC_SUBST([$1_LDFLAGS])
])
