#/////////////////////////////////////////////////////////////////////////////
# FILE    : prjmenu.tcl
# PURPOSE : This file try to make easy the task of creating menus. Inspired in
#           http://mini.net/tcl/827.html
#/////////////////////////////////////////////////////////////////////////////


namespace eval prjmenu {
  variable menus
}


#/////////////////////////////////////////////////////////////////////////////
  proc prjmenu::create { w mdesc } {
#
# PURPOSE:
#
# PARAMETERS:
#   w:
#   mdesc:
#
#/////////////////////////////////////////////////////////////////////////////

  variable menus


  if {$w=="."} {set w2 ""} else {set w2 $w}
  # se comprueba que no exista el menu
  if { [info exists $w2.menubar] } {
  } else {
    frame $w2.menubar -bd 0
    pack  $w2.menubar -side top -fill x -expand no
    set menus($w) [menu $w2.menubar.menu -tearoff 0]
    $w config -menu $w2.menubar.menu
    foreach item $mdesc {
      prjmenu::add $w "" $item
    }
  }

if {0} {
  if {$w=="."} {set w2 ""} else {set w2 $w}
  # se comprueba que no exista el menu
  if { [info exists $w2.menubar] } {
  } else {
    set menus($w) [menu $w2.menubar -tearoff 0]
    $w config -menu $w2.menubar
    foreach item $mdesc {
      prjmenu::add $w "" $item
    }  
  }
}
# comentado todo
#  variable menus

#  if {$w=="."} {set w2 ""} else {set w2 $w}
#  set menus($w) [menu $w2.menubar -tearoff 0]
#  $w config -menu $w2.menubar
#  foreach item $mdesc {
#    prjmenu::add $w "" $item
#  }  
}


#/////////////////////////////////////////////////////////////////////////////
  proc prjmenu::add { w m mdesc } {
#
# PURPOSE:
#
# PARAMETERS:
#   w:
#   m:
#   mdesc:
#
#/////////////////////////////////////////////////////////////////////////////
  variable menus

  if { $m == "" } {
    set top $menus($w)
  } else {
    set top $menus($w,$m)
  }
  set _label [lindex $mdesc 0]
  if { [string equal $_label "--"] } {
    $top add separator
    return
  }

  # look for underline character position
  set idxu -1
  set cur  0
  set escape 0
  set label ""
  foreach ch [split $_label ""] {
    if { $ch == "&" } {
      if { $escape } {
        append label &
        set escape 0
      } else {
        set idxu $cur 
      }
    } elseif { $ch == "\\" } {
      set escape 1
    } else {
      if { $escape } {
        append label "\\"
        set escape 0
      }
      append label $ch
    }
    incr cur
  }
  
  # figure it out the type
  array set options [lrange $mdesc 1 end]
  if { [info exists options(-menu)] ||
     [info exists options(-post)] } {
    set type cascade
  } elseif { [info exists options(-value)] ||
	   [info exists options(-variable)]} {
    set type radio
  } elseif { [info exists options(-onvalue)] || \
		 [info exists options(-offvalue)] } {
    set type check
  } else {
    set type command
  }
  # alias -cmd
  if { [info exist options(-cmd)] } {
    set options(-command) $options(-cmd)
    unset options(-cmd)
  }
  switch $type {
    cascade {
      # specials menus
      if { [string equal $label "Help"] } {
        set idmenu help
      } else {
        regsub -all " " $label "_" id
        set idmenu m$id
      }
      set menus($w,$label) $top.$idmenu
      menu $menus($w,$label) -tearoff 0
      $top add cascade -label $label -menu $menus($w,$label)
      if { [info exist options(-post)] } {
        $menus($w,$label) configure \
          -postcommand "$options(-post) $menus($w,$label)"
      } else {
        foreach item $options(-menu) {
          prjmenu::add $w $label $item
        }
      }
    }
    radio -
    check -
    command {
      eval "$top add $type -label [list $label] [array get options]"
    }
  }
  if { $idxu != -1 } {
    $top entryconfigure last -underline $idxu 
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc prjmenu::delete { w m name } {
#
# PURPOSE:
#
# PARAMETERS:
#   w:
#   m:
#   name:
#
#/////////////////////////////////////////////////////////////////////////////
  variable menus

  if { $m == "" } {
    set top $menus($w)
  } else {
    set top $menus($w,$m)
  }
  set idx [$top index $name]
  $top delete $idx

  # esto no esta ni bien ni completo.
}


#/////////////////////////////////////////////////////////////////////////////
  proc prjmenu::state { w m name state} {
#
# PURPOSE:
#
# PARAMETERS:
#   w:
#   m:
#   name:
#   state:
#
#/////////////////////////////////////////////////////////////////////////////
  variable menus

  if { $m == "" } {
    set top $menus($w)
  } else {
    set top $menus($w,$m)
  }
  if { ![catch "set idx [$top index $name]"] } {
    $top entryconfigure $idx -state $state
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc prjmenu::disable { w m name } {
#
# PURPOSE:
#
# PARAMETERS:
#   w:
#   m:
#   name:
#
#/////////////////////////////////////////////////////////////////////////////
  prjmenu::state $w $m $name disabled
}


#/////////////////////////////////////////////////////////////////////////////
  proc prjmenu::enable { w m name } {
#
# PURPOSE:
#
# PARAMETERS:
#   w:
#   m:
#   name:
#
#/////////////////////////////////////////////////////////////////////////////
  prjmenu::state $w $m $name normal
}


# test description
if { 0 } {
  package require Tk
  package require msgcat

  catch {namespace import ::msgcat::*}

  proc Recent { m } {
    $m delete 0 end
    $m add command -label "File 1" -command "puts file1"
    $m add command -label "File 2" -command "puts file2"
  }
  
  set mdesc {
    {&File -menu {
      {&New -cmd "puts new"}
      {&Open -cmd "puts open"}
      --
      {E&xit -cmd "puts exit"}}}
    {"&Recent files" -post Recent}
    {&Options -menu {
      {&Band -menu {
	{&AM -variable banda}
	{&FM -variable banda}
	{&SW -variable banda}}}
      {Check -onvalue 1 -offvalue 0}}}
    {&Help -menu {
      {&Content -cmd "puts Contenido"}
      {&About -cmd "puts Acerca"}}}
    {&Windows -cmd "puts Windows"}}
  
  if ![winfo exists .tol] {toplevel .tol}  
  prjmenu::create .tol $mdesc

  wm maxsize .tol [winfo vrootw .] 1
  wm geometry .tol 500x1
}

