#/////////////////////////////////////////////////////////////////////////////
# FILE    : bcombobox.tcl
# PURPOSE : Bayes Combo Box. ComboBox con varias columnas aunque solamente una
#           puede ser visible. 
#/////////////////////////////////////////////////////////////////////////////

package require byscommon
package require bfunctions


#/////////////////////////////////////////////////////////////////////////////
::snit::widgetadaptor bcombobox {
#
# PURPOSE: Sobrecargamos un ComboBox para que tenga nuevas funcionalidades
#
#/////////////////////////////////////////////////////////////////////////////
  # opciones especificas
  # -values      -> la lista de valores (con todos los campos)
  # -cols      -> lista de campos del combobox
  # -viscol    -> campo visible
  # -empty     -> indica si se introduce un elemento en blanco [flase|true]
  # -keypress -> Activa la busqueda de filas mediante la pulsacion de teclas
  # -colkeypress -> Indica por que columna se busca al pulsar una tecla  
  #
  # variables
  #  afterTime -> tiempo desde que se pulso la ultima tecla
  #  listKey   -> lista de teclas pulsadas
  #  entry     -> componente entry del combobox
  #  listbox   -> componente listbox del combobox
  #  button    -> path del button del combobox
#/////////////////////////////////////////////////////////////////////////////

  option -values {}
  option -cols {}
  option -viscol {}
  option -empty false
  option -keypress true
  option -colkeypress ""

  delegate option * to hull
  delegate method * to hull

  variable listbox

  variable afterTime 0
  variable listKey   ""


  #///////////////////////////////////////////////////////////////////////////
  constructor { args } {
  #
  # PURPOSE: Constructor de un bcombobox. Crea una instancia de un ComboBox
  #
  # PARAMETERS:
  #   args -> Lista de opciones especificas de un ComboBox, mas las propias de
  #           un bcombobox
  #
  #///////////////////////////////////////////////////////////////////////////
    set values [from args -values ""]
    set cols [from args -cols ""]
    set viscol [from args -viscol ""]
    set empty [from args -empty false]
    set keypress [from args -keypress true]

    # Widget que va a servir de contenedor para blistbox
    eval installhull using ComboBox $args
    
    # Apply all arguments given:
    $self configurelist $args
    $self configure -empty $empty
    $self configure -cols $cols
    $self configure -viscol $viscol
    $self configure -values $values
    $self configure -keypress $keypress
    
    # bind
    set listbox [$hull getlistbox]
    bind $listbox <KeyPress> +[list $self _OnKeyPress %A]
  }


  #///////////////////////////////////////////////////////////////////////////
  #
  # Metodos cget y configure
  #
  #///////////////////////////////////////////////////////////////////////////

  onconfigure -values { lst } {
    if { $options(-empty) } {
      set null {}
      for {set i 0} {$i < [llength [lindex $lst 0]]} {incr i} {
        lappend null {}
      }
      set lst [linsert $lst 0 $null]
    }
    set options(-values) $lst
    
    if { $options(-cols) eq "" } {
       $hull configure -values $lst
    } else {
      set auxlst {}
      if { $options(-viscol) eq "" } {
        set inx 0
      } else {
        set inx [lsearch $options(-cols) $options(-viscol)]
      }
      foreach i $options(-values) {
        lappend auxlst [lindex $i $inx]
      }
      $hull configure -values $auxlst
    }
  }

  onconfigure -cols { cols } {
    set options(-cols) $cols
    set inx 0
    set auxlst {}
    if { $options(-viscol) ne "" } { set inx [lsearch $cols $options(-viscol)] }
    foreach i $options(-values) {
      lappend auxlst [lindex $i $inx]
    }
    $hull configure -values $auxlst
  }
    


  onconfigure -viscol { col } {
    set options(-viscol) $col

    if { $options(-values) ne "" } {
      set auxlst {}
      if { $options(-viscol) eq "" } {
        set inx 0
      } else {
        set inx [lsearch $options(-cols) $options(-viscol)]
      }
      foreach i $options(-values) {
        lappend auxlst [lindex $i $inx]
      }
      $hull configure -values $auxlst
    }
  }


  #///////////////////////////////////////////////////////////////////////////
  method _OnKeyPress {key} {
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
    if {[$self cget -keypress]} {
      set diference 500
      set result [lbOnKeyPress $listbox $key $listKey $afterTime $diference]
      set listKey   [lindex $result 0]
      set afterTime [lindex $result 1]
    }
  }


  #///////////////////////////////////////////////////////////////////////////
  method class { } {
  #
  # PURPOSE: Devuelve el tipo de clase
  #
  #///////////////////////////////////////////////////////////////////////////
    return "BComboBox"
  }

  
  #///////////////////////////////////////////////////////////////////////////
  method get { {cols ""} } {
  #
  # PURPOSE: Devuelve los campos pedidos del elemento seleccionado
  #
  # PARAMETERS:  cols -> campos para los que se quiere devolver valores del
  #                      elemento seleccionado, si no se especifica se
  #                      devuelven todos
  #
  #///////////////////////////////////////////////////////////////////////////
    set data [lindex $options(-values) [$self getvalue]]
    if { "" eq $cols } { return $data }

    set result {}
    foreach col $cols {
      set inx [lsearch $options(-cols) $col]
      lappend result [lindex $data $inx]
    }
    return $result
  }

  #///////////////////////////////////////////////////////////////////////////
  method list { command args } {
  #
  # PURPOSE: comando para el mantenimiento de la lista del combobox
  #
  # PARAMETERS:  
  #     command [args]:
  #        get [<cols>]: devuelve la lista opcionalmente con solo los campos
  #                      que se especifiquen
  #        set <list>: establece la lista 
  #        clear: borra la lista
  #        size: devuelve la cantidad de elementos de la lista
  #        insert <pos> <lst>: inserta elementos (<lst>) en la
  #            lista en la posicion <pos> con informacion solamente para
  #            los campos <cols> si se especifican
  #        add <elems>: igual que insert pero siempre añade al final
  #
  #///////////////////////////////////////////////////////////////////////////
    switch -exact $command {
      get {
        if { [lindex $args 0] ne "" } {
          set cols [lindex $args 0]
	  set result {}
	  
	  foreach elem $options(-values) {
	    set tmpelem {}
	    foreach col $cols {
	      lappend tmpelem [lindex $elem [lsearch $options(-cols) $col]]
	    }
	    lappend result $tmpelem
	  }
	  return $result
	} else {
	  return $options(-values)
	}
      }

      set {
        $self configure -values [lindex $args 0]
      }

      clear {
        $self configure -values {}
      }

      size {
        llength $options(-values)
      }

      insert {
	set lst $options(-values)
	$self configure -values [linsert $lst [lindex $args 0] [lindex $args 1]]
      }

      add {
	set lst $options(-values)
	$self configure -values [lappend lst [lindex $args 0]]
      }
    }
  }

}
  
package provide bcombobox 1.0

#/////////////////////////////////////////////////////////////////////////////
proc test_bcombobox {} {
#
# PURPOSE: Funcion de test de un bcombobox
#
#/////////////////////////////////////////////////////////////////////////////
  
  # defining the widgets
  set __cb1 0
  set __cb2 1
  
  destroy .top
  toplevel .top -width 200 -height 100 
    
  TitleFrame .top.tf1 -text "BComboBox test"
  set f1 [.top.tf1 getframe]
  bcombobox $f1.bcb
  button $f1.b -text "Get values" -command "GetCmd"
  LabelEntry $f1.le -label "Result"

  TitleFrame .top.tf2 -text "Configure"
  set f2 [ .top.tf2 getframe ]
  LabelEntry $f2.le1 -label "Visual Col" -text "col2"
  LabelEntry $f2.le2 -label "Columns" -text {col1 col2 col3}
  LabelEntry $f2.le3 -label "Values" -text {{1 2 3} {a b c} {@ # $} {i ii iii}}
  checkbutton $f2.cb1 -text "Empty" -variable __cb1 \
    -command "EmpCmd"
  checkbutton $f2.cb2 -text "Keypresses" -variable __cb2 \
    -command "KeyCmd"
  $f2.cb1 deselect
  $f2.cb2 select
  button $f2.b1 -text "Configure" -command "CfgCmd"
  

  # packing all 
  
  grid .top.tf1 -row 0 -column 0 -sticky "news" -padx 10 -pady 10
  grid .top.tf2 -row 1 -column 0 -sticky "news" -padx 10 -pady 10
  grid rowconfigure .top 0 -weight 0
  grid rowconfigure .top 1 -weight 1
  grid columnconfigure .top 0 -weight 1

  grid $f1.bcb -row 0 -column 0 -sticky ew
  grid $f1.b   -row 0 -column 1 -sticky ew
  grid $f1.le  -row 1 -column 0 -sticky ew -columnspan 2
  grid columnconfigure  $f1 0 -weight 1
  grid columnconfigure  $f1 1 -weight 0
  grid rowconfigure $f1 0 -weight 1
  grid rowconfigure $f1 1 -weight 1
  
  grid $f2.le1 -row 0 -column 0 -sticky ew -padx 2 -pady 2
  grid $f2.le2 -row 1 -column 0 -sticky ew -padx 2 -pady 2
  grid $f2.le3 -row 2 -column 0 -sticky ew -padx 2 -pady 2 -columnspan 2
  grid $f2.cb1 -row 0 -column 1 -sticky w -padx 2
  grid $f2.cb2 -row 1 -column 1 -sticky w -padx 2
  grid $f2.b1  -row 3 -column 0 -sticky ew -padx 2 -pady 5 -columnspan 3
  grid columnconfigure  $f2 0 -weight 1
  grid columnconfigure  $f2 1 -weight 0
  grid rowconfigure $f2 0 -weight 0
  grid rowconfigure $f2 1 -weight 0
  grid rowconfigure $f2 2 -weight 0
  grid rowconfigure $f2 3 -weight 1


  # commands attached to the widgets

  proc CfgCmd { } {
    .top.tf1.f.bcb configure -values [.top.tf2.f.le3 cget -text]
    .top.tf1.f.bcb configure -cols [.top.tf2.f.le2 cget -text]
    .top.tf1.f.bcb configure -viscol [.top.tf2.f.le1 cget -text]
  }

  proc GetCmd { } {
    .top.tf1.f.le configure -text [.top.tf1.f.bcb get]
  }
  
  proc EmpCmd { } {
    variable __cb1
    .top.tf1.f.bcb configure -empty $__cb1
  }

  proc KeyCmd { } {
  variable __cb2
    .top.tf1.f.bcb configure -keypress $__cb2
  }
}
