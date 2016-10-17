#/////////////////////////////////////////////////////////////////////////////
# FILE    : mulselec.tcl
# PURPOSE : This file contains a namespace called MultipleSelector with its
#           functions. It's for creating a multiple selector of items and 
#           managing its events. A multiple selector consist of two listboxes:
#           unselected (left) and selected (right) with items which you can 
#           move from one listbox to the other.
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

package require blistbox

#/////////////////////////////////////////////////////////////////////////////
  namespace eval MultipleSelector {
#
# PURPOSE: The purpose of this namespace is organize code and functions.
#
#/////////////////////////////////////////////////////////////////////////////
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::MultipleSelector::CreateToplevel { w unselvar selvar
                                          {label1 "Not selected"}
                                          {label2 "Selected"} } {
#
# PURPOSE: Creates a multiple selector in a modal toplevel window.
#
# PARAMETERS:
#   w        -> widget's path (toplevel)
#   unselvar -> name of the variable that has the unselected items
#   selvar   -> name of the variable that has the selected items
#   label1   -> label of the unselected listbox
#   label2   -> label of the selected listbox
#
#/////////////////////////////////////////////////////////////////////////////

  global _MS_QUIT
  upvar #0 $unselvar unsel
  upvar #0 $selvar   sel
  variable widget
  set label1 [mc $label1]
  set label2 [mc $label2]
  
  toplevel $w
  
  # Dialog boxes should be transient with respect to their parent,
  # so that they will always stay on top of their parent window.  However,
  # some window managers will create the window as withdrawn if the parent
  # window is withdrawn or iconified.  Combined with the grab we put on the
  # window, this can hang the entire application.  Therefore we only make
  # the dialog transient if the parent is viewable.
  #
  if { [winfo viewable [winfo toplevel [winfo parent $w]]] } {
    wm transient $w [winfo toplevel [winfo parent $w]]
  }    
  
  wm title $w [mc "Multiple Selector"]
  wm protocol $w WM_DELETE_WINDOW "set _MS_QUIT 0"
  #wm geometry $w +[winfo partent $w]150+100
  wm geometry $w +350+200
  CreateFrame $w.f $unsel $sel $label1 $label2
  
  set fb [frame $w.fb]
  Button $fb.ok -image [::Bitmap::get accept] -relief link \
                -text [mc "Accept"] -command "set _MS_QUIT 1" \
                -helptext [mc "Accept selection"]
  Button $fb.cancel -image [::Bitmap::get txt] -relief link \
                    -text [mc "Cancel"] -command "set _MS_QUIT 0" \
                    -helptext [mc "Cancel selection"]
  
  grid $fb.ok $fb.cancel 
  grid $w.f -sticky news
  grid $w.fb -sticky ew -pady 2
  grid rowconfigure    $w 0 -weight 1
  grid columnconfigure $w 0 -weight 1
  
  set oldFocus [focus]
  set oldGrab [grab current $w]
  if {[string compare $oldGrab ""]} {
    set grabStatus [grab status $oldGrab]
  }
  
  grab $w
  focus $w
  tkwait variable _MS_QUIT
  
  if {$_MS_QUIT} {
    set unsel [GetUnselected $w.f]
    set sel   [GetSelected $w.f]
  }    
  catch {
    focus $oldFocus
  }
  catch {
    # It's possible that the window has already been destroyed,
    # hence this "catch".  Delete the Destroy handler so that
    # data(quit) doesn't get reset by it.
  
    #bind $w <Destroy> {}
    destroy $w
  }
  if {[string compare $oldGrab ""]} {
    if {[string compare $grabStatus "global"]} {
      grab $oldGrab
    } else {
      grab -global $oldGrab
    }
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::MultipleSelector::CreateFrame {w unsel sel
                                       {label1 "Not selected"}
                                       {label2 "Selected"} } {
#
# PURPOSE: Creates a multiple selector in a frame.
#
# PARAMETERS:
#   w        -> widget's path (frame)
#   unsel    -> list of unselected items
#   sel      -> list of selected items
#   label1   -> label of the unselected listbox
#   label2   -> label of the selected listbox
#
# RETURN: Path of multiple selector
#
#/////////////////////////////////////////////////////////////////////////////

  upvar #0 $w data
  set label1 [mc $label1]
  set label2 [mc $label2]

  set data(OnChanged) ""

  frame $w -class MultipleSelector
  set left [frame $w.left]
  set right [frame $w.right]

  # listbox 1 
  label $w.l1 -text $label1 -bd 1 -relief raised
  ScrolledWindow $w.sw1
  blistbox $w.sw1.lb -selectmode extended \
    -breadonly true -bkeypress true -bparent $w
  $w.sw1 setwidget $w.sw1.lb
  bind $w.sw1.lb <Button-1> +[list focus -force %W]
  bind $w.sw1.lb <Double-Button-1> +[list ::MultipleSelector::LBLeftToRight $w]

  # buttons
  set bbox [ButtonBox $w.bb -orient vertical -spacing 0 -padx 4 -pady 1]
  $bbox add -image [Bitmap::get play] -height 16 -width 16 \
    -command [list ::MultipleSelector::LBLeftToRight $w] -relief link \
    -helptext [mc "Moves selection from left to right"]
  $bbox add -image [Bitmap::get backward] -height 16 -width 16 \
    -command [list ::MultipleSelector::LBRightToLeft $w] -relief link \
    -helptext [mc "Moves selection from right to left"]
  $bbox add -image [Bitmap::get end] -height 16 -width 16 \
    -command [list ::MultipleSelector::LBAllLeftToRight $w] -relief link \
    -helptext [mc "Moves all from left to right"]
  $bbox add -image [Bitmap::get begin] -height 16 -width 16 \
    -command [list ::MultipleSelector::LBAllRightToLeft $w] -relief link \
    -helptext [mc "Moves all from right to left"]

  # listbox 2
  label $w.l2 -text $label2 -bd 1 -relief raised 
  ScrolledWindow $w.sw2
  blistbox $w.sw2.lb -selectmode extended \
    -breadonly true -bkeypress true -bparent $w
  $w.sw2 setwidget $w.sw2.lb
  bind $w.sw2.lb <Button-1> +[list focus -force %W]
  bind $w.sw2.lb <Double-Button-1> +[list ::MultipleSelector::LBRightToLeft $w]
  
  # fill the listbox
  FillLB $w $unsel $sel

  grid $w.l1   $bbox  $w.l2  -sticky news 
  grid $w.sw1  ^      $w.sw2 -sticky news
  grid rowconfigure    $w 0 -weight 0
  grid rowconfigure    $w 1 -weight 1
  grid columnconfigure $w 0 -weight 1 
  grid columnconfigure $w 1 -pad 5
  grid columnconfigure $w 2 -weight 1
  update
  bind <Destroy> $w [list ::MultipleSelector::KillData $w]
  return $w
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::MultipleSelector::EvalOnChanged { w } {
#
# PURPOSE: Evaluates a procedure when any of the listboxes changes any of
#          its items.
#
# PARAMETERS: 
#    w -> widget's path (frame). It stores data
#
#/////////////////////////////////////////////////////////////////////////////

  upvar #0 $w data
    
  if { [string length $data(OnChanged)] } {
    eval $data(OnChanged)
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::MultipleSelector::FillLB { w unsel sel } {
#
# PURPOSE: Fill unselected and selected listboxes of widget 'w'.
#
# PARAMETERS: 
#    w     -> widget's path
#    unsel -> list of unselected items
#    sel   -> list of selected items
#
#/////////////////////////////////////////////////////////////////////////////

  $w.sw1.lb delete 0 end
  $w.sw2.lb delete 0 end
  foreach it $unsel {
    $w.sw1.lb insert end $it
  }
  foreach it $sel {
    $w.sw2.lb insert end $it
  }
  EvalOnChanged $w
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::MultipleSelector::OnChanged { w script } {
#
# PURPOSE: Stores a script to evaluate when a listbox changes.
#
# PARAMETERS: 
#    w      -> widget's path (stores data, the script in this case)
#    script -> script to evaluate when a listbox changes
#
#/////////////////////////////////////////////////////////////////////////////
  upvar #0 $w data

  set data(OnChanged) $script
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::MultipleSelector::ListTransferSel {scr dst} {
#
# PURPOSE: Transfer selected items from scr listbox to dst listbox.
# 
# PARAMETERS: 
#    scr -> source listbox path
#    dst -> destination listbox path
#
#/////////////////////////////////////////////////////////////////////////////
  foreach i [$scr curselection] {
    $dst insert end [$scr get $i]
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::MultipleSelector::ListDeleteSel {w} {
#
# PURPOSE: Deletes selected items of a listbox
# 
# PARAMETERS:
#    w -> listbox's path
#
#/////////////////////////////////////////////////////////////////////////////
  foreach i [lsort -integer -decreasing [$w curselection]] {
    $w delete $i
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::MultipleSelector::LBLeftToRight { w } {
# 
# PURPOSE: Moves selected items from unselected listbox to selected listbox
# 
# PARAMETERS:
#    w -> multiple selector's path
#
#/////////////////////////////////////////////////////////////////////////////
  ListTransferSel $w.sw1.lb $w.sw2.lb
  ListDeleteSel   $w.sw1.lb
  EvalOnChanged $w
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::MultipleSelector::LBRightToLeft { w } {
# 
# PURPOSE: Moves selected items from selected listbox to unselected listbox
# 
# PARAMETERS:
#    w -> multiple selector's path
#
#/////////////////////////////////////////////////////////////////////////////
  ListTransferSel $w.sw2.lb $w.sw1.lb
  ListDeleteSel   $w.sw2.lb    
  EvalOnChanged $w
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::MultipleSelector::LBAllLeftToRight { w } {
# 
# PURPOSE: Moves all items from unselected listbox to selected listbox
# 
# PARAMETERS:
#    w -> multiple selector's path
#
#/////////////////////////////////////////////////////////////////////////////
  eval "$w.sw2.lb insert end [$w.sw1.lb get 0 end]"
  $w.sw1.lb delete 0 end
  EvalOnChanged $w
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::MultipleSelector::LBAllRightToLeft { w } {
# 
# PURPOSE: Moves all items from selected listbox to unselected listbox
# 
# PARAMETERS:
#    w -> multiple selector's path
#
#/////////////////////////////////////////////////////////////////////////////
  eval "$w.sw1.lb insert end [$w.sw2.lb get 0 end]"
  $w.sw2.lb delete 0 end
  EvalOnChanged $w
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::MultipleSelector::GetSelected { w } {
# 
# PURPOSE: Gets item from selected listbox (rigth)
# 
# PARAMETERS:
#    w -> multiple selector's path
#
#/////////////////////////////////////////////////////////////////////////////
  $w.sw2.lb get 0 end
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::MultipleSelector::GetUnselected { w } {
# 
# PURPOSE: Gets item from unselected listbox (left)
# 
# PARAMETERS:
#    w -> multiple selector's path
#
#/////////////////////////////////////////////////////////////////////////////
  $w.sw1.lb get 0 end
}
