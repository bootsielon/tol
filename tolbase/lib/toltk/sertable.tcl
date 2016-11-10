#/////////////////////////////////////////////////////////////////////////////
# FILE    : sertable.tcl
# PURPOSE : This file contains a namespace called SeriesTable with its
#           functions. It's for managing series tables.
#           It's a child of BayesTable so inherites all its functionalities.
#/////////////////////////////////////////////////////////////////////////////


#/////////////////////////////////////////////////////////////////////////////
  namespace eval ::SeriesTable {
# 
# PURPOSE: The purpose of this namespace is organize code and functions and
#          provides functions to create and manage a series table.
#          A special namespace is created for each instance of Series table.
#
# VARIABLES:
#   tmpOpt  -> temporary array used to store the widgets and variables of the
#              options dialog. 
#   formats -> 
#
#/////////////////////////////////////////////////////////////////////////////
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::SeriesTable::Create {path sergrp} {
#
# PURPOSE: Creates a series table instance
#
# PARAMETERS:
#   path   -> series table path (frame)
#   sergrp -> tol series group to be tabulated
#
#/////////////////////////////////////////////////////////////////////////////
  set this [::BayesTable::Create $path \
      -fillproc "::SeriesTable::Fill $sergrp" -child SeriesTable\
      -OnWriteIni "::SeriesTable::WriteIni" \
      -OnOptionCreate "::SeriesTable::OptionsCreate" \
      -OnOptionDestroy "::SeriesTable::OptionsDestroy" \
      -OnOptionApply "::SeriesTable::OptionsApply"]
  ::BayesTable::SetData $this sergrp $sergrp
  ReadIni   $this
  Configure $this
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::SeriesTable::Fill {sergrp this} {
#
# PURPOSE: Procedure to fill data elements of the table. Gets data from a 
#          group of series and sets them to the table.
#
# PARAMETERS:
#   sergrp -> tol series group (created by toltcl ::tol::create command)
#   this   -> Instance of BayesTable
#
#/////////////////////////////////////////////////////////////////////////////
  # insert the series into the table
  set numCols [$sergrp serie size]
  set numRows [$sergrp dating size]
  ::BayesTable::SetElement $this 0 0 [$sergrp dating name]
  for {set i 0} {$i < $numRows} {incr i} {
    ::BayesTable::SetElement $this [expr $i+1] 0 [GetTickLabel $sergrp $i]
  }
  for {set j 0} {$j < $numCols} {incr j} {
    ::BayesTable::SetElement $this 0 [expr $j+1] [$sergrp serie $j name]
    foreach vec_data [$sergrp serie $j data] {
      set x [lindex $vec_data 0]
      set y [lindex $vec_data 1]
      for {set i 0} {$i < [llength $x]} {incr i} {
        ::BayesTable::SetElement $this [expr round([lindex $x $i])+1]\
                   [expr $j+1] [lindex $y $i]
      }
    }
  }
  ::BayesTable::SetMatrixInfo $this\
                [list [expr $numRows+1] [expr $numCols+1]] ""
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::SeriesTable::GetTickLabel { group tick } {
#
# PURPOSE:
#
# PARAMETERS:
#   group ->
#   tick  ->
#
#/////////////////////////////////////////////////////////////////////////////
  set str ""
  catch {
    set str [$group dating $tick]
  } 
  return $str
}


#/////////////////////////////////////////////////////////////////////////////
# OPTIONS
#/////////////////////////////////////////////////////////////////////////////

#/////////////////////////////////////////////////////////////////////////////
  proc ::SeriesTable::OptionsDestroy {this} {
#
# PURPOSE: Called before options dialog is destroyed. This procedure is called
#          by parent BayesTable. Actually doesn't do anything.
#
# PARAMETERS:
#   this -> Instance of BayesTable
#
#/////////////////////////////////////////////////////////////////////////////

}

#/////////////////////////////////////////////////////////////////////////////
  proc ::SeriesTable::OptionsCreate {this ts} {
#
# PURPOSE: Creates especific options of a series table. It's called by
#          BayesTable when general table options are created.
#
# PARAMETERS:
#   this -> Instance of BayesTable
#   ts   -> tabset (=>NoteBook) in which options must be created
#
#/////////////////////////////////////////////////////////////////////////////
  #insertar tabset   
  set num_i 1
  set lab_i Series
  $ts insert $num_i $lab_i -text "  [mc $lab_i]  " -background \#d9d9d9 
  set tab_i [$ts getframe $lab_i]
  $tab_i configure -borderwidth 2 -background \#d9d9d9  
  set f [frame $tab_i.f]
  pack $tab_i.f -fill both -expand yes
    
  set w_tabset [NoteBook $f.nb -tabbevelsize 8 -tabpady {2 6} -font {Arial 8}] 
  set num_i 0
  set lab_i "Date Format"
  $w_tabset insert $num_i $lab_i -text "  [mc $lab_i]  " -background \#d9d9d9 
  set tab_i [$w_tabset getframe $lab_i]
  $tab_i configure -borderwidth 2 -background \#d9d9d9
  set f1 [frame $tab_i.f]
  pack $tab_i.f -fill both -expand yes  
  $w_tabset raise [$w_tabset pages 0]    

  OptionsGet $this
  OptionsCreateDateFormat $this $f1
  OptionsCheck $this
  OptionsSet   $this

  $w_tabset compute_size 
  grid $w_tabset -sticky news  
  grid rowconfigure    $f 0 -weight 1
  grid columnconfigure $f 0 -weight 1
}


#///////////////////////////////////////////////////////////////////////////// 
  proc ::SeriesTable::OptionsCreateDateFormat {this w} {
#
# PURPOSE: Creates a frame with date format options
#
# PARAMETERS:
#   this -> Instance of BayesTable
#   w    -> widget's path to allocate the frame
#
#///////////////////////////////////////////////////////////////////////////// 
  variable tmpOpt
  variable formats

  # X-Axis
  TitleFrame $w.tf1 -text [mc "Format String"]
  set f1 [$w.tf1 getframe]

  set formats [list \
     [mc "TOL Long Format"]   "%cy%Ym%md%d%uh%hi%is%s" \
     [mc "TOL Short Format"]  "%cy%Ym%md%d" \
     "Completo YYYY/MM/dd"    "%c%w %Y/%m/%d" \
     "Completo dd/MM/YYYY"    "%c%w %d/%m/%Y" \
     "Fecha larga"            "%c%W, %d de %N de %Y" \
     "Completo dd/Month/YYYY" "%c%w %d/%n/%Y" \
     [mc "User Defined"]]
  foreach {cad for} $formats {
    lappend lstCad $cad
  }

  label $f1.lFormat -text [mc Format]:
  ComboBox $f1.cbFormat -width 30 -height 6 -editable false -values $lstCad\
      -textvariable ::SeriesTable::tmpOpt(var,dateFormat)\
      -modifycmd [list ::SeriesTable::OptionsCheckDateFormat $this]
  label $f1.lString -text [mc String]:
  set tmpOpt(widgets,dateString) [Entry $f1.eString -width 45 -state disabled\
         -textvariable ::SeriesTable::tmpOpt(var,dateString)]
  bind $f1.eString <KeyRelease> [list ::SeriesTable::OptionsCheckDateString $this]
  label $f1.lView   -text [mc Sample]:
  set tmpOpt(widgets,dateView) [Entry $f1.eView   -width 45 -state disabled\
         -textvariable ::SeriesTable::tmpOpt(var,dateView)]
  button $f1.bHelp  -text [mc "Format Specifiers"]\
     -command [list ::SeriesGraph::ShowSpecifiers $w]
  frame $f1.f -width 25
  grid $f1.f $f1.lFormat $f1.cbFormat -sticky nw -padx 2 -pady 2
  grid ^     $f1.lString $f1.eString  -sticky nw -padx 2 -pady 2
  grid ^     $f1.lView   $f1.eView    -sticky nw -padx 2 -pady 2
  grid ^     $f1.bHelp   -            -column 1    -sticky nw -pady 2
  grid rowconfigure    $f1 3 -weight 1
  grid columnconfigure $f1 2 -weight 1
  grid $w.tf1 -sticky news -padx 2
  grid rowconfigure    $w 0 -weight 1
  grid columnconfigure $w 0 -weight 1
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::SeriesTable::OptionsGet {this} {
#
# PURPOSE: Initializes all options variables and stores them into the
#          temporary 'tmpOpt' array. 
#
# PARAMETERS:
#   this   -> Instance of BayesTable
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpOpt

  set tmpOpt(var,dateString)   [::BayesTable::GetOption $this dateString]
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::SeriesTable::OptionsSet {this} {
#
# PURPOSE: Commits all options values from the temporary 'tmpOpt' array to
#          other structure.
#
# PARAMETERS:
#   this   -> Instance of BayesTable
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpOpt

  regsub -all var, [array names ::SeriesTable::tmpOpt var,*] "" optNames
  foreach it $optNames {
    ::BayesTable::SetOption $this dateString $tmpOpt(var,$it)
  }
}


#///////////////////////////////////////////////////////////////////////////// 
  proc ::SeriesTable::OptionsApply {this} {
#
# PURPOSE: Applies options from the options dialog. It's called by BayesTable
#          when general table options are applied.
#
# PARAMETERS:
#   this  -> Instance of BayesTable
#
#///////////////////////////////////////////////////////////////////////////// 
  variable tmpOpt
  OptionsApplyDateFormat $this $tmpOpt(var,dateString)
  #Save options after apply
  OptionsSet $this
}


proc ::SeriesTable::OptionsApplyDateFormat {this dateString} {
  set sergrp [::BayesTable::GetData $this sergrp]
  $sergrp format $dateString
  ::SeriesTable::Fill $sergrp $this
}


proc ::SeriesTable::OptionsCheck {this} {
  OptionsCheckDateString $this
  #OptionsCheckDateFormat $this
}

proc ::SeriesTable::OptionsCheckDateFormat {this} {
  variable tmpOpt
  variable formats

  if [string equal $tmpOpt(var,dateFormat) [mc "User Defined"]] {
    $tmpOpt(widgets,dateString) configure -state normal
    #set tmpOpt(var,dateString) ""
  } else  {
    $tmpOpt(widgets,dateString) configure -state disabled
    set idx [lsearch -exact $formats $tmpOpt(var,dateFormat)]
    set tmpOpt(var,dateString) [lindex $formats [expr $idx+1]]
    #Coger la fecha del sistema, aplicarle el formato y mostrarla
    set tmpOpt(var,dateView) [::tol::date now $tmpOpt(var,dateString)]
  }
}

proc ::SeriesTable::OptionsCheckDateString {this} {
  variable tmpOpt
  variable formats

  if {[set idx [lsearch -exact $formats $tmpOpt(var,dateString)]] != -1} {
    set tmpOpt(var,dateFormat) [lindex $formats [expr $idx-1]]
  } else  {
    set tmpOpt(var,dateFormat) [lindex $formats end]
    $tmpOpt(widgets,dateString) configure -state normal
  }
  set tmpOpt(var,dateView) [::tol::date now $tmpOpt(var,dateString)]
}


#/////////////////////////////////////////////////////////////////////////////
# INIFILE
#/////////////////////////////////////////////////////////////////////////////

#/////////////////////////////////////////////////////////////////////////////
  proc ::SeriesTable::ReadIni {this} {
#
# PURPOSE: Reads series table specific options from .ini file. First of all,
#          reads options from BayesTable. 
#
# PARAMETERS:
#   this  -> Instance of BayesTable
#
#/////////////////////////////////////////////////////////////////////////////
  # General options (parent)
  ::BayesTable::ReadIni $this
  # Specific options
  set rini ::iniFile::Read
  set sergrp [::BayesTable::GetData $this sergrp]
  ::BayesTable::SetOption $this dateString \
     [$rini SeriesTable dateString [$sergrp format]]
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::SeriesTable::WriteIni {this} {
#
# PURPOSE: Writes series table specific options to .ini file. This procedure
#          is called by parent BayesTable. 
#
# PARAMETERS:
#   this  -> Instance of BayesTable
#
#/////////////////////////////////////////////////////////////////////////////
  set wini ::iniFile::Write
  $wini SeriesTable dateString  [::BayesTable::GetOption $this dateString]
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::SeriesTable::Configure {this} {
#
# PURPOSE: Configures some options of Series Table. First of all, configures
#          options from BayesTable. 
#
# PARAMETERS:
#   this  -> Instance of BayesTable
#
#/////////////////////////////////////////////////////////////////////////////
  # General options (parent)
  ::BayesTable::Configure $this
  # Specific options
  OptionsApplyDateFormat $this [::BayesTable::GetOption $this dateString]
}
