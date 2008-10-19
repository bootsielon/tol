# toldbd.m4 - Tol-DB-Drivers M4 Macros. -*-Autoconf-*-
## Copyright 2007 Free Software Foundation, Inc.
## Written by Daniel Rus Morales <danirus@tol-project.org>
##
## This file is free software; the Free Software Foundation gives
## unlimited permission to copy and/or distribute it, with or without
## modifications, as long as this notice is preserved.#

#------------------------------------------------------------------------
# TOLDBD_ENABLE_DEBUG --
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
AC_DEFUN([TOLDBD_ENABLE_DEBUG],[
  AC_ARG_ENABLE([debug],AC_HELP_STRING([--enable-debug],[turn on debugging]),[enable_debug=1],[enable_debug=0])

  if test "${enable_debug}" == "1" ; then
    CFLAGS="-g -O0 ${CFLAGS}"
  else
    CFLAGS="-O2 ${CFLAGS}"
  fi
])

#------------------------------------------------------------------------
# _SET_ENABLE_PACKAGE_OPTION_ --
#
#	Auxiliar macro used by TOLDBD_ENABLE_PACKAGE and TOLDBD_REQUIRE_PACKAGE
#	Create configure script options --enable-.. and --with-...
#
# Arguments:
#	1st: short package name (i.e.: dmalloc).
#       2nd: long package name (i.e.: Debug Malloc).
#
# Results:
#	Adds the following arguments to configure:
#
#	--enable-$1    
#          Activate the use of package $1. (i.e.: --enable-dmalloc)
#
#	Defines the following vars:
#	  $1_enable     Set to 1 whether --enable-$1 option is used
#------------------------------------------------------------------------
AC_DEFUN([_SET_ENABLE_PACKAGE_OPTION_],[
  # create --enable-$1 (being $1==pkgname)
   AC_ARG_ENABLE([$1],AC_HELP_STRING([--enable-$1],[turn on $2 package support]),[enable_$1=1],[enable_$1=0])
])


#------------------------------------------------------------------------
# _SET_WITH_PACKAGE_OPTIONS_ --
#
#	Auxiliar macro used by TOLDBD_REQUIRE_PACKAGE
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
# _CHECK_PACKAGE_LIB_ --
#
#	Auxiliar macro used by TOLDBD_CHECK_PACKAGE.
#
#	Checks for the existence of the library given as 3th parameter.
#       First of all try to use AC_CHECK_LIB, with 3th and 4th parameter.
#       If it doesn't success, try to use directories of --with-... options.
#
#       If the library is found, shows a success message, otherwise
#       it will show an error message and stop the script proccess.
##
# Arguments (same as TOLDBD_CHECK_PACKAGE):
#	1st: short package name (i.e.: dmalloc).
#       2nd: long package name (i.e.: Debug Malloc).
#       3th: short library name used with flag -l (i.e.: dmalloc).
#       4th: function to try to link with (i.e.: malloc).
#
# Results:
#	Defines the following configure cache variable:
#         ac_cv_lib_$1      Will be "yes" if library $3 was found.
#
#	SUBST the following vars:
#	  $1_LIB_DIR    Directory path where library was found.
#         $1_LIB        Library's file name.
#------------------------------------------------------------------------
AC_DEFUN([_CHECK_PACKAGE_LIB_],[
  AC_CACHE_VAL(ac_cv_lib_$1,[
    AC_CHECK_LIB([$3],[$4],[ac_cv_lib_$1=yes],[
      if test "x${$1_dir}" != "x" ; then
        AC_MSG_CHECKING([for lib$3.so in ${$1_dir}/lib])
        if test -r "${$1_dir}/lib/lib$3.so" ; then
          $1_LIB_DIR="${$1_dir}/lib"
          AC_SUBST($1_LIB_DIR)
          ac_cv_lib_$1=yes
          AC_MSG_RESULT([yes])
        fi
        AC_MSG_RESULT([no])
      elif test "x${$1_lib_dir}" != "x" ; then
        AC_MSG_CHECKING([for lib$3.so in $1_dir/lib])
        if test -r "${$1_lib_dir}/lib$3.so" ; then
          $1_LIB_DIR="${$1_lib_dir}"
          AC_SUBST($1_LIB_DIR)
          ac_cv_lib_$1=yes
          AC_MSG_RESULT([yes])
        fi
        AC_MSG_RESULT([no])
      fi
    ])
  ])

  if test "${ac_cv_lib_$1}" != "yes" ; then
    AC_MSG_ERROR([Cannot find library $3 (package: $2)])
  fi

  $1_LIB="$3"
  AC_SUBST($1_LIB)
])

#------------------------------------------------------------------------
# _CHECK_PACKAGE_HEADER_ --
#
#	Auxiliar macro used by TOLDBD_CHECK_PACKAGE.
#
#	Checks for the existence of the header given as 5th parameter.
#       First of all try to use AC_CHECK_HEADER. If it doesn't success, try
#       to use directories of --with-... options.
#
#       If the header file is found, shows a success message, otherwise
#       it will show an error message and stop the script proccess.
#
# Arguments (same as TOLDBD_CHECK_PACKAGE):
#	1st: short package name (i.e.: dmalloc).
#       2nd: long package name (i.e.: Debug Malloc).
#       3th: short library name used with flag -l (i.e.: dmalloc).
#       4th: function to try to link with (i.e.: malloc).
#       5th: one package's header file to look for (i.e.: dmalloc.h).
#
# Results:
#	Defines the following configure cache variable:
#         ac_cv_header_$1      Will be "yes" if header file $5 was found.
#
#	SUBST the following vars:
#	  $1_INC_DIR    Directory path where header file was found.
#------------------------------------------------------------------------
AC_DEFUN([_CHECK_PACKAGE_HEADER_],[
  AC_CACHE_VAL(ac_cv_header_$1,[
    AC_CHECK_HEADER($5,[ac_cv_header_$1=yes],[
      if test "x${$1_dir}" != "x" ; then
        AC_MSG_CHECKING([for $5 in ${$1_dir}/include])
        if test -r "${$1_dir}/include/$5" ; then
          $1_INC_DIR="${$1_dir}/include"
          AC_SUBST($1_INC_DIR)
          ac_cv_header_$1=yes
          AC_MSG_RESULT([yes])
        else 
          AC_MSG_RESULT([no])
        fi
      elif test "x${$1_inc_dir}" != "x" ; then
        AC_MSG_CHECKING([for $5 in $1_inc_dir])
        if test -r "${$1_inc_dir}/$5" ; then
          $1_INC_DIR="${$1_inc_dir}"
          AC_SUBST($1_INC_DIR)
          ac_cv_header_$1=yes
          AC_MSG_RESULT([yes])
        else
          AC_MSG_RESULT([no])
        fi
      fi
    ])
  ])

  if test "${ac_cv_header_$1}" != "yes" ; then
    AC_MSG_ERROR([Cannot find header $5 (package: $2)])
  fi
])

#------------------------------------------------------------------------
# TOLDBD_ENABLE_PACKAGE --
#
#       This macro creates several configure script options to activate 
#       the use of an specified package. First it checks if the option
#       --enable-pkgname was given and looks for the library and header
#       file specified as parameters. If the package was found it'd create
#       SUBST variables for the header directory, the library directory, and
#       the library name, that could be used in Makefile.am files.
#       
#    Uses the following auxiliar macros:
#       _SET_ENABLE_PACKAGE_OPTION_ -> to create --enable-$1 option
#       _SET_WITH_PACKAGE_OPTIONS_  -> to create --with-$1-... options
#       _CHECK_PACKAGE_LIB_         -> to look for the given library package 
#       _CHECK_PACKAGE_HEADER_      -> to look for the header file    
#
# Arguments:
#	1st: short package name (i.e.: dmalloc).
#       2nd: long package name (i.e.: Debug Malloc).
#       3th: short library name used with flag -l (i.e.: dmalloc).
#       4th: function to try to link with (i.e.: malloc).
#       5th: one package's header file to look for (i.e.: dmalloc.h).
#       6th: if $1 exist define $6 as a PREPROC MACRO.
#       7th: if $1 does not exist define $7 as a PREPROC MACRO.
#
# Results:
#	Defines the following configure cache variable:
#         ac_cv_package_$1      Will be "yes" if package was found
#------------------------------------------------------------------------
AC_DEFUN([TOLDBD_ENABLE_PACKAGE],[
  # first create configure script options --enable and --with-blabla...
  _SET_ENABLE_PACKAGE_OPTION_($@)
  _SET_WITH_PACKAGE_OPTIONS_($@)
  AC_LANG([C])

  # if they were used, start looking for the package
  if test "${enable_$1}" = "1" ; then
    _CHECK_PACKAGE_LIB_($@)
    
    if test "$5" != "" ; then
      _CHECK_PACKAGE_HEADER_($@)
    fi
      
    if test "${ac_cv_lib_$1}" = "yes" ; then
      AC_CACHE_VAL(ac_cv_$1,[ac_cv_$1=yes])
      if test "$6" != "" ; then
        AC_DEFINE([$6])
      fi
    fi

  elif test "$7" != "" ; then
    AC_DEFINE($7)
  fi

  AM_CONDITIONAL([$1_ENABLED],[test "${ac_cv_$1}" = "yes"])
  AM_CONDITIONAL([$1_LIB_DIR],[test "x${$1_LIB_DIR}" != "x"])
  AM_CONDITIONAL([$1_INC_DIR],[test "x${$1_INC_DIR}" != "x"])
])
