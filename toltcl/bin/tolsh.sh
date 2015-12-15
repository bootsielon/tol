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

exec ${SCRIPTPATH}/tolsh "${@}"
