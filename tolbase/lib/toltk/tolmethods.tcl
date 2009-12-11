#/////////////////////////////////////////////////////////////////////////////
# FILE    : tolmethods.tcl
# PURPOSE : Metodos de Tol desde TCL
#/////////////////////////////////////////////////////////////////////////////


# includes. 1 -> Include.  0 -> No
if { 0 } {
  # packages
  package require Tk
  package require BWidget
#  package require BLT
  # 
  wm geometry . 1x1+1+1
  set toltk_script_path [file dirname [info script]]
  # load tclIndex
  set dir $toltk_script_path
  source [file join $toltk_script_path "tclIndex"]
  unset dir
  if { ![info exists toltk_library] } {
    set toltk_library [file join $toltk_script_path ..]
  }
  # messages
  package require msgcat
  namespace import msgcat::*					
  # bitmap
  Bitmap::use
  set toltk_script_path [file dirname [info script]]
  #Tol load images:
  set toltk_images_path [file join ${toltk_script_path} images]
  if { [lsearch $Bitmap::path $toltk_images_path] == -1 } {
    #añade la ruta de las imágenes para que Bitmap sepa donde están:
    lappend Bitmap::path [file join $toltk_script_path images]
  }
}

#/////////////////////////////////////////////////////////////////////////////
# Methods to Serie
#/////////////////////////////////////////////////////////////////////////////

#/////////////////////////////////////////////////////////////////////////////
proc Tol_SerieChartMethod { sergrp fileDest } {
#
# PURPOSE: Hook of Chart method
#
# PARAMETERS:
#   sergrp   -> group of series
#   fileDest -> destination file
#/////////////////////////////////////////////////////////////////////////////
  # cargamos la variable GCFFile de TOL
  set fileGCF [TclGetVar Text GCFFile]
  if {![file exists $fileGCF]} {
    set fileGCF {}
  }  
  # MDI
  set tl [::project::CreateForm \
      -title [mc "Series graph"] \
      -type Graphs \
      -iniconfig SeriesGraph \
      -width 600 -height 200
  ]
  set top [$tl getframe]

  set isphoto [expr {$fileDest ne ""}]
  #Tolcon_Trace "set isphoto [expr {$fileDest ne {}}]"
  set Instance [::SeriesGraph::Create $top.g $sergrp \
                    -photo $isphoto -fileGCF $fileGCF]
  bind $top.g <Destroy> +[list ::tol::seriegrp destroy $sergrp]
  pack $top.g -fill both -expand yes
  update

  ::bayesGraph::GrapOptApplyAxisTicks $Instance ${Instance}::options gr,0
  if {$isphoto} {
    ::bayesGraph::TakePhoto $Instance $fileDest
    after idle "destroy $tl"
  } 
}


#/////////////////////////////////////////////////////////////////////////////
  proc Tol_SerieChart { title source gcfFile geometry filetosave } {
#
# PURPOSE: Paint the graph of a set of series
#
# PARAMETERS:
#   title      -> title for the graph
#   source     -> reference to the set
#   gcfFile    -> a gcf to aply to the graph
#   geometry   -> geometry for the window of the graph
#   filetosave -> if a file name is given the graph is written to disk instead 
#               of displayed
#
#/////////////////////////////////////////////////////////////////////////////
  # GCF
  if {![string length $gcfFile]} {
    set gcfFile [TclGetVar Text GCFFile]
    if {[string length $gcfFile]} {
      eval set gcfFile $gcfFile
    }
  }
  if {![file exists $gcfFile]} {
    set gcfFile {}
  }
  #puts "GCF FILE: $gcfFile"  
  # Title
  set wTitle $title
  if {![string length $title]} {
    set wTitle [mc "Series graph"]
  }

  # Create Group Series
  #puts "Tol_SerieChart: source = $source"
  set nsource [lindex [TclGetVar Set $source] 0]
  set lstSerie {}
  for {set i 1} {$i <= $nsource} {incr i} {
    lappend lstSerie "Set $source $i"
  }
  set sergrp tableserie[clock clicks]
  set tl ""
  if {[llength $lstSerie]} {
      # MDI
    set tl [::project::CreateForm \
      -title $wTitle -type Graphs -iniconfig SeriesGraph -geometry $geometry]
    $tl setgeometry $geometry
    set top [$tl getframe]
    #puts "Tol_SerieChart: tol::seriegrp sergrp = $sergrp, lstSerie = $lstSerie"
    eval ::tol::seriegrp create $sergrp $lstSerie
    # Create
    set isphoto [expr {$filetosave ne ""}]
    set Instance [::SeriesGraph::Create $top.g $sergrp \
      -photo $isphoto -fileGCF $gcfFile -title [list $title]]
    bind $top.g <Destroy> +[list ::tol::seriegrp destroy $sergrp]
    pack $top.g -fill both -expand yes
    update
    # Control Axis
    ::bayesGraph::GrapOptApplyAxisTicks $Instance ${Instance}::options gr,0
    # Save graph ?
    if {$isphoto} {
      ::bayesGraph::TakePhoto $Instance $filetosave
      after idle "destroy $tl"
    }
  } else {
    set msg [mc {No selection information related}] 
    ::TolConsole::HciWriter "<W>${msg}</W>"
  }
  return $tl
}


#/////////////////////////////////////////////////////////////////////////////
proc Tol_SerieTabulate { source {title ""} {geometry {}} } {
#
# PURPOSE: Tabulates a serie in a TolBase table window
#
# PARAMETERS:
#   source   -> the tol serie variable or variables (if a list is given)
#   geometry -> geometry specification
#/////////////////////////////////////////////////////////////////////////////

  # chek type TOL of source
  set gramVar [GetGlobalUniqueName __tabser__]
  ::tol::console eval "Text $gramVar = Grammar($source);"
  set namGra [string trim [TclGetVar Text $gramVar] {"}];#{"}    
  # destroy
  ::tol::console stack release $gramVar    
  # list of series tabulate
  set lstSerie {}    
  switch $namGra {
    Serie   { set lstSerie [ list [ list Serie $source ] ] }
    Set     {
      set nsource [lindex [TclGetVar Set $source] 0]
      for {set i 1} {$i <= $nsource} {incr i} {
        lappend lstSerie [list Set $source $i ]
      }
    }
  }  
  # lstSerie > 0 ?
  set tl {}
  if {[llength $lstSerie]} {
    # create sergrp 
    set sergrp group[clock clicks]
    eval "::tol::seriegrp create $sergrp $lstSerie"
    #title
    if { ![llength $title] } {
      set title [mc "Time series table"]
    }
    # create
    set tl [::project::CreateForm \
        -title $title \
        -iniconfig SeriesTable \
        -type Tables \
        -geometry $geometry
    ]
    $tl setgeometry $geometry    
    set tlf [$tl getframe]
    # configuration
    btable $tlf.t -parent $tl \
                  -posteditcmd [list ::TolConsole::TableRecentUpdate $tlf.t] \
                  -iniconfig btable
    $tlf.t fillSerie $sergrp    
    bind $tlf.t <Destroy> +[list ::tol::seriegrp destroy $sergrp]
    $tl bind <Control-F4> [list $tl kill]
    pack $tlf.t -fill both -expand yes
    update
  }
  return $tl
}

#/////////////////////////////////////////////////////////////////////////////
proc Tol_SerieAutocor { source {title ""} {geometry {}} } {
#
# PURPOSE: Tabulates a serie in a TolBase table window
#
# PARAMETERS:
#   source   -> the tol serie variable or variables (if a list is given)
#   geometry -> geometry specification
#/////////////////////////////////////////////////////////////////////////////

  # name
  if {[llength $source] > 1} {
    set nameSerie [lindex [::tol::info variable $source] 1]
  } else {
    set nameSerie $source
  }  
  #title
  if { ![llength $title] } {
    set title "[mc "Autocorrelation graph"]: $nameSerie"
  }
  # create
  set tl [::project::CreateForm \
    -title $title \
    -iniconfig AutocorGraph \
    -type Graphs \
    -geometry $geometry -helpkey "Tb4GraAco"
  ]
  $tl setgeometry $geometry  
  set tlf [$tl getframe]
  # configuration
  ::AutocorGraph::Create $tlf.g $source
  $tl bind <Control-F4> "$tl kill; break"
  pack $tlf.g -fill both -expand yes
  update
  return $tl  
}

#/////////////////////////////////////////////////////////////////////////////
# Methods to Set
#/////////////////////////////////////////////////////////////////////////////

#/////////////////////////////////////////////////////////////////////////////
proc Tol_SetChartMethod { title source gcfFile geometry filetosave lstNames
                          type lstPairs {holdon ""}} {
#
# PURPOSE: Hook of Chart method
#
# PARAMETERS:
#   title    -> title for the graph
#   source   -> reference to the set
#   gcfFile  -> a gcf to aply to the graph
#   geometry -> geometry for the window of the graph
#   file     -> if a file name is given the graph is written to disk instead of displayed
#   lstNames -> names for the series/points,..
#   type     -> 0 for custom association of data columns
#            -> 1 for all data columns to first one
#            -> 2 for all data columns to generic
#            -> 3 for even data columns to odd ones
#   lstPairs -> when type is 0 pairs is the list of correspondence beetwen index
#               columns to data columns (x-y pairs)
#   holdon   -> tell if the new graph goes into a new context or an existing one
#
#/////////////////////////////////////////////////////////////////////////////
  # GCF
  if {![string length $gcfFile]} {
    set gcfFile [TclGetVar Text GCFFile]
  }
  if {![file exists $gcfFile]} {
    set gcfFile {}
  }
  # Title
  set wTitle $title
  if {![string length $title]} {
    set wTitle [mc "Set graph"]
  }
  # MDI
  if {$holdon ne "" && (![winfo exists $holdon] || [winfo class $holdon] ne "Mdidoc")} {
    Tol_HciWriter "<W>[mc {Graphic requested to be drawn on %s but that graphic context does not exist or is not a valid grafic windows} $holdon]</W>"
    set holdon ""
  } 
  if {[winfo exists $holdon]} {
    set tl $holdon
  } else {
    puts "no existe '$holdon' por eso ... ::project::CreateForm"

    set tl [::project::CreateForm \
                -title $wTitle -type Graphs -iniconfig SetGraph -geometry $geometry]
    $tl setgeometry $geometry
  }
  set top [$tl getframe]
  # if source is a list is interpreted as a set name and indexes accesing a element
  if {0} {
  if { [llength $source] > 1 } {
    set setname [GetUniqueName Set]
    set sourcename [lindex $source 0]
    set strtoeval "Set ${setname} = ${sourcename}"
    for {set i 1} {$i < [llength $source]} {incr i} {
      set elem [lindex $source $i]
      set strtoeval "${strtoeval}\[$elem\]"
    }
    ::tol::console eval $strtoeval
    set source $setname
  }
  }
  set cmd tableset[clock clicks]
  ::tol::tableset create $cmd [linsert $source 0 Set]
  set Instance [::SetGraphDialog::DrawSet $top.g $cmd $source $type $lstPairs \
    $lstNames $gcfFile [list $title]]
  if {$holdon ne ""} {
    ::tol::tableset destroy $cmd
    if { [llength $source] > 1} {
      ::tol::stack release $source
    }
  } else {
    bind $top.g <Destroy> +[list ::tol::tableset destroy $cmd]
    if { [llength $source] > 1} {
      bind $top.g <Destroy> +[list ::tol::stack release $source]
    }
    pack $top.g -fill both -expand yes
    update
    ::bayesGraph::GrapOptApplyAxisTicks $Instance ${Instance}::options gr,0
    if { [string length $filetosave] } {
      ::bayesGraph::TakePhoto $Instance $filetosave
      after idle "destroy $tl"
    }
  }
  return $tl
}


#/////////////////////////////////////////////////////////////////////////////
proc Tol_SetTabulate { set {title ""} {geometry {}} } {
#
# PURPOSE: Tabulates a set in a TolBase table window
#
# PARAMETERS:
#   source   -> the tol set variable
#   geometry -> geometry specification
#/////////////////////////////////////////////////////////////////////////////

  if {[llength $set] > 1} {
    set nameSet [lindex [::tol::info variable $set] 1]
    set setref $set
  } else {
    set nameSet $set
    set setref [ list Set $set ]
  }
  
  if { ![llength $title] } { set title  "[mc "Set table"]: $nameSet" }

  set setgrp group[clock clicks]

  set tl {}
  if { ![ catch { ::tol::tableset create $setgrp $setref } ] } {
    # create
    set tl [::project::CreateForm \
        -title $title \
        -iniconfig SetTable \
        -type Tables \
        -geometry $geometry
    ]
    $tl setgeometry $geometry    
    set tlf [$tl getframe]
    # config
    btable $tlf.t -parent $tl \
                  -posteditcmd [list ::TolConsole::TableRecentUpdate $tlf.t] \
                  -iniconfig btable
    $tlf.t fillSet $setgrp
    pack $tlf.t -fill both -expand yes
    bind $tlf.t <Destroy> +[list ::tol::tableset destroy $setgrp]
    $tl bind <Control-F4> [list $tl kill]
    update
  } else {
    tk_messageBox -type ok -icon warning -message [mc "Set not tabulable"]\
      -parent . -title [mc Warning]
    #    -parent $w_tabset -title [mc Warning]
  }
  return $tl  
}
