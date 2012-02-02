#/////////////////////////////////////////////////////////////////////////////
# FILE    : project.tcl
# PURPOSE : This file implement the comunication between the project and 
#           tolcon. Menu description is taken from 
#           http://mini.net/tcl/827.html, the implementation is in file
#           prjmenu.tcl.
#/////////////////////////////////////////////////////////////////////////////

package require bfunctions
package require btoplevel
package require fmenu
package require mdi
package require bodbc

#/////////////////////////////////////////////////////////////////////////////      
namespace eval ::project {
#  
#/////////////////////////////////////////////////////////////////////////////        
  variable data
  variable events
  variable widgets

  array set projects {}

  array set data {
    name           ""
    path           ""
    killing        0
    init           0
    tolconreq      0
    minmode        withdraw
    minforzed      0
    splashed       0
    oculto         0
    defaulthelpkey ""}
  array set widgets {
    children ""
    children,types ""
    children,type,default ""
    children,saved ""
    mainframe .
  }
  array set events {OnInitTcl "" OnInitTol "" OnExit ""}
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::project::project { inf } {
#
# PURPOSE: Defines the projects & initializes the namespace variables.
#
# PARAMETERS:
#  name : 
#  path :
#
#/////////////////////////////////////////////////////////////////////////////
  variable data
  variable widgets
  variable projects

  # definition connection BODBC
  set data(bodbc) [bodbc bodbcgestor]
  #Tolcon_Trace "project.project : data(bodbc)=$data(bodbc)"

  set projects(Project,splash) ""
  set projects(Project,names) ""
  array set projects $inf

  array set events {OnInitTcl "" OnInitTol "" OnExit ""}
  array unset widgets
  array set data {
    killing 0
    init 0
    tolconreq 0
    minmode withdraw
    minforzed 0 
    splashed 0
    defaulthelpkey "Tb4Mai"}
  array set widgets {
    children ""
    children,types ""
    children,type,default ""
    children,saved ""
    mainframe .
  }
  # hide the main window, the one containing the menu bar
  wm withdraw .
  if { [expr ! $data(oculto)] } { 
    splashWindow
  }
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::project::path { name } {
#
# PURPOSE: Returns the project path.
#
#/////////////////////////////////////////////////////////////////////////////
  variable projects
  return $projects($name,path)
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::project::init { } {
#
# PURPOSE:  Starts the project. If there is no project, it starts tolcon.
#           Could be started only once.
#
#/////////////////////////////////////////////////////////////////////////////
  global tcl_platform
  global toltk_images_path
  variable data
  variable widgets
  variable projects

  if { $data(init) } return
  startupResources
  initWM
  if { [info exists projects(Project,names)] && \
       [llength $projects(Project,names)] > 0 } {
    load
  } else {
    UseTolCon
  }
  # Read .ini (paramters TolBase)
  ::TolConsole::ReadIni
  
  # Gives a name for the application
  set prjtitle "TOLBase"
  if { [info exists projects(Project,title)] && $projects(Project,title) ne "" } {
    set prjtitle $projects(Project,title)
  }
  wm title . $prjtitle

  #if { [TolConReq] }
    #Creamos frame de trabajo
    set widgets(mainmenu) [::project::CreateMdiMenu .main.mdi]
	set widgets(mainframe) [frame .main]
	set widgets(mainmdi) [mdi .main.mdi -mdimenu $widgets(mainmenu) \
	                       -relief sunken -bd 1]
    pack $widgets(mainmenu) -fill x -side top
	pack $widgets(mainframe) -fill both -expand yes -side bottom
    pack $widgets(mainmdi) -fill both -expand yes -side bottom
	bind . <Control-Tab> "[list $widgets(mainmdi) cicle]; break"
	::TolConsole::MainFrame $widgets(mainmdi) [TolConReq]
  #
  
  if { [TolConReq] } {
    ::TolConsole::MenuMDI
  }
  foreach prj $projects(Project,names) {
    set prjname $projects(${prj},name)
    if { [string length $prjname] } { eval ${prjname}::InitTcl }
  }
  $widgets(mainmenu) draw

  if { [string equal $tcl_platform(platform) "unix"] } {
    wm iconbitmap . "@[file join $toltk_images_path bayes.xbm]"
  } else {
    wm iconbitmap . -default [file join $toltk_images_path bayes.ico]
  }
  if { [TolConReq] } {
    ::TolInspector::Busy
  }
#   update
  if { $projects(Project,path,root) ne "" } {
    set initol 0
    set iniproject [ file join $projects(Project,path,root) "_iniproject.tol" ]
  } else {
    set initol $data(iniproject)
    set iniproject ""
  }
  tol::initlibrary $initol
  if { $iniproject ne "" } {
    # incluyo _iniproject.tol relativo a -project
    tol::include $iniproject
  }
  foreach prj $projects(Project,names) {
    set prjname $projects(${prj},name)
    if { [string length $prjname] } { eval ${prjname}::InitTol }
  }
  #project::event InitTol
  if { [TolConReq] } {
    ::TolInspector::NotBusy
  }
  if { [expr ! $data(oculto)] } {
    splashWindow
  }

  set data(init) 1
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::project::initWM { } {
#
# PURPOSE: Inits the events & protocols associated to toplevel.
#
#/////////////////////////////////////////////////////////////////////////////
  variable data

  bind . <Map> "::project::mapMain %W"
  bind . <Unmap> "::project::unmapMain %W"
  # establezco los protocolos
  wm protocol . WM_DELETE_WINDOW [list ::project::OnDelete]
  bind . <Destroy> "::project::OnDestroy %W"
  set defgeom "[winfo vrootw .]x[winfo vrooth .]+0+0"
  wm state . normal
  wm geometry . [set geom [::iniFile::Read Project topgeom $defgeom]]
puts "initWM: geometria: $geom"
  if { [expr $project::data(oculto)] } {
    wm iconify .
  } else {
    wm state . [::iniFile::ReadWMState ::iniFile::Read Project]
  }
  # update in order to . gets the geometry 
  update
  showMain
}

#/////////////////////////////////////////////////////////////////////////////      
proc ::project::OnDelete {} {
#  
#/////////////////////////////////////////////////////////////////////////////        
  variable data

  project::SetKilling
  set answer [tk_messageBox -title [mc "Question"]\
         -message [mc "Are you sure you want to quit?"] \
         -type yesno -icon question]
  if { [string compare $answer yes] == 0 } { 
    #::TolConsole::Exit
    Exit
  }
  project::SetKilling 0
}

#/////////////////////////////////////////////////////////////////////////////      
proc ::project::OnDestroy { w } {
#  
#/////////////////////////////////////////////////////////////////////////////        
  variable data

  if {$w eq "."} {
    ::iniFile::Write Project topgeom  $data(geometry,main)
    ::iniFile::Write Project wm,state $data(wm,state)
    ::iniFile::CloseAll
  }
}

#/////////////////////////////////////////////////////////////////////////////      
proc project::Exit { } {
#  
#/////////////////////////////////////////////////////////////////////////////        
  variable data
  if {[event Exit] ne "cancel"} {
    update idletasks
    set data(geometry,main) [wm geometry .]
    set data(wm,state)      [wm state    .]
    destroy .
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::project::mapMain { w } {
#
# PURPOSE: proc to handle <Map> event, normally it is triggered when the "." 
#          window is restored after having being minimized.
#
# PARAMETERS:
#  w :
#
#/////////////////////////////////////////////////////////////////////////////
  variable widgets

  if { [string compare $w .] } return
  # restaurar descendientes
  foreach c $widgets(children,saved) {
    mapChild $c
  }
  set widgets(children,saved) ""
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::project::unmapMain { w } {
#
# PURPOSE: proc to handle <Unmap> event, normally it is triggered when the
#          "." window is minimized.
#
# PARAMETERS:
#  w :
#
#/////////////////////////////////////////////////////////////////////////////
  variable widgets

  if { [string compare $w .] } return
  # guardar informacion de descendientes mapeados
  foreach c $widgets(children) {
    if [winfo ismapped $c] {
      lappend widgets(children,saved) $c
      unmapChild $c
    }
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::project::UseTolCon { } {
#
# PURPOSE: Loads the code for using tolcon.
#
#/////////////////////////////////////////////////////////////////////////////
  global toltk_script_path
  variable data
  
  set data(tolconreq) 1
  source [file join $toltk_script_path "tolbase.tcl"]
}


#/////////////////////////////////////////////////////////////////////////////
   proc ::project::TolConReq { } {
#
# PURPOSE: Tells us if tolcon has being requested (project::UseTolCon).
#
#/////////////////////////////////////////////////////////////////////////////
  variable data
  
  set data(tolconreq)
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::project::load {} {
#
# PURPOSE: Loads the given project. Looks for it in $toltk_library/$prj, if
#          it is not found look as in development environment given by the
#          alternative path $path (normally it will be
#          $toltk_libray/../../../../entity)
#
#/////////////////////////////////////////////////////////////////////////////
  variable data
  variable projects
  foreach prj $projects(Project,names) {
    set prjname $projects(${prj},name)
    if { [string length $prjname] } {
      set init_file \
          [file join $projects(${prj},path) tcl $projects(${prj},name).tcl]
      if { [file exists $init_file] } {
        source $init_file
        eval $projects(${prj},name)::$projects(${prj},name)
      } else {
        error "could not load project '$projects(${prj},name)': \
$projects(${prj},name).tcl not found. $init_file"
      }
    }
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::project::OnInitTcl { ev } {
#
# PURPOSE: Sets the proc to be invoked after toltk had loaded all its tcl/tk
#          components.
#
# PARAMETERS:
#  ev :
#
#/////////////////////////////////////////////////////////////////////////////
  variable events

  set events(OnInitTcl) $ev
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::project::OnInitTol { ev } {
#
# PURPOSE: Sets the proc to be invoked after toltk had loaded TOL.
#
# PARAMETERS:
#  ev :
#
#/////////////////////////////////////////////////////////////////////////////
  variable events

  set events(OnInitTol) $ev
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::project::OnExit { ev } {
#
# PURPOSE: Sets the proc to be invoked before exiting.
#
# PARAMETERS:
#  ev :
#
#/////////////////////////////////////////////////////////////////////////////
  variable events

  set events(OnExit) $ev
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::project::menu { desc } {
#
# PURPOSE: Defines the menu of the toplevel window. For now, the project menu
#          excludes toltk menu.
#
# PARAMETERS:
#  desc :
#
#/////////////////////////////////////////////////////////////////////////////
  variable widgets

  prjmenu::create . $desc
#   update
#   set vrootw [winfo vrootw .]
  #wm maxsize .  $vrootw 1
#   wm geometry . [expr ${vrootw}-6]x1+0+0
  # poner un frame en . de dimension 0x0 y pedir [winfo rootx $f]
  # y asi tener un aproximado de la altura de (main), sumarle 5 o 6.
#   set widgets(dummyframe) [frame .dummy]
#   pack $widgets(dummyframe)
#   showMain
#   wm resizable . 0 0
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::project::showMain { } {
#
# PURPOSE: Shows the main window, the one having the menu bar.
#
#/////////////////////////////////////////////////////////////////////////////
  if { [string equal [wm state .] "withdrawn"] } {
    wm withdraw .
    wm deiconify .
    update
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::project::splashWindow { } {
#
# PURPOSE: Shows a splash window for the project if it is defined or the one
#          found in images in tolcon. The image to show is splash.gif
#
#/////////////////////////////////////////////////////////////////////////////
  variable data
  variable projects
  global toltk_library

  if { $data(splashed) } return
  set w .__splashw
  if { [winfo exists $w] } {
    image delete [$w.l cget -image]
    destroy $w
    set data(splashed) 1
  } else {
    set imgfile $projects(Project,splash)
    if { $projects(Project,splash) eq "" } {
      set imgfile [file join $toltk_library toltk images splash.gif]
    }
    if { [file exists $imgfile] } {
      set img [image create photo -file $imgfile]
      toplevel $w
      wm withdraw $w
      wm overrideredirect $w 1
      label $w.l -image $img -relief ridge -bd 2
      pack $w.l
      update
      wm geometry $w +[expr ([winfo vrootw .]-[winfo reqwidth $w])/2]+[expr ([winfo vrooth .]-[winfo reqheight $w])/2]
      update
      wm withdraw $w
      wm deiconify $w
      update
    } else {
      set data(splashed) 1
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////      
proc project::raiseSplash { } {
#  
#/////////////////////////////////////////////////////////////////////////////        
  set w .__splashw
  if { [winfo exists $w] } {
    raise $w
  } 
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::project::DisableMenu { m e } {
#
# PURPOSE: Disables an entry or menu inside a menu.
#
# PARAMETERS:
#   m : menu
#   e : entry or menu to disable
#
#/////////////////////////////////////////////////////////////////////////////
  ::prjmenu::disable . $m $e
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::project::EnableMenu { m e } {
#
# PURPOSE: Enables an entry or menu inside a menu.
#
# PARAMETERS:
#   m : menu
#   e : entry or menu to enable
#
#/////////////////////////////////////////////////////////////////////////////
  ::prjmenu::enable . $m $e
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::project::hasmenu { } {
#
# PURPOSE: Tells us if the toplevel has a menu defined.  
#
#/////////////////////////////////////////////////////////////////////////////
  string length [. cget -menu]
}

#/////////////////////////////////////////////////////////////////////////////      
proc ::project::raiseChild { w } {
#  
#/////////////////////////////////////////////////////////////////////////////        
  if {[winfo ismapped $w]} {
    raise $w
  } else {
    mapChild $w
  }
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::project::toggleChild { w } {
#
# PURPOSE: Changes the state of a registered child.
#
# PARAMETERS:
#   w :
#
#/////////////////////////////////////////////////////////////////////////////
  variable widgets

  puts toggleChild

  if {[winfo ismapped $w]} {
    #unmapChild $w
    #El usuario suele desmapear minimizando
    #raise $w
    puts "ismapped"
    mapChild $w
  } else {
    mapChild $w
  }
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::project::mapChild { w } {
#
# PURPOSE: Map an unmaped & registered child window. The way of unmapping
#          depend on minimizeMode defined.
#
# PARAMETERS:
#   w :
#
#/////////////////////////////////////////////////////////////////////////////
  variable widgets
  if { [string equal [minimizeMode] "withdraw"] } {
    wm withdraw $w
    if ![info exists widgets(child,geometry,$w)] {
      set widgets(child,geometry,$w) 800x600
    }
    wm geometry $w $widgets(child,geometry,$w)
  }
  wm deiconify $w
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::project::unmapChild { w } {
#
# PURPOSE: Unmap a maped & registered child window. The way of unmapping
#          depend on minimizeMode defined.
#
# PARAMETERS:
#   w :
#
#/////////////////////////////////////////////////////////////////////////////
  variable data
  variable widgets
  
  set data(minforzed) 1
  if { [string equal [minimizeMode] "withdraw"] } {
    set widgets(child,geometry,$w) [wm geometry $w]
    wm withdraw $w
  } else {
    wm iconify $w
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::project::ExternalBottom { } {
#
# PURPOSE: Returns the external bottom y-coordinate of the toplevel window.
#          This is only an approximate value.
#
#/////////////////////////////////////////////////////////////////////////////
  global tcl_platform
  variable widgets

  return [winfo rooty $widgets(mainframe)]

  if { [string equal $tcl_platform(platform) "unix"] } {
    set extra 12
  } else {
    set extra 5
  }
  expr [winfo rooty $widgets(dummyframe)] + $extra
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::project::event { ev } {
#
# PURPOSE: Call an event. Valids events are: InitTcl, InitTol, Exit.
#
# PARAMETERS:
#   ev : Event to call
#
#/////////////////////////////////////////////////////////////////////////////
  variable events

  if {$events(On$ev) ne ""} {
    eval $events(On$ev)
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::project::SetKilling {{what 1}} {
#
# PURPOSE: Signal the killing of the whole aplication. That way when the rest
#          of its children know they should destroy in case the have provide 
#          WM_DELETE_WINDOW protocol.
#
#/////////////////////////////////////////////////////////////////////////////
  variable data
  set data(killing) $what
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::project::IsKilling { } {
#
# PURPOSE: Returns if Is the application destroying?
#
#/////////////////////////////////////////////////////////////////////////////
  variable data

  set data(killing)
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::project::minimizeMode { args } {
#
# PURPOSE: sets/returns the minimize mode, could be:
#    - withdraw
#    - minimize
#
# PARAMETERS:
#   args :
#
#/////////////////////////////////////////////////////////////////////////////
  variable data

  switch [llength $args] {
    0       { set data(minmode) }
    1       { set data(minmode) [lindex $args 0] }
    default { error "wrong # args: '$args'"  }
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::project::postWindows { m } {
#
# PURPOSE: Proc to call when the menu Window is about to post.
#
# PARAMETERS:
#   m : is the menu where to insert the entries. At least there will be one:
#       "Tol Inspector".
#
#/////////////////////////////////////////////////////////////////////////////
  variable widgets

  $m delete 0 end
  foreach subm [winfo children $m] {
    $subm delete 0 end
  }
  $m add command -label [mc "Minimize all"]    -command "::project::minimizeAll"
  $m add command -label [mc "Cascade"]         -command "::project::cascade"
  $m add command -label [mc "Tile Horizontal"] -command "::project::tileHorizontal"
  $m add command -label [mc "Tile Vertical"]   -command "::project::tileVertical"
  $m add separator
  foreach w $widgets(children,type,default) {
    $m add command \
        -label $widgets(child,label,$w) -command "::project::raiseChild $w"
  }
  foreach t $widgets(children,types) {
    if { [string compare $t "default"] } {
      set subm $m.m[join $t _]
      if { ![winfo exists $subm] } {
        ::menu $subm -tearoff 0
      }
      $m add cascade -label [mc $t] -menu $subm
      foreach w $widgets(children,type,$t) {
        $subm add command \
          -label $widgets(child,label,$w) -command "::project::raiseChild $w"
      }
      $subm add separator
      $subm add command -label [mc "Close all"]\
             -command [list ::project::closeType $t]
    }
  } 
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::project::closeType {type} {
#
# PURPOSE: Closes all windows of a specified type
#
# PARAMETERS:
#   type: Type of windows we want to close
#
#/////////////////////////////////////////////////////////////////////////////
  variable widgets

  foreach d [lindex [$widgets(mainmdi) get_documents $type] 1] {
    $d kill
  }
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::project::registerChild { w args } {
#
# PURPOSE: registers a toplevel to be handled by window menu
#
# PARAMETERS:
#   w:
#   args:
#
#/////////////////////////////////////////////////////////////////////////////
  variable widgets

  if { [string compare [winfo toplevel $w] $w] } {
    error "$w must be a toplevel"
  }
  array set options {
    -type     default
    -killmode kill
  }
  set options(-label) [wm title $w]
  # override defaults options
  array set options $args
  if { [lsearch $widgets(children) $w] != -1} {
    error "window $w is already registered"
  }
  if { [info exists options(-varlabel)] } {
    set varlabel $options(-varlabel)
    upvar \#0 $varlabel label
    trace variable $options(-varlabel) w \
	"::project::updateWindowDesc $w $varlabel; #"
    set options(-label) $label 
  }
  lappend widgets(children) $w
  if { [lsearch $widgets(children,types) $options(-type)] == -1 } {
    lappend widgets(children,types) $options(-type)
  }
  lappend widgets(children,type,$options(-type)) $w
  set widgets(child,type,$w)     $options(-type) 
  set widgets(child,killmode,$w) $options(-killmode)
  set widgets(child,label,$w)    $options(-label)
  if { [string equal $options(-killmode) "hide"] } {
    wm protocol $w WM_DELETE_WINDOW [list ::project::minimizeChild $w $w]
  }
  bind $w <Destroy> +[list ::project::deleteChildren %W $w]
  bind $w <Unmap>   "::project::minimizeChild %W $w"  
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::project::updateWindowDesc { w varlabel } {
#
# PURPOSE:
#
# PARAMETERS:
#   w :
#   varlabel :
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 $varlabel label
  variable widgets

  set widgets(child,label,$w) $label
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::project::minimizeAll {} {
#
# PURPOSE: Minimizes all windows
#
#/////////////////////////////////////////////////////////////////////////////
  variable widgets
  foreach c $widgets(children) {
    unmapChild $c
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::project::cascade {} {
#
# PURPOSE:
#
#/////////////////////////////////////////////////////////////////////////////
  variable widgets
  set x 0
  set y [ExternalBottom]
  foreach c $widgets(children) {
    wm geometry $c +$x+$y
    mapChild $c
    incr x 20
    incr y 20
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::project::tileHorizontal {} {
#
# PURPOSE:
#
#/////////////////////////////////////////////////////////////////////////////
  variable widgets
  set widgets(children,tileVer) {}
  foreach c $widgets(children) {
    if [winfo ismapped $c] {
      lappend widgets(children,tileVer) $c
    }
  }
  set num    [llength $widgets(children,tileVer)]
  set y [ExternalBottom]
  set tWidth  [winfo vrootwidth  .]
  set tHeight [expr [winfo vrootheight .] - $y - 30]
  
  if $num {
    set height [expr ($tHeight/$num)-6]
  }

  foreach c $widgets(children,tileVer) {
    wm geometry $c ${tWidth}x${height}+0+$y
    incr y [expr $height + 6]
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::project::tileVertical {} {
#
# PURPOSE:
#
#/////////////////////////////////////////////////////////////////////////////
  variable widgets
  set widgets(children,tileHor) {}
  foreach c $widgets(children) {
    if [winfo ismapped $c] {
      lappend widgets(children,tileHor) $c
    }
  }
  set num    [llength $widgets(children,tileHor)]
  set x 0
  set y [ExternalBottom]
  set tWidth  [winfo vrootwidth  .]
  set tHeight [expr [winfo vrootheight .] - $y - 30]
    
  if $num  {
    set width [expr ($tWidth/$num)-6]
  }
  foreach c $widgets(children,tileHor) {
    wm geometry $c ${width}x${tHeight}+$x+$y
    incr x [expr $width + 6]
  }
  #Tolcon_Trace "tWidth: $tWidth, tHeight: $tHeight, mapped: $num, width: $width, x: $x, y: $y"
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::project::minimizeChild { w _w } {
#
# PURPOSE: This is invoked when w == _w is going to be minimized. Actually it
#          is invoked for all of it's descendents.
#
# PARAMETERS:
#   w  :
#   _w :
#
#/////////////////////////////////////////////////////////////////////////////
  variable data
  variable widgets

  if { [string compare $w $_w] } return
  if { $data(minforzed) } {
    set data(minforzed) 0
    return
  }
  if { [string equal [minimizeMode] "withdraw"] } {
    set widgets(child,geometry,$w) [wm geometry $w]
    wm withdraw $w
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::project::deleteChildren { w _w } {
#
# PURPOSE: handler to delete a child window registred by registerChild
#
# PARAMETERS:
#   w  :
#   _w :
#
#/////////////////////////////////////////////////////////////////////////////
  variable widgets
  if { [string compare $w $_w] } return
  if { [string equal $widgets(child,killmode,$w) kill] } {
    # unregister $w
    blremove widgets(children) $w
    set type $widgets(child,type,$w)
    blremove widgets(children,type,$type) $w
    unset widgets(child,type,$w)
    set type_found 0
    foreach i [array names widgets child,type,*] {
      if {[string equal $type $widgets($i)]} {
        set type_found 1
        break
      }
    }
    if { !$type_found } {
      blremove widgets(children,types) $type
    }
    unset widgets(child,killmode,$w)
    unset widgets(child,label,$w)
    # kill $w as requested in registerChild
    #destroy $w
  } else {
    if { [IsKilling] } {
      # . is going down
      # destroy $w
    } else {
      if { [string equal [minimizeMode] "withdraw"] } {
        set widgets(child,geometry,$w) [wm geometry $w]
        wm withdraw $w
      } else {
        wm iconify $w
      }
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::project::CreateForm {args} {
#
# PURPOSE: Create form
#
# PARAMETERS:
#   args : option-value pairs
#
#/////////////////////////////////////////////////////////////////////////////
  variable widgets
  # Default

  #set path {}
  array set opts [list \
    -height   300 \
    -width    300 \
    -title    {} \
    -closecmd {} \
    -iniconfig {} \
    -type     {} \
    -unique 0 \
    -modal 0 \
    -x 0 \
    -y 0 \
    -state normal \
    -resizable 1 \
    -helpkey "Tb4Mai"
  ]

  array set opts $args

  if {([info exist opts(-geometry)]) && ([string length $opts(-geometry)])} {
    set opts(-state) normal
    set opts(-width)  [lindex [split $opts(-geometry) x] 0]
    set restgeom      [lindex [split $opts(-geometry) x] 1]
    set opts(-height) [lindex [split $restgeom +] 0]
    set opts(-x)      [lindex [split $restgeom +] 1]
    set opts(-y)      [lindex [split $restgeom +] 2]
  } 
  #puts "Nombre: $w Lista: \n\ [array get opts]"
  set path [.main.mdi slave -title $opts(-title)   -type  $opts(-type) \
      -width $opts(-width) -height $opts(-height) -modal $opts(-modal) \
      -closecmd $opts(-closecmd) -iniconfig $opts(-iniconfig) \
      -resizable $opts(-resizable) -x $opts(-x) -y $opts(-y) \
      -state $opts(-state) -helpkey $opts(-helpkey) ]
  update
  return $path
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::project::SelectDoc { dw } {
#
# PURPOSE: selects a mdi document
#
# PARAMTERS:
#  dw: the document window to select
#
#/////////////////////////////////////////////////////////////////////////////
  if { ([$dw cget -state] eq "minimized" ) ||
       ([$dw cget -state] eq "withdrawn") } {
    $dw restore
  } else  {
    $dw raise
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::project::FillWindowMenu {mdi m} {
#
# PURPOSE: Rellena el menu window
#
#/////////////////////////////////////////////////////////////////////////////

  $m delete 0 end
  foreach cw [winfo children $m] {
    destroy $cw
  }
  $m add command -label [mc "Tile across"] -underline 0 -command "$mdi arrange tile" 
  $m add command -label [mc "Tile horizontally"] -underline 5 \
                 -command "$mdi arrange hor" 
  $m add command -label [mc "Tile vertically"] -underline 5 -command "$mdi arrange ver"
  $m add command -label [mc "Cascade"] -underline 0 -command "$mdi arrange cascade"
  $m add separator
  $m add command -label [mc "Arrange icons"] -underline 0 -command "$mdi arrange icons"
  $m add command -label [mc "Minimize all"] -underline 0 -command "$mdi arrange min"
  foreach t [$mdi get_types] {
    $m add cascade -label $t -menu $m.sub$t
    ::menu $m.sub$t -tearoff 0
    foreach d [lindex [$mdi get_documents $t] 1] {
      if {[$d cget -state] != "withdrawn"} {
        $m.sub$t add command -label [$d cget -title] -command "::project::SelectDoc $d "
      }
    }
    $m.sub$t add separator
    $m.sub$t add command -label [mc "Close all"] \
             -command [list ::project::closeType $t]
  }
  foreach d [lindex [$mdi get_documents {}] 1] {
    if {[$d cget -state] != "withdrawn"} {
      $m add command -label [$d cget -title] -command "::project::SelectDoc $d"
    }
  }
  if {[$m index end] > 6} {
    $m insert 7 separator
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::project::CreateMdiMenu { mdi } {
#
# PURPOSE: Crea un menu para mdi con las entradas basicas ayuda y ventanas
#
#/////////////////////////////////////////////////////////////////////////////
  set md [menudef create %AUTO%]

  set id [$md add root cascade -label [mc &Window] \
              -postcommand [list ::project::FillWindowMenu $mdi %W]]
  set ::project::data(menuhook) $id
  $md add root separator
  
  set id [$md add root cascade -label [mc &Help]]
  set ::project::data(menuhookhelp) $id
  
  $md add $id command -label [mc "&About"]... \
      -command "::TolConsole::AboutShow"
  $md add $id command -label [mc "&TolBase"] \
      -command "::TolConsole::TolBaseHelp"
  mdimenu .mainmenu -definition $md
  return .mainmenu
}