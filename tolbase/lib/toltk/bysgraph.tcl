#/////////////////////////////////////////////////////////////////////////////
# FILE    : bysgraph.tcl
# PURPOSE : This file contains a namespace called bayesGraph with its
#           functions. Its for creating a graph and managing its events and 
#           changing its options. It is used as parent for other namespaces
#           that fills graphs elements. Its abstracts the general funcionality
#           of all kind of graphs.
#/////////////////////////////////////////////////////////////////////////////


#/////////////////////////////////////////////////////////////////////////////
# TESTING
#/////////////////////////////////////////////////////////////////////////////
# Put if {1} for testing this file.
if {0} {
  package require Tk
  package require BWidget
  package require BLT  
  Bitmap::use
  set toltk_script_path [file dirname [info script]]
  set toltk_images_path [file join $toltk_script_path images]
  lappend Bitmap::path $toltk_images_path
  package require msgcat
  namespace import msgcat::*
  source bysgropt.tcl
}

package require Img

#/////////////////////////////////////////////////////////////////////////////
  namespace eval ::bayesGraph {
#
# PURPOSE: The purpose of this namespace is organize code and functions.
#          A special namespace is created for each instance of bayesGraph.
#
# VARIABLES:
#   tmpOpt  -> temporary array used to store the widgets and variables of the
#              options dialog. 
#   instance -> asociative array of blt graph widget's path with the instance
#              of bayesGraph.
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpOpt
  variable instance
 
  variable optLinesGen [list \
    linehides        linetypes        barwidth       linewidth \
    dashes           smooth           barrelief      borderwidth \
    colors           areaTypes        areaTypesBar   areaTiles \
    areaForegrounds  areaBackgrounds  stippleimages  stippledegradeds]
  variable optLinesData [list \
    showvalues      valuefont    valuerotate  valuecolor \
    valueformatdec  valueshadow  valueanchor]
  variable optLinesSymbol [list \
    showsymbols  symbol   pixels        scalesymbols \
    fill         outline  outlinewidth]
  variable optLines "$optLinesGen $optLinesData $optLinesSymbol"
  variable optLinesAll "$optLines xyMappedAxis"
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::Create {path {structure g} args} {
#
# PURPOSE: Creates an instance of a bayesGraph. It creates the widget composed
#          by several panels (one for each graph), a toolbar and a statusbar.
#
# PARAMETERS:
#   path      -> 
#   structure -> 
#
# RETURN: BayesGraph instance
#
#/////////////////////////////////////////////////////////////////////////////
  variable instance
  global tcl_platform toltk_images_path
  set Instance ::bayesGraph::$path

  #  A namespace is created for each bayes graph widget.
  namespace eval $Instance {
    # data -> array with information about the instance:
    #
    # options -> 
    #
    # widgets -> 

    variable data
    variable options
    variable widgets
  }

  upvar \#0 ${Instance}::widgets widgets
  upvar \#0 ${Instance}::data data
  upvar \#0 ${Instance}::options options

  array set opts {
    -photo 0
  }
  array set opts $args
  set data(onlyphoto) $opts(-photo)
  # se crea el frame del gráfico  
  set widgets(this)  [frame $path -class bayesGraph]
  bind $widgets(this) <Destroy> [list ::bayesGraph::Destroy $Instance]
  set widgets(frame) ${Instance}::$path
  # el gráfico consta de 3 sub-frames
  # 1.- frame de la toolbar 
  set widgets(tb,path) [frame $widgets(this).tb -bd 2 -relief raised]
  set tb $widgets(tb,path)  
  set data(tb,btgs) ""  
  set data(view,active) ""
  set data(zoom,active) ""
  ButtonsCreateGeneral $Instance 
  set data(tb,zoomPack) {home zoom-frame zoom-in scale zoom-out scroll}

  # pack de los objetos de la toolbar 
  set but_col 0
  foreach but $data(tb,btgs) {
    if { $but == "scale" } {
      grid $widgets(tb,btg,scale) -row 0 -column $but_col -sticky w
    } elseif { [string equal -length 3 $but "sep"] } { 
      grid $widgets(tb,btg,$but) -row 0 -column $but_col -sticky ns -padx 3 
    } else {
      grid $widgets(tb,btg,$but) -row 0 -column $but_col -sticky w -ipadx 2\
           -ipady 2 -pady 1
    }
    incr but_col
  }
  grid columnconfigure $tb $but_col -weight 1
  
  # 2.- frame de el/los gráfico/s -> $widgets(this).gr    
  set framegr [frame $widgets(this).gr -bd 2 -relief raised]
  # procedimiento recursivo que crea los sub-frames y los pw
  set pw_panel [getPanel $framegr $structure]
  set pw [lindex $pw_panel 0]
  set panel [lindex $pw_panel 1]
  # incluir en cada frames un graph. Pack de los .gr .pn .pw

  set i 0
  foreach fr $panel {
    set widgets(gr,$i) [blt::graph $fr.gr]
    set instance($fr.gr) $Instance
    pack $widgets(gr,$i) -fill both -expand yes
    incr i
  }
  set data(gr,numGraph) $i
  #guardamos los panelwindows
  set widgets(pws) $pw
  foreach pan $pw {
    pack $pan -fill both -expand yes
  }

  # 3.- frame de la status bar -> $widgets(this).sb    
  set widgets(sb,path) [frame $widgets(this).sb]
  set sb $widgets(sb,path)
  set sepPath [Separator $sb.ss -orient horizontal]
  set sfPath [frame $sb.sf]
  set widgets(sb,lCursor) [label $sfPath.cursor -anchor w -text "t= ,z= " -relief sunken]
  set widgets(sb,lInfo)   [label $sfPath.info   -anchor w -text "" -relief sunken]

  global zoomInfo
  set data(gr,active) 0
  activeGraph 0 $Instance

  # configurar acciones de cada graph
  for  {set i 0} {$i<$data(gr,numGraph)} {incr i 1} {
    set item $widgets(gr,$i)
    trace variable zoomInfo($item,scale) w "::bayesGraph::OnChangeScale $Instance;\#"
    set zoomInfo($item,scale) 0.5
    #bind $item <ButtonPress-1> [list bayesGraph::activeGraph $i $Instance]
    bind $item <ButtonPress-3> [list bayesGraph::activeGraph $i $Instance]
    bind $item <Any-Motion> [list ::bayesGraph::UpdateCursorLabel $i $Instance %x %y]
    bind $item <Control-c> [list ::bayesGraph::Copy $Instance]
  }
  # set the cursors for each mode
  if { [string equal $tcl_platform(platform) "windows"] } {
    set tail ".cur"
  } else {
    set tail "_cur.xbm black"
  }

  foreach cur_info {{zoom-frame zoomframe} {zoom-in zoomin} {zoom-out zoomout}} {
    set tvcur [file join ${toltk_images_path} [lindex $cur_info 1]]
    regsub -all " " $tvcur {\ } tvcur
    set widgets(cursor,[lindex $cur_info 0]) "@$tvcur$tail" 
  }
  set widgets(cursor,scroll) hand2
  set widgets(cursor,) [$widgets(gr,$data(gr,active)) cget -cursor]
  # bind AXIS
  ::bayesGraph::BindAxis $Instance
  # pack de los objetos de la status bar
  grid $widgets(sb,lInfo)   -row 0 -column 0 -sticky ew -padx 5
  grid $widgets(sb,lCursor) -row 0 -column 1 -sticky w  -padx 5
  grid columnconfigure $sfPath 0 -weight 1
  grid $sepPath -sticky ew -pady 2
  grid $sfPath -sticky ew
  grid columnconfigure $widgets(sb,path) 0 -weight 1
 
  # pack frames...
  grid $tb -sticky ew
  grid $framegr  -sticky news
  grid $sb -sticky ew  
  grid columnconfigure $path 0 -weight 1
  grid rowconfigure    $path 1 -weight 1

   focus $widgets(tb,btg,scale)
#  focus $widgets(this)

  ::bayesGraph::ZoomFrame $Instance
  return $Instance
}


#/////////////////////////////////////////////////////////////////////////////
proc ::bayesGraph::FormatAxis {this axis bltg tick} {
#
#/////////////////////////////////////////////////////////////////////////////  
  return "$tick"
}


#/////////////////////////////////////////////////////////////////////////////
proc ::bayesGraph::BindLegend {Instance} {
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${Instance}::data data
  upvar \#0 ${Instance}::widgets widgets

  for  {set gr 0} {$gr<$data(gr,numGraph)} {incr gr 1} {
    set grpath $widgets(gr,$gr)
    $grpath legend bind all <Enter> \
       [list ::bayesGraph::changeCursor $Instance $grpath $widgets(cursor,scroll)]
    $grpath legend bind all <Leave> \
       [list ::bayesGraph::changeCursor $Instance $grpath last]
    set idx 1
    foreach git $data(gr,$gr,elements) {
      foreach it $git {
        $grpath legend bind $it <Double-Button-1> \
           [list ::bayesGraph::GrapOpt $Instance -tabshow Lines -nline $idx]
      }
      incr idx       
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::bayesGraph::BindAxis {Instance} {
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${Instance}::data data
  upvar \#0 ${Instance}::widgets widgets

  for  {set gr 0} {$gr<$data(gr,numGraph)} {incr gr 1} {
    set grpath $widgets(gr,$gr)
    $grpath axis bind all <Enter> \
       [list ::bayesGraph::changeCursor $Instance $grpath $widgets(cursor,scroll)]
    $grpath axis bind all <Leave> \
       [list ::bayesGraph::changeCursor $Instance $grpath last]

    foreach it {x x2 y y2} {
      $grpath axis bind $it <Double-Button-1> \
        [list ::bayesGraph::GrapOpt $Instance -tabshow Axis -iniaxis $it]
    }
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::bayesGraph::getInstance {graph} {
#
#/////////////////////////////////////////////////////////////////////////////  
  variable instance
  return $instance($graph)
}


#/////////////////////////////////////////////////////////////////////////////
proc ::bayesGraph::Configure {this} {
#  
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::options opt
  set old_active $data(gr,active)
  for {set gr 0} {$gr<$data(gr,numGraph)} {incr gr} {
    set data(gr,active) $gr

    GrapOptApplyMappedLines  $this ${this}::options gr,$gr
    GrapOptApplyAxisPos      $this ${this}::options gr,$gr
    GrapOptApplyAxisTicks    $this ${this}::options gr,$gr
    GrapOptApplyTicksStyle   $this ${this}::options gr,$gr
    GrapOptApplyTitles       $this ${this}::options gr,$gr
    GrapOptApplyLnesAll      $this ${this}::options gr,$gr
    GrapOptApplyMarkers      $this ${this}::options gr,$gr
    GrapOptApplyMarkersIn    $this ${this}::options gr,$gr
    GrapOptApplyGrid         $this ${this}::options gr,$gr,grid
    GrapOptApplyLegend       $this ${this}::options gr,$gr,legend
    GrapOptApplyStyle        $this ${this}::options gr,$gr
    #Esta opción solo tiene efecto cuando el gráfico está empacado
    GrapOptApplyAxisTicks    $this ${this}::options gr,$gr
    GrapOptApplyOrder        $this ${this}::options gr,$gr
    # Show Line
    if {![info exists opt(gr,$gr,linehides)] || \
        ![info exists tmpOpt(${this}::options,linehides)] || \
        ![string equal $opt(gr,$gr,linehides) $tmpOpt(${this}::options,linehides)]} {
      GrapOptApplyLineDefault $this ${this}::options gr,$gr hide linehides
    }      
    
    
    # Ajust axis chart of values
    if {$opt(gr,$gr,selffit)} {
      ::bayesGraph::ZoomExtend $this
    }
  }
  set data(gr,active) $old_active
  if $opt(grid) {
    ::bayesGraph::ToggleGrid $this
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::bayesGraph::ButtonsCreateGeneral {this} {
#
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data data
  set widgets(tbg,path) [frame $widgets(tb,path).tbg -bd 2]
  set tbg $widgets(tbg,path)  

  pack $tbg -side left
  #grid $tbg -sticky ew
  # anexamos botones generales
  set widgets(tb,btg,home) [Button $tbg.home -image [::Bitmap::get "home"] \
    -highlightthickness 0 -activebackground \#ececec -takefocus 0 \
    -relief link -bd 1 -padx 1 -pady 1 -helptext [mc "Zoom extend"] \
    -command [list ::bayesGraph::ZoomExtend $this]]
  lappend data(tb,btgs) home
  set widgets(tb,btg,zoom-frame) [Button $tbg.zframe \
    -image [::Bitmap::get "zoomframe"] -highlightthickness 0 \
    -activebackground \#ececec -takefocus 0 -relief link -bd 1 -padx 1 \
    -pady 1 -helptext [mc "Zoom frame"] \
    -command [list ::bayesGraph::ZoomFrame $this]]
  lappend data(tb,btgs) zoom-frame
  set widgets(tb,btg,zoom-in) [Button $tbg.zoomin -image [::Bitmap::get "zoomin"] \
    -highlightthickness 0 -activebackground \#ececec -takefocus 0 \
    -relief link -bd 1 -padx 1 -pady 1 -helptext [mc "Zoom In"] \
    -command [list ::bayesGraph::ZoomIn $this]]
  lappend data(tb,btgs) zoom-in
  set widgets(tb,btg,scale) [scale $tbg.sc -orient horizontal -width 8 -bd 1 \
    -sliderlength 15 -from 0.01 -to 1 -resolution 0.01 -showvalue 0]
  lappend data(tb,btgs) scale
  set widgets(tb,btg,zoom-out) [Button $tbg.zoomout -image [::Bitmap::get "zoomout"] \
    -highlightthickness 0 -activebackground \#ececec -takefocus 0 \
    -relief link -bd 1 -padx 1 -pady 1 -helptext [mc "Zoom Out"] \
    -command [list ::bayesGraph::ZoomOut $this]]
  lappend data(tb,btgs) zoom-out
  set widgets(tb,btg,scroll) [Button $tbg.scroll -image [::Bitmap::get "scroll"] \
    -highlightthickness 0 -activebackground \#ececec -takefocus 0 \
    -relief link -bd 1 -padx 1 -pady 1 -helptext [mc "Scroll view"] \
    -command [list ::bayesGraph::ScrollView $this]]
  lappend data(tb,btgs) scroll
  set widgets(tb,btg,sep1) [Separator $tbg.sep1 -orient vertical]
  lappend data(tb,btgs) sep1  
  set widgets(tb,btg,grid) [Button $tbg.grid -image [::Bitmap::get "grid"] \
    -highlightthickness 0 -activebackground \#ececec -takefocus 0 \
    -relief link -bd 1 -padx 1 -pady 1 -helptext [mc "Toggle grid mode"] \
    -command [list ::bayesGraph::ToggleGrid $this]]
  lappend data(tb,btgs) grid
  set widgets(tb,btg,crosshair) [Button $tbg.cross \
    -image [::Bitmap::get "crosshair"] -highlightthickness 0 \
    -activebackground \#ececec -takefocus 0 -relief link -bd 1 -padx 1 \
    -pady 1 -helptext [mc "Toggle crosshair mode"] \
    -command [list ::bayesGraph::ToggleCrosshair $this]]
  lappend data(tb,btgs) crosshair
  set widgets(tb,btg,gobegin) [Button $tbg.beg -image [::Bitmap::get "begin"] \
    -highlightthickness 0 -activebackground \#ececec -takefocus 0 \
    -relief link -bd 1 -padx 1 -pady 1 -helptext [mc "Skip to first data"] \
    -command [list ::bayesGraph::SeeBegin $this]]
  lappend data(tb,btgs) gobegin
  set widgets(tb,btg,playback) [Button $tbg.playb \
    -image [::Bitmap::get "backward"] -highlightthickness 0 \
    -activebackground \#ececec -takefocus 0 -relief link -bd 1 -padx 1 \
    -pady 1 -helptext [mc "Play backward"] \
    -command [list ::bayesGraph::PlayBackward $this]]
  lappend data(tb,btgs) playback
  set widgets(tb,btg,stop) [Button $tbg.stop -image [::Bitmap::get "stopplay"] \
    -highlightthickness 0 -activebackground \#ececec -takefocus 0 \
    -relief link -bd 1 -padx 1 -pady 1 -helptext [mc "Stop playing"] \
    -command [list ::bayesGraph::StopPlaying $this]]
  lappend data(tb,btgs) stop
  set widgets(tb,btg,playfor) [Button $tbg.playf \
    -image [::Bitmap::get "forward"] -highlightthickness 0 \
    -activebackground \#ececec -takefocus 0 -relief link -bd 1 -padx 1 \
    -pady 1 -helptext [mc "Play forward"] \
    -command [list ::bayesGraph::PlayForward $this]]  
  lappend data(tb,btgs) playfor
  set widgets(tb,btg,goend) [Button $tbg.end -image [::Bitmap::get "end"] \
    -highlightthickness 0 -activebackground \#ececec -takefocus 0 \
    -relief link -bd 1 -padx 1 -pady 1 -helptext [mc "Skip to last data"] \
    -command [list ::bayesGraph::SeeEnd $this]]
  lappend data(tb,btgs) goend
  set widgets(tb,btg,snap) [Button $tbg.snap -image [::Bitmap::get "camera"] \
    -highlightthickness 0 -activebackground \#ececec -takefocus 0 \
    -relief link -bd 1 -padx 1 -pady 1 \
    -helptext [mc "Take a photo of the graphic"] \
    -command [list ::bayesGraph::TakePhoto $this]]  
  lappend data(tb,btgs) snap
  set widgets(tb,btg,copy) [Button $tbg.copy -image [::Bitmap::get "bcopy"] \
    -highlightthickness 0 -activebackground \#ececec -takefocus 0 \
    -relief link -bd 1 -padx 1 -pady 1 -helptext [mc "Copy to clipboard"] \
    -command [list ::bayesGraph::Copy $this]]  
  lappend data(tb,btgs) copy
  set widgets(tb,btg,calc) [Button $tbg.calc -image [::Bitmap::get "calc"] \
    -highlightthickness 0 -activebackground \#ececec -takefocus 0 \
    -relief link -bd 1 -padx 1 -pady 1 -helptext [mc "To Eval window"] \
    -command [list ::bayesGraph::ToEvalWindow $this]]
  lappend data(tb,btgs) calc
  set widgets(tb,btg,table) [Button $tbg.table -image [::Bitmap::get "table"] \
    -highlightthickness 0 -activebackground \#ececec -takefocus 0 \
    -relief link -bd 1 -padx 1 -pady 1 -helptext [mc "View as table"] \
    -command [list ::bayesGraph::ToTable $this]]
  lappend data(tb,btgs) table
  set widgets(tb,btg,sep2) [Separator $tbg.sep2 -orient vertical]
  lappend data(tb,btgs) sep2  
  set widgets(tb,btg,options) [Button $tbg.options \
    -image [::Bitmap::get "tools"] -highlightthickness 0 \
    -activebackground \#ececec -takefocus 0 -relief link -bd 1 -padx 1 \
    -pady 1 -helptext [mc "Graphic Options"] \
    -command [list ::bayesGraph::GrapOpt $this]]
  lappend data(tb,btgs) options
  trace variable ${this}::options(GCFFile) w [list ::bayesGraph::TraceGCFFile $this]
  set widgets(tb,btg,editConf) [Entry $tbg.editConf -width 12 \
    -textvariable ${this}::options(GCFName) -state disabled]
  lappend data(tb,btgs) editConf
  set widgets(tb,btg,openConf) [Button $tbg.openConf \
    -image [::Bitmap::get "bopen"] -highlightthickness 0 \
    -activebackground \#ececec -takefocus 0 -relief link -bd 1 -padx 1 \
    -pady 1 \
    -helptext [mc "Load graph options from a Graphic Configuration File"] \
    -command [list ::bayesGraph::OpenGraphOpt $this]]
  lappend data(tb,btgs) openConf
  set widgets(tb,btg,saveConf) [Button $tbg.saveConf -image [::Bitmap::get "bsave"] \
    -highlightthickness 0 -activebackground \#ececec -takefocus 0 \
    -relief link -bd 1 -padx 1 -pady 1 \
    -helptext [mc "Save the current graph options in a Graphic Configuration File"] \
    -command [list ::bayesGraph::SaveGraphOpt $this]]
  lappend data(tb,btgs) saveConf

}


#/////////////////////////////////////////////////////////////////////////////
proc ::bayesGraph::TraceGCFFile {this name1 name2 op} {
#
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::options opt

  set opt(GCFName) [file tail $opt(GCFFile)]
}


#/////////////////////////////////////////////////////////////////////////////
proc ::bayesGraph::getPanel {panel list} {
#
#/////////////////////////////////////////////////////////////////////////////  
  set lpw ""
  set lpn ""
  set resp ""
  set tside [lindex $list 0]
  if {$tside == "g"} {
    lappend lpn $panel 
  } else  {
    if {$tside == "v"} {
      set pw [PanedWindow $panel.pw -side bottom -pad 0]
    } else  {
      set pw [PanedWindow $panel.pw -side left -pad 0]
    }
    lappend lpw $pw
    for  {set i 1} {$i <= 2} {incr i 1} {
      if {[lindex $list $i] == "g"} {
        lappend lpn [$pw add -weight 1]
      } else  {
        set x [getPanel [$pw add -weight 1] [lindex $list $i]]
        foreach but [lindex $x 0] {        
          lappend lpw $but
        }
        foreach but [lindex $x 1] {        
          lappend lpn $but
        }
      }
    }
  }
  lappend resp $lpw $lpn
  #Tolcon_Trace "Resp: $resp"
  return $resp
}


#/////////////////////////////////////////////////////////////////////////////
proc bayesGraph::activeGraph {numGraph this} {
#
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data data 
  # inicializo el entorno del gragh seleccionado
  set data(gr,active) $numGraph

  $widgets(tb,btg,scale) configure -variable zoomInfo($widgets(gr,$data(gr,active)),scale)
  #Tolcon_Trace "scale widget variable configurada a zoomInfo($widgets(gr,$data(gr,active)),scale) "
  # resaltar el gráfico
  focus $widgets(gr,$numGraph)
}


#/////////////////////////////////////////////////////////////////////////////
proc ::bayesGraph::Destroy {this} {
#
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::data data
  if {!$data(onlyphoto)} {
    WriteIni $this 0
  }
  namespace delete $this
}


#/////////////////////////////////////////////////////////////////////////////
proc ::bayesGraph::OnChangeScale { this } {
#
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data data  
  global zoomInfo
  set gr $data(gr,active)
  ::DynamicHelp::register $widgets(tb,btg,scale) \
      balloon $zoomInfo($widgets(gr,$gr),scale)
}


#/////////////////////////////////////////////////////////////////////////////
proc ::bayesGraph::UpdateCursorLabel { numGraph this x y } {
#
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data data
  if [$widgets(gr,$numGraph) inside $x $y] {
    set t [expr round([$widgets(gr,$numGraph) axis invtransform x $x])]
    set z [format "%.2f" [$widgets(gr,$numGraph) axis invtransform y $y]]
    if [info exists ${this}::data(proc,OnGetLabel)] {
      set label [$data(proc,OnGetLabel) $this $widgets(gr,$numGraph) $t $z]
    } else {
      set label "X = $t, Y = $z" 
    }
    $widgets(sb,lCursor) configure -text $label
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::bayesGraph::CreateColorTable { table ncolors } {
#
#/////////////////////////////////////////////////////////////////////////////  
  upvar $table tbl
  set ncol [ expr $ncolors - 1]
  if { $ncol <= 0} { set ncol 1}
  set xfir [ expr $ncol * 0.10]
  set xqua [ expr $ncol * 0.35]
  set xmed [ expr $ncol * 0.50]
  set xthi [ expr $ncol * 0.65]
  set xlas [ expr $ncol * 0.90]
  set xdiv1 [ expr $xqua - $xfir]
  set xdiv2 [ expr $xmed - $xqua]
  
  for { set jcolor 1} { $jcolor <= $ncolors} { incr jcolor} {
    set xcolor [ expr $jcolor - 1]
    
    # Los Azules
    if { $xcolor <= $xfir } { 
      set blue [ expr 0.5 + $xcolor / ( $xfir * 2.0)]
    } elseif { $xcolor <= $xqua } { 
      set blue 1.0 
    } elseif { $xcolor <= $xmed } { 
      set blue [ expr 1.0 - ( $xcolor - $xqua) / $xdiv2] 
    } else { set blue 0.0 }
    
    # Los Verdes
    if { $xcolor <= $xfir } { 
      set green 0.0
    } elseif { $xcolor <= $xqua } { 
      set green [ expr ( $xcolor - $xfir) / $xdiv1]
    } elseif { $xcolor <= $xthi } { 
      set green 1.0
    } elseif { $xcolor <= $xlas } { 
      set green [ expr 1.0 - ( $xcolor - $xthi) / $xdiv1]
    } else { set green 0.0 }
    
    # I los Rohos
    if { $xcolor <= $xmed } { 
      set red 0.0 
    } elseif { $xcolor <= $xthi} { 
      set red [ expr ( $xcolor - $xmed) / $xdiv2] 
    } elseif { $xcolor <= $xlas} { 
      set red 1.0 
    } else { 
      set red [ expr 1.0 - ( $xcolor - $xlas) / ( $xfir * 2.0)] 
    }
    
    if { $red < 0.0 } { set red 0.0 }
    if { $green < 0.0 } { set green 0.0 }
    if { $blue < 0.0 } { set blue 0.0 }
    if { $red > 1.0 } { set red 1.0 }
    if { $green > 1.0 } { set green 1.0 }
    if { $blue > 1.0 } { set blue 1.0 }
    
    set red [ expr int( $red * 255.0)]
    set green [ expr int( $green * 255.0)]
    set blue [ expr int( $blue * 255.0)]
    lappend tbl [format \#%02x%02x%02x $red $green $blue]
  }
}

#/////////////////////////////////////////////////////////////////////////////
# TOOLBAR GRAPH OPTIONS IMPLEMENTATION
#/////////////////////////////////////////////////////////////////////////////

#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::ZoomExtend { this } {
#
# PURPOSE: Expands x and y axis to its initial maximum values for the active
#          graph.
#
# PARAMETERS:
#   this -> bayesgraph instance
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data data
  set gr $data(gr,active)
  
  variable tmpOpt
  # valor maximo y minimo
  foreach axis {x y x2 y2} {
    set values_${axis} [::bayesGraph::GetLimitsValueAxis $this $axis]
  }
  # comprobamos que existan valores y aplicamos
  if {([info exists values_y]) && ([llength $values_y])} {

    set tmpOpt(var,yAxisMin) [lindex $values_y 0]
    set tmpOpt(var,yAxisMax) [lindex $values_y 1]
    ::bayesGraph::GrapOptApplyAxisLimits $this y \
      [expr $tmpOpt(var,yAxisMin) - ($tmpOpt(var,yAxisMin) / 100)] \
      [expr $tmpOpt(var,yAxisMax) + ($tmpOpt(var,yAxisMax) / 100)]
  }
  if {([info exists values_y2]) && ([llength $values_y2])} {
    set tmpOpt(var,y2AxisMin) [lindex $values_y2 0]
    set tmpOpt(var,y2AxisMax) [lindex $values_y2 1]
    ::bayesGraph::GrapOptApplyAxisLimits $this y2 \
      [expr $tmpOpt(var,y2AxisMin) - ($tmpOpt(var,y2AxisMin) / 100)] \
      [expr $tmpOpt(var,y2AxisMax) + ($tmpOpt(var,y2AxisMax) / 100)]
  }
  if {([info exists values_x]) && ([llength $values_x])} {
    set tmpOpt(var,xAxisMin) [lindex $values_x 0]
    set tmpOpt(var,xAxisMax) [lindex $values_x 1]
    ::bayesGraph::GrapOptApplyAxisLimits $this x \
      $tmpOpt(var,xAxisMin) $tmpOpt(var,xAxisMax)
  }
  if {([info exists values_x2]) && ([llength $values_x2])} {
    set tmpOpt(var,x2AxisMin) [lindex $values_x2 0]
    set tmpOpt(var,x2AxisMax) [lindex $values_x2 1]
    ::bayesGraph::GrapOptApplyAxisLimits $this x2 \
      $tmpOpt(var,x2AxisMin) $tmpOpt(var,x2AxisMax)
  }
  #Aplicamos valores a loe ejes
  GrapOptApplyAxisTicks    $this ${this}::options gr,$gr
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::ZoomFrame { this } {
#
# PURPOSE: Sets zoom-frame mode for the blt graph
#      
# PARAMETERS:
#   this -> bayesgraph instance
#
#/////////////////////////////////////////////////////////////////////////////
  SetViewMode $this zoom-frame
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::ZoomIn { this } {
#
# PURPOSE: Sets zoom-in mode for the blt graph
#      
# PARAMETERS:
#   this -> bayesgraph instance
#
#/////////////////////////////////////////////////////////////////////////////

  SetViewMode $this zoom-in
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::ZoomOut { this } {
#
# PURPOSE: Sets zoom-out mode for the blt graph
#      
# PARAMETERS:
#   this -> bayesgraph instance
#
#/////////////////////////////////////////////////////////////////////////////

  SetViewMode $this zoom-out
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::ScrollView { this } {
#
# PURPOSE: Sets scroll mode for the blt graph
#      
# PARAMETERS:
#   this -> bayesgraph instance
#
#/////////////////////////////////////////////////////////////////////////////

  SetViewMode $this scroll
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::SetViewMode { this mode } {
#
# PURPOSE: Sets zoom (or scroll) mode for the blt graph. It also enables and
#          disables toolbar buttons and sets cursors types acording to the
#          mode choosen.
#      
# PARAMETERS:
#   this -> bayesgraph instance
#   mode -> mode for the blt graph
#
#/////////////////////////////////////////////////////////////////////////////

  upvar \#0 ${this}::data data
  upvar \#0 ${this}::widgets widgets
  set active $data(zoom,active)
  if { $active != "" } {
    $widgets(tb,btg,$active) configure -relief link
  }
  if { [string compare $active $mode] } {
    set active [set data(zoom,active) $mode]
    $widgets(tb,btg,$mode) configure -relief sunken    
  } else {
    set active [set data(zoom,active) ""]     
  }
  for  {set gr 0} {$gr<$data(gr,numGraph)} {incr gr 1} {
    set item $widgets(gr,$gr)
    $item configure -cursor $widgets(cursor,$active)
    Blt_ViewMode $item $active
  }

}


#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::ToggleButton { this button } {
#
# PURPOSE: Changes state of a button between on (relief sunken) and off
#          (relief link)
#      
# PARAMETERS:
#   this   -> bayesgraph instance
#   button -> button to toggle
#
#/////////////////////////////////////////////////////////////////////////////

  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data data
  if { [string equal [$widgets(tb,btg,$button) cget -relief] "link"] } {
    set relief "sunken"
    set state 1
  } else {
    set relief "link"
    set state 0
  }
  $widgets(tb,btg,$button) configure -relief $relief
  return $state
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::ToggleGrid { this } {
#
# PURPOSE: Changes grid option between enabled and disabled for all graphs in
#          the instance.
#      
# PARAMETERS:
#   this -> bayesgraph instance
#
#/////////////////////////////////////////////////////////////////////////////

  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::options opt

  set opt(grid) [ToggleButton $this "grid"]
  for  {set i 0} {$i<$data(gr,numGraph)} {incr i 1} {
    set item $widgets(gr,$i)
    $item grid toggle
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::ToggleCrosshair { this } {
#
# PURPOSE: Changes crosshair option between enabled and disabled for all 
#          graphs in the instance.
#      
# PARAMETERS:
#   this -> bayesgraph instance
#
#/////////////////////////////////////////////////////////////////////////////

  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data data
  if { [ToggleButton $this "crosshair"] } {
    for  {set i 0} {$i<$data(gr,numGraph)} {incr i 1} {
      set item $widgets(gr,$i)
      blt::Crosshairs $item
    }    
  } else {
    for  {set i 0} {$i<$data(gr,numGraph)} {incr i 1} {
      set item $widgets(gr,$i)
      blt::ClearCrosshairs $item
    }
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::DisableView { this } {
#
# PURPOSE: 
#      
# PARAMETERS:
#   this -> bayesgraph instance
#
#/////////////////////////////////////////////////////////////////////////////

  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data data
  set active $data(view,active)
  if { [string length $active] } {
    # this disable (toggle) the current view mode
    SetViewMode $this $active
  }
  foreach but $data(tb,zoomPack) {
    $widgets(tb,btg,$but) configure -state disable
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::EnableView { this mode } {
#
# PURPOSE:
#      
# PARAMETERS:
#   this -> bayesgraph instance
#   mode -> 
#
#/////////////////////////////////////////////////////////////////////////////

  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data data
  if { [string length $mode] } {
    SetViewMode $this $mode
  }
  foreach but $data(tb,zoomPack) {
    $widgets(tb,btg,$but) configure -state normal
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::StatePlay { this state} {
#
# PURPOSE: 
#      
# PARAMETERS:
#   this -> bayesgraph instance
#
#/////////////////////////////////////////////////////////////////////////////

  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data data
  foreach but [list gobegin playback playfor goend] {
    $widgets(tb,btg,$but) configure -state $state
  }
}
 

#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::SeeBegin { this } {
#
# PURPOSE: 
#      
# PARAMETERS:
#   this -> bayesgraph instance
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data data
  global zoomInfo
  set gr $data(gr,active)
  set active $data(view,active)
  DisableView $this
  set limits  [$widgets(gr,$gr) axis limits x]
  set limits2 [$widgets(gr,$gr) axis limits x2]
  
  set delta  [expr [lindex $limits  1] - [lindex $limits  0]]
  set delta2 [expr [lindex $limits2 1] - [lindex $limits2 0]]
  if {[string length $limits] } {
    ::bayesGraph::GrapOptApplyAxisLimits $this x  0 $delta
  }
  if {[string length $limits2]} {
    ::bayesGraph::GrapOptApplyAxisLimits $this x2 0 $delta2
  }
  EnableView $this $active
  set zoomInfo($widgets(gr,$gr),stack) {}
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::PlayBackward { this } {
#
# PURPOSE:
#
# PARAMETERS:
#   this -> bayesgraph instance
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data data
  global zoomInfo
  set gr $data(gr,active)
  set active $data(view,active)
  DisableView $this
  StatePlay $this disable
  set data(stopflag) 0
  set limits  [$widgets(gr,$gr) axis limits x]
  set limits2 [$widgets(gr,$gr) axis limits x2]
  set t0  [expr round([lindex $limits  0])]
  set t1  [expr round([lindex $limits  1])]
  set t20 [expr round([lindex $limits2 0])]
  set t21 [expr round([lindex $limits2 1])]
  
  set values  [::bayesGraph::GetLimitsValueAxis $this x]
  set values2 [::bayesGraph::GetLimitsValueAxis $this x2]
  set minX  [lindex $values  0]
  set minX2 [lindex $values2 0]
  set ok  1
  set ok2 1
  while { !$data(stopflag) && ($ok || $ok2)} {
    # comprobamos que hay valores para el eje X 
    if {[string length $limits] && [string length $values]} {
      ::bayesGraph::GrapOptApplyAxisLimits $this x  $t0 $t1
      incr t0 -1
      incr t1 -1
      set ok [expr $t0 >= $minX]
    } else  {
      set ok 0 
    }
    # comprobamos que hay valores para el eje X2
    if {[string length $limits2] && [string length $values2]} {
      ::bayesGraph::GrapOptApplyAxisLimits $this x2 $t20 $t21
      incr t20 -1
      incr t21 -1
      set ok2 [expr $t20 >= $minX2]
    } else  {
      set ok2 0
    }
    update
  }
  EnableView $this $active
  StatePlay $this normal
  set zoomInfo($widgets(gr,$gr),stack) {}
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::StopPlaying { this } {
#
# PURPOSE: 
#      
# PARAMETERS:
#   this -> bayesgraph instance
#
#/////////////////////////////////////////////////////////////////////////////

  upvar \#0 ${this}::data data
  set data(stopflag) 1
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::PlayForward { this } {
#
# PURPOSE:
#
# PARAMETERS:
#   this -> bayesgraph instance
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data data
  global zoomInfo
  set gr $data(gr,active)
  set active $data(view,active)
  DisableView $this
  StatePlay $this disable
  set data(stopflag) 0
  set limits  [$widgets(gr,$gr) axis limits x]
  set limits2 [$widgets(gr,$gr) axis limits x2]
  set t0  [expr round([lindex $limits  0])]
  set t1  [expr round([lindex $limits  1])]
  set t20 [expr round([lindex $limits2 0])]
  set t21 [expr round([lindex $limits2 1])]
  
  set values  [::bayesGraph::GetLimitsValueAxis $this x]
  set values2 [::bayesGraph::GetLimitsValueAxis $this x2]
  set maxX  [lindex $values 1]
  set maxX2 [lindex $values2 1]
  set ok  1
  set ok2 1
  
  while { !$data(stopflag) && ($ok || $ok2)} {
    # comprobamos valores eje X
    if {[string length $limits] && [string length $values]} {
      ::bayesGraph::GrapOptApplyAxisLimits $this x $t0 $t1
      incr t0
      incr t1
      set ok [expr $t1 <= $maxX]
    } else  {
      set ok 0
    }
    # comprobamos valores eje X
    if {[string length $limits2] && [string length $values2]} {
      ::bayesGraph::GrapOptApplyAxisLimits $this x2 $t20 $t21
      incr t20
      incr t21
      set ok2 [expr $t21 <= $maxX2]
    } else  {
      set ok2 0 
    }
    update
  }
  
  EnableView $this $active
  StatePlay $this normal
  set zoomInfo($widgets(gr,$gr),stack) {}
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::SeeEnd { this } {
#
# PURPOSE:
#
# PARAMETERS:
#   this -> bayesgraph instance
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data data
  global zoomInfo
  set gr $data(gr,active)
  set active $data(view,active)
  DisableView $this
  set limits  [$widgets(gr,$gr) axis limits x]
  set limits2 [$widgets(gr,$gr) axis limits x2]

  set delta  [expr [lindex $limits  1] - [lindex $limits  0]]
  set delta2 [expr [lindex $limits2 1] - [lindex $limits2 0]]

  set values  [::bayesGraph::GetLimitsValueAxis $this x]
  set values2 [::bayesGraph::GetLimitsValueAxis $this x2]

  if {[string length $limits]  && [string length $values]} {
    ::bayesGraph::GrapOptApplyAxisLimits $this x \
      [expr [lindex $values  1] - $delta]  [lindex $values  1]
  }
  if {[string length $limits2] && [string length $values2]} {
    ::bayesGraph::GrapOptApplyAxisLimits $this x2 \
      [expr [lindex $values2 1] - $delta2] [lindex $values2 1]
  }
      
  EnableView $this $active
  set zoomInfo($widgets(gr,$gr),stack) {}
}


#/////////////////////////////////////////////////////////////////////////////
proc ::bayesGraph::NewLimits { this fr_first fr_last } {
#
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data data
  set gr $data(gr,active)
  $widgets(sb,path) set $fr_first $fr_last
  ::bayesGraph::GrapOptApplyAxisLimits $this x $data(gr,$gr,xj) $data(gr,$gr,xk)
}


#/////////////////////////////////////////////////////////////////////////////
proc ::bayesGraph::OnChangeLimits { this } {
#
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data data
  set gr $data(gr,active)
  set data(gr,$gr,xj) [$widgets(gr,$gr) axis cget x -min]
  set data(gr,$gr,xk) [$widgets(gr,$gr) axis cget x -max]
  set $data(gr,$gr,delta_t)] [expr $data(gr,$gr,xk) - $data(gr,$gr,xj)]
  
  set values [::bayesGraph::GetLimitsValueAxis $this x]
  set fr_j [expr 1.0 * $data(gr,$gr,xj) / [lindex $values 1]]
  set fr_k [expr 1.0 * $data(gr,$gr,xk) / [lindex $values 1]]
  if { $data(gr,$gr,xj) == 0 && $data(gr,$gr,xk) == [lindex $values 1] } {
    grid remove $widgets(sb,path)
  } else {
    grid $widgets(sb,path)
    $widgets(sb,path) set $fr_j $fr_k
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::bayesGraph::TakePhoto {this {fileDest ""}} {
#
# PURPOSE: Takes a photo of the active graph and asks the user for a file to
#          save it.
#      
# PARAMETERS:
#   this -> bayesgraph instance
#   fileDest -> File destiny of photo, default "" 
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::options opt
  set gr $data(gr,active)
  set grpath $widgets(gr,$gr)
  
  # move focus a other widget
  set wdFoc [focus]
  focus $widgets(tb,btg,scale)
  
  if {$fileDest eq ""} {
    set typelist {{"Gif Files" {".gif"}}}
    set fileDest [tk_getSaveFile -title [mc "Choose a file to save the image"]\
                      -initialdir $opt(PhotoLastDir)\
                      -parent $widgets(this) \
                      -defaultextension .gif -filetypes $typelist]
    if {$fileDest eq ""} {
      tk_messageBox -type ok -message [mc "File hasn't been saved"]\
          -icon warning -parent $widgets(this) -title [mc Warning]
      return
    }
  }
  set img [image create photo]
  $grpath snap $img
  $img write -format gif $fileDest
  set opt(PhotoLastDir) [file dirname $fileDest]
  # restore focus
  if {[string length $wdFoc]} {
    focus $wdFoc
  }  
  destroy $img 
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::Copy {this} {
#
# PURPOSE: Makes a copy of active graph image to clipboard. Only works for
#          windows platform.
#      
# PARAMETERS:
#   this -> bayesgraph instance
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data data

  set gr $data(gr,active)
  set grpath $widgets(gr,$gr)

  focus $widgets(this)
  update
  if { [string equal $::tcl_platform(platform) "windows"] } {
    $grpath snap -format emf CLIPBOARD
  } else {
    # codigo para otra plataforma
  }
  focus $grpath
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::ToEvalWindow {this} {
#
# PURPOSE: Inserts in eval window the names of the items drawn in the graphs.
#          It is really implemented in bayesGraph childs.
#      
# PARAMETERS:
#   this -> bayesgraph instance
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
 
  set txt ""
  if [info exists ${this}::data(proc,OnEvalWindow)] {
    set txt [$data(proc,OnEvalWindow) $this]
  } 
  ::TolConsole::ToEvalWindow $txt
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::ToTable {this} {
#
# PURPOSE: Makes a table with data of the items grawn in the graphs. It is
#          really implemented in bayesGraph childs.
#      
# PARAMETERS:
#   this -> bayesgraph instance
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
 
  if [info exists ${this}::data(proc,OnToTable)] {
    $data(proc,OnToTable) $this
  } 
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::ChangeType {this} {
#
# PURPOSE: 
#
# PARAMETERS:
#   this -> bayesgraph instance
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data

  set gr $data(gr,active)
  foreach git $data(gr,$gr,elements) {
    ElemOptChangeType $this [lindex $git 0] $git
  }
}



#/////////////////////////////////////////////////////////////////////////////
proc ::bayesGraph::GetDataGraph { this gr key } {
# 
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ::${this}::data data
  return $data(gr,$gr,$key)
}


#/////////////////////////////////////////////////////////////////////////////
proc ::bayesGraph::SetDataGraph { this gr key value } {
# 
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ::${this}::data data
  set gr $data(gr,active)
  set data(gr,$gr,$key) $value
}


#/////////////////////////////////////////////////////////////////////////////
proc ::bayesGraph::changeCursor { this gr cursor} {
# 
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::data data
  if { [string equal $cursor last] } {
    $gr configure -cursor $data(cursor,last)
  } else  {
    set data(cursor,last) [$gr cget -cursor]
    $gr configure -cursor $cursor
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::bayesGraph::getActiveGraph {this} {
# 
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data data

  set gr $data(gr,active)
  set g $widgets(gr,$gr)
  return $g
}


