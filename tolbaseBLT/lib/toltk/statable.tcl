#/////////////////////////////////////////////////////////////////////////////
# FILE    : statable.tcl
# PURPOSE : This file contains a namespace called StatisticsTable with its
#           functions. It's for managing series statistics tables.
#           It's a child of BayesTable so inherites all its functionalities.
#/////////////////////////////////////////////////////////////////////////////


#/////////////////////////////////////////////////////////////////////////////
  namespace eval ::StatisticsTable {
#
# PURPOSE: The purpose of this namespace is organize code and functions and
#          provides functions to create and manage a series statistics table.
#          A special namespace is created for each instance of statitstics
#          table.
#
#/////////////////////////////////////////////////////////////////////////////
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::StatisticsTable::Create {path sergrp} {
#
# PURPOSE:
#
# PARAMETERS:
#
#/////////////////////////////////////////////////////////////////////////////
  set this [::BayesTable::Create $path \
    -fillproc "::StatisticsTable::Fill $sergrp" -child StatisticsTable]
  ReadIni   $this
  Configure $this
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::StatisticsTable::Fill {sergrp this} {
#
# PURPOSE:
#
# PARAMETERS:
#
#/////////////////////////////////////////////////////////////////////////////
  # insert the series into the table
  set numCols [$sergrp serie size]
  set list  [lindex [::tol::seriestat [$sergrp serie 0 reference]] 1]
  foreach {name value} $list {
    lappend stat $name
  }
  set numRows [llength $stat]
  #::BayesTable::SetElement $this 0 0 [mc "Statistics"]
  for {set i 0} {$i < $numRows} {incr i} {
    ::BayesTable::SetElement $this [expr $i+1] 0 [mc [lindex $stat $i]]
  }

  for {set j 0} {$j < $numCols} {incr j} {
    ::BayesTable::SetElement $this 0 [expr $j+1] [$sergrp serie $j name]
    set values [lindex [::tol::seriestat [$sergrp serie $j reference]] 1]
    for {set i 0} {$i < $numRows} {incr i} {
      ::BayesTable::SetElement $this [expr $i+1] [expr $j+1] \
                [lindex $values [expr 2*$i+1]]
    }
  }
  ::BayesTable::SetMatrixInfo $this \
          [list [expr $numRows+1] [expr $numCols+1]] ""
}


#/////////////////////////////////////////////////////////////////////////////
# INIFILE
#/////////////////////////////////////////////////////////////////////////////

#/////////////////////////////////////////////////////////////////////////////
  proc ::StatisticsTable::ReadIni {this} {
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
  proc ::StatisticsTable::WriteIni {this} {
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
  proc ::StatisticsTable::Configure {this} {
#
# PURPOSE: Configures some options of Statistics Table. First of all,
#          configures options from BayesTable. Actually there isn't specific
#          options.
#
# PARAMETERS:
#   this  -> Instance of BayesTable
#
#/////////////////////////////////////////////////////////////////////////////
  # General options (parent)
  ::BayesTable::Configure $this
  # Specific options
}
