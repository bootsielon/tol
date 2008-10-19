# toltcl.m4 - TOLTCL M4 Macros. -*-Autoconf-*-
## Copyright 2007 Free Software Foundation, Inc.
## Written by Daniel Rus <rus.daniel@gmail.com>, 2007
##
## This file is free software; the Free Software Foundation gives
## unlimited permission to copy and/or distribute it, with or without
## modifications, as long as this notice is preserved.#

#------------------------------------------------------------------------
# _SET_WITH_PACKAGE_OPTIONS_ --
#
#	Auxiliar macro used by TOLTCL_REQUIRE_PACKAGE
#	Create configure script options: --with-..
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
AC_DEFUN([_SET_WITH_PACKAGE_OPTIONS_],[
  # create --with-$1, --with-$1-includes, and --with-$1-libraries options
  AC_ARG_WITH([$1],AC_HELP_STRING([--with-$1=DIR],[DIR where to find $2 package]),[$1_dir=$withval],[$1_dir=""])
  AC_ARG_WITH([$1-includes],AC_HELP_STRING([--with-$1-includes=DIR],[DIR where to find $2 headers]),[$1_inc_dir=$withval],[$1_inc_dir=""])
  AC_ARG_WITH([$1-libraries],AC_HELP_STRING([--with-$1-libraries=DIR],[DIR where to find $2 libraries]),[$1_lib_dir=$withval],[$1_lib_dir=""])
])

#------------------------------------------------------------------------
# _CHECK_PACKAGE_HEADER_ --
#
#	Auxiliar macro used by TOLTCL_REQUIRE_PACKAGE.
#
#	Checks for the existence of the header given as 3th parameter.
#
#       If the header file is found, shows a success message, otherwise
#       it will show an error message and stop the script proccess.
#
# Arguments (same as TOLTCL_REQUIRE_PACKAGE):
#	1st: short package name (i.e.: dmalloc).
#       2nd: long package name (i.e.: Debug Malloc).
#       3th: one package's header file to look for (i.e.: dmalloc.h).
#       4th: short library name used with flag -l (i.e.: dmalloc).
#
# Results:
#	Defines the following configure cache variable:
#         ac_cv_header_$1      Will be "yes" if header file $3 was found.
#
#	Creates the following vars:
#	  $1_INC_DIR    Directory path where header file was found.
#------------------------------------------------------------------------
AC_DEFUN([_CHECK_PACKAGE_HEADER_],[
  AC_CACHE_VAL(ac_cv_header_$1,[
    if test "x${$1_dir}" != "x" ; then
      AC_MSG_CHECKING([for $3 in ${$1_dir}/include])
      if test -r "${$1_dir}/include/$3" ; then
        $1_INC_DIR="${$1_dir}/include"
        ac_cv_header_$1=yes
        AC_MSG_RESULT([yes])
      else 
        AC_MSG_RESULT([no])
      fi
    elif test "x${$1_inc_dir}" != "x" ; then
      AC_MSG_CHECKING([for $3 in $1_inc_dir])
      if test -r "${$1_inc_dir}/$3" ; then
        $1_INC_DIR="${$1_inc_dir}"
        ac_cv_header_$1=yes
        AC_MSG_RESULT([yes])
      else
        AC_MSG_RESULT([no])
      fi
    else
      AC_MSG_CHECKING([for $3])
      for dir in /usr/local/include /usr/local /usr/include /usr
      do
        if test -r "${dir}/$3" ; then
          $1_INC_DIR="${dir}"
          ac_cv_header_$1=yes
          break
        fi
      done
      if test "${ac_cv_header_$1}" = "yes" ; then
        AC_MSG_RESULT([yes])
      else
        AC_MSG_RESULT([no])
      fi
    fi
  ])

  if test "${ac_cv_header_$1}" != "yes" ; then
    AC_MSG_ERROR([Cannot find header $3 (package: $2)])
  fi
])

#------------------------------------------------------------------------
# _CHECK_PACKAGE_LIB_ --
#
#	Auxiliar macro used by TOLTCL_REQUIRE_PACKAGE.
#
#	Checks for the existence of the library given as 4th parameter.
#
#       If the library is found, shows a success message, otherwise
#       it will show an error message and stop the script proccess.
##
# Arguments (same as TOLTCL_REQUIRE_PACKAGE):
#	1st: short package name (i.e.: dmalloc).
#       2nd: long package name (i.e.: Debug Malloc).
#       3th: one package's header file to look for (i.e.: dmalloc.h).
#       4th: short library name used with flag -l (i.e.: dmalloc).
#
# Results:
#	Defines the following configure cache variable:
#         ac_cv_lib_$1      Will be "yes" if library $4 was found.
#
#	Creates the following vars:
#	  $1_LIB_DIR    Directory path where library was found.
#         $1_LIB        Library's file name.
#------------------------------------------------------------------------
AC_DEFUN([_CHECK_PACKAGE_LIB_],[
  AC_CACHE_VAL(ac_cv_lib_$1,[
    if test "x${$1_dir}" != "x" ; then
      AC_MSG_CHECKING([for lib$4.so in ${$1_dir}/lib])
      if test -r "${$1_dir}/lib/lib$4.so" ; then
        $1_LIB_DIR="${$1_dir}/lib"
        ac_cv_lib_$1=yes
        AC_MSG_RESULT([yes])
      else
        AC_MSG_RESULT([no])
      fi
    elif test "x${$1_lib_dir}" != "x" ; then
      AC_MSG_CHECKING([for lib$4.so in $1_dir/lib])
      if test -r "${$1_lib_dir}/lib$4.so" ; then
        $1_LIB_DIR="${$1_lib_dir}"
        ac_cv_lib_$1=yes
        AC_MSG_RESULT([yes])
      else
        AC_MSG_RESULT([no])
      fi
    else
      AC_MSG_CHECKING([for lib$4.so])
      for dir in /usr/local/lib /usr/local /usr/lib /usr
      do
        if test -r "${dir}/lib$4.so" ; then
          $1_LIB_DIR="${dir}"
          ac_cv_lib_$1=yes
          break
        fi
      done
      if test "${ac_cv_lib_$1}" = "yes" ; then
        AC_MSG_RESULT([yes])
      else
        AC_MSG_RESULT([no])
      fi
    fi
  ])

  if test "${ac_cv_lib_$1}" != "yes" ; then
    AC_MSG_ERROR([Cannot find library lib$4.so (package: $2)])
  fi
])

#------------------------------------------------------------------------
# TOLTCL_REQUIRE_PACKAGE --
#
#       This macro creates several configure script options to activate 
#       the use of an specified package. It looks for the given library and 
#       header file. If the package is found it adds the corresponding entries
#       to the compilation environment used by Tcl, using the M4 Macros that
#       comes with TEA Std (in tcl.m4 file).
#       
#    Uses the following auxiliar macros:
#       _SET_WITH_PACKAGE_OPTIONS_  -> to create --with-$1-... options
#       _CHECK_PACKAGE_LIB_         -> to look for the given library package 
#       _CHECK_PACKAGE_HEADER_      -> to look for the header file    
#
# Arguments:
#	1st: short package name (i.e.: dmalloc).
#       2nd: long package name (i.e.: Debug Malloc).
#       3th: one package's header file to look for (i.e.: dmalloc.h).
#       4th: short library name used with flag -l (i.e.: dmalloc).
#
# Results:
#	Defines the following configure cache variable:
#         ac_cv_package_$1      Will be "yes" if package was found
#------------------------------------------------------------------------
AC_DEFUN([TOLTCL_REQUIRE_PACKAGE],[
  # first create configure script options --with-blabla...
  _SET_WITH_PACKAGE_OPTIONS_($@)
  AC_LANG([C])

  _CHECK_PACKAGE_LIB_($@)
    
  if test "$3" != "" ; then
    _CHECK_PACKAGE_HEADER_($@)
    ifelse([$1_INC_DIR],[],[],TEA_ADD_INCLUDES([-I${$1_INC_DIR}]))
  fi

  if test "${ac_cv_lib_$1}" = "yes" ; then
    AC_CACHE_VAL(ac_cv_$1,[ac_cv_$1=yes])
    ifelse([$1_LIB_DIR],[],[],TEA_ADD_LIBS([-L${$1_LIB_DIR} -Wl,-R,${$1_LIB_DIR}]))
    TEA_ADD_LIBS([-l$4])
  fi
])
