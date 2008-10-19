#/////////////////////////////////////////////////////////////////////////////
# FILE    : listorde.tcl
# PURPOSE : This file contains a namespace called ListOrder with its
#           functions. It's for creating a listbox and managing its events.
#           This widgets consist of a listbox and two buttons for ordering the
#           items of the listbox. It can also order the items of other
#           aditional hidden list.
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
  ::msgcat::mclocale es
  ::msgcat::mcload [file join $toltk_script_path msgs]
}


#/////////////////////////////////////////////////////////////////////////////
  namespace eval ListOrder {
#
# PURPOSE: The purpose of this namespace is organize code and functions.
#
#/////////////////////////////////////////////////////////////////////////////
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::ListOrder::Create {w var1 {var2 ""} args} {
#
# PURPOSE: Creates a listbox and two button for ordering the items of the 
#          listbox.
#
# PARAMETERS:
#   w    -> widget's path (frame)
#   var1  -> name of list to order, which is showed at the listbox
#   var2  -> name of list to order, which is hidden
#   args -> other args
#
#/////////////////////////////////////////////////////////////////////////////
  frame $w -class ListOrder  

  ScrolledWindow $w.sw
  listbox $w.sw.lb -selectmode browse -listvariable $var1 -height 6
  $w.sw setwidget $w.sw.lb
  bind $w.sw.lb <Button-1> +[list focus -force %W]
  
  frame $w.f
  Button $w.f.bUp -image [Bitmap::get upNext] -height 16 -width 16 \
    -command [list ::ListOrder::Up $w $var2] -relief link \
    -helptext [mc "Moves up selected item one position"]

  Button $w.f.bDown -image [Bitmap::get downNext] -height 16 -width 16 \
    -command [list ::ListOrder::Down $w $var2] -relief link \
    -helptext [mc "Moves down selected item one position"]

  grid $w.f.bUp   -sticky s -padx 2 -pady 2
  grid $w.f.bDown -sticky n -padx 2 -pady 2

  grid $w.sw $w.f -sticky news
  grid columnconfigure $w 0 -weight 1
  grid rowconfigure    $w 0 -weight 1
  grid columnconfigure $w 1 -pad 5

}


#/////////////////////////////////////////////////////////////////////////////
  proc ::ListOrder::Up {w {var ""}} {
#
# PURPOSE: Moves the item selected in the listbox one position upwards and
#          sorts the items of a list according the up movement.
#
# PARAMETERS:
#   w   -> listorder widget's path
#   var -> name of list to order
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 $var list
  set id [$w.sw.lb curselection]
  if ![string equal $id ""] {
    set item [$w.sw.lb get $id]
    if [expr $id > 0] {
      $w.sw.lb delete $id
      $w.sw.lb insert [expr $id - 1] $item 
      $w.sw.lb selection set [expr $id -1]
      if ![string equal $var ""] {      
        set itemList [lindex $list $id]
        set list [lreplace $list $id $id]
        set list [linsert $list [expr $id -1] $itemList]
      }
    }
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::ListOrder::Down {w {var ""}} {
#
# PURPOSE: Moves the item selected in the listbox one position downwards and
#          sorts the items of a list according the down movement
#
# PARAMETERS:
#   w   -> listorder widget's path
#   var -> name of list to order
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 $var list
  set id [$w.sw.lb curselection]
  if ![string equal $id ""] {
    set item [$w.sw.lb get $id]
    if [expr $id < [expr [$w.sw.lb index end] -1]] {
      $w.sw.lb delete $id
      $w.sw.lb insert [expr $id + 1] $item
      $w.sw.lb selection set [expr $id + 1]
      if ![string equal $var ""] {
        set itemList [lindex $list $id]
        set list [lreplace $list $id $id]
        set list [linsert $list [expr $id + 1] $itemList]
      }
    }
  }
}
