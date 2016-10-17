#/////////////////////////////////////////////////////////////////////////////
# FILE    : editable.tcl
# PURPOSE : This file contains a namespace called EditTable with its funcions.
#           It's for managing table editors of .bst, .bdt, .bmt files or all
#           kind of files with tabular estructure.
#           It's a child of BayesTable so inherites all its functionalities.
#/////////////////////////////////////////////////////////////////////////////


#/////////////////////////////////////////////////////////////////////////////
  namespace eval ::EditTable {
#
# PURPOSE: The purpose of this namespace is organize code and functions and
#          manage common options for all table editors such as last edited
#          files, last directory opened, etc.
#          A special namespace is created for each instance of table editor.
#
# VARIABLES:
#   options -> common options for all table editor windows:
#     geometry  -> initial window table editor geometry
#     wm,state  -> initial window table editor state
#     lastDir   -> last directory used
#     chEdited  -> boolen for saving last edited table files path
#     numEdited -> number of last edited table files path to save
#     edited    -> path of last edited table files
#   num_Table -> incrementally number to identify each table editor window
#   typelist   -> list of type files supported by table editor
#   defExt     -> default extension for files managed by table editor
#
#/////////////////////////////////////////////////////////////////////////////
  variable options
  variable num_Table 0
  variable typelist {{"Bayes Struct Table"  {".bst"}}
                     {"Bayes Data Table"    {".bdt"}}
                     {"Bayes Matrix Table"  {".bmt"}}
                     {"All Files"           {"*"}   }}
  variable defExt   .tol
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::EditTable::New {{path ""} } {
#
# PURPOSE: Creates a new table editor window and an instance of it. This also 
#          initializes the instance and register the window in the project.
#
# PARAMETERS:
#   path  -> path of file to edit. If it isn't specified a new white table
#            is created.
#
# RETURN: A instance of table editor.
#
#/////////////////////////////////////////////////////////////////////////////
  variable num_Table
  incr num_Table

  set Instance [Create $path]
  SetTraces $Instance
  InitState $Instance new

  #Tolcon_Trace "[::BayesTable::GetLocData $Instance labelWindow]"

  if {$path == ""} {
    CmdNew $Instance
  }
  return $Instance
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::EditTable::Open {{path ""}} {
#
# PURPOSE: Opens a file in a table editor window. If file is already opened
#          it raises its window, else it creates a new table editor window and
#          an instance of it.
#
# PARAMETERS:
#   path  -> path of file to open. If it isn't specified it ask for it.
#
# RETURN: A instance of table editor.
#
#/////////////////////////////////////////////////////////////////////////////
  if {$path != ""} {
    #Comprobar si el archivo ya está abierto y si es así hacer un raise
    if { ![ catch { set lst [namespace children ::BayesTable] } msgerr] } {
      foreach ins $lst {
        upvar \#0 ${ins}::data data
        if {[string equal $data(namespace) "EditTable"] &&
            [string equal $data(nameFile) $path]} {
          #wm deiconify $data(f)
          $data(f) raise
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
    GetFromFile $this $path
  }  

  if {[info exists this]} {
    return $this
  } else  {
    return ""
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::EditTable::postEdited {m {ord 0}} {
#
# PURPOSE: Fills a menu with the last table edited files, according to the
#          information of 'options' variable.
#
# PARAMETERS:
#   m   -> menu's path that will have the list of last table edited files
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
      $m add command -label $w -command [list ::EditTable::Open $w]
    }
    if [llength $lst] {
      $m add separator
      $m add command -label [mc "Clear"]\
            -command [list ::EditTable::clearEdited]
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::EditTable::clearEdited {} {
#
# PURPOSE: Clears the list of last edited files
#
#/////////////////////////////////////////////////////////////////////////////
  variable options
  set options(edited) ""
  ::iniFile::Write EditTable recent       $options(edited)
  ::iniFile::Flush
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::EditTable::OnDelete {this} {
#
# PURPOSE: Procedure to check things before destroying table editor window
#
# PARAMETERS:
#   this  -> Instance of table editor
#
#/////////////////////////////////////////////////////////////////////////////
  if {[string equal [CheckOpen $this] "cancel"]} {
    return 0
  }
  destroy [::BayesTable::GetData $this f]
  return 1
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::EditTable::InitState {this from} {
#
# PURPOSE: Initializes some table properties and variables according to the
#          state of table editor.
#
# PARAMETERS:
#   this  -> Instance of table editor
#   from  -> State that defines values for the variables
#
#/////////////////////////////////////////////////////////////////////////////
  switch $from {
    new {
      set table [::BayesTable::GetTkTable $this]
      $table configure -rows 5 -cols 4
      ::BayesTable::SetData $this isSaved 1
      ::BayesTable::SetData $this nameFile ""
      ::BayesTable::UpdateInfoRows $this
    }
    open {
      ::BayesTable::SetData $this isSaved 1
      ::BayesTable::UpdateInfoRows $this
    }
    save {
      ::BayesTable::SetData $this isSaved 1
    }
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::EditTable::SetTraces {this} {
#
# PURPOSE: Sets some traces of variables to control changes in table editor
#          widget
#
# PARAMETERS:
#   this  -> Instance of table editor
#
#/////////////////////////////////////////////////////////////////////////////
  trace variable [::BayesTable::GetLocData $this nameFile] w\
     [list ::EditTable::TraceNameFile $this]
  trace variable [::BayesTable::GetLocData $this isSaved] w\
     [list ::EditTable::TraceSaved $this]
  #trace variable [::BayesTable::GetLocElements $this] w\
  #   "::BayesTable::SetData $this isSaved 0;\
  #    Tolcon_Trace {Ha cambiado la matriz};\
  #    ::BayesTable::UpdateInfoRows $this ;\#"
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::EditTable::TraceNameFile {this name1 name2 op} {
#
# PURPOSE: Sets table file's name each time var 'nameFile' changes.
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
  if {[::BayesTable::GetData $this nameFile] != ""} {
    ::BayesTable::SetData $this labelWindow [::BayesTable::GetData $this nameFile]
  } else  {
    ::BayesTable::SetData $this labelWindow "untitled.bst"
  }
  [::BayesTable::GetData $this f] configure -title \
   "[::BayesTable::GetData $this labelWindow] - [mc [list Bayes Table Editor]]"	
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::EditTable::TraceSaved {this name1 name2 op} {
#
# PURPOSE: Sets saved/not saved info in status bar each time var 'isSaved'
#          changes.
#
# PARAMETERS:
#   this  -> Instance of table editor
#   name1, name2, op -> see above
#
#/////////////////////////////////////////////////////////////////////////////
  #Tolcon_Trace "En TraceSaved, isSaved vale [::BayesTable::GetData $this isSaved]"
  if [::BayesTable::GetData $this isSaved] {
    ::BayesTable::WriteInfo $this [mc "Saved"]
  } else  {
    ::BayesTable::WriteInfo $this [mc "Not Saved"]
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::EditTable::Validate {this val} {
#
# PURPOSE: Procedure that is called each time something is writen in the
#          table. Sets variable 'isSaved' to false.
#
# PARAMETERS:
#   this -> Instance of table editor
#   val  -> Potential new value of the cell being validated
#
# RETURN: Allways return 1 (validation OK), except when Return key is pressed
#
#/////////////////////////////////////////////////////////////////////////////
  ::BayesTable::SetData $this isSaved 0
  if [regexp {^[^\n]*\n$} $val] {
    return 0
  } else {
    return 1
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::EditTable::Create {path} {
#
# PURPOSE: Creates a table editor instance
#
# PARAMETERS:
#   path  -> table editor's path (toplevel)
#
# RETURN: Instance created.
#
#/////////////////////////////////////////////////////////////////////////////

  set this ::Editor::$path
  if { [lsearch [namespace children ::EditTable] $this] != -1 } {
    error "$this already exists as a children of ::Editor"
  }

  # create
  set tl [::project::CreateForm -type Tables \
            -width 400 -height 180 \
            -iniconfig Tables ]
  set tlf [$tl getframe]
  
  set this [::BayesTable::Create $tlf.t -child EditTable \
      -argsWidget {-state normal} -OnWriteIni "::EditTable::WriteIni"]
  $tl configure -closecmd [list ::EditTable::OnDelete $this]
  pack $tlf.t -fill both -expand yes
  ::BayesTable::SetData $this f $tl

  namespace eval $this {
    variable data
  }
  upvar \#0 ${this}::data data
  set data(f) $tl

  ReadIni   $this
  Configure $this

  set t [::BayesTable::GetTkTable $this]
  $t configure -validate 1 -vcmd [list ::EditTable::Validate $this %S]
  bind $tl <<Paste>> [list ::EditTable::OnChangeRows $this]
  bind $tl <<Cut>>   [list ::EditTable::OnChangeRows $this]

  #Añadimos opciones específicas de EditTable
  ::BayesTable::AddOptionCmd $this "sepSQL1" "--" -where both
  ::BayesTable::AddOptionCmd $this "sepSQL2" "--" 
  ::BayesTable::AddOptionCmd $this "new"     "::EditTable::CmdNew"\
      -where tool -icon new -hint "[mc [list New white table]] (CTRL+N)"
  ::BayesTable::AddOptionCmd $this "open"    "::EditTable::CmdOpen"\
      -where tool -icon open -hint "[mc Open] (CTRL+A)"
  ::BayesTable::AddOptionCmd $this "save"    "::EditTable::CmdSave"\
      -where tool -icon save -hint "[mc Save] (CTRL+S)"
  $tl bind <Control-F4> [list $tl kill]
  return $this
}

proc ::EditTable::OnChangeRows {this} {
  Tolcon_Trace "Llamada a OnChangeRows"
  #set this [::BayesTable::GetInstance $w]
  ::BayesTable::SetData $this isSaved 0
  ::BayesTable::UpdateInfoRows $this
}


#/////////////////////////////////////////////////////////////////////////////
# File management commands (new, open, save, save as...)
#/////////////////////////////////////////////////////////////////////////////

#/////////////////////////////////////////////////////////////////////////////
  proc ::EditTable::CmdNew {this} {
#
# PURPOSE: Cleans table editor to edit a new white table
#
# PARAMETERS:
#   this  -> Instance of table editor
#
#/////////////////////////////////////////////////////////////////////////////
  if [string equal [CheckOpen $this] "cancel"] return
  ::BayesTable::CmdSelectAll $this
  ::BayesTable::CmdDelete $this
  InitState $this new
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::EditTable::UpdateListEdited {path} {
#
# PURPOSE: Updates list of last edited table files adding a new file path.
#
# PARAMETERS:
#   path  -> path of the last edited table file
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
 proc ::EditTable::CmdOpen {{this ""}} {
#
# PURPOSE: Asks for a table file path and if it's given opens it into the 
#          table editor window.
#
# PARAMETERS:
#   this -> Instance of table editor
#
#/////////////////////////////////////////////////////////////////////////////
  variable options
  variable typelist
  variable defExt

  if {$this == ""} {
    set parent .
  } else  {
    set parent [::BayesTable::GetTkTable $this]
    if [string equal [CheckOpen $this] "cancel"] return
  } 
  
  set path [tk_getOpenFile -title [mc "Open"] \
                       -parent $parent\
                       -initialdir $options(lastDir) \
                       -defaultextension $defExt -filetypes $typelist]
  
  if { $path != "" } {
    # abrimos solo si no esta ya abierto
    if { [namespace exists ::BayesTable] } {
      foreach ins [namespace children ::BayesTable::] {
        upvar \#0 ${ins}::data data
        if {[info exists data(nameFile)] && [string equal $data(nameFile) $path]} {
          $data(f) raise
		      set resp [tk_messageBox -type ok -icon warning \
		        -message [mc "The table is allready opened!"] ]
          return
        }
      }
    }
    if {$this == ""} {
      set this [New $path]
    }
    GetFromFile $this $path
  }

}


#/////////////////////////////////////////////////////////////////////////////
  proc ::EditTable::GetFromFile {this path} {
#
# PURPOSE: Loads a table file into the table editor window.
#
# PARAMETERS:
#   this -> Instance of table editor
#   path -> table file's path
#
#/////////////////////////////////////////////////////////////////////////////
  if [::BayesTable::Open $this $path] {
    ::BayesTable::SetData $this nameFile $path
    UpdateListEdited $path
  } else {
    ::BayesTable::SetData $this nameFile ""
  }
  upvar \#0 ${this}::data data
  $data(f) raise
  InitState $this open
  ::BayesTable::AdjustTable $this
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::EditTable::CheckAll {} {
#
# PURPOSE: Checks for files opened if anyone has changed and asks for saving.
#
#/////////////////////////////////////////////////////////////////////////////
  if { ![ catch { set lst [namespace children ::BayesTable] } msgerr] } {
    foreach ins $lst {
      upvar \#0 ${ins}::data data
      if {[string equal $data(namespace) "EditTable"]} {
        if {[CheckOpen $ins] eq "cancel"} {
	  return "cancel"
	}
      }
    }
  }
  return "ok"
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::EditTable::CheckOpen {this {type "yesnocancel"}} {
#
# PURPOSE: Checks if a file has changed and asks for saving.
#
# PARAMETERS:
#   this -> Instance of table editor
#   type -> type of messageBox to ask. (yesnocancel by default)
#
# RETURN: Returns user answer for saving.
#
#/////////////////////////////////////////////////////////////////////////////
  set nameFile [::BayesTable::GetData $this nameFile]
  set isSaved  [::BayesTable::GetData $this isSaved]
  # comprobamos si necesita ser guardado
  if {!$isSaved} {
    if {$nameFile != ""} {
      set name $nameFile
    } else  {
      set name "untitled.bst"
    }
    # preguntamos por conformidad
    set rq [tk_messageBox -type $type -icon warning -title [mc "Warning"]\
            -parent [::BayesTable::GetTkTable $this]\
            -message "[mc "Table %1\$s has changed!" $name].\n\
            [mc "Do you want to save changes?"]"] 
    switch -exact $rq {
      yes {
        if {$nameFile != "" } {
          FileSave $this $nameFile
        } else {
          CmdSaveAs $this $nameFile
        }
      }
      cancel {return cancel}
      default {return ok}
    }
  }
  return ok
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::EditTable::CmdSave {this} {
#
# PURPOSE: Saves data table actually opened in the editor. If doesn't have 
#          name, asks for ti (CmdSaveAs).
#
# PARAMETERS:
#   this -> Instance of table editor 
#
# RETURN: Returns 1 if file has been saved correctly or 0 if not.
#
#/////////////////////////////////////////////////////////////////////////////
  set nameFile [::BayesTable::GetData $this nameFile]

  if {$nameFile == ""} {
    set ok [CmdSaveAs $this]
  } else  {
    set ok [FileSave $this $nameFile]
  }
  return $ok
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::EditTable::CmdSaveAs {this {nf ""}} {
#
# PURPOSE: Saves table data actually opened in table editor asking for the
#          user to select a file to save it.
#
# PARAMETERS:
#   this -> Instance of table editor 
#   nf   -> name of file proposed
#
# RETURN: Returns 1 if file has been saved correctly or 0 if not.
#
#/////////////////////////////////////////////////////////////////////////////
  variable options
  variable typelist
  variable defExt
  set t [::BayesTable::GetTkTable $this]
    
  if {$nf == ""} {
    set dir $options(lastDir)
    set fil ""     
  } else  {
    set dir "[file dirname $nf]"
    set fil "[file tail $nf]"    
  }
  set path [tk_getSaveFile -title [mc "Save as"] -initialdir $dir \
       -initialfile $fil -parent [::BayesTable::GetTkTable $this] \
       -defaultextension $defExt -filetypes $typelist]
  if {![string equal $path ""] } {
    set ok [FileSave $this $path]
  } else {
    set ok 0
  }
  return $ok
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::EditTable::FileSave { this path } {
#
# PURPOSE: Saves table data actually opened in table editor in a text file.
#
# PARAMETERS:
#   this -> Instance of editor 
#   path -> name of file 
#
# RETURN: Returns 1 if file has been saved correctly or 0 if not.
#
#/////////////////////////////////////////////////////////////////////////////
  variable options

  if [::BayesTable::Save $this $path] {
    ::BayesTable::SetData $this nameFile $path
    InitState $this save
    # guardamos el ultimo directorio utilizado
    set options(lastDir) [file dirname $path]
    set ok 1
  } else  {
    set ok 0
  }
  return $ok
}


#/////////////////////////////////////////////////////////////////////////////
# INIFILE
#/////////////////////////////////////////////////////////////////////////////

#/////////////////////////////////////////////////////////////////////////////
  proc ::EditTable::ReadIni {this} {
#
# PURPOSE: Reads specific options from .ini file that are relative to table.
#          First of all, reads options from BayesTable. Actually there isn't
#          specific options. They are read for each instance.
#
# PARAMETERS:
#   this  -> Instance of table editor
#
#/////////////////////////////////////////////////////////////////////////////
  # Leer las opciones generales (del padre)
  ::BayesTable::ReadIni $this
  # Leer las opciones específicas
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::EditTable::ReadIniSpecial {} {
#
# PURPOSE: Reads common Edit Table options from .ini file. This options are
#          common for all instances of Table Editor. They are read just one
#          time.
#
#/////////////////////////////////////////////////////////////////////////////
  variable options
    
  set rini ::iniFile::Read
    
  set options(chEdited)   [$rini EditTable saveRecent  1]
  set options(numEdited)  [$rini EditTable numRecent   8]
  set options(edited)     [$rini EditTable recent      {}]
  set options(lastDir)    [$rini EditTable lastDir  ""]
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::EditTable::WriteIni {this} {
#
# PURPOSE: Writes specific options to .ini file. This procedure is called by
#          parent BayesGraph. This options are common for all instances of
#          Table Editor.
#
# PARAMETERS:
#   this  -> Instance of table editor
#
#/////////////////////////////////////////////////////////////////////////////
  variable options

  set wini ::iniFile::Write
  $wini EditTable saveRecent    $options(chEdited)
  $wini EditTable numRecent     $options(numEdited)
  $wini EditTable recent        $options(edited)
  $wini EditTable lastDir       $options(lastDir)
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::EditTable::Configure {this} {
#
# PURPOSE: Configures some options of TableEditor. First of all, configures
#          options from BayesTable. Actually there isn't specific options.
#
# PARAMETERS:
#   this  -> Instance of table editor
#
#/////////////////////////////////////////////////////////////////////////////
  # Configurar las opciones generales (del padre)
  ::BayesTable::Configure $this
  # Configurar las opciones específicas
}


#Read special initial options because are options relative with tolbase menu
::EditTable::ReadIniSpecial
