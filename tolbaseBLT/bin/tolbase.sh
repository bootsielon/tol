#!/bin/bash
# Shell wrapper for TOL executable.

SCRIPT=$(readlink -f "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
echo $SCRIPTPATH

if [ -f ${SCRIPTPATH}/tolenv.sh ] ; then
  source ${SCRIPTPATH}/tolenv.sh
else
  echo "${SCRIPTPATH}/tolenv.sh not found, running without environment"
fi

#TFILE="/tmp/$(basename $0).$$.tmp.tcl"
#echo lappend auto_path file normalize [file join [ file dir [info script] ] .. lib ] ]
#echo package require TolbaseApp > ${TFILE}
#echo after idle {file delete ${TFILE}} >> ${TFILE}

${SCRIPTPATH}/tolbase ${SCRIPTPATH}/tolbase.tcl "${@}"
