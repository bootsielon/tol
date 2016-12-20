#/////////////////////////////////////////////////////////////////////////////
# FILE    : bystext.tol
# PURPOSE : 
#/////////////////////////////////////////////////////////////////////////////

#/////////////////////////////////////////////////////////////////////////////
# TESTING
#/////////////////////////////////////////////////////////////////////////////
# Put if {1} for testing this file.

package require snit

if {0} {
  package require Tk
  package require BWidget
  set toltk_script_path [file dirname [info script]]
  package require msgcat 
  namespace import ::msgcat::*
  ::msgcat::mclocale es
  ::msgcat::mcload [file join $toltk_script_path msgs]
 
}

::snit::widgetadaptor rotext {

  constructor {args} {
    # Turn off the insert cursor
    #installhull using text $self -insertwidth 0
    # DDG the $self gaves an error at least with 0.97 onwards
    installhull using text -insertwidth 0
    
    # Apply an options passed at creation time.
    $self configurelist $args
  }
  
  # Disable the insert and delete methods, to make this readonly.
  method insert {args} {}
  method delete {args} {}
  
  # Enable ins and del as synonyms, so the program can insert and
  # delete.
  delegate method ins to hull as insert
  delegate method del to hull as delete
  
  # Pass all other methods and options to the real text widget, so
  # that the remaining behavior is as expected.
  delegate method * to hull
  delegate option * to hull
}

#/////////////////////////////////////////////////////////////////////////////
# NAMESPACE
#/////////////////////////////////////////////////////////////////////////////

namespace eval ::BayesText {
  variable options
}


#/////////////////////////////////////////////////////////////////////////////
# PROCEDURES
#/////////////////////////////////////////////////////////////////////////////

#/////////////////////////////////////////////////////////////////////////////
   proc ::BayesText::FindShow {editor {next 0} {txt ""} {regexp 0}} {

#
# PURPOSE: This proc mades bla, bla, bla,
#          and bla, bla, bla, ...
#
# ALGORITHM:
#   Xxx xxxx xx:
#     Xxxxx xxx xxx.
#     Xxxxx xxx xxx.
#   Xxxxx.
#
# PARAMETERS:
#   editor -> text widget where text has to be found
#   next   ->
#   txt    ->
#
# RETURN:
#
#/////////////////////////////////////////////////////////////////////////////
  variable options
  
  ::BayesText::ReadIni
  
  set Instance ::BayesText::Find$editor
  if { [lsearch [namespace children ::BayesText] $Instance] != -1 } {
      
  } else  {
    namespace eval $Instance {
      variable data
      
      #data(find,text) --> texto a buscar
      #data(find,dir)  --> dirección de búsqueda
      #data(start)     --> indica desde donde se empieza a buscar
      #data(find,case) -->
      #data(editor)    --> el path del widget text en el q buscar
      #data(path)      --> el path del toplevel que muestra el cuado de búsqueda
      #data(frm,cad)   --> entry donde se inserta el texto a buscar
    }
  }
  upvar \#0 ${Instance}::data data

  # exists data(find,temp) ?
  if {!([info exists data(find,temp)])} {
    set data(find,temp) ""
  }
  
  set data(editor) $editor
  set data(path)   ${editor}.tFind
  if [winfo exist $data(path)] {
    raise $data(path)
    focus $data(frm,cad)
  # $data(frm,cad) selection range 0 end
    $data(frm,cad).e selection range 0 end
    return
  }
  
  # inizialization values
  set data(find,dir)    $options(find,dir)
  set data(find,case)   $options(find,case)  
  set data(find,regexp) $regexp
  # text find ?
  if {[string length $txt]} {
    set data(find,text) $txt
  } else {
    set idxs [$editor tag ranges sel]
    
    # next find ?
    if { $next eq 1 } {
      set data(find,text) $data(find,temp)
    } else {
      # text selected ? 
      if {!(([llength [lindex $idxs 0]] eq 0) && \
            ([llength [lindex $idxs 1]] eq 0))} {
        set txt [$editor get [lindex $idxs 0] [lindex $idxs 1]]
        if {!($txt eq "")} {
          set txt [lindex [split $txt "\n"] 0]
          set data(find,text) $txt
        }
      }
    }
  }
  # type of find ?
  if {$next && [info exists ${Instance}::data(find,text)] && ($data(find,text) !="") } {
    FindNext $Instance
  } else {
    FindCreate $Instance
  }
  # save teh last search
  set data(find,temp) $data(find,text)
}


#/////////////////////////////////////////////////////////////////////////////
proc ::BayesText::SearchShow {instance } {
#
# PURPOSE: Shows the form of function search with the first word selected
#
# PARAMETERS:
#   editor -> text widget where text has to be found
#
#
#/////////////////////////////////////////////////////////////////////////////
  # show find dialog
  set dialog [::funcSearch::Show]
  # function find ?
  set idxs [$instance tag ranges sel]
  if {!(([llength [lindex $idxs 0]] eq 0) && \
        ([llength [lindex $idxs 1]] eq 0))} {      
    set txt [$instance get [lindex $idxs 0] [lindex $idxs 1]]
    if {!($txt eq "")} {
      set txt [lindex [split $txt " "] 0]    
      ::funcSearch::SearchWord $txt
    }
  }    
}  


#/////////////////////////////////////////////////////////////////////////////   
proc ::BayesText::FindPosText { Instance } {
#
# PURPOSE: find the next position of text
#
#/////////////////////////////////////////////////////////////////////////////     
  upvar \#0 ${Instance}::data data
  set start [$data(editor) index insert]
  if [string equal $data(find,dir) "forward"] {
    set end [$data(editor) index end]
  } else {
    set end "1.0"
  }
  set ::_len 0
  if $data(find,case) {
    if {[info exist data(find,regexp)] && $data(find,regexp)} {
      set pos [$data(editor) search -regexp -count ::_len -$data(find,dir) --\
              $data(find,text) $start $end]
    } else {
      set pos [$data(editor) search -count ::_len -$data(find,dir) --\
              $data(find,text) $start $end]
    }
  } else  {
    set pos [$data(editor) search -count ::_len -nocase -$data(find,dir) --\
            $data(find,text) $start $end]
  }
  #Tolcon_Trace "Devuelvo pos=$pos, len=$::_len"
  return [list $pos $::_len]
}


#/////////////////////////////////////////////////////////////////////////////
proc ::BayesText::FindNext { Instance } {
#
# PURPOSE: Make the next find in editor
#
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${Instance}::data data 
  #  Coger la posición actual del cursor en el text y buscar desde ahí,
  set posOrig [$data(editor) index insert]
  
  set res [FindPosText $Instance]
  set pos [lindex $res 0]
  set len [lindex $res 1]
  # comprobamos si ha encontrado algo
  if { $pos != "" } {
    if [string equal $data(find,dir) "forward"] {
        set start "$pos + $len char"
    } elseif [string equal $data(find,dir) "backward"] {
        set start "$pos - $len char"
    }
    tk::TextSetCursor $data(editor) $start
    $data(editor) see $pos
    # Intamos un tag found ya que sel parece no funcionar correctamente en
    # windows cuando el text pierde el foco
    $data(editor) tag delete found
    set fore [$data(editor) tag cget sel -foreground]
    set back [$data(editor) tag cget sel -background]
    set bord [$data(editor) tag cget sel -borderwidth]
    $data(editor) tag configure found -foreground $fore -background $back -borderwidth $bord
    $data(editor) tag add sel $pos "$pos + $len char"
    $data(editor) tag add found $pos "$pos + $len char"
  } else {
    tk::TextSetCursor $data(editor) $posOrig
    $data(editor) tag delete sel
    $data(editor) tag delete found
    if [string equal $data(find,dir) "forward"] {
      set msg "[mc SearchUpEndDoc].\n[mc FindFromBegin]"
    } else {
      set msg "[mc SearchUpBeginDoc].\n[mc FindFromEnd]"
    }
    if [winfo exists $data(path)] {
      set parent $data(path)
    } else  {
      set parent $data(editor)
    }
    set rq [tk_messageBox -type okcancel -icon info -parent $parent\
            -title [mc "Editor"] -message $msg]
    if {$rq == "ok"} {
      if [string equal $data(find,dir) "forward"] {
        tk::TextSetCursor $data(editor) "1.0"
      } else {
        tk::TextSetCursor $data(editor) [$data(editor) index end]
      }
      FindNext $Instance
    }
  }
  # Debo quedarme yo con el foco, no el editor

  #focus $data(editor)
}

#/////////////////////////////////////////////////////////////////////////////
   proc ::BayesText::FindCreate {Instance } {
#
# PURPOSE: Creates a Find Dialog given a Instance
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${Instance}::data data

  set dialog [Dialog $data(path) -title [mc "Find"]\
          -default 0 -modal none -parent $data(editor)]
  wm protocol $dialog WM_DELETE_WINDOW \
    [list ::BayesText::FindDestroy $Instance $dialog]  
  set w [$dialog getframe]
  wm resizable $data(path) 0 0
  
  set data(frm,cad) [LabelEntry  $w.eTxt -label "[mc "Find"]: " \
          -textvariable ${Instance}::data(find,text)]
  checkbutton $w.case  -text [mc "Case sensitive"] \
          -variable ${Instance}::data(find,case) -pady 0
  TitleFrame $w.dir -bd 2 -relief groove -text [mc "Direction"]
  set f [$w.dir getframe]
  radiobutton $f.forward -text [mc "Forward"] -value forward \
          -variable ${Instance}::data(find,dir)\
          -highlightthickness 0 -pady 0
  radiobutton $f.backward -text [mc "Backward"] -value backward \
          -variable ${Instance}::data(find,dir)\
          -highlightthickness 0 -pady 0
  Button $w.findNext -text [mc "Find next"] -width 16 \
          -command [list ::BayesText::FindNext $Instance] \
          -helptext [mc "Find next"]
  Button $w.cancelar -text [mc "Cancel"] -width 16 -helptext "" \
    -command [list ::BayesText::FindDestroy $Instance $dialog]
  
  bind $dialog <Return> [list ::BayesText::FindNext $Instance]
  bind $dialog <F3>     [list ::BayesText::FindNext $Instance]
  bind $dialog <Escape> [list ::BayesText::FindDestroy $Instance $dialog]

  # Hemos añadido un tag para la selección, hay que quitarle como el tag sel
  bind $data(editor) <ButtonPress> +[list $data(editor) tag delete found]
  
  grid $f.forward  $f.backward -sticky nw
  
  grid $w.eTxt  -       $w.findNext -sticky news -padx 5 -pady 2
  grid $w.case  $w.dir  $w.cancelar -sticky new  -padx 5
  
  grid columnconfigure $w  0 -weight 1
  grid columnconfigure $w  1 -weight 0
  grid rowconfigure    $w  0 -weight 0
  grid rowconfigure    $w  1 -weight 0
  grid rowconfigure    $w  2 -weight 0
  grid rowconfigure    $w 10 -weight 1
  
  wm geometry $dialog 360x90
  $dialog draw
  update
  focus $data(frm,cad)
  $data(frm,cad).e selection range 0 end
}


#/////////////////////////////////////////////////////////////////////////////
proc ::BayesText::FindDestroy {Instance dialog} {
#
# PURPOSE: Replace text in editor
#
#/////////////////////////////////////////////////////////////////////////////
  variable options

  upvar \#0 ${Instance}::data data
  # inizialization values
  set options(find,dir)  $data(find,dir)
  set options(find,case) $data(find,case)
  ::BayesText::WriteIni
  # destroy
  destroy $dialog
}    


#/////////////////////////////////////////////////////////////////////////////
proc ::BayesText::ReplaceShow {editor {next 0}} {
#
# PURPOSE: Replace text in editor
#
#/////////////////////////////////////////////////////////////////////////////  
  variable options
  
  ::BayesText::ReadIni
  
  set Instance ::BayesText::Replace$editor
  if { [lsearch [namespace children ::BayesText] $Instance] != -1 } {
      
  } else  {
    namespace eval $Instance {
      variable data
      #data(find,text)    --> texto a buscar
      #data(replace,text) --> texto a por el que remplazar
      #data(replace,dir)  --> dirección de búsqueda
      #data(replace,case) -->
      #data(editor)       --> el path del widget text en el q buscar
      #data(path)         --> el path del toplevel que muestra el cuado de búsqueda
      #data(frm,cad)      --> entry donde se inserta el texto a buscar
    }
  }
  
  upvar \#0 ${Instance}::data data

  # inizialization values
  #set data(replace,dir)    $options(replace,dir)
  set data(find,dir)       $options(replace,dir)
  #set data(replace,case)   $options(replace,case)
  set data(find,case) $options(replace,case)
  
  set data(editor) $editor
  set data(path)   ${editor}.tReplace
  if [winfo exist $data(path)] {
    raise $data(path)
    focus $data(frm,cad)
    return
  }
  if {$next && [info exists ${Instance}::data(find,text)] && \
     [info exists ${Instance}::data(replace,text)] && ($data(find,text) !="") } {
    ReplaceNext $Instance
  } else {
    ReplaceCreate $Instance
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::BayesText::ReplaceCreate { Instance } {
#
# PURPOSE: Creates a Replace Dialog given a Instance
#
#/////////////////////////////////////////////////////////////////////////////   
  upvar \#0 ${Instance}::data data
  # opciones de remplazado

  set dialog [Dialog $data(path) -title [mc "Replace"]\
          -default 0 -modal none -parent $data(editor)]
  wm protocol $dialog WM_DELETE_WINDOW \
    [list ::BayesText::ReplaceDestroy $Instance $dialog]   
  
  set w [$dialog getframe]
  wm resizable $data(path) 0 0
  
  set data(replace,cad) [LabelEntry  $w.eTxt -width 35 \
          -label "[mc "Find"]: " -labelwidth 15 \
          -textvariable ${Instance}::data(find,text)]
  set data(replace,rem) [LabelEntry  $w.eRem -width 35 -labelwidth 15 \
          -label "[mc "Replace with"]: " \
          -textvariable ${Instance}::data(replace,text)]
  checkbutton $w.case  -text [mc "Case sensitive"] \
          -variable ${Instance}::data(replace,case) -pady 0
  Button $w.findNext -text [mc "Find next"] -width 16 \
          -command [list ::BayesText::FindNext $Instance] \
          -helptext [mc "Find next"]
  Button $w.replaceNext -text [mc "Replace"] -width 16 \
          -command [list ::BayesText::ReplaceNext $Instance] \
          -helptext [mc "Replace next"]
  Button $w.replaceAll -text [mc "Replace all"] -width 16 \
          -command [list ::BayesText::ReplaceAll $Instance] \
          -helptext [mc "Replace all"]
  Button $w.cancelar -text [mc "Cancel"] -width 16 -helptext "" \
    -command [list ::BayesText::ReplaceDestroy $Instance $dialog]
  
  bind $dialog <Return> [list ::BayesText::FindNext $Instance]
  bind $dialog <Escape> [list ::BayesText::ReplaceDestroy $Instance $dialog]
  
  grid $w.eTxt   $w.findNext     -sticky nw -padx 5 -pady 2
  grid $w.eRem   $w.replaceNext  -sticky nw -padx 5 -pady 2
  grid $w.case   $w.replaceAll   -sticky nw -padx 5 -pady 2
  grid ^         $w.cancelar     -sticky nw -padx 5 -pady 2
  
  wm geometry $dialog 360x90
  $dialog draw
  update
  focus $data(replace,cad)
}


#/////////////////////////////////////////////////////////////////////////////
proc ::BayesText::ReplaceDestroy {Instance dialog} {
#
# PURPOSE: Replace text in editor
#
#/////////////////////////////////////////////////////////////////////////////
  variable options

  upvar \#0 ${Instance}::data data
  # inizialization values
  #set options(replace,dir)  $data(replace,dir)
  #set options(replace,case) $data(replace,case)
  set options(replace,dir)  $data(find,dir)
  set options(replace,case) $data(find,case)
  ::BayesText::WriteIni  
  # destroy
  destroy $dialog
}    

#///////////////////////////////////////////////////////////////////////////// 
proc ::BayesText::ReplaceNext { Instance } {
#
# PURPOSE: Replace next ocurrence
#
#/////////////////////////////////////////////////////////////////////////////   
  upvar \#0 ${Instance}::data data
  # comprobamos que este lleno entry de buscar
  if {$data(find,text) != ""} {
    if {[catch "$data(editor) index sel.first" pos] == 0} {
      set lon    [string length $data(find,text)]
      set lonRem [string length $data(replace,text)]
      set posFin "$pos + $lon char"
      # reemplazamos caracteres seleccionados
      $data(editor) delete $pos $posFin
      $data(editor) insert $pos $data(replace,text)
      #tk::TextSetCursor $data(editor) $pos
      #tk::TextSetCursor $data(editor) $posFin
    }
  }
  FindNext $Instance
}


#///////////////////////////////////////////////////////////////////////////// 
proc ::BayesText::ReplaceAll { Instance } {
#
# PURPOSE: Replace all
#
#/////////////////////////////////////////////////////////////////////////////   
  upvar \#0 ${Instance}::data data
  # guardamos la posición actual
  set posOrig [$data(editor) index insert]
  # nos posicionamos en el principio del texto
  tk::TextSetCursor $data(editor) "1.0"
  # mientras posicion actual menor que fin repetimos busqueda
  set lon    [string length $data(find,text)]
  set lonRem [string length $data(replace,text)]
  set res    [FindPosText $Instance]
  set pos    [lindex $res 0]
  set lon    [lindex $res 1]
  set posFin "$pos + $lon char"
  while {[string length $pos]} {
    # reemplazamos caracteres seleccionados
    $data(editor) delete $pos $posFin
    $data(editor) insert $pos $data(replace,text)
    tk::TextSetCursor $data(editor) "$pos + $lonRem char"
    set res [FindPosText $Instance]
    set pos [lindex $res 0]
    set lon [lindex $res 1]
    set posFin "$pos + $lon char"
  }
  # recuperamos posicion, si es mayor que fin, ponemos fin
  set end [$data(editor) index end]
  if {[$data(editor) compare $posOrig > $end]} {
    tk::TextSetCursor $data(editor) $end
  } else  {
    tk::TextSetCursor $data(editor) $posOrig
  }
}


#///////////////////////////////////////////////////////////////////////////// 
proc ::BayesText::Open {editor path {pos 1.0}} {
#
# PURPOSE: 
#
#/////////////////////////////////////////////////////////////////////////////   
  if { ![catch {set fd [open $path r] } msgerr] } {
    $editor insert $pos [read -nonewline $fd]
    close $fd
    $editor mark set insert 1.0
    return 1
  }
  error $msgerr
}


#///////////////////////////////////////////////////////////////////////////// 
proc ::BayesText::SaveTxt {texto path} {
#
# PURPOSE: Save text editor
#
#/////////////////////////////////////////////////////////////////////////////   
  if { ![ catch { set fd [open $path w] } msgerr] } {
    #@ puts -nonewline $fd $texto
    close $fd
    return 1
  }
  error $msgerr
}


#///////////////////////////////////////////////////////////////////////////// 
proc ::BayesText::Save {editor path} {
#
# PURPOSE: Save 
#
#/////////////////////////////////////////////////////////////////////////////   
  set texto [$editor get 1.0 end]
  SaveTxt $texto $path
}


#///////////////////////////////////////////////////////////////////////////// 
proc ::BayesText::Cut {editor} {
#
# PURPOSE: Cut text
#
#/////////////////////////////////////////////////////////////////////////////   
  #clipboard clear
  tk_textCut $editor
}


#///////////////////////////////////////////////////////////////////////////// 
proc ::BayesText::Copy {editor} {
#
# PURPOSE: Copy text
#
#/////////////////////////////////////////////////////////////////////////////   
  #clipboard clear
  tk_textCopy $editor
}


#///////////////////////////////////////////////////////////////////////////// 
proc ::BayesText::Paste {editor} {
#
# PURPOSE: Paste text
#
#/////////////////////////////////////////////////////////////////////////////   
  # a partir de la version 8.4
  tk_textPaste $editor
  #catch {$editor insert insert [selection get -selection CLIPBOARD]}
}


#///////////////////////////////////////////////////////////////////////////// 
proc ::BayesText::SelectAll {editor} {
#
# PURPOSE: Select all text
#
#/////////////////////////////////////////////////////////////////////////////   
  $editor tag add sel 1.0 end
}


#///////////////////////////////////////////////////////////////////////////// 
proc ::BayesText::Delete {editor} {
#
# PURPOSE: Delete text selected
#
#/////////////////////////////////////////////////////////////////////////////   
  set state [$editor cget -state]
  $editor configure -state normal
  
  if ![catch {set ini [$editor index sel.first]}] {
    set fin [$editor index sel.last]
    $editor delete $ini $fin
  }
  $editor configure -state $state
}


#///////////////////////////////////////////////////////////////////////////// 
proc ::BayesText::DeleteAll {editor} {
#
# PURPOSE: Delete all
#
#/////////////////////////////////////////////////////////////////////////////   
  set state [$editor cget -state]
  $editor configure -state normal
  $editor delete 1.0 end
  $editor configure -state $state
}

#/////////////////////////////////////////////////////////////////////////////
proc ::BayesText::Refresh {editor} {
#
# PURPOSE: Refresh highlight 
#
# PARAMETERS:
#  editor -> text widget to refresh highlight
#
#/////////////////////////////////////////////////////////////////////////////
  $editor highlight 1.0 end
}


#///////////////////////////////////////////////////////////////////////////// 
proc ::BayesText::Indent {editor chars} {
#
# PURPOSE: Indent text
#
#/////////////////////////////////////////////////////////////////////////////   
  if ![catch {set ini [$editor index sel.first]}] {
    set fin [$editor index sel.last]
    set lineIni [lindex [split $ini .] 0]
    set lineFin [lindex [split $fin .] 0]
      
    for {set i $lineIni} {$i<= $lineFin} {incr i} {
      $editor insert $i.0 [string repeat " " $chars]
    }
    $editor tag add sel $ini $fin
  } else {
    $editor insert {insert linestart} [string repeat " " $chars]
  }
}


#///////////////////////////////////////////////////////////////////////////// 
proc ::BayesText::UnIndent {editor chars} {
#
# PURPOSE: UnIndent text
#
#/////////////////////////////////////////////////////////////////////////////   
  if ![catch {set ini [$editor index sel.first]}] {
    set fin [$editor index sel.last]
    set lineIni [lindex [split $ini .] 0]
    set lineFin [lindex [split $fin .] 0]
    
    for {set i $lineIni} {$i<= $lineFin} {incr i} {
      if [string equal [$editor get $i.0 $i.$chars] [string repeat " " $chars]] {
        $editor delete $i.0 $i.$chars
      }
    }
    $editor tag add sel $ini $fin
  } else {
    if [string equal [$editor get {insert linestart} "insert linestart + $chars c"]\
       [string repeat " " $chars]] {
      $editor delete {insert linestart} "insert linestart + $chars c"
    }
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::BayesText::GetSelectionOrAll {w} {
#
# PURPOSE: Returns selected text in text widget w (or all text if nothing 
#          is selected)
#
# PARAMETERS:
#  w -> text widget to refresh highlight
#
#/////////////////////////////////////////////////////////////////////////////
  set idxs [$w tag ranges sel]
  if [llength $idxs] {
    set txt [$w get [lindex $idxs 0] [lindex $idxs 1]]
  } else {
    set txt [$w get 0.0 end]
  }
  return $txt
}


#/////////////////////////////////////////////////////////////////////////////   
proc ::BayesText::FontChange {editor} {
#
# PURPOSE: Change font text
#
#/////////////////////////////////////////////////////////////////////////////     
  set fnt [SelectFont .sf -title [mc "Select font"] \
          -parent $editor -font [$editor cget -font]\
          -sampletext "Bayes Forecast"]
  if {($fnt != "") && ($editor != "")} {
    $editor configure -font $fnt
  }
  return $fnt
}


#/////////////////////////////////////////////////////////////////////////////   
proc ::BayesText::MenuCreate {editor} {
#
# PURPOSE: Create menu
#
#/////////////////////////////////////////////////////////////////////////////     
  set m [menu $editor.m -tearoff 0 ]
  set state [$editor cget -state]
  $m add command -label "[mc Find]..." -accelerator CTRL+F \
          -command [list ::BayesText::FindShow $editor]
  $m add command -label "[mc Replace]..." -accelerator CTRL+R -state $state\
          -command [list ::BayesText::ReplaceShow $editor]
  $m add command -label [mc "Find next"] -accelerator F3 \
          -command [list ::BayesText::FindShow $editor 1]
  $m add command -label [mc "Search function"] -accelerator CTRL+G \
          -command [list ::BayesText::SearchShow $editor]   
  $m add separator
  $m add command -label [mc "Cut"] -accelerator CTRL+X -state $state\
          -command [list ::BayesText::Cut $editor]
  $m add command -label [mc "Copy"] -accelerator CTRL+C \
          -command [list ::BayesText::Copy $editor]
  $m add command -label [mc "Paste"] -accelerator CTRL+V -state $state \
          -command [list ::BayesText::Paste $editor]
  $m add separator
  $m add command -label [mc "Select all"] -accelerator CTRL+E \
          -command [list ::BayesText::SelectAll $editor]
  $m add command -label [mc "Delete"] -accelerator SUPR -state $state\
          -command [list ::BayesText::Delete $editor]
  $m add command -label "[mc Font]..." -accelerator CTRL+T \
          -command [list ::BayesText::FontChange $editor]
  
  bind $editor <F3>        [list ::BayesText::FindShow $editor 1]
  if {[string equal $state "normal"]} {
    #bind $editor <Delete>    [list ::BayesText::Delete $editor]
  }
  
  bind $editor <Control-KeyRelease> \
    [list ::BayesText::OnControlKey $editor %K %k]
  
  #Por si el text esta desabilitado (para poder seleccionar)
  bind $editor <Button-1> {focus %W}
  bind $editor <Button-3> "tk_popup $m %X %Y"
  #Retornar el menu por si el que lo llama quiere añadir opciones
  return $m
}


#/////////////////////////////////////////////////////////////////////////////   
proc ::BayesText::OnControlKey {editor keysym keycode} {
#
# PURPOSE: Call of bind associate of letter
#
#/////////////////////////////////////////////////////////////////////////////     
  set state [$editor cget -state]
  switch [string toupper $keysym] {
    E { SelectAll $editor}
    F { FindShow $editor}
    R { if [string equal $state "normal"] { ReplaceShow $editor }  }
    T { FontChange $editor}
    C { if { [string equal $::tcl_platform(platform) "unix"] ||
             [string equal $keysym "C"] } {
          Copy $editor
      }
    }
    V { if { ([string equal $::tcl_platform(platform) "unix"] &&
              [string equal $state "normal"]) ||
              [string equal $keysym "V"]} {
          Paste $editor
      }
    }
    X { if { [string equal $keysym "X"] } {
          Cut $editor
      }
    }
  }
}

proc ::BayesText::TolEscape { buffer } {
  return [ string map [ list \n \\n \t \\t \" \\\" \\ \\\\ ] $buffer ]
}

proc ::BayesText::DlgEscapeTextApply { editor } {
  set w .dlgescape

  set idxs [ $editor tag ranges sel ]
  if { [ llength $idxs ] } {
    foreach {i0 i1} $idxs break
    $editor configure -undo 0
    $editor delete $i0 $i1
    $editor configure -undo 1
    set f [ $w getframe ]
    set buffer [ $f.t get 1.0 "end -1 char" ]
    set len [ string length $buffer ]
    $editor insert $i0 $buffer
    $editor tag add sel $i0 "$i0 + $len char"
  }
}

proc ::BayesText::DlgEscapeTextIdle { w } {
  variable DlgEscapeTextData

  if { [ winfo exists $w ] } {
    set f [ $w getframe ]
    set buffer [ $f.o get 1.0 "end -1 char" ]
    if { $buffer ne $DlgEscapeTextData(buffer) } {
      $f.t del 1.0 end
      $f.t ins end [ ::BayesText::TolEscape $buffer ]
      set DlgEscapeTextData(buffer) $buffer
    }
    update
    after idle [ list ::BayesText::DlgEscapeTextIdle $w ]
  }
}

proc ::BayesText::DlgEscapeText { editor } {
  variable DlgEscapeTextData

  set w .dlgescape
  if { [ winfo exists $w ] } {
    set f [ $w getframe ]
    raise $w
    focus $f.o
  } else {
    Dialog $w -title [ mc "Escape text" ] \
        -default 2 -modal none -parent $editor -separator 1
    $w add -text [ mc "Close" ] -command "destroy $w"
    $w add -text [ mc "Apply" ]
    set f [ $w getframe ]
    label $f.lo -text [ mc "Original:" ]
    text $f.o -width 40 -height 4
    label $f.lt -text [ mc "Transformed:" ]
    rotext $f.t -width 40 -height 4
    grid $f.lo -row 0 -column 0 -sticky w
    grid $f.o -row 1 -column 0 -sticky snew
    grid $f.lt -row 0 -column 1 -sticky w
    grid $f.t -row 1 -column 1 -sticky snew
    foreach c {0 1} {
      grid columnconfigure $f $c -weight 1
    }
    grid rowconfigure $f 1 -weight 1
    $w draw
  }
  $f.o delete 1.0 end
  $f.t del 1.0 end
  set DlgEscapeTextData(buffer) ""
  set idxs [ $editor tag ranges sel ]
  if { [ llength $idxs ] } {
    $f.o insert end [ $editor get [ lindex $idxs 0 ] [ lindex $idxs 1 ] ]
  }
  $w itemconfigure 1 -command [ list ::BayesText::DlgEscapeTextApply $editor ]
  update
  after idle [ list ::BayesText::DlgEscapeTextIdle $w ]
}

#/////////////////////////////////////////////////////////////////////////////
proc ::BayesText::TolSyntaxCheck { editor {dir ""} } {
#/////////////////////////////////////////////////////////////////////////////
  global toltk_script_path

  set buffer [ GetSelectionOrAll $editor ]
  if { $dir ne "" } {
    set cwd [ pwd ]
    cd $dir
  }

  if { [ catch { tol::checksyntax $buffer } check ] } {
    Tolcon_Trace "within ::BayesText::TolSyntaxCheck($editor,$dir): $check"
    return 0
  }
  set check [ string trim $check ]
  if { $check eq "" } {
    tk_messageBox -type ok -icon info -title [mc "Syntax Check"] \
        -parent $editor -message [mc "Syntax check done successfully"]
  } else {
    ShowSyntaxErrors $editor $check
  }
  return 1
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::BayesText::ShowSyntaxErrors {parent error} {
#
# PURPOSE: Shows syntax errors in a dialog window with a monospaced font
#
# PARAMETERS:
#   parent -> path of dialog parent
#   error  -> Syntax error to display
#
#/////////////////////////////////////////////////////////////////////////////
  global toltk_options

  set title "[mc "Syntax Check"]. [mc {Syntax errors in file}]"
  set dialog [Dialog $parent.dsc -title $title -modal local -parent $parent]
  set w [$dialog getframe]
  
  bind $dialog <Escape> [list destroy $dialog]  

  ScrolledWindow $w.sw
  text $w.sw.t -font $toltk_options(fonts,Fixed) -state normal
  $w.sw.t insert end $error
  $w.sw.t configure -state disabled
  $w.sw setwidget $w.sw.t
  
  set w_bbox \
    [ButtonBox $w.bb -orient horizontal -spacing 10 -padx 0 -pady 0 \
     -homogeneous false]
  $w_bbox add -text [mc "Accept"] -relief link \
          -helptext [mc "OK"] \
          -image [::Bitmap::get accept] -compound left -padx 5 -pady 1 \
           -command "destroy $dialog"
   
  pack $w_bbox -fill x -expand false -side bottom
  pack $w.sw -fill both -expand yes
  
  $dialog draw
}


#/////////////////////////////////////////////////////////////////////////////
proc ::BayesText::TolFileSyntaxCheck {path} {
#/////////////////////////////////////////////////////////////////////////////
  set ok 0
  # evaluamos el archivo
  set tolCheck "Text __EditorCheck = ParseError(ReadFile(\"$path\"))"
  #puts "TolFileSyntaxCheck: tolCheck = $tolCheck"
  if {[catch {::tol::console eval $tolCheck}]} {
    Tolcon_Trace "Falla porque TOL está ocupado"
  } else  {
    # comprobamos si ha dado algun error
    if {![catch { set objInfo [::tol::info variable [list Text __EditorCheck]]}]} {
	  #puts "TolFileSyntaxCheck: objInfo = $objInfo"
      # destruimos la variable creada en TOL
      ::tol::console stack release __EditorCheck
      #Tolcon_Trace "ParseError devuelve: [lindex $objInfo 0]"
      set ok [string equal [lindex $objInfo 2] "\"\""]
      if !$ok {
        set ok [list $ok [lindex $objInfo 2]]
      }
    }
  }
  return $ok
}

#/////////////////////////////////////////////////////////////////////////////
proc ::BayesText::CreateHLText {path lang args} {
#
# PURSPOSE: Creates a highlighted text. 
#
# PARAMETERS: 
#    path -> path where text has to be created
#    lang -> languaje to highlight (TOL, SQL, MSQ)
#         TOL -> Time Oriented Language
#         SQL -> SQL ansii 92
#         MSQ -> SQL for Microsoft SQL Server
#         ORA -> SQL for Oracle
#
#/////////////////////////////////////////////////////////////////////////////
  global toltk_options    

  array set opts [list \
    -background    white \
    -wrap          none  \
    -font          $toltk_options(fonts,Fixed) \
    -linemap       0 ]
  array set opts $args

  set w  [eval ctext $path [array get opts]]
  switch [string toupper $lang] {
    TOL {
          init_tolctext $w
        }
    SQL { 
          init_sqlctext $w
          init_msqctext $w
        }
    MSQ { 
          init_sqlctext $w
          init_msqctext $w
        }
    default {}
  }
  return $w
}



#/////////////////////////////////////////////////////////////////////////////
proc ::BayesText::init_sqlctext { w } {
#/////////////////////////////////////////////////////////////////////////////
  set lstSQL1 {\
     alter as by cascade commit connect create else end\
     from group having max min\
     insert into is primary select set table union update when where\
  }
  set lstSQL2 {and or not null in between outer join}
  set lstTypes {\
    int integer real float decimal char varchar nvarchar bit varbinary text\
    date datetime smalldatetime\
  }

  set lstSQL1upper {}
  foreach it $lstSQL1 {
    lappend lstSQL1upper [string toupper $it]
  }
  set lstSQL2upper {}
  foreach it $lstSQL2 {
    lappend lstSQL2upper [string toupper $it]
  }
  set lstTypesUpper {}
  foreach it $lstTypes {
    lappend lstTypesUpper [string toupper $it]
  }

  ctext::addHighlightClass $w sql1 blue $lstSQL1
  ctext::addHighlightClass $w sql1 blue $lstSQL1upper
  ctext::addHighlightClass $w sql2 #777777 $lstSQL2
  ctext::addHighlightClass $w sql2 #777777 $lstSQL2upper
  ctext::addHighlightClass $w sqlTypes blue $lstTypes
  ctext::addHighlightClass $w sqlTypes blue $lstTypesUpper
  # Line comments:
  ctext::addHighlightClassForRegexp $w comments #558855 {--[^\n^\r]+}
}


#/////////////////////////////////////////////////////////////////////////////
proc ::BayesText::init_msqctext { w } {
#/////////////////////////////////////////////////////////////////////////////
  set lst {\
    avg case cast convert count sum var varp checksum stdev stdevp checksum checksum_agg\
    dateadd datediff datename datepart day getdate getutcdate month year\
    ascii charindex difference left len lower ltrim  patindex quotename\
    replace replicate reverse right rtrim soundex space str stuff substring unicode upper\
    abs acos asin atan atn2 ceiling cos cot degrees exp floor\
    log log10 pi power radians rand round sign sin square sqrt tan\
  }

  set lstUpper {}
  foreach it $lst {
    lappend lstUpper [string toupper $it]
  }
  ctext::addHighlightClass $w specialMSQ #FF00AA $lst
  ctext::addHighlightClass $w specialMSQ #FF00AA $lstUpper

}

#/////////////////////////////////////////////////////////////////////////////
proc ::BayesText::init_tolctext { w } {
#/////////////////////////////////////////////////////////////////////////////
  ctext::addHighlightClass $w grammars black [list Anything Code Complex \
    CDate CTimeSet Date Matrix NameBlock Polyn Ration Real Serie Set Special Struct \
    Text TimeSet VMatrix PolMatrix Class \#Embed \#Require Static ]  

  ctext::addHighlightClass $w stackControl purple {BinGroup Case Copy Dating\
    Do Element Eval EvalArg Field Find Group IF If MakeGlobal PutDescription\
    PutName PutValue ReCalc TOLPath While Write WriteLn False True For\
    EvalSet \? }

  # Line comments:
  ctext::addHighlightClassForRegexp $w comments blue {//[^\n^\r]+}
}


#/////////////////////////////////////////////////////////////////////////////
proc ::BayesText::ReadIni {} {
#/////////////////////////////////////////////////////////////////////////////  
  variable options

  set rini ::iniFile::Read
  # find and replace dialog
  set options(find,dir)     [$rini BayesText FindDir     forward]
  set options(find,case)    [$rini BayesText FindCase    1]
  set options(replace,dir)  [$rini BayesText ReplaceDir  forward]
  set options(replace,case) [$rini BayesText ReplaceCase 1]   
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::BayesText::WriteIni {} {
#
# PURPOSE: Write common Editor options to .ini file
#
# PARAMETERS:
#   this  -> Instance of editor
#
#/////////////////////////////////////////////////////////////////////////////
  variable options
  set wini ::iniFile::Write

  # find and replace dialog
  $wini BayesText FindDir      $options(find,dir)
  $wini BayesText FindCase     $options(find,case)
  $wini BayesText ReplaceDir   $options(replace,dir)
  $wini BayesText ReplaceCase  $options(replace,case)  
  ::iniFile::Flush
}