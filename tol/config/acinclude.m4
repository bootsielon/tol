# acinclude.m4 - some aditional macro definition for gnu_toll
#
# Copyright 1996, 1997, 1998, 1999, 2000, 2001, 2002
# Free Software Foundation, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.

# TOLC_PROG_GCC_VERSION
#-------------------
 
dnl @synopsis TOLC_PROG_GCC_VERSION(version, [action-if-ok], [action-if-not-ok])
dnl
dnl Check whether the current version of gcc is greater or equal than version
dnl number given in TOL_PROG_GCC_VERSION. 
dnl
dnl action-if-ok is a list of shell commands to run if the current version
dnl is greater or equal than version specified.
dnl
dnl action-if-not-ok is a list of shell commands to run if the current version
dnl is not greater or equal than version specified
dnl
dnl  Example of use:
dnl  TOL_PROG_GCC_VERSION(3.0, [
dnl	 EXTRALIB="-lsupc++"
dnl	 AC_MSG_NOTICE([Adding libsupc++ to extralib path])
dnl  ])
dnl
dnl @version 1.0
dnl @author Daniel Rus <daniel@uponeits.com>
dnl
AC_DEFUN([TOL_PROG_GCC_VERSION],[dnl
ac_gcc_req_version="$1"
ac_gcc_req_major=`echo $ac_gcc_req_version | cut -d . -f 1`
ac_gcc_req_minor=`echo $ac_gcc_req_version | cut -d . -f 2`
if test "x${CC-cc}" != "x"; then
  AC_MSG_CHECKING(for gcc version greater than or equal to $ac_gcc_req_version)
  ac_gcc_cur_version=`${CC-cc} -dumpversion 2> /dev/null`
  ac_gcc_cur_major=`echo $ac_gcc_cur_version | cut -d . -f 1`
  ac_gcc_cur_minor=`echo $ac_gcc_cur_version | cut -d . -f 2`
  if test "$ac_gcc_cur_major" -gt "$ac_gcc_req_major" ; then
    AC_MSG_RESULT(ok);
    $2
  elif test "$ac_gcc_cur_major" -eq "$ac_gcc_req_major" && \
       test "$ac_gcc_cur_minor" -gt "$ac_gcc_req_minor" ; then
    AC_MSG_RESULT(ok);
    $2
  else  
    AC_MSG_RESULT(no);
    $3
  fi
else
  AC_MSG_WARN(could not find ${CC-cc})
fi
])dnl

# TOL_LIB_GSL_VERSION
#-------------------
 
dnl @synopsis TOL_LIB_GSL_VERSION(version, [action-if-ok], [action-if-not-ok])
dnl
dnl Check whether the current version of gcc is greater or equal than version
dnl number given in TOL_LIB_GSL_VERSION. 
dnl
dnl action-if-ok is a list of shell commands to run if the current version
dnl is greater or equal than version specified.
dnl
dnl action-if-not-ok is a list of shell commands to run if the current version
dnl is not greater or equal than version specified
dnl
dnl  Example of use:
dnl  TOL_LIB_GSL_VERSION(1.4, ${GSL-CONFIG_BIN_PATH}, 
dnl	 [AC_MSG_NOTICE([GSL Library version... Ok])],
dnl	 [AC_MSG_ERROR([Your GSL Library version is older than 1.4])]
dnl  )
dnl
dnl @version 1.0
dnl @author Daniel Rus <daniel@uponeits.com>
dnl
AC_DEFUN([TOL_LIB_GSL_VERSION],[dnl
tol_gsl_req_version="$1"
tol_gsl_req_major=`echo $tol_gsl_req_version | cut -d . -f 1`
tol_gsl_req_minor=`echo $tol_gsl_req_version | cut -d . -f 2`
if test "x$2" != "x"; then
  AC_MSG_CHECKING(for gsl version greater than or equal to $tol_gsl_req_version)
  tol_gsl_cur_version=`$2 --version 2> /dev/null`
  tol_gsl_cur_major=`echo $tol_gsl_cur_version | cut -d . -f 1`
  tol_gsl_cur_minor=`echo $tol_gsl_cur_version | cut -d . -f 2 | cut -d + -f 1`
  if test "$tol_gsl_cur_major" -gt "$tol_gsl_req_major" ; then
    AC_MSG_RESULT(ok);
    $3
  elif test "$tol_gsl_cur_major" -eq "$tol_gsl_req_major" && \
       test "$tol_gsl_cur_minor" -gt "$tol_gsl_req_minor" ; then
    AC_MSG_RESULT(ok);
    $3
  elif test "$tol_gsl_cur_major" -eq "$tol_gsl_req_major" && \
       test "$tol_gsl_cur_minor" -eq "$tol_gsl_req_minor" ; then
    AC_MSG_RESULT(ok);
    $3
  else  
    AC_MSG_RESULT(no);
    $4
  fi
else
  AC_MSG_WARN(could not find $2)
fi
])dnl
