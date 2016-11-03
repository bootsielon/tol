
#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::DrawSet {setref {type 0}} {
#
# PURPOSE: Graph a Set. For it it calls to a form for the selection of 
#          columns to graph
#
# PARAMETERS:
#   setref -> Set to graph
#   type   -> 0 for custom association of data columns
#          -> 1 for all data columns to first one
#          -> 2 for all data columns to generic
#          -> 3 for even data columns to odd ones
#
#/////////////////////////////////////////////////////////////////////////////
  variable group_id
  variable w_tabset
  #puts "::TolInspector::DrawSet. setref: $setref"
  incr group_id
  if {[llength $setref] > 1} {
    set name [lindex [::tol::info variable $setref] 1]
  } else {
    set name $setref
  }
  set cmd tablecmd$group_id
  if {![catch {::tol::tableset create $cmd $setref}]} {
    switch $type {
      0 {
        set tl [::project::CreateForm \
          -width 500 -height 500 -type Graphs \
          -iniconfig Graph \
          -title [mc "Set graph: %s" $name] -helpkey "Tb4GraSet"]
        $tl withdraw
        set tlf [$tl getframe] 
        if {[set Instance [::SetGraphDialog::Init $tlf.g $cmd $setref]] != 0} {
          bind $tlf.g <Destroy> +[list ::tol::tableset destroy $cmd]
          pack $tlf.g -fill both -expand yes
          $tl restore
          update
          # Esta opción se debe aplicar cuando el gráfico esté empacado
          ::bayesGraph::GrapOptApplyAxisTicks $Instance ${Instance}::options gr,0
        } else {
          destroy $tl
          return
        }
        $tl bind <Control-F4> [list $tl kill]              
      }
      1 -
      2 -
      3 {
        # windows 
        set tl [::project::CreateForm \
          -title [mc "Set graph: %s" $name] -type Graphs -iniconfig SetGraph]
        $tl withdraw        
        set tlf [$tl getframe]
        set Instance [::SetGraphDialog::DrawSet $tlf.g $cmd $setref $type]
        bind $tlf.g <Destroy> +[list ::tol::tableset destroy $cmd]
        pack $tlf.g -fill both -expand yes
        $tl restore
        update
        # Esta opción se debe aplicar cuando el gráfico esté empacado        
        ::bayesGraph::GrapOptApplyAxisTicks $Instance ${Instance}::options gr,0
      }
    }
  } else {
    tk_messageBox -type ok -icon warning \
        -message [mc "Set is not graficable: %s" $::errorInfo] \
        -parent $w_tabset -title [mc "Warning"]
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::DrawMatrix {matref} {
#
# PURPOSE: draw the given matrix
#
# PARAMETERS:
#   matref -> reference to the tol matrix
#
#/////////////////////////////////////////////////////////////////////////////
  variable group_id
  variable w_tabset

  incr group_id
  if {[llength $matref] > 1} {
    set name [lindex [::tol::info variable $matref] 1]
  } else {
    if { ([info exists obj]) } {
      set name $obj
    }
  }
  if { ([info exists name]) } {
    set cmd tablecmd$group_id
    if {![catch {::tol::tablematrix create $cmd $matref}]} {
      set tl [::project::CreateForm \
                  -width 500 -height 500 -type Graphs \
                  -iniconfig Graph \
		  -title [mc "Matrix graph: %s" $name] -helpkey "Tb4Gra"]
      $tl withdraw
    
      set tlf [$tl getframe]
      if {[set Instance [::SetGraphDialog::Init $tlf.g $cmd $matref]] != 0} {
        bind $tlf.g <Destroy> +[list ::tol::tablematrix destroy $cmd]
        pack $tlf.g -fill both -expand yes
	    $tl restore
        update
        # Esta opción se debe aplicar cuando el gráfico esté empacado
        ::bayesGraph::GrapOptApplyAxisTicks $Instance ${Instance}::options gr,0
      } else {
        destroy $tl
        return
      }
      $tl bind <Control-F4> [list $tl kill]
    } else {
      tk_messageBox -type ok -icon warning \
        -message [mc "Matrix is not graficable: %s" $::errorInfo] \
        -parent $w_tabset -title [mc "Warning"]
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::TableSet { set } {
#/////////////////////////////////////////////////////////////////////////////  
  variable w_tabset
  variable group_id

  incr group_id
  if {[llength $set] > 1} {
    set nameSet [lindex [::tol::info variable $set] 1]
  } else {
    set nameSet $set
  }
  set setgrp group$group_id
  if {![catch {::tol::tableset create $setgrp $set}]} {
    # create
    set tl [::project::CreateForm \
        -title    "[mc "Set table"]: $nameSet" \
        -iniconfig SetTable \
        -type Tables -helpkey "Tb4Edt"
    ]
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
        -parent $w_tabset -title [mc Warning]
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::TableMatrix { matrix } {
#/////////////////////////////////////////////////////////////////////////////
  set mattb [::tol::matrix $matrix]
  # create
  set tl [::project::CreateForm \
              -title "[mc {Matrix table}]: [lindex $mattb 0]" \
              -iniconfig MatrixTable \
              -type Tables -helpkey "Tb4Edt"
  ]
  set tlf [$tl getframe]
  # create matrix
  btable $tlf.t -parent $tl \
                -posteditcmd [list ::TolConsole::TableRecentUpdate $tlf.t] \
                -iniconfig btable
  $tlf.t fillMatrix $mattb
  $tl bind <Control-F4> [list $tl kill]  
  pack $tlf.t -fill both -expand yes

  #if {$grammar == "VMatrix"} {
    #::tol::console stack release "zxw._.wxz"
  #}
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::ExportBDTMatrix { matrix } {
#/////////////////////////////////////////////////////////////////////////////
  variable group_id
  
  incr group_id
  set tl [toplevel .top$group_id]
  wm title $tl "ExportBDTMatrix"
  
  #pack $tl.t -fill both -expand yes
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::DrawSerie { } {
#/////////////////////////////////////////////////////////////////////////////  
  variable options_selected
  upvar \#0 ::TolInspector::options_selected(Serie) series_selected
  
  #puts "DrawSerie: series_selected=$series_selected"
  set series ""
  foreach it $series_selected {
    lappend series [lindex $it 0]
  }
  DrawSeries $series
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::DrawSeries { series {gcf ""}} {
#/////////////////////////////////////////////////////////////////////////////  
  variable group_id

  if { [llength $series] } {
    incr group_id
    # series     
    set sergrp group$group_id
    set t0 [time {
    eval "::tol::seriegrp create $sergrp $series"
    }]
    # name
    set name [$sergrp serie 0 name]
    if { [$sergrp serie size] > 1} {
      set name ${name}...
    }
    # create
    set t1 [time {
    set tl [::project::CreateForm \
        -title [mc "Series graph: %s" $name] \
        -type Graphs \
        -iniconfig SeriesGraph \
	-width 900 -height 650 -helpkey "Tb4GraSer"
           ]}]
    set tlf [$tl getframe]
    # configuration
    set t2 [time {
    set Instance [::SeriesGraph::Create $tlf.g $sergrp]
    }]
    #puts "t0 = $t0"
    #puts "t1 = $t1"
    #puts "t2 = $t2"
    if { $gcf != "" } {
      ::bayesGraph::LoadGCF $Instance $gcf
    }
    bind $tlf.g <Destroy> +[list ::tol::seriegrp destroy $sergrp]
    pack $tlf.g -fill both -expand yes
    $tl bind <Control-F4> [list $tl kill]
    update
    # Esta opción se debe aplicar cuando el gráfico esté empacado
    #::bayesGraph::GrapOptApplyAxisTicks $Instance ${Instance}::options gr,0
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::DrawAutocorSerie { serie } {
#/////////////////////////////////////////////////////////////////////////////  
  variable group_id
    
  incr group_id
  # title
  
  if {[llength $serie] > 1} {
    set nameSerie [lindex [::tol::info variable $serie] 1]
  } else {
    set nameSerie $serie
  }
  # create
  set tl [::project::CreateForm \
      -title "[mc "Autocorrelation graph"]: $nameSerie" \
      -iniconfig AutocorGraph \
      -type Graphs \
      -width 900 -height 650 -helpkey "Tb4GraAco"
  ]
  set tlf [$tl getframe]
  # configuration
  ::AutocorGraph::Create $tlf.g $serie
  $tl bind <Control-F4> "$tl kill; break"
  pack $tlf.g -fill both -expand yes
  update
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::TableSerie { } {
#/////////////////////////////////////////////////////////////////////////////  
  variable options_selected
  variable group_id
  upvar \#0 ::TolInspector::options_selected(Serie) series_selected

  set series ""
  foreach it $series_selected {
    lappend series [lindex $it 0]
  }

  if { [llength $series] } {
    incr group_id

    set sergrp group$group_id
    eval "::tol::seriegrp create $sergrp $series"
    
    # create
    set tl [::project::CreateForm \
        -title [mc "Time series table"] \
        -iniconfig SeriesTable \
        -type Tables -helpkey "Tb4Edt"
    ]
	set tlf [$tl getframe]
    # configuration
    btable $tlf.t -parent $tl \
                  -posteditcmd [list ::TolConsole::TableRecentUpdate $tlf.t] \
                  -iniconfig btable
    $tlf.t fillSerie $sergrp 
    bind $tlf.t <Destroy> +[list ::tol::seriegrp destroy $sergrp]
    pack $tlf.t -fill both -expand yes
    $tl bind <Control-F4> [list $tl kill]
    update
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::TableAutocorSerie { serie } {
#/////////////////////////////////////////////////////////////////////////////  
  variable group_id
    
  incr group_id
  # title
  if {[llength $serie] > 1} {
    set nameSerie [lindex [::tol::info variable $serie] 1]
  } else {
    set nameSerie $serie
  }
  # create
  set tl [::project::CreateForm \
      -title "[mc "Autocorrelation table"]: $nameSerie" \
      -iniconfig AutocorGraphTable \
      -type Tables -helpkey "Tb4Edt"
  ]
  set tlf [$tl getframe]
  # configuration
  btable $tlf.t -parent $tl \
                -posteditcmd [list ::TolConsole::TableRecentUpdate $tlf.t] \
                -iniconfig btable
  $tlf.t fillAutocor $serie ""
  $tl bind <Control-F4> [list $tl kill]
  pack $tlf.t -fill both -expand yes
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::TableStatisticsSerie { } {
#/////////////////////////////////////////////////////////////////////////////
  variable options_selected
  variable group_id
  upvar \#0 ::TolInspector::options_selected(Serie) series_selected

  set series ""
  foreach it $series_selected {
    lappend series [lindex $it 0]
  }

  if { [llength $series] } {
    incr group_id

    set sergrp group$group_id
    eval "::tol::seriegrp create $sergrp $series"
    # create
    set tl [::project::CreateForm \
        -title [mc "Statistics table"] \
        -iniconfig StatisticsTable \
        -type Tables -helpkey "Tb4Edt"
    ]
    set tlf [$tl getframe]
    # configuration
    btable $tlf.t -parent $tl \
                  -posteditcmd [list ::TolConsole::TableRecentUpdate $tlf.t] \
                  -iniconfig btable
    $tlf.t fillStatistics $sergrp
    bind $tlf.t <Destroy> +[list ::tol::seriegrp destroy $sergrp]
    $tl bind <Control-F4> [list $tl kill]
    pack $tlf.t -fill both -expand yes
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::ViewTimeSet { tms args } {
#/////////////////////////////////////////////////////////////////////////////
  variable group_id
  
  incr group_id
  
  #puts "::TolInspector::ViewTimeSet $tms"
  # args
  array set opts [list \
    -title "[mc "TimeSet calendar"]: $tms" \
    -geometry {}
  ]
  array set opts $args  
  
  # create
  set tl [::project::CreateForm \
      -title $opts(-title) \
      -iniconfig Calendar \
      -type TimeSets -width 470 -height 345 -helpkey "Tb4Cal"
  ]
  # geometry
  $tl setgeometry $opts(-geometry)
  
  set tlf [$tl getframe]
  # configuration
  set tmscmd tms$group_id
  ::tol::timeset create $tmscmd $tms
  ::CalendarTms::Create $tlf.t $tmscmd
  bind $tlf.t <Destroy> +[list ::tol::timeset destroy $tmscmd]
  bind $tl <Control-F4> [list $tl kill]
  pack $tlf.t -fill both -expand yes
  
  return $tl
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::ViewDefinition {name path grammar} {
#/////////////////////////////////////////////////////////////////////////////
  variable w_tabset
  if {$path == ""} {
    tk_messageBox -type ok -icon warning\
      -message [mc "Object %1\$s is not defined in a tol file" $name]\
      -parent $w_tabset -title [mc Warning]
    return 
  }
  set ext [file extension $path]
  switch -- $ext {
    .tol -
    .TOL {
      set Instance [::Editor::Open $path]
      update
      if {[string equal $grammar "Code"] ||
          [string equal $grammar "Anything"]} {
        append txtFind " " $name "( )*\\("
      } else {
        #append txtFind $grammar "( )+" $name
        append txtFind $name
      }
      #Me situo al principio del texto
      set editor [::Editor::GetText $Instance]
      tk::TextSetCursor $editor 1.0
      ::BayesText::FindShow $editor 1 $txtFind 1
      #::Editor::CmdFindNext $Instance $txtFind
    }
    .bst -
    .BST -
    .bdt -
    .BDT {
      # Igual luego se abre como tabla
      ::Editor::Open $path
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::ViewFile {name} {
#
# PURPOSE:
#
# PARAMETERS:
#   name -> cadena con el nombre de un archivo o el de una variable
#           que hace referencia a un archivo
#
#/////////////////////////////////////////////////////////////////////////////
  #  En name puede venir el nombre de un archivo o el de una variable
  # que hace referencia a un archivo.
  #puts "ViewFile. name=$name"
  if [file isfile $name] {
    set path $name
  } else  {
    set path [::tol::info path $name]
  }
  ::Editor::Open $path
}
