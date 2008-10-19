#/////////////////////////////////////////////////////////////////////////////
# FILE    : bselectfields.tcl
# PURPOSE : Select Fields. Componente que nos permite seleccionar un elemento
#           mostrandonos una lista con toda la informacion que le indiquemos.
#           El resultado obtenido le divide en 1+n partes, una para el codigo 
#           y n para campos que deseemos mostrar. n puede valar cero.
#/////////////////////////////////////////////////////////////////////////////

# includes
if {0} {
  package require Tk
  package require snit 0.81
  package require BWidget

  package require msgcat
  namespace import msgcat::*


  source ini_byswidget.tcl
  source ./../toltk/mclistbox.tcl

}

package require byscommon


#/////////////////////////////////////////////////////////////////////////////
::snit::widget bselectfields {
#
# PURPOSE: Defincición de las funcionalidades de un BayesSelectFields
#
#/////////////////////////////////////////////////////////////////////////////
#/////////////////////////////////////////////////////////////////////////////
# options:
#   -blstfields -> Lista de campos a mostar. El primero es el codigo
#   -blsthelptexts -> Lista de helptext que se muestran cuando se pasa el 
#                  raton por encima de los 'entries' (o labelentries)
#   -bvalues    -> lista con los valores a mostrar para la seleccion
#                  (los datos que se insertan en el listbox desplegable)
#   -btitle     -> nombre a asignar a las columnas para la selecion
#   -bmodifycmd -> comando a ejecutar despues de seleccionar un item
#   -bpostcommand -> comando a ejecutar antes de mostrar la lista
#   -label      -> Texto de encabezado del campo. Si no se especifica, se deja
#                  en blanco.
#   -labeldesc  -> Booleano para etiquetar o no las descripciones.
#   -color      -> Color para las etiquetas (red, blue, etc.).
#   -width      -> Anchura de los entries.
#
# variable
#   blstfields   -> lista de campos a visualizar
#   blstvalues   -> lista de valores a mostrar para la seleccion
#   blsttitlecol -> lista de titulos de las columnas a mostrar
#   blstentry    -> lista de objetos entry visualizados
#   blstcolumns  -> lista de listbox visualizados en la seleccion
#   bmodifycmd   -> comando a aplicar al aceptar
#   bpostcommand -> comando a ejecutar antes de mostrar la lista
#
#/////////////////////////////////////////////////////////////////////////////
  widgetclass BSelectFields

  option   -blstfields  ""
  option   -blsthelptexts  ""
  option   -bvalues     ""
  option   -btitle      ""
  option   -bmodifycmd  ""
  option   -bpostcommand ""
  option   -label       ""
  option   -labeldesc   false
  option   -color       "black"
  option   -width       20

  variable  blstfields
  variable  blsthelptexts
  variable  blstvalues
  variable  blsttitlecol
  variable  blstentry
  variable  blstcolumns
  variable  bmodifycmd
  variable  bpostcommand
  variable  blabel
  variable  bcolor
  variable  blabdesc
  variable  width
  variable data


  #///////////////////////////////////////////////////////////////////////////
  constructor {args} {
  #
  # PURPOSE: bselectfields constructor
  #
  # PARAMETERS:
  #   args ->
  #
  #///////////////////////////////////////////////////////////////////////////
    set blstvalues    $options(-bvalues)
    # Apply all options passed at creation time.
    $self configurelist $args
    #set parent $options(-parent)
    set blstfields    $options(-blstfields)
    set blsthelptexts $options(-blsthelptexts)

    set blsttitlecol  $options(-btitle)
    set bmodifycmd    $options(-bmodifycmd)
    set bpostcommand  $options(-bpostcommand)
    set blabel        $options(-label)
    set blabdesc      $options(-labeldesc)
    set bcolor        $options(-color)
    set width         $options(-width)
    # Apply all options passed at creation time.
    #$self configurelist $args
    $self CreateWidgets
  }

  #///////////////////////////////////////////////////////////////////////////
  onconfigure -bvalues {lst} {
  #
  # PURPOSE: Change list values of bselectfields.
  #          Use '$widget configure -bvalues $lst' instead of 'SetLstValues'
  #
  # PARAMETERS:
  #   lst: List with new values
  #
  #///////////////////////////////////////////////////////////////////////////
    set blstvalues $lst
  }

  #///////////////////////////////////////////////////////////////////////////
  onconfigure -width {value} {
  #
  # PURPOSE: Change all entries' width when configure -width is called.
  #
  # PARAMETERS:
  #   value: Width at wich all entries will be adjusted.
  #
  #///////////////////////////////////////////////////////////////////////////
if {0} {
    #Change the code width:
    $self.f.eCode configure -width $value
    #Change all field's width:
    for {set x 1} {$x < [llength $blstfields]} {incr x} {
      $self.f.eCode configure -width $value
    }
}
  }

  #///////////////////////////////////////////////////////////////////////////
  method CreateWidgets {args} {
  #
  # PURPOSE: Create all widgets needed for the form.
  #  The widgets are created acording to the information given in blstfields:
  #
  #///////////////////////////////////////////////////////////////////////////
    set data(ftop) [frame $self.f]
    set blstentry ""
    #CODE
    set lab $blabel
    set auxObj [LabelEntry $self.f.eCode -editable false -label $lab \
          -foreground $bcolor -width 3 -helptext [lindex $blsthelptexts 0]]

    #Se podría hacer que cuando el arrowbutton esta activo, al pinchar 
    #sobre el entry del LabelEntry se muestre la lista también
    #bind $auxObj <Button-1> [list $self ShowList]

    lappend blstentry $auxObj
    grid $auxObj  -sticky nwe -row 0 -column 0 

    # ArrowButton
    set auxObj [ArrowButton $self.f.abCode -dir bottom \
      -helptext [mc "List of values"] -command [list $self ShowList]]
    grid $auxObj  -sticky nws -row 0 -column 1

    # Descriptions
    for {set x 1} {$x<[llength $blstfields]} {incr x} {
      if {$blabdesc} {
        set lab [mc [lindex $blstfields $x]]
      } else {
        set lab ""
      }
      set auxObj [LabelEntry $self.f.eField$x -editable false \
                    -label $lab -width 9 -helptext [lindex $blsthelptexts $x]]
      lappend blstentry $auxObj
      grid $auxObj  -sticky new -row 0 -column [expr $x + 1] 
    }
    
    grid $self.f -sticky news

    grid columnconfigure $self.f 0 -weight 1
    grid rowconfigure    $self.f 0 -weight 1
    
    for {set i 1} {$i<[llength $blstentry]} {incr i} {
      grid columnconfigure $self.f [expr $i + 1] -weight 3
    }

    # Toplevel desplegable
    set data(fbottom) [set f [toplevel $self.t -bg white -borderwidth 0\
            -relief solid]]
    #wm deiconify        $f
    wm resizable        $f 0 0
    wm overrideredirect $f 1
    wm withdraw         $f

    ScrolledWindow $f.sw
    set data(lb) [::mclistbox::mclistbox $f.sw.lb  \
         -background white -columnborderwidth 0\
         -font {Arial 8} -labelfont {Arial 8}]
    $f.sw setwidget $f.sw.lb
    set lv $data(lb)
    #bind $lv <Button-1>        +[list focus -force %W]
    bind $lv <KeyPress-Escape> +[list $self CloseList -1 -1]
    bind $lv <KeyPress-Return> +[list $self ListViewOnClick]
    # cabeceras de las columnas
    if {[llength $blsttitlecol]} {
      set lst $blsttitlecol
    } else {
      set lst $blstfields
    }
    set blstcolumns ""
    foreach nc $lst {
      # Ajustamos el mínimo ancho de cada columna:
      set len [string length $nc]
      $lv column add $nc -label $nc -width $len
      lappend blstcolumns $lv.frame$nc.listbox
    }
    # La ultima columna es la que se expande
    if [info exist nc] {
      $data(lb) configure -fillcolumn $nc
    }
    
    #Bind command to aply when double-clicking:
    bind $f <ButtonPress> [list $self CloseList %X %Y]

    bind $lv <Double-Button-1> +[list $self ListViewOnClick]

    grid $f.sw           -sticky news
    grid columnconfigure $f 0 -weight 1
    grid rowconfigure    $f 0 -weight 1

    #Configuramos el bselectfields
    grid columnconfigure $self 0 -weight 1
    grid rowconfigure    $self 0 -weight 1
  }


  #///////////////////////////////////////////////////////////////////////////
  method ShowList {args} {
  #
  # PURPOSE: Show the selection list in a new window.
  #
  # TODO: Leave creation of the widget to the Create method. This shoul only show it!
  #
  #///////////////////////////////////////////////////////////////////////////
    # ejecutamos comando previo a la visulizacion
    if {[string length $bpostcommand  ]} {
      eval $bpostcommand  
    }
    set lv $data(lb)
    focus $lv
    # cabeceras de las columnas
    if {[llength $blsttitlecol]} {
      set lst $blsttitlecol
    } else {
      set lst $blstfields
    }
    if {[winfo viewable $data(fbottom)] == 1} {
      wm withdraw  $data(fbottom)
      grab release $data(fbottom)
    } else {
      set vwidth  [winfo reqwidth  $data(fbottom)]
      set vheight [winfo reqheight $data(fbottom)]
      wm deiconify $data(fbottom)
      wm geometry $data(fbottom) \
        "${vwidth}x${vheight}+[winfo rootx $data(ftop)]+[expr \
        [winfo rooty $data(ftop)]+[winfo height $data(ftop)]]"
      focus -force $data(fbottom)
      grab -global $data(fbottom)
      bind $data(fbottom) <ButtonPress> [list $self CloseList %X %Y]
    }


    #Limpiamos el listbox:
    $lv delete 0 end
    #Cargamos los valores del listbox:
    foreach val $blstvalues {
      $lv insert end $val
      #Ajustamos el ancho de cada columna, si es necesario:
      set i 0
      foreach col $val {
        # Leemos el ancho del dato:
        set datalen [string length $col]
        #Leemos el ancho de la actual columna del listbox:
        set name [lindex $lst $i]
        set length [$lv.frame$name cget -width]
        #Comparamos el ancho del dato con el del listbox:
        set datalen [expr $datalen * 8]
        if {$datalen > $length} {
          #Adjust column width:
          $lv.frame$name configure -width $datalen
        }
        incr i
      }
    }
    # Marcamos la línea seleccionada, de existir
    set txt [$self.f.eCode cget -text]
    if {[string length $txt]} {
      set l [[lindex $blstcolumns 0] get 0 end]
      set pos [lsearch $l $txt]
      if {$pos != -1} {
        foreach obj $blstcolumns {
          $obj selection set $pos
        }
      }
    }


################################################################################

     # Posicionamos el dialogo:
     set tp  [expr [winfo pointery $self.f.eCode] + ([winfo height $self.f.eCode] / 2)]
     set lft [expr [winfo pointerx $self.f.eCode] - [winfo width  $self.f.eCode]]

     set width 0
     foreach name $lst {
       #   Ancho del texto.
       # + 2 * Ancho del borde.
       # + Ancho de la barra de scroll.
       # + Ancho de separador.
       set width [expr $width + [$lv.frame$name cget -width] \
                              + ( 2 * [$lv.frame$name cget -border] ) + 2 ]
     }
     #Si el ancho de la ventana no es suficiente para los botones:
     if {$width < 250} {
       #Añadimos el ancho extra repartido entre todas las columnas:
       set plus [expr (250 - $width) / [llength $lst]]
       foreach name $lst {
         set w [$lv.frame$name cget -width]
         $lv.frame$name configure -width [expr $w + $plus]
       }
       set width 250
     } elseif {$width > 800} {
       #If the window width is too much just cut out the wider column:
       set maxw 0
       foreach name $lst {
         set w [$lv.frame$name cget -width]
         if {$w > $maxw} {
           set maxw $w
           set wn $name
         }
       }
       $lv.frame$wn configure -width [expr $maxw - ($width - 800) ]
       set width 800
     }
     #20 * numero de datos + 100 (Botones y separador)
     set height [expr ([llength $blstvalues] * 20) + 100]
     if { $height > 300} {
       set height 300
     }

#     #wm geometry $dialog ${wd}x250+$lft+$tp
     wm geometry $data(fbottom) ${width}x$height+$lft+$tp
  }

  method CloseList {x y} {
    if {[expr $x==-1 && $y==-1]}  {
      wm withdraw  $data(fbottom)
      grab release $data(fbottom)
    } else {
      # Puede dar un error si la ventana no es de la aplicacion actual
      set nw [winfo containing $x $y]
      if {[winfo toplevel $nw] != $data(fbottom)} {
        wm withdraw  $data(fbottom)
        grab release $data(fbottom)
      }
    } 
  }

  #///////////////////////////////////////////////////////////////////////////
  method ListViewOnClick {args} {
  #
  # PURPOSE: Captura la fila seleccionada, captura cada elemento e introduce
  #          sus valores en los entrys
  #
  #///////////////////////////////////////////////////////////////////////////
    # capturamos los valores seleccionados
    set i 0
    foreach obj $blstentry {
      set lb [lindex $blstcolumns $i]
      set lst [$lb curselection]
      if {[llength $lst]} {
        set pos [$lb index active]
        $obj configure -text [$lb get $pos]
      }
      incr i
    }
    # ocultamos el dialogo
    #$self CloseList 0 0
    $self CloseList [winfo rootx $data(ftop)] [winfo rooty $data(ftop)]
    if {[string length $bmodifycmd]} {
      eval $bmodifycmd
    }
  }


  #///////////////////////////////////////////////////////////////////////////
  method SetLstValues {lst args} {
  #
  # PURPOSE: Especificamos los valores a mostrar en la lista de seleccion
  #          DEPRECATED. Se debe llamar a '$widget configure -bvalues $lst'
  #
  # PARAMETERS:
  #   lst -> lista de listas con los valores a mostrar
  #
  #///////////////////////////////////////////////////////////////////////////
    set blstvalues $lst
  }

  #///////////////////////////////////////////////////////////////////////////
  method GetValueList {args} {
  #
  # PURPOSE:
  #
  #///////////////////////////////////////////////////////////////////////////
    return $blstvalues
  }

  #///////////////////////////////////////////////////////////////////////////
  method GetValue {args} {
  #
  # PURPOSE: Obtenemos los valores seleccionados
  #
  # RETRUN: Una lista con los valores seleccionados
  #   
  #///////////////////////////////////////////////////////////////////////////
    set lst ""
    foreach obj $blstentry {
      lappend lst [$obj cget -text]
    }
    return $lst
  }

  #///////////////////////////////////////////////////////////////////////////
  method SetValue {lst args} {
  #
  # PURPOSE: Insertamos los valores en los entrys
  #
  # PARAMETERS: lst -> lista con los valores a incluir
  #   
  #///////////////////////////////////////////////////////////////////////////
    # limpiamos valores visualizados
    $self Clean
    # introducuimos valores
    set i 0
    foreach obj $blstentry {
      $obj configure -text [lindex $lst $i]
      incr i
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method Clean {args} {
  #
  # PURPOSE: Vaciamos el valor de todos los entrys
  #   
  #///////////////////////////////////////////////////////////////////////////
    # introducuimos valores
    foreach obj $blstentry {
      $obj configure -text ""
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method Configure {opc val args} {
  #
  # PURPOSE: Aplicamos una opcion a todos los entry 
  #
  # PARAMETERS:
  #   opc: opcion a aplicar
  #   val: valor asociado a la opcion
  #   
  #///////////////////////////////////////////////////////////////////////////
    # introducuimos valores
    foreach obj $blstentry {
      $obj configure -$opc $val
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method ConfigureDesc {opc val args} {
  #
  # PURPOSE: Aplicamos una opcion a todos los entry, menos al codigo (primero)
  #
  # PARAMETERS:
  #   opc: opcion a aplicar
  #   val: valor asociado a la opcion
  #   
  #///////////////////////////////////////////////////////////////////////////
    # introducuimos valores
    set lst [lrange $blstentry 1 end] 
    foreach obj $lst {
      $obj configure $opc $val
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method Activate {stt args} {
  #
  # PURPOSE: Aplicamos una opción a los elemetos del componente. Por un lado
  #          al entry del código, al botón y al resto de campos entry.
  #
  # PARAMETERS:
  #   stt: opción a aplicar
  #     normal    -> Code (yes) Button (no)  Desc (no) 
  #     active    -> Code (yes) Button (no)  Desc (yes) 
  #     select    -> Code (no)  Button (yes) Desc (no) 
  #     disabled  -> Code (no)  Button (no)  Desc (yes)    
  #     notactive -> Code (no)  Button (no)  Desc (no)    
  #   
  #///////////////////////////////////////////////////////////////////////////
    # introducimos valores
    switch $stt {
      "normal" {
        BfoActivateWidget   $self.f.eCode  $stt
        $self.f.eCode configure -editable  true
        BfoActivateWidget   $self.f.abCode "disabled"
        $self ConfigureDesc -state         disabled
        $self ConfigureDesc -editable      false
      }
      "active" {
        BfoActivateWidget   $self.f.eCode  "normal"
        $self.f.eCode configure -editable  true
        BfoActivateWidget   $self.f.abCode "disabled"
        $self ConfigureDesc -state normal
        $self ConfigureDesc -editable      true
      }
      "select" {
        BfoActivateWidget   $self.f.eCode  "disabled"
        $self.f.eCode configure -editable  false
        BfoActivateWidget   $self.f.abCode "active"
        $self ConfigureDesc -state         normal
        $self ConfigureDesc -editable      false
      }
      "disabled" {
        BfoActivateWidget   $self.f.eCode  "disabled"
        $self.f.eCode configure -editable  false
        BfoActivateWidget   $self.f.abCode "disabled"
        $self ConfigureDesc -state         normal
        $self ConfigureDesc -editable      true
      }
      "notactive" {
        BfoActivateWidget   $self.f.eCode  "disabled"
        $self.f.eCode configure -editable  false
        BfoActivateWidget   $self.f.abCode "disabled"
        $self ConfigureDesc -state         normal
        $self ConfigureDesc -editable      false
      }
      default {
        puts "BSelectFields Activate ERROR: invalid state: $stt"
      }
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method SetModifyCmd {modCmd args} {
  #
  # PURPOSE: Especifica un comando a aplicar al presionar el boton aceptar
  #          de la lista de seleccion
  #
  # PARAMETERS: modCmd -> sentencia a ejecutar
  #
  #///////////////////////////////////////////////////////////////////////////
    set bmodifycmd $modCmd
  }

  #///////////////////////////////////////////////////////////////////////////
  method GetCode {args} {
  #
  # PURPOSE: Devuelve el primer elemento de los seleccionados. Corresponde al
  #          codigo 
  #
  # RETURN: Valor del codigo
  #
  #///////////////////////////////////////////////////////////////////////////
    return [lindex [$self GetValue] 0]
  }

  #///////////////////////////////////////////////////////////////////////////
  method SelCode {code} {
  #
  # PURPOSE: Insertamos los valores en los entrys
  #
  # PARAMETERS: lst -> lista con los valores a incluir
  #   
  # Corrected (Bugs: Si no encuentra el code debe devolver un error (-1))
  #///////////////////////////////////////////////////////////////////////////
    #Recorremos la lista para encontrar el indice del code:
    set i 0
    set index -9999
    foreach {row} $blstvalues {
      if { [lindex $row 0] == $code } {
        set index $i
      }
      incr i
    }
    if {$index eq -9999} {
       return -1
    }
    #set index [lsearch -exact $blstvalues $code]
    # limpiamos valores visualizados
    $self Clean
    # introducimos valores
    set i 0
    foreach obj $blstentry {
      $obj configure -text [lindex [lindex $blstvalues $index] $i]
      incr i
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method SetValueView {lst args} {
  #
  # PURPOSE: Insertamos los valores en los entrys
  #
  # PARAMETERS: args -> lista con los valores a incluir, el primero es el del
  #             codigo
  #   
  #///////////////////////////////////////////////////////////////////////////
    # limpiamos valores visualizados
    $self Clean
    # introducuimos valores
    set i 0
    foreach obj $blstentry {
      set current [$obj cget -state]
      $obj configure -state normal
      $obj configure -text [lindex $lst $i] 
      $obj configure -state $current
      incr i
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method getwidget {} {
  #
  # PURPOSE: Obtenemos el nombre de todos los objetos creados
  #
  # PARAMETERS: lista con el path de los entrys creados
  #   
  #///////////////////////////////////////////////////////////////////////////
    return $blstentry
  }

}

package provide bselectfields 1.0
