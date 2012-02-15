#/////////////////////////////////////////////////////////////////////////////
# FILE    : editor.tol
# PURPOSE :  This file contains a namespace called Editor with its functions.
#           It's for creating a editor widget with TOL highlight syntax and
#           managing its events.
#/////////////////////////////////////////////////////////////////////////////

#/////////////////////////////////////////////////////////////////////////////
# TESTING
#/////////////////////////////////////////////////////////////////////////////
# Put if {1} for testing this file.
if { 0 } {
  package require Tk
  package require BWidget
  package require BLT
  Bitmap::use

  set toltk_script_path [file dirname [info script]]
  set toltk_images_path [file join ${toltk_script_path} images]

  #source [file join $toltk_script_path "ctext.tcl"]
  source [file join $toltk_script_path "supertext.tcl"]
  Supertext::overrideTextCommand
  source [file join $toltk_script_path "bystext.tcl"]
  lappend ::Bitmap::path $toltk_images_path

  package require msgcat 
  namespace import ::msgcat::*
  ::msgcat::mclocale es
  ::msgcat::mcload [file join $toltk_script_path msgs]

  set home_path $env(HOME)
  ::iniFile::Create [file join $home_path "tolcon.ini"]
  proc startupResources { } {
    global tcl_platform
    global toltk_options
    DynamicHelp::configure -background \#FFFFC0
    if [string equal $tcl_platform(platform) "windows"] {
      set toltk_options(colors,SystemButtonFace) SystemButtonFace
      set toltk_options(fonts,MenuNor) {Tahoma 8}
      set toltk_options(fonts,Fixed)   {{Courier New} 9}
      set toltk_options(fonts,Title1)  {Arial 9 bold}
      set toltk_options(fonts,Label)   {Arial 9}
      font create Letra1 -family "Arial" -slant roman -size 8
      set toltk_options(relief,ridge) ridge
    } else  {
      set toltk_options(colors,SystemButtonFace) #d4d4d4
      set toltk_options(fonts,MenuNor) {-*-helvetica-medium-r}
      set toltk_options(fonts,Fixed)   {Courier 10}
      set toltk_options(fonts,Title1)  {-*-helvetica-bold-r}
      set toltk_options(fonts,Label)   {-*-helvetica-medium-r}
      set toltk_options(relief,ridge) raised
    }
  }
  startupResources
}


#/////////////////////////////////////////////////////////////////////////////
  namespace eval ::Editor {
#
# PURPOSE: The purpose of this namespace is organize code and functions and
#          manage common options for all editors such as last edited files, 
#          last directory opened, etc.
#          A special namespace is created for each instance of editor.
#
# VARIABLES:
#   options -> common options for all editor windows:
#     geometry  -> initial window editor geometry
#     wm,state  -> initial window editor state
#     font      -> editor's text widget font
#     lastDir   -> last directory used
#     chEdited  -> boolen for saving last edited files path
#     numEdited -> number of last edited files path to save
#     edited    -> path of last edited files
#   tmpOpt     -> temporary array used to store the widgets and variables of
#              the options dialog.
#   num_editor -> incrementally number to identify each editor window
#   typelist   -> list of type files supported by editor
#   defExt     -> default extension for files managed by editor
#
#/////////////////////////////////////////////////////////////////////////////
  variable options
  variable num_editor 0
  variable typelist {{"Tol Files" {".tol"}} {"All Files" {"*"}}}
  variable defExt   .tol
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::Editor::New {{path ""}} {
#
# PURPOSE: Creates a new editor window and an instance of it. This also 
#          initializes the instance and register the window in the project.
#
# PARAMETERS:
#   path  -> path of file to edit. If it isn't specified a new white document
#            is created.
#
# RETURN: A instance of editor.
#
#/////////////////////////////////////////////////////////////////////////////
  variable num_editor
  incr num_editor

  set this [Create .editor$num_editor]
  SetTraces $this
  InitState $this new
  update
  if {$path == ""} {
    CmdNew $this
  }
  return $this
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::Editor::Open {{path ""} {run 0}} {
#
# PURPOSE: Opens a file in a editor window. If file is already opened it
#          raises its window, else it creates a new editor window and an
#          instance of it.
#
# PARAMETERS:
#   path  -> path of file to open. If it isn't specified it ask for it.
#   run   -> automatically once opened, compiles the file
#
# RETURN: A instance of editor.
#
#/////////////////////////////////////////////////////////////////////////////
  if {$path != ""} {
    #Comprobar si el archivo ya está abierto y si es así hacer un raise
    foreach ins [namespace children ::Editor] {
      upvar \#0 ${ins}::data data
      if {[string equal $data(nameFile) $path]} {
        #wm deiconify $data(f)
        $data(f) raise
        #focus $data(f)
        return $ins
      }
    }
    update
  }

  if {$path == ""} {
    CmdOpen ""
  } else  {
    set this [New $path]
    GetFromFile $this $path $run
  }  

  if {[info exists this]} {
    return $this
  } else  {
    return ""
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::Editor::postEdited {m {ord 0}} {
#
# PURPOSE: Fills a menu with the last edited files, according to the
#          information of 'options' variable.
#
# PARAMETERS:
#   m   -> menu's path that will have the list of last edited files
#   ord -> flag that indicates if files must be shown sorted
#
#/////////////////////////////////////////////////////////////////////////////
  variable options
  $m delete 0 end
  if $options(chEdited) {
    set options(edited) [lrange $options(edited) 0 [expr $options(numEdited)-1]]
    if $ord {
      set lst [lsort -ascii $options(edited)]
    } else {
      set lst $options(edited)
    }
    foreach w $lst {
      $m add command -label $w -command [list ::Editor::Open $w]
    }
    if [llength $lst] {
      $m add separator
      $m add command -label [mc "Clear"]\
            -command [list ::Editor::clearEdited]
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::Editor::clearEdited {} {
#
# PURPOSE: Clears the list of last edited files
#
#/////////////////////////////////////////////////////////////////////////////
  variable options
  set options(edited) ""
  ::iniFile::Write Editor recent       $options(edited)
  ::iniFile::Flush
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::Editor::OnDelete {this} {
#
# PURPOSE: Procedure to check things before destroying editor window
#
# PARAMETERS:
#   this  -> Instance of editor
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  if {[string equal [CheckOpen $this] "cancel"]} {
    return 0
  }
  destroy $data(f)
  return 1
}

#/////////////////////////////////////////////////////////////////////////////
proc ::Editor::Configure {this} {
#
# PURPOSE: Configures editor
#
#/////////////////////////////////////////////////////////////////////////////
  variable options

  set fnt $options(font)
 
  #puts "Configure con $fnt"

  if {[llength $fnt]>1} {
    font configure ctfnBold   -family [lindex $fnt 0] -size [lindex $fnt 1]
    font configure ctfnNormal -family [lindex $fnt 0] -size [lindex $fnt 1] 
  } else {
    font configure ctfnBold   -family $fnt -size 9
    font configure ctfnNormal -family $fnt -size 9
  }
  CmdRightMargin $this
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::Editor::Destroy {this w} {
#
# PURPOSE: Deletes the instance of editor. Before this,writes the ini file.
# 
# PARAMETERS:
#   this  -> Instance of editor
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  #CheckOpen $this
  if { [string equal $data(f) $w] } {
    # guardamos variables en el .ini
#puts "estamos destruyendo y guardando en el ini"
    WriteIni $this
    namespace delete $this
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::Editor::InitState {this from} {
#
# PURPOSE: Initializes some variables according to the state of editor.
#
# PARAMETERS:
#   this  -> Instance of editor
#   from  -> State that defines values for the variables
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data

  switch $from {
    new {
      set data(isSaved) 1
      set data(isCompiled) 0
      #If clipboard has something (to enable paste)
      set data(clipboard) 0 
      #If there is something selected (to enable cut, copy)
      set data(selected)  0 
      set data(nameFile) ""
      # Time of last modification of file
      set data(mTime)    0
      #pedro - balloon controller
      set data(balloon) 0
    }
    open {
      if {[lsearch [::tol::info included] $data(nameFile)] != -1} {
        set data(isCompiled) 1
      } else {
        set data(isCompiled) 0
      }
      set data(isSaved) 1
#      set data(nameFile) ""
    }
    save {
      set data(isSaved) 1
      #set data(isCompiled) 0
    }
  }
  #pedro-controls balloon hlp
  IniSupertext $this
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::Editor::InsertFileHeader {this} {
#
# PURPOSE:
#
# PARAMETERS:
#
#/////////////////////////////////////////////////////////////////////////////
  variable options
  upvar \#0 ${this}::data data
  if $options(chFileH) {
    if [file exists $options(fileH)] {
      ::BayesText::Open $data(txt) $options(fileH)
      #Lo marcamos como guardado a pesar de no ser realmente cierto
      set data(isSaved) 1
      tk::TextSetCursor $data(txt) end
    } else {
      #Sacar mensaje de que no existe el archivo especificado
      tk_messageBox -type ok -icon warning \
         -parent $data(txt) -title [mc Warning] \
         -message "[mc "Header file %1\$s doesn't exist" $options(fileH)].\n\
            [mc "Check editor's options"]"
      $data(txt) mark set insert 1.0
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::Editor::InsertFileFunction {this} {
#
# PURPOSE:
#
# PARAMETERS:
#
#/////////////////////////////////////////////////////////////////////////////
  variable options
  upvar \#0 ${this}::data data

  if [file exists $options(funcH)] {
    ::BayesText::Open $data(txt) $options(funcH) $data(position)
    #tk::TextSetCursor $data(txt) end
  } else {
    #Sacar mensaje de que no existe el archivo especificado
    tk_messageBox -type ok -icon warning \
       -parent $data(txt) -title [mc Warning] \
       -message "[mc "Functions file header %1\$s doesn't exist" $options(funcH)].\n\
          [mc "Check editor's options"]"
  }
  $data(txt) mark set insert $data(position)
}

proc ::Editor::IniSupertext {this} {
  upvar \#0 ${this}::data data
  set Supertext::undo($data(txt,supertext))      {}
  set Supertext::undoIndex($data(txt,supertext)) -1
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::Editor::SetTraces {this} {
#
# PURPOSE: Sets some traces of variables to control changes in editor widget
#
# PARAMETERS:
#   this  -> Instance of editor
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data

  trace variable ${this}::data(nameFile)   w \
        [list ::Editor::TraceNameFile $this]
  trace variable ${this}::data(selected)   w \
        [list ::Editor::TraceSelected $this]
  trace variable ${this}::data(isSaved)    w \
        [list ::Editor::TraceSaved $this]
  trace variable ${this}::data(isCompiled) w \
        [list ::Editor::TraceCompiled $this]
  trace variable ${this}::data(position)   w \
        [list ::Editor::TracePosition $this]
  trace variable Supertext::undo($data(txt,supertext)) w \
        [list ::Editor::TraceUndo $this]
}

proc ::Editor::UpdateMarkPos { this h} {
  #puts "UpdateMarkPos $this $h"

  variable options
  upvar \#0 ${this}::data data  
  if $options(chLine) {
    $data(txt,mark) configure -height $h
    set xpos [expr $options(posLine) * [font measure ctfnNormal "/"]]
    place $data(txt,mark) -in $data(txt) -x $xpos -y 0
  } else {
    place forget $data(txt,mark)
  }
}

proc ::Editor::OnConfigure { this w h } {
  UpdateMarkPos $this $h
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::Editor::Create {path} {
#
# PURPOSE: Creates a editor instance
#
# PARAMETERS:
#   path  -> editor's path (toplevel)
#
# RETURN: Editor's instance
#
#/////////////////////////////////////////////////////////////////////////////
  variable options

  set this ::Editor::$path
  if { [lsearch [namespace children ::Editor] $this] != -1 } {
    error "$this already exists as a children of ::Editor"
  }
  namespace eval $this {
    variable data
  }
  upvar \#0 ${this}::data data

  # create
  set data(f) [set f [::project::CreateForm -width 575 -height 700 \
                        -type Files \
                        -iniconfig Editor \
                        -closecmd [list ::Editor::OnDelete $this] \
                        -helpkey "Tb4Eda"] ]
  set f [$f getframe]
  
  # bind Destroy más abajo
  set n_b [list \
     "bnew"         "CmdNew"      "[mc [list New white document]] (CTRL+N)" \
     "bopen"        "CmdOpen"     "[mc Open] (CTRL+A)" \
     "bsave"        "CmdSave"     "[mc Save] (CTRL+S)" \
     "sep1"        "--"          ""\
     "bcut"         "CmdCut"      "[mc Cut] (CTRL+X)" \
     "bcopy"        "CmdCopy"     "[mc Copy] (CTRL+C)" \
     "bpaste"       "CmdPaste"    "[mc Paste] (CTRL+V)" \
     "sep2"        "--"          ""\
     "bundo"        "CmdUndo"     "[mc Undo] (CTRL+Z)" \
     "bredo"        "CmdRedo"     "[mc Redo] (CTRL+Y)" \
     "sep3"        "--"          ""\
     "bfind"        "CmdFind"     "[mc Find] (CTRL+F)" \
     "findnext"    "CmdFindNext" "[mc [list Find next]] (F3)" \
     "funsearch"   "CmdSearch"   "[mc {Search function}] (CTRL+G)" \
     "sep4"        "--"          ""\
     "unindent"     "CmdUnIndent"  "[mc Unindent] (CTRL+U)" \
     "indent"       "CmdIndent"    "[mc Indent] (CTRL+I)" \
     "sep5"        "--"          ""\
     "bprint"      "CmdPrint"    "[mc Print]" \
     "font"        "CmdFont"     "[mc Font] (CTRL+T)" \
     "sep6"        "--"          ""\
     "eval"        "CmdCompile"  "[mc Compile] (F11)" \
     "savecom"     "CmdSaveCompile" "[mc [list Save and Compile]] (F9)" \
     "decom"       "CmdDecompile"  "[mc Decompile] (F8)" \
     "syntaxcheck" "CmdSyntaxCheck" "[mc [list Syntax check]] (F7)" \
     "refresh"     "CmdRefresh" "[mc Refresh] (F5)"\
     "balloon"     "CmdBalloon"    "[mc BalloonHelp]" \
     "sep7"        "--"          ""\
     "tools"       "CmdOptions" "[mc Options]"]
  # botonera superior
  set data(tb,frame) [frame $f.ftop -relief raised -bd 1]
  foreach {n_img n_proc n_hint} $n_b {
    if {[string equal $n_proc "--"]} {
      Separator $f.ftop.$n_img -orient vertical
      #pack $f.ftop.$n_img -side left -fill y
    } else {
      set data(tb,$n_img) [Button $f.ftop.$n_img\
                -image [::Bitmap::get ${n_img}] -relief link\
                -helptext $n_hint -bd 2\
                -command [list ::Editor::$n_proc $this]]
    }
    pack $f.ftop.$n_img -anchor nw -side left -pady 2 -fill y -padx 1
  }
  #desactivamos botones de print y redo
  #$data(tb,redo)  configure -state disabled
  $data(tb,bprint) configure -state disabled
      
    
  # frame y label para barra de estado
  set data(sb,frame) [frame $f.fop -relief groove -bd 1 -height 7]
  set data(sb,posi)  [label $f.fop.posi -relief groove -bd 1 -width 16]
  set data(sb,comp)  [label $f.fop.comp -text [mc "Not Compiled"] \
                                        -relief raised -bd 1 -width 16]
  set data(sb,modi)  [label $f.fop.modi -bd 1 -width 14]
  set data(sb,nlin)  [label $f.fop.nlin -relief groove -bd 1 -width 16]
  set data(sb,rell)  [label $f.fop.rell -relief groove -bd 1]
  # texto y scroll
  scrollbar $f.sy -command "$f.t yview"
  scrollbar $f.sx -orient horizontal -command "$f.t xview"

  set data(txt) [::BayesText::CreateHLText $f.t tol \
         -background white -borderwidth 0 -wrap none\
         -foreground black -relief sunken \
         -yscrollcommand "$f.sy set" \
         -xscrollcommand "$f.sx set" \
         -preproc "::Editor::PreProc $this" \
         -linemap 0 -font $options(font)]

  bind $data(txt) <Configure> "::Editor::OnConfigure $this %w %h" 
  set data(txt,ctext)      $data(txt).t
  set data(txt,mark) [frame $f.mark -bg gray50 -width 1 \
			  -cursor [$data(txt,ctext) cget -cursor]]

  set data(txt,supertext)  __$data(txt,ctext)

  focus $data(txt,ctext)
  # empaquetamos los label de estado
  pack $f.fop.posi $f.fop.comp $f.fop.modi $f.fop.nlin -side left
  pack $f.fop.rell -expand true -fill x
    
  grid $f.ftop  -     -sticky new 
  grid $f.t     $f.sy -sticky news 
  grid $f.sx          -sticky ew 
  grid $f.fop   -     -sticky ews 

  grid rowconfigure    $f 1 -weight 1
  grid columnconfigure $f 0 -weight 1

  #Creación de un menú contextual
  # leer del archivo de cinfiguracion para coger la fuente del texto
  set m [menu $f.m -tearoff 0]
  $m add command -label [mc "Save and Compile"] -accelerator F9 \
                 -command [list ::Editor::CmdSaveCompile $this]
  $m add command -label [mc "Decompile"] -accelerator F8 \
                 -command [list ::Editor::CmdDecompile $this]
  $m add command -label [mc "Syntax Check"] -accelerator F7 \
                 -command [list ::Editor::CmdSyntaxCheck $this]
  $m add command -label [mc "Compile"] -accelerator F11 \
                 -command [list ::Editor::CmdCompile $this]
  $m add command -label [mc "Refresh"] -accelerator F5 \
                 -command [list ::Editor::CmdRefresh $this]

  $m add separator 
  $m add command -label "[mc Open]..." -accelerator CTRL+A \
                 -command [list ::Editor::CmdOpen $this]
  $m add command -label [mc "Save"] -accelerator CTRL+S \
                 -command [list ::Editor::CmdSave $this]
  $m add command -label "[mc [list Save as]] ..." \
                 -command [list ::Editor::CmdSaveAs $this]
  $m add separator
  $m add command -label [ mc "Escape text" ]... \
      -command [ list ::BayesText::DlgEscapeText $data(txt) ]
  $m add command -label "[mc Find]..." -accelerator CTRL+F \
                 -command [list ::Editor::CmdFind $this]
  $m add command -label [mc "Find next"] -accelerator F3 \
                 -command [list ::Editor::CmdFindNext $this]
  $m add command -label "[mc Replace]..." -accelerator CTRL+R \
                 -command [list ::Editor::CmdReplace $this]
  $m add command -label "[mc {Search function}]..." -accelerator CTRL+G \
                 -command [list ::Editor::CmdSearch $this]
  $m add separator
  $m add command -label [mc "Insert function schema"] \
                 -command [list ::Editor::InsertFileFunction $this]
  $m add command -label "[mc Font]..." -accelerator CTRL+T \
                 -command [list ::Editor::CmdFont $this]
  $m add check   -label [mc "Show right margin line"] -variable ::Editor::options(chLine) \
                 -command [list ::Editor::CmdRightMargin $this]
  set me [menu $f.m.edit -tearoff 0]
  $m add cascade -label [mc "Edit"] -menu $me
  $me add command -label [mc "Undo"] -accelerator CTRL+Z \
                  -command [list ::Editor::CmdUndo $this]
  $me add command -label [mc "Cut"] -accelerator CTRL+X \
                  -command [list ::Editor::CmdCut $this]
  $me add command -label [mc "Copy"] -accelerator CTRL+C \
                  -command [list ::Editor::CmdCopy $this]
  $me add command -label [mc "Paste"] -accelerator CTRL+V \
                  -command [list ::Editor::CmdPaste $this]
  $me add command -label [mc "Select all"] -accelerator CTRL+E \
                  -command [list ::Editor::CmdSelectAll $this]
  
  #CVS Utilities
  #$m add separator
  #set mc [menu $f.m.cvs -tearoff 0]
  #$m add cascade -label [mc "CVS Utilities"] -menu $mc
  #$mc add command -label [mc "TkCVS"] \
  #                -command [list ::Editor::CmdTkcvs $this 0]
  #$mc add command -label [mc "TkDiff"] \
  #                -command [list ::Editor::CmdTkcvs $this  1]
  

  bind $data(txt,ctext) <Button-3>  "tk_popup $m %X %Y"
  #pedro
  #$data(txt,ctext) tag bind Text <Enter> [list ::Editor::BalloonHlp $this]
  #set data(menu) [::BayesText::MenuCreate $data(txt)]
  set data(menu) $m
  #Quitar los eventos que pueden dar conflictos
  bind Text <Control-a> ""
  # necesario para q al escribir el simbolo del euro no ejecute tb el comando
  #bind Text <Control-e> "::Editor::CmdSelectAll $this"
  bind Text <Control-f> ""
  bind Text <Control-i> ""
  bind Text <Control-n> ""
  bind Text <Control-t> ""
   
  # eventos asociados al formulario 
  bind $data(txt) <F1>        "::Editor::CmdHelpEditor $this ; break"
  bind $data(txt) <F3>        "Editor::CmdFindNext $this ; break"
  bind $data(txt) <F5>        "::Editor::CmdRefresh $this ; break"
  bind $data(txt) <F7>        "::Editor::CmdSyntaxCheck $this ; break"
  bind $data(txt) <F8>        "::Editor::CmdDecompile $this  ; break"
  bind $data(txt) <F9>        "Editor::CmdSaveCompile $this ; break"
  bind $data(txt) <F11>       "Editor::CmdCompile $this ; break"
  bind $data(txt) <Control-KeyRelease> "::Editor::OnControlKey $this %K %k ; break"
  bind $data(f)   <Destroy>    +[list ::Editor::Destroy $this %W]
  Configure $this
  $data(f) bind <Control-F4> "$data(f) kill; break"
  #BalloonHelp para Sintaxis de funciones al escribir
  bind $data(txt) <parenleft>    [list ::Editor::AnalyzeCurrentText $this]
  bind $data(txt) <parenright>   [list destroy $data(txt).balloon]
  bind $data(txt) <BackSpace>    [list destroy $data(txt).balloon]
  bind $data(txt) <Return>       [list destroy $data(txt).balloon]
  bind $data(txt) <Escape>       [list destroy $data(txt).balloon]
  bind $data(txt) <Any-Leave>    [list destroy $data(txt).balloon]
  return $this
}

#pedro-start

#/////////////////////////////////////////////////////////////////////////////
proc ::Editor::AnalyzeCurrentText { this } {
#
#PURPOSE: Verifica si el texto escrito pertenece a alguna funcion definida
#         , se muestra la sintaxis y su descripcion como BalloonHlp.
#         Se activa cuando el usuario pulsa "(". El balloon creado se destruye
#         con backspace o ")". Ver Binds.
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  #Guardo posicion del cursor
  set origen  [$data(txt) index insert]
  set def     $origen
  set anatext " "
  #descompongo el index de origen en  partes
  set invariante [lindex [split $origen .] 0 ]
  set variante   [lindex [split $origen .] 1 ]
  set a 1;
  
  # Verifico que no estemos en el index n,0
  if { $variante == 0 } {
    return    
  }
  set def 1
  # Recorro hacia atras a partir del insert index para obtener index de la ultima
  # letra de la palabra inmediatamente detras del "("
  while { [expr ![regexp {[A-Za-z0-9]} $anatext]] && $variante > 0 && $def > 0} {  
    set def [expr $variante -$a]
    set anatext [$data(txt) get "$invariante.$def"]
    incr a    
  }
  # Encuentro la letra, obtener nombre de la funcion con wordstart y wordend.
  set ori  [$data(txt) index "$invariante.$def wordstart"]
  set end  [$data(txt) index "$invariante.$def wordend"]
  set word [$data(txt) get $ori $end]

  # procedimiento para establecer posicion del BalloonHelp bajo el Cursor
  set lineinfox     [lindex [$data(txt) dlineinfo $origen] 2]
  # hallo el total de caracteres en la linea con la segunda parte del insert index
  set totalchars [lindex [split [$data(txt) index "$invariante.end"] . ] 1]
  # Necesito convertir el indice del cursor en coordenadas de pantalla [pixelx pixely]
  # posicion del "(" MULT por largo de linea en pixeles ENTRE total de letras.
  # más posicion relativa de la ventana = posicion en X
  set posicionbichox [expr (round(($variante * $lineinfox) / $totalchars)) + \
                     ([winfo rootx $data(txt)])]
  
  # Posicion de linea + alto de linea + posicion relativa de la ventana = Posicion Y 
  set posicionbichoy [expr ([lindex [$data(txt) dlineinfo $origen] 1]) + \
                           ([lindex [$data(txt) dlineinfo $origen] 3]) + \
                           ([winfo rooty $data(txt)])]
  
  # Ejecuto un BalloonHlp usando los 3 ultimos parametros opcionales.
  ::Editor::BalloonHlp $this $data(txt) $word 1 $posicionbichox $posicionbichoy 1
  #Ayuda creada.
}


#///////////////////////////////////////////////////////////////////////////
proc ::Editor::BalloonHlp {this w arg {fixed 0} {posx 0} {posy 0} {special 0}} {
# PURPOSE: Look for Tol value of the compiled variable and display balloon text
#          If availiable. 
# ARGS   : Instance, Window, VarName
#///////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  #el segundo condicional impide que se cree el balloon si existe otro
  if { ($data(balloon) == 1 && [expr !([winfo exist $data(txt).balloon])]) || \
        $special == 1} {
   #creacion del toplevel para mostrar ayuda
   set definition [::TclInfoVar "Anything" $arg]
   #Continuamos mostrando solo si la variable tiene algo.
   if { [llength $definition] < 1 } {
      return 
    } else {
    set top $w.balloon
    catch {destroy $top}
    toplevel $top -bd 1 -bg black
    wm overrideredirect $top 1
    #obtengo info de la variable  
     set cont [lindex $definition end-2]
     set desc [lindex $definition end]
     set information "$arg: \n Valor: $desc \n Definicion: $cont"  
     #Crear Helpwindow
     font create Letra1 -family "Arial" -slant roman -size 8
     pack [message $top.txt -aspect 10000 -bg lightyellow \
         -font Letra1 -text $information]
     #Fixed 1 para invocado por text, Fixed 0 por defecto, obedece al raton.
     if { $fixed != 1} {
       #Si el balloon es invocado por el raton, obedecemos a posicion del raton.
       # A partir de la posicion del raton, modificamos vars para poner el toplevel
       # amarillo en un lugar diferente al cursor,  nunca sobre el.
        set wmx [winfo pointerx $w]
        set wmy [winfo pointery $w]
        incr wmx 40
        set wmy [expr $wmy - 40]
      } else {
         #Si el Balloon es invocado por el texto, obedecemos a la posicion del indice.
         set wmx $posx 
         set wmy $posy
      }
      #muestro
      wm geometry $top \
      [winfo reqwidth $top.txt]x[winfo reqheight $top.txt]+$wmx+$wmy
      raise $top
      font delete Letra1
    }
  }
}
#pedro-end


#///////////////////////////////////////////////////////////////////////////
  proc ::Editor::OnControlKey {this keysym keycode} {
#
# PURPOSE: Control the "Control + Key Press"
#
# PARAMETERS: this ->  Instance of editor
#             keysym -> The keysym corresponding to the event, 
#                       substituted as a textual string
#             keycode -> The keycode field from the event
#
#///////////////////////////////////////////////////////////////////////////
  #puts "PLATFORM: $::tcl_platform(platform)"
  switch [string toupper $keysym] {
    A { ::Editor::CmdOpen      $this }
    E { ::Editor::CmdSelectAll $this }
    F { ::Editor::CmdFind      $this }
    G { ::Editor::CmdSearch    $this }
    I { ::Editor::CmdIndent     $this }
    N { ::Editor::CmdNew       $this }
    R { ::Editor::CmdReplace   $this }
    S { ::Editor::CmdSave      $this }
    T { ::Editor::CmdFont      $this }
    U { ::Editor::CmdUnIndent   $this }
    X { ::Editor::CmdCut       $this }
    Y { ::Editor::CmdRedo      $this }
    Z { ::Editor::CmdUndo      $this }
    C { if { [string equal $::tcl_platform(platform) "unix"] } {
          ::Editor::CmdCopy $this
        } elseif { [string equal $::tcl_platform(platform) "windows"] } {
          switch $keysym {
            C {
              ::Editor::CmdCopy $this }
          }
        }
      }
    V { if { [string equal $::tcl_platform(platform) "unix"] } {
          ::Editor::CmdPaste $this
        } elseif { [string equal $::tcl_platform(platform) "windows"] } {
          switch $keysym {
            V {
              ::Editor::CmdPaste $this }
          }
        }
      }
  }
}  


#/////////////////////////////////////////////////////////////////////////////
  proc ::Editor::TracePosition {this name1 name2 op} {
#
# PURPOSE: Sets position of cursor and lines info in status bar each time var
#          'position' changes.
#
# PARAMETERS:
#   this  -> Instance of editor
#   name1, name2, op ->
#     Name1 and name2 give the name(s) for the variable being accessed: if the
#     variable is a scalar then name1 gives the variable's name and name2 is
#     an empty string; if the variable is an array element then name1 gives
#     the name of the array and name2 gives the index into the array; if an
#     entire array is being deleted and the trace was registered on the
#     overall array, rather than a single element, then name1 gives the array
#     name and name2 is an empty string. Name1 and name2 are not necessarily
#     the same as the name used in the trace variable command: the upvar
#     command allows a procedure to reference a variable under a different
#     name. Op indicates what operation is being performed on the variable,
#     and is one of r, w, or u. 
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  
  set pos [split $data(position) .]
  set text "[mc Lin]. [lindex $pos 0]  [mc Col]. [lindex $pos 1]"
  $data(sb,posi) configure -text $text
  set pos [split [Supertext::text_index $data(txt) end] .]
  $data(sb,nlin) configure -text "[lindex $pos 0] [mc lines]"
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::Editor::TraceNameFile {this name1 name2 op} {
#
# PURPOSE: Sets edited file name each time var 'nameFile' changes.
#
# PARAMETERS:
#   this  -> Instance of editor
#   name1, name2, op -> see above
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data

  if {$data(nameFile) != ""} {
    set data(labelWindow) $data(nameFile)
  } else  {
    set data(labelWindow) "untitled.tol"
  }
  $data(f) configure -title "$data(labelWindow) - [mc [list Bayes Editor]]"
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::Editor::TraceSaved {this name1 name2 op} {
#
# PURPOSE: Sets saved/not saved info in status bar each time var 'isSaved'
#          changes.
#
# PARAMETERS:
#   this  -> Instance of editor
#   name1, name2, op -> see above
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data

  if $data(isSaved) {
    $data(sb,modi) configure -text [mc "Saved"] -relief groove
    #$data(tb,save) configure -state disabled
  } else  {
    $data(sb,modi) configure -text [mc "Not Saved"] -relief raised
    #$data(tb,save) configure -state normal
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::Editor::TraceSelected {this name1 name2 op} {
#
# PURPOSE: Sets ??? each time var 'selected' changes.
#
# PARAMETERS:
#   this  -> Instance of editor
#   name1, name2, op -> see above
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  if $data(selected) {
#    $data(tb,cut) configure -state normal
#    $data(tb,copy) configure -state normal
  } else  {
#    $data(tb,cut)  configure -state disabled
#    $data(tb,copy) configure -state disabled
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::Editor::TraceCompiled {this name1 name2 op} {
#
# PURPOSE: Sets compiled/not compiled info in status bar each time var
#          'isCompiled' changes.
#
# PARAMETERS:
#   this  -> Instance of editor
#   name1, name2, op -> see above
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data

  if {$data(isCompiled) eq 2} {
    $data(sb,comp) configure -text [mc "Compiling"] -relief solid
    $data(tb,savecom) configure -state disabled
    $data(menu) entryconfigure 0 -state disabled
  } else  {
    $data(tb,savecom) configure -state normal
    $data(menu) entryconfigure 0 -state normal
    if {$data(isCompiled) eq 1} {
      $data(sb,comp) configure -text [mc "Compiled"] -relief groove
    } else  {
      $data(sb,comp) configure -text [mc "Not Compiled"] -relief raised
    }
  }
}



#/////////////////////////////////////////////////////////////////////////////
  proc ::Editor::TraceUndo {this name1 name2 op} {
#
# PURPOSE: Enables/disables undo button in toolbar each time value of undo
#          index changes. (see supertext.tcl)
#
# PARAMETERS:
#   this  -> Instance of editor
#   name1, name2, op -> see above
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  #if {[llength $Supertext::undo($data(txt,supertext))]} 
  if { $::Supertext::undoIndex($data(txt,supertext)) != -1 } {
#    $data(tb,undo) configure -state normal
  } else {
#    $data(tb,undo) configure -state disabled
  }
}

proc ::Editor::InsertText {this new_text {pos end} } {
  upvar \#0 ${this}::data data
  $data(txt) insert $pos $new_text
}

proc ::Editor::GetText {this} {
  upvar \#0 ${this}::data data
  return $data(txt,ctext)
}

proc ::Editor::PreProc {this args} {
  upvar \#0 ${this}::data data
  set command [lindex $args 0]
  switch $command {
    insert {
      set data(isSaved) 0
      set data(position) [Supertext::text_index $data(txt) insert]
    }
    delete {
      set data(isSaved) 0
      set data(position) [Supertext::text_index $data(txt) insert]
    }
    tag {
      if {[string equal [lindex $args 2] "sel"]} {
        switch [lindex $args 1] {
          add     {set data(selected) 1}
          remove  {set data(selected) 0}
        }
      }
    }
    mark {
      if {[string equal [lindex $args 2] "anchor"]} {
        set data(position) [Supertext::text_index $data(txt) insert]
      }
    }
    see {
      set data(position) [Supertext::text_index $data(txt) insert]
    }
    undo {
      set data(position) [Supertext::text_index $data(txt) insert]
    }
  }
  #Tolcon_Trace $args
}


#/////////////////////////////////////////////////////////////////////////////
# File management commands (new, open, save, save as...)
#/////////////////////////////////////////////////////////////////////////////

#/////////////////////////////////////////////////////////////////////////////
  proc ::Editor::CmdNew {this} {
#
# PURPOSE: Cleans editor to edit a new white document
#
# PARAMETERS:
#   this  -> Instance of editor
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data

  if [string equal [CheckOpen $this] "cancel"] return
  $data(txt) delete 1.0 end
  InitState $this new
  InsertFileHeader $this
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::Editor::UpdateListEdited {path} {
#
# PURPOSE: Updates list of last edited files adding a new file path.
#
# PARAMETERS:
#   path  -> path of the last edited file
#
#/////////////////////////////////////////////////////////////////////////////
  variable options
  # guardamos el ultimo directorio abierto
  set options(lastDir) [file dirname $path]
  if { [set idx [lsearch $options(edited) $path]] == -1 } {
    set options(edited) [linsert $options(edited) 0 $path]
  } else {
    set ll ""
    lappend ll $path
    set i 0
    foreach f $options(edited) {
      if  {$i != $idx} {
        lappend ll $f
      }
      incr i
    }
    set options(edited) $ll
  }
  set options(edited) [lrange $options(edited) 0 [expr $options(numEdited)-1]]  
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::Editor::CmdOpen {{this ""}} {
#
# PURPOSE: Asks for a file path and if it's given opens it into the editor
#          window.
#
# PARAMETERS:
#   this -> Instance of editor
#
#/////////////////////////////////////////////////////////////////////////////
  variable options
  variable typelist
  variable defExt

  if {$this == ""} {
    set parent .
  } else  {
    upvar \#0 ${this}::data data
    set parent $data(txt)
    if [string equal [CheckOpen $this] "cancel"] return
  }

  set path [tk_getOpenFile -title [mc "Open"] \
              -parent $parent -initialdir $options(lastDir) \
              -defaultextension $defExt -filetypes $typelist]

  if {$path != ""} {
    # abrimos solo si no esta ya abierto
    foreach ins [namespace children ::Editor] {
      upvar \#0 ${ins}::data data
      if {[string equal $data(nameFile) $path]} {
        $data(f) raise
		set resp [tk_messageBox -type ok -icon warning \
		  -message [mc "The file is already opened!"] ]
        return
      }
    }
    if {$this == ""} {
      set this [New $path]
    }
    GetFromFile $this $path
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::Editor::GetFromFile {this path {run 0}} {
#
# PURPOSE: Loads a file into the editor window.
#
# PARAMETERS:
#   this -> Instance of editor
#   path -> file's path
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data

  $data(txt) delete 1.0 end
  if [::BayesText::Open $data(txt) $path] {
    set data(nameFile) $path
    set data(mTime) [file mtime $path]
    UpdateListEdited $path
    # Guardamos en el ini por si se cae el interface para que guarde
    # la lista de archivos recientes
    WriteIni $this
  } else {
    set data(nameFile) ""
  }
  $data(f) raise
  InitState $this open
  tk::TextSetCursor $data(txt) 1.0
  if { $run } {
    CmdSaveCompile $this
  }
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::Editor::CheckAll {} {
#
# PURPOSE: Checks for files opened if anyone has changed and asks for saving.
#
#/////////////////////////////////////////////////////////////////////////////
  foreach ins [namespace children ::Editor] {
    if {[CheckOpen $ins] eq "cancel"} {
      return cancel
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::Editor::CheckOpen {this {type "yesnocancel"}} {
#
# PURPOSE: Checks if a file has changed and asks for saving.
#
# PARAMETERS:
#   this -> Instance of editor
#   type -> type of messageBox to ask. (yesnocancel by default)
#
# RETURN: Returns user answer for saving.
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
    
  # comprobamos si necesita ser guardado
  if { !$data(isSaved)} {
    if {$data(nameFile) != ""} {
      set name $data(nameFile)
    } else  {
      set name "untitled.tol"
    }
    # preguntamos por conformidad
    set parent $data(txt)
    set rq [tk_messageBox -type $type -icon warning -title [mc "Warning"]\
            -parent $parent -message "[mc "File %1\$s has changed!" $name].\n\
            [mc "Do you want to save changes?"]"]
    switch -exact $rq {
      yes {
        if {$data(nameFile) != "" } {
          FileSave $this $data(nameFile)
          set data(mTime) [file mtime $data(nameFile)]
        } else {
          CmdSaveAs $this $data(nameFile)
          set data(mTime) [file mtime $data(nameFile)]
        }
      }
      cancel {return cancel}
      default {return ok}
    }
  }
  return ok
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::Editor::CmdSave {this} {
#
# PURPOSE: Saves text actually opened in the editor. If doesn't have a name, 
#          asks for ti (CmdSaveAs).
#
# PARAMETERS:
#   this -> Instance of editor 
#
# RETURN: Returns 1 if file has been saved correctly or 0 if not.
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data

  if {$data(nameFile) == ""} {
    set ok [CmdSaveAs $this]
  } else  {
    if { $data(mTime) eq [file mtime $data(nameFile)] } {
      set ok [FileSave $this $data(nameFile)]
      set data(mTime) [file mtime $data(nameFile)]
    } else {
      set parent $data(txt)
      set msg "[mc File] $data(nameFile) [mc {has been changed externally!}] \
               \n[mc {Do you want to rewrite it and save new changes?}]"
      set rq [tk_messageBox -type okcancel -icon warning -title [mc "Warning"]\
            -parent $parent -message $msg]
      set ok -1
      switch -exact $rq {
        ok {
          set ok [FileSave $this $data(nameFile)]
          # updates the modification time
          set data(mTime) [file mtime $data(nameFile)]
        }
      }
    }
  }
  return $ok
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::Editor::CmdSaveAs {this {nf ""}} {
#
# PURPOSE: Saves text actually opened in editor asking for the user to select
#          a file to save it.
#
# PARAMETERS:
#   this -> Instance of editor 
#   nf   -> name of file proposed
#
# RETURN: Returns 1 if file has been saved correctly or 0 if not.
#
#/////////////////////////////////////////////////////////////////////////////
  variable options
  variable typelist
  variable defExt
  upvar \#0 ${this}::data data
    
  if {$nf == ""} {
    set dir $options(lastDir)
    set fil ""     
  } else  {
    set dir "[file dirname $nf]"
    set fil "[file tail $nf]"    
  }
  set path [tk_getSaveFile -title [mc "Save as"] -initialdir $dir \
       -initialfile $fil -parent $data(txt) \
       -defaultextension $defExt -filetypes $typelist]
  if {![string equal $path ""] } {
    set ok [FileSave $this $path]
    set data(mTime) [file mtime $data(nameFile)]
  } else {
    set ok 0
  }
  return $ok
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::Editor::FileSave { this path } {
#
# PURPOSE: Saves text actually opened in editor in a text file.
#
# PARAMETERS:
#   this -> Instance of editor 
#   path -> name of file 
#
# RETURN: Returns 1 if file has been saved correctly or 0 if not.
#
#/////////////////////////////////////////////////////////////////////////////
  variable options
  upvar \#0 ${this}::data data

  if [::BayesText::Save $data(txt) $path] {
    set data(nameFile) $path
    InitState $this save
    # guardamos el ultimo directorio utilizado
    set options(lastDir) [file dirname $path]
    UpdateListEdited $path
    set ok 1
  } else  {
    set ok 0
  }
  return $ok
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::Editor::CmdTkcvs { this mode} {
#
# PURPOSE: Execute CVS utilities
#
# PARAMETERS:
#   this -> Instance of editor 
#   mode -> type of execution (0 -> tkCvs, 1 -> tkdiff)
#
#/////////////////////////////////////////////////////////////////////////////
  global toltk_bin
  global tkcvs_library
  
  variable options
  upvar \#0 ${this}::data data
  
  if {$mode == 0} {
    set cmd "[file join $toltk_bin wish84.exe] [file join $tkcvs_library tkcvs.tcl]"
    #exec c:/TclTk/bin/wish84.exe c:/TclTk/bin/tkcvs.tcl -dir $options(lastDir) &
    exec $cmd -dir $options(lastDir) &
  } else {
    set cmd "[file join $toltk_bin wish84.exe] [file join $tkcvs_library tkdiff.tcl]"
    #exec c:/TclTk/bin/wish84.exe c:/TclTk/bin/tkdiff.tcl $data(nameFile) &
    exec $cmd $data(nameFile) &
  }

  
}

#/////////////////////////////////////////////////////////////////////////////
# Comandos de edición (deshacer, cortar, copiar, pegar, buscar, remplazar,...)
#/////////////////////////////////////////////////////////////////////////////
proc ::Editor::CmdUndo {this} {
  upvar \#0 ${this}::data data
  $data(txt) undo
}

proc ::Editor::CmdRedo {this} {
  upvar \#0 ${this}::data data
#  $data(txt) redo
  Tolcon_Trace "---------------------------"
}

proc ::Editor::CmdCut {this} {
  upvar \#0 ${this}::data data
  ::BayesText::Cut $data(txt)
}

proc ::Editor::CmdCopy {this} {
  upvar \#0 ${this}::data data
  ::BayesText::Copy $data(txt)
}

proc ::Editor::CmdPaste {this} {
  upvar \#0 ${this}::data data
  ::BayesText::Paste $data(txt)
}

proc ::Editor::CmdSelectAll {this} {
  upvar \#0 ${this}::data data
  ::BayesText::SelectAll $data(txt)
}

proc ::Editor::CmdHelpEditor {this} {
    upvar \#0 ${this}::data data
    ShowHelp Tb4Eda
}

proc ::Editor::CmdFind {this} {
  upvar \#0 ${this}::data data
  ::BayesText::FindShow $data(txt,ctext)
}

proc ::Editor::CmdSearch {this} {
  upvar \#0 ${this}::data data
  :::BayesText::SearchShow  $data(txt)
}

proc ::Editor::CmdFindNext {this {txt ""}} {
  upvar \#0 ${this}::data data
  eval ::BayesText::FindShow $data(txt,ctext) 1 {$txt}
}

proc ::Editor::CmdReplace {this} {
  upvar \#0 ${this}::data data
  ::BayesText::ReplaceShow $data(txt,ctext)
}

proc ::Editor::CmdIndent {this} {
  upvar \#0 ${this}::data data
  variable options
  ::BayesText::Indent $data(txt,ctext) $options(charsIndent)
}

proc ::Editor::CmdUnIndent {this} {
  upvar \#0 ${this}::data data
  variable options
  ::BayesText::UnIndent $data(txt,ctext) $options(charsIndent)
}


#/////////////////////////////////////////////////////////////////////////////
# Comandos 
#/////////////////////////////////////////////////////////////////////////////
proc ::Editor::CmdFont {this} {
  variable options
  global toltk_options
  upvar \#0 ${this}::data data
  set fnt [::BayesText::FontChange $data(txt)]
  if {$fnt != ""} {
    set options(font) $fnt
    font configure ctfnBold   -family [lindex $fnt 0] -size [lindex $fnt 1]
    font configure ctfnNormal -family [lindex $fnt 0] -size [lindex $fnt 1]
    UpdateMarkPos $this [winfo height $data(txt)]
  }
}

proc ::Editor::CmdPrint {this} {
  upvar \#0 ${this}::data data
  global tcl_platform
  if { [string equal $tcl_platform(platform) "windows"] } {
    #int PrintCmd(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
#    printer open
#    printer styledialog
#    printer jobdialog
#    puts "print from [printer getattr firstpage] to [printer getattr lastpage]"
#    printer opendoc
#    printer openpage
#    printer test   ; # funcion que usa para imprimir
#    printer text [$data(txt) get 1.0 end]
#    printer closepage
#    printer closedoc
#    printer close
  }
}

proc ::Editor::CmdSaveCompile {this} {
  upvar \#0 ${this}::data data

  #Si se está compilando no hago nada
  if {$data(isCompiled) eq 2} { return }
  set ok [CmdSave $this]
  if {$ok} {
    CmdCompileFile $this
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::Editor::CmdCompileFile {this} {
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  #Si se está compilando no hago nada
  if {$data(isCompiled) eq 2} { return }
  $data(tb,balloon) configure -state disabled
  #set check [::BayesText::TolFileSyntaxCheck $data(nameFile)]
  set check 1
  if {$check} { 
    set data(isCompiled) 2
    ::TolConsole::Include $data(nameFile)
    set data(isCompiled) 1
    
    #pedro-start
    # Mapear contenido de texto de la instancia y generar ayudas dinamicas
    # Puede tomar entre 5 y 10 segundos
    #::Editor::VariableMap $this
    set data(balloon) 1
    ::Editor::CmdBalloon $this
    update idletasks
    #pedro-fin
    
  } else  {
    # hay errores
    tk_messageBox -type ok -icon warning -title [mc Editor] -parent $data(txt) \
            -message "[mc {Syntax errors in file}]\n\
                      [mc {Look over output window}]"
  }
}

#pedro-start
#/////////////////////////////////////////////////////////////////////////////
proc ::Editor::wordindexes {s} {
# PURPOSE: Divide un texto en palabras, devuelve los indices de inicio y final de
#          cada palabra.
#          Subtract whole words from a paragraph, returns a pair of index values
#          indicating where the word starts and where it ends.
#
# ARGS:    user defined text widget
#/////////////////////////////////////////////////////////////////////////////
    set i 0
    set res {}
    foreach c [split $s ""] {
        if {$c!=" " && $i==[string wordstart $s $i] && [regexp {[A-Za-z]} $c]} {
            lappend res $i [string wordend $s $i]
        }
        incr i
    }
    set res
 }

#/////////////////////////////////////////////////////////////////////////////
proc ::Editor::VariableMap {this} {
# PURPOSE: Creates a tag inside the editor instance for each word in the tol text
#          excluding comment lines, virtually fragmenting the editor window in
#          about 6000 - 15000 pieces, depends on the amount of words.
#          Bind two mouse events to each tag created using  <Enter> and <Leave>,
#          this way we call the process BalloonHlp to display the help text.
#/////////////////////////////////////////////////////////////////////////////
#          Creamos un tag por cada palabra que se encuentre dentro del texto tol
#          excluyendo las lineas comentadas. A cada rango encontrado se le
#          hace un Bind a dos eventos del raton; <enter> y <leave> para entonces
#          llamar a BalloonHlp y mostrar la ayuda.
#
# ARGS:    Instance of Editor / Instancia del Editor
#
#
# TODO:   1) Tol is marvelous, did you know you can actually create 2 variables
#          with identical names differing only in the grammar type?.. example..
#          Real a1 = 2;
#          Set  a1 = SetOfText("","");
#          (This will create 2 separate variables with the same name.. amazing huh?)
#          This is PERFECLTY possible in our amazing language, wich SUCKS for
#          the purpuse of this procedure... that is, because analyzed text is
#          arbitrary. With no reading rules, i can't really determine the
#          variable type before calling BalloonHlp. If two variables happen to
#          have the same name, the ::TclInfoVar loop function will catch the info
#          on the last valid grammar type in the checklist (See Editor::BalloonHlp 
#          for more details).
#          
#          The idea would be to determine the grammar type before calling BalloonHlp.
#          I ain't genious enought to take it that far out.... yet. :-D
#
#         2) I managed to exclude comment lines from being parsed as valid tags.
#            If you can manage to exclude text or quote delimited expressions
#            from the loop, you're my geek... and offcourse, you defenetly have
#            more free time than me. Good Luck.
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  # Luego de compilar, obtengo la lista de variables creadas.
  # "const" sera lista de palabras que nunca seran variables
  set const [list Anything Code Complex CDate CTimeSet Date Matrix Polyn \
                  Ratio Ration Real Serie Set Special Struct Text TimeSet ]
    
  #Obtener lista de variables y funciones ya compiladas desde editor
  #controla el numero de linea mientras recorre el txt del editor
  set linenum 0
    
  foreach line [split [$data(txt) get 1.0 end-1c] \n] {
    incr linenum 1
    if { [string range $line 0 1] != "//" && [regexp "//" $line] == "0" } {
      foreach {from to} [::Editor::wordindexes $line] {
        #guardar palabra e indices
        set word [string range $line $from [expr $to-1]]
        set verif [lsearch -exact $const "$word"]
        if  { $verif == "-1" } {
          #solo se crea tag si no pertenece a ningun nombre de variable
          set num [expr rand()]
          #numero aleatorio para complementar nombres de tags repetidos
          set to [expr $to-1]
          #crear tag con nombre y rango dentro del text
          $data(txt) tag add "$word$num" "$linenum.$from" "$linenum.$to"
          #Crear bind a eventos para cada tag creado
          $data(txt) tag bind "$word$num" <Enter> "after 1500 [list ::Editor::BalloonHlp $this $data(txt) $word]"
          $data(txt) tag bind "$word$num" <Leave> "destroy $data(txt).balloon"
          #Subrayo las variables Definidas
          #if { [llength [::TclInfoVar "Anything" "$word" ]] > 0 } {
          #$data(txt) tag configure "$word$num" -underline 1 }
          update idletasks
        }
      }
    }
  }
  # - fin
}


#/////////////////////////////////////////////////////////////////////////////
proc ::Editor::CmdCompile {this} {
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data

  set idxs [$data(txt) tag ranges sel]
  Tolcon_Trace "idxs = $idxs"
  # Si hay seleccion evaluo texto seleccionado, sino incluyo el archivo
  if [llength $idxs] {
    ::TolConsole::EvalExpr [$data(txt) get [lindex $idxs 0] [lindex $idxs 1]]
  } else  {
    CmdSaveCompile $this
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::Editor::CmdDecompile {this} {
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::data data
  if {$data(isCompiled) eq 1} {
    ::TolInspector::DecompileFile $data(nameFile)
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::Editor::CmdSyntaxCheck {this} {
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::data data
  ::BayesText::TolSyntaxCheck $data(txt) [ file dir $data(nameFile) ]
}


#/////////////////////////////////////////////////////////////////////////////
proc ::Editor::CmdRefresh {this} {
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::data data

  foreach tag [$data(txt) tag names] {
    $data(txt) tag remove $tag 1.0 end
  }
  update idletasks
  $data(txt) highlight 1.0 end
}


#/////////////////////////////////////////////////////////////////////////////
proc ::Editor::CmdRightMargin {this} {
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::data data
  UpdateMarkPos $this [winfo height $data(txt)]
}


#/////////////////////////////////////////////////////////////////////////////
proc ::Editor::CmdOptions {this} {
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::data data
  OptionsCreate $this $data(txt)
}


#/////////////////////////////////////////////////////////////////////////////
proc ::Editor::CmdBalloon {this} {
# PURPOSE: Balloon Control
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  
  if {  $data(balloon) != 1 } {
    #Reconfigure
    $data(tb,balloon) configure -state disabled
     #Recalculate values
     ::Editor::VariableMap $this
     #flag
     set data(balloon) 1
     #reconfigure
     $data(tb,balloon) configure -image [::Bitmap::get "balloon_enable"]
    $data(tb,balloon) configure -state normal
  } else {
     #destroy tag & binds
     $data(tb,balloon) configure -state disabled
       set tagnames [$data(txt) tag names]
       set limit [llength $tagnames]
       for {set x 6} {$x < $limit} {incr x} {
        $data(txt) tag delete [lindex $tagnames $x]
        #puts "tag eliminada"
       }
       set data(balloon) 0
       $data(tb,balloon) configure -image [::Bitmap::get "balloon_disable"]
      $data(tb,balloon) configure -state normal
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::Editor::OptionsCreate {this path} {
#/////////////////////////////////////////////////////////////////////////////  
  set dialog [Dialog $path.d -title [mc "Editor Options"]\
            -modal local -parent $path]
  wm resizable $dialog 0 0
  wm protocol $dialog WM_DELETE_WINDOW [list ::Editor::OptionsDestroy $this $dialog]
  set w [$dialog getframe]
  set w_tabset [::blt::tabset $w.ts -relief flat -highlightthickness 0\
            -bd 0 -outerpad 0 -tier 2 -slant right -textside right ]
    
  #insertar tabset
  $w_tabset insert end Headers Others
  set f1 [frame $w_tabset.f1]
  set f2 [frame $w_tabset.f2]
    
    
  $w_tabset tab configure "Headers" -text [mc "Headers"] -window $f1\
          -fill both -padx 0 -selectbackground \#d9d9d9 -bg gray75
  $w_tabset tab configure "Others" -text [mc "Others"] -window $f2\
          -fill both -padx 0 -selectbackground \#d9d9d9 -bg gray75
    
  OptionsGet $this
  OptionsCreateHeaders $this $f1
  OptionsCreateOthers  $this $f2
    
  OptionsCheck $this
    
  set w_bbox \
    [ButtonBox $w.bb -orient horizontal -spacing 10 -padx 0 -pady 0 \
     -homogeneous false]
  $w_bbox add -text [mc "Accept"] -relief link \
    -helptext [mc "OK"] \
    -image [::Bitmap::get accept] -compound left -padx 5 -pady 1 \
    -command [list ::Editor::OptionsOK $this $dialog]
  $w_bbox add -text [mc "Cancel"] -relief link \
    -helptext [mc "Cancel"]\
    -image [::Bitmap::get cancelx] -compound left -padx 5 -pady 1 \
    -command [list ::Editor::OptionsDestroy $this $dialog]
  $w_bbox add -text [mc "Apply"] -relief link \
    -helptext [mc "Apply"]\
    -image [::Bitmap::get apply] -compound left -padx 5 -pady 1 \
    -command [list ::Editor::OptionsDestroy $this $dialog]
  
  pack $w_bbox   -fill x -expand false -side bottom
  pack $w_tabset -fill both -expand true
  
  bind $dialog <F1> "ShowHelp Tb4Eda ; break"    
  $dialog draw
}


#/////////////////////////////////////////////////////////////////////////////
proc ::Editor::OptionsCreateHeaders {this w} {
#/////////////////////////////////////////////////////////////////////////////  
  variable tmpOpt
    
  TitleFrame $w.tf1 -text [mc "Files of headers"]
  set f1 [$w.tf1 getframe]
    
  checkbutton $f1.chFileH -text [mc "Insert header in new files"]\
            -variable ::Editor::tmpOpt(var,chFileH)\
            -command [list ::Editor::OptionsCheckHeaderFileInsert $this]
  label  $f1.lFileH -text [mc "Files Header"]
  set tmpOpt(widget,eFileH) [entry  $f1.eFileH -width 50\
                  -textvariable ::Editor::tmpOpt(var,fileH)]
  set tmpOpt(widget,bFileH) [::Objects::ChooseFileCreate ::Editor::tmpOpt(var,fileH) $f1.bFileH]

  label  $f1.lFuncH -text [mc "Functions Header"]
  entry  $f1.eFuncH -width 50 -textvariable ::Editor::tmpOpt(var,funcH)
  ::Objects::ChooseFileCreate ::Editor::tmpOpt(var,funcH) $f1.bFuncH
    
   
  frame $f1.f -width 25
    
  grid $f1.f $f1.chFileH -          -          -sticky nw -padx 2
  grid ^     $f1.lFileH  $f1.eFileH $f1.bFileH -sticky nw -padx 2
  grid ^     $f1.lFuncH  $f1.eFuncH $f1.bFuncH -sticky nw -padx 2
  grid rowconfigure    $f1 0 -weight 1
  grid columnconfigure $f1 3 -weight 1
    
  grid $w.tf1 -sticky news -padx 2
    
  grid rowconfigure    $w 1 -weight 1
  grid rowconfigure    $w 2 -weight 2
  grid columnconfigure $w 0 -weight 1    
}


#/////////////////////////////////////////////////////////////////////////////
proc ::Editor::OptionsCreateOthers {this w} {
#/////////////////////////////////////////////////////////////////////////////  
  variable tmpOpt
    
  TitleFrame $w.tf1 -text [mc "General Options"]
  set f1 [$w.tf1 getframe]
    
  checkbutton $f1.chEdited -text [mc "Files edited recently"]\
          -variable ::Editor::tmpOpt(var,chEdited)\
          -command [list ::Editor::OptionsCheckEdited $this]
    
  set tmpOpt(widget,edited) [SpinBox $f1.sbEdited -justify right -width 4 \
          -editable 0 -range {0 40} \
          -textvariable ::Editor::tmpOpt(var,numEdited)]
  label $f1.lFiles -text [mc files]
  
  checkbutton $f1.chLine -text [mc "Show line at"] \
          -variable ::Editor::tmpOpt(var,chLine)
  SpinBox $f1.sbLinePos -justify right -width 4 \
          -editable 0 -range {20 100} \
          -textvariable ::Editor::tmpOpt(var,posLine)
  label $f1.lChars -text [mc characters]
  
  label $f1.lCharsIndent -text [mc "Characters to indent"]
  SpinBox $f1.sbCharsIndent -justify right -width 4 \
          -editable 0 -range {1 8} \
          -textvariable ::Editor::tmpOpt(var,charsIndent)

  frame $f1.f -width 25
    
  grid $f1.f $f1.chEdited     $f1.sbEdited     $f1.lFiles -sticky nw -padx 2
  grid ^     $f1.chLine       $f1.sbLinePos    $f1.lChars -sticky nw -padx 2
  grid ^     $f1.lCharsIndent  $f1.sbCharsIndent         -sticky nw -padx 2
  grid rowconfigure    $f1 2 -weight 1
  grid columnconfigure $f1 3 -weight 1
    
  grid $w.tf1 -sticky news -padx 2
    
  grid rowconfigure    $w 1 -weight 1
  grid rowconfigure    $w 2 -weight 2
  grid columnconfigure $w 0 -weight 1
}


#/////////////////////////////////////////////////////////////////////////////
proc ::Editor::OptionsGet {this} {
#/////////////////////////////////////////////////////////////////////////////  
  variable options
  variable tmpOpt
  set tmpOpt(var,chEdited)   $options(chEdited)
  set tmpOpt(var,numEdited)  $options(numEdited)
  set tmpOpt(var,chFileH)    $options(chFileH)
  set tmpOpt(var,fileH)      $options(fileH)
  set tmpOpt(var,funcH)      $options(funcH)
  set tmpOpt(var,chLine)     $options(chLine)
  set tmpOpt(var,posLine)    $options(posLine)
  set tmpOpt(var,charsIndent) $options(charsIndent)
}


#/////////////////////////////////////////////////////////////////////////////
proc ::Editor::OptionsSet {this} {
#/////////////////////////////////////////////////////////////////////////////  
  variable options
  variable tmpOpt
    
  regsub -all var, [array names ::Editor::tmpOpt var,*] "" optNames
  #Tolcon_Trace "$optNames"
  foreach it $optNames {
    set options($it) $tmpOpt(var,$it)
  }  
}


#/////////////////////////////////////////////////////////////////////////////
proc ::Editor::OptionsCheck {this} {
#/////////////////////////////////////////////////////////////////////////////  
  OptionsCheckEdited           $this
  OptionsCheckHeaderFileInsert $this 
}


#/////////////////////////////////////////////////////////////////////////////
proc ::Editor::OptionsCheckEdited {this} {
#/////////////////////////////////////////////////////////////////////////////  
  variable tmpOpt
  if {!$tmpOpt(var,chEdited)} {      
    $tmpOpt(widget,edited) configure -state disabled
  } else  {
    $tmpOpt(widget,edited) configure -state normal
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::Editor::OptionsCheckHeaderFileInsert {this} {
#/////////////////////////////////////////////////////////////////////////////  
  variable tmpOpt
  if {!$tmpOpt(var,chFileH)} {
    $tmpOpt(widget,eFileH) configure -state disabled
    $tmpOpt(widget,bFileH) configure -state disabled
  } else  {
    $tmpOpt(widget,eFileH) configure -state normal
    $tmpOpt(widget,bFileH) configure -state normal
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::Editor::OptionsDestroy {this dialog} {
#/////////////////////////////////////////////////////////////////////////////  
  variable tmpOpt
  array unset tmpOpt
  destroy $dialog
}


#/////////////////////////////////////////////////////////////////////////////
proc ::Editor::OptionsApply {this} {
#/////////////////////////////////////////////////////////////////////////////  
  OptionsSet $this
  CmdRightMargin $this
}


#/////////////////////////////////////////////////////////////////////////////
proc ::Editor::OptionsOK {this dialog} {
#/////////////////////////////////////////////////////////////////////////////  
  OptionsApply   $this
  OptionsDestroy $this $dialog
}


#/////////////////////////////////////////////////////////////////////////////
# INIFILE 
#/////////////////////////////////////////////////////////////////////////////


#/////////////////////////////////////////////////////////////////////////////
proc ::Editor::ReadIni {} {
#/////////////////////////////////////////////////////////////////////////////  
  variable options
  global toltk_options

  set rini ::iniFile::Read
  set options(chFileH)    [$rini Editor insertFileH 0]
  set options(fileH)      [$rini Editor fileH      ""]
  set options(funcH)      [$rini Editor funcH      ""]
  set options(chEdited)   [$rini Editor saveRecent  1]
  set options(numEdited)  [$rini Editor numRecent   8]
  set options(edited)     [$rini Editor recent      {}]
  set options(font)       [$rini Editor font     $toltk_options(fonts,Fixed)]
  set options(lastDir)    [$rini Editor lastDir  ""]
  set options(chLine)     [$rini Editor chLine    1]
  set options(posLine)    [$rini Editor posLine  78]
  # ticket https://www.tol-project.org/ticket/1168
  set cindent [$rini Editor charsIndent -1] 
  if { $cindent == -1 } {
    # no aparecia la clave charsIndent, pregunto por la vieja
    set options(charsIndent) [$rini Editor charsIdent 2]
  } else {
    # aparecia la clave, hay que pasar al nombre Indent
    set options(charsIndent) $cindent
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::Editor::WriteIni {this} {
#
# PURPOSE: Write common Editor options to .ini file
#
# PARAMETERS:
#   this  -> Instance of editor
#
#/////////////////////////////////////////////////////////////////////////////
  variable options
  upvar \#0 ${this}::data data
  set wini ::iniFile::Write

  $wini Editor insertFileH  $options(chFileH)
  $wini Editor fileH        $options(fileH)
  $wini Editor funcH        $options(funcH)
  $wini Editor saveRecent   $options(chEdited)
  $wini Editor numRecent    $options(numEdited)
  $wini Editor recent       $options(edited)
  $wini Editor font         $options(font)
  $wini Editor lastDir      $options(lastDir)
  $wini Editor chLine       $options(chLine)
  $wini Editor posLine      $options(posLine)
  $wini Editor charsIndent   $options(charsIndent)
  ::iniFile::Flush
}

::Editor::ReadIni
