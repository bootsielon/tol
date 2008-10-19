#/////////////////////////////////////////////////////////////////////////////
# FILE    : settable.tcl
# PURPOSE : This file contains a namespace called SetTable with its functions.
#           It's for managing set tables.
#           It's a child of BayesTable so inherites all its functionalities.
#/////////////////////////////////////////////////////////////////////////////

#/////////////////////////////////////////////////////////////////////////////
  namespace eval ::SetTable {
# 
# PURPOSE: The purpose of this namespace is organize code and functions and
#          provides functions to create and manage a set table.
#          A special namespace is created for each instance of Set table.
#
# VARIABLES:
#
#/////////////////////////////////////////////////////////////////////////////

}

#/////////////////////////////////////////////////////////////////////////////
proc ::SetTable::Create {path tableset set {onlywidget no}} {
#/////////////////////////////////////////////////////////////////////////////
  set this [::BayesTable::Create $path \
    -fillproc "::SetTable::Fill [list $tableset]" \
    -child SetTable -onlywidget $onlywidget]
  ReadIni   $this
  Configure $this
}

proc ::SetTable::Fill {tableset this} {


  set numRows [$tableset info datarows]
  set numCols [llength [$tableset info columns]]


  set i_ini 0
  #insert the set into the table
  if {[string equal [lindex [$tableset info header] 0] "row"]} {
    set i_ini 1
    set j 1
    set colsF [expr [llength [$tableset info columns]] - [lindex [$tableset info header] 1] -1]
    foreach lstInfo [lrange [$tableset info columns] 0 $colsF] {
      ::BayesTable::SetElement $this $i_ini $j [lindex $lstInfo 1]
      incr j
    }
  } elseif {[string equal [$tableset info header] "struct"]} {
    set j 1
    foreach lstInfo [$tableset info columns] {
      ::BayesTable::SetElement $this $i_ini $j [lindex $lstInfo 1]
      incr j
    }
  } 
  set i 1
  foreach name [$tableset info rowsnames] {
    ::BayesTable::SetElement $this $i 0 $name
    incr i
  }
  $tableset data apply all [list ::SetTable::FillCell $this [expr $i_ini+1]]

  ::BayesTable::SetMatrixInfo $this [list [expr $numRows+$i_ini+1] [expr $numCols+1]] ""
}

proc ::SetTable::FillCell {this i_ini i j value} {
  ::BayesTable::SetElement $this [expr $i_ini+$i] [expr $j+1] $value
}


#/////////////////////////////////////////////////////////////////////////////
# INIFILE
#/////////////////////////////////////////////////////////////////////////////

#/////////////////////////////////////////////////////////////////////////////
  proc ::SetTable::ReadIni {this} {
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
  proc ::SetTable::WriteIni {this} {
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
  proc ::SetTable::Configure {this} {
#
# PURPOSE: Configures some options of Set Table. First of all, configures
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

