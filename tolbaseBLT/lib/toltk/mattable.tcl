#/////////////////////////////////////////////////////////////////////////////
# FILE    : mattable.tcl
# PURPOSE : This file
# PURPOSE : This file contains a namespace called MatrixTable with its
#           functions. It's for managing matrix tables.
#           It's a child of BayesTable so inherites all its functionalities.
#/////////////////////////////////////////////////////////////////////////////


#/////////////////////////////////////////////////////////////////////////////
  namespace eval ::MatrixTable {
#
# PURPOSE: The purpose of this namespace is organize code and functions and
#          provides functions to create and manage a matrix object table.
#          A special namespace is created for each instance of Matrix table.
#
#/////////////////////////////////////////////////////////////////////////////
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::MatrixTable::Create {path matrix} {
#
# PURPOSE:
#
# PARAMETERS:
#
#/////////////////////////////////////////////////////////////////////////////
  set this [::BayesTable::Create $path\
    -fillproc "::MatrixTable::Fill [list $matrix]" -child MatrixTable] 
  ReadIni   $this
  Configure $this
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::MatrixTable::Fill {refMatrix this} {
#
# PURPOSE:
#
# PARAMETERS:
#
#/////////////////////////////////////////////////////////////////////////////
  set matrix [::tol::matrix $refMatrix]
  # insert the matrix into the table
  set numCols [lindex $matrix 2]
  set numRows [lindex $matrix 1]

  ::BayesTable::SetElement $this 0 0 [lindex $matrix 0]
  for {set j 0} {$j < $numCols} {incr j} {
      ::BayesTable::SetElement $this 0 [expr $j+1] [expr $j+1] 1
  }
  set lst [lindex $matrix 3]
  for {set i 0} {$i < $numRows} {incr i} {
    ::BayesTable::SetElement $this [expr $i+1] 0 [expr $i+1] 1
    for {set j 0} {$j < $numCols} {incr j} {
      ::BayesTable::SetElement $this [expr $i+1] [expr $j+1] \
           [lindex $lst [expr $numCols*$i+$j]]
    }
  }
  ::BayesTable::SetMatrixInfo $this\
          [list [expr $numRows+1] [expr $numCols+1]] ""
}


#/////////////////////////////////////////////////////////////////////////////
# INIFILE
#/////////////////////////////////////////////////////////////////////////////

#/////////////////////////////////////////////////////////////////////////////
  proc ::MatrixTable::ReadIni {this} {
#
# PURPOSE: Reads specific options from .ini file. First of all, reads options
#          from BayesTable. Actually there isn't specific options.
#
# PARAMETERS:
#   this  -> Instance of BayesTable
#
#/////////////////////////////////////////////////////////////////////////////
  # General options (parent)
  ::BayesTable::ReadIni $this
  # Specific options
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::MatrixTable::WriteIni {this} {
#
# PURPOSE: Writes specific options to .ini file. This procedure is called by
#          parent BayesGraph. Actually doesn't do anything.
#
# PARAMETERS:
#   this  -> Instance of BayesTable
#
#/////////////////////////////////////////////////////////////////////////////

}


#/////////////////////////////////////////////////////////////////////////////
  proc ::MatrixTable::Configure {this} {
#
# PURPOSE: Configures some options of Matrix Table. First of all, configures
#          options from BayesTable. Actually there isn't specific options.
#
# PARAMETERS:
#   this  -> Instance of BayesTable
#
#/////////////////////////////////////////////////////////////////////////////
  # General options (parent)
  ::BayesTable::Configure $this
  # Specific options
}

