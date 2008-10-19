#/////////////////////////////////////////////////////////////////////////////
# FILE    : objects.tcl
# PURPOSE : This file contains a namespace called Objects with its functions.
#           It's for creating and managing compound widgets asociated with a
#           variable. We have:
#             - Date Chooser
#             - File Chooser
#             - Directory Chooser
#             - Font Selector
#             - Color Selector
#             - Position Selector
#             - Line Style Selector
#             - Order list
#
#/////////////////////////////////////////////////////////////////////////////

#/////////////////////////////////////////////////////////////////////////////
# TESTING
#/////////////////////////////////////////////////////////////////////////////
# Put if {1} for testing this file.
if {0} {
  package require Tk
  package require BWidget
  # force BWidget autoload
  Bitmap::use
  set toltk_script_path [file dirname [info script]]
  set toltk_images_path [file join ${toltk_script_path} images]
  if { [lsearch $Bitmap::path $toltk_images_path] == -1 } {
      lappend Bitmap::path [file join $toltk_script_path images]
  }
  package require msgcat
  namespace import msgcat::*
}

namespace eval ::Objects {
}


proc ::Objects::ChooseDateCreate {var path} {
  set Instance ::Objects::ChooseDate$path
  namespace eval $Instance {
    variable data
  }
  upvar \#0 ${Instance}::data data
  set data(path) [frame $path]

  set data(ftop) [frame $path.f]
  Entry  $path.f.e -textvariable $var -state disabled -width 16
  Button $path.f.b -image [::Bitmap::get Date]\
          -command [list eval ::Objects::ChooseDateChange $Instance $var $path]
  grid $path.f.e $path.f.b -padx 0 -sticky nw
  pack $path.f

  ::date::Init
 # set ::date::a(otherday) 0
  set w [toplevel $data(path).t]
  set data(fbottom) $w
  wm resizable $w 0 0
  #wm transient $w [winfo toplevel $data(path)]
  wm overrideredirect $w 1
  wm withdraw $w
  
  upvar \#0 $var variable
  if [info exist variable] {
    set year  [lindex $variable 0]
    set month [lindex $variable 1]
  } else {
    set year  [lindex [::tol::date now] 0]
    set month [lindex [::tol::date now] 1]
  }

  set this [::date::Chooser $w.c $month $year [list OnDoubleDay "::Objects::ChooseDateGetDay $Instance $var $path"]]
  #$w.c configure -bd 0
  frame $w.f -bd 1 -bg white -relief solid
  
  Button $w.f.bby -text "<<" -height 1 -bg white\
    -command [list ::date::Adjust $this  0 -1] -relief link -width 2\
    -helptext [mc "Moves to previous year"]
  Button $w.f.bbm -text "< " -height 1 -bg white\
    -command [list ::date::Adjust $this -1  0] -relief link -width 2\
    -helptext [mc "Moves to previous month"]
  Button $w.f.bnm -text " >" -height 1 -bg white\
    -command [list ::date::Adjust $this  1  0] -relief link -width 2\
    -helptext [mc "Moves to next month"]
  Button $w.f.bny -text ">>" -height 1 -bg white\
   -command [list ::date::Adjust $this  0  1] -relief link -width 2\
   -helptext [mc "Moves to next year"]
  frame $w.f.f

  grid $w.f.bby $w.f.bbm $w.f.f $w.f.bnm $w.f.bny -sticky nw  -padx 2
  grid columnconfigure $w.f 2 -weight 1

  grid $w.f -sticky nwe  -padx 2
  grid $w.c -sticky nwe  -padx 2

  return $path
}

proc ::Objects::ChooseDateChange {this var path} {
  upvar \#0 ${this}::data data
  upvar \#0 $var variable

  if {[winfo viewable $data(fbottom)] == 1} {
    wm withdraw  $data(fbottom)
    grab release $data(fbottom)
  } else {
    set vwidth  [winfo reqwidth  $data(fbottom)]
    set vheight [winfo reqheight $data(fbottom)]
    wm deiconify $data(fbottom)
    wm geometry $data(fbottom) \
      "${vwidth}x${vheight}+[winfo rootx $data(ftop)]+[expr \
      [winfo rooty $data(ftop)]+[winfo height $data(ftop)]]"
    focus -force $data(fbottom)
    grab -global $data(fbottom)
    bind $data(fbottom) <ButtonPress> [list ::Objects::ChooseDateClose $this %X %Y]
  }
}

proc ::Objects::ChooseDateClose {this x y} {
  upvar \#0 ${this}::data data
  if {[expr $x==-1 && $y==-1]}  {
    wm withdraw  $data(fbottom)
    grab release $data(fbottom)
  } else  {
    set obj [winfo containing $x $y]
    if {[string length $obj]} {
      if {[winfo toplevel $obj] != $data(fbottom)} {
        wm withdraw  $data(fbottom)
        grab release $data(fbottom)
      }
    } 
  }
}

proc ::Objects::ChooseDateGetDay {this var path day month year tags} {
  upvar \#0 ${this}::data data
  upvar \#0 $var variable
  set variable [list $year $month $day]
  wm withdraw  $data(fbottom)
  grab release $data(fbottom)
}


proc ::Objects::ChooseDirCreate {var path args} {
  Button $path -image [::Bitmap::get open]\
          -command [list eval ::Objects::ChooseDirChange $var $path $args]

  set cmdTrace [list ::Objects::ChooseDirUpdate $path $var]
  trace variable $var w $cmdTrace
  bind $path <Destroy> [list ::Objects::ClearTrace $var $cmdTrace]
  #Si la variable ya tiene valor actualizo el widget
  if [info exists $var] {
	ChooseDirUpdate $path $var
  }
  return $path
}

proc ::Objects::ChooseDirChange {var widget args} {
  upvar \#0 $var variable
  set parent [winfo parent $widget]
  if [info exist variable] {
    lappend args -initialdir  $variable
  }
  set temp [eval tk_chooseDirectory -parent $parent -mustexist true $args]
  if {$temp != ""} {
    set variable $temp
    $widget configure -helptext "[mc [list Choose directory]]: ($variable)"
  }
}

proc ::Objects::ChooseDirUpdate {path var args} {
  upvar \#0 $var variable
  $path configure -helptext "[mc [list Choose directory]]: ($variable)"
}

proc ::Objects::ChooseFileCreate {var path args} {
  Button $path -image [::Bitmap::get bopen] -relief link\
          -command [list eval ::Objects::ChooseFileChange $var $path $args] \
          -helptext [mc "Choose file"]

  set cmdTrace [list ::Objects::ChooseFileUpdate $path $var]
  trace variable $var w $cmdTrace
  bind $path <Destroy> [list ::Objects::ClearTrace $var $cmdTrace]
  #Si la variable ya tiene valor actualizo el widget
  if [info exists $var] {
	ChooseFileUpdate $path $var
  }
  return $path
}

proc ::Objects::ChooseFileChange {var widget args} {
  upvar \#0 $var variable

  set parent [winfo parent $widget]

# jima begin
# This code is commented out as calls from outside shoudl use
# -initialdir directly
#  if [info exist variable] {
#    lappend args -initialdir  [file dirname $variable]
#    lappend args -initialfile [file tail $variable]
#  }
# jima end

  set temp [eval tk_getOpenFile -parent $parent $args]
  if {$temp != ""} {
    set variable $temp
    $widget configure -helptext "[mc {Choose file}]: ($variable)"
  }
}

proc ::Objects::ChooseFileUpdate {path var args} {
  upvar \#0 $var variable
  $path configure -helptext "[mc [list Choose file]]: ($variable)"
}


proc ::Objects::FontCreate {var path args} {
  upvar \#0 $var variable
  Button $path -image [::Bitmap::get font]\
        -command [list eval ::Objects::FontChange $var $path $args]
  set cmdTrace [list ::Objects::FontUpdate $path $var]
  trace variable $var w $cmdTrace
  bind $path <Destroy> [list ::Objects::ClearTrace $var $cmdTrace]

  #Si la variable ya tiene valor actualizo el widget
  if [info exists $var] {
    FontUpdate $path $var
  }
}

proc ::Objects::FontUpdate {path var args} {
  upvar \#0 $var variable
  $path configure -helptext "[mc [list Change font]]: ($variable)"
}

proc ::Objects::FontChange {var widget args} {
  upvar \#0 $var variable
    
  set parent [winfo parent $widget]
  if [info exists variable] {
    lappend args -font $variable
  }
  set temp [eval SelectFont .sf -parent $parent $args]
  if {$temp != ""} {
    set variable $temp
  }
}

proc ::Objects::ColorCreate {var path {helptext ""} {null 0} args} {
  frame $path -bd 1 -relief sunken
  frame $path.fcolor -height 4
  bind $path.fcolor <Button-1> [list eval ::Objects::ColorChange $var $path $args]
    
  #button $path.bcolor -image [::Bitmap::get pen] \
  \# -command  [list eval ::Objects::ColorChange $var $path $args]

  Button $path.bcolor -relief link\
          -command  [list eval ::Objects::ColorChange $var $path $args] \
          -height 12 -image [::Bitmap::get pen]
    
  if [string equal $helptext ""] {
    set helptext [mc "Change color"]:
  }
  set cmdTrace [list ::Objects::ColorUpdate $path $var $helptext]
  trace variable $var w $cmdTrace
  bind $path.fcolor <Destroy> [list ::Objects::ClearTrace $var $cmdTrace]
  #Si la variable ya tiene valor actualizo el widget
  if [info exists $var] {
	ColorUpdate $path $var $helptext
  }
  pack $path.bcolor $path.fcolor -expand true -fill x
  if $null {
  	#Creación de un menú contextual
  	#upvar \#0 $var variable
  	set menu [menu $path.m -tearoff 0]
  	bind $path.fcolor <Button-3>  "::Objects::PostEmptyColor $path"
  	bind $path.bcolor <Button-3>  "::Objects::PostEmptyColor $path"
  }
  if [string equal $null 1] {
    $menu add command -label [mc "Without color"] -command "set $var {}"
  }
  if [string equal $null 2] {
    $menu add command -label [mc "Default color"] -command "set $var defcolor"
  }
  if [string equal $null 3] {
    $menu add command -label [mc "Without color"] -command "set $var {}"
    $menu add command -label [mc "Default color"] -command "set $var defcolor"
  }
  return $path
}

proc ::Objects::ClearTrace { v cmd } {
    trace vdelete $v w $cmd
}

proc ::Objects::PostEmptyColor { w } {
  set x [winfo rootx $w]
  set y [expr [winfo rooty $w] + [winfo height $w]]
  tk_popup $w.m $x $y
}

proc ::Objects::ColorUpdate {path var helptext args} {
  upvar \#0 $var variable
  if {[string length $variable] && ![string equal $variable "defcolor"]} {
    $path.fcolor configure -bg $variable
  } else  {
    $path.fcolor configure -bg [$path cget -bg]
  }
    
  $path.bcolor configure -helptext "$helptext ($variable)"
}

proc ::Objects::ColorChange {var widget args} {
  upvar \#0 $var variable
    
  set parent [winfo parent $widget]
  if {[info exists variable] && ![string equal $variable ""] && ![string equal $variable "defcolor"]} {
    set temp [eval tk_chooseColor -initialcolor $variable -parent $parent $args]
  } else  {
    set temp [eval tk_chooseColor -parent $parent $args]
  }
  if {$temp != ""} {
    set variable $temp
  }
}

proc ::Objects::ComboCreate {var path vwidth vheight} {
  set Instance ::Objects::ComboCreate$path
  namespace eval $Instance {
    variable data
  }
  upvar \#0 ${Instance}::data data
  set data(path) $path
  frame $data(path)
  bind $data(path) <Destroy> [list ::Objects::Destroy $Instance]
  set data(ftop) [frame ${data(path)}.top]
  set data(fbottom) [toplevel ${data(path)}.bottom \
                     -height ${vheight} -width ${vwidth} \
                     -bg white -borderwidth 1 -relief solid]
  wm resizable $data(fbottom) 0 0
  #wm transient $data(fbottom) [winfo toplevel $data(fbottom)]
  wm overrideredirect $data(fbottom) 1
  wm withdraw $data(fbottom)

  puts [wm geometry $data(fbottom)]

  set data(canvas)  [canvas $data(ftop).c -bg white -height 16 -width [expr $vwidth -16 ] -relief sunken]
  
  set data(fbutton) [ArrowButton $data(ftop).button \
                     -dir bottom -command [list ::Objects::ComboUpdate $Instance]]
  pack $data(canvas) $data(fbutton) -expand true -fill both -side left
  
  set cmdTrace [list ::Objects::ComboChange $Instance $var]
  trace variable $var w $cmdTrace
  bind $path <Destroy> [list ::Objects::ClearTrace $var $cmdTrace]

  pack $data(ftop)
  return $Instance
}

proc ::Objects::Destroy {this} {
  namespace delete $this
}

proc ::Objects::ComboUpdate {this} {
  upvar \#0 ${this}::data data
  if {[winfo viewable $data(fbottom)] == 1} {
    wm withdraw  $data(fbottom)
    grab release $data(fbottom)
  } else {
    set vwidth  [winfo reqwidth  $data(fbottom)]
    set vheight [winfo reqheight $data(fbottom)]
    wm deiconify $data(fbottom)
    wm geometry $data(fbottom) \
      "${vwidth}x${vheight}+[winfo rootx $data(ftop)]+[expr \
      [winfo rooty $data(ftop)]+[winfo height $data(ftop)]]"
    focus -force $data(fbottom)
    grab -global $data(fbottom)
    bind $data(fbottom) <ButtonPress> [list ::Objects::ComboClose $this %X %Y]
  }
}

proc ::Objects::ComboChange {this var args} {
  upvar \#0 ${this}::data data
  upvar \#0 $var variable
  set widget $data(canvas)
  $widget delete all
  set valueUser $variable
  if {[info exist data(fillSpecial)]} {
    $data(fillSpecial) $valueUser $widget
  } else  {
    set center [expr [$widget cget -height] / 2]
    $widget create text 5 $center -text $valueUser -anchor w
  }
}

proc ::Objects::ComboClose {this x y} {
  upvar \#0 ${this}::data data
  if {[winfo toplevel [winfo containing $x $y]] != $data(fbottom)} {
    wm withdraw  $data(fbottom)
    grab release $data(fbottom)
  } 
}

proc ::Objects::MatrixFCreate {var path} {
  set Instance ::Objects::MatrixFCreate$path
  namespace eval $Instance {
    variable data
  }
  upvar \#0 ${Instance}::data data
  set data(path) $path 
  set data(coord) [list nw n ne w center e sw s se]
  set vwidth  [$data(path) cget -width]
  set vheight [$data(path) cget -height]
  set vwidthf  [expr $vwidth / 3]
  set vheightf [expr $vheight / 3]
  set cont 0
  for {set i 1} {$i <= 3} {incr i 1} {
    set fila [frame $data(path).$i -width $vwidth -height $vheightf]
    for {set j 1} {$j <= 3} {incr j 1} {
      set tcoord [lindex $data(coord) $cont]
      set data(widget,$tcoord) [frame $data(path).$i.$tcoord \
        -width $vwidthf -height $vheightf -relief flat -borderwidth 2]
      pack $data(widget,$tcoord) -side left
      bind $data(widget,$tcoord) <ButtonPress-1> \
        "[list ::Objects::MatrixFClose $Instance $var $tcoord]"
      bind $data(widget,$tcoord) <Enter> [list $data(widget,$tcoord) configure -bg grey40]
      bind $data(widget,$tcoord) <Leave> [list $data(widget,$tcoord) configure -bg grey]
      incr cont 1
    } 
    grid $fila -sticky w
  }

  set cmdTrace [list ::Objects::MatrixFUpdate $Instance $var]
  trace variable $var w $cmdTrace
  bind $path <Destroy> [list ::Objects::ClearTrace $var $cmdTrace]

  $data(path) configure -relief ridge -borderwidth 2
  return $Instance
}


proc ::Objects::MatrixFUpdate {this var args} {
  upvar \#0 ${this}::data data
  upvar \#0 $var coord  
  foreach tcoord $data(coord) {
    $data(widget,$tcoord) configure -relief flat
  }
  $data(widget,$coord) configure -relief ridge
}



proc ::Objects::MatrixFChange {var value} {
  upvar \#0 $var variable
  set variable $value
}

proc ::Objects::MatrixFClose {this var value} {
  upvar \#0 ${this}::data data
  upvar \#0 $var variable
  set variable $value
  if {[info exist data(closeFrame)]} {
    eval $data(closeFrame)
  } 
}

proc ::Objects::ComboUnTranslateDbl {combo list var} {
  upvar \#0 $var variable
  if {[$combo getvalue] != -1} {
    set variable [lindex $list [expr 2*[$combo getvalue]]]
  }
}

proc ::Objects::ComboSetIniDbl {combo list var} {
  upvar \#0 $var variable
  set idx [lsearch -exact $list $variable]
  if {$idx != -1} {
    $combo setvalue @[expr $idx/2]
  }
}

proc ::Objects::ComboUnTranslate {combo list var} {
  #Tolcon_Trace "Llamada a ComboUnTranslate combo: $combo, list: $list, var: $var"
  upvar \#0 $var variable
  if {[$combo getvalue] != -1} {
    set variable [lindex $list [$combo getvalue]]
  }
  #Tolcon_Trace "Ahora var ($var) vale $variable"
}

proc ::Objects::ComboSetIni {combo list var} {
  # Actualizo el valor en el combo
  ComboUpdateVar $combo $list $var
  # Trazo la variable, para q la actualización se haga siempre
  set cmdTrace [list ::Objects::ComboUpdateVar $combo $list $var]
  trace variable $var w $cmdTrace
  bind $combo <Destroy> +[list ::Objects::ClearTrace $var $cmdTrace]
}

proc ::Objects::ComboUpdateVar {combo list var args} {
  upvar \#0 $var variable
  set idx [lsearch -exact $list $variable]
  if {$idx != -1} {
    $combo setvalue @$idx
  }
}

proc ::Objects::DashesCreate {var path} {
  set Instance ::Objects::DashesCreate$path
  namespace eval $Instance {
    variable data
  }
  upvar \#0 ${Instance}::data data
  set vwidth   [$path cget -width]
  set vheight  [$path cget -height]
  set data(sb) [scrollbar $path.sb -command [list $path.c yview] -orient vertical]
  set data(path) [canvas $path.c -width [expr $vwidth - 16] -height $vheight -bd 0 \
    -bg white -yscrollcommand [list $data(sb) set]]  
  bind $data(path) <Destroy> [list ::Objects::Destroy $Instance]
  set lmax [expr [$data(path) cget -width] - 5]
  set lista [list {} {1} {1 1} {2 1 1 1} {2 1} {2 2} {3 1} {4 1 1 1} {4 2}]
  set llista [llength $lista]
  set yini  2
  for {set i 0} {$i < $llista} {incr i 1} {
    set center [expr $yini + 8]
    set idrect [$data(path) create polygon \
      2 $yini $lmax $yini $lmax [expr $yini + 16] 2 [expr $yini + 16] \
      -activefill gray60 -fill {}]
    set idline [$data(path) create line \
      5 $center [expr $lmax - 5] $center -width 2 -fill black -dash [lindex $lista $i]]
    $data(path) bind $idline <Enter> [list $data(path) itemconfigure $idrect -fill gray60]
    $data(path) bind $idline <Leave> [list $data(path) itemconfigure $idrect -fill {}]
    $data(path) bind $idline <ButtonPress-1> "[list ::Objects::DashesClose $Instance $var [lindex $lista $i]]"
    $data(path) bind $idrect <ButtonPress-1> "[list ::Objects::DashesClose $Instance $var [lindex $lista $i]]"
    set yini [expr $yini + 16]
  }
  $data(path) configure -scrollregion "1 1 $vwidth [expr [expr $llista * 16] + 5]"
  
  set cmdTrace [list ::Objects::DashesUpdate $Instance $var]
  trace variable $var w $cmdTrace
  bind $path <Destroy> [list ::Objects::ClearTrace $var $cmdTrace]

  pack $data(path) -side left
  pack $data(sb) -expand true -fill both -side left
  return $Instance
}

proc ::Objects::DashesUpdate {this var args} {
  upvar \#0 ${this}::data data
  upvar \#0 $var coord  
}

proc ::Objects::DashesChange {var value} {
  upvar \#0 $var variable
  set variable $value
}

proc ::Objects::DashesClose {this var value} {
  upvar \#0 ${this}::data data
  upvar \#0 $var variable
  set variable $value
  if {[info exist data(closeFrame)]} {
    eval $data(closeFrame)
  } 
}

proc ::Objects::ComboPosFill {value widget} {
  set lista [list nw top-left    n top         ne top-right \
                  w  left        center center e  right \
                  sw bottom-left s bottom      se bottom-right]
  array set arrCoord $lista
  set valueUser $arrCoord($value)
  set center [expr [$widget cget -height] / 2]
  $widget create text 5 $center -text $valueUser -anchor w
}

proc ::Objects::ComboPosCreate {var path} {
  set InstanceCombo  [::Objects::ComboCreate $var $path 75 36]
  upvar \#0 ${InstanceCombo}::data dataCombo
  set dataCombo(fillSpecial) ::Objects::ComboPosFill
  set InstanceMatrix [::Objects::MatrixFCreate $var $dataCombo(fbottom)]
  upvar \#0 ${InstanceMatrix}::data dataMatrix
  set dataMatrix(closeFrame) [list ::Objects::ComboUpdate $InstanceCombo]
  set $var [set $var] ;# para inicializar en el valor de var
  return $dataCombo(path)
}

proc ::Objects::ComboDashesFill {value widget} {
  set center [expr [$widget cget -height] / 2]
  set lmax [expr [$widget cget -width] - 5]
  $widget create line 5 $center $lmax $center -width 2 -fill black -dash $value
}

proc ::Objects::ComboDashesCreate {var path} {
  set InstanceComboD  [::Objects::ComboCreate $var $path 100 50]
  upvar \#0 ${InstanceComboD}::data dataComboD
  set dataComboD(fillSpecial) ::Objects::ComboDashesFill
  set InstanceDashes [::Objects::DashesCreate $var $dataComboD(fbottom)]
  upvar \#0 ${InstanceDashes}::data dataDashes
  set dataDashes(closeFrame) [list ::Objects::ComboUpdate $InstanceComboD]
  set $var [set $var] ;# para inicializar en el valor de var
  return $dataComboD(path)
}

proc ::Objects::ComboAutoFillCreate {path args} {
  set Instance ::Objects::ComboAutoFill$path
  namespace eval $Instance {
    variable data
  }
  upvar \#0 ${Instance}::data data
  set data(path) $path
  eval ComboBox $path $args
  bind $data(path) <Destroy> [list ::Objects::Destroy $Instance]
  $data(path) configure -command [list ::Objects::ComboAutoFillCheck $Instance]
  return $data(path)
}  

proc ::Objects::ComboAutoFillCheck {this} {
  upvar \#0 ${this}::data data
  focus $data(path)
  set lvalues [$data(path) cget -values]
  set value [$data(path) cget -text]
  if {[lsearch $lvalues $value] == -1} {
    lappend lvalues $value
    $data(path) configure -values $lvalues 
  }
}



