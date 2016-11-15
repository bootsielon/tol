#/////////////////////////////////////////////////////////////////////////////
# FILE    : setgraph.tcl
# PURPOSE : This file 
#           It's a child of bayesGraph so inherites all its functionalities.
#/////////////////////////////////////////////////////////////////////////////

namespace eval ::SetGraph {
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SetGraph::Create {path info set tableset {fileGCF {}} {title {}}} {
#
# PURPOSE: Creates an instance of a bayesGraph and inserts the graph lines
#
# PARAMETERS:
#   path      -> path of the graph
#   info      -> list with info of series to graph. Has next syntax:
#     {vectors <vectors> names <names> minX <minX> maxX <maxX> xTicks <xTicks>}
#     where:
#       vectors ->
#       names   ->
#       xTicks  ->
#   set       -> reference to the TOL set
#   tableset  ->
#   fileGCF   -> file the graphic configuration file. Default ""
#   title     -> title of chart
#
# RETURN: The instance of the graph.
#
#/////////////////////////////////////////////////////////////////////////////
  if {[winfo exists $path] && [winfo class $path] eq "bayesGraph"} {
    set Instance "::bayesGraph::$path"
    set holdon 1
  } else {
    set Instance [::bayesGraph::Create $path g]
    set holdon 0
  }
  upvar \#0 ${Instance}::widgets widgets
  upvar \#0 ${Instance}::data data
  upvar \#0 ${Instance}::options opt

  if {!$holdon} {
    set data(namespace) SetGraph
    set data(proc,OnComputeStepX)  ::SetGraph::ComputeStepX
    set data(proc,OnEvalWindow)    ::SetGraph::EvalWindow
    set data(proc,OnToTable)       ::SetGraph::ToTable
    set data(proc,OnWriteIni)      ::SetGraph::WriteIni
    set data(proc,xAxisCommand)    [list ::SetGraph::GetTickLabel $Instance]
    set data(proc,x2AxisCommand)   [list ::SetGraph::GetTickLabel $Instance]
  }
  
  set gr 0
  set grpath $widgets(gr,$gr)

  #::SetGraphDialog::Init $tableset

  set gInfo(vectors) ""
  set gInfo(names)   ""
  set gInfo(xTicks)  ""

  #puts "***** info : ***** \n$info"
  array set gInfo $info

  if {!$holdon} {
    set data(xTicks) $gInfo(xTicks)
    
    set data(gr,$gr,set) $set
    set data(gr,$gr,tableset) $tableset
    set data(gr,$gr,OnGetXTicks)    ::SetGraph::GetXTicks  
    set data(gr,$gr,OnGetXValues)   ::SetGraph::GetXValues  
  }
  # insert the series into the graph
  if {$holdon} {
    set ii 0
    foreach item [$grpath element names] {
      regexp {l(\d+)-(\d+)} $item ==> i
      if {$i>$ii} {
        set ii $i
      }
      incr ii
    }
  } else {
    set ii  0
  }
  set i 0
  # TICKET 1450
  foreach segments $gInfo(vectors) {
    #puts "**** $i -- [llength $segments] -- $segments"
    set j  0
    set segIds  {}

    foreach seg $segments {
      set xdata [lindex $seg 0]
      set ydata [lindex $seg 1]
      $grpath line create l$ii-$j -xdata $xdata -ydata $ydata
      lappend segIds l$ii-$j
      if { $j } {
        $grpath line configure l$i-$j -label ""
      } else {
        set _label_ [lindex $gInfo(names) $i]
        foreach _it_ [$grpath element names] {
          if {$_label_ eq [$grpath element cget $_it_ -label]} {
            append _label_ "x"
          }
        }
        $grpath line configure l$ii-0 -label $_label_
      }
      incr j
    }
    incr i
    incr ii
    lappend data(gr,$gr,elements) $segIds
  }
  ::bayesGraph::BindLegend $Instance

  if {!$holdon} {
    # configurar grafico
    #  $grpath axis configure x  -command "::SetGraph::GetTickLabel $Instance" 
    
    #Establecer el fichero GCF del que se lee
    #Establecer el fichero GCF del que se lee
    if {[string length $fileGCF]} {
      ::bayesGraph::LoadGCF $Instance $fileGCF
    } else  {
      ::bayesGraph::LoadGCF $Instance
    }
  }
  ReadIni $Instance $holdon
    
  Configure $Instance
  
  if {!$holdon} {
    #set title chart
    if {[string length $title]} {
      ::bayesGraph::SetMainTitle $Instance $title
    }  
    if {[string equal [winfo class [winfo parent $widgets(this)]] Toplevel]} {
      wm geometry [winfo parent $widgets(this)] $data(geometry)
      wm state    [winfo parent $widgets(this)] $data(wm,state)
    }
  }
  ::bayesGraph::ZoomExtend  $Instance
  return $Instance
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SetGraph::Configure {this} {
# 
#/////////////////////////////////////////////////////////////////////////////
  ::bayesGraph::Configure $this
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SetGraph::ComputeStepX {this min max total} {
# 
#/////////////////////////////////////////////////////////////////////////////  
  set step 1
  # validamos que max y total tengan valor y min no
  if {(![string length $min] && [string length $max] && \
        [string length $total])} {
    set step [expr $max / $total]
  } elseif {([string length $min] && [string length $max] && \
             [string length $total])} {
    # validamos que max, min y total tengan valor
    set step [expr ($max-$min)/$total]
  }
  return $step
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SetGraph::GetTickLabel {this bltg tick} {
# 
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::options options

  if {![string equal $data(xTicks) ""]} {
    if {[string is int $tick]} {
      set str [lindex $data(xTicks) $tick]
    } else  {
      return ""
    }
    set tvalue $str
  } else {
    set tvalue $tick
  }
  set gr $data(gr,active)
  ::bayesGraph::FormatNumberAxis $options(gr,$gr,xFormatNumber) \
      $options(gr,$gr,xFormatPercent) $bltg $tvalue 
}

#/////////////////////////////////////////////////////////////////////////////
proc ::SetGraph::GetXTicks {this} {
# 
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::data data
  set gr $data(gr,active)
  set values ""
  if {![string equal $data(xTicks) ""]} {
    set values $data(xTicks)
  } else {
    set limAxis [::bayesGraph::GetLimitsValueAxis $this x]
    if {[llength $limAxis]} {
      set minX [expr int(floor([lindex $limAxis 0]))]
      set maxX [expr int(floor([lindex $limAxis 1]))]
      # If the difference between the maximum and the minimum is very great,
      # we remained with a sample
      set inc [expr ($maxX-$minX) > 1000 ? ($maxX - $minX) / 1000 : 1]
      for {set i $minX} {$i <= $maxX} {incr i $inc} {
        lappend values $i
      }
    }    
  }
  return $values
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SetGraph::GetXValues {this} {
# 
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::data data
  # si es unique X Axis
  set gr $data(gr,active)
  set values ""
  set limAxis [::bayesGraph::GetLimitsValueAxis $this x]
  set minX [expr int(floor([lindex $limAxis 0]))]
  set maxX [expr int(floor([lindex $limAxis 1]))]
  # If the difference between the maximum and the minimum is very great,
  # we remained with a sample
  set inc [expr ($maxX-$minX)>1000 ? ($maxX - $minX) / 1000 : 1]
  for {set i $minX} {$i <= $maxX} {incr i $inc} {
    lappend values $i
  }   
  return $values
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SetGraph::getSet {this} {
# 
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::data data

  set gr $data(gr,active)
  set set $data(gr,$gr,set)
  return $set
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SetGraph::getTableSet {this} {
# 
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::data data

  set gr $data(gr,active)
  set tableset $data(gr,$gr,tableset)
  return $tableset
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SetGraph::EvalWindow {this} {
# 
#/////////////////////////////////////////////////////////////////////////////  
  set grpath [::bayesGraph::getActiveGraph $this]
  set set [getSet $this]
  if {[llength $set] > 1} {
    set nameSet [lindex [::tol::info variable $set] 1]
  } else {
    set nameSet $set
  }
  return $nameSet
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SetGraph::ToTable {this} {
# 
#/////////////////////////////////////////////////////////////////////////////  
  set grpath [::bayesGraph::getActiveGraph $this]
  set set [getSet $this]
  set tableset [getTableSet $this]

  if {[llength $set] > 1} {
    set nameSet [lindex [::tol::info variable $set] 1]
  } else {
    set nameSet $set
  }
 
  set tl .tbl_from_$nameSet
  if [winfo exists $tl] {
     raise $tl
  } else  {
	set tl [::project::CreateForm \
                    -title    "[mc {Set table}]: $nameSet" \
                    -iniconfig Tables \
                    -type     Tables ]

    $tl withdraw
	set tlf [$tl getframe]
    btable $tlf.t -parent $tl \
                  -posteditcmd [list ::TolConsole::TableRecentUpdate $tlf.t] \
                  -iniconfig btable
    $tlf.t fillSet $tableset
    pack $tlf.t -fill both -expand yes
    update
    $tl bind <Control-F4> [list $tl kill]
	$tl restore
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SetGraph::ReadIni {this {holdon 0}} {
#
# PURPOSE: Reads set graph specific options from .ini file. First of all, 
#          reads options from BayesGraph. Ini file is the active graphic
#          configuration file.
#
# PARAMETERS:
#   this  -> Instance of BayesGraph
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data

  if {!$holdon} {
    set rini ::iniFile::ReadGCF
    set data(geometry)   [$rini SetGraph geometry 800x600]
    set data(wm,state)   [::iniFile::ReadWMState $rini SetGraph]
  }
  ::bayesGraph::ReadIni $this $holdon
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::SetGraph::WriteIni {this} {
#
# PURPOSE: Writes set graph specific options to .ini file. This procedure 
#          is called by parent BayesGraph. Ini file is the active graphic
#          configuration file.
#
# PARAMETERS:
#   this  -> Instance of BayesGraph
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::widgets widgets

  set wini ::iniFile::WriteGCF

  if {[string equal [winfo class [winfo parent $widgets(this)]] Toplevel]} {
    set data(geometry) [wm geometry [winfo parent $widgets(this)]]
    set data(wm,state) [wm state    [winfo parent $widgets(this)]]
  }
  $wini SetGraph geometry $data(geometry)
  $wini SetGraph wm,state $data(wm,state)
}


#/////////////////////////////////////////////////////////////////////////////
  namespace eval ::SetGraphDialog {
# PURPOSE: The purpose of this namespace is to manage the columns selecting 
#          process of a set.
#
# VARIABLES:
#   tmpOpt -> temporary array used to store the widgets and variables of the
#             assistant. 
#   data   -> array used to store information about the set and its columns
#     tableset     -> command of the set
#     names        -> list with names of the elements selected for drawing
#     vectors      -> list with pairs
#     nameVectors  -> list with vector names created when partition is
#                     necessary and when a type 2 column is choosen for x-axis
#     xTicks       -> list with ticks of the x values when a type 2 column is
#                     choosen for x-axis
#     selected     -> temporary list with the pair of x-y columns selected for
#                     drawing
#     uniqueXAxis  -> flag that indicates if the user has choosen an element
#                     which X data comes from a type 2 column. (if so, it
#                     stores the column's index in the set)
#     infoCols     -> array with information of '$tableset info columns'
#                     command.
#     idxColsType1 -> list of type 1 column's indexes in the set
#     idxColsType2 -> list of type 2 column's indexes in the set
#     idxColsType3 -> list of type 3 column's indexes in the set
#     idxColsType4 -> list of type 4 column's indexes in the set
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpOpt
  variable data
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::SetGraphDialog::InitData {tableset} {
#
# PURPOSE: Checks if the set is graphicable and if so, draws and assistant 
#          that allows to choose x and y columns for the elements to draw.
#
# PARAMETERS:
#   tableset -> command of the set
#
#/////////////////////////////////////////////////////////////////////////////
  variable data

  set data(tableset)    $tableset
  set data(names)       ""
  set data(vectors)     ""
  set data(nameVectors) ""

  set data(xTicks)      ""
  set data(selected)    ""
  set data(uniqueXAxis) ""

  set data(firstcolumn) {}

  GetColTypes $tableset
}
      
#/////////////////////////////////////////////////////////////////////////////
proc ::SetGraphDialog::DrawSet {path tableset set type {pairs {}} {names {}} {fileGCF {}} {title {}}} {
#
# PURPOSE: Checks if the set is graphicable and if so, draws and assistant 
#          that allows to choose x and y columns for the elements to draw.
#
# PARAMETERS:
#   path     -> path of the rbc graph
#   tableset -> command of the set
#   set      -> reference to the TOL set
#   type     -> 0 for custom association of data columns
#            -> 1 for all data columns to first one
#            -> 2 for all data columns to generic
#            -> 3 for even data columns to odd ones
#   pairs    -> when type is 0 pairs is the list of correspondence beetwen index
#               columns to data columns (x-y pairs)
#   names    -> names for the series/points,..
#   fileGcf  -> file configuration graph
#   title    -> list of title of chart
#   holdon   -> tell if the new graph goes into a new context or an existing one
#
#/////////////////////////////////////////////////////////////////////////////
  variable data

  #puts "**** ::SetGraphDialog::DrawSet $tableset type=$type"

  ::SetGraphDialog::InitData $tableset

  if {![CheckGraphicable] } {
      tk_messageBox -type ok -icon warning -title [mc Warning]\
          -message [mc "Set not graphicable"]
      return ""
  }
  # fill listX and listY
  # TICKET 1450
  #::SetGraphDialog::SetDataVarList
  switch $type {
    0 {
      # custom
      set lstLines {}
      set len [llength $pairs]
      for {set i 0} {[expr $i < $len]} {incr i} {
        set xy   [lindex $pairs $i]
        set name [lindex $names $i]
        if {![string length $name]} {
          set name "Column [expr $i + 1]"
        }
        lappend lstLines [::SetGraphDialog::AddLine \
                              [expr [lindex $xy 0] - 1] \
                              [expr [lindex $xy 1] - 1] \
                              $name          \
                              $lstLines] 
      }
    }
    1 {
      # all to one
      ::SetGraphDialog::GrapthAllLine 1 0 $names
    }
    2 {
      # all to generic
      ::SetGraphDialog::GrapthAllLine 0 0 $names
    }
    3 {
      # even to odd
      ::SetGraphDialog::GrapthEvenUneven $names
    }
  } 
  # build info to graph
  if [llength $data(selected)] {
    ::SetGraphDialog::SetSelection
    set info [list vectors $data(vectors) names $data(names) xTicks $data(xTicks)]
    set instance [::SetGraph::Create $path $info $set $tableset $fileGCF $title]
    bind $path <Destroy> +[list ::SetGraphDialog::OnDestroyGraph $data(nameVectors)]
    return $instance
  } else {
    set msg [mc "There are no data to draw"] 
    ::TolConsole::HciWriter "<W>$msg</W>"
    return ""
  }
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::SetGraphDialog::Init {path tableset set} {
#
# PURPOSE: Checks if the set is graphicable and if so, draws and assistant 
#          that allows to choose x and y columns for the elements to draw.
#
# PARAMETERS:
#   path     -> path of the rbc graph
#   tableset -> command of the set
#   set      -> reference to the TOL set
#
#/////////////////////////////////////////////////////////////////////////////
  variable data

  ::SetGraphDialog::InitData $tableset

  if {![CheckGraphicable]} {
    tk_messageBox -type ok -icon warning -title [mc Warning]\
        -message [mc "Set not graphicable"]
    return 0
  }
  DrawDialog
  if [llength $data(vectors)] {
    set info [list vectors $data(vectors) names $data(names) xTicks $data(xTicks)]
    set Instance [::SetGraph::Create $path $info $set $tableset]
    bind $path <Destroy> +[list ::SetGraphDialog::OnDestroyGraph $data(nameVectors)]
    return $Instance
  } else {
    return 0
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::SetGraphDialog::OnDestroyGraph {list} {
# 
#/////////////////////////////////////////////////////////////////////////////  
  if [llength $list] {
    eval rbc::vector destroy $list
  }
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::SetGraphDialog::GetColTypes {tableset} {
#
# PURPOSE: Extracts information about the types of the columns of the tableset
#          and stores this in the 'data' array of the namespace. There are
#          four types of columns:
#            1- Graficable in X and Y axis (Real without ? or Inf)
#            2- Graficable only in X axis  (Text or Date)
#            3- Graficable only in Y axis  (Real with ?)
#            4- Not graficable             (Real with Inf)
#
# PARAMETERS:
#   tableset  -> command of the tableset
#
#/////////////////////////////////////////////////////////////////////////////

  variable data

  set data(var,listX) \
    [list [list -1 "<Generic 1-[$tableset info datarows]>"]]

  set data(var,listY) {}

  set idx 0
  for {set i 1} {$i <= 4} {incr i} {
    set data(idxColsType$i) ""
    set data(nameColsType$i) ""
  }
  foreach it [$tableset info columns] {
    #puts "**** it = $it"
    foreach {key value} $it {
      set data(infCols,$idx,$key) $value
    }
    switch -- $data(infCols,$idx,type) {
      Date -
      Text {
        # first column
        if {![info exists data(firstcolumn)] || ![string length $data(firstcolumn)]} {
          set data(firstcolumn) $idx
        }

        lappend data(idxColsType2) $idx
        lappend data(var,listX) [ list $idx $data(infCols,$idx,name) ]
      }
      Real {
        # first column
        if {![info exists data(firstcolumn)] || ![string length $data(firstcolumn)]} {
          set data(firstcolumn) $idx
        }
        if {$data(infCols,$idx,infinity)} {
          lappend data(idxColsType4) $idx
#         lappend data(idxColsType1) $idx
          lappend data(var,listY) [ list $idx $data(infCols,$idx,name) ]
        } elseif {$data(infCols,$idx,unknown)} {
          lappend data(idxColsType3) $idx
          lappend data(var,listY) [ list $idx $data(infCols,$idx,name) ]
        } else {
          lappend data(idxColsType1) $idx 
          lappend data(var,listX) [ list $idx $data(infCols,$idx,name) ]
          lappend data(var,listY) [ list $idx $data(infCols,$idx,name) ]
        }
      }
    }
    incr idx
  }
}

#///////////////////////////////////////////////////////////////////////////// 
  proc ::SetGraphDialog::CheckGraphicable {} {
#
# PURPOSE: Check if the set is graphicable and returns a code 
# 
# RETURN:
#    0 if the set isn't graphicable
#    !0 if there are some posibilities for choosing columns (assistant will be
#   drawn)
#///////////////////////////////////////////////////////////////////////////// 
  variable data

  # there must be at least Real numbers
  return [expr {[llength $data(idxColsType1)] + \
                    [llength $data(idxColsType3)] + \
                    [llength $data(idxColsType4)]}]
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::SetGraphDialog::DrawDialog {} {
#
# PURPOSE: Draw an assistant to select the columns to plot
#
#/////////////////////////////////////////////////////////////////////////////
  variable data
  variable tmpOpt

  #puts "******* ::SetGraphDialog::DrawDialog"
  set dialog [Dialog .t -title [mc "Columns Selector"] -separator yes\
                -modal local -parent .]
  set tmpOpt(dialog) $dialog

  wm protocol $dialog WM_DELETE_WINDOW [list ::SetGraphDialog::Cancel $dialog]
  set w [$dialog getframe]

  set f1 [frame $w.f1]
  
  # fill listX and listY
  # TICKET 1450
  #::SetGraphDialog::SetDataVarList

  set values ""
  set lst    ""
  foreach it $data(var,listX) {
    lappend values [lindex $it 1]
    lappend lst    [lindex $it 0]
  }
  label $f1.lXAxis -text [mc "X Values"]:
  set tmpOpt(widget,cbXAxis) [ComboBox $f1.cbXAxis -width 16 -height 5\
     -editable false -justify right\
     -values $values\
     -modifycmd  [list ::SetGraphDialog::OnModifyXValues $lst ]]

  set values ""
  set lst    ""
  foreach it $data(var,listY) {
    lappend values [lindex $it 1]
    lappend lst    [lindex $it 0]
  }

  label $f1.lInfoSelect -text [mc "Select series"]:
  label $f1.lYAxis      -text [mc "Y Values"]:
  set tmpOpt(widget,cbYAxis) [ComboBox $f1.cbYAxis -width 16 -height 5\
     -editable false -justify right\
     -values $values\
     -modifycmd  [list ::SetGraphDialog::OnModifyYValues $lst ]]
  label $f1.lName -text [mc Name]:
  set tmpOpt(widget,eName) [entry $f1.eName -width 18]

  grid $f1.lInfoSelect   -           -sticky nws -pady 2 
  grid $f1.lXAxis        $f1.cbXAxis -sticky news -pady 2 
  grid $f1.lYAxis        $f1.cbYAxis -sticky news -pady 2
  grid $f1.lName         $f1.eName   -sticky news -pady 2 

  grid rowconfigure    $f1 4 -weight 1
  grid columnconfigure $f1 1 -weight 1

  set f2 [frame $w.f2]
  label $f2.lSeries -text [mc "Series to draw"]:

  set tmpOpt(widget,lbSeries) [blistboxplus $f2.lb \
    -selectmode multiple -nomenu true \
    -cols { lines } \
    -readonly false -parent $f2 -orientation vertical]
  bind $tmpOpt(widget,lbSeries) <Button-1> +[list focus -force %W]

  button $f2.bAdd -width 6 -text [mc Add]    -command ::SetGraphDialog::AddItem
  button $f2.bDel -width 6 -text [mc Delete] -command ::SetGraphDialog::DelItem

  grid $f2.lSeries  -     -sticky nw    -pady 0
  grid $f2.lb       -     -sticky news  -pady 0
  grid $f2.bAdd $f2.bDel -sticky new   -pady 0

  grid rowconfigure    $f2 1 -weight 1
  grid columnconfigure $f2 0 -weight 1
  grid columnconfigure $f2 1 -weight 1

  # options advanced
  TitleFrame $w.f3 -text [mc "Automatic generation"]  
  set f3 [$w.f3 getframe]

  radiobutton $f3.rbAllFirst -text [mc SetGraphAllFirst] \
     -value f -highlightthickness 0 -pady 0 \
     -variable ::SetGraphDialog::data(var,genAut)
  DynamicHelp::add $f3.rbAllFirst -text [mc SetGraphAllFirstHint]
  radiobutton $f3.rbAllGeneric -text [mc SetGraphAllGeneric] \
     -value g -highlightthickness 0 -pady 0\
     -variable ::SetGraphDialog::data(var,genAut)
  DynamicHelp::add $f3.rbAllGeneric -text [mc SetGraphAllGenericHint]
  radiobutton $f3.rbEvenUneven -text [mc SetGraphEvenUneven] \
     -value e -highlightthickness 0 -pady 0 \
     -variable ::SetGraphDialog::data(var,genAut)
  DynamicHelp::add $f3.rbEvenUneven -text [mc SetGraphEvenUnevenHint]
  Button $f3.bAddGenAut -text [mc Add] \
    -width 6 -command [list ::SetGraphDialog::AddGenAut]

  grid $f3.rbAllFirst    -              -sticky nws  -pady 1
  grid $f3.rbAllGeneric  -              -sticky nws  -pady 1
  grid $f3.rbEvenUneven  $f3.bAddGenAut -sticky nws  -pady 1

  grid rowconfigure    $f3 0 -weight 1
  grid rowconfigure    $f3 1 -weight 1
  grid rowconfigure    $f3 2 -weight 1
  grid columnconfigure $f3 0 -weight 1

  set w_bbox \
    [ButtonBox $w.bb -orient horizontal -spacing 10 -padx 0 -pady 0 \
    -homogeneous false]
  $w_bbox add -text [mc "Accept"] -relief link \
    -helptext [mc "Accept"] \
    -image [::Bitmap::get accept] -compound left -padx 5 -pady 1 \
    -command [list ::SetGraphDialog::Finish $dialog]
  $w_bbox add -text [mc "Cancel"] -relief link \
    -helptext [mc "Cancel"]\
    -image [::Bitmap::get cancelx] -compound left -padx 5 -pady 1 \
    -command [list ::SetGraphDialog::Cancel $dialog]
  
  grid $f1   $f2 -sticky news
  grid $w.f3 -   -sticky news
  grid $w_bbox -   -sticky news

  grid rowconfigure    $w 0 -weight 1
  grid columnconfigure $w 0 -weight 1
  grid columnconfigure $w 1 -weight 3

  bind $dialog <F1> "ShowHelp Tb4GraSetSel"
  $dialog draw
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::SetGraphDialog::SetDataVarList {} {
#
# PURPOSE: Fill varible data(var,listX) and data(var,listY)
#
#/////////////////////////////////////////////////////////////////////////////
  variable data

  # fill data(var,listX)
  set data(var,listX) {}

  set data(var,listX) \
    [list [list -1 "<Generic 1-[$data(tableset) info datarows]>"]]

  foreach it [concat $data(idxColsType2) $data(idxColsType1)] {
    lappend data(var,listX) [list $it $data(infCols,$it,name)]
  }

  if {![string equal [$data(tableset) info header] "generic"] } {
    set data(var,listX) [lsort -index 1 $data(var,listX)]
  }
  #set data(var,listX) [lsort $data(var,listX)]  

  # fill data(var,listY)
  set data(var,listY) {}
  foreach it [concat $data(idxColsType1) $data(idxColsType3)] {
    lappend data(var,listY) [list $it $data(infCols,$it,name)]
  }
  if {![string equal [$data(tableset) info header] "generic"] } {
    set data(var,listY) [lsort -index 1 $data(var,listY)]
  }
  #set data(var,listY) [lsort $data(var,listY)]
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::SetGraphDialog::AddGenAut {} {
#
# PURPOSE: Control de la opción seleccionada para la generacion automatica de
#          series
#
#/////////////////////////////////////////////////////////////////////////////
  variable data
  switch -exact $data(var,genAut) {
    f {
      ::SetGraphDialog::GrapthAllLine 1 0
    }
    g {
      ::SetGraphDialog::GrapthAllLine 0 0
    }
    e {
      ::SetGraphDialog::GrapthEvenUneven
    }
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::SetGraphDialog::GrapthAllLine {indexX rangeY {names {}}} {
#
# PURPOSE: Seleccionamos todas las series contra una de ellas
#
# PARAMETERS: indexX -> Indice en X con el que se cruzan las demas columnas
#             rangeY -> Rango inicial de columnas a usar en Y
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpOpt
  variable data
  # first line ?
  if {[expr $indexX == 1]} {
    set firLinX $data(firstcolumn)
  } else {
    set firLinX [lindex [lindex $data(var,listX) $indexX] 0]
  }
  # control: uniqueXAxis
  set ok [::SetGraphDialog::CheckAddSerie $firLinX]
  if {!$ok} {
    if {[info exists tmpOpt(dialog)]} {
      set parent $tmpOpt(dialog)
    } else {
      set parent .
    }
    set parent  
    set msg "[mc {Item not selectable}].\n[mc UniqueXAxis]"
    tk_messageBox -type ok -icon warning -message $msg\
                  -parent $parent        -title [mc Warning] 
  } else {
    # find line axis X ?
    if {([lindex $firLinX 0]==-1) ||
        ([lsearch -exact $data(idxColsType1) $firLinX]!=-1) ||
        ([lsearch -exact $data(idxColsType2) $firLinX]!=-1)} {
      # remove first line
      set row 0
      set lst [lrange $data(var,listY) $rangeY end]
      foreach it $lst {
        # line is graph Y ?
        if {([lsearch -exact $data(idxColsType1) [lindex $it 0]]!=-1) ||
            ([lsearch -exact $data(idxColsType3) [lindex $it 0]]!=-1)} {
          if {![string equal $firLinX [lindex $it 0]]} {              
            set name [lindex $names $row]
            if {![string length $name]} {
              set name [lindex $it 1]
            }
            lappend data(selected) [list $firLinX [lindex $it 0] $name]
            if {([info  exists  tmpOpt(widget,lbSeries)]) && \
                ([winfo exists $tmpOpt(widget,lbSeries)])} { 
              $tmpOpt(widget,lbSeries) insert end \
                [list "$name ( X: $firLinX Y: [lindex $it 0] )"]
            }
            incr row            
          }
        } else {
        # no se puede graficar
Tolcon_Trace "Y. No se puede graficar: $it"  
        }
      }
    } else {
    # No se puede graficar
Tolcon_Trace "X. No se puede graficar [lindex $data(var,listX) $indexX]: $firLinX"
    }
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::SetGraphDialog::GrapthEvenUneven {{names {}}} {
#
# PURPOSE: Seleccionamos las serie impar contr la impar y las agregamos
#          a la lista series a dibujar. Por ejemplo 1-2, 3-4, 5-6 
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpOpt
  variable data
  # travel all columuns
  #puts "DEBUG: ::SetGraphDialog::GrapthEvenUneven"
  set row 0

  # TICKET 1450: que cosa mas rara se hace aqui!!!!!!!
  for {set i 0} {$i<[expr [llength $data(var,listY)] - 1]} {incr i 2} {
    set colX [lindex [lindex $data(var,listY) $i] 0]
    set colY [lindex $data(var,listY) [expr $i + 1]]
    # control: uniqueXAxis
    set ok [::SetGraphDialog::CheckAddSerie $colX]
    if {$ok} {
      # find line axis X ?
      if {([lsearch -exact $data(idxColsType1) $colX]!=-1) ||
          ([lsearch -exact $data(idxColsType2) $colX]!=-1)} {
        # line is graph Y ?
        if {([lsearch -exact $data(idxColsType1) [lindex $colY 0]]!=-1) ||
            ([lsearch -exact $data(idxColsType3) [lindex $colY 0]]!=-1)} {
          set name [lindex $names $row]
          if {![string length $name]} {
            set name [lindex $colY 1]
          }
          lappend data(selected) [list $colX [lindex $colY 0] $name]
          if {(([info  exists  tmpOpt(widget,lbSeries)]) && \
               ([winfo exists $tmpOpt(widget,lbSeries)]))} {
            $tmpOpt(widget,lbSeries) insert end \
              [list "$name ( X: $colX Y: [lindex $colY 0] )"]
          }
        } else {
          # no se puede graficar
Tolcon_Trace "Y. No se puede graficar: $colY"  
        }
      } else {
      # No se puede graficar
Tolcon_Trace "X. No se puede graficar: $colX"
      }
    }
    incr row
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::SetGraphDialog::CheckAddSerie {indexX} {
#
# PURPOSE: Comprobamos si una serie se puede anhadir a la lista de seleccion
#
# PARAMETERS: indexX -> index column X
#
# RETURN: true  -> ok
#         false -> not added
#
#/////////////////////////////////////////////////////////////////////////////
  variable data
  set ok 1
  # si el elemento que intento insertar es de tipo 2
  if {[string length $data(uniqueXAxis)]} {
    if {$data(uniqueXAxis)!=$indexX} {
      set ok 0
    }
  } else {
    if {[string length $data(selected)]} {
      if {[lsearch -exact $data(idxColsType1) $indexX] == -1} {
        # trere are items selection with type 1
        set ok 0
      }
    } else {
      if {($indexX==-1) ||
          ([lsearch -exact $data(idxColsType2) $indexX] != -1)} {
        set data(uniqueXAxis) $indexX
      } else {
        # Type 1
      }
    }
  }
  return $ok
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::SetGraphDialog::OnModifyXValues {list} {
#
# PURPOSE: Update variable tmpOpt(var,cbXAxis) with the index of the column
#          when combo that shows names of possible X columns is changed
#
# PARAMETERS:
#   list -> list of tranlated values (indexes) of the columns names
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpOpt

  ::Objects::ComboUnTranslate $tmpOpt(widget,cbXAxis) $list\
      ::SetGraphDialog::tmpOpt(var,cbXAxis)
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::SetGraphDialog::OnModifyYValues {list} {
#
# PURPOSE: Update variable tmpOpt(var,cbYAxis) with the index of the column
#          when combo that shows names of possible Y columns is changed.
#          It also changes the default name of the element
#
# PARAMETERS:
#   list -> list of tranlated values (indexes) of the columns names
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpOpt

  ::Objects::ComboUnTranslate $tmpOpt(widget,cbYAxis) $list\
      ::SetGraphDialog::tmpOpt(var,cbYAxis)
  CheckName
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::SetGraphDialog::CheckName {} {
#
# PURPOSE: Sets the element's default name to the y column's name 
#
#/////////////////////////////////////////////////////////////////////////////

  variable tmpOpt
  $tmpOpt(widget,eName) delete 0 end
  $tmpOpt(widget,eName) insert 0 [$tmpOpt(widget,cbYAxis) cget -text]
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::SetGraphDialog::AddItem {} {
#
# PURPOSE:  Adds a element to the selected series listbox checking if it can
#          be added.
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpOpt
  variable data
  if { [$tmpOpt(widget,cbXAxis) getvalue] == -1} {
    tk_messageBox -type ok -icon warning -message [mc "You must fill X Values"]\
                  -parent $tmpOpt(dialog) -title [mc Warning] 
    return
  }
  if { [$tmpOpt(widget,cbYAxis) getvalue] == -1} {
    tk_messageBox -type ok -icon warning -message [mc "You must fill Y Values"]\
                  -parent $tmpOpt(dialog) -title [mc Warning] 
    return
  }
  if {[string equal [$tmpOpt(widget,eName) get] ""]} {
    tk_messageBox -type ok -icon warning -message [mc "You must specify a name"]\
                  -parent $tmpOpt(dialog) -title [mc Warning] 
    return
  }
 
  #  uniqueXAxis es un flag que indica que se ha elegido un elemento cuyos datos
  # en X provienen de una columna de tipo 2 (guardamos su indice)
  #  En los comentarios de este bloque diremos que un elemento es de tipo 2 si sus
  # datos para el eje X provienen de una columna tipo 2 (solo graficable en X)

  # lst names
  set lstNames {}
  set inx [$tmpOpt(widget,lbSeries) get 0 end]
  foreach it $inx {
    lappend lstNames [$tmpOpt(widget,lbSeries) get $it]
  }
  set name [::SetGraphDialog::AddLine \
    $tmpOpt(var,cbXAxis) \
    $tmpOpt(var,cbYAxis) \
    [$tmpOpt(widget,eName) get]        \
    $lstNames]
  if {[string length $name]} {
    $tmpOpt(widget,lbSeries) insert end [list $name]
  }
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::SetGraphDialog::AddLine {colX colY name lstLines} {
#
# PURPOSE:  Adds a element to the selected series listbox checking if it can
#          be added.
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpOpt
  variable data
  # parent ?
  if {[info exists tmpOpt(dialog)]} {
    set parent $tmpOpt(dialog)
  } else {
    set parent .
  }
  # uniqueXAxis es un flag que indica que se ha elegido un elemento cuyos
  #  datos en X provienen de una columna de tipo 2 (guardamos su indice)
  # En los comentarios de este bloque diremos que un elemento es de tipo 2 si
  #  sus datos para el eje X provienen de una columna tipo 2
  #  (solo graficable en X)

  # si no hay ningun elemento tipo 2
  if {[string equal $data(uniqueXAxis) ""]} {
    # si el elemento que intento insertar es de tipo 2
    if {(($colX == -1) ||
         ([lsearch -exact $data(idxColsType2) $colX] != -1))} {
      # si no hay ningún elemento insertado
      if {[llength $lstLines]} {
        set msg "[mc {Item not selectionable}].\n[mc UniqueXAxis]"
        tk_messageBox -type ok -icon warning -message $msg\
             -parent $parent -title [mc Warning] 
        return {}
      } else {
        set data(uniqueXAxis) $colX
      }
    }
  # si ya hay algún elemento tipo 2
  } else {
    # si el que intento insertar no es el mismo que el que ya hay
    if { ![string equal $data(uniqueXAxis) $colX] } {
      set msg "[mc {Item not selectionable}].\n[mc UniqueXAxis]"
      tk_messageBox -type ok -icon warning -message $msg\
                    -parent $parent -title [mc Warning] 
      return {}
    }
  }

  if {[lsearch $name $lstLines] != -1} {
    tk_messageBox -type ok -icon warning\
        -parent $parent -title [mc Warning]\
        -message [mc "You have added another serie with this name"]
#    return {}
  }
  lappend data(selected) [list $colX $colY $name]
  return "$name ( X: $colX Y: $colY )"
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::SetGraphDialog::DelItem {} {
#
# PURPOSE: Deletes an elements selected from the selected series listbox
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpOpt
  variable data
  set idx [$tmpOpt(widget,lbSeries) curselection]
  if {![llength $idx]} {
    tk_messageBox -type ok -icon warning -message [mc "You must select a serie"]\
                  -parent $tmpOpt(dialog) -title [mc Warning]   
  } else {
    set ids [lsearch -all -regexp [$tmpOpt(widget,lbSeries) get] \
      [join [ListToStr $idx " " ^ $] |]]
    for {set i [expr [llength $ids] - 1]} {[expr $i >= 0]} { incr i -1} {
      set id [lindex $ids $i]
      set data(selected) [lreplace $data(selected) $id $id]
    }
    foreach id $idx {
      $tmpOpt(widget,lbSeries) delete $id
    }
    # si eliminamos todos los elementos
    if {[$tmpOpt(widget,lbSeries) size]==0} {
      set data(uniqueXAxis) ""
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::SetGraphDialog::Finish {dialog} {
#
# PURPOSE: Creates the rbc vectors acording to the columns selection done by
#          the user. It also get the maximun and minimun x value of the
#          selected series
#
# PARAMETERS:
#   dialog   -> path of the dialog
#
#/////////////////////////////////////////////////////////////////////////////
  ::SetGraphDialog::SetSelection
  Cancel $dialog
}

# TICKET #1450: aqui es donde se ponen los datos que luego se pintan.
#/////////////////////////////////////////////////////////////////////////////
  proc ::SetGraphDialog::SetSelection {} {
#
# PURPOSE: Creates the rbc vectors acording to the columns selection done by
#          the user. It also get the maximun and minimun x value of the
#          selected series
#
# PARAMETERS:
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpOpt
  variable data
 
  #puts "DEBUG: ::SetGraphDialog::SetSelection"
  set colX [lindex [lindex $data(selected) 0] 0]
  # Si la columna que fija el X del primer elemento es genérica o de tipo 2
  # (será la misma para todas)
  if {($colX == -1) || ([lsearch -exact $data(idxColsType2) $colX] != -1)} {
    for {set i 0} {$i < [$data(tableset) info datarows]} {incr i} {
      lappend lst $i
    }
    if {($colX != -1)} {
      set data(xTicks) [$data(tableset) data column $colX]
    }
    set uniqueXAxis 1
  } else  {
    set uniqueXAxis 0
  }

  # cross the selection
  foreach it $data(selected) {
    set colX [lindex $it 0]
    set colY [lindex $it 1]
    set name [lindex $it 2]
    if {$uniqueXAxis} {
      set vecX $lst
    } else {
      set vecX  [$data(tableset) data column $colX]
    }
    set vecsY [$data(tableset) data column $colY]
    if {[llength $vecsY] > 1} {
      set i 0
      set subVectors ""
      set subcurves {}
      set lstX {}
      set segX {}
      set lstY {}
      set segY {}
      foreach item $vecsY {
        #si no es un omitido (vacio o ?)
        if {[string equal $item ""] || [string equal $item "?"]} {
          # TICKET 1450
          # close this segment
          if { [ llength $segX ] } {
            lappend subcurves [ list $segX $segY ]
            set segX {}
            set segY {}
          }
        } else  {
          # TICKET 1450
          #set datX [lindex $vecX $i] 
          #lappend lstX $datX 
          #lappend lstY $item
          lappend segX [lindex $vecX $i]
          lappend segY $item
        }
        incr i
      }
      # TICKET 1450
      #lappend data(vectors) [list $lstX $lstY]
      if { [ llength $segX ] } {
        lappend subcurves [ list $segX $segY ]
      }
      lappend data(vectors) $subcurves
    } else {
      #puts "****** lappend data(vectors) [list [list $vecX $vecsY]]"
      lappend data(vectors) [list [list $vecX $vecsY]]
    }
    lappend data(names) $name
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::SetGraphDialog::Cancel {dialog} {
#
# PURPOSE: Destroy the dialog
#
# PARAMETERS:
#   dialog   -> path of the dialog
#
#/////////////////////////////////////////////////////////////////////////////
  destroy $dialog
}
