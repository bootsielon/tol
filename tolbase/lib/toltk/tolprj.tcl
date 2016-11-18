#/////////////////////////////////////////////////////////////////////////////
# FILE    : tolprj.tcl
# PURPOSE : This file contains a namespace called TolProject with its
#           functions. It's for managing TOL projects that are groups of TOL
#           objects (text files, table files, etc.). They are showed as a 
#           hierarchical treeview.
#/////////////////////////////////////////////////////////////////////////////

package require bfunctions

#/////////////////////////////////////////////////////////////////////////////
namespace eval TolProject {
#
# PURPOSE: The purpose of this namespace is organize code and functions and
#          manage common options for all tol project editors such as last
#          edited files, last directory used, etc.
#          A special namespace is created for each instance of project.
#
# VARIABLES:
#   options -> common options for all project windows:
#     wm,state  -> initial window project state
#     geometry  -> initial window project geometry
#     lastDir   -> last directory used
#     chEdited  -> boolen for saving last project files path
#     numEdited -> number of last project files path to save
#     edited    -> path of last used projects
#   tmpOpt      -> temporary array used to store the widgets and variables of
#              the options dialog.
#   num_Project -> incrementally number to identify each project window
#   typelist    -> list of type files supported by project editor
#   defExt      -> default extension for files managed by project editor
#   num         -> incrementally number to identify each item in the 
#                 [Included TOL Files] section of project file
#
#/////////////////////////////////////////////////////////////////////////////
  variable options
  variable tmpOpt
  variable num_Project 0
  variable typelist {{"Project" {".prj"}}}
  variable defExt .prj
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::ComputeFullPath {this path} {
#
# PURPOSE: Prepends name of project base directory if necessary to a given 
#          path.
#          This will be necessary in case path is not an absolute path.
#
# PARAMETERS:
#   this  -> instance of tol project
#   path  -> path to which name of project base directory is to be prepended.
#
# RETURN: The path modified or not as necessary.
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data

  if {[file pathtype $path] == "absolute"} {
    return $path
  } else {
    return [file join $data(baseDir) $path]
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::New {{path ""}} {
#
# PURPOSE: Creates a new project window and an instance of it. This also 
#          initializes the instance and register the window in the project.
#
# PARAMETERS:
#   path  -> path of project to use. If it isn't specified a new white
#            project is created.
#
# RETURN: A instance of TolProject.
#
#/////////////////////////////////////////////////////////////////////////////
  variable num_Project
  incr num_Project

  set this [Create .proy$num_Project]
  SetTraces $this
  InitState $this new
  update
  if {$path == ""} {
    CmdNew $this
  }
  return $this
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::Open {{path ""} {run 0}} {
#
# PURPOSE: Open a project file in a project window. If file is already opened
#          it raises its window, else it creates a new tol project window and
#          an instance of it.
#
# PARAMETERS:
#   path  -> path of file to open. If it isn't specified it ask for it.
#   run   -> indicates if the project must be compiled.
#
# RETURN: A instance of tol project.
#
#/////////////////////////////////////////////////////////////////////////////
  if {$path != ""} {
    #Comprobar si el archivo ya está abierto y si es así hacer un raise
    if { ![ catch { set lst [namespace children ::TolProject] } msgerr] } {
      foreach ins $lst {
        upvar \#0 ${ins}::data data
        if {[string equal "$data(baseDir)/$data(nameFile)" $path]} {
          #wm deiconify $data(f)
          raise $data(f)
          return $ins
        }
      }
      update
    }
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
proc ::TolProject::postEdited {m {ord 0}} {
#
# PURPOSE: Fills a menu with the last used project files, according to the
#          information of 'options' variable.
#
# PARAMETERS:
#   m   -> menu's path that will have the list of last used projects
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
      $m add command -label $w -command [list ::TolProject::Open $w]
    }
    if [llength $lst] {
      $m add separator
      $m add command -label [mc "Clear"]\
           -command [list ::TolProject::clearEdited]
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::clearEdited {} {
#
# PURPOSE: Clears the list of last edited files
#
#/////////////////////////////////////////////////////////////////////////////
  variable options
  set options(edited) ""
  ::iniFile::Write TolProject recent       $options(edited)
  ::iniFile::Flush
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::OnDelete {this} {
#
# PURPOSE: Procedure to check things before destroying tol project window
#
# PARAMETERS:
#   this  -> Instance of tol project 
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
proc ::TolProject::Destroy {this w} {
#
# PURPOSE: Deletes the instance of the tol project. Before this, writes the 
#          ini file.
# 
# PARAMETERS:
#   this  -> Instance of tol project 
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  if [string equal $data(f) $w] {
    WriteIni $this
    namespace delete $this
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::InitState {this from} {
#
# PURPOSE: Initializes some variables according to the state of the tol
#          project.
#
# PARAMETERS:
#   this  -> Instance of tol project 
#   from  -> State that defines values for the variables
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data

  switch $from {
    new {
      set data(compiling) 0
      set data(isSaved) 1
      set data(nameFile) ""
      set data(baseDir) ""
      set data(lastOpenedNodeDir) ""
    }
    open {
      set data(compiling) 0
      set data(isSaved) 1
#      set data(nameFile) ""
    }
    save {
      set data(isSaved) 1
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::SetTraces {this} {
#
# PURPOSE: Sets some traces of variables to control changes in tol project
#          widget
#
# PARAMETERS:
#   this  -> Instance of tol project 
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data

  trace variable ${this}::data(nameFile) w \
          [list ::TolProject::TraceNameFile $this]
  trace variable ${this}::data(isSaved)  w \
          [list ::TolProject::TraceSaved $this]
  trace variable ${this}::data(compiling)  w \
          [list ::TolProject::TraceCompiling $this]
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::TraceNameFile {this name1 name2 op} {
#
# PURPOSE: Sets tol project's name each time var nameFile changes.
#
# PARAMETERS:
#   this  -> Instance of tol project
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
  if {$data(nameFile) != ""} {
    set data(labelWindow) $data(nameFile)
  } else  {
    set data(labelWindow) "untitled.prj"
  }
  $data(f) configure -title "[mc Project] $data(labelWindow)"
  set tv [GetTreeview $this]
  #@ Antes se usaba $data(nameFile) como FileName y $data(labelWindow) como label   
  $tv item text root FileName [file tail $data(labelWindow)]
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::TraceSaved {this name1 name2 op} {
#
# PURPOSE: Sets saved/not saved info in status bar each time var 'isSaved'
#          changes.
#
# PARAMETERS:
#   this  -> Instance of tol project
#   name1, name2, op -> see above
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data

  if $data(isSaved) {
    $data(sb,modi) configure -text [mc "Saved"] -relief groove
  } else  {
    $data(sb,modi) configure -text [mc "Not Saved"] -relief raised
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::TraceCompiling {this name1 name2 op} {
#
# PURPOSE: Enables/disables compiling options each time var 'compiling'
#          changes.
#
# PARAMETERS:
#   this  -> Instance of tol project
#   name1, name2, op -> see above
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  set menu [GetMenu $this]

  if {$data(compiling) eq 1} {
    set state disabled
  } else  {
    set state normal    
  }
  foreach ite [list 4 5 6 7 8] {
    catch { $menu entryconfigure $ite -state $state }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::Create {path} {
#
# PURPOSE: Creates a Tol Project instance. Creates and packs the tol projedt
#          window (toplevel, treeview, toolbar, statusbar, popup menu, ...).
#
# PARAMETERS:
#   path  -> project window's path (toplevel)
#
# RETURN: Tol project's instance
#
#/////////////////////////////////////////////////////////////////////////////
  variable options
  global toltk_options
  set this ::TolProject::$path
  if { [lsearch [namespace children ::TolProject] $this] != -1 } {
    error "$this already exists as a children of ::TolProject"
  }

  #  A namespace is created for each tol project widget.

  namespace eval $this {
  
    # data -> array with information about the instance:
    #   f          -> toplevel path
    #   tb,frame   -> toolbar's frame path
    #   sb,frame   -> statusbar's frame path
    #   mainMenu

    variable data
  }

  upvar \#0 ${this}::data data
  # create
  set data(f) [set f [::project::CreateForm -width 400 -height 500 \
                        -type [mc Projects] \
                        -iniconfig TolProject \
                        -closecmd [list ::TolProject::OnDelete $this] \
                        -helpkey "Tb4Edp"]]
  set f [$f getframe]

  set n_b [list \
     "bnew"     "CmdNew"      "[mc [list New white project]] (CTRL+N)" \
     "openprj"  "CmdOpen"     "[mc Open] (CTRL+A)" \
     "bsave"    "CmdSave"     "[mc Save] (CTRL+S)" \
     "sep1"     "--"          ""\
     "tools"    "CmdOptions"  "[mc Options]"]
  # botonera superior
  set data(tb,frame) [frame $f.ftop -relief raised -bd 1]
  foreach {n_img n_proc n_hint} $n_b {
    if {[string equal $n_proc "--"]} {
      Separator $f.ftop.$n_img -orient vertical
    } else {
      set data(tb,$n_img) [Button $f.ftop.$n_img\
              -image [::Bitmap::get ${n_img}] -relief link -helptext $n_hint\
              -bd 2 -command [list ::TolProject::$n_proc $this]]
    }
    pack $f.ftop.$n_img -side left -pady 2 -fill y -padx 1
  }

  # frame y label para barra de estado
  set data(sb,frame) [frame $f.fop -relief groove -bd 1 -height 7]
  set data(sb,modi)  [label $f.fop.modi -bd 1 -width 14]
  set data(sb,desc)  [label $f.fop.desc -relief groove -bd 1 -anchor w]

  # treeview y scroll
  ScrolledWindow $f.sw
  set tv [::wtree $f.sw.t -table 0 -filter no \
    -background white -showroot 1 -selectmode single -showheader 0 \
    -highlightbackground gray75 -highlightcolor black \
    -highlightthickness 0 -font $toltk_options(fonts,Label) \
    -columns [list \
      [list {image text} -label FileName -tags FileName] \
      [list text -label Description -tags Description] \
      [list text -label Type -tags Type] \
      [list text -label Exclude -tags Exclude] \
      [list text -label ExcludeTOL -tags ExcludeTOL] \
    ] ]
  set data(treeview) $tv
  $tv item style set root FileName    [$tv column cget FileName -itemstyle]
  $tv item style set root Description [$tv column cget Description -itemstyle]
  $tv item style set root Type        [$tv column cget Type -itemstyle]
  $tv item style set root Exclude     [$tv column cget Exclude -itemstyle]
  $tv item style set root ExcludeTOL  [$tv column cget ExcludeTOL -itemstyle]
  $tv column configure "range 1 end" -visible no
  $tv column configure first -expand yes -weight 1
  $tv column configure all -itembackground ""       
  $tv notify bind $tv <Selection> "::TolProject::SelectProc $this"        
  
  $f.sw setwidget $f.sw.t
   
  # empaquetamos los label de estado
  pack $f.fop.modi -side left
  pack $f.fop.desc -expand true -fill x
    
  grid $f.ftop  -sticky new 
  grid $f.sw    -sticky news  
  grid $f.fop   -sticky ews 

  grid rowconfigure    $f 1 -weight 1
  grid columnconfigure $f 0 -weight 1


  bind $tv.frameTree.t <Control-KeyRelease> [list ::TolProject::OnControlKey $this %K %k]

  bind $tv.frameTree.t <Button-1> "::focus $tv"  
 
  bind $tv.frameTree.t <Button-3> +[list ::TolProject::PostVariable $this %X %Y]
  
  set prevBind [bind $tv.frameTree.t <Double-1>]
  set newBind "[lrange $prevBind 0 [expr [llength $prevBind]-3]] ; mdidoc disableglobalraise ; ::TolProject::OpenNodeIfTerminal $this ; break"
  #@ mdidoc disableglobalraise y break parece que cumplen el mismo cometido 
  bind $tv.frameTree.t <Double-1> $newBind
  
  set prevBind [bind $tv.frameTree.t <KeyPress-Return>]
  set newBind "[lrange $prevBind 0 [expr [llength $prevBind]-3]] ; ::TolProject::OpenNodeIfTerminal $this ; break"
  bind $tv.frameTree.t <KeyPress-Return> $newBind    

  bind $tv.frameTree.t <KeyPress-space>  +[list ::TolProject::NodeActiveAttributes $this]
  bind $tv.frameTree.t <KeyPress-Delete> +[list ::TolProject::DeleteActiveNode     $this]
  bind $tv.frameTree.t <KeyPress-F9>     "::TolProject::CompileActiveNode    $this ; break"
  bind $tv.frameTree.t <KeyPress-F8>     "::TolProject::DecompileActiveNode  $this ; break"  
 
  set data(mainMenu) [menu $tv.m -tearoff 0]

  bind $data(f) <Destroy> +[list ::TolProject::Destroy $this %W]
  $data(f) bind <Control-F4> "$data(f) kill; break"
  return $this
}

#///////////////////////////////////////////////////////////////////////////
proc ::TolProject::OnControlKey {this keysym keycode} {
#
# PURPOSE: Control the "Control + Key Press"
#
# PARAMETERS: this ->  Instance of project editor
#             keysym -> The keysym corresponding to the event, 
#                       substituted as a textual string
#             keycode -> The keycode field from the event
#
#///////////////////////////////////////////////////////////////////////////
  #puts "PLATFORM: $::tcl_platform(platform)"
  switch [string toupper $keysym] {
    A { ::TolProject::CmdOpen $this}
    N { ::TolProject::CmdNew  $this}
    S { ::TolProject::CmdSave $this}
  }
}

#/////////////////////////////////////////////////////////////////////////////
# File management commands (new, open, save, save as...)
#/////////////////////////////////////////////////////////////////////////////

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::CmdNew {this} {
#
# PURPOSE: Cleans project window to edit a new white project
#
# PARAMETERS:
#   this  -> Instance of Tol project
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data

  set tv [GetTreeview $this]
  if [string equal [CheckOpen $this] "cancel"] return
  $tv item delete all
  set img [GetImageFromType ROOT]
  $tv item image root FileName $img
  $tv item text root FileName "" Description "Root node" \
    Type "ROOT" Exclude 0 ExcludeTOL 0
  InitState $this new
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::UpdateListEdited {path} {
#
# PURPOSE: Updates list of last edited project files adding a new file path.
#
# PARAMETERS:
#   path  -> path of the last edited project file
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
proc ::TolProject::CmdOpen {{this ""}} {
#
# PURPOSE: Asks for a project file path and if it's given opens it into the
#          project window.
#
# PARAMETERS:
#   this -> Instance of tol project
#
#/////////////////////////////////////////////////////////////////////////////
  variable options
  variable typelist
  variable defExt
  
  if {$this == ""} {
    set parent .
  } else  {
    set parent [GetTreeview $this]
    if [string equal [CheckOpen $this] "cancel"] return
  }

  set path [tk_getOpenFile -title [mc "Open"] \
          -parent $parent -initialdir $options(lastDir) \
          -defaultextension $defExt -filetypes $typelist]

  if { $path != "" } {
    # abrimos solo si no esta ya abierto
    foreach ins [namespace children ::TolProject::] {
      upvar \#0 ${ins}::data data
      if {[string equal "$data(baseDir)/$data(nameFile)" $path]} {
        $data(f) raise
	    set resp [tk_messageBox -type ok -icon warning \
	     -message [mc "The project is allready opened!"] ]
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
proc ::TolProject::GetFromFile {this path {run 0}} {
#
# PURPOSE: Loads a project into the project window . This converts from the 
#          file especification to the treeview representation.
#
# PARAMETERS:
#   this -> Instance of tol project
#   path -> project file's path
#   run  -> if 1 the project will be automatically compiled
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  set tv [GetTreeview $this]
  $tv item delete all
  if {[::TolProject::WtreeFromFile $tv $path ::TolProject::GetImageFromType]} {
    set data(baseDir) [file dirname $path]
    set data(lastOpenedNodeDir) $data(baseDir)
    set data(nameFile) [file tail $path]
    UpdateListEdited $path
  } else {
    set data(nameFile) ""
  }
  $data(f) raise
  InitState $this open
  if { $run } {
    CompileNode $this root
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::CheckAll {} {
#
# PURPOSE: Checks for files opened if anyone has changed and asks for saving.
#
#/////////////////////////////////////////////////////////////////////////////
  if { ![ catch { set lst [namespace children ::TolProject] } msgerr] } {
    foreach ins $lst {
      if {[CheckOpen $ins] eq "cancel"} {
	return "cancel"
      }
    }
  }
  return "ok"
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::CheckOpen {this {type "yesnocancel"}} {
#
# PURPOSE: Checks if a file has changed and asks for saving.
#
# PARAMETERS:
#   this -> Instance of Tol project
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
      set name "untitled.prj"
    }
    # preguntamos por conformidad
    set tv [GetTreeview $this]
    set rq [tk_messageBox -type $type -icon warning -title [mc "Warning"]\
            -parent $tv -message "[mc "File %1\$s has changed!" $name].\n\
            [mc "Do you want to save changes?"]"] 
    switch -exact $rq {
      yes {
        if {$data(nameFile) != "" } {
          FileSave $this $data(nameFile)
        } else {
          CmdSaveAs $this $data(nameFile)
        }
      }
      cancel {return cancel}
      default {return ok}
    }
  }
  return ok
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::CmdSave {this} {
#
# PURPOSE: Saves project info actually opened in the project editor. If it 
#          doesn't have a name, asks for ti (CmdSaveAs).
#
# PARAMETERS:
#   this -> Instance of Tol project
#
# RETURN: Returns 1 if file has been saved correctly or 0 if not.
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data

  if {$data(nameFile) == ""} {
    set ok [CmdSaveAs $this]
  } else {
    set ok [FileSave $this $data(nameFile)]
  }
  return $ok
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::CmdSaveAs {this {nf ""}} {
#
# PURPOSE: Saves project actually opened in project editor asking for the
#          user to select a file to save it.
#
# PARAMETERS:
#   this -> Instance of Tol project
#   nf   -> name of file proposed
#
# RETURN: Returns 1 if file has been saved correctly or 0 if not.
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  variable options
  variable typelist
  variable defExt
  set tv [GetTreeview $this]
  
  if {$nf == ""} {
    set dir $options(lastDir)
    set fil ""
  } else  {
    set dir "[file dirname $nf]"
    set fil "[file tail $nf]"
  }
  set path [tk_getSaveFile -title [mc "Save as"] -initialdir $dir \
       -initialfile $fil -parent $tv \
       -defaultextension $defExt -filetypes $typelist]
  if {![string equal $path ""] } {
    set data(baseDir) [file dirname $path]
    set data(lastOpenedNodeDir) $data(baseDir)
    set data(nameFile) [file tail $path]
    set ok [FileSave $this [file tail $path]]
  } else {
    set ok 0
  }
  return $ok
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::FileSave { this path } {
#
# PURPOSE: Saves project actually opened in project editor in a text file.
#
# PARAMETERS:
#   this -> Instance of tol project
#   path -> name of file
#
# RETURN: Returns 1 if file has been saved correctly or 0 if not.
#
#/////////////////////////////////////////////////////////////////////////////
  variable options
  upvar \#0 ${this}::data data

  #Guardar la transformación de treeview a archivo
  if {[::TolProject::WtreeToFile [GetTreeview $this] \
      [::TolProject::ComputeFullPath $this $path]]} {
    #Añadir al archivo la entrada para que pueda ser llamado desde tol.exe
    AddEntry $this [::TolProject::ComputeFullPath $this $path]
    set data(nameFile) $path
    InitState $this save
    # guardamos el ultimo directorio utilizado
    set options(lastDir) $data(baseDir)
    set ok 1
  } else  {
    set ok 0
  }
  return $ok
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::AddEntry {this path} {
#
# PURPOSE: Adds a special entry in file path for including proyect from TOL.
#
# PARAMETERS:
#   this -> Instance of tol project
#   path -> name of file
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  set data(num) 1
  set fd [iniparse:openfile $path] 
  NodeToFile $this root $fd [file tail [file rootname $path]]
  iniparse:closefile $fd
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::NodeToFile {this index fd name} {
#
# PURPOSE: 
#
# PARAMETERS:
#   this -> Instance of tol project
#   index -> treeview node index
#   fd    -> channel identifier of text file
#   name  -> 
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  set key "Included TOL Files"

  set tv [GetTreeview $this]
  # Guardar data del nodo actual
  set excludeTOL [$tv item text $index ExcludeTOL]
  if ![string equal $excludeTOL 1] {
    if {[IsCompilable $this $index] } {
      set filename [$tv item text $index FileName]
      set item "Set $name.$data(num)"
      set value "Include(\"$filename\");"
      writeini $fd $key $item $value
      incr data(num)
    }
    # Llamar recursivamente para los hijos
    foreach child [$tv item children $index] {
      NodeToFile $this $child $fd $name
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::GetTreeview {this} {
#
# PURPOSE: Returns treeview's path of a given instance
#
# PARAMETERS:
#   this -> Instance of tol project
#
# RETURN: Treeview's path
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  return $data(treeview)
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::GetMenu {this} {
#
# PURPOSE: Returns treeview's menu of a given instance
#
# PARAMETERS:
#   this -> Instance of tol project
#
# RETURN: Treeview menu's path
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  return $data(mainMenu)
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::SelectProc {this} {
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  set tv [GetTreeview $this]
  set node [$tv selection get end] ;# anchor
  if { $node eq "" } return
  set description [$tv item text $node Description]
  $data(sb,desc) configure -text "$description"
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::PostVariable {this x y} {
#
# PURPOSE: Composes a popup menu and displays it. Its composed depending on 
#          the node clicked by user.
#
# PARAMETERS:
#   x -> x-coordinate where popup menu is drawn
#   y -> y-coordinate where popup menu is drawn
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  set tv [GetTreeview $this]
  set menu [GetMenu $this]
  $menu delete 0 end

  set state [expr {$data(compiling)? "disabled" : "normal" }]

  set node [$tv selection get end] ;# current
  if { [string length $node] } {
    if [IsTerminal $this $node] {
      $menu add command -label [mc "Open node"] -accelerator Return\
         -command [list ::TolProject::OpenNode $this $node]
  
      # Compute insertion point and prepare menu entry      
      set nodefocus [$tv selection get end] ;# focus
      set parent [$tv item parent $nodefocus]   
      set prev [$tv item prevsibling $nodefocus]
      if {[string equal $prev ""]} {
        set prevCount 1
      } else {
        set prevCount [expr [lsearch [$tv item children $parent] $node] + 1]
      } 
      $menu add command -label [mc "Add node"] \
       	-command [list ::TolProject::AddNode $this $parent $prevCount]\
        -accelerator Ins
  
    } else {
      # This adds to the end of the non-terminal node
      $menu add command -label [mc "Add node"] -accelerator Ins\
	    -command [list ::TolProject::AddNode $this $node]
    }
    $menu add command -label [mc "Attributes"] -accelerator space\
          -command [list ::TolProject::NodeAttributes $this $node]
    $menu add command -label [mc "Delete node"] -accelerator Sup\
          -command [list ::TolProject::DeleteNode $this $node]
    $menu add separator
    if [IsTerminal $this $node] {
      $menu add command -label [mc "Compile node"] -state $state \
         -command [list ::TolProject::CompileNode $this $node]\
         -accelerator F9
      $menu add command -label [mc "Decompile node"]  -state $state \
         -command [list ::TolProject::DecompileNode $this $node]\
         -accelerator F8
    } else {
      $menu add command -label [mc "Compile node"] -state $state \
         -command [list ::TolProject::CompileNode $this $node]\
         -accelerator F9
      $menu add command -label [mc "Decompile node"]  -state $state \
         -command [list ::TolProject::DecompileNode $this $node]\
         -accelerator F8
    }
    $menu add command -label [mc "Compile to the end"] -state $state \
       -command [list ::TolProject::CompileIfBeforeNode $this root $node]
    $menu add command -label [mc "Compile up to here"] -state $state \
       -command [list ::TolProject::CompileUpToHereNode $this root $node]    
    # save as
    $menu add separator    
    $menu add command -label "[mc "Save as"] ..." \
      -command [list ::TolProject::CmdSaveAs $this]    
    # show menu
    tk_popup $menu $x $y      
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::OpenNodeIfTerminal {this} {
#
# PURPOSE: 
#
# PARAMETERS:
#   this -> Instance of tol project
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  set tv [GetTreeview $this]
  set node [$tv selection get end] ;# anchor
  OpenNode $this $node
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::OpenNode {this node} {
#
# PURPOSE: Opens a terminal node of the project. It depends on the node type
#          what kind of editor use for opening the node (text or table).
#
# PARAMETERS:
#   this -> Instance of tol project
#   node -> Node active in treeview, which is opened
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  set tv [GetTreeview $this]
  
  set type [$tv item text $node Type]
  set filename [$tv item text $node FileName]
  switch $type {
    TOL { ::Editor::Open \
          [::TolProject::ComputeFullPath $this $filename] }
    BDT -
    BST { ::EditTable::Open \
          [::TolProject::ComputeFullPath $this $filename] } 
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::AddNode {this node {prev end}} {
#
# PURPOSE: Adds a node to the project. First of all, asks for the node type.
#
# PARAMETERS:
#   this -> Instance of tol project
#   node -> Node active in treeview, in which the new node is added
#   prev -> where is added, end if nothing is specified
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  set tv [GetTreeview $this]
  
  #si no hay un directorio base establecido mostramos dialogo de guardar proyecto
  if { $data(baseDir) eq "" } {
    CmdSave $this
    return
  }
  
  ClearData $this
  DialogType $this $tv
  # si no se ha pulsado cancelar
  if {$data(type) != -1} {
    if {!($data(type) eq "Fol") } {
      set title "[mc Add] [mc {a file of type}] $data(type) "
    } else {
      set title "[mc Add] [mc folder]"
    }
    DialogAttributes $this $tv $title
    # si no se ha pulsado cancelar
    if {![string equal $data(name) ""]} {

      # Path name gets transformed for files
      switch -exact $data(type) {
        TOL -
        BDT -
        BST {
          set data(lastOpenedNodeDir) [file dirname $data(name)]
          set data(name) \
          [ObtainRelativePath $data(name) $data(baseDir)]
        }
        default {
          # do nothing for folders
        }
      }
     
      set img [GetImageFromType $data(type)]                                        
      if { $data(type) eq "Fol" } {
        set hasbutton 1
      } else {
        set hasbutton 0
      }
      if {$prev eq "end"} {
        set idx [$tv item create -parent $node -button $hasbutton -open 0]
      } else {
        set idx [$tv item create -prevsibling $prev -button $hasbutton -open 0]
      }
      $tv item image $idx FileName $img
      $tv item text $idx FileName $data(name) Description $data(desc) \
        Type $data(type) Exclude $data(exclude) ExcludeTOL $data(excludeTOL)
      set data(isSaved) 0
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::NodeActiveAttributes {this args} {
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  set tv [GetTreeview $this]
  set node [$tv selection get end] ;# anchor
  NodeAttributes $this $node
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::NodeAttributes {this node} {
#
# PURPOSE: Shows the attributes of the selected node.
#
# PARAMETERS:
#   this -> Instance of tol project
#   node -> Node active in treeview, whose attributes are shown
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  set tv [GetTreeview $this]
  GetDataFromNode $this $node
  set name_old $data(name)
  set desc_old $data(desc)
  set excl_old $data(exclude)
  set exct_old $data(excludeTOL)
  set title "[$tv item text $node FileName] [mc Attributes]"
  DialogAttributes $this $tv $title
  # si no se ha pulsado cancelar
  if {![string equal $data(name) ""]} {
    # si se ha cambiado algun dato
    if {![string equal $name_old $data(name)] ||
        ![string equal $desc_old $data(desc)] ||
        ![string equal $excl_old $data(exclude)] ||
        ![string equal $exct_old $data(excludeTOL)]} {
      $tv item text $node FileName $data(name) Description $data(desc) \
        Exclude $data(exclude) ExcludeTOL $data(excludeTOL)                       
      set data(isSaved) 0
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::DeleteActiveNode {this args} {
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  set tv [GetTreeview $this]
  set node [$tv selection get end] ;# current
  DeleteNode $this $node
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::DeleteNode {this node} {
#
# PURPOSE: Deletes a node and all its childs from the project.
#
# PARAMETERS:
#   this -> Instance of tol project
#   node -> Node active in treeview, which is deleted
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data

  set tv [GetTreeview $this]
  set name [$tv item text $node FileName]
  set answer [tk_messageBox -title [mc "Question"]\
            -parent $tv\
            -message [mc "Are you sure you want to delete node %1\$s?" $name] \
            -type yesno -icon question]
  if { [string compare $answer yes] == 0 } {
    $tv item delete $node
    set data(isSaved) 0
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::CompileActiveNode {this args} {
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  set tv [GetTreeview $this]
  set node [$tv selection get end] ;# current
  CompileNode $this $node
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::CompileNode {this node } {
#
# PURPOSE: Compiles a node and all its childs if parameter 'childs' is true.
#
# PARAMETERS:
#   this   -> Instance of tol project
#   node   -> Node active in treeview, which is compiled
#   childs -> flag that indicates if childs must be compiled too
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  set tv [GetTreeview $this]
  set exclude [$tv item text $node Exclude]
  if ![string equal $exclude 1] {  
    if [IsCompilable $this $node] {
      #Tolcon_Trace "  Compilo $node"
      set data(compiling) 1
      set filename [$tv item text $node FileName]    
      ::TolConsole::Include \
        [::TolProject::ComputeFullPath $this $filename]
      set data(compiling) 0
    }
    foreach child [$tv item children $node] {
      CompileNode $this $child
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::CompileIfBeforeNode {this node ref} {
#
# PURPOSE: Compiles a node if it's after a node given as reference
#
# PARAMETERS:
#   this -> Instance of tol project
#   node -> Node to check if it's after a given node to compile it
#   ref  -> Node given as reference
#
#/////////////////////////////////////////////////////////////////////////////
  set tv [GetTreeview $this]
  #Tolcon_Trace "CompileIfBeforeNode. node: $node, ref: $ref"
  set isbefore [expr [$tv item order $node] < [$tv item order $ref]]  
  if !$isbefore {
    CompileNode $this $node
  } else  {
    foreach child [$tv item children $node] {
      CompileIfBeforeNode $this $child $ref
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::CompileUpToHereNode {this node ref} {
#
# PURPOSE: Compiles up to here
#
# PARAMETERS:
#   this -> Instance of tol project
#   node -> Node to check if it's after a given node to compile it
#   ref  -> Node given as reference
#
#/////////////////////////////////////////////////////////////////////////////
  set tv [GetTreeview $this]
  #Tolcon_Trace "CompileUpToHereNode. node: $node, ref: $ref"
  set isbefore [expr [$tv item order $node] < [$tv item order $ref]]  
  if {((![string eq root $node] && [string eq $node $ref]) ||
       (![string eq root $node] && $isbefore))} {
    CompileNode $this $node
  } else  {
    foreach child [$tv item children $node] {
      CompileUpToHereNode $this $child $ref
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::DecompileActiveNode {this args} {
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  set tv [GetTreeview $this]
  set node [$tv selection get end] ;# current
  DecompileNode $this $node
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::DecompileNode {this node} {
#
# PURPOSE: Decompiles a node and all its childs.
#
# PARAMETERS:
#   this   -> Instance of tol project
#   node   -> Node active in treeview, which is decompiled
#
#/////////////////////////////////////////////////////////////////////////////
  set tv [GetTreeview $this]
  set exclude [$tv item text $node Exclude]
  if ![string equal $exclude 1] {
    if [IsCompilable $this $node] {
      set filename [$tv item text $node FileName]    
      catch {::TolInspector::DecompileFile \
             [::TolProject::ComputeFullPath $this $filename]}
      ::TolInspector::UpdateFileRoot
    }
    foreach child [$tv item children $node] {
      DecompileNode $this $child
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::ClearData {this} {
#
# PURPOSE: Clears some variables of data array relatives to attributes of 
#          active node.
#
# PARAMETERS:
#   this -> Instance of tol project
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  set data(name) ""
  set data(desc) ""
  set data(type) ""
  set data(exclude) 0
  set data(excludeTOL) 0
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::GetDataFromNode {this index} {
#
# PURPOSE: Sets some variables of data array, getting its values from a given
#          node.
#
# PARAMETERS:
#   this  -> Instance of tol project
#   index -> node index of a treeview to get values
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  set tv [GetTreeview $this]
  set data(name)       [$tv item text $index FileName]
  set data(desc)       [$tv item text $index Description]
  set data(type)       [$tv item text $index Type]
  set data(exclude)    [$tv item text $index Exclude]
  set data(excludeTOL) [$tv item text $index ExcludeTOL]
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::GetImageFromType {type} {
#
# PURPOSE: Returns the image's path of a node given its type
#
# PARAMETERS:
#   type -> type of node
#
# RETURN: Image's path that correspond to type or "" if error
#
#/////////////////////////////////////////////////////////////////////////////
  set path ""
  switch -exact $type {
    ROOT { set path [Bitmap::get Files] }
    Fol  { set path [Bitmap::get DIRFile] }
    TOL -
    BDT -
    BST  { set path [Bitmap::get ${type}File] }
  }
  return $path
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::IsTerminal {this node} {
#
# PURPOSE: Determines if a node is terminal or not
#
# PARAMETERS:
#   this -> Instance of tol project
#   node -> node index of a treeview
#
# RETURN: True if node is terminal and false if not
#
#/////////////////////////////////////////////////////////////////////////////
  set terminal 0
  set tv [GetTreeview $this]
  set type [$tv item text $node Type]
  switch $type {
    Fol -
    ROOT { set terminal 0 }
    TOL -
    BDT -
    BST { set terminal 1 } 
  }
  return $terminal
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::IsCompilable {this node} {
#
# PURPOSE: Determines if a node is compilable or not
#
# PARAMETERS:
#   this -> Instance of tol project
#   node -> node index of a treeview
#
# RETURN: True if node is compilable and false if not
#
#/////////////////////////////////////////////////////////////////////////////
  set compilable 0
  set tv [GetTreeview $this]
  set type [$tv item text $node Type]
  switch $type {
    Fol -
    ROOT { set compilable 0 }
    TOL -
    BDT -
    BST {
        set compilable 1
    } 
  }
  return $compilable
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::DialogType {this path} {
#
# PURPOSE: Shows a dialog to ask for the type of a node
#
# PARAMETERS:
#   this -> Instance of tol project
#   path -> dialog's parent path
# 
#/////////////////////////////////////////////////////////////////////////////
  #Type node dialog
  set tv [GetTreeview $this]
  set dialog [Dialog $tv.dt -title [mc "Select node type"]\
          -modal local -parent $tv]
  wm resizable $dialog 0 0
  wm protocol $dialog WM_DELETE_WINDOW \
          [list ::TolProject::OptionsDestroy $this $dialog]
  set w [$dialog getframe]
  
  set ${this}::data(type) TOL
  set f [labelframe $w.f -text [mc "Node type"]]
  radiobutton $f.rbTOL -text [mc "TOL file"] -value TOL -highlightthickness 0\
        -pady 0 -variable ${this}::data(type)
  radiobutton $f.rbBDT -text [mc "BDT file"] -value BDT -highlightthickness 0\
        -pady 0 -variable ${this}::data(type)
  radiobutton $f.rbBST -text [mc "BST file"] -value BST -highlightthickness 0\
        -pady 0 -variable ${this}::data(type)
  radiobutton $f.rbFol -text [mc "Folder"]   -value Fol -highlightthickness 0\
        -pady 0 -variable ${this}::data(type)  

  grid $f.rbTOL $f.rbBDT -sticky nw -padx 0
  grid $f.rbBST $f.rbFol -sticky nw -padx 0
  
  
  
   set w_bbox \
          [ButtonBox $w.bb -orient horizontal -spacing 10 -padx 0 -pady 0 \
          -homogeneous false]
    $w_bbox add -text [mc "Accept"] -relief link \
          -helptext [mc "Continue"] \
          -image [::Bitmap::get accept] -compound left -padx 5 -pady 1 \
          -command [list ::TolProject::DialogTypeOK $this $dialog]
    $w_bbox add -text [mc "Cancel"] -relief link \
          -helptext [mc "Cancel"]\
          -image [::Bitmap::get cancelx] -compound left -padx 5 -pady 1 \
          -command [list ::TolProject::DialogTypeDestroy $this $dialog]
    
  pack $w_bbox -expand true -fill x    -side bottom
  pack $f      -expand true -fill both

  bind $dialog <F1> "ShowHelp Tb4Edp"
  $dialog draw
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::DialogTypeOK {this dialog} {
#
# PURPOSE: 
#
# PARAMETERS:
#   this   -> Instance of tol project
#   dialog -> Get type dialog
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  
  destroy $dialog
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::DialogTypeDestroy {this dialog} {
#
# PURPOSE: Destroy get type dialog
#
# PARAMETERS:
#   this   -> Instance of tol project
#   dialog -> Get type dialog
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  set data(type) -1
  destroy $dialog
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::DialogAttributes {this path title} {
#
# PURPOSE: Shows a dialog to ask for the attributes of a node
#
# PARAMETERS:
#   this  -> Instance of tol project
#   path  -> dialog's parent path
#   title ->
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  #Attributes node dialog
  set tv [GetTreeview $this]
  set dialog [Dialog $tv.da -title $title -modal local -parent $tv\
     -default 0 -cancel 1]
  wm resizable $dialog 0 0
  wm protocol $dialog WM_DELETE_WINDOW \
          [list ::TolProject::OptionsDestroy $this $dialog]
  set w [$dialog getframe]
  
  set f1 [labelframe $w.f1 -text [mc "File name"]]
  
  switch -exact $data(type) {
    TOL {
      set typelist {{"Tol Files" {".tol"}} {"All Files" {"*"}}}
      set defExt   .tol
    }
    BDT {
      set typelist {{"Bayes Data Table" {".bdt"}} {"All Files" {"*"}}}
      set defExt   .bdt
    }
    BST {
      set typelist {{"Bayes Struct Table" {".bst"}} {"All Files" {"*"}}}
      set defExt   .bst
    }
    default {
      set typelist {{"All Files" {"*"}}}
      set defExt ""
    }
  }

  # This passes -initialdir to ::Objects::ChooseFileCreate
  # which might need further study
  entry  $f1.eFile -width 50 -textvariable ${this}::data(name)
  ::Objects::ChooseFileCreate ${this}::data(name) $f1.bFile \
    -filetypes $typelist -defaultextension $defExt \
    -initialdir $data(lastOpenedNodeDir)
  
  set f2 [labelframe $w.f2 -text [mc "Description"]]
  entry $f2.description -width 60 -textvariable ${this}::data(desc)

  checkbutton $w.chExclude -text [mc "Exclude in TOLBase"] \
            -variable ${this}::data(exclude)

  checkbutton $w.chExcludeTOL -text [mc "Exclude in tol.exe"] \
        -variable ${this}::data(excludeTOL)

  if {[string equal $data(type) "Fol"] || 
      [string equal $data(type) "ROOT"] } {
    grid $f1.eFile -sticky nw -padx 2 -pady 2
  } else  {
    grid $f1.eFile $f1.bFile -sticky nw -padx 2 -pady 2
  }
  grid $f2.description     -sticky nw -padx 2 -pady 2

  set w_bbox \
    [ButtonBox $w.bb -orient horizontal -spacing 10 -padx 0 -pady 0 \
    -homogeneous false]
  $w_bbox add -text [mc "Accept"] -relief link \
    -helptext [mc "Continue"] \
    -image [::Bitmap::get accept] -compound left -padx 5 -pady 1 \
    -command [list ::TolProject::DialogAttributesOK $this $dialog]
  $w_bbox add -text [mc "Cancel"] -relief link \
    -helptext [mc "Cancel selection"]\
    -image [::Bitmap::get cancelx] -compound left -padx 5 -pady 1 \
    -command [list ::TolProject::DialogAttributesDestroy $this $dialog]
  
  grid $f1 - -sticky news
  grid $f2 - -sticky news
  grid $w.chExclude $w.chExcludeTOL -sticky nw -pady 2
  grid $w_bbox                       -sticky ns -pady 2 -columnspan 2

  focus $f1.eFile
  
  bind $dialog <F1> "ShowHelp Tb4Edp"
  $dialog draw

}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::DialogAttributesOK {this dialog} {
#
# PURPOSE:
#
# PARAMETERS:
#   this   -> Instance of tol project
#   dialog -> Get type dialog
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
    
  destroy $dialog
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::DialogAttributesDestroy {this dialog} {
#
# PURPOSE: Destroy get type dialog
#
# PARAMETERS:
#   this   -> Instance of tol project
#   dialog -> Get type dialog
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  set data(name) ""
  destroy $dialog
}

#/////////////////////////////////////////////////////////////////////////////
# OPTIONS
#/////////////////////////////////////////////////////////////////////////////

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::CmdOptions {this} {
#
# PURPOSE: Creates a dialog options for Tol Project Editors
#
# PARAMETERS:
#   this -> Instance of tol project
#
#/////////////////////////////////////////////////////////////////////////////
  set path [GetTreeview $this]
  set dialog [Dialog $path.d -title [mc "TOL Project Editor Options"]\
            -modal local -parent $path]
  wm resizable $dialog 0 0
  wm protocol $dialog WM_DELETE_WINDOW \
        [list ::TolProject::OptionsDestroy $this $dialog]
  set w [$dialog getframe]
  set w_tabset [NoteBook $w.nb -tabbevelsize 8 -tabpady {2 6} -font {Arial 8}] 
  #insertar tabset
  set num_i 0
  foreach lab_i [list Others] {
    $w_tabset insert $num_i $lab_i -text "  [mc $lab_i]  " -background \#d9d9d9 
    set tab_i [$w_tabset getframe $lab_i]
    $tab_i configure -borderwidth 2 -background \#d9d9d9
    incr num_i    
    set f$num_i [frame $tab_i.f]
    pack $tab_i.f -fill both -expand yes  
  }
  $w_tabset raise [$w_tabset pages 0]
      
  OptionsGet $this
  OptionsCreateOthers  $this $f1  
  OptionsCheck $this
    
  set w_bbox \
    [ButtonBox $w.bb -orient horizontal -spacing 10 -padx 0 -pady 0 \
    -homogeneous false]
  $w_bbox add -text [mc "Accept"] -relief link \
    -helptext [mc "Accept"] \
    -image [::Bitmap::get accept] -compound left -padx 5 -pady 1 \
    -command [list ::TolProject::OptionsOK $this $dialog]
  $w_bbox add -text [mc "Cancel"] -relief link \
    -helptext [mc "Cancel"]\
    -image [::Bitmap::get cancelx] -compound left -padx 5 -pady 1 \
    -command [list ::SetGraphDialog::Cancel $dialog]
    
  pack $w_bbox   -fill x -expand false -side bottom
  $w_tabset compute_size
  pack $w_tabset -fill both -expand true

  bind $dialog <F1> "ShowHelp Tb4EdpOpc"
  $dialog draw
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::OptionsCreateOthers {this w} {
#
# PURPOSE: Creates a frame with general options as number of files edited
#          recently
#
# PARAMETERS:
#   this -> Instance of tol project
#   w    -> widget's path to allocate the frame
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpOpt
    
  TitleFrame $w.tf1 -text [mc "General Options"]
  set f1 [$w.tf1 getframe]
    
  checkbutton $f1.chEdited -text [mc "Files edited recently"]\
          -variable ::TolProject::tmpOpt(var,chEdited)\
          -command [list ::TolProject::OptionsCheckEdited $this]
    
  set tmpOpt(widget,edited) [SpinBox $f1.sbEdited -justify right -width 4 \
          -editable 0 -range {0 10} \
          -textvariable ::TolProject::tmpOpt(var,numEdited)]
  label $f1.lFiles -text [mc files]
  
  frame $f1.f -width 25
    
  grid $f1.f $f1.chEdited     $f1.sbEdited     $f1.lFiles -sticky nw -padx 2
  grid rowconfigure    $f1 0 -weight 1
  grid columnconfigure $f1 3 -weight 1
    
  grid $w.tf1 -sticky news -padx 2
    
  grid rowconfigure    $w 1 -weight 1
  grid rowconfigure    $w 2 -weight 2
  grid columnconfigure $w 0 -weight 1
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::OptionsGet {this} {
#
# PURPOSE: Gets all options variables values from ini file  and copies them
#          to the temporary tmpOpt array. 
#
# PARAMETERS:
#   this   -> Instance of tol project
#
#/////////////////////////////////////////////////////////////////////////////
  variable options
  variable tmpOpt
  set tmpOpt(var,chEdited)   $options(chEdited)
  set tmpOpt(var,numEdited)  $options(numEdited)
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::OptionsSet {this} {
#
# PURPOSE: Copies all . 
#
# PARAMETERS:
#   this   -> Instance of tol project
#
#/////////////////////////////////////////////////////////////////////////////
  variable options
  variable tmpOpt
    
  regsub -all var, [array names ::TolProject::tmpOpt var,*] "" optNames
  foreach it $optNames {
    set options($it) $tmpOpt(var,$it)
  }  
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::OptionsCheck {this} {
#
# PURPOSE: Checks for all widgets from dialog options that need to be checked
#          for enabling/disabling other associated controls. 
#
# PARAMETERS:
#   this   -> Instance of tol project
#
#/////////////////////////////////////////////////////////////////////////////
  OptionsCheckEdited           $this
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::OptionsCheckEdited {this} {
#
# PURPOSE: Checks if option chEdited is checked and enables/disabled other 
#          associated controls. 
#
# PARAMETERS:
#   this   -> Instance of tol project
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpOpt
  if {!$tmpOpt(var,chEdited)} {      
    $tmpOpt(widget,edited) configure -state disabled
  } else  {
    $tmpOpt(widget,edited) configure -state normal
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::OptionsDestroy {this dialog} {
#
# PURPOSE: Destroy options dialog
#
# PARAMETERS:
#   this   -> Instance of tol project
#   dialog -> Options dialog
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpOpt
    
  array unset tmpOpt
  destroy $dialog
}

#/////////////////////////////////////////////////////////////////////////////  
proc ::TolProject::OptionsApply {this} {
#
# PURPOSE: Applies options from the options dialog
#
# PARAMETERS:
#   this   -> Instance of tol project
#
#///////////////////////////////////////////////////////////////////////////// 
  OptionsSet $this
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::OptionsOK {this dialog} {
#
# PURPOSE: Applies options and closes options dialog
#
# PARAMETERS:
#   this   -> Instance of tol project
#   dialog -> Options dialog
#
#/////////////////////////////////////////////////////////////////////////////
  OptionsApply   $this
  OptionsDestroy $this $dialog
}

#/////////////////////////////////////////////////////////////////////////////
# INIFILE
#/////////////////////////////////////////////////////////////////////////////

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::ReadIniSpecial {} {
#
# PURPOSE: Reads common Edit Table options from .ini file. This options are
#          common for all instances of Tol Project Editor. They are read just 
#          one time.
#
#/////////////////////////////////////////////////////////////////////////////
  variable options
  global toltk_options

  set rini ::iniFile::Read
  set options(chEdited)   [$rini TolProject saveRecent  1]
  set options(numEdited)  [$rini TolProject numRecent   8]
  set options(edited)     [$rini TolProject recent      {}]

  set options(font)       [$rini TolProject font  $toltk_options(fonts,Fixed)]
  set options(lastDir)    [$rini TolProject lastDir  ""]
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::WriteIni {this} {
#
# PURPOSE: Write common Tol Projects options to .ini file
#
# PARAMETERS:
#   this  -> Instance of tol project
#
#/////////////////////////////////////////////////////////////////////////////
  variable options
  upvar \#0 ${this}::data data
  set wini ::iniFile::Write

  # Common options
  $wini TolProject saveRecent   $options(chEdited)
  $wini TolProject numRecent    $options(numEdited)
  $wini TolProject recent       $options(edited)
  $wini TolProject font         $options(font)
  $wini TolProject lastDir      $options(lastDir)

  ::iniFile::Flush
}

#/////////////////////////////////////////////////////////////////////////////
# Métodos que estaban en ::BayesTreeview (bftrview.tcl)
#/////////////////////////////////////////////////////////////////////////////

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::WtreeFromFile {tv path {proc ""}} {
#
# PURPOSE: Loads a wtree info data from a text especification file of a wtree. 
#
# PARAMETERS:
#   tv   -> wtree's path
#   path -> name of file
# proc -> procedure that converts type node to the path of the image of node
#
# RETURN: Returns 1 if wtree has been loaded correctly or 0 if not.
#
#/////////////////////////////////////////////////////////////////////////////
  if ![file exists $path] {
    return -code error "File $path doesn't exist"
  }
  set fd [iniparse:openfile $path] 
  BNodeFromFile $tv root "" $fd $proc
  iniparse:closefile $fd
  return 1
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::WtreeToFile {tv path} {
#
# PURPOSE: Saves a wtree info data into a text especification file of a wtree.
#
# PARAMETERS:
#   tv   -> wtree's path
#   path -> name of file
#
# RETURN: Returns 1 if file has been saved correctly or 0 if not.
#
#/////////////////////////////////////////////////////////////////////////////
  #borrar archivo pq iniparse:openfile abre para solo lectura si existe
  file delete $path
  set fd [iniparse:openfile $path] 
  BNodeToFile $tv root $fd
  iniparse:closefile $fd
  return 1
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::BNodeToFile {tv index fd} {
#
# PURPOSE: Saves a wtree's node info data into a text file.
#
# PARAMETERS:
#   tv    -> wtree's path
#   index -> wtree node index
#   fd    -> channel identifier of text file
#
#/////////////////////////////////////////////////////////////////////////////
  set key $index

  # Guardar data del nodo actual
  set columnNames [list FileName Description Type Exclude ExcludeTOL]
  foreach item $columnNames {
    writeini $fd $key $item [$tv item text $index $item]
  }
  
  writeini $fd $key Label [$tv item text $index FileName]
  # Guardar sus hijos
  writeini $fd $key Children [$tv item children $index]

  # Llamar recursivamente para los hijos
  foreach child [$tv item children $index] {
    BNodeToFile $tv $child $fd
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolProject::BNodeFromFile {tv index parent fd {imgFromType ""}} {
#
# PURPOSE: Loads a wtree's node info data from a text file.
#
# PARAMETERS:
#   tv     -> wtree's path
#   index  -> wtree node index
#   parent -> wtree node's parent index
#   fd     -> channel identifier of text file
#
#/////////////////////////////////////////////////////////////////////////////
  set key $index
  set image ""  
  set filename ""
  set description ""
  set type ""
  set exclude 0
  set excludeTOL 0 ;#@ puede que no lo tenga el prj
  foreach item [readini $fd $key] {
    set value [readini $fd $key $item]
    switch -exact $item {
      FileName { set filename $value }
      Description { set description $value }
      Type { set type $value }
      Exclude { set exclude $value }
      ExcludeTOL { set excludeTOL $value }
      Label { set void 0 }      
    }
    if {[string equal $item "Type"] && ![string equal $imgFromType ""]} {
      if ![string equal [set img [$imgFromType $value]] ""] {
        #@ $tv entry configure $idx -icons [list $img $img]
        set image $img
      }
    }
  }
  if ![string equal $parent ""] {
    #@ se inserta un nodo
    if { $type eq "Fol" } {
      set hasbutton 1
    } else {
      set hasbutton 0
    }  
    set idx [$tv item create -parent $parent -button $hasbutton -open 0]
    $tv item image $idx FileName $img
    $tv item text $idx FileName $filename Description $description \
      Type $type Exclude $exclude ExcludeTOL $excludeTOL  
  } else {
    #@ se actualiza root
    $tv item image root FileName $img
    $tv item text root FileName $filename Description $description \
      Type $type Exclude $exclude ExcludeTOL $excludeTOL
    set idx root
  }  
  # Llamar recursivamente para los hijos
  foreach child [readini $fd $key Children] {
    BNodeFromFile $tv $child $idx $fd $imgFromType
  }
}

::TolProject::ReadIniSpecial
