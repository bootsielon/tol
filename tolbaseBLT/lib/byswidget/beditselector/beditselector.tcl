#/////////////////////////////////////////////////////////////////////////////
# FILE    : beditselector.tcl
# PURPOSE : Bayes Edit Selector. Creates a widget that has an entry widget
#           and a button that calls a multiple selector (a widget that allows
#           selecting multiple items from a list, see bmultiselect.tcl).
#/////////////////////////////////////////////////////////////////////////////

package require byscommon
package require bmultiselect
package require bfieldsdialog


#/////////////////////////////////////////////////////////////////////////////
::snit::widget beditselector {
#
# PURPOSE: Beditselector widget class definition
#
#/////////////////////////////////////////////////////////////////////////////
#/////////////////////////////////////////////////////////////////////////////
# options:
#  -list  : initial list values
#  -sel : selected values list
#  -label : label of the widget
#  -labelsel : label for the selected list on the multiselect child
#  -labelunsel : label for the unselected list on the multiselect child
#  -font : font for the blistboxplus inthe bmultiselect component
#  -readonly : if not true an icon appears to allow introduce items manually
#  -cols : columns associated with the item fields 
#  -selcols : columns which are displayed in the selection
#  -entcols : columns which are displayed in the entry
#  -hlpcols : columns which are displayed in the helptext
#  -unique : if true the multiple selector associated can only select one item
#  -width : width of the entry component.
#           If the size is zero, the component does not visualize
#  -lwidth : width of the label component
#  -msgeometry : geometry for the multiselect dialog component
#  -procedure : procedure to execute before showing the list of available \
#     values. It must return a the list of posible values.
#  -editstate: normal, active -> enabled editselector,
#              disabled -> disabled editselector
#  -postcommand: Procedure to execute after select any element
#  -iconopen: Icon to visualize in the button to open selection
#  -iconwrite: Icon to visualize in the button to write item
#
# variables:
#   label: label component
#   entry: entry component
#   menu: the contextual menu
#   dialogWRentry: entry variable of the insert element dialog
#   buttonMS: button that launches the Multiple Selector
#   buttonWR: button that launches the insert element dialog
#/////////////////////////////////////////////////////////////////////////////

  option -list {}
  option -sel {}
  option -label ""
  option -labelsel "Selected"
  option -labelunsel "Not Selected"
  option -font {arial 10}
  option -readonly "true"
  option -cols {}
  option -selcols {}
  option -entcols {}
  option -hlpcols {}
  option -unique false
  option -width  20
  option -lwidth 0
  option -msgeometry ""
  option -procedure ""
  option -editstate normal
  option -postcommand ""
  option -iconopen  puntos
  option -iconwrite lapiz
  
  variable label
  variable entry
  variable menu
  variable dialogWRentry
  variable buttonMS
  variable buttonWR

  variable created 0

  delegate option * to entry

  #///////////////////////////////////////////////////////////////////////////
  constructor { args } {
  #
  # PURPOSE: beditselector constructor
  #
  # PARAMETERS:
  #   args ->
  #
  #///////////////////////////////////////////////////////////////////////////
    # Apply all options passed at creation time.
    $self configurelist $args
    # Create widget
    $self _CreateWidget
    $self configure -labelsel [mc $options(-labelsel)]
    $self configure -labelunsel [mc $options(-labelunsel)]
    if { "" eq $options(-selcols) } {
      set options(-selcols) $options(-cols)
    }
    if { "" eq $options(-entcols) } {
      set options(-entcols) $options(-cols)
    }
    if { "" eq $options(-hlpcols) } {
      set options(-hlpcols) $options(-cols)
    }
    if { "" eq $options(-msgeometry) } {
      set swidth [expr [winfo screenwidth .] / 3]
      set sheight [expr [winfo screenwidth .] / 3]
      set options(-msgeometry) "${swidth}x${sheight}+${swidth}+[expr ${sheight} / 2]"
    }
    
    $self configure -postcommand  [from args -postcommand ""]
  }
  
  #///////////////////////////////////////////////////////////////////////////
  #
  # PURPOSE: Metodos cget y configure
  #
  #///////////////////////////////////////////////////////////////////////////
  
  onconfigure -label { t } {
    if { $created } {
	  $label configure -text $t
      if { [llength $t] } {
        grid $self.l
      } else {
        grid remove $self.l
      }
    }
    set options(-label) $t
  }
  
  oncget -label {
    if { $created } {
      return [$label cget -text]labe}
  }

  onconfigure -width { w } {
    if { $created } {
      if {[expr $w == 0]} {
        grid remove $entry
      }
      $entry configure -width $w
    }
    set options(-width) $w
  }
  
  oncget -width {
    if { $created } {
      return [$entry cget -width]
    }
  }
  
  onconfigure -editstate { stt } {
    if { $created } {
      $buttonMS configure -state $stt
      $entry configure -state $stt
      if {([info exists buttonWR]) && ([winfo exists $buttonWR])} {
        $buttonWR configure -state $stt
      }
      if {([info exists label]) && ([winfo exists $label])} {
        $label configure -state $stt
      }
    }
    set options(-editstate) $stt
  }  
  
  oncget -editstate {
    if { $created } {
      return [$buttonMS cget -state]
    }
  }

  onconfigure -lwidth { w } {
    if { $created } {
      $label configure -width $w
    }
    set options(-lwidth) $w
  }
  
  oncget -lwidth {
    if { $created } {
      return [$label cget -width]
    }
  }
  
  onconfigure -sel { list } {
    set options(-sel) $list
    set ent [$self selection get $options(-entcols)]
    set hlp [$self selection get $options(-hlpcols)]
    if { $created } {
      $entry configure -text [$self _ListToStr $ent] \
        -helptext [::ListToStr $hlp "\n" { } { }]
    }
  }

  oncget -font {
    if { [winfo exists $self.d] } {
      set fr [$self.d getframe]
      return [$fr.bms cget -font]
    }
  }

  onconfigure -msgeometry { geom } {
    set options(-msgeometry) $geom
    if { [winfo exists $self.d] } {
      $self.d configure -geometry $geom
    }
  }
  
  onconfigure -postcommand { lst } {
    set options(-postcommand) $lst
  }

  oncget -msgeometry {
    if { [winfo exists $self.d] } {
      $self.d cget -geometry
    }
  }

#/////////////////////////////////////////////////////////////////////////////
# PRIVATE METHODS
#/////////////////////////////////////////////////////////////////////////////

  #///////////////////////////////////////////////////////////////////////////
  method _CreateWidget { args } {
  #
  # PURPOSE: Creates an edit selector. It creates the widget composed by
  #          a label, an entry and a button that shows a bmultiselect widget.
  #          Also creates a procedure asociated.
  #
  # PARAMETERS:
  #   args ->
  #
  #///////////////////////////////////////////////////////////////////////////
    
    set options(-labelsel) [mc $options(-labelsel)]
    set options(-labelunsel) [mc $options(-labelunsel)]
    set dialogWRentry ""
    install label using label $self.l -text $options(-label) -anchor w \
      -width $options(-lwidth)
    install entry using Entry $self.e -state normal -editable false \
      -width $options(-width)
    install buttonMS using Button $self.b1 \
      -image [Bitmap::get $options(-iconopen)] \
      -relief link -helptext [mc "Multiple Selector"] \
      -command [list  $self _OpenMS]
    if {! $options(-readonly) } {
      install buttonWR using Button $self.b2 \
        -image [Bitmap::get $options(-iconwrite)] \
        -relief link -helptext [mc "Introduce item"] \
        -command [list  $self _OpenWR]
    }
  
    $self _CreateMenu
    set created 1

    $self configure -width $options(-width) -editstate $options(-editstate)
	
    if {[expr $options(-width) == 0]} {
      grid $self.l $self.b1 -sticky ew -padx 2      
    } else {
      grid $self.l $self.e $self.b1 -sticky ew -padx 2
    }
    if { ! [llength [$label cget -text]] } { grid remove $self.l }
    if { ! $options(-readonly) } {
      grid $self.b2 -row 0 -column 4 -sticky ew -padx 2
    }
    grid columnconfigure $self 1 -weight 1
  }


  #///////////////////////////////////////////////////////////////////////////
  method _CreateMenu {args} {
  #
  # PURPOSE: Append menu to listbox.
  #
  #///////////////////////////////////////////////////////////////////////////
    install menu using menu $self.m -tearoff 0

    # read option
    $menu add command -label [mc "Select from a List"] \
                      -command "$self _OpenMS"
    $menu add command -label [mc "Select all"] \
                      -command "$self _SelectAll; $self _PostCommand" \
                      -accelerator CTRL+E
    $menu add command -label [mc "Unselect all"] \
                      -command "$self selection clear; $self _PostCommand"
    # read/write option
    if {![$self cget -readonly]} {
      $menu add separator
      $menu add command -label [mc "Insert"] \
                        -command "$self _OpenWR; $self _PostCommand" \
                        -accelerator CTRL+I
    }
    # read option
    $menu add command -label [mc "Copy"] \
                      -command "$self _CopySelected" -accelerator CTRL+C

    set comps "$label $entry $buttonMS"
    if { ! $options(-readonly) } { lappend comps $buttonWR }
    foreach comp $comps {
      bind $comp <Control-e> "$self _SelectAll; $self _PostCommand"
      bind $comp <Control-i> "$self _OpenWR; $self _PostCommand"
      bind $comp <Control-c> "$self _CopySelected"
      bind $comp <Button-1> +[list focus -force %W]
      bind $comp <Button-3> "tk_popup $menu %X %Y"
    }

    bind $self <Button-1> +[list focus -force %W]
    bind $self <Button-3> "tk_popup $menu %X %Y"
  }

  #///////////////////////////////////////////////////////////////////////////
  method _SelectAll {} {
  #
  # PURPOSE: Sets the selected list to all posible values
  #
  # RETURN: The list
  #
  #///////////////////////////////////////////////////////////////////////////
    if {$options(-unique)} {
      tk_messageBox -icon warning -message \
        [mc "The option to select to all the elements of the list this disabled"]\
        -title [mc "Attention"] -type ok
    } else {
      $self configure -sel [$self get]
    }
  }


  #///////////////////////////////////////////////////////////////////////////
  method _CopySelected {} {
  #
  # PURPOSE: Copy the selected items to the clipboard
  #
  # RETURN: The list
  #
  #///////////////////////////////////////////////////////////////////////////
    clipboard clear
    clipboard append $options(-sel)
  }

  #///////////////////////////////////////////////////////////////////////////
  proc _AddWR { elem father } {
  #
  # PURPOSE: adds the item introduced in the dialog to the selected list
  #
  # PARAMETERS:
  #  elem -> the element to insert from the add dialog
  #  father -> The dialog's window pathname
  #
  #///////////////////////////////////////////////////////////////////////////
    # unique ?
    if {[$father cget -unique]} {
      set list {}
    } else {
      set list [$father curselection]      
    }
    # add new value
    eval lappend list $elem
    eval $father selection set $list
  }


  #//////////////////////////////////////////////////////////////////////////
  method _OpenWR {} {
  #
  # PURPOSE: Calls (and creates) a dialog for adding elements manually.
  #
  #///////////////////////////////////////////////////////////////////////////
    destroy $self.dw
    set dialog  [bfieldsdialog $self.dw -title $options(-label) -parent $self \
      -fields $options(-cols) -procedure [myproc _AddWR] \
      -buttons { add exit} -parent $self]

    $dialog draw
  }


  #///////////////////////////////////////////////////////////////////////////
  method _CloseMS { w { onlyexit 0 } } {
  #
  # PURPOSE: closes the dialog and returns the result
  #
  # PARAMETERS:
  #  w -> The dialog's window pathname
  #  onlyexit -> the dialogs end without doing nothing
  #
  #///////////////////////////////////////////////////////////////////////////    
    if { $onlyexit } { 
      $self configure -msgeometry [wm geometry $self.d]
      $w enddialog ""
    } else {
      set f [$w getframe]
      set result [$f.bms get selected]
      $self configure -sel $result
      $self configure -msgeometry [wm geometry $self.d]
      $w enddialog $result
    }
    
    $self _PostCommand
  }
  
  #//////////////////////////////////////////////////////////////////////////  
  method _PostCommand { } {
  #
  # PURPOSE: Execute postcommand
  #
  #///////////////////////////////////////////////////////////////////////////
    set cmd [$self cget -postcommand]
    if {[string length $cmd]} {
      eval $cmd
    }  
  }


  #//////////////////////////////////////////////////////////////////////////
  method _OpenMS {} {
  #
  # PURPOSE: Calls (and creates) a multiple selector widget with the selected
  #          and unselected items.
  #
  #///////////////////////////////////////////////////////////////////////////

    destroy $self.d
    set dialog [Dialog $self.d -title $options(-label) \
                -modal local -parent $self]
    
    set dialogframe [$dialog getframe]
    wm protocol $dialog WM_DELETE_WINDOW [list $self _CloseMS $dialog]
    bmultiselect $dialogframe.bms -cols $options(-cols) \
          -hiddencols [lsub $options(-cols) $options(-selcols)] \
	  -unsel [lsub [$self get] $options(-sel)] -sel $options(-sel) \
	  -unique $options(-unique) -font $options(-font) \
          -labelunsel $options(-labelunsel) -labelsel $options(-labelsel)
  
    set dialogframe_bbox \
          [ButtonBox $dialogframe.bb -orient horizontal -spacing 10 -padx 0 -pady 0 \
          -homogeneous false]
    $dialogframe_bbox add -text [mc "Accept"] -relief link \
          -helptext [mc "Accept selection"] \
          -image [::Bitmap::get accept] -compound left -padx 5 -pady 1 \
           -command [list $self _CloseMS $dialog]
    $dialogframe_bbox add -text [mc "Cancel"] -relief link \
          -helptext [mc "Cancel selection"]\
          -image [::Bitmap::get cancelx] -compound left -padx 5 -pady 1 \
            -command [list $self _CloseMS $dialog 1]     
    
    pack $dialogframe_bbox -fill x -expand false -side bottom
    pack $dialogframe.bms -fill both -expand true
    
    if { $options(-msgeometry) ne "" } {
      $dialog configure -geometry $options(-msgeometry)
    }
    $dialog draw
  }


  #///////////////////////////////////////////////////////////////////////////
  method _ListToStr { list } {
  #
  # PURPOSE: Concatenates list element names into a string for the label of the
  #          edit selector widget. If list has more than one element the string
  #          is first element name plus '...'.
  #
  # RETURN: The string created.
  #
  #///////////////////////////////////////////////////////////////////////////

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
# PUBLIC METHODS
#/////////////////////////////////////////////////////////////////////////////

  #///////////////////////////////////////////////////////////////////////////
  method get { {cols ""} } {
  #
  # PURPOSE: Gets the given list or obtains it from a process
  #
  # PARAMETERS:
  #   cols: specifies only which column to return data for
  #
  # RETURN: The list
  #
  #/////////////////////////////////////////////////////////////////////////// 
    if { $options(-procedure) ne "" } {
      set result [eval $options(-procedure) $self]
    } else {
      set result $options(-list)
    }

					      
    if { "" ne $cols } {
      set colsresult {}
      foreach elem $result {
        set elemres {}
        foreach col $cols {
          lappend elemres [lindex $elem [lsearch $options(-cols) $col]]
        }
        lappend colsresult $elemres
      }
      set result $colsresult
    }

    return $result
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method selectAll {} {
  #
  # PURPOSE: Public method to set the selected list to all posible values
  #
  # RETURN: The list
  #
  #/////////////////////////////////////////////////////////////////////////// 
    $self _SelectAll
  }
  

  #///////////////////////////////////////////////////////////////////////////
  method selection { args } {
  #
  # PURPOSE: selection command, performs various operations with selected items
  #    like get, set or clear the selected items
  #  
  # PARAMETERS:
  #   arg0: <none>, set, get or clear
  #   arg1: <none>, selected or unselected (for set and get switches)
  #   arg2: <none>, cols (for get selected and get unselected switches)
  #
  # RETURN: returns in the case the selected or unselected items
  #
  #///////////////////////////////////////////////////////////////////////////
    switch -exact [lindex $args 0] {
      get {
        if { [lindex $args 1] eq "unselected" } {
	  set result [lsub [$self get] $options(-sel)]
	  set cols [lindex $args 2]
	} else {
	  set result $options(-sel)
	  if { [lindex $args 1] eq "selected" } {
	    set cols [lindex $args 2]
	  } else {
	    set cols [lindex $args 1]
	  }
	}
        if { "" ne $cols } {
          set colsresult {}
          foreach elem $result {
            set elemres {}
            foreach col $cols {
              lappend elemres [lindex $elem [lsearch $options(-cols) $col]]
            }
            lappend colsresult $elemres
          }
          set result $colsresult
        }
	return $result
      }
      set {
        if { [lindex $args 1] eq "unselected" } {
          set values [lsub [$self get] [lrange $args 2 end]]
          $self configure -sel $values
	} else { if { [lindex $args 1] eq "selected" } {
	    $self configure -sel [lrange $args 2 end]
	  } else {
	    $self configure -sel [lrange $args 1 end]
	  }
	}
      }
      clear {
        $self configure -sel {}
      }
      default {
        return $options(-sel)
      }
    }
  }


  #///////////////////////////////////////////////////////////////////////////
  method curselection {  } {
  #
  # RETURN: returns the selected items.
  #
  #///////////////////////////////////////////////////////////////////////////
    $self selection get
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method getButtonAccept {  } {
  #
  # RETURN: returns accept Button Path.
  #
  #///////////////////////////////////////////////////////////////////////////
    return $buttonMS
  }
}


  
# TEST CODE

proc __TbesElProc { w } {
  # this proc should load a list of values from any source
  return {{4 5 6} {cuatro cinco seis} {four five six}}
}


#/////////////////////////////////////////////////////////////////////////////
proc test_beditselector {} {
#
# PURPOSE: Test function of a beditselector
#
#/////////////////////////////////////////////////////////////////////////////
  destroy .top
  toplevel .top
  variable uniq
  variable multicol
  variable withproc

  TitleFrame .top.f1 -text "BEditSelector test"
  set f1 [.top.f1 getframe]
  set bes [beditselector $f1.bes -label Selector -width 35 -readonly false]
  puts "beditselector widget : $bes"
  button $f1.b -text "View All" -command "cmdViewAll"
  LabelEntry $f1.le -label "All values" -editable false -width 60
  
  TitleFrame .top.f2 -text "Configure"
  set f2 [.top.f2 getframe]
  LabelEntry $f2.le1 -label "Columns" -text "col1 col2 col3" -width 60
  LabelEntry $f2.le2 -label "Value list" -width 60 \
                     -text "{1 2 3} {un dos tres} {one two three}"
  checkbutton $f2.cb1 -text "Multiple columns" -variable multicol
  checkbutton $f2.cb2 -text "Only one selected" -variable uniq
  checkbutton $f2.cb3 -text "With proc" -variable withproc
  $f2.cb1 select
  $f2.cb2 deselect
  $f2.cb3 deselect
  button $f2.b -text "Load config" -command "cmdLoad"
  LabelEntry .top.le -label Result

  grid $f1.bes -row 0 -column 0 -sticky "news"
  grid $f1.b   -row 0 -column 1 -sticky "nes"
  grid $f1.le  -row 1 -column 0 -sticky "news" -columnspan 2
  
  grid $f2.le1 -row 0 -column 0 -columnspan 4
  grid $f2.le2 -row 1 -column 0 -columnspan 4
  grid $f2.cb1 -row 2 -column 0
  grid $f2.cb2 -row 2 -column 1
  grid $f2.cb3 -row 2 -column 2
  grid $f2.b   -row 2 -column 3
  
  grid .top.f1 -row 0 -column 0 -padx 5 -pady 5 -sticky "news"
  grid .top.f2 -row 1 -column 0 -padx 5 -pady 5 -sticky "news"

  proc cmdLoad {} {
    variable uniq
    variable multicol
    variable withproc
	
    set bes .top.f1.f.bes
    set lst .top.f2.f.le2
    set cols .top.f2.f.le1

    set entcols [lrange [$cols cget -text] 0 0]
    set selcols [lrange [$cols cget -text] 1 end]
    set hlpcols [lrange [$cols cget -text]  0 1]
    $bes configure -list [$lst cget -text] -cols [$cols cget -text] \
                   -unique $uniq
    $bes configure -entcols $entcols -selcols $selcols -hlpcols $hlpcols 
    if { $withproc } { $bes configure -procedure "__TbesElProc"
    } else { $bes configure -procedure "" }
  }

  proc cmdViewAll {} {
    .top.f1.f.le configure -text [ .top.f1.f.bes cget -sel ]
  }
}

package provide beditselector 1.0
