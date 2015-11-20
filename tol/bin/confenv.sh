#!/bin/sh
# create tolenv.sh
#

echo "$# arguments provided"

check_installed () {
  Rscript -e "suppressMessages(cat(if( '$1' %in% installed.packages() ) {1} else {0}))"
}

check_tol_program () {
  if [ -x $1/$2 ]; then
    echo "$1/$2 checked"
  else
    echo $1/$2 not exists
    exit
  fi
}

if [ $# -eq 1 ]; then
  TOL_PREFIX=$1
  TOLENV=$1/bin/tolenv.sh
elif [ $# -eq 2 ]; then
  TOL_PREFIX=$1
  TOLENV=$2
else
  echo "usage: $0 TOL_PREFIX_DIR ?tolenv.sh?"
  exit
fi

check_tol_program $TOL_PREFIX bin/tolcon
check_tol_program $TOL_PREFIX bin/tolsh

# look for R
#

if  command R RHOME >/dev/null 2>/dev/null ; then
  echo "R can be invoked"
else
  echo "R cannot be invoked"
  exit
fi

R_HOME_DIR=$(R RHOME)
Rcpp_INSTALLED=$(check_installed Rcpp)
Rinside_INSTALLED=$(check_installed RInside)

if [ $Rcpp_INSTALLED -eq 1 ] ; then
  echo "Rcpp is installed"
else
  echo "Rcpp is not installed"
fi

if [ $Rinside_INSTALLED -eq 1 ] ; then
  echo "RInside is installed"
  RINSIDE_PATH=$(Rscript -e "suppressMessages(cat(system.file(package='RInside')))")
else
  echo "RInside is not installed"
fi

_LD_PATH=$TOL_PREFIX/lib:$R_HOME_DIR/lib
if [ x"$RINSIDE_PATH" != x ]; then
 _LD_PATH=$_LD_PATH:$RINSIDE_PATH/lib
fi

cat << EOF > $TOLENV
# environment needed for tol processes
#
LD_LIBRARY_PATH=$_LD_PATH:\$LD_LIBRARY_PATH
export LD_LIBRARY_PATH
export R_HOME=$R_HOME_DIR
EOF

