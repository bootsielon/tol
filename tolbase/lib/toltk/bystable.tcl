#/////////////////////////////////////////////////////////////////////////////
# FILE    : bystable.tcl
# PURPOSE : This file contains a namespace called BayesTable with its
#           functions. Its for creating a table and managing its events and 
#           changing its options. It is used as parent for other namespaces
#           that fills tables data. Its abstracts the general funcionality of
#           all kind of tables.
#/////////////////////////////////////////////////////////////////////////////


#/////////////////////////////////////////////////////////////////////////////
# TESTING
#/////////////////////////////////////////////////////////////////////////////
# Put if {1} for testing this file.
if {0} {
  package require Tk
  package require BWidget
  package require rbc
  Bitmap::use
  set toltk_script_path [file dirname [info script]]
  set toltk_images_path [file join $toltk_script_path images]
  lappend Bitmap::path $toltk_images_path
  package require msgcat
  namespace import msgcat::*
}

bind BysTable <Tab> {
  if { [%W index active col] < [expr {[%W cget -cols] -1}] } {
    ::tk::table::MoveCell %W 0 1
  } elseif { [%W index active row] < [expr {[%W cget -rows] -1}] } {
    ::tk::table::MoveCell %W 1 -[%W cget -cols]
  } else {
    %W insert rows end
    ::tk::table::MoveCell %W 1 -[%W cget -cols]
  }
  focus %W
  break
}

#/////////////////////////////////////////////////////////////////////////////
  namespace eval ::BayesTable {
#
# PURPOSE: The purpose of this namespace is organize code and functions.
#          A special namespace is created for each instance of BayesTable.
#
# VARIABLES:
#   tmpOpt  -> temporary array used to store the widgets and variables of the
#              options dialog. 
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpOpt
}


# Procedimientos
proc ::BayesTable::GetInstance {w} {
  return ::BayesTable::[winfo parent [winfo parent [winfo parent $w]]]
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::BayesTable::Create {path args} {
#
# PURPOSE: Creates an instance of a BayesTable. It creates the widget
#
# PARAMETERS:
#   path -> path where the new table is stored
#   args -> 
#          -OnWriteIni: 
#          -OnOptionCreate: 
#          -OnOptionDestroy: 
#          -OnOptionApply: 
#          -child: Child of the new table's Widget
#          -onlywidget: Boolean to create the table by itself, or
#                       create the table plus a button bar and a status bar.
#          -clientproc: 
#          -argsWidget: 
#          -fillproc: Procedure that will fill the table.
#
# RETURN: The new created BayesTable instance
#
#/////////////////////////////////////////////////////////////////////////////

  if {[expr {[llength $args] & 1}] != 0} {
    return -code error "Invalid number of arguments given to ctext\
           (uneven number): $args"
  }
  set Instance ::BayesTable::$path

  array set cmdArgs $args
  if {![info exists cmdArgs(-child)] } {
    return -code error "Child not specified"
  }
  
  # A namespace is created for each bayes table widget.

  namespace eval $Instance {
  
    # data -> array with information about the instance:
    #
    # options -> 
    #
    # widgets -> 
    #
    # matrix -> 
    #
    variable data
    variable options
    variable widgets
    variable matrix
  }

  upvar \#0 ${Instance}::data data
  upvar \#0 ${Instance}::widgets widgets
  upvar \#0 ${Instance}::options options

  if {[info exists cmdArgs(-OnWriteIni)] } {
    set data(proc,OnWriteIni) $cmdArgs(-OnWriteIni)
  }
  if {[info exists cmdArgs(-OnOptionCreate)] } {
    set data(proc,OnOptionCreate)  $cmdArgs(-OnOptionCreate)
  }
  if {[info exists cmdArgs(-OnOptionDestroy)] } {
    set data(proc,OnOptionDestroy)  $cmdArgs(-OnOptionDestroy)
  }
  if {[info exists cmdArgs(-OnOptionApply)] } {
    set data(proc,OnOptionApply)  $cmdArgs(-OnOptionApply)
  }

  # se crea el frame del widget bayesTable
  set widgets(this)  [frame $path -class bayesTable]
  set data(namespace) $cmdArgs(-child)
  bind $widgets(this) <Destroy> [list ::BayesTable::Destroy $Instance]

  set widgets(tb,path) [frame $widgets(this).tb -bd 2 -relief raised]
  set widgets(cl,path) [frame $widgets(this).cl -bd 2 -relief raised]
  set widgets(sb,path) [frame $widgets(this).sb -relief groove -bd 1 -height 7]
  # Se crean la barra de botones y la barra de estado
  if { ![info exists cmdArgs(-onlywidget)] ||
       ![string equal $cmdArgs(-onlywidget) "yes"]} {
    CreateToolBar $Instance
    CreateStatusBar $Instance
    set data(onlywidget) 0
  } else  {
    set data(onlywidget) 1
  }
  # Se crea la tabla y se rellena
  if {[info exists cmdArgs(-clientproc)] == 1} {
    eval $cmdArgs(-clientproc) $Instance
  } else {
    if {[info exists cmdArgs(-argsWidget)] == 1} {  
      eval CreateWidget $Instance $widgets(cl,path) $cmdArgs(-argsWidget)
    } else  {
      CreateWidget $Instance $widgets(cl,path)
    }
  }

  if {[info exists cmdArgs(-fillproc)] == 1} {
    eval $cmdArgs(-fillproc) $Instance
  }
  if {[info exists cmdArgs(-fillproc)] == 1} {
    AdjustTable $Instance
  }
  if $data(onlywidget) {
    grid $widgets(cl,path) -sticky news
    grid columnconfigure $path 0 -weight 1
    grid rowconfigure    $path 0 -weight 1 
  } else {
    grid $widgets(tb,path) -sticky ew
    grid $widgets(cl,path) -sticky news
    grid $widgets(sb,path) -sticky ew
    grid columnconfigure $path 0 -weight 1
    grid rowconfigure    $path 1 -weight 1
  }


  return $Instance
}

proc ::BayesTable::CreateWidget {this w args} {
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::options options

  global tcl_platform toltk_images_path
  global toltk_options

  if { [string equal $tcl_platform(platform) "windows"] } {
    set tvcur [file join ${toltk_images_path} treeview.cur]
    regsub -all " " $tvcur {\ } tvcur
    set bordercursor @$tvcur
  } else {
    set bordercursor \
        "@[file join ${toltk_images_path} treeview.xbm] [file join ${toltk_images_path} treeview_m.xbm] black white"
  }
  set cmdArgs(-state) "disabled"
  array set cmdArgs $args
  ScrolledWindow $w.sw
  #Creación de la tabla
  set t [table $w.sw.t -variable ${this}::matrix -state $cmdArgs(-state)\
                       -multiline 0 -selectmode extended -resizeborders col \
                       -bordercursor $bordercursor -relief groove \
                       -browsecommand [list ::BayesTable::Active $this %W %S %s] \
                       -validate 1 -vcmd [list ::BayesTable::Validate $this] \
                       -bg white -font $toltk_options(fonts,Label) -selecttitles 0 \
                       -titlecols 1 -titlerows 1 -colseparator \t -rowseparator \n]
  #Las dos últimas filas son configurables

  bindtags $t [eval list [bindtags $t] BysTable]

  set widgets(tktable) $t
  set state $cmdArgs(-state)
  #Configuramos los tags de las celdas
  $t tag configure usr1   -bd 2                -relief raised -font $toltk_options(fonts,Title1)
  $t tag configure usr2   -bg grey30 -fg white -relief flat   -font $toltk_options(fonts,Title1)

  $t tag configure title  -bd 1 -bg $toltk_options(colors,SystemButtonFace) -fg black\
                          -relief $toltk_options(relief,ridge) -anchor c -state $state
  $t tag configure active -bd 1 -bg White -fg black -relief solid -anchor w\
                          -font $toltk_options(fonts,Title1)
  $t tag configure sel    -bd 1;# -fg black -bg Azure2
  $t tag raise usr1
  $t tag raise usr2

  $w.sw setwidget $t
  pack $w.sw -expand true -fill both


  #Creación de un menú contextual
  set menu [menu $w.sw.m -tearoff 0]
  set widgets(menu) $menu
  set mi [menu $w.sw.m.ins -tearoff 0]
  set me [menu $w.sw.m.eli -tearoff 0]
  set widgets(cl,menu) $menu
  $menu add command -label [mc "Select all"] -accelerator CTRL+E\
        -command [list ::BayesTable::CmdSelectAll $this]
  $menu add command -label [mc "Delete"] -accelerator SUPR -state $state \
        -command [list ::BayesTable::CmdDelete    $this]
  $menu add separator
  $menu add cascade -label [mc "Insert"]    -state $state -menu $mi
  $menu add cascade -label [mc "Eliminate"] -state $state -menu $me
  $mi add command -label [mc "Rows"]\
        -command [list ::BayesTable::CmdModifyRowsCols $this insert rows]  
  $mi add command -label [mc "Columns"]\
        -command [list ::BayesTable::CmdModifyRowsCols $this insert cols]  
  $me add command -label [mc "Rows"]\
        -command [list ::BayesTable::CmdModifyRowsCols $this delete rows]  
  $me add command -label [mc "Columns"]\
        -command [list ::BayesTable::CmdModifyRowsCols $this delete cols]  

  proc OnB3 { m w x y X Y} {
     if ![llength [$w border mark $x $y]] {
       tk_popup $m $X $Y
     }
  }
  bind $t <ButtonPress-3> "::BayesTable::OnB3 $menu %W %x %y %X %Y"
  bind $t <Double-ButtonPress-1> [list ::BayesTable::OnDblClickHeader $this]
  bind $t <F3>        [list ::BayesTable::CmdFind      $this]  
  bind $t <Delete>    [list ::BayesTable::CmdDelete    $this]
  bind $t <Return>    [list ::BayesTable::CmdNext      $this]
  bind $t <Control-KeyRelease> [list ::BayesTable::OnControlKey $this %K %k]
}

#///////////////////////////////////////////////////////////////////////////
  proc ::BayesTable::OnControlKey {this keysym keycode} {
#
# PURPOSE: Control the "Control + Key Press"
#
# PARAMETERS: this ->  Instance of BayesTable
#             keysym -> The keysym corresponding to the event, 
#                       substituted as a textual string
#             keycode -> The keycode field from the event
#
#///////////////////////////////////////////////////////////////////////////
  switch [string toupper $keysym] {
    E { ::BayesTable::CmdSelectAll $this }
    F { ::BayesTable::CmdFind      $this }
    N { ::EditTable::CmdNew        $this }
    A { ::EditTable::CmdOpen       $this }
    S { ::EditTable::CmdSave       $this }
    C { ::BayesTable::CmdCopy      $this }
    V { ::BayesTable::CmdPaste     $this }
  }   
  if {$keysym == "X"} { tk_tableCut [GetTkTable $this] }
}


proc ::BayesTable::CreateToolBar {this} {
  upvar \#0 ${this}::widgets widgets
    
  set f $widgets(tb,path)
  set n_b [list \
        "home"        "AdjustTable" ""\
        "sep0"        "--"          ""\
        "leftEnd"     "CmdFirst"    "[mc First]" \
        "leftNext"    "CmdBack"     "[mc Back] " \
        "rightNext"   "CmdNext"     "[mc Next] " \
        "rightEnd"    "CmdLast"     "[mc Last] " \
        "sep1"        "--"          ""\
        "bcopy"        "CmdCopy"     "[mc Copy] (CTRL+C)" \
        "bfind"        "CmdFind"     "[mc Find] (CTRL+F)" \
        "bfilter"      "CmdFilter"   "[mc Filter] (ALT+F)" \
        "sep2"        "--"          ""\
        "ordAsc"      "CmdOrdAsc"   ""\
        "ordDes"      "CmdOrdDes"   ""\
        "sep3"        "--"          ""\
        "tools"       "CmdOptions" "[mc Options]"\
  ]
  foreach {n_img n_proc n_hint} $n_b {
    if {[string equal $n_proc "--"]} {
      Separator $f.$n_img -orient vertical
    } else {
      set widgets(tb,$n_img) [Button $f.$n_img -image [::Bitmap::get ${n_img}] -bd 2 \
           -relief link -helptext $n_hint -command [list ::BayesTable::$n_proc $this]]
    }
    pack $f.$n_img -side left -pady 2 -fill y -padx 1
  }
}

proc ::BayesTable::CreateStatusBar {this} {
  upvar \#0 ${this}::widgets widgets

  set f $widgets(sb,path)
  # frame y label para barra de estado
  set widgets(sb,lRows) [label $f.rows -relief groove -bd 1 -width 16]
  set widgets(sb,lPosi) [label $f.posi -relief groove -bd 1 -width 16]
  set widgets(sb,lInfo) [label $f.info -relief groove -bd 1 -width 60 -anchor w -padx 4]
  set widgets(sb,rell) [label $f.rell -relief groove -bd 1]

  pack $f.rows $f.posi $f.info -side left
  pack $f.rell -expand true -fill x
}

proc ::BayesTable::Save {this path {replace 0}} {
  if { ![ catch { set fd [open $path w] } msgerr] } {
    set table [GetTkTable $this]
    set numCols [$table cget -cols]
    set list    [$table get @0,0 end]
    if $replace {
      set list [ListReplace $list {} "?"]
    }
    set struct "{$numCols {\n} {} { ; }} {1 { ; } {} {}}"
    set txt [ListToStrStruct $list $struct]  
    #@ puts $fd $txt
    close $fd
    return 1
  }
  error $msgerr
}

proc ::BayesTable::Open {this path} {
  set table [GetTkTable $this]

  if { ![catch {set fd [open $path r] } msgerr] } {
    set txt [read -nonewline $fd]
    set rsep [$table cget -rowseparator]
    set csep [$table cget -colseparator]
    $table configure -rowseparator "\n" -colseparator ";"
    # Llamamos al manejador de pegado de datos en la tabla
    PasteHandler $table 0,0 $txt
    $table configure -rowseparator $rsep -colseparator $csep
    # Los archivos de TOL que se abren tienen el separador de 
    #columna despues de la ultima columna, por eso quitamos una columna
    # Faltaría comprobar si esto ocurre en todos los tipos de archivos
    #que se abren con el editor de tablas
    $table configure -cols [expr [$table cget -cols] -1]
    close $fd
    return 1
  }
  error $msgerr
}

proc ::BayesTable::SetMatrixInfo {this dim colTypes} {
  upvar \#0 ${this}::options options
  set tktable [GetTkTable $this]
  $tktable configure -rows [lindex $dim 0] -cols [lindex $dim 1]
}

proc ::BayesTable::AddOptionCmd {this name proc args} {
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data    data

  set f $widgets(tb,path)
  array set cmdArgs $args

  if {![info exists cmdArgs(-where)] || 
      [string equal $cmdArgs(-where) "both"] ||
      [string equal $cmdArgs(-where) "tool"] } {
    if {[string equal $proc "--"]} {
      Separator $f.$name -orient vertical
    } else {
      set widgets(tb,$name) [Button $f.$name -bd 2 -relief link -command [list $proc $this]]
      if [info exists cmdArgs(-icon)] {
        $f.$name configure -image [::Bitmap::get $cmdArgs(-icon)]
      }
      if [info exists cmdArgs(-hint)] {
        $f.$name configure -helptext $cmdArgs(-hint)
      }
    }
    pack $f.$name -side left -pady 2 -fill y -padx 1
  }

  if {[info exists cmdArgs(-where)] && 
      ([string equal $cmdArgs(-where) "both"] ||
       [string equal $cmdArgs(-where) "menu"]) } {
    Tolcon_Trace "También en el menú contextual"
  }
}

proc ::BayesTable::GetTkTable {this} {
  upvar \#0 ${this}::widgets widgets
  return $widgets(tktable)
}

proc ::BayesTable::GetMenu {this} {
  upvar \#0 ${this}::widgets widgets
  return $widgets(menu)
}

proc ::BayesTable::UnPackTkTable {this} {
  set t [GetTkTable $this]
  pack forget [winfo parent $t]
  #pack forget $t
}

proc ::BayesTable::PackTkTable {this} {
  set t [GetTkTable $this]
  pack [winfo parent $t] -expand yes -fill both
  #pack $t -expand yes -fill both
}


proc ::BayesTable::GetClientArea {this} {
  upvar \#0 ${this}::widgets widgets
  return $widgets(cl,path)
}

proc ::BayesTable::GetContextMenu {this} {
  upvar \#0 ${this}::widgets widgets
  return $widgets(cl,menu)
}

proc ::BayesTable::WriteRows {this text} {
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data    data
  if !$data(onlywidget) {
    $widgets(sb,lRows) configure -text $text
  }
}

proc ::BayesTable::WritePosi {this text} {
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data    data
  if !$data(onlywidget) {
    $widgets(sb,lPosi) configure -text $text
  }
}

proc ::BayesTable::WriteInfo {this text} {
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data    data
  if !$data(onlywidget) {
    $widgets(sb,lInfo) configure -text $text
  }
}

proc ::BayesTable::SetElement {this i j value {asString 0}} {
  upvar \#0 ${this}::matrix matrix 
  if $asString {
    set matrix($i,$j) $value
  } else  {
    set matrix($i,$j) [::TolConsole::Format $value]
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::BayesTable::SetSpans {this fil col fn cn} {
#
# PURPOSE: Combination of cells
#
# PARAMETERS:
#   this -> Instance of BayesTable
#   fil  -> Rows of table
#   col  -> Column of table
#   fn   -> Number of rows
#   cn   -> Number of rows 
#
#/////////////////////////////////////////////////////////////////////////////
  set tabla [GetTkTable $this]
  $tabla spans $fil,$col $fn,$cn
}


proc ::BayesTable::GetElement {this i j} {
  upvar \#0 ${this}::matrix matrix
  return $matrix($i,$j)
}

proc ::BayesTable::GetLocElements {this} {
  return ${this}::matrix
}

proc ::BayesTable::Active {this W S s} {
  upvar \#0 ${this}::options opt

  #Actualizar información de la fila y columna actual
  set row [lindex [split $S ,] 0]
  set col [lindex [split $S ,] 1]
  WritePosi $this "[mc Row]: $row, [mc Col]: $col"

  #Rehacer completo porque las filas y columnas de título pueden ser varias
  #return

  for {set i 0} {$i < $opt(titlerows)} {incr i} {
    for {set j 0} {$j < $opt(titlecols)} {incr j} {
      regsub (.*), $s $i, otc ; # old table col
      regsub ,(.*) $s ,$j otr ; # old table row
      if {$otc != "" && $otr !=""} {
        $W tag cell title $otc $otr
      }

      regsub (.*), $S $i, tc
      regsub ,(.*) $S ,$j tr
      if { ![string equal $tc "0,0"] && ![string equal $tr "0,0"]} {
        $W tag cell usr1 $tc $tr
      } else {
        if {[string equal $tc "0,0"]} {
          $W tag cell usr2 $tr
        } else {
          $W tag cell usr2 $tc
        }
      }
    }
  }

}

proc ::BayesTable::OnDblClickHeader { this } {
  set tabla [GetTkTable $this]
  # celda seleccionada
  set cordenada [$tabla index active]
  if {$cordenada != ""} {
    set row [lindex [split $cordenada ,] 0]
    set col [lindex [split $cordenada ,] 1]
    # comprobamos si hemos pinchado en la cabereca (row = 0)
    if {$row == 0} {
      AdjustColumn $this $col
    }
  }
}

proc ::BayesTable::AdjustColumn {this column} {
  set tbl [GetTkTable $this]
  # control de spnas
  set find 0
  set lstSpan [$tbl spans]
  if { [llength $lstSpan] } {
    set find 1  
    set lstInd ""
    set ok 1
    # lista con los indices
    for {set x 0} {$x < [expr [llength $lstSpan] -1]} {incr x 2} {
	  lappend lstInd [lindex $lstSpan $x]
    }
  }
  # buscamos ancho de columna
  set maxLenFin 1
  for {set i [$tbl cget -roworigin]} {$i <= [$tbl cget -rows]} {incr i} {
    set maxLen [string length [$tbl get $i,$column]]
    # comprobamos si la celda esta expandida horizontalmente
    if { $find } {
      set posi [lsearch $lstInd $i,$column]
      if [expr $posi > -1] {
        set fc [lindex $lstSpan [expr $posi * 2 + 1]]
        set nc [string range $fc [expr [string first , $fc] + 1] end]
        set maxLen [expr $maxLen / ($nc+1) + 3] 
      }
    }
    if {$maxLen > $maxLenFin} {
      set maxLenFin $maxLen
    } 
  }
  # controlamos los spans
  $tbl width $column [expr $maxLenFin + 1]
}

proc ::BayesTable::AdjustTable {this} {
  set table [GetTkTable $this]
  for {set i [$table cget -colorigin]} {$i < [$table cget -cols]} {incr i} {
    ::BayesTable::AdjustColumn $this $i
  }
}

proc ::BayesTable::Destroy {this} {
  WriteIni $this
  namespace delete $this
}

proc ::BayesTable::SetWidget {this key widget} {
  upvar \#0 ${this}::widgets widgets
  set widgets(cl,$key) $widget
}

proc ::BayesTable::GetWidget {this key} {
  upvar \#0 ${this}::widgets widgets
  return $widgets(cl,$key)
}

proc ::BayesTable::SetData {this key value} {
  upvar \#0 ${this}::data data
  set data($key) $value
}

proc ::BayesTable::GetData {this key} {
  upvar \#0 ${this}::data data
  return $data($key)
}

proc ::BayesTable::GetLocData {this key} {
  return ${this}::data($key)
}

proc ::BayesTable::SetOption {this key value} {
  upvar \#0 ${this}::options opt
  set opt($key) $value
}

proc ::BayesTable::GetOption {this key} {
  upvar \#0 ${this}::options opt
  return $opt($key)
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::BayesTable::ReadIni {this} {
#
# PURPOSE: Reads general table options from .ini file.
#
# PARAMETERS:
#   this  -> Instance of BayesTable
#
#/////////////////////////////////////////////////////////////////////////////
  global toltk_options
  upvar \#0 ${this}::data data

  set rini ::iniFile::Read
  set key $data(namespace)
  SetOption $this bg           [$rini $key bg white]
  SetOption $this font         [$rini $key font $toltk_options(fonts,Label)]
  SetOption $this titlecols    [$rini $key titlecols     1]
  SetOption $this titlerows    [$rini $key titlerows     1]
  SetOption $this selecttitles [$rini $key selecttitles  1]
  SetOption $this colSepType   [$rini $key colSepType TAB]
  SetOption $this colSeparator [$rini $key colSeparator ""]
  SetOption $this rowSepType   [$rini $key rowSepType RET]
  SetOption $this rowSeparator [$rini $key rowSeparator ""]
}


proc ::BayesTable::WriteIni {this} {
  upvar \#0 ${this}::data    data
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::options opt

  set wini ::iniFile::Write
  set key $data(namespace)

  if !$data(onlywidget) {
    foreach op {bg font titlecols titlerows selecttitles} {
      $wini $key $op $opt($op)
    }
    foreach op {colSepType colSeparator rowSepType rowSeparator} {
      $wini $key $op $opt($op)
    }
  }
  if [info exists ${this}::data(proc,OnWriteIni)] {
    $data(proc,OnWriteIni) $this
  }
  ::iniFile::Flush
}


proc ::BayesTable::Configure {this} {
  OptionsApplyStyle  $this
  OptionsApplyTitle  $this
  OptionsApplyExport $this
}

proc ::BayesTable::OptionsApply {this} {
  upvar \#0 ${this}::data    data
  #Save options before apply
  OptionsSet $this
  OptionsApplyStyle  $this
  OptionsApplyTitle  $this
  OptionsApplyExport $this
  if [info exists ${this}::data(proc,OnOptionApply)] {
    $data(proc,OnOptionApply) $this
  }

}

proc ::BayesTable::OptionsApplyStyle {this} {
  upvar \#0 ${this}::options opt
  set table [GetTkTable $this]
  foreach op {bg font} {
    $table configure -$op $opt($op)
  }
}

proc ::BayesTable::OptionsApplyTitle {this} {
  upvar \#0 ${this}::options opt
  set table [GetTkTable $this]
  foreach op {titlecols titlerows selecttitles} {
    $table configure -$op $opt($op)
  }
  UpdateInfoRows $this
}

proc ::BayesTable::UpdateInfoRows {this} {
  upvar \#0 ${this}::options opt
  set table [GetTkTable $this]
  set dataRows [$table cget -rows]
  WriteRows $this "$dataRows [mc rows] ($opt(titlerows) [mc title])"
  #Tolcon_Trace "En UpdateInfoRows. rows: $dataRows"
}

proc ::BayesTable::OptionsApplyExport {this} {
  upvar \#0 ${this}::options opt
  set table [GetTkTable $this]

  switch $opt(colSepType) {
    RET   { $table configure -colseparator \n }
    TAB   { $table configure -colseparator \t }
    OTHER { $table configure -colseparator $opt(colSeparator) }
  }
  switch $opt(rowSepType) {
    RET   { $table configure -rowseparator \n }
    TAB   { $table configure -rowseparator \t }
    OTHER { $table configure -rowseparator $opt(rowSeparator) }
  }

}

proc ::BayesTable::CmdOptions {this} {
  global toltk_options
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data data
  set path $widgets(this)
  set dialog [Dialog $path.t -title [mc "Table Options"] -modal local -parent $path]
  wm protocol $dialog WM_DELETE_WINDOW [list ::BayesTable::OptionsDestroy $this $dialog]
  wm resizable $dialog 0 0
  set w [$dialog getframe]
  set w_tabset [NoteBook $w.nb -tabbevelsize 8 -tabpady {2 6} -font {Arial 8}] 
  #insertar tabset
  set num_i 0
  set lab_i General
  $w_tabset insert $num_i $lab_i -text "  [mc $lab_i]  " -background \#d9d9d9 
  set tab_i [$w_tabset getframe $lab_i]
  $tab_i configure -borderwidth 2 -background \#d9d9d9   
  set f$num_i [frame $tab_i.f]
  pack $tab_i.f -fill both -expand yes  
  $w_tabset raise [$w_tabset pages 0]
  
  OptionsCreate $this $f0
  #Crear opciones especificas 
  if [info exists ${this}::data(proc,OnOptionCreate)] {
    $data(proc,OnOptionCreate) $this $w_tabset
  }  

  set w_bbox \
          [ButtonBox $w.bb -orient horizontal -spacing 10 -padx 0 -pady 0 \
          -homogeneous false]
  $w_bbox add -text [mc "Accept"] -relief link \
         -helptext [mc "Accept"] \
          -image [::Bitmap::get accept] -compound left -padx 5 -pady 1 \
          -command  [list ::BayesTable::OptionsOK $this $dialog]
  $w_bbox add -text [mc "Cancel"] -relief link \
          -helptext [mc "Cancel"]\
          -image [::Bitmap::get cancelx] -compound left -padx 5 -pady 1 \
          -command [list ::BayesTable::OptionsDestroy $this $dialog]
  $w_bbox add -text [mc "Apply"] -relief link \
          -helptext [mc "Apply"]\
          -image [::Bitmap::get apply] -compound left -padx 5 -pady 1 \
          -command [list ::BayesTable::OptionsApply $this]    
  
  pack $w_bbox -fill x -expand false -side bottom
  pack $w_tabset -fill both -expand true
    
  $dialog draw
}

proc ::BayesTable::OptionsCreate {this frame} { 
  set w_tabset [NoteBook $frame.nb -tabbevelsize 8 -tabpady {2 6} -font {Arial 8}] 
  set num_i 0
  foreach lab_i [list Export Style] {
    $w_tabset insert $num_i $lab_i -text "  [mc $lab_i]  " -background \#d9d9d9 
    set tab_i [$w_tabset getframe $lab_i]
    $tab_i configure -borderwidth 2 -background \#d9d9d9
    incr num_i    
    set f$num_i [frame $tab_i.f]
    pack $tab_i.f -fill both -expand yes  
  }
  $w_tabset raise [$w_tabset pages 0]
  
  OptionsGet           $this
  OptionsCreateExport  $this $f1
  OptionsCreateStyle   $this $f2
  #OptionsInit  $this
  OptionsCheck $this
  #OptionsSet   $this

  $w_tabset compute_size
  grid $w_tabset -sticky news  
  grid rowconfigure    $frame 0 -weight 1
  grid columnconfigure $frame 0 -weight 1
}

proc ::BayesTable::OptionsCreateStyle {this w} {
  variable tmpOpt

  TitleFrame $w.tf1 -text [mc "Style"]
  set f1 [$w.tf1 getframe]

  label $f1.lFont -text [mc "Font"]:
  ::Objects::FontCreate ::BayesTable::tmpOpt(var,font) $f1.bFont

  label $f1.lColor -text [mc "Color"]:
  ::Objects::ColorCreate ::BayesTable::tmpOpt(var,bg) $f1.fColor
  frame $f1.f -width 25

  grid $f1.f $f1.lFont $f1.bFont $f1.lColor $f1.fColor -sticky w -padx 2 -pady 1

  grid rowconfigure    $f1 3 -weight 1
  grid columnconfigure $f1 4 -weight 1
  grid columnconfigure $f1 2 -pad   20

  TitleFrame $w.tf2 -text [mc "Title"]
  set f2 [$w.tf2 getframe]

  label $f2.lTitleRows -text [mc "Number of rows to use as title area"]:
  SpinBox $f2.sbTitleRows -range {0 3} -width 4 -justify right -editable false\
      -textvariable ::BayesTable::tmpOpt(var,titlerows)
  label $f2.lTitleCols -text [mc "Number of columns to use as title area"]:
  SpinBox $f2.sbTitleCols -range {0 3} -width 4 -justify right -editable false\
      -textvariable ::BayesTable::tmpOpt(var,titlecols)
  checkbutton $f2.chSelectTitles -text [mc "Allow title cells in the selection"] \
     -variable ::BayesTable::tmpOpt(var,selecttitles)

  frame $f2.f -width 25
  grid $f2.f $f2.lTitleRows $f2.sbTitleRows -sticky w -padx 2
  grid ^     $f2.lTitleCols $f2.sbTitleCols -sticky w -padx 2
  grid ^     $f2.chSelectTitles -           -sticky w -padx 2

  grid rowconfigure    $f2 3 -weight 1
  grid columnconfigure $f2 3 -weight 1

  grid $w.tf1 -sticky news -padx 2
  grid $w.tf2 -sticky news -padx 2
  grid rowconfigure    $w 0 -weight 1
  grid rowconfigure    $w 1 -weight 3
  grid columnconfigure $w 0 -weight 1
}

proc ::BayesTable::OptionsCreateExport {this w} {
  variable tmpOpt

  TitleFrame $w.tf1 -text [mc "Columns separator"]
  set f1 [$w.tf1 getframe]
  set tmpOpt(widget,rbColSep1) [radiobutton $f1.rbColSep1 -text [mc Tab] \
     -value TAB -highlightthickness 0 -pady 0 \
     -variable ::BayesTable::tmpOpt(var,colSepType) \
     -command [list ::BayesTable::OptionsCheckColSepType $this]]
  set tmpOpt(widget,rbColSep2) [radiobutton $f1.rbColSep2 -text [mc Return] \
     -value RET -highlightthickness 0 -pady 0 \
     -variable ::BayesTable::tmpOpt(var,colSepType) \
     -command [list ::BayesTable::OptionsCheckColSepType $this]]
  radiobutton $f1.rbColSepN -text [mc Other]: \
     -value OTHER -highlightthickness 0 -pady 0 \
     -variable ::BayesTable::tmpOpt(var,colSepType) \
     -command [list ::BayesTable::OptionsCheckColSepType $this]
  set tmpOpt(widget,colSeparator) [Entry $f1.eColSep \
     -width 5 -justify right -textvariable ::BayesTable::tmpOpt(var,colSeparator)]
  frame $f1.f -width 25
  #grid $f1.f $f1.rbColSep1 -           -sticky nw -padx 2 -pady 0
  #grid ^     $f1.rbColSepN $f1.eColSep -sticky w  -padx 2 -pady 0
  #grid rowconfigure    $f1 2 -weight 1
  #grid columnconfigure $f1 2 -weight 1
  grid $f1.f $f1.rbColSep1 $f1.rbColSep2 $f1.rbColSepN $f1.eColSep -sticky w  -padx 2 -pady 0
  grid rowconfigure    $f1 1 -weight 1
  grid columnconfigure $f1 4 -weight 1

  TitleFrame $w.tf2 -text [mc "Rows separator"]
  set f2 [$w.tf2 getframe]
  set tmpOpt(widget,rbRowSep1) [radiobutton $f2.rbRowSep1 -text [mc Return] \
     -value RET -highlightthickness 0 -pady 0 \
     -variable ::BayesTable::tmpOpt(var,rowSepType) \
     -command [list ::BayesTable::OptionsCheckRowSepType $this]]
  set tmpOpt(widget,rbRowSep2) [radiobutton $f2.rbRowSep2 -text [mc Tab] \
     -value TAB -highlightthickness 0 -pady 0 \
     -variable ::BayesTable::tmpOpt(var,rowSepType) \
     -command [list ::BayesTable::OptionsCheckRowSepType $this]]
  radiobutton $f2.rbRowSepN -text [mc Other]: \
     -value OTHER -highlightthickness 0 -pady 0 \
     -variable ::BayesTable::tmpOpt(var,rowSepType) \
     -command [list ::BayesTable::OptionsCheckRowSepType $this]
  set tmpOpt(widget,rowSeparator) [Entry $f2.eRowSep \
     -width 5 -justify right -textvariable ::BayesTable::tmpOpt(var,rowSeparator)]
  checkbutton $f2.chOmitLast -text [mc "Omit last row separator (Excel)"] \
     -variable ::BayesTable::tmpOpt(var,omitLast)
  frame $f2.f -width 25
  grid $f2.f $f2.rbRowSep1 $f2.rbRowSep2 $f2.rbRowSepN $f2.eRowSep -sticky w -padx 2 -pady 0
  #grid ^     $f2.chOmitLast -            -           -sticky w -padx 2 -pady 0
  grid rowconfigure    $f2 1 -weight 1
  grid columnconfigure $f2 4 -weight 1

  grid $w.tf1 -sticky news -padx 2
  grid $w.tf2 -sticky news -padx 2
  grid rowconfigure    $w 0 -weight 1
  grid rowconfigure    $w 1 -weight 3
  grid columnconfigure $w 0 -weight 1
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::BayesTable::OptionsCheck {this} {
#
# PURPOSE: Checks for all widgets from dialog options that need to be checked
#          for enabling/disabling other associated controls. 
#
# PARAMETERS:
#   this   -> Instance of BayesTable
#
#/////////////////////////////////////////////////////////////////////////////
  ::BayesTable::OptionsCheckColSepType $this
  ::BayesTable::OptionsCheckRowSepType $this
}

proc ::BayesTable::OptionsCheckColSepType {this} {
  variable tmpOpt

  switch $tmpOpt(var,colSepType) {
    TAB   {
      focus $tmpOpt(widget,rbColSep1)
      $tmpOpt(widget,colSeparator) configure -state disabled 
    }
    RET   {
      focus $tmpOpt(widget,rbColSep2)
      $tmpOpt(widget,colSeparator) configure -state disabled 
    }
    OTHER { $tmpOpt(widget,colSeparator) configure -state normal }
  }
}

proc ::BayesTable::OptionsCheckRowSepType {this} {
  variable tmpOpt

  switch $tmpOpt(var,rowSepType) {
    RET   {
      focus $tmpOpt(widget,rbRowSep1)
      $tmpOpt(widget,rowSeparator) configure -state disabled 
    }
    TAB   {
      focus $tmpOpt(widget,rbRowSep2)
      $tmpOpt(widget,rowSeparator) configure -state disabled 
    }
    OTHER { $tmpOpt(widget,rowSeparator) configure -state normal }
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::BayesTable::OptionsGet {this} {
#
# PURPOSE: Initializes all options variables and stores them into the
#          temporary 'tmpOpt' array. 
#
# PARAMETERS:
#   this   -> Instance of BayesTable
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::options opt
  variable tmpOpt

  set table [GetTkTable $this]
  foreach op {bg font titlecols titlerows selecttitles} {
    set tmpOpt(var,$op) [$table cget -$op]
  }
  foreach op {colSepType colSeparator rowSepType rowSeparator} {
    set tmpOpt(var,$op) $opt($op)
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::BayesTable::OptionsSet {this} {
#
# PURPOSE: Commits all options values from the temporary 'tmpOpt' array to
#          other structure.
#
# PARAMETERS:
#   this   -> Instance of BayesTable
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::options options
  variable tmpOpt

  regsub -all var, [array names ::BayesTable::tmpOpt var,*] "" optNames
  foreach it $optNames {
    set options($it) $tmpOpt(var,$it)
  }
}

proc ::BayesTable::OptionsOK {this dialog} {
  OptionsApply   $this
  OptionsDestroy $this $dialog
}

proc ::BayesTable::OptionsDestroy {this dialog} {
  upvar \#0 ${this}::data data
  variable tmpOpt
  #Destruir opciones especificas
  if [info exists ${this}::data(proc,OnOptionDestroy)] {
    $data(proc,OnOptionDestroy) $this
  }
  array unset tmpOpt
  destroy $dialog
}


proc ::BayesTable::CmdFirst {this} {
  set table [GetTkTable $this]
  $table see origin
}

proc ::BayesTable::CmdBack {this} {
  set table [GetTkTable $this]
  ::tk::table::MoveCell $table -1  0
  #set cell [split [$table index active] ,]
  #$table selection clear all
  #$table activate  [expr [lindex $cell 0] -1],[lindex $cell 1]  
  #$table selection set active
  #$table see active
}

proc ::BayesTable::CmdNext {this} {
  set table [GetTkTable $this]
  ::tk::table::MoveCell $table 1  0
}

proc ::BayesTable::CmdLast {this} {
  set table [GetTkTable $this]
  $table see end
}

proc ::BayesTable::CmdSelectAll {this} {
  set table [GetTkTable $this]
  ::tk::table::SelectAll $table
  #$table selection set origin end
}

proc ::BayesTable::CmdDelete {this} {
  set table [GetTkTable $this]
  if {[llength [$table curselection]] > 1 } {
    $table curselection {}
    $table selection clear all
  }
}

proc ::BayesTable::CmdModifyRowsCols {this oper item} {
  if {![string equal $oper "insert"] && ![string equal $oper "delete"]} { return }
  if {![string equal $item "rows"]   && ![string equal $item "cols"]  } { return }

  set table [GetTkTable $this]
  set cells [$table curselection]
  if ![llength $cells] {
    set cells {0,0}
  }
  set rowIni [lindex [split [lindex $cells 0] ","] 0]
  set colIni [lindex [split [lindex $cells 0] ","] 1]
  set rowEnd [lindex [split [lindex $cells end] ","] 0]
  set colEnd [lindex [split [lindex $cells end] ","] 1]
  set rows [expr $rowEnd-$rowIni+1]
  set cols [expr $colEnd-$colIni+1]

  switch $item {
    rows {
      switch $oper {
        insert { $table insert rows -holdselection $rowIni -$rows }
        delete { $table delete rows -holdselection $rowIni $rows }
      }
    }
    cols {
      switch $oper {
        insert { $table insert cols -holdselection $colIni -$cols }
        delete { $table delete cols -holdselection $colIni $cols }
      }
    }
  }
  UpdateInfoRows $this
}


proc ::BayesTable::CmdCopy {this} {
  # before delete clipboard 
  clipboard clear

  set table [GetTkTable $this]
  tk_tableCopy $table
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::BayesTable::CmdPaste {this} {
#
# PURPOSE: Paste information in the cell
#
# PARAMETERS:
#   this -> Instance of Bayes Table
#
#/////////////////////////////////////////////////////////////////////////////
  set table [GetTkTable $this]
  set txt [clipboard get]
  set p   [$table index active]
  if {![string length $p]} {
    set p 0,0
  }
  PasteHandler $table $p $txt
}

#/////////////////////////////////////////////////////////////////////////////
proc ::BayesTable::PasteHandler {table cell txt} {
#
# PURPOSE: This procedure handles how data is pasted into the table widget.
#          This handles data in the default table selection form.
#          Allows paste data in title cells and grows in rows or/and columns
#          if necesary
#
# PARAMETERS:
#   table -> Name of a table widget
#   cell  -> Cell to start pasting in
#   txt   -> Text to paste
#
#/////////////////////////////////////////////////////////////////////////////
  # Calculo las filas y columnas necesarias para pegar  
  set rowIni [lindex [split $cell ","] 0]
  set colIni [lindex [split $cell ","] 1]
  set rsep [$table cget -rowseparator]
  set csep [$table cget -colseparator]

  if {[string comp {} $rsep]} { set drows [split $txt $rsep] }
  set numRows [llength $drows]
  if [string equal [lindex $drows end] {}] {
    set numRows [incr numRows -1]
  }
  set numCols 0  
  foreach line $drows {
    #Tolcon_Trace "line=$line"
    if [expr [llength [split $line $csep]] > $numCols] {
      set numCols [llength [split $line $csep]]
    }
  }
  #Tolcon_Trace "rowIni=$rowIni, numRows=$numRows"
  #Tolcon_Trace "colIni=$colIni, numCols=$numCols"

  #Si necesito más filas las creo  
  if [expr $rowIni+$numRows > [$table cget -rows]] {
    $table configure -rows [expr $rowIni+$numRows]
  }
  #Si necesito más columnas 
  if [expr $colIni+$numCols > [$table cget -cols]] {
    $table configure -cols [expr $colIni+$numCols]
  }
  # En la versión 2.8 tktable no permite pegar en las celdas de título
  # así que le engañamos
  set titlerows [$table cget -titlerows]
  set titlecols [$table cget -titlecols]
  $table configure -titlerows 0 -titlecols 0
  #Tolcon_Trace "Llamada a tk_tablePasteHandler table=$table, p=$p, txt=$txt"
  tk_tablePasteHandler $table $cell $txt
  $table configure -titlerows $titlerows -titlecols $titlecols
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::BayesTable::CmdFind {this} {
#
# PURPOSE:
#
# PARAMETERS:
#   this -> Instance of Bayes Table
#
#/////////////////////////////////////////////////////////////////////////////

  ::BayesTable::FindShow $this
}

proc ::BayesTable::CmdFilter {this} {
}
proc ::BayesTable::CmdOrdAsc {this} {
}
proc ::BayesTable::CmdOrdDes {this} {
}
 
#/////////////////////////////////////////////////////////////////////////////
  proc ::BayesTable::FindText { this } {
#
# PURPOSE: Find the text in the table
#
# PARAMETERS:
#   this -> Instance of Bayes Table
#
# RETURN: A list with the coordinates of the cell in which is the text find. 
#         Empty does not find it.
#
#/////////////////////////////////////////////////////////////////////////////
    
  upvar \#0 ${this}::data data
  #inizializamos valores
  set ok 0
  set rc ""
  #fijamos numero de filas y columnas y origen de fila y columna
  set r   [$data(tabla) cget -rows]
  set c   [$data(tabla) cget -cols]
  set ro  [$data(tabla) cget -roworigin]
  set co [$data(tabla) cget -colorigin]
  #celda activa
  if { ![catch {set pos [$data(tabla) index active]} msgerr] } {
    if {$pos eq "" } { 
      set rp $ro
      set cp $co
    } else  {
      set pc [string first , $pos]
      set rp [string range $pos 0         [expr $pc - 1]]
      set cp [string range $pos [incr pc] end]
    } 
  } else  {
    set rp $ro
    set cp $co
  }
  #establecemos direccion de busqueda
  if [string equal $data(find,dir) "forward"] {
    set dir 1
  } else {
    set dir -1
  }  
  #comprobamos si muyusculas minusculas
  if $data(find,case) {
    set nc ""    
  } else  {
    set nc -nocase    
  }
  set sw $data(tabla,next)
  #comparamos valores
  if {![string equal $data(find,text) ""]} {
    while {[expr ($ok == 0) && ($cp >= $co) && ($cp < $c)]} {
      while {[expr ($ok == 0) && ($rp >= $ro) && ($rp < $r)]} {
        set txt [$data(tabla) get $rp,$cp]
        if {[eval string match $nc [list $data(find,text)] [list $txt]]} {
          if {($sw eq "0") || ($pos ne "$rp,$cp")} {
            set ok 1  
            set rc "$rp $cp"
          }
          set sw 0
        }
        incr rp $dir
      }
      if {[expr ($dir == 1)]} {
        set rp $ro
      } else  {
        set rp [expr $r - 1]
      }
      incr cp $dir
    }
  }
  return $rc
}
       
#/////////////////////////////////////////////////////////////////////////////
  proc ::BayesTable::FindCreate { this } {
#
# PURPOSE: Creates a Find Dialog given a Instance
#
# PARAMETERS:
#   this -> Instance of Bayes Table
#
#/////////////////////////////////////////////////////////////////////////////

  upvar \#0 ${this}::data data

  set data(find,dir)   forward
  set dialog [Dialog $data(path) -title [mc "Find"]\
          -default 0 -modal none -parent $data(tabla)]
  set w [$dialog getframe]
  wm resizable $data(path) 0 0
  
  set data(frm,cad) [LabelEntry  $w.eTxt -width 35 -label "[mc "Find"]: " \
          -textvariable ${this}::data(find,text)]
  checkbutton $w.full -text [mc "Palabra completa"] \
          -variable ${this}::data(find,full) -pady 0 -state disabled
  checkbutton $w.case  -text [mc "Case sensitive"] \
          -variable ${this}::data(find,case) -pady 0
  TitleFrame $w.dir -bd 2 -relief groove -text [mc "Direction"]
  set f [$w.dir getframe]
  radiobutton $f.forward -text [mc "Forward"] -value forward \
          -variable ${this}::data(find,dir)\
          -highlightthickness 0 -pady 0
  radiobutton $f.backward -text [mc "Backward"] -value backward \
          -variable ${this}::data(find,dir)\
          -highlightthickness 0 -pady 0
  Button $w.findNext -text [mc "Find next"] -width 16 \
          -command [list ::BayesTable::FindNext $this] \
          -helptext [mc "Find next"]
  Button $w.cancelar -text [mc "Cancel"] -width 16 \
          -helptext "" -command "destroy $dialog"
  
  bind $dialog <Return> [list ::BayesTable::FindNext $this]
  bind $dialog <F3>     [list ::BayesTable::FindNext $this]
  bind $dialog <Escape> [list destroy $dialog]

  # Hemos añadido un tag para la selección, hay que quitarle como el tag sel
  bind $data(tabla) <ButtonPress> +[list $data(tabla) tag delete found]
  grid $f.forward  $f.backward -sticky nw
  
  grid $w.eTxt -       $w.findNext -sticky nw -padx 5 -pady 2
  grid $w.full $w.dir  $w.cancelar -sticky ws -padx 5
  grid $w.case ^                   -sticky nw -padx 5
  wm geometry $dialog 360x90
  $dialog draw
  update
  focus $data(frm,cad)
  $data(frm,cad).e selection range 0 end
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::BayesTable::FindNext { this } {
#
# PURPOSE: Creates a Find Dialog given a Instance
#
# PARAMETERS:
#   this -> Instance of Bayes Table
#
#/////////////////////////////////////////////////////////////////////////////

  upvar \#0 ${this}::data data
    
  set pos     [FindText $this]
  # comprobamos si ha encontrado algo
  if { $pos != "" } {
    set data(tabla,next) 1
    set p "[lindex $pos 0],[lindex $pos 1]"
    $data(tabla) selection clear all
    $data(tabla) see $p    
    $data(tabla) activate $p
#    $data(tabla) selection set $p
  } else {
    set data(tabla,next) 0
    if [string equal $data(find,dir) "forward"] {
      set msg "[mc SearchUpEndDoc].\n[mc FindFromBegin]"
    } else {
      set msg "[mc SearchUpBeginDoc].\n[mc FindFromEnd]"
    }
    if [winfo exists $data(path)] {
      set parent $data(path)
    } else  {
      set parent $data(tabla)
    }
    set rq [tk_messageBox -type okcancel -icon info -parent $parent\
            -title [mc "Table"] -message $msg]
    if {$rq == "ok"} {
      if [string equal $data(find,dir) "forward"] {
        set ro [$data(tabla) cget -roworigin]
        set co [$data(tabla) cget -colorigin]
        $data(tabla) activate "$ro,$co"
      } else {
        $data(tabla) activate end
      }
      FindNext $this
    }
  }
  # Debo quedarme yo con el foco, no la tabla

  #focus $data(tabla)
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::BayesTable::FindShow {this {next 0} {txt ""}} {
#
# PURPOSE: Creates a Find Dialog given a Instance
#
# PARAMETERS:
#   this -> Instance of Bayes Table
#   next
#   txt 
#
#/////////////////////////////////////////////////////////////////////////////

  set tabla [GetTkTable $this]
  set Instance ::BayesTable::Find$tabla
  if { [lsearch [namespace children ::BayesTable] $Instance] != -1 } {
      
  } else  {
    namespace eval $Instance {
      variable data
      #data(find,text) --> texto a buscar
      #data(find,dir)  --> dirección de búsqueda
      #data(start)     --> indica desde donde se empieza a buscar
      #data(find,full) -->
      #data(find,case) -->
      #data(tabla)    --> el path del widget tabla en el q buscar
      #data(path)      --> el path del toplevel que muestra el cuado de búsqueda
      #data(frm,cad)   --> entry donde se inserta el texto a buscar
      #data(tabla,next) -> 0 primera busqueda, 1 sigiente busqueda
    }
  }

  upvar \#0 ${Instance}::data data

  set data(tabla) $tabla
  set data(namespace) "FindDialog"
  set data(path) ${tabla}.tFind
  if [winfo exist $data(path)] {
    raise $data(path)
    focus $data(frm,cad)
    $data(frm,cad).e selection range 0 end
    return
  }

  set data(tabla,next) 0
  if {$txt != ""} {
    set data(find,dir)  forward
    set data(find,case) 1
    set data(find,text) $txt
  }

  if {$next && [info exists ${Instance}::data(find,text)] && ($data(find,text) !="") } {
      FindNext $Instance
  } else {
      FindCreate $Instance
  }
}