#/////////////////////////////////////////////////////////////////////////////
# FILE    : sergraph.tcl
# PURPOSE : This file 
#           It's a child of bayesGraph so inherites all its functionalities.
#/////////////////////////////////////////////////////////////////////////////

package require bcolor
package require bmultiselect

namespace eval ::SeriesGraph {
  variable tmpOpt
}

proc ::SeriesGraph::ShowSerieInfo { Instance graph x y } {
  upvar \#0 ${Instance}::widgets widgets
  upvar \#0 ${Instance}::data data
  
  if ![$graph element closest $x $y info -interpolate yes] {
    beep
    return
  }
  parray info
}

proc ::SeriesGraph::BindSerieInfo { Instance } {
  upvar \#0 ${Instance}::widgets widgets
  upvar \#0 ${Instance}::data data

  set graph $widgets(gr,0)
  bind closest-point-$graph <Control-ButtonPress-2>  \
      [ string map [list %I $Instance] {
        ::SeriesGraph::ShowSerieInfo %I %W %x %y
      } ]
  blt::AddBindTag $graph closest-point-$graph
  
}

#/////////////////////////////////////////////////////////////////////////////
proc ::SeriesGraph::Create { path list args } {
#
# PURPOSE:
#      
# PARAMETERS:
#   path
#   this -> bayesgraph instance
#   structure ->
#   fileGCF -> file the graphic configuration file. Default ""
#
#/////////////////////////////////////////////////////////////////////////////
#puts "SeriesGraph::Create path=$path list=$list args=$args ........"
  array set opts {
    -struct g
    -fileGCF ""
    -photo 0
    -title {}
  }
  array set opts $args
  set t0 [time {
  set Instance [::bayesGraph::Create $path $opts(-struct) -photo $opts(-photo)]
  }]
  puts "time in ::bayesGraph::Create = $t0"
  upvar \#0 ${Instance}::widgets widgets
  upvar \#0 ${Instance}::data data

  #::SeriesGraph::BindSerieInfo $Instance
  #Blt_ClosestPoint $widgets(gr,0)

  set data(namespace)            SeriesGraph
  set data(proc,OnGetLabel)      ::SeriesGraph::GetLabel
  set data(proc,OnOptionCreate)  ::SeriesGraph::OptionsCreate
  set data(proc,OnOptionDestroy) ::SeriesGraph::OptionsDestroy
  set data(proc,OnOptionApply)   ::SeriesGraph::OptionsApply
  set data(proc,OnComputeStepX)  ::SeriesGraph::ComputeStepX
  set data(proc,OnFixStepX)      ::SeriesGraph::FixStepX
  set data(proc,OnEvalWindow)    ::SeriesGraph::EvalWindow
  set data(proc,OnToTable)       ::SeriesGraph::ToTable
  set data(proc,OnWriteIni)      ::SeriesGraph::WriteIni
  set data(proc,xAxisCommand)    [list ::SeriesGraph::GetTickLabel $Instance]
  set data(proc,x2AxisCommand)   [list ::SeriesGraph::GetTickLabel $Instance]  

  PushButtons $Instance

  # Fill graph
  set lstInfo [::SeriesGraph::CreateLine $Instance $list]
  set lstname [lindex $lstInfo 0]
  set lstdesc [lindex $lstInfo 1]

  #Establecer el fichero GCF del que se lee
  if {[string length $opts(-fileGCF)]} {
    ::bayesGraph::LoadGCF $Instance $opts(-fileGCF)
  } else  {
    ::bayesGraph::LoadGCF $Instance
  }
  ReadIni   $Instance
  Configure $Instance

  SetLabelSerie $Instance $lstname $lstdesc
  
  #set title chart
  if {[string length $opts(-title)]} {
    ::bayesGraph::SetMainTitle $Instance $opts(-title)
  }
  ::bayesGraph::ZoomExtend  $Instance
  return $Instance
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::SeriesGraph::CreateLine { this lst } {
#
# PURPOSE:
#      
# PARAMETERS:
#   this -> bayesgraph instance
#   lst  -> values to draw
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::widgets widgets

  for  {set gr 0} {$gr<$data(gr,numGraph)} {incr gr 1} {
    set grpath $widgets(gr,$gr)
    set sergrp [lindex $lst $gr]
    # variables especificas de cada grafico
    set data(gr,active) $gr
    set data(gr,$gr,sergrp) $sergrp
    set data(gr,$gr,n_ser) [$sergrp serie size]
    set data(gr,$gr,OnGetXTicks)  ::SeriesGraph::GetXTicks
    set data(gr,$gr,OnGetXValues) ::SeriesGraph::GetXValues
    # insert the series into the graph
    for {set i 0} {$i < $data(gr,$gr,n_ser)} {incr i} {
      # names and description
      set objref  [$sergrp serie $i reference]
      if {[llength $objref]>1} {
        array set objinfo [TclInfoRef $objref]
      } else {
        array set objinfo [TclInfoVar Serie $objref]
      }
      if {[info exists objinfo(DESC)]} {
        lappend lstdesc $objinfo(DESC)
      } else  {
        lappend lstdesc {}
      }
      lappend lstname l$i-0
      # values
      set j 0
      set segs {}
      foreach vec_data [$sergrp serie $i data] {
        $grpath line create l$i-$j -xdata [lindex $vec_data 0] \
                                   -ydata [lindex $vec_data 1]
        lappend segs l$i-$j
        if { $j } {
          $grpath line configure l$i-$j -label ""
        } else {
          set nameSerie [$sergrp serie $i name]
          $grpath line configure l$i-0 -label $nameSerie
          set data(labels,l$i) $nameSerie
        }
        incr j
      }
      lappend data(gr,$gr,elements) $segs
    }
  
    global zoomInfo
  }
  ::bayesGraph::BindLegend $this
  return [list $lstname $lstdesc]
}

#/////////////////////////////////////////////////////////////////////////////
proc ::SeriesGraph::AddSerieToGraph { this newSer {typSer LineElement} } {
#
# PURPOSE: Anhadimos una nueva Serie a un grafico existente.
#   Los pasos a seguir son los siguientes:
#   1. borramos todas las lineas existentes
#   2. capturamos las Series anteriores
#   3. creamos un nuevo conjunto de Series
#   4. pintamos las nuevas series
#   5. ?? dejar el grafico como estaba (exactamente no se lo que implica)
#
# PARAMETERS:
#   this -> bayesgraph instance
#   newSer -> new serie
#   typSer -> type of line [LineElement | BarElement]
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ::bayesGraph::tmpOpt ary
  
  for  {set gr 0} {$gr<$data(gr,numGraph)} {incr gr 1} {
    # path of Graph
    set grpath $widgets(gr,$gr)
    #   1. borramos todas las lineas existentes
    # type of series
    foreach item $data(gr,$gr,elements) {
      lappend typesSeries [$grpath element type $item]
    }
    lappend typesSeries $typSer
    # clear lines
    set data(gr,$gr,elements) {}
    set namLin [$grpath element names]
    foreach item $namLin {
      $grpath element delete $item
    }
    #   2. capturamos las Series anteriores
    for {set s 0} {$s<$data(gr,$gr,n_ser)} {incr s 1} {
      lappend series [$data(gr,$gr,sergrp) serie $s name]
    }  
    #   3. creamos un nuevo conjunto de Series
    if {[info exists data(gr,$gr,sergrp)]} {
      ::tol::seriegrp destroy $data(gr,$gr,sergrp)
    }
    lappend series $newSer
    set sergrp $data(gr,$gr,sergrp)
    eval "::tol::seriegrp create $sergrp $series"
    #   4. pintamos las nuevas series    
    set lstInfo [::SeriesGraph::CreateLine $this $sergrp]
    set lstname [lindex $lstInfo 0]
    set lstdesc [lindex $lstInfo 1]
    #   5. ?? dejar el grafico como estaba (exactamente no se lo que implica)
    ::SeriesGraph::Configure     $this
    ::SeriesGraph::SetLabelSerie $this $lstname $lstdesc
    # change types
    foreach git $data(gr,$gr,elements) typ $typesSeries {
      if ![string equal $typ [$grpath element type [lindex $git 0]]] {
        ::bayesGraph::ElemOptChangeType $this [lindex $git 0] $git
      }
    }
    # show info
    #set data(gr,$gr,listNames)  $lstname
    #set data(gr,$gr,listLabels) $lstdesc
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SeriesGraph::SetLabelSerie {this lines lstdesc} {
#
# PURPOSE: Establecemos nombres de las lineas con la descripcion de las
#          series pintadas
#
# PARAMETERS: this -> Instancia bysgraph
#             lines -> nombre de las series pintadas
#             lstdesc -> descripciones de las lineas pintadas
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::options opt
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::widgets widgets
  for {set gr 0} {$gr<$data(gr,numGraph)} {incr gr} {
    set grpath $widgets(gr,$gr)
    if {$opt(gr,$gr,sergrp,lineShowDescription)==1} {
      foreach l $lines d $lstdesc {
        if {[string length $d]} {
          $grpath element configure $l -label $d
        }
      }
    }
  } 
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SeriesGraph::PushButtons {this} {
# 
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data data
  set widgets(tbe,path) [frame $widgets(tb,path).tbe -bd 2]
  set tbe $widgets(tbe,path)  
  pack $tbe -side left
  #grid $tbe -sticky ew
  set data(tb,btes) ""
  set widgets(tb,bte,sep) [Separator $tbe.sep -orient vertical]
  lappend data(tb,btes) sep
  set widgets(tb,bte,average) [Button $tbe.average -image [::Bitmap::get "average"] \
			      -highlightthickness 0 -activebackground \#ececec \
			      -takefocus 0 -relief link -bd 1 -padx 1 -pady 1 \
			      -helptext [mc "Show/Hide Average"] \
			      -command [list ::SeriesGraph::OptionAverage $this]]
  lappend data(tb,btes) average
  set widgets(tb,bte,stddev) [Button $tbe.stddev -image [::Bitmap::get "stddev"] \
			      -highlightthickness 0 -activebackground \#ececec \
			      -takefocus 0 -relief link -bd 1 -padx 1 -pady 1 \
			      -helptext [mc "Show/Hide Std. Dev."] \
			      -command [list ::SeriesGraph::OptionStdDev $this]]
  lappend data(tb,btes) stddev
  set widgets(tb,bte,stattb) [Button $tbe.stattb -image [::Bitmap::get "tablestat"] \
			      -highlightthickness 0 -activebackground \#ececec \
			      -takefocus 0 -relief link -bd 1 -padx 1 -pady 1 \
			      -helptext [mc "View Statistics table"] \
			      -command [list ::SeriesGraph::StatisticsTable $this]]
  lappend data(tb,btes) stattb

 # set widgets(tb,bte,accgra) [Button $tbe.acugra -image [::Bitmap::get "Serie"] \
			      -highlightthickness 0 -activebackground \#ececec \
			      -takefocus 0 -relief link -bd 1 -padx 1 -pady 1 \
			      -helptext [mc "Accumulate graph"] \
			      -command [list ::SeriesGraph::AccumulateGraph $this]]
 # lappend data(tb,btes) accgra


  # pack de los botones especificos
  set but_col 0
  foreach but $data(tb,btes) {
    if { [string equal -length 3 $but "sep"] } { 
      grid $widgets(tb,bte,$but) -row 0 -column $but_col -sticky ns -padx 3 
    } else  {
      grid $widgets(tb,bte,$but) -row 0 -column $but_col -sticky w -ipadx 2 -ipady 2 -pady 1
    }
    incr but_col
  }
  grid columnconfigure $tbe $but_col -weight 1
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SeriesGraph::Configure {this} {
# 
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::options opt
  ::bayesGraph::Configure $this
  set old_active $data(gr,active)
  for {set gr 0} {$gr<$data(gr,numGraph)} {incr gr} {
    set data(gr,active) $gr

    OptionsApplyDateFormat $this $opt(gr,$gr,sergrp,dateString) 
    OptionsApplyAxis       $this $opt(gr,$gr,sergrp,xAxis)
    OptionsApplyXMarkers $this $opt(gr,$gr,sergrp,xMarkers) $opt(gr,$gr,sergrp,xMarkersColors)\
           $opt(gr,$gr,sergrp,xMarkersDashes) $opt(gr,$gr,sergrp,xMarkersWidths)
    OptionsApplyYMarkers    $this ${this}::options gr,$gr,sergrp
  }
  set data(gr,active) $old_active
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SeriesGraph::GetLabel { this bltg name tick y} {
# 
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::data data
  set gr $data(gr,active)

  set t [ expr {round($tick)} ]
  if { [ catch { $data(gr,$gr,sergrp) dating $t } labelTick ] } {
    set labelTick $tick
  }
  if { [ regexp {(l[0-9])+(-[0-9])*} $name ==> codeLine ] } {
    set nameLine $data(labels,$codeLine)
  } else {
    set nameLine "NO LINE"    
  }
  return "$nameLine: ($labelTick,$y)"
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SeriesGraph::GetTickLabel { this bltg tick } {
# 
#/////////////////////////////////////////////////////////////////////////////  
  set sergrp [getSerGroup $this]
  if [string is int $tick] {
    set str ""
    catch {
      set str [$sergrp dating $tick]
    } 
    return $str
  } else {
#    return $tick
     return ""
  }
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::SeriesGraph::GetXTicks {Instance} {
#
# PURPOSE:
#
# PARAMETERS:
#   Instance -> Instancia bysgraph
#
# RETURN:
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${Instance}::data data
  upvar \#0 ${Instance}::widgets widgets
  
  set gr $data(gr,active)
  set values [::bayesGraph::GetLimitsValueAxis $Instance x]
  if {[llength $values]} {
    for {set i [expr int([lindex $values 0])]} \
        {$i <= [expr int([lindex $values 1])]} {incr i} {
      lappend lst [GetTickLabel $Instance $widgets(gr,$gr) $i]
    }
  } else {
    set lst {}
  }
  return $lst
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::SeriesGraph::GetXValues {Instance } {
#
# PURPOSE:
#
# PARAMETERS:
#   Instance -> Instancia bysgraph
#
# RETURN:
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${Instance}::data data
  set gr $data(gr,active)
  set values [::bayesGraph::GetLimitsValueAxis $Instance y]
  if {[llength $values]} {
    for {set i [expr int([lindex $values 0])]} \
        {$i <= [expr int([lindex $values 1])]} {incr i} {
      lappend lst $i
    }
  } else  {
    set lst {}
  }
  return $lst
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::SeriesGraph::ComputeStepX {this min max total} {
#
# PURPOSE:
#
# PARAMETERS:
#   this  -> Instancia bysgraph
#   min   ->
#   max   ->
#   total ->
#
# RETURN:
#
#/////////////////////////////////////////////////////////////////////////////
  set step {}
  if {[string length $min] && [string length $max]} {
    set dif [expr $max - $min]
    if {$total > $dif} {
      set step 1
    } else {
#puts "SeriesGraph:.ComputeStepX $this  min=$min  max=$max  total=$total  dif=$dif"
      if {![catch {expr int(ceil($dif/$total))} res]} {
        set step $res
      } else {
        #puts "SeriesGraph.ComputeStepX. $res"
        Tolcon_Trace "SeriesGraph.ComputeStepX. $res"
        set step 0
      }
      #set step [expr int(ceil($dif/$total))]
    }
  }
  return $step
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SeriesGraph::FixStepX {this min max step} {
# 
#/////////////////////////////////////////////////////////////////////////////  
#TODO: Error en la resta
  set ok 1
  if {[string length $max] && [string length $min]} {
    set dif [expr $max - $min]
  } else  {
    set ok 0
  }
  if {($ok) && ($step>$dif)} {
    set step [expr round($dif)]
  } else {
    set step [expr round($step)]
  }
  return $step
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SeriesGraph::getSerGroup {this} {
# 
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::data data

  set gr $data(gr,active)
  set sergrp $data(gr,$gr,sergrp)
  return $sergrp
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SeriesGraph::EvalWindow {this} {
# 
#/////////////////////////////////////////////////////////////////////////////  
  set sergrp [getSerGroup $this]
  set result ""
  set numCols [$sergrp serie size]
  for {set i 0} {$i < $numCols} {incr i} {
    lappend result [$sergrp serie $i name]
  }
  return [ListToStr $result]
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SeriesGraph::ToTable {this} {
# 
#/////////////////////////////////////////////////////////////////////////////  
  set grpath [::bayesGraph::getActiveGraph $this]
  set sergrp [getSerGroup $this]
  set tl .tbl_from_$sergrp
  if [winfo exists $tl] {
     raise $tl
  } else {
    # create
	set tl [::project::CreateForm \
        -title [mc "Time series table"] \
        -type Tables \
        -iniconfig SeriesTable -helpkey "Tb4GraSer"
    ]
	set tlf [$tl getframe]
    # config
    btable $tlf.t -parent $tl \
                  -posteditcmd [list ::TolConsole::TableRecentUpdate $tlf.t] \
                  -iniconfig btable
    set tempformat [$sergrp format]
    $sergrp format ""
    $tlf.t fillSerie $sergrp [TolDateFormat2TclDateFormat $tempformat]
    $sergrp format $tempformat
    $tl bind <Control-F4> [list $tl kill]
    pack $tlf.t -fill both -expand yes
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SeriesGraph::StatisticsTable {this} {
# 
#/////////////////////////////////////////////////////////////////////////////  
  set grpath [::bayesGraph::getActiveGraph $this]
  set sergrp [getSerGroup $this]

  set tl .stattbl_from_$sergrp
  if [winfo exists $tl] {
    raise $tl
  } else {
    # create
    set tl [::project::CreateForm \
        -title [mc "Statistics table"] \
        -type Tables \
        -labelini StatisticsTable
    ]
	set tlf [$tl getframe]
    # config
    btable $tlf.t -parent $tl \
                  -posteditcmd [list ::TolConsole::TableRecentUpdate $tlf.t] \
                  -iniconfig btable
    $tlf.t fillStatistics $sergrp
    $tl bind <Control-F4> [list $tl kill]
    pack $tlf.t -fill both -expand yes
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SeriesGraph::OptionAverage {this} {
# 
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::options opt
  set gr $data(gr,active)
  
  set opt(gr,$gr,sergrp,yMarkerAve) [expr !$opt(gr,$gr,sergrp,yMarkerAve)]
  OptionsApplyYMarker    $this ${this}::options gr,$gr,sergrp {Ave Average}

}


#/////////////////////////////////////////////////////////////////////////////
proc ::SeriesGraph::OptionStdDev {this} {
# 
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::options opt
  set gr $data(gr,active)
  
  set opt(gr,$gr,sergrp,yMarkerDev) [expr !$opt(gr,$gr,sergrp,yMarkerDev)]
  OptionsApplyYMarkerNDev    $this ${this}::options gr,$gr,sergrp
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SeriesGraph::AccumulateGraph {this} {
# 
#/////////////////////////////////////////////////////////////////////////////  
  set grpath [::bayesGraph::getActiveGraph $this]
  set sergrpOld [getSerGroup $this]

  set tl .accgra_from_$sergrpOld
  if [winfo exists $tl] {
    raise $tl
  } else {
    set tl [::project::CreateForm $tl \
              -title    [mc "Accumulate Time series graph"] \ 
              -type     Graphs ]

    set sergrp groupFrom$sergrpOld
    eval "::tol::seriegrp create $sergrp {FinSemMes}"
    wm withdraw $tl

    set Instance [::SeriesGraph::Create $tl.g $sergrp]
    bind $tl.g <Destroy> +[list ::tol::seriegrp destroy $sergrp]
    pack $tl.g -fill both -expand yes
    wm withdraw $tl
    wm deiconify $tl
    update
    # Esta opción se debe aplicar cuando el gráfico esté empacado
    ::bayesGraph::GrapOptApplyAxisTicks $Instance ${Instance}::options gr,0
    $tl bind <Control-F4> [list $tl kill]
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::SeriesGraph::OptionsDestroy {this} {
#
# PURPOSE: Called before options dialog is destroyed. This procedure is called
#          by parent BayesGraph. Actually doesn't do anything.
#
# PARAMETERS:
#   this -> Instance of BayesGraph
#
#/////////////////////////////////////////////////////////////////////////////
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::SeriesGraph::OptionsCreate {this ts} {
#
# PURPOSE: Creates especific options of a series graph. It's called by
#          BayesGraph when general graph options are created.
#
# PARAMETERS:
#   this -> Instance of BayesTable
#   ts   -> tabset in which options must be created
#
#/////////////////////////////////////////////////////////////////////////////
  #insertar tabset
  $ts insert end Series
  set f [frame $ts.f]
  $ts tab configure "Series" -text "Series" -window $f\
    -fill both -padx 0 -selectbackground \#d9d9d9 -bg gray75

  set w_tabset [::blt::tabset $f.ts -relief flat -highlightthickness 0\
		        -bd 0 -outerpad 0 -tier 2 -slant right -textside right]
  $w_tabset insert end DateFormat Axis X-Markers Y-Markers Legend

  set f1 [frame $w_tabset.f1]
  set f2 [frame $w_tabset.f2]
  set f3 [frame $w_tabset.f3]
  set f4 [frame $w_tabset.f4]
  set f5 [frame $w_tabset.f5]


  $w_tabset tab configure "DateFormat" -text [mc "Date Format"]\
    -window $f1 -fill both -padx 0 -selectbackground \#d9d9d9 -bg gray75
  $w_tabset tab configure "Axis" -text [mc "Axis"] \
    -window $f2 -fill both -padx 0 -selectbackground \#d9d9d9 -bg gray75
  $w_tabset tab configure "X-Markers" -text [mc "X-Markers"]\
    -window $f3 -fill both -padx 0 -selectbackground \#d9d9d9 -bg gray75
  $w_tabset tab configure "Y-Markers" -text [mc "Y-Markers"]\
    -window $f4 -fill both -padx 0 -selectbackground \#d9d9d9 -bg gray75
  $w_tabset tab configure "Legend" -text [mc "Legend"]\
    -window $f5 -fill both -padx 0 -selectbackground \#d9d9d9 -bg gray75


  OptionsGet $this

  OptionsCreateDateFormat $this $f1
  OptionsCreateAxis       $this $f2
  OptionsCreateXMarkers   $this $f3
  OptionsCreateYMarkers   $this $f4
  OptionsCreateLegend     $this $f5

  OptionsInit  $this
  OptionsCheck $this
  OptionsSet   $this

  grid $w_tabset -sticky news  
  grid rowconfigure    $f 0 -weight 1
  grid columnconfigure $f 0 -weight 1
      
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SeriesGraph::ShowSpecifiers {w} {
# 
#/////////////////////////////////////////////////////////////////////////////  
  if [winfo exist $w.t] {
    raise $w.t
    return
  }
  set dialog [Dialog $w.t -title [mc "Format Specifiers"]\
              -default 0 -parent $w -modal none -separator true]
  wm resizable $dialog 0 0
  set w [$dialog getframe]

  set f1 [frame $w.f1]
  set f2 [frame $w.f2]
  set f3 [frame $w.f3]
  set f4 [frame $w.f4]
  set f5 [frame $w.f5]
  set color1 red

  label $f1.lwda -text "%W" -foreground $color1
  label $f1.lwd  -text [mc "Long Weekday"]
  label $f1.lmna -text "%N" -foreground $color1
  label $f1.lmn  -text [mc "Long Month Name"]
  label $f1.swda -text "%w" -foreground $color1
  label $f1.swd  -text [mc "Short Weekday"]
  label $f1.smna -text "%n" -foreground $color1
  label $f1.smn  -text [mc "Short Month Name"]
  grid $f1.lwda $f1.lwd -sticky nw -padx 5
  grid $f1.lmna $f1.lmn -sticky nw -padx 5
  grid $f1.swda $f1.swd -sticky nw -padx 5
  grid $f1.smna $f1.smn -sticky nw -padx 5

  label $f2.dna  -text "%d" -foreground $color1
  label $f2.dn   -text [mc "Day Number"]
  label $f2.mna  -text "%m" -foreground $color1
  label $f2.mn   -text [mc "Month Number"]
  label $f2.tyda -text "%y" -foreground $color1
  label $f2.tyd  -text [mc "Two Year Digit"]
  label $f2.fyda -text "%Y" -foreground $color1
  label $f2.fyd  -text [mc "Four Year Digit"]
  grid $f2.dna  $f2.dn  -sticky nw -padx 5
  grid $f2.mna  $f2.mn  -sticky nw -padx 5
  grid $f2.tyda $f2.tyd -sticky nw -padx 5
  grid $f2.fyda $f2.fyd -sticky nw -padx 5

  label $f3.ha  -text "%h" -foreground $color1
  label $f3.h   -text [mc "Hour"]
  label $f3.ia  -text "%i" -foreground $color1
  label $f3.i   -text [mc "Minute"]
  label $f3.sa  -text "%s" -foreground $color1
  label $f3.s   -text [mc "Second"]
  grid $f3.ha $f3.h   -sticky nw -padx 5
  grid $f3.ia $f3.i   -sticky nw -padx 5
  grid $f3.sa $f3.s   -sticky nw -padx 5


  label $f4.fuda -text "%u" -foreground $color1
  label $f4.fud  -text [mc "Forze uncompleted dates"]
  label $f4.fcda -text "%c" -foreground $color1
  label $f4.fcd  -text [mc "Forze completed dates"]
  label $f4.psa  -text "%%" -foreground $color1
  label $f4.ps   -text [mc "Percent symbol (%)"]
  
  grid $f4.fuda $f4.fud   -sticky nw -padx 5
  grid $f4.fcda $f4.fcd   -sticky nw -padx 5
  grid $f4.psa  $f4.ps    -sticky nw -padx 5
  
  Button $f5.b -image [::Bitmap::get "accept"]\
      -helptext [mc "OK"] -relief link -compound left -text [mc "Accept"]\
      -command "destroy $dialog"   

  grid $f5.b -columnspan 2 -sticky ns

  grid $f1 $f2 -sticky nw -padx 20 -pady 5
  grid $f3 $f4 -sticky n
  grid $f5 -sticky s -columnspan 2 -pady 3

  bind $dialog <F1> "ShowHelp Tb4GraSerOpc"
  $dialog draw
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SeriesGraph::OptionsCreateDateFormat {this w} {
# 
#/////////////////////////////////////////////////////////////////////////////  
  variable tmpOpt
  variable formats

  # X-Axis
  TitleFrame $w.tf1 -text [mc "Format String"]
  set f1 [$w.tf1 getframe]

  set formats [list \
     [mc "TOL Long Format"]        "%cy%Ym%md%d%uh%hi%is%s" \
     [mc "TOL Short Format"]       "%cy%Ym%md%d" \
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
      -textvariable ::SeriesGraph::tmpOpt(var,dateFormat)\
      -modifycmd [list ::SeriesGraph::OptionsCheckDateFormat $this]
  label $f1.lString -text [mc String]:
  set tmpOpt(widgets,dateString) [Entry $f1.eString -width 45 -state disabled\
         -textvariable ::SeriesGraph::tmpOpt(var,dateString)]
  bind $f1.eString <KeyRelease> [list ::SeriesGraph::OptionsCheckDateString $this]
  label $f1.lView   -text [mc Sample]:
  set tmpOpt(widgets,dateView) [Entry $f1.eView   -width 45 -state disabled\
         -textvariable ::SeriesGraph::tmpOpt(var,dateView)]
  button $f1.bHelp  -text [mc "Format Specifiers"] \
         -image [::Bitmap::get infor] -compound left \
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
proc ::SeriesGraph::OptionsCreateAxis {this w} {
# 
#/////////////////////////////////////////////////////////////////////////////  
  variable tmpOpt

  # X-Axis

  TitleFrame $w.tf -text [mc "X-Axis Ticks"]
  set f [$w.tf getframe]

  label $f.lxAxis -text [mc "Select TimeSets for axis"]:
  set lst [::tol::info variable TimeSet] 
  # list of values
  foreach it $tmpOpt(var,xAxis) {
    if { [set idx [lsearch $lst $it]] != -1} {
      set lst [lreplace $lst $idx $idx]
    } else  {
      set idx [lsearch $tmpOpt(var,xAxis) $it]
      set tmpOpt(var,xAxis) [lreplace $tmpOpt(var,xAxis) $idx $idx]
    }
  }
  set tmpOpt(widget,xAxisMSEL) \
    [bmultiselect $f.msel -unsel $lst -sel $tmpOpt(var,xAxis)]


  frame $f.f -width 25
  grid $f.lxAxis - -sticky nw -padx 2
  #grid $f.f $f.msel   -sticky nw -padx 10 -pady 2
  grid $f.f $f.msel   -sticky news -padx 10 -pady 2  
  grid rowconfigure    $f 1 -weight 1
  grid columnconfigure $f 1 -weight 1
  grid $w.tf -sticky news -padx 2
  grid rowconfigure    $w 0 -weight 1
  grid columnconfigure $w 0 -weight 1
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SeriesGraph::OptionsCreateXMarkers {this w} {
# 
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::options opt
  variable tmpOpt

  set gr $data(gr,active)
  set g $widgets(gr,$gr)

  # X-Markers
  set w_tabset [::blt::tabset $w.ts -relief flat -highlightthickness 0\
        -bd 0 -outerpad 0 -tier 2 -slant right -textside right]
  $w_tabset insert end Markers Style

  set f1 [frame $w_tabset.f1]
  set f2 [frame $w_tabset.f2]

  $w_tabset tab configure "Markers" -text [mc "Markers"]\
        -window $f1 -fill both -padx 0 -selectbackground \#d9d9d9 -bg gray75
  $w_tabset tab configure "Style" -text [mc "Style"] \
        -window $f2 -fill both -padx 0 -selectbackground \#d9d9d9 -bg gray75

  TitleFrame $f1.tf -text [mc "X-Markers"]
  set f [$f1.tf getframe]
  label $f.lxMarker -text [mc "Select TimeSets to mark"]:
  set lst [::tol::info variable TimeSet]
  # list of values
  foreach it $tmpOpt(var,xMarkers) {
    if { [set idx [lsearch $lst $it]] != -1} {
      set lst [lreplace $lst $idx $idx]
    } else  {
      set idx [lsearch $tmpOpt(var,xMarkers) $it]
      set tmpOpt(var,xMarkers) [lreplace $tmpOpt(var,xMarkers) $idx $idx]
    }
  }

  set tmpOpt(widget,xMarkerMSEL) \
    [bmultiselect $f.msel -unsel $lst -sel $tmpOpt(var,xMarkers)]  

  frame $f.f -width 25
  grid $f.lxMarker - -sticky nw -padx 2
  grid $f.f $f.msel     -sticky news -padx 10 -pady 2
  grid rowconfigure    $f 1 -weight 1
  grid columnconfigure $f 1 -weight 1

  grid $f1.tf -sticky news -padx 2
  grid rowconfigure    $f1 0 -weight 1
  grid columnconfigure $f1 0 -weight 1  


  TitleFrame $f2.tf -text [mc "X-Markers Style"]
  set f [$f2.tf getframe]
  
  label $f.lStyle -text [mc "Choose lines color, type and width for 5 X-Markers"]:
  for {set i 0} {$i < 5} {incr i} {
    label $f.lDesc$i -text "[mc Marker] [expr $i+1]:"
    label $f.lColor$i -text [mc Color]:
    bcolor $f.fColor$i -variable ::SeriesGraph::tmpOpt(var,xMarkerColor$i)
    label $f.lDashes$i -text [mc "Style"]:
    ::Objects::ComboDashesCreate ::SeriesGraph::tmpOpt(var,xMarkerDashes$i) $f.sbDashes$i
    label   $f.lWidth$i -text [mc "Width"]:
    SpinBox $f.sbWidth$i -justify right -width 4 -editable 0 -range {0 10}\
            -textvariable ::SeriesGraph::tmpOpt(var,xMarkerWidth$i)
  }

  grid $f.lStyle - - - - - - - -sticky nw -padx 2 -pady 2
  frame $f.f -width 25
  for {set i 0} {$i < 5} {incr i} {
    grid $f.f $f.lDesc$i $f.lColor$i $f.fColor$i $f.lDashes$i $f.sbDashes$i\
              $f.lWidth$i $f.sbWidth$i -sticky nw -padx 2
  }  

  grid columnconfigure $f 1 -pad 10
  grid columnconfigure $f 3 -pad 20
  grid columnconfigure $f 5 -pad 20
  grid columnconfigure $f 7 -pad 20

  grid rowconfigure    $f 6 -weight 1
  grid columnconfigure $f 8 -weight 1
    
  grid $f2.tf -sticky news -padx 2
  grid rowconfigure    $f2 0 -weight 1
  grid columnconfigure $f2 0 -weight 1
    
  grid $w_tabset -sticky news
  grid rowconfigure    $w 0 -weight 1
  grid columnconfigure $w 0 -weight 1
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SeriesGraph::OptionsCreateYMarkers {this w} {
# 
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data data
  variable tmpOpt
  
  set gr $data(gr,active)
  set g $widgets(gr,$gr)  

  # Y-Markers
  TitleFrame $w.tf -text [mc "Y-Markers"]
  set f [$w.tf getframe]
  label $f.lyMarker -text [mc "Select Statistics to mark"]:

  foreach {key txt} {Ave "Average" Dev "Std. Dev." Var "Varianze" Med "Median"
                     Max "Maximun" Min "Minimun"} {
    checkbutton $f.ch$key -text [mc $txt]   -highlightthickness 0\
     -pady 0 -padx 0 -variable ::SeriesGraph::tmpOpt(var,yMarker$key)  
    label $f.lColor$key -text [mc Color]:
    bcolor $f.fColor$key -optmenu 2 \
      -variable ::SeriesGraph::tmpOpt(var,yMarkerColor$key)
    label $f.lDashes$key -text [mc "Style"]:
    ::Objects::ComboDashesCreate ::SeriesGraph::tmpOpt(var,yMarkerDashes$key) $f.sbDashes$key
    label   $f.lWidth$key -text [mc "Width"]:
    SpinBox $f.sbWidth$key -justify right -width 4 -editable 0 -range {0 10}\
            -textvariable ::SeriesGraph::tmpOpt(var,yMarkerWidth$key)
  }
  frame $f.fNDev
  label $f.fNDev.lNDev -text [mc "Draw Average +- "]
  SpinBox $f.fNDev.sbNDev -justify right -width 4 -editable 0 -range {1 10}\
            -textvariable ::SeriesGraph::tmpOpt(var,yMarkerNDev)
  label $f.fNDev.lNDev2 -text [mc "Std. Dev."]
  
  grid $f.fNDev.lNDev $f.fNDev.sbNDev $f.fNDev.lNDev2 -sticky nw -padx 2

  grid $f.lyMarker - - - - - - -sticky nw -padx 2 -pady 4
  frame $f.f -width 25
  set key Ave
  grid $f.f $f.ch$key $f.lColor$key $f.fColor$key $f.lDashes$key $f.sbDashes$key \
         $f.lWidth$key $f.sbWidth$key -sticky nw -padx 2
  foreach key {Dev Var Med Max Min} {
    grid ^ $f.ch$key $f.lColor$key $f.fColor$key $f.lDashes$key $f.sbDashes$key \
         $f.lWidth$key $f.sbWidth$key -sticky nw -padx 2
  }
  grid ^ ^ $f.fNDev - - - - - -sticky nw -padx 2 -pady 4
  grid rowconfigure    $f 7 -weight 1
  grid columnconfigure $f 1 -pad 10
  grid columnconfigure $f 3 -pad 20
  grid columnconfigure $f 5 -pad 20
  grid columnconfigure $f 7 -weight 1
  
  grid $w.tf -sticky news -padx 2
  grid rowconfigure    $w 0 -weight 1
  grid columnconfigure $w 0 -weight 1

}

#/////////////////////////////////////////////////////////////////////////////
proc ::SeriesGraph::OptionsCreateLegend {this w} {
#
# PURPOSE: Opciones sobre la leyenda
#
# PARAMETERS:
#   this   -> Instance of BayesGraph
#   w -> frame 
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpOpt

  # Legend
  TitleFrame $w.tf -text [mc "Legend"]
  set f [$w.tf getframe]
  checkbutton $f.cbshow -text [mc "showDesc"] -highlightthickness 0\
     -pady 0 -padx 0 -variable ::SeriesGraph::tmpOpt(var,lineShowDescription)

  grid $f.cbshow       -sticky nw
  grid rowconfigure    $f 1 -weight 1 
  grid columnconfigure $f 1 -weight 1
 
  grid $w.tf           -sticky news -padx 2
  grid rowconfigure    $w 0 -weight 1
  grid columnconfigure $w 0 -weight 1
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::SeriesGraph::OptionsGet {this} {
#
# PURPOSE: Initializes all options variables and stores them into the
#          temporary 'tmpOpt' array. 
#
# PARAMETERS:
#   this   -> Instance of BayesGraph
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::options opt
  variable tmpOpt

  set gr $data(gr,active)
  set g [::bayesGraph::getActiveGraph $this]
  set sergrp [getSerGroup $this]

  set tmpOpt(var,dateString)   $opt(gr,${gr},sergrp,dateString)

  #Opciones inicializadas desde código
  #if {[info exist ${this}::options(gr,${gr},sergrp,xMarkers)]} 
  set tmpOpt(var,xMarkers)  $opt(gr,$gr,sergrp,xMarkers) 
  set tmpOpt(var,yMarkers)  $opt(gr,$gr,sergrp,yMarkers) 
  set tmpOpt(var,xAxis)     $opt(gr,$gr,sergrp,xAxis) 
  set i 0
  foreach color $opt(gr,$gr,sergrp,xMarkersColors) {
    set tmpOpt(var,xMarkerColor$i) $color
    incr i
  }
  set i 0
  foreach width $opt(gr,$gr,sergrp,xMarkersWidths) {
    set tmpOpt(var,xMarkerWidth$i) $width
    incr i
  }
  set i 0
  foreach dashes $opt(gr,$gr,sergrp,xMarkersDashes) {
    set tmpOpt(var,xMarkerDashes$i) $dashes
    incr i
  }
  foreach key {Ave Dev Var Med Max Min} {
    set tmpOpt(var,yMarker$key)       $opt(gr,$gr,sergrp,yMarker$key)
    set tmpOpt(var,yMarkerColor$key)  $opt(gr,$gr,sergrp,yMarkerColor$key)  
    set tmpOpt(var,yMarkerDashes$key) $opt(gr,$gr,sergrp,yMarkerDashes$key) 
    set tmpOpt(var,yMarkerWidth$key)  $opt(gr,$gr,sergrp,yMarkerWidth$key)
  }
  set tmpOpt(var,yMarkerNDev) $opt(gr,$gr,sergrp,yMarkerNDev)

  set tmpOpt(var,lineShowDescription) $opt(gr,$gr,sergrp,lineShowDescription)

}


#/////////////////////////////////////////////////////////////////////////////
proc ::SeriesGraph::OptionsInit {this} {
# 
#/////////////////////////////////////////////////////////////////////////////  

}


#/////////////////////////////////////////////////////////////////////////////
  proc ::SeriesGraph::OptionsSet {this} {
#
# PURPOSE: Commits all options values from the temporary 'tmpOpt' array to
#          other structure.
#
# PARAMETERS:
#   this   -> Instance of BayesGraph
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::options options
  set gr $data(gr,active)
  variable tmpOpt

  regsub -all var, [array names ::SeriesGraph::tmpOpt var,*] "" optNames
  foreach it $optNames {
    set options(gr,$gr,sergrp,$it) $tmpOpt(var,$it)
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SeriesGraph::OptionsApply {this} {
# 
#/////////////////////////////////////////////////////////////////////////////  
  variable tmpOpt
  OptionsApplyDateFormat $this $tmpOpt(var,dateString)

  set lstXAxis  [$tmpOpt(widget,xAxisMSEL) cget -sel]  
  set tmpOpt(var,xAxis) $lstXAxis
  OptionsApplyAxis       $this $lstXAxis

  set ::SeriesGraph::tmpOpt(var,xMarkersColors) {}
  set ::SeriesGraph::tmpOpt(var,xMarkersDashes) {}
  set ::SeriesGraph::tmpOpt(var,xMarkersWidths) {}
  for {set i 0} {$i < 5} {incr i} {
    lappend tmpOpt(var,xMarkersColors) $tmpOpt(var,xMarkerColor$i)
    lappend tmpOpt(var,xMarkersDashes) $tmpOpt(var,xMarkerDashes$i)
    lappend tmpOpt(var,xMarkersWidths) $tmpOpt(var,xMarkerWidth$i)
  }

  set lstXMarkers [$tmpOpt(widget,xMarkerMSEL) cget -sel]  
  set tmpOpt(var,xMarkers) $lstXMarkers
  OptionsApplyXMarkers $this $lstXMarkers $tmpOpt(var,xMarkersColors) \
         $tmpOpt(var,xMarkersDashes) $tmpOpt(var,xMarkersWidths)

  OptionsApplyYMarkers  $this ::SeriesGraph::tmpOpt var
  #Save options after apply
  OptionsSet $this
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SeriesGraph::OptionsApplyDateFormat {this dateString} {
# 
#/////////////////////////////////////////////////////////////////////////////  
  set g [::bayesGraph::getActiveGraph $this]
  set sergrp [getSerGroup $this]
  $sergrp format $dateString
  $g axis view x
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SeriesGraph::OptionsApplyAxis {this lstXAxis} {
# 
#///////////////////////////////////////////////////////////////////////////// 
  set  MAXTICKS 10001.0
  set g      [::bayesGraph::getActiveGraph $this]
  set sergrp [getSerGroup $this]

  $g axis configure x -majorticks ""
  if {[llength $lstXAxis]} {   
    catch {eval $sergrp ticks add $lstXAxis}
    set _majorticks [list ]
    foreach tms $lstXAxis {
      if {![catch {::tol::info var TimeSet $tms}]} {
        set _majorticks [lunion $_majorticks [$sergrp ticks get $tms]]
      }
    }
    set _majorticks [lsort $_majorticks]
    set length [llength $_majorticks]
    if {$length >= $MAXTICKS} {
      set P [expr {int(ceil($length/$MAXTICKS))}]
      set majorticks [list]
      for {set idx 0} {$idx < $length} {incr idx $P} {
        lappend majorticks [lindex $_majorticks $idx]
      }
    } else {
      set majorticks $_majorticks
    }
    $g axis configure x -majorticks $majorticks
    catch {eval $sergrp ticks remove $lstXAxis}
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SeriesGraph::OptionsApplyXMarkers {this lstXMarkers lstColors lstDashes lstWidths} {
# 
#/////////////////////////////////////////////////////////////////////////////  
  variable tmpOpt
  set g      [::bayesGraph::getActiveGraph $this]
  set sergrp [getSerGroup $this]
  
  #foreach marker [$g marker names] {
  #  $g marker delete $marker
  #}
  
  #X-Markers
  if [llength $lstXMarkers] {
    set coltbl $lstColors
    ::bayesGraph::CreateColorTable coltbl [llength $lstXMarkers]
    set dif [expr [llength $lstXMarkers] - [llength lstDashes]]
    for {set i 0} {$i < $dif} {incr i} { 
       lappend lstDashes {2 2}
       lappend lstWidths 1
    }
    set i 0
    catch {eval $sergrp ticks add $lstXMarkers}
    foreach tms $lstXMarkers {
      if {![catch {::tol::info var [list TimeSet $tms]}]} {
        $g marker bind $tms <Enter> [list ::bayesGraph::WriteInfo $this "[mc Marker]X: $tms"]
        $g marker bind $tms <Leave> [list ::bayesGraph::WriteInfo $this ""]
        set color  [lindex $coltbl $i]
        set dashes [lindex $lstDashes $i]
        set width  [lindex $lstWidths $i]
        Tolcon_Trace "DEBUG=[$sergrp ticks get $tms]"
        foreach x [$sergrp ticks get $tms] {
          catch {$g marker create line -name ${i}_$x -coords [list $x -Inf $x Inf] \
                        -outline $color -dashes $dashes -linewidth $width -bindtags $tms}
        }
        incr i
      }
    }
    catch {eval $sergrp ticks remove $lstXMarkers}
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::SeriesGraph::OptionsApplyYMarker {this var pre marker} {
#
# PURPOSE: 
#
# PARAMETERS:
#   this  -> 
#   var   -> 
#   pre   ->
#   maker -> list with {key name} of statistic. Example: {Ave Average}
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 $var ary
  set g      [::bayesGraph::getActiveGraph $this]
  set sergrp [getSerGroup $this]
  foreach {key name} $marker { }
  for {set i 0} {$i < [$sergrp serie size]} {incr i} {
    if $ary(${pre},yMarker$key) {
      array set stats [ $sergrp serie $i stats ]
      set y $stats($name)
      if {[string equal $y "?"]} {
        tk_messageBox -parent $g -type ok -title [mc "Warning"] \
          -message [mc "Cant draw %1\$s y marker because its value is ?" [mc $name]]
      } else  {
        set nameSer [$g line cget l${i}-0 -label]
#rcsoto
        set mapY    [$g line cget l${i}-0 -mapy]
        set text "[mc Marker]Y: [mc $name] [mc of] $nameSer: $y"
        if [string equal $ary(${pre},yMarkerColor$key) "defcolor"] {
          if {[$g element type l${i}-0] eq "LineElement"} {
            set color [$g line cget l${i}-0 -color]
          } else {
            set color [$g line cget l${i}-0 -background]
          }
        } else  {
          set color $ary(${pre},yMarkerColor$key)
        }
        set dashes  $ary(${pre},yMarkerDashes$key)
        set width   $ary(${pre},yMarkerWidth$key)
        $g marker bind $name$nameSer <Enter> [list ::bayesGraph::WriteInfo $this $text]
        $g marker bind $name$nameSer <Leave> [list ::bayesGraph::WriteInfo $this ""]
        $g marker create line -name ${i}_$key -coords [list -Inf $y Inf $y]\
          -outline $color -dashes $dashes -linewidth $width \
          -bindtags $name$nameSer -mapy $mapY
      }   
    } else  {
      if [llength [$g marker names ${i}_$key]] {
        $g marker delete ${i}_$key
      }
    }
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SeriesGraph::OptionsApplyYMarkerNDev {this var pre} {
# 
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 $var ary
  set g      [::bayesGraph::getActiveGraph $this]
  set sergrp [getSerGroup $this]

  for {set i 0} {$i < [$sergrp serie size]} {incr i} {
    if $ary(${pre},yMarkerDev) {
      array set stats [ $sergrp serie $i stats ]
      set dev $stats(Std. Dev.)
      set avg $stats(Average)
      if { $dev eq "?" || $avg eq "?" } {
        tk_messageBox -parent $g -type ok -title [mc "Warning"] \
            -message [mc "Cant draw '%1\$s' y marker because its value is ?" [mc {Std. Dev.}]]
      } else {
        set y1 [expr $avg + $ary(${pre},yMarkerNDev) * $dev]
        set y2 [expr $avg - $ary(${pre},yMarkerNDev) * $dev]
        set nameSer [$g line cget l${i}-0 -label]
        
        set text1 "[mc Marker] Y: [mc "Average"] + $ary(${pre},yMarkerNDev) * [mc "Std. Dev."] [mc of] $nameSer: $y1"
        set text2 "[mc Marker] Y: [mc "Average"] - $ary(${pre},yMarkerNDev) * [mc "Std. Dev."] [mc of] $nameSer: $y2"
        if [string equal $ary(${pre},yMarkerColorDev) "defcolor"] {
          if {[$g element type l${i}-0] eq "LineElement"} {
            set color [$g line cget l${i}-0 -color]
          } else {
            set color [$g line cget l${i}-0 -background]
          }
        } else  {
            set color $ary(${pre},yMarkerColorDev)
        }
        set dashes  $ary(${pre},yMarkerDashesDev)
        set width   $ary(${pre},yMarkerWidthDev)
        $g marker bind StdDev1$nameSer <Enter> [list ::bayesGraph::WriteInfo $this $text1]
        $g marker bind StdDev2$nameSer <Enter> [list ::bayesGraph::WriteInfo $this $text2]
        $g marker bind StdDev1$nameSer <Leave> [list ::bayesGraph::WriteInfo $this ""]
        $g marker bind StdDev2$nameSer <Leave> [list ::bayesGraph::WriteInfo $this ""]
        $g marker create line -name ${i}_std1 -coords [list -Inf $y1 Inf $y1]\
                -outline $color -dashes $dashes -linewidth $width -bindtags StdDev1$nameSer
        $g marker create line -name ${i}_std2 -coords [list -Inf $y2 Inf $y2]\
                -outline $color -dashes $dashes -linewidth $width -bindtags StdDev2$nameSer
      }   
    } else {
      if [llength [$g marker names ${i}_std1]] {
        $g marker delete ${i}_std1
        $g marker delete ${i}_std2
      }
    }
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SeriesGraph::OptionsApplyYMarkers {this var pre} {
# 
#/////////////////////////////////////////////////////////////////////////////  
  foreach stat {{Ave "Average"} {Var "Varianze"} {Med "Median"} {Max "Maximun"} {Min "Minimun"}} {
    OptionsApplyYMarker $this $var $pre $stat
  }
  OptionsApplyYMarkerNDev $this $var $pre
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SeriesGraph::OptionsCheck {this} {
# 
#/////////////////////////////////////////////////////////////////////////////  
  OptionsCheckDateString $this
  #OptionsCheckDateFormat $this
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SeriesGraph::OptionsCheckDateFormat {this} {
# 
#/////////////////////////////////////////////////////////////////////////////  
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


#/////////////////////////////////////////////////////////////////////////////
proc ::SeriesGraph::OptionsCheckDateString {this} {
# 
#/////////////////////////////////////////////////////////////////////////////  
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
  proc ::SeriesGraph::ReadIni {this} {
#
# PURPOSE: Reads series graph specific options from .ini file. First of all,
#          reads options from BayesGraph. Ini file is the active graphic 
#          configuration file.
#
# PARAMETERS:
#   this  -> Instance of BayesGraph
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::options opt
  #set rini ::iniFile::Read
  set rini ::iniFile::ReadGCF
  #Leer las opciones específicas

  for  {set gr 0} {$gr<$data(gr,numGraph)} {incr gr} {
    set sergrp $data(gr,$gr,sergrp)
    set opt(gr,$gr,sergrp,dateString) [$rini SeriesGraph dateString [$sergrp format]]
    set opt(gr,$gr,sergrp,xMarkers)   [$rini SeriesGraph SxMarkers   {}]
    set opt(gr,$gr,sergrp,xMarkersColors) [$rini SeriesGraph SxMarkersColors\
         {#356a6a #804000 #05eefa #009148 #ffffbb}]
    set opt(gr,$gr,sergrp,xMarkersWidths) \
        [$rini SeriesGraph SxMarkersWidths   {1 1 1 1 1}]
    set opt(gr,$gr,sergrp,xMarkersDashes) \
        [$rini SeriesGraph SxMarkersDashes   {{1 1} {2 2} {3 3} {4 4} {5 5}}]
    foreach key {Ave Dev Var Med Max Min} {
      set opt(gr,$gr,sergrp,yMarker$key)  [$rini SeriesGraph SyMarker$key  0]
      set opt(gr,$gr,sergrp,yMarkerColor$key)  [$rini SeriesGraph SyMarkerColor$key  defcolor]
      set opt(gr,$gr,sergrp,yMarkerDashes$key) [$rini SeriesGraph SyMarkerDashes$key  {3 1}]
      set opt(gr,$gr,sergrp,yMarkerWidth$key)  [$rini SeriesGraph SyMarkerWidth$key   1]
    }
    set opt(gr,$gr,sergrp,yMarkerNDev)  [$rini SeriesGraph SyMarkerNDev   2]
    set opt(gr,$gr,sergrp,yMarkers)     [$rini SeriesGraph SyMarkers   {}]
    set opt(gr,$gr,sergrp,xAxis)        [$rini SeriesGraph SxAxis      {}]
    set opt(gr,$gr,sergrp,lineShowDescription) \
      [$rini SeriesGraph lineShowDescription 0]
  }
  #Leer las opciones de bayesGraph
  ::bayesGraph::ReadIni $this
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::SeriesGraph::WriteIni {this} {
#
# PURPOSE: Writes series graph specific options to .ini file. This procedure 
#          is called by parent BayesGraph. Ini file is the active graphic
#          configuration file.
#
# PARAMETERS:
#   this  -> Instance of BayesGraph
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::options opt
  
  set gr $data(gr,active) 
  #set wini ::iniFile::Write
  set wini ::iniFile::WriteGCF

  $wini SeriesGraph dateString $opt(gr,$gr,sergrp,dateString)
  $wini SeriesGraph SxMarkers  $opt(gr,$gr,sergrp,xMarkers)
  $wini SeriesGraph SxMarkersColors  $opt(gr,$gr,sergrp,xMarkersColors)
  $wini SeriesGraph SxMarkersWidths  $opt(gr,$gr,sergrp,xMarkersWidths)
  $wini SeriesGraph SxMarkersDashes  $opt(gr,$gr,sergrp,xMarkersDashes)
  foreach key {Ave Dev Var Med Max Min} {
    $wini SeriesGraph SyMarker$key       $opt(gr,$gr,sergrp,yMarker$key)
    $wini SeriesGraph SyMarkerColor$key  $opt(gr,$gr,sergrp,yMarkerColor$key)  
    $wini SeriesGraph SyMarkerDashes$key $opt(gr,$gr,sergrp,yMarkerDashes$key)
    $wini SeriesGraph SyMarkerWidth$key  $opt(gr,$gr,sergrp,yMarkerWidth$key)
  }
  $wini SeriesGraph SyMarkerNDev $opt(gr,$gr,sergrp,yMarkerNDev)
  $wini SeriesGraph SyMarkers  $opt(gr,$gr,sergrp,yMarkers)
  $wini SeriesGraph SxAxis     $opt(gr,$gr,sergrp,xAxis)

  $wini SeriesGraph lineShowDescription \
    $opt(gr,$gr,sergrp,lineShowDescription)
}
