#/////////////////////////////////////////////////////////////////////////////
# FILE    : ediselec.tcl
# PURPOSE : This file contains a namespace called EditSelector with its
#           functions. It's for creating a widget that has an entry widget
#           and a button that calls a multiple selector (a widget that allows
#           selecting multiple items from a list of items, see mulselec.tcl).
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
  namespace eval EditSelector {
#
# PURPOSE: The purpose of this namespace is organize code and functions.
#          A special namespace is created for each instance of editSelector.
#
#/////////////////////////////////////////////////////////////////////////////

}


#/////////////////////////////////////////////////////////////////////////////
  proc ::EditSelector::Create { parent w list {label "Nombre" } {pack 1} } {
#
# PURPOSE: Creates an instance of a edit selector. It creates the widget
#          composed by a label, an entry and a button that shows a multiple
#          selector widget. Also creates a procedure asociated.
#
# PARAMETERS:
#   parent -> widget's parent
#   w      -> name of widget
#   list   -> list of initial values of multiple selector
#   label  -> text of the label
#   pack   -> if the widget must be packed or not (yes by default)
#
# RETURN: The procedure created.
#
#/////////////////////////////////////////////////////////////////////////////

  set Instance ::EditSelector::${parent}$w 
  if { [lsearch [namespace children ::EditSelector] $Instance] != -1 } {
    error "$Instance already exists as a children of ::EditSelector"
  }

  #  A namespace is created for each edit selector widget.

  namespace eval $Instance {

    # data -> array with information about the instance:
    #   parent     -> widget's parent path
    #   label      -> label widget's path
    #   entry      -> entry widget's path
    #   button     -> button widget's path
    #   unselected -> list of unselected items
    #   selected   -> list of selected items
    #

    variable data
  }

  upvar #0 ${Instance}::data data
  
  #set f [frame  $w]
  set data(parent) $parent
  set data(label)  [label  $parent.${w}ldes -text $label -anchor w]
  set data(entry)  [Entry  $parent.${w}sel  -state disabled \
                          -disabledforeground black]
  set data(button) [Button $parent.${w}bsel -image [Bitmap::get puntos] \
                          -relief link -helptext [mc "Multiple Selector"] \
                          -command [list ::EditSelector::OpenMS $Instance]]
  if $pack {
    grid $data(label) $data(entry) $data(button) -sticky ew -padx 2
    grid columnconfigure $parent 1 -weight 1
  }

  bind $data(label) <Destroy> [list ::EditSelector::Destroy $Instance]
  InitInstance $Instance $list

  set InstanceProc ${Instance}::proc 
  proc $InstanceProc { option args} \
      "eval ::EditSelector::InstanceProc {$Instance} \$option \$args"
  return $InstanceProc
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::EditSelector::InstanceProc { instance option args } {
#
# PURPOSE: Procedure created to attend calls to the widget. Sets or gets the
#          selected or unselected items of the multiple selector.
#
# PARAMETERS:
#   instance -> Instance of edit selector
#   option   -> set or get
#   args     -> arguments of the command:
#               if option is set, args is a list of the form: 
#                 {selected|unselected values}
#               if option is get, args is selected or unselected
#
# RETURN: If options is get it returns the selected or unselected items of the
#         multiple selector.
#
#/////////////////////////////////////////////////////////////////////////////

  upvar #0 ${instance}::data data

  switch -- $option {
    get {
      if { [catch {set a $data($args)}] } {
       error "Invalid argument '$args'"
      }
      return $a
    }
    set {
      set item [lindex $args 0]
      if { ([string equal $item unselected]) ||
           ([string equal $item selected]) } {
        set data($item) [lindex $args 1]
      }
    }
    default {
      error "Invalid option '$option': should be set or get"
    }
  }
}
    

#/////////////////////////////////////////////////////////////////////////////
  proc  ::EditSelector::Destroy { instance } {
#
# PURPOSE: Destroy the instance given by instance.
#
#/////////////////////////////////////////////////////////////////////////////

  namespace delete $instance
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::EditSelector::InitInstance { instance list} {
#
# PURPOSE: Initializes the lists of selected and unselected items.
#
# PARAMETERS:
#   instance -> the instance 
#   list     -> list of selected items
#
#/////////////////////////////////////////////////////////////////////////////

  upvar #0 ${instance}::data data
      
  set data(unselected) $list
  set data(selected)   {}
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::EditSelector::OpenMS { instance } {
#
# PURPOSE: Calls (and creates) a multiple selector widget with the selected
#          and unselected items.
#
# PARAMETERS:
#   instance -> the instance of edit selector
#
#/////////////////////////////////////////////////////////////////////////////

  upvar #0 ${instance}::data data
  
  ::MultipleSelector::CreateToplevel $data(parent).s \
      ${instance}::data(unselected) ${instance}::data(selected)
  $data(entry) configure -text [ListToStr $data(selected)] \
                         -helptext [::ListToStr $data(selected) "\n" { } { }]
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::EditSelector::ListToStr { list } {
#
# PURPOSE: Concatenates list element names into a string for the label of the
#          edit selector widget. If list has more than one element the string
#          is first element name plus '...'.
#
# RETURN: The string created.
#
#/////////////////////////////////////////////////////////////////////////////

  switch -exact -- [llength $list] {
    0       {
      set result {}
    }
    1       {
      set result [lindex $list 0]
    }
    default {
      set result [lindex $list 0]...
    }
  }
  return $result
}

#/////////////////////////////////////////////////////////////////////////////
proc ::EditSelector::BClear { instance } {
#
# PURPOSE: Empty de element
#
#/////////////////////////////////////////////////////////////////////////////
  upvar #0 ${instance}::data data
  
  set data(unselected) {}
  set data(selected)   {}

  $data(entry) configure -text "" -helptext ""
}




