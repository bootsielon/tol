#/////////////////////////////////////////////////////////////////////////////
# FILE    : blistbox.tcl
# PURPOSE : Bayes List Box. ListBox con opciones sobre su elementos
#/////////////////////////////////////////////////////////////////////////////

package require byscommon
package require bfunctions
package require bfieldsdialog


#/////////////////////////////////////////////////////////////////////////////
::snit::widgetadaptor blistbox {
#
# PURPOSE: Sobrecargamos un ListBox para que tenga nuevas funcionalidades
#
#/////////////////////////////////////////////////////////////////////////////
  # opciones especificas
  # breadonly -> componente en modo de lectura o lectura/escritura
  # bwriter -> widget donde mostrar los mensages
  # bkeypress -> Activa la busqueda de filas medainte la pulsacion de teclas
  # bparent -> indica el padre donde se ubica el blistbox

  # varaibles
  # afterTime -> tiempo desde que se pulso la ultima tecla
  # listKey -> lista de teclas pulsadas
#/////////////////////////////////////////////////////////////////////////////
  option -breadonly  false
  option -bwriter    {}
  option -bkeypress  true
  option -bparent    {}
  option -ilistvariable {}

  variable afterTime 0
  variable listKey   ""
  variable ctxMenu

  #///////////////////////////////////////////////////////////////////////////
  constructor {args} {
  #
  # PURPOSE: Constructor de un blistbox. Crea una instancia de un Listbox
  #
  # PARAMETERS:
  #   args -> Lista de opciones especificas de un ListBox, mas las propias de
  #           un blistbox
  #
  #///////////////////////////////////////////////////////////////////////////
    # Widget que va a servir de contenedor para blistbox
    installhull [listbox $self]
    # options default
    $self configure -selectmode extended
    # Apply all arguments given:
    $self configurelist $args
    # creamos menu
    $self CreateMenu    
    # bind
    bind $self <KeyPress> +[list $self OnKeyPress %A]
  }

  #///////////////////////////////////////////////////////////////////////////
  method OnKeyPress {key} {
  #
  # PURPOSE: Activa la fila que coincide con las teclas pulsadas. Distinge
  #          entre muysuculas y minusculas
  #
  # PARAMETERS: key -> tecla pulsada
  # TODO: opciones de componente para un futuro
  #   - especificar el tiempo que espara entre pulsaciones
  #   - case sensotive
  #
  #///////////////////////////////////////////////////////////////////////////
    if {[$self cget -bkeypress]==true} {
      set diference 500
      set result [lbOnKeyPress $self $key $listKey $afterTime $diference]
      set listKey   [lindex $result 0]
      set afterTime [lindex $result 1]
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method CreateMenu {args} {
  #
  # PURPOSE: Append menu to listbox. 
  #
  #///////////////////////////////////////////////////////////////////////////
    set m [menu $self.m -tearoff 0]
    set ctxMenu $m
    # read option
    $m add command -label [mc "Select all"] \
      -command "lbSelectAll $self"  -accelerator CTRL+E
    $m add command -label [mc "Unselect all"] \
      -command "$self selection clear 0 end"
    $m add command -label [mc "Toggle selection"] \
      -command "lbToggleSelection $self"
    # read/write option
    if {![$self cget -breadonly]} {
      # delete 
      $m add separator
      $m add command -label [mc "Insert"] \
        -command "$self openWR" -accelerator Ins
      $m add command -label [mc "Delete all"]  \
        -command "$self deletespecial"
# -accelerator SUPR
      $m add command -label [mc "Delete selected"] \
        -command "$self deletespecial -unselected 0"
    }
    # read option
    # copy
    $m add separator
    set mc [menu $m.copy -tearoff 0]
    $m add cascade -label [mc "Copy"] -menu $mc
    $mc add command -label [mc "Copy all"] \
      -command "lbToClipBoard $self 1 1"
    $mc add command -label [mc "Copy selected"] \
      -command "lbToClipBoard $self 1 0" -accelerator CTRL+C
    $mc add command -label [mc "Copy unselect"] \
      -command "lbToClipBoard $self 0 1"
    # count
    $m add separator
    $m add command -label [mc "Count elements"] \
      -command "$self ShowCountItems"
    # file
    $m add separator
    set mf [menu $m.file -tearoff 0]
    $m add cascade -label [mc "File"] -menu $mf
    $mf add command -label "[mc "Save to file"] ..." \
      -command "lbSaveToFile $self 1 1"
    if {![$self cget -breadonly]} {
      $mf add command -label "[mc "Load form file"] ..." \
        -command "lbLoadFromFile $self"
    }

    bind $self <Control-KeyRelease> +[list [codename OnControlKey] $self %K %k]
    bind $self <Key-Delete> "lbDeleteSelection $self"
    bind $self <Key-Insert> "$self openWR"
    bind $self <Button-1> +[list focus -force %W]
    bind $self <Button-3> "tk_popup $m %X %Y"
  }

  #///////////////////////////////////////////////////////////////////////////
  method getMenu {} {
  #
  # PURPOSE: Get Menu path to add entries on it. 
  #
  #///////////////////////////////////////////////////////////////////////////
    return $ctxMenu
  }


  #///////////////////////////////////////////////////////////////////////////
  proc OnControlKey {_lb keysym keycode} {
  #
  # PURPOSE: Controla es uso de teclas rápidas para le ejecución de funciones
  #
  # PARAMETERS: _lb -> self
  #             keysym -> The keysym corresponding to the event, 
  #                       substituted as a textual string
  #             keycode -> The keycode field from the event
  #
  #///////////////////////////////////////////////////////////////////////////
    switch [string toupper $keysym] {
      E { lbSelectAll   $_lb     }
      C { lbToClipBoard $_lb 1 0 }
    }
  }  

  #///////////////////////////////////////////////////////////////////////////
  method ShowCountItems {} {
  #
  # PURPOSE: Shows information on the selection of the elements
  #
  #///////////////////////////////////////////////////////////////////////////
    set info [lbCountItems $self]
    set msg "[mc "Total"]: [lindex $info 0] \n\
             [mc "Selected"]: [lindex $info 1] \n\
             [mc "Not Selected"]: [lindex $info 2]"
    if {[string length [$self cget -bparent]]} {
      set par $self
    } else {
      set par [$self cget -bparent] 
    }
    tk_messageBox -default ok \
                  -icon info  \
                  -message $msg \
                  -type ok \
                  -parent $par \
                  -title [mc "numero de elementos"]
  }

  #///////////////////////////////////////////////////////////////////////////
  method getspecial {args} {
  #
  # PURPOSE: get the values the special list
  # PARAMETERS:
  #   selected: the selected item
  #   unselected: the unselected item
  #   ini: the first index
  #   end: the last index
  # RETURN: one list with then range values
  #
  #///////////////////////////////////////////////////////////////////////////
    # inizializate
    array set opts [list \
      -selected   1   \
      -unselected 1   \
      -ini        0   \
      -end        end \
    ]
    array set opts $args
    set lst {}
    # find
    set n [$self index end]
    set orig \
      [expr [$self index $opts(-ini)]<$n ? [$self index $opts(-ini)] : $n-1]
    set last \
      [expr [$self index $opts(-end)]<$n ? [$self index $opts(-end)] : $n-1]
    for {set f $orig} {[expr $f<=$last]} {incr f} { 
      if {( [$self selection includes $f] && $opts(-selected)) || \
          (![$self selection includes $f] && $opts(-unselected))} {
          lappend lst [lindex $options(-ilistvariable) $f]
      }
    }
    # return
    return $lst 
  } 

  #///////////////////////////////////////////////////////////////////////////
  method insertspecial {pos lst} {
  #
  # PURPOSE: insert the elements in listbox and the special list
  # PARAMETERS:
  #   lst: list the list. The first item corresponds in listbox, the second
  #        in special list
  #   pos: position to insert
  # RETURN: one list with then range values
  #
  #///////////////////////////////////////////////////////////////////////////
    # items 
    foreach l $lst {
      lappend lst_lb [lindex $l 0]
      lappend lst_sl [lindex $l 1]
    }
    # insert
    eval $hull insert $pos $lst_lb
    set options(-ilistvariable) \
        [eval linsert [list $options(-ilistvariable)] $pos $lst_sl]
  }

  #///////////////////////////////////////////////////////////////////////////
  method deletespecial {args} {
  #
  # PURPOSE: delete the elements in listbox and the special list
  # PARAMETERS:
  #   selected: the selected item
  #   unselected: the unselected item
  #   ini: the first index
  #   end: the last index
  #
  #///////////////////////////////////////////////////////////////////////////
    # inizializate
    array set opts [list \
      -selected   1   \
      -unselected 1   \
      -ini        0   \
      -end        end \
    ]
    array set opts $args
    set lst {}
    # find
    set n [$self index end]
    set orig \
      [expr [$self index $opts(-ini)]<$n ? [$self index $opts(-ini)] : $n-1]
    set last \
      [expr [$self index $opts(-end)]<$n ? [$self index $opts(-end)] : $n-1]
    for {set f $last} {[expr $f>=$orig]} {incr f -1} { 
      if {( [$self selection includes $f] && $opts(-selected)) || \
          (![$self selection includes $f] && $opts(-unselected))} {
        $hull delete $f
        set options(-ilistvariable) [lreplace $options(-ilistvariable) $f $f]
      }
    }
  }


  #///////////////////////////////////////////////////////////////////////////
  method insert {index args} {
  #
  # PURPOSE: insert the elements in listbox and mantains the special list
  # PARAMETERS:
  #   index: position to insert
  #   args: list of elements to insert.
  #
  #///////////////////////////////////////////////////////////////////////////
    if { [llength $args] > 0 } {

      foreach par $args {
        lappend lst_lb [lindex $par 0]
        lappend lst_sl [lindex $par 1]
      }
	
      set options(-ilistvariable) \
        [eval linsert [list $options(-ilistvariable)] $index $lst_sl]
    
      #llamamos al metodo original
      eval $hull insert $index $lst_lb
    }
    return ""
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method delete {ini {fin ""}} {
  #
  # PURPOSE: deletes the elements in listbox between ini and fin
  # PARAMETERS:
  #   ini: first position to delete 
  #   fin: last position to delete
  #
  #///////////////////////////////////////////////////////////////////////////
    if { $fin eq "" } { set fin $ini }
    set options(-ilistvariable) \
        [eval lreplace [list $options(-ilistvariable)] $ini $fin]
    #llamamos al metodo original
    $hull delete $ini $fin
  }


  #///////////////////////////////////////////////////////////////////////////
  proc addWR { elem father } {
  #
  # PURPOSE: adds the item introduced in the dialog to the selected list
  #
  # PARAMETERS:
  #  elem -> the element to insert from the add dialog
  #  father -> The dialog's window pathname
  #
  #///////////////////////////////////////////////////////////////////////////

    eval $father insert 0 $elem
  }


  #//////////////////////////////////////////////////////////////////////////
  method openWR {} {
  #
  # PURPOSE: Calls (and creates) a multiple selector widget with the selected
  #          and unselected items.
  #
  #///////////////////////////////////////////////////////////////////////////

    destroy $self.dw
    set dialog  [bfieldsdialog $self.dw -title [mc "Insert element"] \
                           -parent $self -fields [list [mc "Element"]] \
			   -procedure [codename addWR] -buttons {add exit}]

    $dialog draw
  }

  # delegacion de metodos y opciones
  delegate method * to hull
  delegate option * to hull
}

package provide blistbox 1.0

# ------- TEST CODE ----

#/////////////////////////////////////////////////////////////////////////////
proc test_blistbox {} {
#
# PURPOSE: Funcion de test de un blistbox
#
#/////////////////////////////////////////////////////////////////////////////
  
  # defining the widgets
  variable data
  set data(-selected) 1
  
  destroy .top
  toplevel .top -width 200 -height 100 
    
  TitleFrame .top.f1 -text "BListBox test"
  set f1 [ .top.f1 getframe ]
  blistbox $f1.blb
  LabelEntry $f1.ler -label Result
  TitleFrame .top.f2 -text "Configure"
  set f2 [ .top.f2 getframe ]
  radiobutton $f2.rbs -text "Selected" -value 1 -variable data(-selected)
  radiobutton $f2.rbu -text "UnSelected" -value 0 -variable data(-selected)
  LabelEntry $f2.lei -width 5 -label Index -text 0
  LabelEntry $f2.lee -width 5 -label End
  LabelEntry $f2.lec -width 30 -label Content \
            -text "{1 uno} {2 dos} {3 tres} {4 cuatro}"
  button $f2.bi -text "Insert" -command { insertCmd }
  button $f2.bd -text "Delete" -command { deleteCmd }
  button $f2.br -text "Result" -command { resultCmd }
  button $f2.bis -text "InsertSpecial" -command { insertSpecialCmd }
  button $f2.bds -text "DeleteSpecial" -command { deleteSpecialCmd }
  

  # packing all 
  
  grid .top.f1 -row 0 -column 0 -sticky "new" -padx 10 -pady 10
  grid .top.f2 -row 1 -column 0 -sticky "new" -padx 10 -pady 10
  grid columnconfigure .top 0 -weight 1
  grid rowconfigure .top 0 -weight 0
  grid rowconfigure .top 1 -weight 1
  
  grid $f1.blb -row 0 -column 0 -sticky "new"
  grid $f1.ler -row 1 -column 0 -sticky "new"
  grid columnconfigure $f1 0 -weight 0
  grid rowconfigure $f1 0 -weight 0
  grid rowconfigure $f1 1 -weight 1
  
  grid $f2.rbs -row 0 -column 0 -sticky "new"
  grid $f2.rbu -row 0 -column 1 -sticky "new"
  grid $f2.lei -row 1 -column 0 -sticky "new"
  grid $f2.lee -row 1 -column 1 -sticky "new"
  grid $f2.lec -row 1 -column 2 -sticky "new"
  grid $f2.bi -row 2 -column 0 -sticky "new"
  grid $f2.bd -row 2 -column 1 -sticky "new"
  grid $f2.br -row 2 -column 2 -sticky "news" -rowspan 2
  grid $f2.bis -row 3 -column 0 -sticky "new"
  grid $f2.bds -row 3 -column 1 -sticky "new"
  grid columnconfigure $f2 0 -weight 0
  grid columnconfigure $f2 1 -weight 0
  grid columnconfigure $f2 2 -weight 0
  grid columnconfigure $f2 3 -weight 1
  grid rowconfigure $f2 0 -weight 0
  grid rowconfigure $f2 1 -weight 0
  grid rowconfigure $f2 2 -weight 0
  grid rowconfigure $f2 3 -weight 0
  
  
  
  # commands attached to the widgets
  
  proc insertCmd {} {
    eval .top.f1.f.blb insert [.top.f2.f.lei cget -text] \
                         [.top.f2.f.lec cget -text]
  }
  
  proc deleteCmd {} {
    .top.f1.f.blb delete [.top.f2.f.lei cget -text] \
                         [.top.f2.f.lee cget -text]
  }
  
  proc insertSpecialCmd {} {
    .top.f1.f.blb insertspecial [.top.f2.f.lei cget -text] \
                                [.top.f2.f.lec cget -text]
  }
  
  proc deleteSpecialCmd {} {
    variable data
	
	set ini [.top.f2.f.lei cget -text]
    set end [.top.f2.f.lee cget -text]
	set selected $data(-selected)
	set unselected [expr ! $data(-selected)]
	
	if { $ini ne "" } { set ini "-ini $ini" }
	if { $end ne "" } { set end "-end $end" }
	
	if { $ini ne "" } {
	  if { $end ne ""} {
	    eval .top.f1.f.blb deletespecial $ini $end
	  } else {
	    eval .top.f1.f.blb deletespecial $ini
      }
    } else {
	  .top.f1.f.blb deletespecial -selected $selected -unselected $unselected
    }
  }
  
  proc resultCmd {} {
    set res [.top.f1.f.blb getspecial]
    .top.f1.f.ler configure -text $res
  }
}
