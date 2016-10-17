# tolbase.m4 --
#
#	This file provides a set of autoconf macros to help Tolbase
#	auto configuration. It is based on tcl.m4 file developed by
#       Ajuba Solutions (Copyright (c) 1999-2000) and
#       ActiveState Corporation (Copyright (c) 2002-2003).
#
# Copyright (c) 2004, Bayes Decision, SL (Madrid [EU])
#
# See the file "COPYING" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#

AC_PREREQ(2.50)

#------------------------------------------------------------------------
# TB_CHECK_TCLPKG --
#	Locate the pkgIndex.tcl file of a package given as argument
#
# Arguments:
#	Name of Tcl packages searched
#
# Results:
#       no  - the specified package not exist
#       yes - the package is reacheable by Tcl interpreter
#------------------------------------------------------------------------

AC_DEFUN([TB_CHECK_TCLPKG], [
    #
    # This macro try to load the package given as argument.
    # tclsh conf.tcl test package_name has to return
    # "1" if package_name exist in the target system or "0" 
    # if not exist or can not be loaded
    #
    AC_REQUIRE([TEA_INIT])
    AC_REQUIRE([TEA_PROG_TCLSH])
    AC_MSG_CHECKING($1 Tcl package)

    #tb_chk_tclpkg=`${TCLSH_PROG} \`${CYGPATH}\` ${srcdir}/conf.tcl test $1`
    tb_chk_tclpkg=`tclsh ${srcdir}/conf.tcl test $1`

    if test "$tb_chk_tclpkg" == "yes" ; then
      AC_MSG_RESULT(yes)
    else
      AC_MSG_RESULT(no)
      $2
    fi
])
