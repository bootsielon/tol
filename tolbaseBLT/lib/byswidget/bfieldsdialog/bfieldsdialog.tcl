#/////////////////////////////////////////////////////////////////////////////
# FILE    : bfieldsdialog.tcl
# PURPOSE : Bayes Fields Dialog. Creates a dialog that has many fields that
#           could be entries or comboboxes and configurable buttons (accept,
#           apply , add, info, cancel and exit) and calls an external procedure
#           with the selected data in the fields
#/////////////////////////////////////////////////////////////////////////////

package require byscommon
package require BWidget

#/////////////////////////////////////////////////////////////////////////////
::snit::widgetadaptor bfieldsdialog {
#
# PURPOSE: Beditselector widget class definition
#
#/////////////////////////////////////////////////////////////////////////////
#/////////////////////////////////////////////////////////////////////////////
# options:
#  -fields : names of the fields for each entry or combobox
#  -list  : if a list is provided then a combo box is available to select values
#           it's in fact a list of lists one for each combo
#           must be provided at definition time
#  -width : width of the entries/comboboxes components
#  -orientation : orientation for the fields, vertical or horizontal
#  -procedure : this procedure takes two arguments a list and a window path
#    the list, will contain the fields of an element introduced
#    the window path is the path of the widget father's
#    this procedure must do the effective insert command
#  -initialvalue: initial value to display
#  -titles: list with titles to show in each one of columns
#
# variable:
#   labels: array of labels
#   entries: array of entries
#   combos: array of comboboxes
#   created: indicates if all components of the widget have been created
#
#/////////////////////////////////////////////////////////////////////////////

  option -fields {}
  option -list {}
  option -initialvalue ""
  option -orientation "horizontal"
  option -width
  option -procedure ""
  option -buttons {cancel accept}
  option -info ""
  option -titles {}
  
  variable labels
  variable entries
  variable combos

  variable created


  #///////////////////////////////////////////////////////////////////////////
  constructor { args } {
  #
  # PURPOSE: bfieldsdialog constructor
  #
  # PARAMETERS:
  #   args ->
  #
  #///////////////////////////////////////////////////////////////////////////

    set list [from args -list {}]
    set fields [from args -fields {" "}]
    set titles [from args -titles {}]
    if {[llength $titles] eq 0} {
      set titles $fields
    }
    if { "" eq $fields } { set fields {" "} }
    set initialvalue [from args -initialvalue {}]

    # instaling components
    installhull [Dialog $self -modal local]

    set dialogframe [$self getframe]

    set i 0
    if { "" eq $list } {
      foreach col $fields {
        if {!([lindex $titles $i] eq "")} {
          install labels($i) using label $dialogframe.l$i -text [lindex $titles $i]
        }
        install entries($i) using Entry $dialogframe.e$i \
          -text [lindex $initialvalue $i]
        incr i
      }
    } else {
      if {!([lindex $titles $i] eq "")} {
        install labels($i) using label $dialogframe.l$i -text [lindex $titles $i]
      }
      install combos using bcomboboxplus $dialogframe.cb$i \
                                             -cols $fields \
                                             -widthlb auto
      foreach item [lindex $list $i] {
        $combos insert end $item
      }
      $combos selectIndex 2
      $combos selectIndex 1
      #When Return is pressed the test in the entry is selected
      set comboek [$combos getEntryKey]
      bind $comboek <Return> [list $comboek selection range 0 end]
    }

    set created 1

    #configuring
    $self configurelist $args
    $self configure -list $list
    $self configure -fields $fields
    $self configure -initialvalue $initialvalue
    $self configure -title [mc [$self cget -title]]
    
    # adding the buttons
    set notapply 1
    foreach button $options(-buttons) {
      switch -exact $button {
        info {
          $self add -image [::Bitmap::get infor] -relief link \
                -text [mc "Info"] -compound left \
                -command [list $self _Info] -helptext [mc "Information"]
        }

        accept {
          $self add -image [::Bitmap::get accept] -relief link \
                -text [mc "Accept"] -compound left \
                -command [list $self _Proc 1] -helptext [mc "Accept element"]
        }
    
        apply {
          $self add -image [::Bitmap::get apply] -relief link \
                -text [mc "Apply"] -compound left \
                -command [list $self _Proc] -helptext [mc "Apply"]
          set notapply 0
        }
        add {
          $self add -image [::Bitmap::get add] -relief link \
                -text [mc "Add"] -compound left \
                -command [list $self _Enter] -helptext [mc "Add element"]
          set notapply 0
        }

        addall {
          $self add -image [::Bitmap::get addall] -relief link \
                -text [mc "Add all"] -compound left \
                -command [list $self _Proc 0 1] -helptext [mc "Add all elements"]
          set notapply 0
        }

        exit {
          $self add -image [::Bitmap::get exit] -relief link \
            -text [mc "Exit"] -compound left \
            -command [list $self _CloseDialog $self] -helptext [mc "Exit dialog"]
        }
        
        cancel {
          $self add -image [::Bitmap::get cancel] -relief link \
              -text [mc "Cancel"] -compound left \
              -command [list $self _CloseDialog $self] -helptext [mc "Cancel"]
        }
      }
    }
    
    if { "" eq $list } {
      for { set i 0 } { $i < [llength $options(-fields)] } { incr i } {
        bind $dialogframe.e$i <Key-Return> "$self _Focus; $self _Proc $notapply"
      }
    }
    
    # packing
    set i 0
    if { "" eq $list } {
      foreach col $fields {
        if { $options(-orientation) eq "horizontal" } {
          grid $labels($i)  -row 0 -column [expr $i * 2]    -padx 4
          grid $entries($i) -row 0 -column [expr $i * 2 +1] -padx 4
        } else {
          grid $labels($i)  -row $i -column 0 -pady 4 -sticky w
          grid $entries($i) -row $i -column 1 -pady 4 -sticky we
          grid columnconfigure $dialogframe 0 -weight 0
          grid columnconfigure $dialogframe 1 -weight 1
        }
        incr i
      }
    } else  {
      if { $options(-orientation) eq "horizontal" } {
        grid $labels($i) -row 0 -column [expr $i * 2]     -padx 4
        grid $combos -row 0 -column [expr $i * 2 + 1] -padx 4
      } else {
        grid $labels($i) -row $i -column 0 -sticky w 
        grid $combos -row $i -column 1 -sticky we
        grid columnconfigure $dialogframe 0 -weight 0
        grid columnconfigure $dialogframe 1 -weight 1
      }
    }
    
    #The selected field is proccesed and the window is not closed
    bind $self <Return> [list $self _Enter]
    bind $self <Escape> [list $self _CloseDialog $self]
  }
  
  #///////////////////////////////////////////////////////////////////////////
  #
  # PURPOSE: Metodos cget y configure
  #
  #///////////////////////////////////////////////////////////////////////////
  
  onconfigure -width { w } {
    if { $created } {
      for { set i 0 } { $i < [llength $options(-fields)] } { incr i  } {
        if { $options(-list) eq "" } {
          $entries($i) configure -width $w
        } else {
          $combos  configure -width $w
        }
      }
      set options(-width) $w
    }
  }

  onconfigure -initialvalue { values } {
    if { $created } {
      for { set i 0 } { $i < [llength $options(-fields)] } { incr i } {
        if { $options(-list) eq "" } {
          $entries($i) configure -text [lindex $values $i]
        }
      }
      set options(-initialvalue) $values
    }
  }

  onconfigure -list { lists } {
    if { $created } {
      for { set i 0 } { $i < [llength $options(-fields)] } { incr i } {
        $combos configure -values [lindex $lists $i]
      }
      set options(-list) $lists
    }
  }
  
  onconfigure -titles { lists } {
    if { $lists eq {} } {
      set options(-titles) [from args -fields {" "}]
    } else {
      set options(-titles) $lists
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method _Info { } {
  #
  # PURPOSE: shows an information message in a dialog
  #
  #///////////////////////////////////////////////////////////////////////////
    destroy $self.infodialog
    
    Dialog $self.infodialog -modal local -title [mc "Filter Information"]

    label $self.infodialog.l -justify left -text $options(-info)
    pack $self.infodialog.l

     $self.infodialog add -image [::Bitmap::get accept] -relief link \
               -text [mc "OK"] -compound left \
               -command [list $self.infodialog  enddialog ""]

    $self.infodialog draw
  }


  #///////////////////////////////////////////////////////////////////////////
  method _Enter { } {
  #
  # PURPOSE: Check content of entrykey and executes _Proc
  #
  #///////////////////////////////////////////////////////////////////////////
    if { $options(-list) eq "" } {
      $self _Proc
    } else {
      #the text in the entry key
      set text    [$combos key get]
      #All the possible outputs
      set indexes [$combos get]
      
      foreach index $indexes {
        if { [lindex [$combos get $index] 0] == $text } {
          #If the output matches with the written text, is selected and inserted
          $combos selectIndex $index
          $self _Proc
          break
        }
      }
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method _Proc { {close 0} {all 0} } {
  #
  # PURPOSE: processes the fields of the widget via an external process
  # PARAMETERS:
  #  close -> Sets if the window is closed after execution or not
  #  all -> Sets if the process will be applied to all elements from combobox
  #         or not
  #
  #///////////////////////////////////////////////////////////////////////////
    set elements {}
    if { $options(-list) eq "" } {
      for { set i 0 } { $i < [llength $options(-fields)] } { incr i } {
        lappend elements [$entries($i) cget -text]
        $entries($i) configure -text ""
      }
      $options(-procedure) [list $elements] [winfo parent $self]
    } else  {
      if { $all } {
        lappend elements [$combos get [$combos get]]
        set elements [lindex $elements 0]
        
      } else {
        lappend elements [$combos get [$combos curselection]]
      }
      $options(-procedure) $elements [winfo parent $self]
    }

    if { $close } { $self _CloseDialog $self }
  }


  #///////////////////////////////////////////////////////////////////////////
  method _CloseDialog { w } {
  #
  # PURPOSE: closes de dialog
  #
  # PARAMETERS:
  #  w -> The dialog's window pathname
  #
  #///////////////////////////////////////////////////////////////////////////
    set f [$w getframe]
    set dialogEntry ""
    $w enddialog ""
  }


  #///////////////////////////////////////////////////////////////////////////
  method _Focus { } {
  #
  # PURPOSE: focuses the first entry or combobox
  #
  #///////////////////////////////////////////////////////////////////////////
    if { "" eq $options(-list) } {
      focus $entries(0)
    } else  {
      focus $combos
    }
  }
    

  #///////////////////////////////////////////////////////////////////////////
  method draw {  } {
  #
  # PURPOSE: draws the dialog and gives focus to entry/combobox
  #
  #///////////////////////////////////////////////////////////////////////////
    $self _Focus
    $hull draw
  }

  delegate method * to hull
  delegate option * to hull
}

#TEST CODE

proc __MiProc { elems w } {
  $w configure -text $elems
}

proc test_bfieldsdialog {} {

  destroy .top
  toplevel .top
  variable horizontal 1
  variable combos 0

   radiobutton .top.rb1 -text "Horizontal layout" -value 1 -variable horizontal
   radiobutton .top.rb2 -text "Vertical layout"  -value 0 -variable horizontal
   checkbutton .top.cb -text "With combos" -command togglecombos
   .top.cb deselect
   button .top.b -text "Launch a Fields Dialog" -command "__PreLaunchDialog"
   LabelEntry .top.le -label "Fields processed..."


   grid .top.rb1 -row 0 -column 0 -sticky nsw
   grid .top.rb2 -row 1 -column 0 -sticky nsw
   grid .top.cb -row 0 -column 1
   grid .top.b -row 1 -column 1
   grid .top.le -row 2 -column 0 -columnspan 2 -sticky "ew"
   
   proc togglecombos {} {
     variable combos
     set combos [expr ! $combos]
   }

   proc __PreLaunchDialog {} {
     variable combos
     variable horizontal
     __LaunchDialog $horizontal $combos .top.le
   }

  proc __LaunchDialog { hor combos father } {
    set orientation vertical
    if { $hor } { set orientation horizontal }

    if { $combos } {
      bfieldsdialog ${father}.d -title "Prueba Dialog with Combos" -width 20 \
                             -fields { "campo1" "campo2" "campo3" } \
                             -titles { "Titulo1" "Titulo2" "Titulo3" } \
                             -initialvalue {uno 2 iii} \
                             -orientation $orientation -procedure __MiProc \
                             -buttons {info apply accept cancel} \
                             -list {{uno} {1 2} {i ii iii}} \
                             -info \
"Los valores iniciales suministrados se
buscan en los combos y son seleccionados"
    } else {
      bfieldsdialog ${father}.d -title "Prueba Dialog with Entries" \
                             -width 20 -fields { "campo1" "campo2" "campo3" } \
                             -titles { "Titulo1" "Titulo2" "Titulo3" } \
                             -initialvalue {k kk kkk} -orientation $orientation \
                             -buttons {add exit} -procedure __MiProc
    }
    
    ${father}.d draw
    destroy ${father}.d
  }
}

package provide bfieldsdialog 1.0
