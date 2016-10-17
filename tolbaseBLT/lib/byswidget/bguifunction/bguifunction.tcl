#/////////////////////////////////////////////////////////////////////////////
# FILE    : guifunction.tcl
# PURPOSE : This file contains a namespace called GuiFunction with its
#           functions. It's for creating a general dialog that takes values
#           for arguments of a given function. Finally it calls to this 
#           function
#/////////////////////////////////////////////////////////////////////////////

package require byscommon


#/////////////////////////////////////////////////////////////////////////////
::snit::widgetadaptor bguifunction {
#
# PURPOSE: Defincición de las funcionalidades de un 
#
#/////////////////////////////////////////////////////////////////////////////

#
#/////////////////////////////////////////////////////////////////////////////
  option -regpars
  variable parInf

  #///////////////////////////////////////////////////////////////////////////
  constructor {args} {
  #
  # PURPOSE: guifunction constructor
  #
  # PARAMETERS:
  #   args ->
  #
  #///////////////////////////////////////////////////////////////////////////
  installhull using Dialog
  # Apply default arguments
  $self configure -modal local -default 0 -cancel 1 -separator true  
  # Apply all options passed at creation time.
  $self configurelist $args
  set lstPar [$self get parameters]

  if [llength $lstPar] {
  
    wm protocol $self WM_DELETE_WINDOW [list $self withdraw]

    set f [$self getframe]
    foreach idx $lstPar {
      set tfp [$self createParForm $f $idx]
      grid $tfp -sticky news
    }

    # Botones
    $self add -text [mc Accept] -compound left -helptext [mc "Load"] \
            -relief link -command [list $self LoadFunction]
    $self add -text [mc Cancel] -helptext [mc Cancel] -relief link\
            -command [list $self withdraw]
  
    grid columnconfigure $f 0 -weight 1
    grid rowconfigure    $f $idx -weight 1
    $self draw
  }
    
  }

  #///////////////////////////////////////////////////////////////////////////
  #
  # PURPOSE: Metodos cget y configure
  #
  #///////////////////////////////////////////////////////////////////////////


  #/////////////////////////////////////////////////////////////////////////////
  onconfigure -regpars {value} {
  #/////////////////////////////////////////////////////////////////////////////
    array set parInf $value
  }

  # delegacion de metodos y opciones
  delegate method * to hull
  delegate option * to hull

  #///////////////////////////////////////////////////////////////////////////
  method assocVarChooseDir {var} {
  #///////////////////////////////////////////////////////////////////////////
    upvar #0 $var variable
    set variable [tk_chooseDirectory]
  }

  #///////////////////////////////////////////////////////////////////////////
  method createParForm {w idx} {
  #
  #
  #///////////////////////////////////////////////////////////////////////////
    set tf [TitleFrame $w.tf$idx -text [mc $parInf($idx,nameGui)]  -bd 1]
    set f [$tf getframe]

    label $f.label -text [mc $parInf($idx,nameGui)]:
    switch [expr int( $parInf($idx,typeSel) )] {
      0 {
        set lstValues [::tol::info variable $parInf($idx,typePar)]
        ComboBox $f.entry -width 18 -height 5 -editable false\
          -values $lstValues \
          -textvariable ${selfns}::parInf($idx,value) 
        grid $f.label $f.entry -sticky nw 
      }
      1 {
        ComboBox $f.entry -width 18 -height 5 -editable false\
          -values $parInf($idx,lstVal)\
          -textvariable ${selfns}::parInf($idx,value) 
        grid $f.label $f.entry -sticky nw 
      }
      default {
        switch $parInf($idx,typePar) {
          "Dir"   {
            Entry $f.entry   -state normal -width 30 \
              -textvariable ${selfns}::parInf($idx,value)
  
            Button $f.button -compound left -padx 10 \
              -height 16 -relief link \
              -helptext [mc "Choose a directory"] \
              -image [Bitmap::get open] \
              -command [list $self assocVarChooseDir ${selfns}::parInf($idx,value)]
            grid $f.label $f.entry $f.button -sticky nw 
          }
          default {
            Entry $f.entry -textvariable ${selfns}::parInf($idx,value)
            grid $f.label $f.entry -sticky nw 
          }
        }
      }
    }

    grid columnconfigure $f 1 -weight 1
    grid rowconfigure    $f 0 -weight 1
    return $tf

  }
  #///////////////////////////////////////////////////////////////////////////
  method get {item {idx ""}} {
  #
  # PURPOSE: Returns items of guifunction dialog. Item can be:
  #  parameters -> Returns a list of all parameters of function
  #  value idx  -> Returns the value given by the user of the parameter idx
  #
  #///////////////////////////////////////////////////////////////////////////
   #puts "method get. item=$item, idx=$idx"
   set res ""
    switch -- $item {
      type {
        if {$idx eq ""} {
          error "You must specify an index: shold be get type index"
        } else  {
          set res $parInf($idx,typePar)
        }
      }
      value {
        if {$idx eq ""} {
          error "You must specify an index: shold be get value index"
        } else  {
          switch [$self get type $idx] {
            "Text" -
            "Dir"    { set res \"$parInf($idx,value)\" }
            default  { set res $parInf($idx,value)     }
          }
        }
      }
      values {
        set lst ""
        foreach p [$self get parameters] {
          lappend lst [$self get value $p]
        }
        set res $lst
      }
      parameters {
        regsub -all ,nameGui \
          [array names ${selfns}::parInf *,nameGui] "" lstArgs
        set res [lsort -integer $lstArgs]
      }
      default {
        error "Invalid option '$item': should be parameters or value"
      }
    }
    #puts "method get. Devuelvo $res"
    return $res
  }

  #///////////////////////////////////////////////////////////////////////////
  method LoadFunction {} {
  #
  #
  #///////////////////////////////////////////////////////////////////////////
    #Chequear que los parámetros obligatorios se hayan rellenado
    set lstPar [$self get parameters]
    foreach idp $lstPar {
      if {$parInf($idp,value) eq ""} {
        tk_messageBox -type ok -icon warning \
          -message [mc "Debe rellenar el parámetro %1\$s" $parInf($idp,nameGui)]\
          -parent . -title [mc Warning]
        return
      }
    }
    $self enddialog ok
  }
}



package provide bguifunction 1.0

# -------- TEST CODE ------

proc test_bguifunction { } {

  set idx 0

  incr idx
  set regPar($idx,nameGui)  TimeSet
  set regPar($idx,typePar)  TimeSet
  set regPar($idx,typeSel)  2

  incr idx  
  set regPar($idx,nameGui)  Statistic
  set regPar($idx,typePar)  Code
  set regPar($idx,typeSel)  1

  set regPar($idx,lstVal)   [list SumS LastS FirstS AvrS StDsS MinS MaxS MedianS]
  
  set gui [bguifunction .t -title "Cambio de Fechado" -regpars [array get regPar]]
  puts "values= [$gui get values]"
  #puts "El usuario ha elegido timeset=$::timeset, sta=$::statistic"
  destroy $gui
}

#test_bguifunction
