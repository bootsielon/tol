#/////////////////////////////////////////////////////////////////////////////
# FILE    : bmultiselec.tcl
# PURPOSE : Component for creating a multiple selector of items and 
#           managing its events. A multiple selector consist of two listboxes:
#           unselected (left) and selected (right) with items which you can 
#           move from one listbox to the other.
#/////////////////////////////////////////////////////////////////////////////

package require byscommon
package require blistbox
package require blistboxplus


#/////////////////////////////////////////////////////////////////////////////
::snit::widget bmultiselect {
#
# PURPOSE: Selector de elementos multiple
#
# opciones especificas
#  unsel -> valores no seleccionados
#  sel -> valores seleccionados
#  labelsel -> etiqueta para la lista de elementos seleccionados
#  labelunsel -> etiqueta para la lista de elementos no seleccionados
#  font -> font for the both listboxes
#  onchanged -> comando a ejecutar cada vez que cambia algo en las listas
#  unique -> si esta a true solamente se permite pasar un elemento a la lista
#            de seleccionados
#  cols -> lista con las columnas de cada ListBox
#  hiddencols -> aquellas columnas del ListBox que queremos mantener ocultas
#  width -> ancho total orientativo del widget
#  height -> alto total orientativo del widget
#  initialdir -> directory where to go when opening a save dialog
#
# variables
#  lright -> the right label
#  lleft -> the left label
#  right -> the right blistbox or blistcontrol
#  left -> the left blistbox or blistcontrol
#  bbox -> the buttonbox
#  plus -> indicates if the listbox is listbox+ (columns)
#  created -> indicates if the widget has been fully created
#  
#/////////////////////////////////////////////////////////////////////////////
  widgetclass Bmultiselect

  option -sel {}
  option -unsel {}
  option -labelsel   "Selected"
  option -labelunsel "Not Selected"
  option -font {arial 10}
  option -onchanged ""
  option -unique false
  option -width {}
  option -height {}
  option -cols {}
  option -hiddencols {}
  option -initialdir ""
  
  variable lright
  variable lleft
  variable right
  variable left
  variable bbox
  variable bboxright

  variable created 0
  variable plus 0

  #///////////////////////////////////////////////////////////////////////////
  constructor { args } {
  #
  # PURPOSE: Constructor de un bmultiselect. Crea una instancia de un selector
  #          multiple
  #
  # PARAMETERS:
  #   args -> Lista de opciones para el bmultiselect
  #
  #///////////////////////////////////////////////////////////////////////////
    $self configure -cols [from args -cols ""]
    $self configure -sel [from args -sel ""]
    $self configure -unsel [from args -unsel ""]
    $self configure -unique [from args -unique 0]
    
    # Create the widget
    $self createwidget $args

    # Apply all arguments given:
    $self configurelist $args
  }


  #///////////////////////////////////////////////////////////////////////////
  #
  # PURPOSE: Metodos cget y configure
  #
  #///////////////////////////////////////////////////////////////////////////

  onconfigure -sel { lst } {
    set options(-sel) $lst
    if { $created } {
      $self _FillLB
    }
  }
  
  onconfigure -unsel { lst } {
    set options(-unsel) $lst
    if { $created } {
      $self _FillLB
    }
  }
  
  onconfigure -labelsel { value } {
    if { $created } {
      set options(-labelsel) $value
      $lright configure -text $value
    }
  }
  
  onconfigure -labelunsel { value } {
    if { $created } {
	  set options(-labelunsel) $value
      $lleft configure -text $value
    }
  }
  
  onconfigure -cols { value } {
    set options(-cols) $value
    set plus [llength $options(-cols)]
    if { $created && $plus } {
      $right configure -cols $value
      $left configure -cols $value
    }
  }

  onconfigure -hiddencols { value } {
    set options(-hiddencols) $value
    set plus [llength $options(-cols)]
    if { $created && $plus } {
      $right configure -hiddencols $value
      $left configure -hiddencols $value
    }
  }

  onconfigure -width { value } {
    set options(-width) $value
    if { ! $plus } {
      set value [expr $value / 2]
    }
    if { $created } {
      $left configure -width $value
      $right configure -width $value
    }
  }

  onconfigure -height { value } {
    set options(-height) $value
    if { ! $plus } {
      set value [expr $value / 2]
    }
    if { $created } {
      $left configure -height $value
      $right configure -height $value
    }
  }

   onconfigure -initialdir { value } {
     set options(-initialdir) $value
     if { $created && $plus } {
       $left configure -initialdir $options(-initialdir)
       $right configure -initialdir $options(-initialdir)
     }
   }

   onconfigure -font { f } {
     set options(-font) $f
     if { $created } {
       $left configure -font $f
       $right configure -font $f
     }
   }

  
  #///////////////////////////////////////////////////////////////////////////
  method createwidget { args } {
  #
  # PURPOSE: Creacion de los objetos que componen el widget
  #
  # PARAMETERS:
  #   args -> Lista de opciones para el bmultiselect
  #
  #///////////////////////////////////////////////////////////////////////////
    set labelunsel [from args -labelunsel [mc "Not Selected"]]
    set labelsel [from args -labelsel [mc "Selected"]]
    set cols [from args -cols ""]
    set initialdir [from args -initialdir ""]

    # listbox 1 
    install lleft using label $self.l1 -text $labelunsel -bd 1 -relief raised
    if { ! $plus } {
      ScrolledWindow $self.sw1
      install left using blistbox $self.sw1.lb -selectmode extended \
        -breadonly true -bkeypress true -bparent $self
      $self.sw1 setwidget $self.sw1.lb
      bind $left <Button-1> +[list focus -force %W]
      bind $left <Double-Button-1> +[list $self _LBLeftToRight]
    } else {
       install left using blistboxplus $self.lbl -cols $options(-cols) \
         -hiddencols $options(-hiddencols) -initialdir $initialdir -readonly true
       if { $options(-unique) } { 
         $left configure -selectmode single
       } else {
         $left configure -selectmode multiple
       }
       #@ anteriormente era una llamada a un método bind
       bind $left <Double-Button-1> +[list $self _LBLeftToRight]
    }

    # buttons
    install bbox using ButtonBox $self.bb -orient vertical -spacing 0 \
                                          -padx 4 -pady 1
    $bbox add -image [Bitmap::get play] -height 16 -width 16 \
      -command [list $self _LBLeftToRight] -relief link \
      -helptext [mc "Moves selection from left to right"]
    $bbox add -image [Bitmap::get reverse] -height 16 -width 16 \
      -command [list $self _LBRightToLeft] -relief link \
      -helptext [mc "Moves selection from right to left"]
	if { ! $options(-unique) } {
    $bbox add -image [Bitmap::get end] -height 16 -width 16 \
      -command [list $self _LBAllLeftToRight] -relief link \
      -helptext [mc "Moves all from left to right"]
    $bbox add -image [Bitmap::get begin] -height 16 -width 16 \
      -command [list $self _LBAllRightToLeft] -relief link \
      -helptext [mc "Moves all from right to left"]
    }


    install bboxright using ButtonBox $self.bbr -orient vertical -spacing 0 \
                                          -padx 4 -pady 1
    $bboxright add -image [Bitmap::get upNext] -height 16 -width 16 \
      -command [list $self _LBDownToUp] -relief link \
      -helptext [mc "Moves selection up"]
    $bboxright add -image [Bitmap::get downNext] -height 16 -width 16 \
      -command [list $self _LBUpToDown] -relief link \
      -helptext [mc "Moves selection down"]
 
 
    # listbox 2
    install lright using label $self.l2 -text $labelsel -bd 1 -relief raised 
    if { ! $plus  } {
      ScrolledWindow $self.sw2
      install right using blistbox $self.sw2.lb -selectmode extended \
        -breadonly true -bkeypress true -bparent $self
      $self.sw2 setwidget $self.sw2.lb
      bind $right <Button-1> +[list focus -force %W]
      bind $right <Double-Button-1> +[list $self _LBRightToLeft]
    } else {
       install right using blistboxplus $self.lbr -cols $options(-cols) \
                                   -initialdir $initialdir -readonly true
       if { $options(-unique) } { 
         $right configure -selectmode single
       } else {
         $right configure -selectmode multiple
       }
       #@ anteriormente era una llamada a un método bind
       bind $right <Double-Button-1> +[list $self _LBRightToLeft]
    }
  
    # fill the listbox
    $self _FillLB

    grid $self.l1 $bbox $self.l2 $bboxright -sticky news
    if { ! $plus } {
      grid $self.sw1  ^      $self.sw2 ^ -sticky news
    } else {
	  grid $left  ^      $right ^ -sticky news
    }
    grid rowconfigure    $self 1 -weight 1
    grid columnconfigure $self 0 -weight 1 
    grid columnconfigure $self 1 -pad 5
    grid columnconfigure $self 2 -weight 1
    grid columnconfigure $self 3 -pad 5
    update
    bind <Destroy> $self [list $self KillData]
    
    set created 1
    
    return $self
  }
  

  #///////////////////////////////////////////////////////////////////////////
    method _EvalOnChanged {} {
  #
  # PURPOSE: Evaluates a procedure when any of the listboxes changes any of
  #          its items and actualizes the -sel and -unsel options.
  #
  #///////////////////////////////////////////////////////////////////////////
    
    set options(-unsel) [$left get 0 end]
    set options(-sel) [$right get 0 end]
	
    if { [string length $options(-onchanged)] } {
      eval $options(-onchanged)
    }
  }


  #///////////////////////////////////////////////////////////////////////////
    method _FillLB {} {
  #
  # PURPOSE: Fill unselected and selected listboxes.
  #
  #///////////////////////////////////////////////////////////////////////////

    if { "" eq $options(-cols) } {
      $left delete 0 end
      $right delete 0 end
    } else {
      $left clear
      $right clear
    }
    
    foreach it $options(-unsel) {
      $left insert end $it
    }
    foreach it $options(-sel) {
      
      $right insert end $it
      if { $options(-unique) } { break }
    }
    $self _EvalOnChanged
  }


  #///////////////////////////////////////////////////////////////////////////
    method _ListTransferSel { src dst } {
  #
  # PURPOSE: Transfer selected items from scr listbox to dst listbox.
  # 
  # PARAMETERS: 
  #    src -> source listbox path
  #    dst -> destination listbox path
  #
  #///////////////////////////////////////////////////////////////////////////

    if { "$dst" eq "$right" && $options(-unique) } {
      if { [$right size] eq 0 } {
        if { "" ne [$src curselection] } {
          switch [llength $options(-cols)] {
            1 {
              eval $dst insert end [$src get [lindex [$src curselection] 0]]
            }
            default {
              $dst insert end [$src get [lindex [$src curselection] 0]]
            }
	  }
        }
      } else {
        if { $plus } {
          $src selection clearall
        } else {
          $src select clear 0 end
        }
      }
    } else {
      foreach i [$src curselection] {
        switch [llength $options(-cols)] {
          1       { eval $dst insert end [$src get $i] }
          default { $dst insert end [$src get $i]      }
        }
      }
    }
  }


  #///////////////////////////////////////////////////////////////////////////
    method _ListDeleteSel { w } {
  #
  # PURPOSE: Deletes selected items of a listbox
  # 
  # PARAMETERS:
  #    w -> listbox's path
  #
  #///////////////////////////////////////////////////////////////////////////
  
    if { "$w" eq "$left" && $options(-unique) } {
	  set index [lindex [$w curselection] 0]
	  if { $index >= 0 } { $w delete $index }
    } else {
      foreach i [lsort -integer -decreasing [$w curselection]] {
        $w delete $i
      }
    }
  }


  #///////////////////////////////////////////////////////////////////////////
    method _LBLeftToRight {} {
  #
  # PURPOSE: Moves selected items from unselected listbox to selected listbox
  #
  #///////////////////////////////////////////////////////////////////////////
    $self _ListTransferSel $left $right
    $self _ListDeleteSel   $left
    $self _EvalOnChanged
  }


  #///////////////////////////////////////////////////////////////////////////
    method _LBRightToLeft {} {
  #
  # PURPOSE: Moves selected items from selectedu listbox to unselected listbox
  #
  #///////////////////////////////////////////////////////////////////////////
    $self _ListTransferSel $right $left
    $self _ListDeleteSel   $right
    $self _EvalOnChanged
  }


  #///////////////////////////////////////////////////////////////////////////
    method _LBAllLeftToRight {} {
  #
  # PURPOSE: Moves all items from unselected listbox to selected listbox
  #
  #///////////////////////////////////////////////////////////////////////////
    if { $plus } {
      foreach i [$left get] {
        switch [llength $options(-cols)] {
          1       { eval $right add [$left get $i] }
          default {      $right add [$left get $i] }
        }
	$left delete $i
      }
    } else {
      eval $right insert end [$left get 0 end]
      $left delete 0 end
    }
    $self _EvalOnChanged
  }


  #///////////////////////////////////////////////////////////////////////////
    method _LBAllRightToLeft {} {
  # 
  # PURPOSE: Moves all items from selected listbox to unselected listbox
  #
  #///////////////////////////////////////////////////////////////////////////
    if { $plus } {
      foreach i [$right get] {
        switch [llength $options(-cols)] {
          1       { eval $left add [$right get $i] }
          default {      $left add [$right get $i] }
        }
	$right delete $i
      }
    } else {
      eval $left insert end [$right get 0 end]
      $right delete 0 end
    }
    $self _EvalOnChanged
  }


  #///////////////////////////////////////////////////////////////////////////
    method _LBDownToUp {} {
  # 
  # PURPOSE: Moves selected items in listbox on the right side up
  #
  #///////////////////////////////////////////////////////////////////////////
    set indexes [$right curselection]
    if {$indexes != ""} {
      set first [lindex $indexes 0]
      set last [lindex $indexes end]      
      if { $plus } {
        # hacia arriba
        if {$first eq [$right item firstchild root]} return
        foreach index $indexes {
          $right move $index before {}
        }        
      } else {
        if { $first != 0 } {
          set previous [$right get [expr {$first-1}]]
          $right delete [expr {$first-1}]
          $right insert $last $previous
        }
      }
    }
  }


  #///////////////////////////////////////////////////////////////////////////
    method _LBUpToDown {} {
  # 
  # PURPOSE: Moves selected items in listbox on the right side down
  #
  #///////////////////////////////////////////////////////////////////////////
    set indexes [$right curselection]
    if {$indexes != ""} {
      set first [lindex $indexes 0]
      set last [lindex $indexes end]
      if { $plus } {
        # hacia abajo
        if {$last eq [$right item lastchild root]} return
        foreach index [lreverse $indexes] {
          $right move $index after {}
        }
      } else {
        set end [$right index end]
        if { $last != [expr {$end-1}] } {
          set next [$right get [expr {$last+1}]]
          $right delete [expr {$last+1}]
          $right insert $first $next
        }
      }
    }
  }

  
  #///////////////////////////////////////////////////////////////////////////
    method get { {sel "selected"} } {
  # 
  # PURPOSE: gets items selected or unselected
  #
  # PARAMETERS:
  #  sel: selected or unselected, indicates which ones to return
  #
  #///////////////////////////////////////////////////////////////////////////
    if { $sel eq "selected" } {
      if { $plus } {
        set result {}
        foreach i [$right get] {
          switch [llength $options(-cols)] {
            1       { lappend result [lindex [$right get $i] 0] }
	    default { lappend result [$right get $i] }
          }
	}
	return $result
      } else {
        return [$right get 0 end]
      }
    }
    
    if { $sel eq "unselected" } {
      if { $plus } {
        set result {}
        foreach i [$left get] {
          switch [llength $options(-cols)] {          
	    1       { lappend result [lindex [$left get $i] 0] }
            default { lappend result [$left get $i] }
          }
	}
	return $result
      } else {
        return [$left get 0 end]
      }
    }
  }

  #///////////////////////////////////////////////////////////////////////////
    method GetSelected {} {
  # 
  # PURPOSE: Gets item from selected listbox (rigth)
  #
  #///////////////////////////////////////////////////////////////////////////
    puts "This function is deprecated use get selected instead"
    $self get selected
  }


  #///////////////////////////////////////////////////////////////////////////
    method GetUnselected {} {
  # 
  # PURPOSE: Gets item from unselected listbox (left)
  #
  #///////////////////////////////////////////////////////////////////////////
    puts "This function is deprecated use get unselected instead"
    $self get unselected
  }
}


# TEST CODE


#/////////////////////////////////////////////////////////////////////////////
proc closeBmultiselectDialog { w } {
#
# PURPOSE: closes the dialog and returns the result
#
# PARAMETERS:
#  w -> The dialog's window pathname
#
#/////////////////////////////////////////////////////////////////////////////
  set f [$w getframe]
  set result [$f.bms get selected]
  $w enddialog $result
}


#/////////////////////////////////////////////////////////////////////////////
proc launchDialog { unique multicolumn le } {
#
# PURPOSE: Launches a Dialog with a bmultiselect widget
#
# PARAMETERS:
#  w -> The dialog's window pathname
#  unique -> This option tells the bmultiselect widget if it will admit only
#            one selected item
#
#/////////////////////////////////////////////////////////////////////////////

  # example of how to use in a dialog
  destroy .top.d
  set dialog [Dialog .top.d -title "Prueba de BMultiSelect" \
    -modal local -parent .top]
  set dialogframe [$dialog getframe]

  if { $multicolumn } {
    bmultiselect $dialogframe.bms -cols {una dos tres} -unique $unique \
	  -unsel { {un dos tres} {i ii iii} {x y z} {@ # &} } \
	  -sel   { {one two three} {a b c} {k kk kkk} } \
	  -labelunsel "no seleccionados" -labelsel "seleccionados" \
	  -height 200
  } else {
    bmultiselect $dialogframe.bms -unsel {uno 2 tres 4} -sel {cinco 5 siete} \
      -labelunsel "no seleccionados" -labelsel "seleccionados" -unique $unique
  }
  
  set dialogframe_bbox \
          [ButtonBox $dialogframe.bb -orient horizontal -spacing 10 -padx 0 -pady 0 \
          -homogeneous false]
  $dialogframe_bbox add -text [mc "Accept"] -relief link \
          -helptext [mc "Accept selection"] \
          -image [::Bitmap::get accept] -compound left -padx 5 -pady 1 \
          -command [list closeBmultiselectDialog $dialog]
  $dialogframe_bbox add -text [mc "Cancel"] -relief link \
          -helptext [mc "Cancel selection"]\
          -image [::Bitmap::get cancelx] -compound left -padx 5 -pady 1 \
          -command [list $dialog enddialog ""]    
  
  pack $dialogframe_bbox -fill x -expand false -side bottom
  pack $dialogframe.bms -fill both -expand true
  
  set result [$dialog draw]
  $le configure -text $result
}


#/////////////////////////////////////////////////////////////////////////////
proc test_bmultiselect {} {
#
# PURPOSE: Test function of a bmultiselect
#
#/////////////////////////////////////////////////////////////////////////////
  destroy .top
  toplevel .top
  variable multcols 0
  variable uniq 0

  radiobutton .top.rb1 -text "Multiple columns" -value 1 -variable multcols
  radiobutton .top.rb2 -text "Only one column"  -value 0 -variable multcols
  checkbutton .top.cb -text "Only one selected" -command toggleuniq
  .top.cb deselect
  button .top.b -text "Launch a MultiSelect Dialog" -command "preLaunchDialog"
  LabelEntry .top.le -label Result


  grid .top.rb1 -row 0 -column 0
  grid .top.rb2 -row 1 -column 0
  grid .top.cb -row 0 -column 1
  grid .top.b -row 1 -column 1
  grid .top.le -row 2 -column 0 -columnspan 2 -sticky "ew"

  proc toggleunique {} {
    variable uniq
    set uniq [expr ! $uniq]
  }
  
  proc preLaunchDialog {} {
    variable uniq
	variable multcols
    launchDialog $uniq $multcols .top.le
  }

}

package provide bmultiselect 1.0
