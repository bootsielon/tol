#/////////////////////////////////////////////////////////////////////////////
# FILE    : bflistbox.tcl
# PURPOSE : This file implements functions to a component listbox
#/////////////////////////////////////////////////////////////////////////////

#/////////////////////////////////////////////////////////////////////////////
proc lbDeleteSelection {lb_} {
# PURPOSE: Delete selected elements
#
# PARAMETERS:
#   lb_ -> List of elements
#
#/////////////////////////////////////////////////////////////////////////////
  set lst [$lb_ curselection]
  set lst [lreverse $lst]
  foreach it $lst {
    $lb_ delete $it
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc lbToggleSelection {lb_} {
# PURPOSE: Toggle selection elements
#
# PARAMETERS:
#   lb_ -> List of elements
#
#/////////////////////////////////////////////////////////////////////////////
  # controlamos que no se permita seleccion multiple
  set sm [$lb_ cget -selectmode]
  if {[string equal $sm multiple] || [string equal $sm extended]} {
    set sz [$lb_ size]
    for {set f 0} {$f < $sz} {incr f} {
      if {[$lb_ selection includes $f]} {
        $lb_ selection clear $f
      } else {
        $lb_ selection set   $f
      }
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc lbToClipBoard {lb_ sl unSe {pref ""} {suf ""} {sep "\n"}} {
# PURPOSE: copy the elements indicates in clipboard
#
# PARAMETERS:
#   lb_  -> List of elements
#   sl   -> Copy items selections
#   unSe -> Copy items not selections
#   pref -> Text to concatenate in the beginning. Default ""
#   suf  -> Text to concatenate in the end. Default ""
#   sep  -> Separating text of elements. Default "\n" 
#
#/////////////////////////////////////////////////////////////////////////////
  # items to copy
  set lst [lbGetItems $lb_ $sl $unSe]
  # copy
  set str [ListToStr $lst $sep $pref $suf]
  clipboard clear
  clipboard append $str
}

#/////////////////////////////////////////////////////////////////////////////
proc lbGetItems {lb_ se unSe} {
# PURPOSE: Obtain one lists with the selected and/or not selected elements
#
# PARAMETERS:
#   lb_  -> List of elements
#   sl   -> Items selections
#   unSe -> Items not selections
#
# RETURN: List with the elements
#
#/////////////////////////////////////////////////////////////////////////////  
  set lst {}
  if {$se || $unSe} {
    if {$se && $unSe} {
      # all
      set lst [$lb_ get 0 end]
    } elseif {$se} {
      # selections
      set ls [$lb_ curselection]
      foreach it $ls {
        lappend lst [$lb_ get $it]
      }
    } elseif {$unSe} {
      # not selections
      set sz [$lb_ size]
      for {set f 0} {$f < $sz} {incr f} {
        if {![$lb_ selection includes $f]} {
          lappend lst [$lb_ get $f]
        }
      }
    }
  }
  return $lst
}

#/////////////////////////////////////////////////////////////////////////////
proc lbCountItems {lb_} {
# PURPOSE: Information about the elemetos of one lists 
#
# PARAMETERS:
#   lb_  -> List of elements
#
# RETURN: One list. 1º Total, 2º selections, 3º not selections
#
#/////////////////////////////////////////////////////////////////////////////
  set lon     [$lb_ size]
  set lSel    [llength [$lb_ curselection]]
  set lNotSel [expr $lon - $lSel]
  return [list $lon $lSel $lNotSel]
}

#/////////////////////////////////////////////////////////////////////////////
proc lbLoadFromFile {lb_} {
# PURPOSE: Load the content from listbox in a file
#
# PARAMETERS:
#   lb_  -> List of elements
#
#/////////////////////////////////////////////////////////////////////////////
  # select file
  set filNam [tk_getOpenFile -parent $lb_ -title [mc "File selector"]]
  if {$filNam != ""} {
    # open file to read
    if { ![ catch { set fd [open $filNam r] } msgerr] } {
      while { [gets $fd line ] >= 0} { 
        $lb_ insert end $line
      } 
      close $fd      
      return 1
    }
    error $msgerr
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc lbSaveToFile {lb_ se unSe {pref ""} {suf ""} {sep "\n"}} {
# PURPOSE: Save the content from listbox in a file
#
# PARAMETERS: 
#   lb_  -> List of elements
#   sl   -> Copy items selections
#   unSe -> Copy items not selections
#   pref -> Text to concatenate in the beginning. Default ""
#   suf  -> Text to concatenate in the end. Default ""
#   sep  -> Separating text of elements. Default "\n" 
#
#/////////////////////////////////////////////////////////////////////////////
  # select file
  set filNam [tk_getSaveFile -parent $lb_ -title [mc "File selector"]]
  if {$filNam != ""} {
     # open file to write
    if { ![ catch { set fd [open $filNam w] } msgerr] } {
      # text to save
      set lst [lbGetItems $lb_ $se $unSe]
      set str [ListToStr $lst $sep $pref $suf]
      # write
      puts -nonewline $fd $str
      close $fd
      return 1
    }
    error $msgerr
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc lbSelectAll {lb_} {
# PURPOSE: Selection all
#
# PARAMETERS: 
#   lb_  -> List of elements
#
#/////////////////////////////////////////////////////////////////////////////
  # controlamos que no se permita seleccion multiple
  set sm [$lb_ cget -selectmode]
  if {[string equal $sm multiple] || [string equal $sm extended]} {
    $lb_ selection set 0 end  
  }
}

#///////////////////////////////////////////////////////////////////////////
proc lbOnKeyPress {lb key lstkey aftertime diference {col ""}} {
#
# PURPOSE: Activa la fila que coincide con las teclas pulsadas. Distinge
#          entre muysuculas y minusculas
#
# PARAMETERS: 
#  lb -> path del lisbox 
#  key -> tecla pulsada
#  lstkey -> lista de caracteres pulsados anteriormente
#  aftertime -> momento de la ultima pulsacion
#  diference -> tiempo maxiom entre pulsaciones
#  col -> columna por la que realizar el filtrado (solo blistbox+)
#
# RETURN: Una lista con los valores actualizados para la lista de caracteres
#         y el momento de la ultma pulsacion
#
# TODO: opciones de componente para un futuro
#   - case sensitive
#
#///////////////////////////////////////////////////////////////////////////
  if {[expr [string length $key] == 1]} {
    # time control
    set now_press_key [clock clicks -milliseconds]
    set dif [expr $now_press_key - $aftertime]
    if {$dif <= $diference} {
      set key $lstkey$key
    }
    set aftertime $now_press_key
    # busqueda en minusculas
    set lstkey    [string tolower $key]
    # FIND
    # widget ?
    switch [winfo class $lb] {
      Listbox -
      Blistbox {        
        for {set row 0} {$row < [$lb index end]} {incr row} {
          set r [lsearch [string tolower [$lb get $row]] ${lstkey}*]
          if {[expr $r > -1]} {
            $lb selection clear 0 end
            $lb selection set $r
            $lb activate $r
            $lb see      $r            
            break
          }            
        }    
      }
      Treeview -
      Btreeview -
      Btreeviewplus -
      Blistboxplus {
        foreach id [$lb get] {
          set item [string tolower [$lb get $id $col]]
          if { "-1" ne [lsearch $item ${lstkey}*] } {
            $lb selection clearall
            $lb selection set $id
            $lb see   $id
            $lb focus $id
            break
          }
        }
      }
    }
  }
  update
  return [list $lstkey $aftertime]
}
