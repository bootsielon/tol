#/////////////////////////////////////////////////////////////////////////////
# FILE    : tolbase.tcl
# PURPOSE : This file 
#/////////////////////////////////////////////////////////////////////////////

#/////////////////////////////////////////////////////////////////////////////      
if { 0 } {
#  
#/////////////////////////////////////////////////////////////////////////////        
  set toltk_script_path [file dirname [info script]]
  
  puts $toltk_script_path
    
  if { [lsearch $auto_path $toltk_library] == -1 } {
    lappend auto_path $toltk_library
  }
  if { ![info exists blt_libPath] } {
    set blt_libPath [file join $toltk_library blt]
  }
  if { ![info exists env(BLT_LIBRARY)] } {
    set env(BLT_LIBRARY) [file join $toltk_library blt]
  }
  package require BWidget
 
  source [file join $toltk_script_path "prjmenu.tcl"]
  source [file join $toltk_script_path "tolinspect.tcl"]
  source [file join $toltk_script_path "toltable.tcl"]
  source [file join $toltk_script_path "calendar.tcl"]
  source [file join $toltk_script_path "editSelector.tcl"]
  source [file join $toltk_script_path "selector.tcl"]
  source [file join $toltk_script_path "bayesgraph.tcl"]
  source [file join $toltk_script_path "sergraph.tcl"]
  source [file join $toltk_script_path "autocorgraph.tcl"]
  source [file join $toltk_script_path "graph.tcl"]
  source [file join $toltk_script_path "BfString.tcl"]
  source [file join $toltk_script_path "BfList.tcl"]
  source [file join $toltk_script_path "newButtonDiag2.tcl"]
  source [file join $toltk_script_path "newToolbarDiag.tcl"]
} else {
    if {$::tcl_platform(platform) eq "unix"} {
        proc ::dnd {args} {}
    } else {
        package require tkdnd
        source [file join $toltk_script_path "newButtonDiag2.tcl"]
        source [file join $toltk_script_path "newToolbarDiag.tcl"]
    }
}

global tcl_platform
if { [string equal $tcl_platform(platform) "unix"] } {
  source [file join $toltk_script_path "panedw.tcl"]
} else {
  source [file join $toltk_script_path "panedw.tcl"]
}

#source [file join $toltk_script_path "scrollw.tcl"]

namespace eval ::TolConsole {
  # widgets -> array with entries:
  #   inspector -> toplevel containing inspector and tabs of info.
  #   tabset    -> tabset containing: eval, output & info.
  #   eval      -> text for evaluating.
  #   output    -> text with the output.
  #   info      -> text with info on inspector objects.
  # data -> array with entries:
  #   (path) -> directory of the last included file
  #   (included) -> last included files
  #   (font,eval)
  #   (font,output)
  #   (font,info)
  #   (viewmode)
  variable data
  variable widgets
  variable time     [clock clicks -milliseconds]
  variable languages [list de German en English es Spanish fr French \
    it Italian ja Japanese nl Dutch po Portuguese ru Russian sv Swedish \
    zh Chinese]
}


#/////////////////////////////////////////////////////////////////////////////
proc ::TolConsole::Create { w } {
#
# PURPOSE: Creates TolBase main windows (tol inspector + eval/output/info
#          windows)
# PARAMTERS:
#  w: path of father the widget inspector
#  name: name of widget inspector
#
# RETURN: path of widget inspector
#
#/////////////////////////////////////////////////////////////////////////////
  variable widgets
  variable data

  # connection BODBC
  set data(bodbc) [$::project::data(bodbc) connection \
                     -name TolBase \
                     -stdtolcon 1  \
                     -stdtclcon 0  \
                     -iniconfig TolBaseODBC \
                     -savealias 1  \
                     -saveuser  1]
  Tolcon_Trace "TolConsole.Create : data(bodbc)=$data(bodbc)"

  #Control de modo de visualizacion
  #Frame

  set path [::project::CreateForm \
              -iniconfig ObjectInspector \
              -title [mc {Inspector}] -width 700 -height 400 \
              -helpkey "Tb4Ins"]
  $path configure -closecmd "$path minimize; return 0"
  set pathf [$path getframe]
					   
  set pw1   [PanedWindow $pathf.pw -side right -pad 0 -width 4]
  set pane1 [$pw1 add -weight $data(geometry,wtop)]
  set pane2 [$pw1 add -weight $data(geometry,wbottom)]

#  set widgets(pw)           $path
  set widgets(pw)           $pw1
  set widgets(panes,top)    $pane1
  set widgets(panes,bottom) $pane2
  # object inspector component

  ::TolInspector::Create $pane1.ti \
      -onselcmd ::TolConsole::OnInfo \
      -wleft $data(geometry,wleft) \
      -wright $data(geometry,wright)

  ::TolInspector::OnBusy ::TolConsole::OnBusy

  # tabset for eval, output & description
  set widgets(tabset) $pane2.ts
  set w_tabset [::blt::tabset $widgets(tabset) -side top -relief flat \
		    -bd 0 -outerpad 0 -tier 2 -slant right -textside right \
		    -highlightthickness 0]
  set sw2 [ScrolledWindow $w_tabset.sw2 -auto both]
  set sw3 [ScrolledWindow $w_tabset.sw3 -auto both]
    
  $w_tabset insert end \
     Eval   -text "[mc Eval]   <Ctrl+A>" -selectbackground \#d9d9d9 -bg gray75 -pady 0\
     Output -text "[mc Output] <Ctrl+O>" -selectbackground \#d9d9d9 -bg gray75 -pady 0\
     Info   -text "[mc Info]   <Ctrl+I>" -selectbackground \#d9d9d9 -bg gray75 -pady 0

  # OUTPUT CONSOLE COMPONENT  
  set widgets(output) $sw2.out
  set w_output [text $widgets(output) -wrap none -cursor ""\
     -state disabled -font $data(font,output)]
  $sw2 setwidget $w_output

  # EVAL CONSOLE COMPONENT      
  set feval $w_tabset.f  
  CreateEvalWindow $feval

  # configuro los mensajes del output
  $w_output tag configure errorTag   -foreground $data(colorErr)
  $w_output tag configure warningTag -foreground $data(colorWar)
  set data(output,TAG,E) errorTag
  set data(output,TAG,W) warningTag

  set menu [::BayesText::MenuCreate $w_output]
  # Añadimos la opción de recuperar la salida
  $menu insert 0 separator
  $menu insert 0 command -label [mc "Recover from log"] \
                  -command [list ::TolConsole::Recover $w_output]
  $menu insert 0 command -label [mc "Delete all"] \
                  -command [list ::BayesText::DeleteAll $w_output]

  # Habilitamos la opción de borrar
  $menu entryconfigure [mc "Delete"] -state normal
  bind $w_output <Delete>    [list ::BayesText::Delete $w_output]
      
  # INFO CONSOLE COMPONENT
  set widgets(info) $sw3.info
  set w_info [text $widgets(info) -wrap word -cursor ""\
     -state disabled -font $data(font,info)]
  $sw3 setwidget $w_info
  ::BayesText::MenuCreate $w_info
    
  set def_font [$w_info cget -font]
  if { [lsearch -exact [font names] fnBold] == -1 } {
    if {[string length [lindex $def_font 1]]} {
      font create fnBold -family [lindex $def_font 0] \
            -size [lindex $def_font 1] -weight bold
    } else {
       font create fnBold -family [lindex $def_font 0] -size 10 -weight bold
    }
  }

  # teclas acceso rapido solapas de Informacion
  $path bind <Control-F4> +[list $path kill]
  
  bind $path <Control-KeyRelease> \
      +[list ::TolConsole::OnControlKey %K %k]
  $w_tabset tab configure Eval   -window $feval -fill both -padx 0
  $w_tabset tab configure Output -window $sw2   -fill both -padx 0
  $w_tabset tab configure Info   -window $sw3   -fill both -padx 0
  pack $w_tabset -fill both -expand yes
    
  pack $pw1 -fill both -expand yes    
  Configure
 
  bind . <Control-Key-k> +::tol::stop
  bind Text <Control-Key-k> ""
  return $path
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolConsole::CreateEvalWindow {w} {
# 
# PURPOSE: Creates eval window as widget w
#
#/////////////////////////////////////////////////////////////////////////////
  variable widgets
  variable data

  set feval   [frame $w]
  set sw1 [ScrolledWindow $feval.sw1 -auto both]
  set widgets(eval) $sw1.eval

  #Creamos text con resaltado TOL
  set w_eval [::BayesText::CreateHLText $widgets(eval) tol \
    -font $data(font,eval)]

  set m [::BayesText::MenuCreate $w_eval]
  # Añadimos la opcion de refrescar el resaltado en la ventana eval
  $m insert 0 separator
  $m insert 0 command -label [mc Refresh]   -accelerator F5\
     -command [list ::BayesText::Refresh $w_eval]
  $m insert 0 command -label [mc "Syntax Check"] -accelerator F7 \
     -command [list ::BayesText::TolSyntaxCheck $w_eval]
  $m insert 0 command -label [mc Decompile] -accelerator F8\
     -command ::TolConsole::ClearConsole
  $m insert 0 command -label [mc Compile]   -accelerator F9\
     -command ::TolConsole::Compile

  # Añadimos la opción de limpiar la ventana de eval
  $m insert 0 separator
  $m insert 0 command -label [mc "Delete all"] \
     -command [list ::BayesText::DeleteAll $w_eval]
  bind $w_eval <F9> "::TolConsole::Compile ; break"
  bind $w_eval <F8> "::TolConsole::ClearConsole ; break"
  bind $w_eval <F5> "::BayesText::Refresh $w_eval ; break"
  bind $w_eval <F7> "::BayesText::TolSyntaxCheck $w_eval ; break"
  bind $w_eval <F10> "::TolConsole::OpenEval ; break"
  
  ## 1) Create a horizontal toolbar frame...
  set TF [::toolbar::ToolbarFrame $feval.toolbarFrame]
    ## 2) Create a toolbar in this toolbar frame...
    set tb1 [::toolbar::create $TF.tb1]
    # add compile button
    ::toolbar::addbutton $tb1 -image [::Bitmap::get "eval"] \
      -statuswidget $widgets(sb,path) -tooltip "[mc Compile] (F9)"\
      -command [list ::TolConsole::Compile]
    # add decompile button
    ::toolbar::addbutton $tb1 -image [::Bitmap::get "decom"] \
      -statuswidget $widgets(sb,path) -tooltip "[mc Decompile] (F8)"\
      -command [list ::TolConsole::ClearConsole]
    # add syntax check button
    ::toolbar::addbutton $tb1 -image [::Bitmap::get "syntaxcheck"] \
      -statuswidget $widgets(sb,path) -tooltip "[mc Syntax Check] (F7)"\
      -command [list ::BayesText::TolSyntaxCheck $w_eval]
    #add Refresh button
    ::toolbar::addbutton $tb1 -image [::Bitmap::get "refresh"] \
      -statuswidget $widgets(sb,path) -tooltip "[mc Refresh] (F5)"\
      -command [list ::BayesText::Refresh $w_eval]
    #add History button
    ::toolbar::addbutton $tb1 -image [::Bitmap::get "History"] \
      -statuswidget $widgets(sb,path) -tooltip "[mc "Show history file"] (F10)"\
      -command [list ::TolConsole::OpenEval]

    ::toolbar::addseparator $tb1

    ::toolbar::addbutton $tb1 -image [::Bitmap::get "delete"] \
      -statuswidget $widgets(sb,path) \
      -tooltip [mc "Delete all the messages of the window of output"] \
      -command [list ::BayesText::DeleteAll $widgets(output)]

    ## 4) Pack the toolbar inside the toolbar frame...
    pack $tb1 -side left -fill y

  ## 5) Pack the toolbar frame in our window...

  # Si empacabamos la barra de botones debajo del editor de la ventana eval,
  # cuando la ventana de Eval se hacía pequeña, la barra botones desaparecía
  # Con la opción -before se evita esto

  pack $sw1 -expand true -fill both -padx 0 -pady 0
  pack $TF  -side top -fill both -padx 0 -pady 0 -before $sw1

  $sw1 setwidget $w_eval
}


#/////////////////////////////////////////////////////////////////////////////
proc ::TolConsole::OnControlKey {keysym keycode} {
#
# PURPOSE: Controla es uso de teclas rápidas para le ejecución de funciones
#
# PARAMETERS: keysym -> The keysym corresponding to the event, 
#                       substituted as a textual string
#             keycode -> The keycode field from the event
#
#///////////////////////////////////////////////////////////////////////////
  variable widgets
  switch [string toupper $keysym] {
    A { ShowWindow "eval"   }
    O { ShowWindow "output" }
    I { ShowWindow "info"   }
  }
}

#///////////////////////////////////////////////////////////////////////////
proc ::TolConsole::ShowWindow {win} {
#///////////////////////////////////////////////////////////////////////////
  variable widgets
  #win can have values "eval" "output" "info"
  switch -exact $win  {
    "eval"   {
      $widgets(tabset) select 0 
      $widgets(eval) mark set anchor insert
    }
    "output" {
      $widgets(tabset) select 1 
    }
    "info"   {
      $widgets(tabset) select 2
    }
  }

}

#/////////////////////////////////////////////////////////////////////////////      
proc ::TolConsole::Configure {} {
#  
#/////////////////////////////////////////////////////////////////////////////        
  OptionsApplyDefDates
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::TolConsole::postIncluded {m {ord 0}} {
#
# PURPOSE: Fills a menu with the last included files from TolBase, according
#          to information of 'data' variable.
#
# PARAMETERS:
#   m  -> menu's path that will have the list of last included files
#   ord -> flag that indicates if files must be shown sorted
#
#/////////////////////////////////////////////////////////////////////////////
  variable data
  $m delete 0 end
  if $data(chIncluded) {
    set data(included) [lrange $data(included) 0 [expr $data(numRecent)-1]]
    if $ord {
      set lst [lsort -ascii $data(included)]
    } else {
      set lst $data(included)
    }
    foreach w $lst {
      #set w [lindex $w 0]
      $m add command -label $w -command [list ::TolConsole::IncludeCmd $w]
    }
    if [llength $lst] {
       $m add separator
       $m add command -label [mc "Clear"]\
             -command [list ::TolConsole::clearRecent]
    }
}
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::TolConsole::clearRecent {} {
#
# PURPOSE: Clears the list of last Recent files
#
#/////////////////////////////////////////////////////////////////////////////
  variable data
  set data(included) ""
  ::iniFile::Write TolConsole recent       data(included)
  ::iniFile::Flush
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::TolConsole::Format {value} {
#
# PURPOSE: Receives a value and if it's double, formats it according to
#          the information of 'data' variable.
#
# PARAMETERS:
#   value  -> value to format
#
# RETURN:  The value formated
#
#/////////////////////////////////////////////////////////////////////////////
  variable data
  set val $value
  if [string is double -strict $value] { 
    if {$data(chExponential) &&
       ![catch {expr log10($value) >= $data(numExponential)} result]} {
      if $result {
        if $data(chDecimals) {
          set val [format %.$data(chDecimals)e $value]
        } else {
          set val [format %e $value]
        }
      } else  {
        if $data(chDecimals) {
          set val [format "%.$data(numDecimals)f" $value]
        }
      }
    } else {
      if $data(chDecimals) {
        set val [format "%.$data(numDecimals)f" $value]
      }
    }
  }
  return $val
}

#/////////////////////////////////////////////////////////////////////////////      
proc ::TolConsole::OnInfo { args } {
#  
#/////////////////////////////////////////////////////////////////////////////        
  variable widgets
  
  set w_info $widgets(info)
  set w_tabset $widgets(tabset)
  $w_info configure -state normal
  $w_info delete 1.0 end
  switch [llength $args] {
    0 {
    }
    1 {
      #$w_info configure -wrap word
      $w_info insert end [lindex $args 0]
      $w_tabset select 2
    }
    default {
      #$w_info configure -wrap none
      foreach {icon grammar name value path desc} $args {}
        foreach {prop val} [list "Grammar: " $grammar "Name: " $name "Value: " $value "Path: " $path] {
        set idx_start [$w_info index "end -1 chars"]
        $w_info insert end $prop
        set idx_end   [$w_info index "end -1 chars"]
        $w_info tag add tagLabel $idx_start $idx_end
        if {[string equal $prop "Grammar: "]} {
          $w_info image create end -image $icon
          $w_info insert end "  "
        }
        $w_info insert end "$val\n"
        $w_info tag add tagValue $idx_end [$w_info index "end -1 chars"]
      }
      $w_info tag configure tagLabel -font fnBold
      $w_info tag configure tagValue -wrap none
      if { [string length $desc] } {
        set idx_start [$w_info index "end -1 chars"]
        $w_info insert end "Description\n"
        set idx_end [$w_info index "end -1 chars"]
        $w_info tag add tagPropLabel $idx_start $idx_end
        $w_info insert end $desc
        $w_info tag add tagPropValue $idx_end [$w_info index "end -1 chars"]
        $w_info tag configure tagPropLabel -font fnBold -underline 1
        #$w_info tag configure tagPropValue -wrap word
      }
      $w_tabset select 2
    }
  }
  $w_info configure -state disabled
}

#/////////////////////////////////////////////////////////////////////////////      
proc ::TolConsole::Include {file} {
#  
#/////////////////////////////////////////////////////////////////////////////        
  #::LoadStatus::Show $file

  ::TolInspector::Busy
  update
  ::tol::include $file
  ::TolInspector::UpdateFileRoot
  ::TolInspector::NotBusy
  #::LoadStatus::Hide
}


#/////////////////////////////////////////////////////////////////////////////
proc ::TolConsole::Compile { } {
#
# PURPOSE: Compiles text of eval window. If any text is selected, then only
#          selection is compiled, else all text is compiled.
#
#/////////////////////////////////////////////////////////////////////////////
  variable widgets
  set w_eval $widgets(eval)
  
  set tolexpr [::BayesText::GetSelectionOrAll $w_eval]
  EvalExpr $tolexpr
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolConsole::EvalExpr {tolexpr} {
#
# PURPOSE: Compiles a TOL expresion in TOL console
#
#/////////////////////////////////////////////////////////////////////////////
  ::TolInspector::Busy
  Tolcon_Trace "tolexpr = $tolexpr"
  if { [string length $tolexpr] } {
    ::tol::console eval $tolexpr
    SaveEval $tolexpr
  }
  ::TolInspector::UpdateConsoleObj
  ::TolInspector::NotBusy
}

#/////////////////////////////////////////////////////////////////////////////      
proc ::TolConsole::SaveEval {txt} {
#  
#/////////////////////////////////////////////////////////////////////////////        
  global home_path
  set evalFile [file join $home_path "eval.tol"]

  set texto ""
  if { [file exists $evalFile] } {
    set fd [open $evalFile r]
    set texto [read -nonewline $fd] 
    close $fd
  }
  set fd [open $evalFile w]
  set txt "// [::tol::date now {%W, %Y/%m/%d %h:%i:%s}]\n$txt\n\n"
  puts -nonewline $fd $txt
  puts -nonewline $fd $texto
  close $fd
}

#/////////////////////////////////////////////////////////////////////////////      
proc ::TolConsole::OpenEval {} {
#  
#/////////////////////////////////////////////////////////////////////////////        
  global home_path
  set evalFile [file join $home_path "eval.tol"]
  if { ![file exists $evalFile] } {
    set fd [open $evalFile w]
    close $fd
  }
  ::Editor::Open $evalFile
}

#/////////////////////////////////////////////////////////////////////////////      
proc ::TolConsole::OnConnect {alias user pass datasource} {
#  
#/////////////////////////////////////////////////////////////////////////////        
  set name [GetUniqueName Real]
  ::tol::console eval "Real $name = DBOpen(\"$alias\",\"$user\",\"$pass\");"
  ::tol::console stack release $name
}

#/////////////////////////////////////////////////////////////////////////////      
proc ::TolConsole::OnDisconnect {alias user pass datasource} {
#  
#/////////////////////////////////////////////////////////////////////////////        
  set name [GetUniqueName Real]
  ::tol::console eval "Real $name = DBClose(\"$alias\");"
  ::tol::console stack release $name
}

#/////////////////////////////////////////////////////////////////////////////      
proc ::TolConsole::DBConnect {} {
#  
#/////////////////////////////////////////////////////////////////////////////        
  ::ODBCDialog::Create $::project::widgets(mainframe) "" "" "" \
  ::TolConsole::OnConnect ::TolConsole::OnDisconnect
}

#/////////////////////////////////////////////////////////////////////////////      
proc ::TolConsole::ClearConsole { } {
#  
#/////////////////////////////////////////////////////////////////////////////        
  ::TolInspector::ClearConsoleObj
  ::TolInspector::UpdateConsoleObj
}

#/////////////////////////////////////////////////////////////////////////////      
proc ::TolConsole::OnBusy { state } {
#  
#/////////////////////////////////////////////////////////////////////////////        
    variable widgets
    
    if { $state } {
        $widgets(eval) configure -cursor watch
        $widgets(tabset) configure -cursor watch
       #  ::blt::busy hold .       
    } else {
        $widgets(eval) configure -cursor xterm
        $widgets(tabset) configure -cursor ""
      #  ::blt::busy release .      
    }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::TolConsole::ToEvalWindow { message } {
#
# PURPOSE: Writes a message in eval window and selects this tab (eval window)
#
# PARAMETERS:
#   message -> Message to write in eval window
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ::TolConsole::widgets(tabset) w_tabset
  upvar \#0 ::TolConsole::widgets(eval) w_eval

  $w_eval insert end "$message\n"
  $w_tabset select 0
  $w_eval see end
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolConsole::HciWriter { message } {
#  
#/////////////////////////////////////////////////////////////////////////////
  variable widgets
  variable data
  variable time
  
  upvar \#0 ::TolConsole::widgets(tabset) w_tabset
  upvar \#0 ::TolConsole::widgets(output) w_output
  if $data(chWrite) {
    $w_output configure -state normal
    set tr_message [string trim $message]
    # {<([^>]+)>([^<]+)</([^>]+)>}
    if { [regexp -- {^<([^>]+)>(.+)</(.+)>} $tr_message -> start content end] } {
      if {($start ne $end) || (($start ne "E") && ($start ne "W"))} {
        $w_output insert end "$message"
      } else { 
        set idx_start [$w_output index "end -1 char"]
        $w_output insert end "$content\n"
        set idx_end [$w_output index "end -1 char"]
        $w_output tag add $data(output,TAG,$start) $idx_start $idx_end
      }
    } else {
      $w_output insert end "$message"
    }
    $w_output configure -state disabled
  }
  
  switch $data(updateType) {
    always {
      if $data(chWrite) {
        $w_tabset select 1
        $w_output see end      
      }
      update
    }
    each   {
      set time_now [clock clicks -milliseconds]
      set dif [expr $time_now - $time]
      if { $dif >= $data(updateSec) } {
        set time $time_now
        if $data(chWrite) {
          $w_tabset select 1
          $w_output see end
        }
        update
      }
    }
    never  { }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::Tol_HciWriter { message } {
#  
#/////////////////////////////////////////////////////////////////////////////
  ::TolConsole::HciWriter $message
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolConsole::Recover {editor} {
#  
#/////////////////////////////////////////////////////////////////////////////
  set state [$editor cget -state]
  $editor configure -state normal
  $editor delete 1.0 end

  set cmd "Text $nRes = PutDumpFile(\"\");"
  ::tol::console eval $cmd
  array set inf [TclInfoVar Text $nRes]
  eval "set file $inf(CONTENT)"
  ::tol::console stack release $nRes
    
  if { ![catch {set fd [open $file r] } msgerr] } {
    $editor insert 1.0 [read -nonewline $fd]
    #HciWriter [read -nonewline $fd]
    close $fd
  }
  tk::TextSetCursor $editor end
  $editor configure -state $state
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolConsole::MainFrame { w {tolcon 1} } {
#  
#/////////////////////////////////////////////////////////////////////////////
  variable widgets
  variable data

  #bind ::project::widgets(mainframe) <Destroy> +[list ::TolConsole::Exit]
  bind . <Destroy> +[list ::TolConsole::Exit %W]

  # Aqui deberiamos llamar a la inicializacion tcl del proyecto.
  # solo si el proyecto ha puesto menus poner el de tolcon.
  project::OnExit ::TolConsole::BeforeExit

  #Creamos StatusBar antes de ButtonBar
  set sb [::TolConsole::CreateStatusBar]
  #Creamos ButtonBar
  set bb [::TolConsole::CreateButtonBar $tolcon]
  #Empacamos ButtonBar
  #if { [lsearch {left right} $data(toolbars,side)] ne -1 } {
  #  ::toolbar::SetOrientation $bb V
  #}
  if { $tolcon } {
    #Creamos inspector+Ventanas
    set widgets(inspector) [Create $w]
    bind $widgets(inspector) <F6> "::TolConsole::ApplyViewMode ; break"
    Tolcon_Trace "widgets(inspector): $widgets(inspector)"
  } else {
   # ::TolConsole::OptionsApplyDefDates 
  }
  
  # VISUALIZAMOS O NO EL INSPECTOR DE OBJETOS
  ::TolConsole::ObjectInspectorShow
  
  project::raiseSplash
  return $w
}


#/////////////////////////////////////////////////////////////////////////////
proc ::TolConsole::RaiseInspector {} {
#/////////////////////////////////////////////////////////////////////////////
  $::project::widgets(mainmdi).slave1 restore
  $::project::widgets(mainmdi).slave1 raise
}


#pedro-start
#/////////////////////////////////////////////////////////////////////////////
proc ::TolConsole::NewToolFrame { name { first 1 }} {
#  Crea una nueva ToolFrame para Toolbars
#/////////////////////////////////////////////////////////////////////////////
  variable widgets
  variable data
  
  switch $first {
    1 {
      set commandAux "toolbarFrame_${name}"
      set newTFname $commandAux
    }
    2 {
      set commandAux $name
      set newTFname  $name
    }
    default {
      set newTFname "toolbarFrame_${name}"
      set commandAux $::project::widgets(mainframe).${newTFname}
    }
  }
  # create a toolframe new  
  Tolcon_Trace "ToolFrame sale-----> $commandAux"
  set TF [::toolbar::ToolbarFrame $commandAux]
  #Añadimos como objetivo de Drop (tkdnd)
  dnd bindtarget $TF text/plain <Drop> "::TolConsole::TMotion %W %D $newTFname"
  #empacamos
  #pack $TF -side top -fill y 
  pack $TF -side $data(toolbars,side) \
    -before $::project::widgets(mainmdi) \
    -anchor nw -fill $data(toolbars,fill)
  #añadimos Botones por defecto
  #if { $first } {
  #::TolConsole::NewToolBarB $TF $name
  #}
  switch $first {
    1 {
      ::TolConsole::NewToolBarB $commandAux $name
    }
    2 {
      #
    }
    default {
      ::TolConsole::NewToolBarB $TF $name
    }
  }
  return $TF
}


#/////////////////////////////////////////////////////////////////////////////
proc ::TolConsole::NewToolBarB { parentTF name { jump 0 } {Child 0} {ini 0}} {
#  Crea una nueva toolbar para botones dentro del frame parentTF
#  parentTF : Frame papa
#  jump     : Opcion utilizada en caso de adopcion. (cambio de frame padre).
#  loopyPath: Contiene los parametros de creacion de TODOS los botones
#             de un toolbar, se recorre cuando jump es 1.
#/////////////////////////////////////////////////////////////////////////////
  variable widgets
  variable data
    
  switch $ini {
    1 {
      set data(tolbase,original) 0
      set newName $name
      set largo   [string length $newName]
      set newName [string range $newName 0 [expr $largo - 2]]
      set newTb   [::toolbar::create "$parentTF.$newName"]
    }
    2 {
      set newName [lindex [split $Child .] 3]
      set largo   [string length $newName]
      set newName [string range $newName 0 [expr $largo - 1]]
      if {[winfo exists $parentTF.$newName]} {
        set data(tolbase,original) 1
        } else {
        set data(tolbase,original) 0
        set newTb   [::toolbar::create "$parentTF.$newName"]
      }
    }
    default {
      set data(tolbase,original) 0
      set newName "tb_${name}"
      set newTb [::toolbar::create "$parentTF.$newName"]
    }
  }
  
  #if { $ini } {
  #  set newName $name
  #  Tolcon_Trace "newName -> $newName"
  #  set largo   [string length $newName]
  #  set newName [string range $newName 0 [expr $largo - 2]]
  #  set newTb   [::toolbar::create "$parentTF.$newName"]
  #} else {
  #  set newName "tb_${name}"
  #  Tolcon_Trace "newName -> $newName"
  #  set newTb [::toolbar::create "$parentTF.$newName"]
  #}
  
  if { $jump } {
    set test "\{"
    foreach path $widgets(createScript,$Child) {
      set Apic  [lindex $path 1]
      set Ahelp [lindex $path 2]
      set Acmd  [lindex $path 3]
      if { [string index $Acmd 0] == $test } {
        set largo  [string length $Acmd]
        set newCmd [string range  $Acmd 1 [expr $largo - 2]]
      } else {
        set newCmd $Acmd
      }
      #creamos botones exactamente iguales y en el mismo orden sobre nuevo
      #parent
      if {$data(tolbase,original) == 0} {
        ::TolConsole::NewButtonB $newTb $Apic $Ahelp $newCmd $parentTF
      }
     
    }
   Tolcon_Trace "/n COMPLETE DUMP ---> $widgets(createScript,$Child)"
  } 
  
  
  #empacamos 
  if {$data(tolbase,original)} {
    dnd bindsource $parentTF.$newName text/plain "return $parentTF.$newName"
    pack $parentTF.$newName -side left -anchor w \
              -fill x
    return $parentTF.$newName
  } else {
    #Drag-n-Drop para mover a diferentes toolframes
    dnd bindsource $newTb text/plain "return $newTb"
    pack $newTb -side left -anchor w \
              -fill x -in $parentTF
    return $newTb
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::TolConsole::NewButtonB { Tb pic help cmd parentTF} {
#  --- Crea un nuevo boton en la toolbar parametro ---
#
#  pathToolbar : Ruta completa al toolbar padre que tendra el boton
#  pic    : Imagen del Boton
#  prueba : Introducir boton de prueba (llevar a 0 para no empacar
#           dos botones por llamada.
#  help   : Ayuda del Boton Custom
#  cmd    :comando a ejecutar por el boton
#/////////////////////////////////////////////////////////////////////////////
  variable widgets
  variable data

  # imagen ?
  if { $pic == "" } {
    set pic index
  }
  # exists imagen ?
  if {[catch {::Bitmap::get $pic}]} {
    # no exists
    Tolcon_Trace "La imagen asociada al comando no existe. \
      No se mostrara el botón: imagen: $pic - comando: $help"
  } else {
    # exists
    set but [::toolbar::addbutton $Tb -image [::Bitmap::get $pic] \
      -tooltip [mc $help] -command $cmd]
    #event add <<TMotion_1>> <Button1-Motion>
    bind $but <<TMotion_1>> "dnd drag $Tb"
    bind $but <ButtonPress-2> "::toolbar::ButtonMenu"
    dnd bindsource $Tb text/plain "return $Tb"
    #guardamos parametros del create del boton, si es el primer boton, 
    #iniciamos la matriz.
    if { [info exists widgets(createScript,$Tb)] } {
      lappend widgets(createScript,$Tb) "$Tb $pic [list $help] [list $cmd] $parentTF"
    } else {
      set     widgets(createScript,$Tb) "{$Tb $pic [list $help] [list $cmd] $parentTF}"
    }
    if { ![regexp "$Tb $pic [list $help] [list $cmd] $parentTF" "$data(tolbase,lsttoolbars)" ] } {
      lappend data(tolbase,lsttoolbars) "$Tb $pic [list $help] [list $cmd] $parentTF"
    }
  }
  return 
}


#/////////////////////////////////////////////////////////////////////////////
proc ::TolConsole::TMotion { targetTF newChild name} {
#  Mueve Toolbar de un Frame a otro cuando ocurre un DND
#/////////////////////////////////////////////////////////////////////////////
  variable widgets
  variable data
  set test [winfo parent $newChild]

  if { $targetTF != $test } {
    pack forget $newChild

    set data(tolbase,lsttoolbars) \
      [lsub $data(tolbase,lsttoolbars) $widgets(createScript,$newChild)]
      
    if {$data(tolbase,ini)} {
      ::TolConsole::NewToolBarB $targetTF $name 1 $newChild 2
    } else {
      ::TolConsole::NewToolBarB $targetTF $name 1 $newChild 2
    }    
  } else {
    pack $newChild -in $targetTF -side left -anchor w \
                -fill x
  }
 return
}


#/////////////////////////////////////////////////////////////////////////////
proc ::TolConsole::CreateButtonBar { {tolcon 1} } {
#  
#/////////////////////////////////////////////////////////////////////////////
  variable widgets
  variable data
  
  set TF {}
  if {![llength $data(tolbase,lsttoolbars)]} {
    #Creamos ToolFrame Principal para botones por defecto.
    set TF [set widgets(toolbar,main) [::TolConsole::NewToolFrame 0 0]]
    
    # 2) Separamos Botones en diferentes Toolframes
    #Grupo Botones Inspector
    set tb1 [::TolConsole::NewToolBarB $TF "tb1"]
    #Show Inspector Window
    ::TolConsole::NewButtonB $tb1 insobj [mc "TOL Inspector"] \
      "$::project::widgets(mainmdi).slave1 restore; $::project::widgets(mainmdi).slave1 raise" $TF
    #Show Eval Window
    ::TolConsole::NewButtonB $tb1 evalwin [mc "Show Eval Window"] \
    [list ::TolConsole::ShowWindow "eval"] $TF
     #Show Output Window
     ::TolConsole::NewButtonB $tb1 outputwin [mc "Show Output Window"] \
     [list ::TolConsole::ShowWindow "output"] $TF
     #Show Info
     ::TolConsole::NewButtonB $tb1 infowin [mc "Show Info Window"] \
     [list ::TolConsole::ShowWindow info] $TF
       #Show ViewMode	
     ::TolConsole::NewButtonB $tb1 changeview [mc "Change View Mode"] \
     [list ::TolConsole::ApplyViewMode] $TF
    
    #Grupo Botones para Mostrar Acciones
     set tbExe [::TolConsole::NewToolBarB $TF "tbExe"]
     #New Editor Dialog
     ::TolConsole::NewButtonB $tbExe newfile [mc "New TOL file"] \
     [list ::Editor::New] $TF
     #Open TOL File
       ::TolConsole::NewButtonB $tbExe openfile [mc "Open TOL file"] \
     [list ::Editor::Open] $TF
     #New Table Dialog
     ::TolConsole::NewButtonB $tbExe newtable [mc "New Table"] \
     [list ::TolConsole::TableNew] $TF
     #Open Table Dialog
     ::TolConsole::NewButtonB $tbExe opentable [mc "Open Table"] \
     [list ::TolConsole::TableOpen] $TF

     #New Project
     ::TolConsole::NewButtonB $tbExe newproject [mc "New Project"] \
     [list ::TolProject::New] $TF
     #Open Project
     ::TolConsole::NewButtonB $tbExe openprj [mc "Open Project"] \
     [list ::TolProject::Open] $TF
    
      #Grupo Botones para Herramientas
     set tbTool [::TolConsole::NewToolBarB $TF "tbTool"]
     #DB Connection
       ::TolConsole::NewButtonB $tbTool db [mc "Connection to database"] \
     [list $::project::data(bodbc) showdialog]	$TF
     #Function Search
       ::TolConsole::NewButtonB $tbTool funsearch [mc "Functions Search"] \
     [list ::funcSearch::Show] $TF
     #SQL
     ::TolConsole::NewButtonB $tbTool bysSql [mc "Bayes SQL"] \
     [list ::SQLTable::TableBayesSQL] $TF
     #Calendar
     ::TolConsole::NewButtonB $tbTool TimeSet [mc "Calendar Expert"] \
     [list ::CalendarTms::ViewTimeSet] $TF
     #Calendar
     ::TolConsole::NewButtonB $tbTool tools [mc Options] \
     [list ::TolConsole::OptionsCreate] $TF
    
      #Grupo Botones para Distribucion de Ventanas y Ayuda Visual
     set tbHelp [::TolConsole::NewToolBarB $TF "tbHelp"]
     #Cascade
     ::TolConsole::NewButtonB $tbHelp cascade [mc "Cascade"] \
     [list $::project::widgets(mainmdi) arrange cascade] $TF
       #Horizontal
     ::TolConsole::NewButtonB $tbHelp tilehor [mc "Tile Horizontal"] \
     [list $::project::widgets(mainmdi) arrange hor] $TF
       #Vertical
     ::TolConsole::NewButtonB $tbHelp tilever [mc "Tile Vertical"] \
     [list $::project::widgets(mainmdi) arrange ver] $TF
       #About
     ::TolConsole::NewButtonB $tbHelp about [mc "About"] \
       [list ::TolConsole::AboutShow] $TF
     set widgets(tb1,path) $tb1
     set widgets(tbExe,path) $tbExe
     set widgets(tbTool,path) $tbTool
     set widgets(tbHelp,path) $tbHelp
     set widgets(toolbar,frame) $TF
  } else {
    #aqui creamos las barras de botones que hemos recuperado del INI
    set tfgitb [::TolConsole::GlobalIniToolbarsButtons]
    set TF [set widgets(toolbar,main) [lindex $tfgitb 0]]
  }
  # toolbar by default, in order to add bellboys from the outside
  if {![string length $TF]} {
    # if not exists, create new
    set TF [::TolConsole::NewToolFrame 0 0]
  }
  set widgets(tbCustom,path) [::TolConsole::NewToolBarB $TF "tbCustom"]
  return
}


#/////////////////////////////////////////////////////////////////////////////
proc ::TolConsole::GlobalIniToolbarsButtons {} {
#  
#/////////////////////////////////////////////////////////////////////////////
  variable widgets
  variable data
  
  set data(tolbase,ini) 1
  set lstToolFrames {}
  set lstFrames {}
  set lstFrameBut {}
  set i 1
  foreach item $data(tolbase,lsttoolbars) {
    lappend lstFrameBut   $item
    lappend lstToolFrames [lindex $item 4]
    lappend lstFrames     [list [lindex $item 0] [lindex $item 4]]
    incr i
  }
  set lstFrames     [lsort -unique $lstFrames]
  set lstFrameBut   [lsort -unique $lstFrameBut]
  set lstToolFramesAux {}
  foreach it $lstToolFrames {
    puts $it
    if {!([lsearch -exact $lstToolFramesAux $it ] >= 0)} {
      lappend lstToolFramesAux $it
    }
  }
  set tfaux {}
  foreach toolFrame $lstToolFramesAux {
    set tfaux [::TolConsole::NewToolFrame "$toolFrame" 2]
  }
  set lstFramesAux {}
  foreach it $lstFrames {
    puts $it
    if {!([lsearch -exact $lstFramesAux $it ] >= 0)} {
      lappend lstFramesAux $it
    }
  }
  foreach toolBar $lstFrames {
    ::TolConsole::NewToolBarB  [lindex $toolBar 1] [lindex [split $toolBar .] 3] 0 0 1 
  }
  foreach but $lstFrameBut {
    ::TolConsole::NewButtonB [lindex $but 0] [lindex $but 1] \
      [lindex $but 2] [lindex $but 3] [lindex $but 4]
  }
  return $tfaux
}


#/////////////////////////////////////////////////////////////////////////////
proc ::TolConsole::CreateStatusBar {} {
#  
#/////////////////////////////////////////////////////////////////////////////
  variable widgets
  
  set widgets(sb,path) [statusbar .sbar]
  .sbar add statTime -image [::Bitmap::get tinyclock]
  ::TolConsole::UpdateTime
if {0} {
  variable widgets
  # frame y label para barra de estado
  set widgets(sb,path) [set sf [frame $w.sb]]
  set widgets(sb,lInfo)   [label $sf.info   -anchor w -relief sunken]
  set widgets(sb,lTime)   [label $sf.cursor -anchor w -relief sunken]
  # empaquetamos los label de la barra de estado
  grid $w.sb.info   -row 0 -column 0 -sticky ew -padx 2
  grid $w.sb.cursor -row 0 -column 1 -sticky w  -padx 2
  grid columnconfigure $sf 0 -weight 1
  UpdateTime
  return $w.sb
}
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolConsole::UpdateTime {} {
#
# PURPOSE: Updates time in status bar
#
#/////////////////////////////////////////////////////////////////////////////
  variable widgets
  $widgets(sb,path) itemconf statTime \
    -text [clock format [clock seconds] -format {%H:%M:%S }]
  after 1000 ::TolConsole::UpdateTime
if {0} {
  variable widgets
  $widgets(sb,lTime) configure\
      -text [clock format [clock seconds] -format {%H:%M:%S }]
  after 1000 ::TolConsole::UpdateTime
}
}


#/////////////////////////////////////////////////////////////////////////////
proc ::TolConsole::ApplyViewMode {{change 1}} {
#
# PURPOSE: Changes heights of inspector and eval/output/info windows
#
#/////////////////////////////////////////////////////////////////////////////
  variable data
  variable widgets

  set state $data(viewmode)
  set states [list middle max min middle]
  if {$change} {
    set idx [lsearch -exact $states $state]
    set state [lindex $states [expr $idx+1]]
  }
  ::PanedWindow::MoveSash $widgets(pw) 1 y $state
  set data(viewmode) $state
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::TolConsole::AboutShow {} {
#
# PURPOSE: Shows the function search dialog
#
#/////////////////////////////////////////////////////////////////////////////
  set path .__about

  if ![winfo exist $path] {
    ::TolConsole::About $path
  }
  wm state $path normal
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::TolConsole::TolBaseHelp {} {
#
# PURPOSE: Shows the home page of help of Tolbase
#
#/////////////////////////////////////////////////////////////////////////////
  ShowHelp "Tb4Mai"
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::TolConsole::About { w } {
#
# PURPOSE: Shows a dialog that shows information about TolBase and Bayes.
#
#/////////////////////////////////////////////////////////////////////////////
  global toltk_library
  global GuiVersionInfo

  set imgfile [file join $toltk_library toltk images splash.gif]
  set img [image create photo -file $imgfile]
  toplevel $w
  wm title $w "[mc About] TOLBase"
  wm withdraw $w
  wm transient $w .
  wm iconbitmap $w [wm iconbitmap .]
  wm resizable $w 0 0
  label $w.lsplash -image $img -relief ridge -bd 2
  array set TolVersionInfo [::tol::info version]
  label $w.lversion -bg black -fg white -text \
    "[mc {TOL Version}]: $TolVersionInfo(major).$TolVersionInfo(minor).$TolVersionInfo(release) ($TolVersionInfo(comment))"
  label $w.laddress -bg black -fg white -text {
    Bayes Forecast: dynamic modelling of operations and markets
    Voice: (+34) 91.5327440; Fax: (+34) 91.5322636
    e-Mail: request@bayesinf.com; URL: http://www.tol-project.org
  }
  
  button $w.bc -text [mc Close] -command [list destroy $w]
  grid $w.lsplash
  grid $w.lversion -sticky "ew"
  grid $w.laddress -sticky "ew"
  grid $w.bc -padx 2 -pady 5 -sticky ns
  update
  # Aqui deberiamos invocar a un proc CenterWindow
  wm geometry $w +[expr ([winfo vrootw .]-[winfo reqwidth $w])/2]+[expr ([winfo vrooth .]-[winfo reqheight $w])/2]
  update
  wm withdraw $w
  wm deiconify $w
  update
}

#/////////////////////////////////////////////////////////////////////////////      
proc ::TolConsole::IncludeCmd {{path ""} } {
#  
#/////////////////////////////////////////////////////////////////////////////        
  variable data
  set types {
    {{Tol Files}  {.tol .bst .bsi .bst .bdt}}
    {{Text Files}  {.txt}}
    {{All Files}  *}
  }

  if {$path == ""} {
    if { [catch {tk_getOpenFile -filetypes $types \
  		   -initialdir $data(path) \
  		   -title "Source File"} fn] || [string match {} $fn] } {
      return
    }   
  } else  {
    set fn $path
  }
  UpdateListIncluded $fn
  Include $fn
   

}

#/////////////////////////////////////////////////////////////////////////////      
proc ::TolConsole::UpdateListIncluded {path} {
#  
#/////////////////////////////////////////////////////////////////////////////        
  variable data
  # guardamos el ultimo directorio abierto
  set data(path) [file dirname $path]
  if { [set idx [lsearch $data(included) $path]] == -1 } {
    set data(included) [linsert $data(included) 0 $path]
  } else {
    set ll ""
    lappend ll $path
    set i 0
    foreach f $data(included) {
      if  {$i != $idx} {
        lappend ll $f
      }
      incr i
    }
    set data(included) $ll
  }
  set data(included) [lrange $data(included) 0 [expr $data(numRecent)-1]]
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolConsole::Exit { w } {
#
#/////////////////////////////////////////////////////////////////////////////
  variable data
  variable widgets

  if {$w eq "."} {
    ProcessPendingTasks
    
    # Options project
    if {[info exists data(proc,OnOptionDestroy)] && \
            [string length $data(proc,OnOptionDestroy)]} {
      foreach f $data(proc,OnOptionDestroy) {
        $f
      }
    }
    WriteIni
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolConsole::BeforeExit { } {
#
#/////////////////////////////////////////////////////////////////////////////
  variable data
  variable widgets

  # check all ...
  set okEdi ok
  set okETr ok
  set okTPr ok
  catch {set okEdi [::Editor::CheckAll]}
  catch {set okETa [::EditTable::CheckAll]}
  catch {set okTPr [::TolProject::CheckAll]}

  if {[string eq $okEdi cancel] || [string eq $okETa cancel] || \
      [string eq $okTPr cancel]} {
    return cancel
  }
  # exit
  if { [info exists widgets(inspector)] } {
    foreach a {x y width height} {
      set data(geometry,$a) [winfo $a $widgets(inspector)]
    }
    set data(geometry,wtop) [winfo height $widgets(panes,top)]
    set data(geometry,wbottom) [winfo height $widgets(panes,bottom)]
    set data(geometry,wleft) [::TolInspector::GetWeight left]
    set data(geometry,wright) [::TolInspector::GetWeight right]
    set data(font,eval) [$widgets(eval) cget -font]
    set data(font,output) [$widgets(output) cget -font]
    set data(font,info) [$widgets(info) cget -font]
  }
  return "ok"
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolConsole::ProcessPendingTasks {} {
#
#/////////////////////////////////////////////////////////////////////////////
  # para que este procedimiento?
  return
  foreach id [after info] {
    set aft_info [after info $id]
    after cancel $id
    eval [lindex $aft_info 0]
  }
}

#/////////////////////////////////////////////////////////////////////////////  
proc ::TolConsole::OptionsCreate { {tabinit ""} } {
#
# PURPOSE: Create the dialog of options of TolBase
#
# PARAMETERS:
#   tabinit -> Tab where is focused the dialog when it's displayed (Optional). 
#
#/////////////////////////////////////////////////////////////////////////////  
  variable widgets
  variable data

  set path $widgets(inspector)

  set dialog [Dialog $path.t -title [mc "TOLBase Options"]\
              -parent $path -default 0 -cancel 1 -modal local]  
  wm protocol $dialog WM_DELETE_WINDOW \
    [list ::TolConsole::OptionsDestroy $dialog]
  set w [$dialog getframe]
  
  wm resizable $dialog 0 0
  # TabSet TolBase
  set w_ts [::blt::tabset $w.ts -relief flat -highlightthickness 0 -bd 0 \
    -outerpad 0 -tier 2 -slant both -textside right -side left -rotate 90 ]
  #insertar tabset
  $w_ts insert end TolBase
  set f0 [frame $w_ts.f0]  
  $w_ts tab configure "TolBase" -text "TolBase" \
    -window $f0 -fill both -padx 0 -selectbackground \#d9d9d9 -bg gray75  
  
  # TabSet Options
  set w_tabset [::blt::tabset $f0.ts -relief flat -highlightthickness 0\
    -bd 0 -outerpad 0 -tier 2 -slant right -textside right ]
  
  $w_tabset insert end TOL Output Precision Paths Others
  set f1 [frame $w_tabset.f1]
  set f2 [frame $w_tabset.f2]
  set f3 [frame $w_tabset.f3]
  set f4 [frame $w_tabset.f4]
  set f5 [frame $w_tabset.f5]

  $w_tabset tab configure "TOL" -text [mc "TOL"] -window $f1\
       -fill both -padx 0 -selectbackground \#d9d9d9 -bg gray75
  $w_tabset tab configure "Output" -text [mc "Output"] -window $f2\
       -fill both -padx 0 -selectbackground \#d9d9d9 -bg gray75
  $w_tabset tab configure "Precision" -text [mc "Precision"] -window $f3\
       -fill both -padx 0 -selectbackground \#d9d9d9 -bg gray75
  $w_tabset tab configure "Paths" -text [mc "Paths"] -window $f5\
       -fill both -padx 0 -selectbackground \#d9d9d9 -bg gray75
  $w_tabset tab configure "Others" -text [mc "Others"] -window $f4\
       -fill both -padx 0 -selectbackground \#d9d9d9 -bg gray75

  OptionsGet
  OptionsCreateTOL        $f1
  OptionsCreateOutput     $f2
  OptionsCreatePrecision  $f3
  OptionsCreatePaths      $f5
  OptionsCreateOthers     $f4
  
  # Options project
  if {[info exists data(proc,OnOptionCreate)] && \
      [string length $data(proc,OnOptionCreate)]} {
    foreach f $data(proc,OnOptionCreate) {
      $f $w_ts
    }
  }

  OptionsCheck

  set w_bbox \
          [ButtonBox $w.bb -orient horizontal -spacing 10 -padx 0 -pady 0 \
          -homogeneous false]
  $w_bbox add -text [mc "Accept"] -relief link \
          -helptext [mc "Make the changes and leave the dialog"] \
          -image [::Bitmap::get accept] -compound left -padx 5 -pady 1 \
          -command [list ::TolConsole::OptionsOK $dialog]
  $w_bbox add -text [mc "Cancel"] -relief link \
          -helptext [mc "Leave the dialog without apply any change"] \
          -image [::Bitmap::get cancelx] -compound left -padx 5 -pady 1 \
          -command [list ::TolConsole::OptionsDestroy $dialog]
  $w_bbox add -text [mc "Apply"] -relief link \
          -helptext [mc "Make the changes without leave the dialog"] \
          -image [::Bitmap::get apply] -compound left -padx 5 -pady 1 \
          -command ::TolConsole::OptionsApply

  pack $w_bbox   -side bottom -expand no  -fill x 
  pack $w_tabset -side top    -expand yes -fill both 
  pack $w_ts -expand yes -fill both ;#-sticky news
  
  if {!($tabinit eq "")} {
    switch -exact $tabinit  {
      "TOL" {
        $w_tabset select 0 
      }
      "Output" {
        $w_tabset select 1 
      }
      "Precision" {
        $w_tabset select 2 
      }
      "Paths" {
        $w_tabset select 3 
      }
      "Others" {
        $w_tabset select 4 
      }      
    }
  }
  bind $dialog <F1> "ShowHelp Tb4Opc"
  $dialog draw
}


#/////////////////////////////////////////////////////////////////////////////  
proc ::TolConsole::OptionsCreateTOL {w} {
#
#/////////////////////////////////////////////////////////////////////////////  
  variable tmpOpt

  #TitleFrame $w.tf1 -text [mc "--"]
  #set f1 [$w.tf1 getframe]

  TitleFrame $w.tf2 -text [mc "Default dates"]
  set f2 [$w.tf2 getframe]

  label $f2.lFirstDate -text [mc "First date"]:
  ::Objects::ChooseDateCreate ::TolConsole::tmpOpt(var,DefFirst) $f2.cFD

  label $f2.lLastDate -text [mc "Last date"]:
  ::Objects::ChooseDateCreate ::TolConsole::tmpOpt(var,DefLast) $f2.cLD

  frame $f2.f -width 25

  grid $f2.f $f2.lFirstDate $f2.cFD $f2.lLastDate $f2.cLD -sticky nw -padx 2
  grid rowconfigure    $f2 0 -weight 1
  grid columnconfigure $f2 4 -weight 1
  grid columnconfigure $f2 2 -pad 10
  

  #grid $w.tf1 -sticky news -padx 2
  grid $w.tf2 -sticky news -padx 2
     
  grid rowconfigure    $w 0 -weight 1
  grid rowconfigure    $w 1 -weight 3
  grid columnconfigure $w 0 -weight 1

}

#/////////////////////////////////////////////////////////////////////////////    
proc ::TolConsole::OptionsCreateOutput {w} {
#  
#/////////////////////////////////////////////////////////////////////////////    
  global toltk_options
  variable tmpOpt

  TitleFrame $w.tf1 -text [mc "Write"]
  set f1 [$w.tf1 getframe]

  checkbutton $f1.chWrite -text [mc "Write messages in output window"]\
      -variable ::TolConsole::tmpOpt(var,chWrite)
  label $f1.lColorErr -text [mc "Errors color"]:
  bcolor $f1.fColorErr -variable ::TolConsole::tmpOpt(var,colorErr)
  #::Objects::ColorCreate ::TolConsole::tmpOpt(var,colorErr) $f1.fColorErr
  label $f1.lColorWar -text [mc "Warnings color"]:
  ::Objects::ColorCreate ::TolConsole::tmpOpt(var,colorWar) $f1.fColorWar

  frame $f1.f  -width 20
  grid $f1.f $f1.chWrite   -             -             -             -sticky nw -padx 2
  grid ^     $f1.lColorErr $f1.fColorErr $f1.lColorWar $f1.fColorWar -sticky nw -padx 2

  grid rowconfigure    $f1 1 -weight 1
  grid columnconfigure $f1 4 -weight 1
  grid columnconfigure $f1 2 -pad 20


  TitleFrame $w.tf2 -text [mc "Update"]
  set f2 [$w.tf2 getframe]

  label $f2.lUpdateType -text [mc "Update options"]: 
  frame $f2.f1 -width 15
  radiobutton $f2.rbAlways -text [mc "Update always"]\
     -value always -highlightthickness 0 -pady 0\
     -variable ::TolConsole::tmpOpt(var,updateType)\
     -command ::TolConsole::OptionsCheckUpdateType
  radiobutton $f2.rbEach   -text [mc "Update each"]\
     -value each   -highlightthickness 0 -pady 0\
     -variable ::TolConsole::tmpOpt(var,updateType)\
     -command ::TolConsole::OptionsCheckUpdateType
  radiobutton $f2.rbNever -text [mc "Don't update until the end"]\
     -value never  -highlightthickness 0 -pady 0\
     -variable ::TolConsole::tmpOpt(var,updateType)\
     -command ::TolConsole::OptionsCheckUpdateType
  set tmpOpt(widget,updateSec) [SpinBox $f2.sbSeconds -justify right -width 4 \
      -editable 0 -range {0 2000 20} \
      -textvariable ::TolConsole::tmpOpt(var,updateSec)]
  label $f2.lSeconds -text [mc milliseconds]

  frame $f2.f -width 20
  grid $f2.f $f2.lUpdateType -     -             -             -sticky nw -padx 2
  grid ^     $f2.f1  $f2.rbAlways  -             -             -sticky nw -padx 2
  grid ^     ^       $f2.rbEach    $f2.sbSeconds $f2.lSeconds  -sticky nw -padx 2
  grid ^     ^       $f2.rbNever   -             -             -sticky nw -padx 2

  grid rowconfigure    $f2 3 -weight 1
  grid columnconfigure $f2 4 -weight 1
  
  grid $w.tf1 -sticky news -padx 2
  grid $w.tf2 -sticky news -padx 2
  grid rowconfigure    $w 1 -weight 1
  grid columnconfigure $w 0 -weight 1
}

#/////////////////////////////////////////////////////////////////////////////      
proc ::TolConsole::OptionsCreatePrecision {w} {
#  
#/////////////////////////////////////////////////////////////////////////////      
  variable tmpOpt

  TitleFrame $w.tf1 -text [mc "Precision"]
  set f1 [$w.tf1 getframe]

  checkbutton $f1.chDecimals -text [mc "Reduce decimal digits to"]\
      -variable ::TolConsole::tmpOpt(var,chDecimals)\
      -command ::TolConsole::OptionsCheckDecimals
  set tmpOpt(widget,decimals) [SpinBox $f1.sbDecimals -justify right -width 4 \
      -editable 0 -range {0 10} \
      -textvariable ::TolConsole::tmpOpt(var,numDecimals)]
  label $f1.lDecimals -text [mc digits]
    
  checkbutton $f1.chExponential \
      -text [mc "Use exponential notation when number has more than"]\
      -variable ::TolConsole::tmpOpt(var,chExponential)\
      -command ::TolConsole::OptionsCheckExponential
  set tmpOpt(widget,exponential) [SpinBox $f1.sbExponential -justify right -width 4 \
      -editable 0 -range {0 10} \
      -textvariable ::TolConsole::tmpOpt(var,numExponential)]
  label $f1.lExponential -text [mc digits]

  frame $f1.f -width 25

  grid $f1.f $f1.chDecimals    $f1.sbDecimals    $f1.lDecimals    -sticky nw -padx 2
  grid ^     $f1.chExponential $f1.sbExponential $f1.lExponential -sticky nw -padx 2
  grid rowconfigure    $f1 1 -weight 1
  grid columnconfigure $f1 3 -weight 1     
  
  grid $w.tf1 -sticky news -padx 2
  #grid $w.tf2 -sticky news -padx 2
     
  grid rowconfigure    $w 0 -weight 1
  #grid rowconfigure    $w 1 -weight 2
  grid columnconfigure $w 0 -weight 1
}

#/////////////////////////////////////////////////////////////////////////////      
proc ::TolConsole::OptionsCreatePaths {w} {
#
#/////////////////////////////////////////////////////////////////////////////        
  variable tmpOpt

  TitleFrame $w.tf -text [mc "Paths"]
  set f [$w.tf getframe]

  label $f.lPathBrowser   -text [mc "Browser path"]
  label $f.lPathPDFViewer -text [mc "PDF viewer path"]
  label $f.lPathHelp      -text [mc "Help path"]
  Entry $f.ePathBrowser   -state normal -width 30 \
    -textvariable ::TolConsole::tmpOpt(var,pathBrowser) \
    -helptext [mc pathexplanation]
  Entry $f.ePathPDFViewer -state normal -width 30 \
    -textvariable ::TolConsole::tmpOpt(var,pathPDFViewer)  \
    -helptext [mc pathexplanation]
  Entry $f.ePathHelp -state normal -width 30 \
    -textvariable ::TolConsole::tmpOpt(var,pathHelp)  
  Button $f.bPathBrowser -compound left -padx 10 \
    -height 16 -relief link \
    -helptext [mc "Search the program path"] \
    -image [Bitmap::get bopen] \
    -command {::TolConsole::OptionsCreatePath "Browser"}
  Button $f.bPathPDFViewer -compound left -padx 10 \
    -height 16 -relief link \
    -helptext [mc "Search the program path"] \
    -image [Bitmap::get bopen] \
    -command {::TolConsole::OptionsCreatePath "PDFViewer"}
  Button $f.bPathHelp -compound left -padx 10 \
    -height 16 -relief link \
    -helptext [mc "Search the folder"] \
    -image [Bitmap::get bopen] \
    -command  {set tmp [tk_chooseDirectory -initialdir $::TolConsole::tmpOpt(var,pathHelp) -mustexist 1]
               if {!($tmp eq "")} {
                 set ::TolConsole::tmpOpt(var,pathHelp) $tmp 
               } }

  grid $f.lPathBrowser   -row 0 -column 0 -sticky nw  -padx 5 -pady 5
  grid $f.lPathPDFViewer -row 1 -column 0 -sticky nw  -padx 5 -pady 5
  grid $f.lPathHelp      -row 2 -column 0 -sticky nw  -padx 5 -pady 5
  grid $f.ePathBrowser   -row 0 -column 1 -sticky new -padx 5 -pady 5
  grid $f.ePathPDFViewer -row 1 -column 1 -sticky new -padx 5 -pady 5
  grid $f.ePathHelp      -row 2 -column 1 -sticky new -padx 5 -pady 5
  grid $f.bPathBrowser   -row 0 -column 2 -sticky e   -padx 5 -pady 5
  grid $f.bPathPDFViewer -row 1 -column 2 -sticky e   -padx 5 -pady 5
  grid $f.bPathHelp      -row 2 -column 2 -sticky e   -padx 5 -pady 5

  grid columnconfigure $f 1  -weight 1
  grid rowconfigure    $f 10 -weight 1
  
  grid $w.tf -sticky news
  
  grid columnconfigure $w 0 -weight 1
  grid rowconfigure    $w 0 -weight 1
}

#/////////////////////////////////////////////////////////////////////////////      
proc ::TolConsole::OptionsCreatePath { type } {
#
# PURPOSE: Executes the command associated to the create path buttons:
#          Shows a dialog OpenFile with the directory and the appropriate
#          file type according to the operating system
#
# PARAMETERS: Type of path. Allowed values "Browser" "PDFViewer"
#
# TODO: For linux, to verify that the file is executable
#
#/////////////////////////////////////////////////////////////////////////////         
  variable tmpOpt
  
  if { $::tcl_platform(platform) == "windows" } {
    # get the "Program Files" directory
    set dirProgramFiles [registry get \
      {HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\Control\Session Manager\SFC} \
      ProgramFilesDir]
    set txtExecFileType "\{\{\{[mc {Executable files} ]\} .exe\}\}" 
    set commandFile "tk_getOpenFile \
      -initialdir [list $dirProgramFiles] -f $txtExecFileType"
  } else {  ;# linux
    set commandFile "tk_getOpenFile -initialdir \\"
  }
  
  #set ::TolConsole::tmpOpt(var,path$type) [eval $commandFile]
  set tmp [eval $commandFile]
  if { !($tmp eq "") } {
     set ::TolConsole::tmpOpt(var,path$type) $tmp
  }  
}
  
#/////////////////////////////////////////////////////////////////////////////      
proc ::TolConsole::OptionsCreateOthers {w} {
#  
#/////////////////////////////////////////////////////////////////////////////        
  variable tmpOpt
  variable languages

  TitleFrame $w.tf1 -text [mc "General Options"]
  set f1 [$w.tf1 getframe]

  label $f1.lRecent -text [mc "Number of last edited files"]

  set tmpOpt(widget,recent) [SpinBox $f1.sbRecent -justify right -width 4 \
      -editable 0 -range {0 10} \
      -textvariable ::TolConsole::tmpOpt(var,numRecent)]
  label $f1.lFiles -text [mc files]

  radiobutton $f1.rbOrdDate -text [mc "Order by date"]\
     -value date -highlightthickness 0 -pady 0\
     -variable ::TolConsole::tmpOpt(var,ordRecent)
  radiobutton $f1.rbOrdAsc   -text [mc "Order alphabetically"]\
     -value asc   -highlightthickness 0 -pady 0\
     -variable ::TolConsole::tmpOpt(var,ordRecent)
  radiobutton $f1.rbOrdDesc -text [mc "Order reverse alphabetically"]\
     -value desc  -highlightthickness 0 -pady 0\
     -variable ::TolConsole::tmpOpt(var,ordRecent)
    
  frame $f1.f -width 25

  grid $f1.f $f1.lRecent   $f1.sbRecent $f1.lFiles -sticky nw -padx 2
  grid ^     $f1.rbOrdDate -            -          -sticky nw -padx 2
  grid ^     $f1.rbOrdAsc  -            -          -sticky nw -padx 2
  grid ^     $f1.rbOrdDesc -            -          -sticky nw -padx 2

  grid rowconfigure    $f1 0 -weight 1
  grid rowconfigure    $f1 1 -weight 1
  
  grid rowconfigure    $f1 2 -weight 1
  
  grid columnconfigure $f1 3 -weight 1
  grid columnconfigure $f1 4 -weight 1
  
  TitleFrame $w.tf2 -text [mc "Language"]
  set f2 [$w.tf2 getframe]

  label $f2.lLanguage -text [mc "Choose language"]:
  foreach {key value} $languages {
    lappend values [mc $value]
  }

  ComboBox $f2.cbLanguages -width 12 -height 5 -editable false -justify right -values $values\
   -modifycmd   [list ::Objects::ComboUnTranslateDbl $f2.cbLanguages $languages\
       ::TolConsole::tmpOpt(var,language)]
  ::Objects::ComboSetIniDbl $f2.cbLanguages $languages ::TolConsole::tmpOpt(var,language)
  

  label $f2.lLangExp -text ([mc "Language is set when you restart interface"])
  frame $f2.f -width 25
  grid $f2.f $f2.lLanguage $f2.cbLanguages -sticky nw -padx 2
  grid ^     $f2.lLangExp  -               -sticky nw -padx 2
  grid rowconfigure    $f2 1 -weight 1
  grid columnconfigure $f2 2 -weight 1
  
  TitleFrame $w.tf3 -text [mc "Object Inspector"]
  set f3 [$w.tf3 getframe]
  checkbutton $f3.chObjIns -text [mc "Show Object Inspector"]\
      -variable ::TolConsole::tmpOpt(var,chObjIns)
  
  label $f3.lObjIns -text ([mc "Object Inspector visualization is set when you restart interface"])
  
  grid $f3.chObjIns -sticky nw -padx 2
  grid $f3.lObjIns  -sticky nw -padx 2
  grid rowconfigure    $f3 1 -weight 1
  grid columnconfigure $f3 2 -weight 1
  
  
  grid $w.tf1 -sticky news -padx 2
  grid $w.tf2 -sticky news -padx 2
  grid $w.tf3 -sticky news -padx 2
     
  grid rowconfigure    $w 0 -weight 1
  grid rowconfigure    $w 1 -weight 1
  grid rowconfigure    $w 2 -weight 1
  grid columnconfigure $w 0 -weight 1
}

#/////////////////////////////////////////////////////////////////////////////      
proc ::TolConsole::OptionsGet {} {
#  
#/////////////////////////////////////////////////////////////////////////////        
  variable data
  variable tmpOpt
  set tmpOpt(var,numRecent)      $data(numRecent)
  set tmpOpt(var,ordRecent)      $data(ordRecent)
  set tmpOpt(var,language)       $data(language)
  set tmpOpt(var,chDecimals)     $data(chDecimals)
  set tmpOpt(var,numDecimals)    $data(numDecimals)
  set tmpOpt(var,chExponential)  $data(chExponential)
  set tmpOpt(var,numExponential) $data(numExponential)
  set tmpOpt(var,DefFirst)       $data(DefFirst)
  set tmpOpt(var,DefLast)        $data(DefLast)
  set tmpOpt(var,colorErr)       $data(colorErr)
  set tmpOpt(var,colorWar)       $data(colorWar)
  set tmpOpt(var,chWrite)        $data(chWrite)
  set tmpOpt(var,updateType)     $data(updateType)
  set tmpOpt(var,updateSec)      $data(updateSec)
  set tmpOpt(var,showODBC)       $data(showODBC)
  set tmpOpt(var,pathBrowser)    $data(pathBrowser)
  set tmpOpt(var,pathPDFViewer)  $data(pathPDFViewer)
  set tmpOpt(var,pathHelp)       $data(pathHelp)
  set tmpOpt(var,chObjIns)       $data(chObjIns)
  
  
}

#/////////////////////////////////////////////////////////////////////////////      
proc ::TolConsole::OptionsSet {} {
#  
#/////////////////////////////////////////////////////////////////////////////        
  variable data
  variable tmpOpt
    
  regsub -all var, [array names ::TolConsole::tmpOpt var,*] "" optNames
    
  foreach it $optNames {
    set data($it) $tmpOpt(var,$it)
  }
}

#/////////////////////////////////////////////////////////////////////////////      
proc ::TolConsole::OptionsCheck {} {
#  
#/////////////////////////////////////////////////////////////////////////////        
  OptionsCheckUpdateType
  OptionsCheckDecimals
  OptionsCheckExponential
}

#/////////////////////////////////////////////////////////////////////////////      
proc ::TolConsole::OptionsCheckDecimals {} {
#  
#/////////////////////////////////////////////////////////////////////////////        
  variable tmpOpt
  if {!$tmpOpt(var,chDecimals)} {
    $tmpOpt(widget,decimals) configure -state disabled 
  } else  {
    $tmpOpt(widget,decimals) configure -state normal
  }
}

#/////////////////////////////////////////////////////////////////////////////      
proc ::TolConsole::OptionsCheckExponential {} {
#  
#/////////////////////////////////////////////////////////////////////////////        
  variable tmpOpt
  if {!$tmpOpt(var,chExponential)} {
    $tmpOpt(widget,exponential) configure -state disabled 
  } else  {
    $tmpOpt(widget,exponential) configure -state normal
  }
}

#/////////////////////////////////////////////////////////////////////////////      
proc ::TolConsole::OptionsCheckUpdateType {} {
#  
#/////////////////////////////////////////////////////////////////////////////        
  variable tmpOpt

  switch $tmpOpt(var,updateType) {
    always {
      $tmpOpt(widget,updateSec) configure -state disabled
    }
    each {
      $tmpOpt(widget,updateSec) configure -state normal
    }
    never {
      $tmpOpt(widget,updateSec) configure -state disabled
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////      
proc ::TolConsole::OptionsDestroy {dialog} {
#  
#/////////////////////////////////////////////////////////////////////////////        
  #variable data
  variable tmpOpt

  array unset tmpOpt
  destroy $dialog
}

#/////////////////////////////////////////////////////////////////////////////      
proc ::TolConsole::OptionsApply {} {
#  
#/////////////////////////////////////////////////////////////////////////////
  variable data
  # Apply
  OptionsSet
  OptionsApplyLanguage
  OptionsApplyDefDates
  OptionsApplyColors
  # Options project Apply
  if {[info exists data(proc,OnOptionApply)] && \
      [string length $data(proc,OnOptionApply)]} {
    foreach f $data(proc,OnOptionApply) {
      $f
    }
  }
  # Write INI
  WriteIni
}

#/////////////////////////////////////////////////////////////////////////////      
proc ::TolConsole::OptionsApplyLanguage {} {
#  
#/////////////////////////////////////////////////////////////////////////////        
  global toltk_script_path
  variable data

  ::tol::language $data(language)
  ::msgcat::mclocale $data(language)
  ::msgcat::mcload [file join $toltk_script_path msgs]
}

#/////////////////////////////////////////////////////////////////////////////      
proc ::TolConsole::OptionsApplyDefDates {} {
#  
#/////////////////////////////////////////////////////////////////////////////        
  variable data
  set expr "Date PutDefaultDates([DateTol $data(DefFirst)],[DateTol $data(DefLast)]);"
  #Tolcon_Trace "$expr"
  ::tol::console eval $expr
}

#/////////////////////////////////////////////////////////////////////////////      
proc ::TolConsole::OptionsApplyColors {} {
#  
#/////////////////////////////////////////////////////////////////////////////        
  variable data
  variable widgets

  $widgets(output) tag configure errorTag   -foreground $data(colorErr)
  $widgets(output) tag configure warningTag -foreground $data(colorWar)
}

#/////////////////////////////////////////////////////////////////////////////      
proc ::TolConsole::OptionsOK {dialog} {
#  
#/////////////////////////////////////////////////////////////////////////////        
  OptionsApply 
  OptionsDestroy $dialog
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::TolConsole::ReadIni {} {
#
# PURPOSE: Reads general TolBase options from .ini file.
#
#/////////////////////////////////////////////////////////////////////////////
  global toltk_script_path
  global toltk_options
  variable data
  set rini ::iniFile::Read
  
  #inicializo variable necesaria para las toolbars
  set data(tolbase,lsttoolbars) [$rini TolConsole listtoolbars]
  if {![llength $data(tolbase,lsttoolbars)]} {
    set data(tolbase,lsttoolbars) {}
  }
    
  set data(path)           [$rini TolConsole path       $toltk_script_path]
  set data(included)       [$rini TolConsole recent     {}]
  set data(chIncluded)     [$rini TolConsole saveRecent 1]
  set data(numRecent)      [$rini TolConsole numRecent  8]
  set data(ordRecent)      [$rini TolConsole ordRecent  "date"]
  set data(tabRecent)      [$rini TolConsole tabRecent  {}]
  set data(prjRecent)      [$rini TolConsole prjRecent  {}]
  set data(incRecent)      [$rini TolConsole incRecent  {}]
  set data(ediRecent)      [$rini TolConsole ediRecent  {}]  
  set data(font,eval)      [$rini TolConsole evalFont   \
    $toltk_options(fonts,Fixed)]
  set data(font,output)    [$rini TolConsole outputFont \
    $toltk_options(fonts,Fixed)]
  set data(font,info)      [$rini TolConsole infoFont   \
    $toltk_options(fonts,Fixed)]
  set data(language)       [$rini TolConsole language   es]
  set data(chDecimals)     [$rini TolConsole redDecimals    0]
  set data(numDecimals)    [$rini TolConsole numDecimals    4]
  set data(chExponential)  [$rini TolConsole useExponential 0]
  set data(numExponential) [$rini TolConsole numExponential 8]
  set data(DefFirst)       [$rini TolConsole DefFirst [::tol::date first]]
  set data(DefLast)        [$rini TolConsole DefLast  [::tol::date last ]]
  set data(colorWar)       [$rini TolConsole colorWar blue]
  set data(colorErr)       [$rini TolConsole colorErr red]
  set data(chWrite)        [$rini TolConsole writeOutput 1]
  set data(updateType)     [$rini TolConsole updateType always]
  set data(updateSec)      [$rini TolConsole updateSec 200]
  set data(showODBC)       [$rini TolConsole ShowODBCDialog 0]
  
  # program paths
  set data(pathBrowser)   [$rini TolConsole pathBrowser {}]
  set data(pathPDFViewer) [$rini TolConsole pathPDFViewer {}]
  set data(pathHelp)      [$rini TolConsole pathHelp {}]
  
  # Object Inspector visualization
  set data(chObjIns)      [$rini TolConsole chObjIns {}]
  
  # geometry

#  set data(geometry,x) [$rini TolConsole geometry:x -1]
#  set data(geometry,y) [$rini TolConsole geometry:y -1]
#  set data(geometry,width) [$rini TolConsole geometry:width -1]
#  set data(geometry,height) [$rini TolConsole geometry:height -1]
  set data(geometry,wtop)    [$rini TolConsole geometry:wtop 5]
  set data(geometry,wbottom) [$rini TolConsole geometry:wbottom 1]
  set data(geometry,wleft)   [$rini TolConsole geometry:wleft 3]
  set data(geometry,wright)  [$rini TolConsole geometry:wright 7]
  set data(viewmode)         [$rini TolConsole viewmode middle]
  
  #cargamos los parametros de las toolbars
  set data(toolbars,side) [$rini ToolBars side top]
  set data(toolbars,fill) [$rini ToolBars fill x]
  set data(toolbars,buttonbar,side) [$rini ToolBars buttonbar:side left]
  set data(toolbars,buttonbar,fill) [$rini ToolBars buttonbar:fill y]
  
  set data(tolbase,ini) 0
  set data(tolbase,original) 0
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolConsole::WriteIni {} {
#
# PURPOSE: Writes general TolBase options to .ini file.
#
#/////////////////////////////////////////////////////////////////////////////
  variable data
  variable widgets

  set wini ::iniFile::Write
  $wini TolConsole path           $data(path)
  $wini TolConsole recent         $data(included)
  $wini TolConsole saveRecent     $data(chIncluded)
  $wini TolConsole numRecent      $data(numRecent)
  $wini TolConsole ordRecent      $data(ordRecent)
  $wini TolConsole evalFont       $data(font,eval)
  $wini TolConsole outputFont     $data(font,output)
  $wini TolConsole infoFont       $data(font,info)
  $wini TolConsole language       $data(language)
  $wini TolConsole redDecimals    $data(chDecimals)
  $wini TolConsole numDecimals    $data(numDecimals)
  $wini TolConsole useExponential $data(chExponential)
  $wini TolConsole numExponential $data(numExponential)
  $wini TolConsole DefFirst       $data(DefFirst)
  $wini TolConsole DefLast        $data(DefLast)
  $wini TolConsole colorWar       $data(colorWar)
  $wini TolConsole colorErr       $data(colorErr)
  $wini TolConsole writeOutput    $data(chWrite)
  $wini TolConsole updateType     $data(updateType)
  $wini TolConsole updateSec      $data(updateSec)
  $wini TolConsole ShowODBCDialog $data(showODBC)
  $wini TolConsole tabRecent      $data(tabRecent)
  $wini TolConsole prjRecent      $data(prjRecent)
  $wini TolConsole incRecent      $data(incRecent)
  $wini TolConsole ediRecent      $data(ediRecent)

  # program paths
  $wini TolConsole pathBrowser    $data(pathBrowser)
  $wini TolConsole pathPDFViewer  $data(pathPDFViewer)
  $wini TolConsole pathHelp       $data(pathHelp)
  
  # Object Inspector visualization
  $wini TolConsole chObjIns       $data(chObjIns)
 
  # geometry configuration
  #  $wini TolConsole geometry:x $data(geometry,x)
  #  $wini TolConsole geometry:y $data(geometry,y)
  #  $wini TolConsole geometry:width $data(geometry,width)
  #  $wini TolConsole geometry:height $data(geometry,height)
  $wini TolConsole geometry:wtop    $data(geometry,wtop)
  $wini TolConsole geometry:wbottom $data(geometry,wbottom)
  $wini TolConsole geometry:wleft   $data(geometry,wleft)
  $wini TolConsole geometry:wright  $data(geometry,wright)
  $wini TolConsole viewmode         $data(viewmode)
  #::iniFile::Flush
  
  # escribimos las opciones para las barra de botones
  if { [winfo exists ${::project::widgets(mainframe)}.toolbarFrame]} {
    set packopts [pack info ${::project::widgets(mainframe)}.toolbarFrame]
    array set packoptsarray $packopts
    $wini ToolBars side $packoptsarray(-side)
	$wini ToolBars fill $packoptsarray(-fill)
    #set packopts [pack info ${::project::widgets(mainframe)}.toolbarFrame.tb1]
    #array set packoptsarray $packopts
    #$wini ToolBars buttonbar:side $packoptsarray(-side)
	#$wini ToolBars buttonbar:fill $packoptsarray(-fill)
  }
  
  # escribimos las opciones de los documentos abiertos
  $::project::widgets(mainmdi) writeDocsIni
  
  # escribilos las barras de botones que tenemos creadas
  # clear the bars created by code
  if {[info exists widgets(tbCustom,path)]} {
    set lsttollbarsaux {}
    foreach ltb $data(tolbase,lsttoolbars) {
      if {![string eq $widgets(tbCustom,path) [lindex $ltb 0]]} {
        lappend lsttollbarsaux $ltb
      }  
    }
    $wini TolConsole listtoolbars  $lsttollbarsaux
  }
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::TolConsole::ODBCShow {} {
#
# PURPOSE: Show the connection ODBC dialog
#
#/////////////////////////////////////////////////////////////////////////////
  variable data
  if {$data(showODBC)} {
    set values [::ODBCDialog::ReadIni]  
    ::ODBCDialog::Create $::project::widgets(mainframe) \
      [lindex $values 0] [lindex $values 1] "" \
      ::TolConsole::OnConnect ::TolConsole::OnDisconnect
  }
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::TolConsole::ObjectInspectorShow {} {
#
# PURPOSE: Show Object inspector
#
#/////////////////////////////////////////////////////////////////////////////
  variable data
  if {$data(chObjIns) ne "" &&
      [string is boolean $data(chObjIns)] && !$data(chObjIns)} {
    $::TolConsole::widgets(inspector) withdraw
  }
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::TolConsole::GetGCFFile {} {
#
# PURPOSE: Obtain the GCFFile file of .ini and showed to message in the
#          window output
#
#/////////////////////////////////////////////////////////////////////////////
  # GCFFile
  set gcfFile [::iniFile::Read SeriesGraph GCFFile {}]
  if {[string length $gcfFile]} { 
    ::tol::console eval "Text (GCFFile := \"$gcfFile\");"
    ::TolConsole::HciWriter "Text GCFFile = \"$gcfFile\"\n"
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::TolConsole::MenuMDI { } {
#
# PURPOSE: Crea un menu para mdi
#
#/////////////////////////////////////////////////////////////////////////////
  #cargo la lista de formularios
  set ::toolbar::data(toolbar,tolbase,lstcmd) {}
  
  set md [.mainmenu cget -definition]
  set hook $::project::data(menuhook)
  set hookhelp $::project::data(menuhookhelp)

  set id [$md insert $hook cascade -label [mc &File]]

  set cid [$md add $id cascade -label [mc &New] ]
  $md add $cid command -label [mc "TOL &file"] \
      -command "::Editor::New"
  $md add $cid command -label [mc "&Table"] \
      -command "::TolConsole::TableNew"
  $md add $cid command -label [mc "&Project"] \
      -command "::TolProject::New"
  
  set cid [$md add $id cascade -label [mc &Open] ]
  $md add $cid command -label [mc "TOL &file"] \
      -command "::Editor::Open"
  $md add $cid command -label [mc "&Table"] \
      -command "::TolConsole::TableOpen"
  $md add $cid command -label [mc "&Project"] \
      -command "::TolProject::Open"
  
  $md add $id command -label [mc &Include] -command "::TolConsole::IncludeCmd"
  
  $md add $id separator
  
  set cid [$md add $id cascade -label [mc "&Recent files"] ]
  $md add $cid cascade -label [mc "TOL &files"] \
      -postcommand "::Editor::postEdited %W"
  $md add $cid cascade -label [mc "&Tables"] \
      -postcommand "::TolConsole::TableRecent %W"
  $md add $cid cascade -label [mc "&Projects"] \
      -postcommand "::TolProject::postEdited %W"
  $md add $cid cascade -label [mc "&Included"] \
      -postcommand "::TolConsole::postIncluded %W"

  $md add $id separator
  
  #$md add $id command -label [mc E&xit] -command "::project::Exit"
  #$md add $id command -label [mc E&xit] -command "::TolConsole::Exit"
  $md add $id command -label [mc E&xit] -command "::project::OnDelete"

  
  set id [$md insert $hook cascade -label [mc &Tools]]
  set ::project::data(menu,tools) $id
    
  set cid [$md add $id cascade -label [mc "TOL &Editor"]]
  $md add $cid cascade -label [mc "&Recent files"] \
      -postcommand "::Editor::postEdited %W"
  $md add $cid command -label [mc "&New TOL file"] \
      -command "::Editor::New"
  $md add $cid command -label [mc "&Open TOL file"] \
      -command "::Editor::Open"
  
  set cid [$md add $id cascade -label [mc "TOL &Table Editor"]]
  $md add $cid cascade -label [mc "&Recent files"] \
      -postcommand "::TolConsole::TableRecent %W"
  $md add $cid command -label [mc "&New Table"] \
      -command "::TolConsole::TableNew"
  $md add $cid command -label [mc "&Open Table"] \
      -command "::TolConsole::TableOpen"
  
  set cid [$md add $id cascade -label [mc "TOL &Project Editor"]]
  $md add $cid cascade -label [mc "&Recent files"] \
      -postcommand "::TolProject::postEdited %W"
  $md add $cid command -label [mc "&New Project"] \
      -command "::TolProject::New"
  $md add $cid command -label [mc "&Open Project"] \
      -command "::TolProject::Open"
  
  $md add $id separator
  set cid [$md add $id cascade -label [mc "Analysis Tools"]]
  set cid2 [$md add $cid cascade -label [mc "ARIMA"]]
  $md add $cid2 command -label [mc "Draw ACF and PACF of ARIMA Models"] \
      -command "::drawArima::Show"
  $md add $id separator
  
  $md add $id command -label [mc "TOL Inspector"] \
    -command "$::project::widgets(mainmdi).slave1 restore; $::project::widgets(mainmdi).slave1 raise"
  $md add $id command -label [mc "&Functions Search"] \
      -command "::funcSearch::Show"
  $md add $id command -label [mc "&Calendar Expert"] \
      -command "::CalendarTms::ViewTimeSet"
  $md add $id command -label [mc "&Bayes SQL"] \
      -command "::SQLTable::TableBayesSQL"
  
  $md add $id separator
  
  $md add $id command -label [mc "C&onnection to database"] \
      -command "$::project::data(bodbc) showdialog"

  $md add $id command -label [mc "&Options"] \
      -command "::TolConsole::OptionsCreate"

  return $md
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::TolConsole::TableNew { } {
#
# PURPOSE: Creates a new table
#
#/////////////////////////////////////////////////////////////////////////////
  set tl [::project::CreateForm -type Tables \
            -width 400 -height 180 \
            -iniconfig Tables -helpkey "Tb4Edt"]
  set tlf [$tl getframe]
  # configuration
  btable $tlf.t -parent $tl \
                -posteditcmd [list ::TolConsole::TableRecentUpdate $tlf.t] \
                -iniconfig btable
  pack $tlf.t -fill both -expand yes
  $tl bind <Control-F4> [list $tl kill]
  #pedro
  
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::TolConsole::TableOpen { {path ""} } {
#
# PURPOSE: Opens a table
# PARAMETERS:
#   path   -> path of the file to be opened, if no file is given an Openfile
#             dialogs is shown
#
#/////////////////////////////////////////////////////////////////////////////
  set tl [::project::CreateForm -type Tables \
            -width 400 -height 180 \
            -iniconfig Tables -helpkey "Tb4Edt"]
  set tlf [$tl getframe]
  $tl configure -state withdrawn
  # configuration
  btable $tlf.t -parent $tl \
                -posteditcmd [list ::TolConsole::TableRecentUpdate $tlf.t] \
                -iniconfig btable
  if { $path == "" } {
    $tlf.t cmdOpen
    $tl configure -state normal
    if { [$tlf.t cget -path] == "" } {
      $tl kill
    } else {
      pack $tlf.t -fill both -expand yes
      $tl bind <Control-F4> [list $tl kill]
    }
  } else {
    $tlf.t configure -path $path
    $tl configure -state normal
    pack $tlf.t -fill both -expand yes
    $tl bind <Control-F4> [list $tl kill]  
  }
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::TolConsole::TableRecentUpdate { w args } {
#
# PURPOSE: Updates the last edited files when a table is closed
#
# PARAMETERS:
#   m   -> menu's path that will have the list of last table edited files
#   ord -> flag that indicates if files must be shown sorted
#
#/////////////////////////////////////////////////////////////////////////////
  variable data

  set newfile [$w cget -path]
  set pos [lsearch $data(tabRecent) $newfile]

  if { $pos == -1 } {
    set data(tabRecent) [linsert $data(tabRecent) 0 $newfile]
  } else {
    set data(tabRecent) [lreplace $data(tabRecent) $pos $pos]
    set data(tabRecent) [linsert $data(tabRecent) 0 $newfile]    
  }
  if { $data(numRecent) <= [llength $data(tabRecent)] } {
    set data(tabRecent) [lreplace $data(tabRecent) $data(numRecent) end]    
  }
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::TolConsole::TableRecent { m } {
#
# PURPOSE: Fills a menu with the last table edited files, according to the
#          information of 'options' variable.
#
# PARAMETERS:
#   m   -> menu's path that will have the list of last table edited files
#   ord -> flag that indicates if files must be shown sorted
#
#/////////////////////////////////////////////////////////////////////////////
  variable data
  
  set data(tabRecent) [lrange $data(tabRecent) 0 [expr $data(numRecent)-1]]
  
  $m delete 0 end
  if { $data(ordRecent) == "asc" } {
    set lst [lsort -ascii $data(tabRecent)]
  } elseif { $data(ordRecent) == "desc" } {
    set lst [lsort -ascii -decreasing $data(tabRecent)]
  } else {
    set lst $data(tabRecent)
  }
  foreach w $lst {
    $m add command -label $w -command [list ::TolConsole::TableOpen $w]
  }
  if [llength $lst] {
    $m add separator
    $m add command -label [mc "Clear"]\
          -command [list ::TolConsole::TableRecentClear]
  }
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::TolConsole::TableRecentClear { } {
#
# PURPOSE: Clears the list of last edited files
#
#/////////////////////////////////////////////////////////////////////////////
  variable data
  set data(tabRecent) ""
}
