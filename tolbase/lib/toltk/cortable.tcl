#/////////////////////////////////////////////////////////////////////////////
# FILE    : cortable.tcl
# PURPOSE : This file contains a namespace called AutocorTable with its
#           functions. It's for managing series autocorrelation data tables.
#           It's a child of BayesTable so inherites all its functionalities.
#/////////////////////////////////////////////////////////////////////////////


#/////////////////////////////////////////////////////////////////////////////
  namespace eval ::AutocorTable {
#
# PURPOSE: The purpose of this namespace is organize code and functions and
#          provides functions to create and manage a serie autocorrelation
#          data table.
#          A special namespace is created for each instance of autocor table.
#
#/////////////////////////////////////////////////////////////////////////////
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::AutocorTable::Create {path serie {nLag ""}} {
#
# PURPOSE:
#
# PARAMETERS:
#
#/////////////////////////////////////////////////////////////////////////////
  set this [::BayesTable::Create $path\
     -fillproc "::AutocorTable::Fill {$serie} {$nLag}" -child AutocorTable]
  ReadIni   $this
  Configure $this
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::AutocorTable::Fill {serie nLag this} {
#
# PURPOSE:
#
# PARAMETERS:
#
#/////////////////////////////////////////////////////////////////////////////
  # insert the autocor data into the table
  set typecor {ACF PACF IACF IPACF}

  set numCols [llength $typecor]
  if {$nLag == ""} {
    set numRows [lindex [::tol::autocor [lindex $typecor 0] $serie] 1]
  } else {
    set numRows $nLag
  }
    
  for {set i 0} {$i < $numRows} {incr i} {
    ::BayesTable::SetElement $this [expr $i+1] 0 [expr $i+1]
  }
  for {set j 0} {$j < $numCols} {incr j} {
    ::BayesTable::SetElement $this 0 [expr $j+1] [lindex $typecor $j]
    set values [eval ::tol::autocor [lindex $typecor $j] {$serie} $nLag]
    for {set i 0} {$i < $numRows} {incr i} {
      ::BayesTable::SetElement $this [expr $i+1] [expr $j+1] \
             [lindex [lindex $values 3] $i]
    }
  }
  ::BayesTable::SetMatrixInfo $this \
         [list [expr $numRows+1] [expr $numCols+1]] ""
}


#/////////////////////////////////////////////////////////////////////////////
# INIFILE
#/////////////////////////////////////////////////////////////////////////////

#/////////////////////////////////////////////////////////////////////////////
  proc ::AutocorTable::ReadIni {this} {
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
  proc ::AutocorTable::WriteIni {this} {
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
  proc ::AutocorTable::Configure {this} {
#
# PURPOSE: Configures some options of Autocor Table. First of all, configures
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

