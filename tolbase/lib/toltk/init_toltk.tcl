#!/bin/sh
#-*-tcl-*-
# the next line restarts using wish \
exec wish "$0" -- ${1+"$@"}

#/////////////////////////////////////////////////////////////////////////////
# FILE    : init_toltk.tcl
# PURPOSE : This file loads packages and libraries and initializes a project.
#/////////////////////////////////////////////////////////////////////////////

package require Tk

# show_filters: muestra (o no) los filtros en el inspector de TOL
set show_filters 0
# trace: muestra (o no) la ventana de trazas de TOL
set trace 0

#/////////////////////////////////////////////////////////////////////////////
proc Tolcon_Trace { msg {tags ""} {args ""} } {
#
# PURPOSE: Writes a trace message in a toplevel window. It's used for
#          debugging. If release versions uncomment the initial return.
#
# PARAMETERS:
#   msg -> Message to trace
#
#/////////////////////////////////////////////////////////////////////////////
  global trace 
  if {$trace eq 0} {
    return
  }
  
  set w .__debugwin
  if { ![winfo exists $w] } {
    toplevel $w
    ScrolledWindow $w.sw
    text $w.sw.t
    $w.sw setwidget $w.sw.t
    # tags of text widget
    $w.sw.t tag configure bold -font {Tahoma 8 bold}
    $w.sw.t tag configure big -font {Tahoma 10 bold}
    $w.sw.t tag configure verybig -font {Tahoma 12 bold}
    $w.sw.t tag configure normal -font {Tahoma 8}
    $w.sw.t tag configure small -font {Tahoma 7} 
    $w.sw.t tag configure centered -justify center
    $w.sw.t tag configure underline -underline on    
    $w.sw.t tag configure red -foreground red
    $w.sw.t tag configure green -foreground darkgreen
    $w.sw.t tag configure blue -foreground darkblue
    $w.sw.t tag configure grey -foreground darkgrey
    
    #::BayesText::MenuCreate $w.sw.t
    pack $w.sw -fill both -expand yes
  }
  $w.sw.t insert end "$msg\n" $tags
  $w.sw.t see end
  update
}

#/////////////////////////////////////////////////////////////////////////////
proc TraceMainGeometry { msg } {
#/////////////////////////////////////////////////////////////////////////////
  puts "== $msg == wm geometry . == [wm geometry .]"
  puts "== $msg == winfo geometry . == [winfo geometry .]"
}

#/////////////////////////////////////////////////////////////////////////////
proc LoadPackages {} {
#
# PURPOSE: Load packages
#
#/////////////////////////////////////////////////////////////////////////////
  package require snit
  package require BWidget
  package require msgcat
  namespace import msgcat::*
  package require Tktable
  package require rbc
  package require autoscroll 1.1
  package require tclodbc
  package require Toltcl
  package require rmtps_client
  package require byswidget
  package require mkWidgets 1.3
  #NOTE. Por precaución cargamos el paquete aqui
  #En un futuro cambiar
  package require bfunctions
}

#/////////////////////////////////////////////////////////////////////////////
proc OpenAssocFile {file {run 0} {inc 0}} {
#/////////////////////////////////////////////////////////////////////////////
  switch [string toupper [file extension $file]] {
    ".SQL" {
      #::Editor::Open $file
    }
    ".PRJ" {
      if { $inc } {
        ::tol::console eval "Set IncludePRJ(Text \"$file\")"
      } else {
        ::TolProject::Open $file $run
      }
    }
    ".BDT" {
      if { $inc } {
        ::tol::console eval "Set IncludeBDT(Text \"$file\")"
      } else {
        ::EditTable::Open $file
      }
    }
    ".BST" {
      if { $inc } {
        ::tol::console eval "Set IncludeBST(Text \"$file\")"
      } else {
        ::EditTable::Open $file
      }
    }
    ".BMT" {
      if { $inc } {
        ::tol::console eval "Set IncludeBMT(Text \"$file\")"
      } else {
        ::EditTable::Open $file
      }
    }
    ".TOL" {
      if { $inc } {
        ::tol::console eval "Set IncludeTOL(Text \"$file\")"
      } else {
        ::Editor::Open $file $run
      }
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc loadInfoProjects { prjini } {
#
# PURPOSE: Loads in a list the name and paths of each project to load
#
# RETURN: the list with the data
#
#/////////////////////////////////////////////////////////////////////////////
  global iniparse
  global toltk_library
  global toltk_script_path
  
  set values {}
  # File exists ?
  set ok 0
  if { [ file isfile $prjini ] } {
    if { [ file readable $prjini ] } {
      set ok 1
      set fileProject [ file normalize $prjini ]
      # se intentara leer el iniproject.tol desde este sitio
      lappend values Project,path,root [ file dir $fileProject ]
    } else {
      tk_messageBox -default ok -icon error \
          -message [ mc "The project does not exist, file not found or not readable: %s" $prj ] \
          -parent . -title "Error" -type ok
      return ""
    }
  } else {
    # comportamiento anterior, pudiera limpiarse
    lappend values Project,path,root ""
    set pathInitTol ""
    set dir $prjini
    set dirPrjIni "[file normalize $dir]/../../../"
    set fileProject [file join $dirPrjIni "project.ini"]
    if {[file exists $fileProject]} {
      set ok [ file writable $fileProject ] 
    } else {
      set fileProject [file join $dir "project.ini"]
      if {[file exists $fileProject]} {
        set ok [ file writable $fileProject ]  
      }
    }
  }
  # exists project.ini ?
  if {$ok} {
    set id [iniparse:openfile $fileProject]
    catch { ::iniFile::Create $fileProject }
    foreach prj $iniparse($id,sections) {
      if { $prj eq "Project" } {
        lappend values Project,title [::iniFile::Read Project title {}]
        lappend values Project,splash [::iniFile::Read Project splash {}]
        lappend values Project,names [lrange $iniparse($id,sections) 1 end]
      } else {
        set name [::iniFile::Read $prj name ""]
        set path [::iniFile::Read $prj path ""]
        if { [string length $path] } {
          set _path [file join $toltk_library $path]
          if { [file exists $_path] } {
            set $path $_path
          } else {
            #si la ruta se da relativa y tolbase se ejecuta desde archivo asociado
            set __path [file join $toltk_script_path $path]
            if { [file exists $__path] } {
              set $path $__path
            }
          }
          set pathtcl [file join $path tcl]
        } else {
          set path [file join $toltk_library toltk]
          set pathtcl $path
        }
        # check exists file ?
        if {[string length $name] && [string length $path] &&
            [file exists [file join $pathtcl ${name}.tcl]]} {
          lappend values ${prj},name $name
          lappend values ${prj},path $path
          lappend values ${prj},path,tcl $pathtcl
        } else {
          # erase prj in list of project
          array set arrtemp $values
          set lstAux $arrtemp(Project,names)
          set arrtemp(Project,names) [lreplace $arrtemp(Project,names) \
            [lsearch -exac $arrtemp(Project,names) $prj] \
            [lsearch -exac $arrtemp(Project,names) $prj]]
          set values [array get arrtemp]
          unset arrtemp
          # show mwssage
          if {[string length $name] && [string length $path]} {
            tk_messageBox -default ok -icon error \
              -message "The project does not exist: $prj ([file join $pathtcl ${name}.tcl])" \
              -parent . -title "Error" -type ok
          }
        }
      }
    }
    iniparse:closefile $id
  }
  return $values
}

#/////////////////////////////////////////////////////////////////////////////
# Main
# Secuencia de instrucciones para la carga de tolbase
# 

if {[info exist ::env(DEBUG_TOLTCL)] && $::env(DEBUG_TOLTCL) eq "yes"} {
  set tt_debug "d"  
} else {
  set tt_debug ""  
}

set info_script [info script]
if {[file type $info_script] eq "link"} {
  set info_script [file readlink $info_script]
}
set toltk_script_path [ file normalize [file dirname $info_script] ]
# load tclIndex
set dir $toltk_script_path
source [file join $toltk_script_path "tclIndex"]
unset dir

set executable_path [ file normalize [file dirname [info nameofexecutable] ] ]
array set GuiVersionInfo {major 1 minor 1 release 6 comment "cvs trunk"}

# path user
if { [string equal $tcl_platform(platform) "windows"] } {
  set home_path $env(USERPROFILE)  
} else {
  set home_path $env(HOME)  
}

if { [string equal $tcl_platform(platform) "windows"] } {
  #@ Cambia el 'current directory' al bin de tol para evitar problemas en la carga
  set tolBin [file normalize [file join [file dir [info script]] .. .. .. bin]]
  cd $tolBin
}

if { ![info exists toltk_library] } {
  set toltk_library [file normalize [file join $toltk_script_path ..]]
}
if {[lsearch $auto_path $toltk_library] == -1} {
  set auto_path [linsert $auto_path 0 $toltk_library]
}


# Load project
namespace import project::*
set project::data(iniproject) 1
set project::data(defaultpackages) 1
set state ""
set project::data(-project) $toltk_script_path
foreach option $argv {
  if { $state eq "" } {
    switch -- $option {
      -b {
        set project::data(oculto) 1
      }
      -np {
        set project::data(iniproject) 0
      }
      -ndp {
        set project::data(defaultpackages) 0
      }
      -project {
        set state "project"
      }
      default {
        puts "unknown option $option"
      }
    }
  } else {
    switch $state {
      project {
        set project::data(-project) [ file normalize $option ]
      }
      default {
        puts "unknown state $state"
      }
    }
    set state ""
  }
}

# Get info about project
set infoProjects [ loadInfoProjects $project::data(-project) ]
project::project $infoProjects

# load iniFile
::iniFile::Create [file join $home_path "tolcon.ini"]

# load language files, stored in msgs subdirectory
set lang [::iniFile::Read TolConsole language es]

::msgcat::mclocale $lang

# Load Packages
LoadPackages

Tolcon_Trace "toltk_script_path=$toltk_script_path"

Tolcon_Trace "begin"
if { [winfo exists .__debugwin] } {
  wm geometry .__debugwin 1000x170+20+550
}

::msgcat::mcload [file join $toltk_script_path msgs]
#option read [file join $::BWIDGET::LIBRARY "lang" "${lang}.rc"] startupFile

# inicializo el kernel de tol con el lenguaje apropiado

set _lang [ string range $lang 0 1 ]
if { $_lang ne "es" && $_lang ne "en" } {
  # tol just know to deal with english or spanish
  set _lang "en"
}

::tol::initkernel $_lang vA

# Evalute the client TOL code to expose rmtps_client functionality
#
::rmtps_client::define_TOLAPI 


#Cargo word.tcl (version modificada por Bayes)
AutoLoadWord

Tolcon_Trace "Librerias cargadas"

::tol::language $_lang
Supertext::overrideTextCommand
# force BWidget autoload
Bitmap::use

set toltk_images_path [file join ${toltk_script_path} images]
if { [lsearch $Bitmap::path $toltk_images_path] == -1 } {
  lappend Bitmap::path [file join $toltk_script_path images]
}

#Añado las opciones de un text a la ventana de debug
if { [winfo exists .__debugwin] } {
  ::BayesText::MenuCreate .__debugwin.sw.t
}

# cargo los hooks ya que no entran en el mecanismo de tclIndex
source [file join $toltk_script_path tolmethods.tcl]

catch {
  project::init
} msgerr
Tolcon_Trace "$msgerr : $::errorInfo" "red bold"

# Show Info GCFFile
::TolConsole::GetGCFFile

Tolcon_Trace "argv0= $::argv0, argc= $::argc, argv= $::argv"
#puts "argv0= $::argv0, argc= $::argc, argv= $::argv"

set loadandrun 0
set include 0
set salir 0
foreach file $argv {
  if { [string equal $file "-b"] } { set salir 1 }
  if { [string equal $file "-r"] } {
    set loadandrun 1
    continue
  }
  if { [string equal $file "-i"] } {
    set include 1
    continue
  }
  OpenAssocFile $file $loadandrun $include
  set loadandrun 0
  set include 0
}

if { $salir } { exit }

# Gives a name for the application
#if {$project::data(name) eq ""} {
#  set prj ""
#} else  {
#  set prj "- $project::data(name)"
#}
#wm title . "TOLBase $prj"
# Show the dialog

#::TolConsole::ODBCShow

#/////////////////////////////////////////////////////////////////////////////
