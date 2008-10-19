#/////////////////////////////////////////////////////////////////////////////
# FILE    : btreeviewplus.tcl
# PURPOSE : Bayes Tree View. Tree View con un menu contextual y añadidos
#/////////////////////////////////////////////////////////////////////////////

package require byscommon
package require BLT
package require bfunctions
package require bfieldsdialog


#/////////////////////////////////////////////////////////////////////////////
::snit::widget btreeviewplus {
#
# PURPOSE: Treeview con menu contextual y caracteristicas adicionales
#
#/////////////////////////////////////////////////////////////////////////////
# opciones especificas
# readonly -> componente en modo de lectura o lectura/escritura
# keypress -> Activa la busqueda de filas mediante la pulsacion de teclas
# cols -> columnas del treeview no tiene en cuenta el "valor" principal
# hiddencols -> columnas del treeview que no se mostraran
# filtercols -> columnas del treeview que por las que puede realizarse filtrado
# initialdir -> directorio inicial para los dialogos de lec/esc de archivos,...
# orientation -> orientacion para el diálogo de añadir
# nomenu -> Si es 1, no se mostrará el menu contextual
# showtitles -> muestra o no los titulos de las columnas
# dragable -> Habilita o Deshabilita entorno Drag and Drop entre Elementos
#
# variables
# treeview -> el treeview de BLT sobre el que sobrecargamos funcionalidad
# menu -> el menu contextual del treeview
# xbar -> horizontal scrollbar
# ybar -> vertical scrollbar
# afterTime -> tiempo desde que se pulso la ultima tecla
# listKey -> lista de teclas pulsadas
#/////////////////////////////////////////////////////////////////////////////
  widgetclass Btreeviewplus

  option -readonly    0
  option -keypress    true
  option -colkeypress ""  
  option -cols {}
  option -hiddencols {}
  option -filtercols {}
  option -initialdir {}
  option -orientation vertical
  option -showtitles true
  option -nomenu 0
  #pedro - DragNDrop Boolean
  option -dragable 0

  variable _treeview
  variable _menu
  variable xbar
  variable ybar
  variable created 0
  
  variable afterTime 0
  variable listKey   {}

  delegate method * to _treeview
  delegate option * to _treeview

  #///////////////////////////////////////////////////////////////////////////
  constructor {args} {
  #
  # PURPOSE: Constructor de un btreeviewplus. Crea una instancia de un Treeview
  #
  # PARAMETERS:
  #   args -> opciones
  #
  #///////////////////////////////////////////////////////////////////////////

    if {0} {
      install _treeview using ::blt::treeview $self.tv \
          -activeicons "" -icons {} -selectmode multiple \
          -selectbackground grey -selectforeground white \
          -highlightthickness 0 -borderwidth 1 -selectborderwidth 1 \
          -nofocusselectbackground gray90 -font {Arial 9} -flat no
    } else {
      set _treeview [::blt::treeview $win.tv \
                         -activeicons "" -icons {} -selectmode multiple \
                         -selectbackground grey -selectforeground white \
                         -highlightthickness 0 -borderwidth 1 -selectborderwidth 1 \
                         -nofocusselectbackground gray90 -font {Arial 9} -flat no]
    }

    install _menu using menu $win.m -tearoff 0
    install xbar using scrollbar $win.sbh -orient horizontal \
                                           -command "$_treeview xview"
    install ybar using scrollbar $self.sbv -orient vertical \
                                           -command "$_treeview yview"

    $_treeview style  configure text     -bg white
    $_treeview column configure treeView -bg white

    $self configure -readonly [from args -readonly]
    $self _CreateMenu

    $_treeview column configure [list BLT TreeView $_treeview] -titleborderwidth 1
    
    # Apply all arguments given:
    $self configurelist $args
    $_treeview configure -xscrollcommand "$xbar set"
    $_treeview configure -yscrollcommand "$ybar set"
    
    # bind
    $_treeview bind all <KeyPress> +[list $self _OnKeyPress %A]
    
    $self configure -nomenu [from args -nomenu]
    $self configure -filtercols [from args -filtercols]
    
    # packing
    grid $_treeview -row 0 -column 0 -sticky news
    grid $ybar -row 0 -column 1 -sticky ns
    grid $xbar -row 1 -column 0 -sticky ew
    grid columnconfigure $self 0 -weight 1
    grid rowconfigure    $self 0 -weight 1

    ::autoscroll::autoscroll $ybar
    ::autoscroll::autoscroll $xbar
    set created 1
  }

  #///////////////////////////////////////////////////////////////////////////
  #
  # PURPOSE: Metodos cget y configure
  #
  #///////////////////////////////////////////////////////////////////////////

  onconfigure -cols { lst } {
   set options(-cols) $lst
     foreach col [$self column names] {
       $self column delete $col
     }
     foreach col $options(-cols) {
       $_treeview column insert end $col -justify left
       $_treeview column configure $col -command "$self _SortByColumn [list $col]" \
           -titleborderwidth 1
     }
  }

  onconfigure -hiddencols { lst } {
    set options(-hiddencols) $lst
    foreach col [$self column names] {
      if { "-1" eq [lsearch $options(-hiddencols) $col] } {
        $_treeview column configure $col -hide 0
      } else {
        $_treeview column configure $col -hide 1
      }
    }
  }

  onconfigure -showtitles { bool } {
    set options(-showtitles) $bool
    $_treeview configure -showtitles $bool
    $_treeview show
  }

  onconfigure -nomenu { bool } {
    if {$bool} {
      # no mostrar menu
      bind $_treeview  <Button-3>   ""      
    } else {
      # mostrar menu      
      bind $_treeview <Button-3>   "tk_popup $_menu %X %Y"
    }
  }

  onconfigure -readonly { bool } {
    if {!($created)} {
      set options(-readonly) $bool        
    } else {
      puts "Btreeviewplus widget error: option -readonly only can be set on\
            creation time"
    }
  }
  
  #pedro - DND
  onconfigure -dragable { bool } {
    if { $bool } {
      package require tkdnd
      #pedro - Create Drag n Drop Binds & Link dnd events.
      event add <<DragAttempt>> <Button1-Motion>
      $self bind all <<DragAttempt>>  "dnd drag $_treeview"
      dnd bindsource $_treeview text/plain "return 1"
      dnd bindtarget $_treeview text/plain <Drop> "dnd_event $_treeview %x %y"
    } else {
      event delete <<DragAttempt>>
    }
  }
  
  #pedro- end  
  
  onconfigure -filtercols { lst } {
    set cols [$self cget -cols]   
    set hiddencols [$self cget -hiddencols]
    if { [llength cols] && [llength hiddencols]} {
      set filtercols {}
      foreach col $cols {
        if {([lsearch -exact $hiddencols  $col])} {
          lappend filtercols $col
        }
      } 
      set options(-filtercols) $filtercols   
    } else {
      if { [llength cols] } {
        set options(-filtercols) $options(-cols)
      }
    }
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method _CreateMenu {args} {
  #
  # PURPOSE: Adds menu to treeview.
  #
  #///////////////////////////////////////////////////////////////////////////
    # read option
    $_menu add command -label [mc "Select all"] \
      -command "$self _SelectAll"  -accelerator CTRL+E
    $_menu add command -label [mc "Unselect all"] \
      -command "$self selection clearall"
    $_menu add command -label [mc "Toggle selection"] \
      -command "$self _ToggleSelection"
    # read/write option
    if {!([$self cget -readonly])} {
      # delete 
      $_menu add separator
      #$_menu add command -label [mc "Insert"] \
                -command "$self _OpenWR" -accelerator Ins
      $_menu add command -label [mc "Delete all"]  \
        -command "$self clear"
      $_menu add command -label [mc "Delete selected"] \
        -command "lbDeleteSelection $self"
    }
    # read option
    # copy
    $_menu add separator
    $_menu add command -label [mc "Filter"] -command "$self _OpenFD"
    $_menu add command -label [mc "Unfilter"] -command "$self show all"
    $_menu add separator
    set mc [menu $_menu.copy -tearoff 0]
    $_menu add cascade -label [mc "Copy"] -menu $mc
    $mc add command -label [mc "Copy all"] \
      -command "$self _CopyToClipBoard 1 1"
    $mc add command -label [mc "Copy selected"] \
      -command "$self _CopyToClipBoard 1 0" -accelerator CTRL+C
    $mc add command -label [mc "Copy unselect"] \
      -command "$self _CopyToClipBoard 0 1"
    # count
    $_menu add separator
    $_menu add command -label [mc "Count elements"] \
      -command "$self _ShowCountItems"
    
    # falta el menu archivo
    # file
    #$_menu add separator
    #set mf [menu $_menu.file -tearoff 0]
    #$_menu add cascade -label [mc "File"] -menu $mf
    #$mf add command -label "[mc "Save to file"] ..." \
      -command "$self _SaveToFile 1 1"
    #if {![$self cget -readonly]} {
    #  $mf add command -label "[mc "Load form file"] ..." \
        -command "$self _LoadFromFile"
    #}
    # bindings   
    $self bind all <Control-KeyRelease> +[list $self _OnControlKey $self %K %k]
    $self bind all <Key-Delete>    "lbDeleteSelection $self"
    $self bind all <Key-Insert>    "$self _OpenWR"    
    $self bind all <Shift-Up>      "$self addPrevious"
    $self bind all <Shift-Down>    "$self addNext"
    $self bind all <Up>            "$self previous"
    $self bind all <Down>          "$self next"
    $self bind all <Home>          "$self first"
    $self bind all <End>           "$self last"
    $self bind all <Shift-Home>    "$self addAllPrevious"
    $self bind all <Shift-End>     "$self addAllNext"
    $self bind all <Button-1>      +[list focus -force %W]
  }
  #///////////////////////////////////////////////////////////////////////////
  method getMenu { } {
  #
  # PURPOSE: Devuelve el menu del btreeviewplus para poder modificarlo y
  #          ampliarlo
  #
  # RETURN: La variable de instancia que apunta al menu
  #
  #///////////////////////////////////////////////////////////////////////////  
    return $_menu
 }


  #///////////////////////////////////////////////////////////////////////////
  method _OnKeyPress {key} {
  #
  # PURPOSE: Activa la fila que coincide con las teclas pulsadas. Distinge
  #          entre mayúculas y minúsculas
  #
  # PARAMETERS: key -> tecla pulsada
  #
  #///////////////////////////////////////////////////////////////////////////
    if {[$self cget -keypress]} {      
      set diference 500
      set result [lbOnKeyPress $self $key $listKey $afterTime $diference \
        [$self cget -colkeypress]]
      set listKey   [lindex $result 0]
      set afterTime [lindex $result 1]
    }
  }


  #///////////////////////////////////////////////////////////////////////////
  method _OnControlKey {_lb keysym keycode} {
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
      E { $_lb _SelectAll }
      C { lbToClipBoard $_lb 1 0 }
    }
  }  

  #///////////////////////////////////////////////////////////////////////////
  method _ShowCountItems {} {
  #
  # PURPOSE: Shows information on the selection of the elements
  #
  #///////////////////////////////////////////////////////////////////////////
    set info [lbCountItems $self]
    set msg "[mc "Total"]: [lindex $info 0] \n\
             [mc "Selected"]: [lindex $info 1] \n\
             [mc "Not Selected"]: [lindex $info 2]"
    tk_messageBox -default ok \
                  -icon info  \
                  -message $msg \
                  -type ok \
                  -parent $self \
                  -title [mc "numero de elementos"]
  }


  #/////////////////////////////////////////////////////////////////////////////
  method _ToggleSelection {} {
  #
  # PURPOSE: Toggle selection elements
  #
  #/////////////////////////////////////////////////////////////////////////////
    foreach i [$self get] {
      $self selection toggle $i
    }
  }


  #///////////////////////////////////////////////////////////////////////////
  method _SelectAll { } {
  #
  # PURPOSE: Selection all
  #
  #///////////////////////////////////////////////////////////////////////////
    foreach i [$self get] {
      $self selection set $i
    }
  }


  #/////////////////////////////////////////////////////////////////////////////
  method _CopyToClipBoard { sl unSe {pref ""} {suf ""} {sep "\n"}} {
  #
  # PURPOSE: copy the elements indicates in clipboard
  #
  # PARAMETERS:
  #   sl   -> Copy items selections
  #   unSe -> Copy items not selections
  #   pref -> Text to concatenate in the beginning. Default ""
  #   suf  -> Text to concatenate in the end. Default ""
  #   sep  -> Separating text of elements. Default "\n" 
  #
  #/////////////////////////////////////////////////////////////////////////////
    # items to copy
    set lst {}
    if { $sl } {
      foreach i [$self curselection] {
        lappend lst [$self get $i]
      }
    }
    if { $unSe } {
      foreach i [$self selection get unselected] {
        lappend lst [$self get $i]
      }
    }
    # copy
    set str [ListToStr $lst $sep $pref $suf]
    clipboard clear
    clipboard append $str
  }


  #/////////////////////////////////////////////////////////////////////////////
  method _SaveToFile {se unSe {pref ""} {suf ""} {sep "\n"}} {
  #
  # PURPOSE: Save the content from treeview in a file
  #
  # PARAMETERS: 
  #    sl   -> Copy items selections
  #   unSe -> Copy items not selections
  #   pref -> Text to concatenate in the beginning. Default ""
  #   suf  -> Text to concatenate in the end. Default ""
  #   sep  -> Separating text of elements. Default "\n" 
  #
  #/////////////////////////////////////////////////////////////////////////////
    # select file

    set filNam [tk_getSaveFile -parent $self -title [mc "File selector"] \
                               -initialdir $options(-initialdir)]
    if {$filNam != ""} {
      # open file to write
      if { ![ catch { set fd [open $filNam w] } msgerr] } {
        # text to save
	set lst {}
	if { $se } {
	  foreach i [$self selection get selected] {
	    lappend lst [$self get $i]
	  }
	}
	if { $unSe } {
	  foreach i [$self selection get unselected] {
	    lappend lst [$self get $i]
	  }
	}
	set str [ListToStr $lst $sep $pref $suf]
	# write
	puts -nonewline $fd $str
	close $fd
	return 1
      }
      error $msgerr
    }
  }



  #/////////////////////////////////////////////////////////////////////////////
  method  _LoadFromFile { } {
  #
  # PURPOSE: Load the content from treeview in a file
  #
  #/////////////////////////////////////////////////////////////////////////////
    # select file
    set filNam [tk_getOpenFile -parent $self -title [mc "File selector"] \
                               -initialdir $options(-initialdir)]
    if {$filNam != ""} {
      # open file to read
      if { ![ catch { set fd [open $filNam r] } msgerr] } {
         while { [gets $fd line ] >= 0} { 
           $self add $line
         } 
         close $fd      
         return 1
      }
      error $msgerr
    }
  }

  
  #///////////////////////////////////////////////////////////////////////////
  method _SortByColumn { col } {
  #
  # PURPOSE: Sorts the ListBox by the values of the given column
  #
  # PARAMETERS:
  #  col -> the column which the sort is performed
  #
  #///////////////////////////////////////////////////////////////////////////
    set lst [$self get]
    set len [$self size]
    for { set i [expr $len - 1] } { $i > 0 } { incr i -1 } {
      for { set j 0 } { $j < $i } { incr j } {
        set ida [lindex [$self get] $j]
        set idb [lindex [$self get] [expr $j + 1]]
        set a [$self get $ida $col]
	set b [$self get $idb $col]
	if { $a > $b } { $self move $ida after $idb }
      }
    }
  }


  #///////////////////////////////////////////////////////////////////////////
  proc _AddWR { elem father } {
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
  method _OpenWR {} {
  #
  # PURPOSE: Calls (and creates) an add dialog for insert by hand elements
  #
  #///////////////////////////////////////////////////////////////////////////
        
    destroy $self.dw
    set dialog  [bfieldsdialog $self.dw -title [mc "Insert element"] \
                -parent $self -orientation $options(-orientation) \
                -fields $options(-cols) -procedure [codename _AddWR] \
		-buttons { add exit }]
        
    $dialog draw
  }
  
  
  #///////////////////////////////////////////////////////////////////////////
  proc _Filter { elem father } {
  #
  # PURPOSE: Filters the treeview with the filter suministred
  #
  # PARAMETERS:
  #  elem -> the list with the filter fields
  #  father -> The dialog's window pathname
  #
  #///////////////////////////////////////////////////////////////////////////
    eval $father filter $elem
  }


  #//////////////////////////////////////////////////////////////////////////
  method _OpenFD {} {
  #
  # PURPOSE: Calls (and creates) a add dialog for getting a filter
  #
  #///////////////////////////////////////////////////////////////////////////
        
    destroy $self.fd
    set dialog  [bfieldsdialog $self.fd -title [mc "Filter elements"] \
                -parent $self -orientation $options(-orientation) \
                -fields $options(-filtercols) -procedure [codename _Filter] \
		-buttons { info apply accept cancel }] 
    $dialog configure -info \
"Los filtros admiten expresiones regulares:

  expresiones regulares básicas:
    - literales: caracteres simples
    - brackets:  conincide con cualquier caracter especificado dentro
       tambien pueden expecificarse rangos pe \[a-d\] equivale a \[abcd\]
       o indicar la no coincidencia colocando como primer caracter ^
       rangos predefinidos:
        \[:alnum:\] equivale a \[0-9a-zA-Z\]
        \[:alpha:\] equivale a \[a-zA-Z\]
        \[:digit:\] equivale a \[0-9\]
        \\w equivale a \[:alnum:\]
        \\W equivale a \[^:alnum:\]
        para incluir el literal - ponerlo al final del braquet 
        para incluir el literal ^ ponerlo en cualquier lugar menos el primero
    - punto . : coincide con cualquier caracter simple
    - ^ y $ : coinciden respectivamente con el principio o final de linea
    - caracteres especiales: los caracteres que tienen significado en las
      expresiones regulares pueden escribirse precediendolos con \\
    
  operadores sobre las expresiones:
    - <e1><e2>: concatenacion, la expresion resultante coincide si coinciden
         secuencialmente las expresiones
    - <e1>|<e2> : disyuncion, coincide si coincide alguna de las expresiones
    - <e>? : opcionalidad, coincide si la expresion anterior coincide 0 o 1 vez
    - <e>* : repeticion, coincide si la expresion anterior coincide 0 o mas veces
    - <e>+ : repeticion, coincide si la expresion anterior coincide 1 o mas veces
    - <e>{n} : repeticion, coincide si la expresion anterior coincide  n veces
    - <e>{n,} : repeticion, coincide si la exp anterior coincide n o mas veces
    - <e>{n,m} : repeticion, coincide si la exp anterior coincide por lo menos n
         y no mas de m veces

  precedencia de los operadores:
    - los operadores de repeticion tienen preferencia sobre la concatenacion
    - la concatenacion tiene preferencia sobre la disyuncion
    - las reglas pueden alterarse con el uso de parentesis
"
        
    $dialog draw
  }


  #///////////////////////////////////////////////////////////////////////////
  method selection { args } {
  #
  # PURPOSE: extends the original command to extend capabilities
  #
  # PARAMETERS:
  #   args: arguments
  #
  #///////////////////////////////////////////////////////////////////////////
    if { [lindex $args 0] eq "get" } {
     if { [lindex $args 1] eq "unselected" } {
       set result {}
       set selected [$self curselection]
       foreach i [$self get] {
         if { "-1" eq [lsearch $selected $i] } {
	   lappend result $i
	 }
       }
       return $result
     } else {
      return [eval $_treeview curselection]
     }
    }

    eval $_treeview selection $args
  }

   #///////////////////////////////////////////////////////////////////////////
  method insert { args } {
  #
  # PURPOSE: inserts an element into the treeview
  #
  # PARAMETERS:
  #   args: [-at node] position value list_data_values
  #         the list_data_values must be provided in order
  #   dragable: make the elements dragable between them.
  #
  #///////////////////////////////////////////////////////////////////////////   
    set index 0
    set father ""
    
    if { [lindex $args $index] eq "-at" } {
      incr index
      set father [lindex $args $index]
      incr index
    }

    set pos [lindex $args $index]
    incr index

    set value [lindex $args $index]
    incr index

    set dats [lrange $args $index end]
    if { [llength $dats] == 1 } { set dats [lindex $dats 0] }
    set datos {}
    set i 0

    foreach col $options(-cols) {
      lappend datos $col [lindex $dats $i]
      incr i
    }

    if { $father == "" } {
      if { $datos == {} } {
       set node [$_treeview insert $pos $value]
      } else {
       set node [$_treeview insert $pos $value -data $datos]
      }
    } else {
      if { $datos == {} } {
       set node [$_treeview insert -at $father $pos $value]
      } else {
       set node [$_treeview insert -at $father $pos $value -data $datos]
      }
    }

  }

  #///////////////////////////////////////////////////////////////////////////
  method delete { args } {
  #
  # PURPOSE: deletes elements from the treeview
  #
  # PARAMETERS:
  #   args: the tags or ids of the items to be deleted
  #
  #///////////////////////////////////////////////////////////////////////////
    # OJO un delete sobre un arbol vacio puede provocar un crash, es
    # un bug en BLT!!!
    eval $_treeview delete $args
    #if { "-1" eq [lsearch $args 0] } {
    #  eval $_treeview delete $args
    #} else {
    #  puts "Root node can't be deleted"
    #}
  }
  

  #///////////////////////////////////////////////////////////////////////////
  method get { {id ""} {cols ""}} {
  #
  # PURPOSE: retrieves data from an entry identified by id
  #
  # PARAMETERS:
  #  id: id of the node
  #  cols: name of the cols from which retrieve data
  #
  #///////////////////////////////////////////////////////////////////////////
    if { $id eq "" } { return [lrange [$self find] 1 end] }
  
    set result {}
    set data [$_treeview entry cget $id -data]
    
    if { "$cols" eq "" } {
      set cols [$self column names]
    }
    
    foreach col $cols {
      set inx [lsearch $data $col]
      if { "-1" ne $inx } {
        incr inx
        lappend result [lindex $data $inx]
      }
    }
    return $result
  }


  #///////////////////////////////////////////////////////////////////////////
  method getName { args } {
  #
  # PURPOSE: retrieves name from an entry identified by id or its parent name
  #
  # PARAMETERS:
  #  id: id of the node
  #
  #///////////////////////////////////////////////////////////////////////////
    set id [lindex $args 0]
    if { [llength $args] == 2 } { set id [lindex $args 1] }
    
    set result [$_treeview get -full $id]
    set index  [expr [llength $result] - 2]

    if { [lindex $args 0] eq "-parent" } {
      return [lindex $result $index]
    }

    if { [lindex $args 0] eq "-full" } {
      return $result
    }

    return [lindex [$_treeview get -full $id] end]
  }

 #///////////////////////////////////////////////////////////////////////////
  method getId { args } {
  #
  # PURPOSE: retrieves id from an entry by a name pattern
  #
  # PARAMETERS:
  #  name: name of the node
  #
  # RETURN: Id of the node
  #
  #///////////////////////////////////////////////////////////////////////////
    set name [lindex $args 0]
    if { [llength $args] == 2 } {
      set name [lindex $args 1]
      }    
    set result [$_treeview find -name $name]
    return $result
  }


  #///////////////////////////////////////////////////////////////////////////
  method set { id data {cols ""}} {
  #
  # PURPOSE: changes data for an entry identified by id
  #
  # PARAMETERS:
  #  id: id of the node
  #  data: data to insert
  #  cols: name of the cols for which we are providing data
  #
  #///////////////////////////////////////////////////////////////////////////
    if { "$cols" eq "" } {
      set cols [$self column names]
    }
    set datos {}
    set i 0
    foreach col $cols {
      lappend datos $col
      lappend datos [lindex $data $i]
      incr i
    }
    return [$_treeview entry configure $id -data $datos]
  }


  #///////////////////////////////////////////////////////////////////////////
  method first {} {
  #
  # PURPOSE: Selects and returns the first row of btreeviewplus
  #
  # RETURN: The index of first row
  # 
  #///////////////////////////////////////////////////////////////////////////
    set all [$self get]
    set selected [$self selection get]
    if {$selected != ""} {
      foreach item $selected {
        $self selection clear $item
      }
    }
    $self selection set [lindex $all 0]
    return [lindex $all 0]
  }
    
    
  #///////////////////////////////////////////////////////////////////////////
  method last {} {
  #
  # PURPOSE: Selects and returns the last row of btreeviewplus
  #
  # RETURN: The index of last row
  # 
  #///////////////////////////////////////////////////////////////////////////
    set all [$self get]
    set selected [$self selection get]
    if {$selected != ""} {
      foreach item $selected {
        $self selection clear $item
      }             
    }
    $self selection set [lindex $all [expr [llength $all] - 1]]
    return [lindex $all [expr [llength $all] - 1]]            
  }


  #///////////////////////////////////////////////////////////////////////////
  method addPrevious {} {
  #
  # PURPOSE: adds the row previous to the present selection
  #
  # RETURN: The index of the row previous
  #
  #///////////////////////////////////////////////////////////////////////////
    set all [$self get]
    set selected [$self selection get]
    if {$selected == ""} {
      return -1
    } else  {
      set firstSelected [lindex $selected 0]
      set first [lindex $all 0]
      if { $first == $firstSelected } {
        return [lindex $all 0]
      } else  {
        set allPosition [lsearch -exact $all $firstSelected]
        set firstSelected [lindex $all [expr $allPosition - 1]]
        #set firstSelected [expr $firstSelected - 1]
        $self selection set $firstSelected
        return $firstSelected
      }
    }
  }
  

  #///////////////////////////////////////////////////////////////////////////
  method addNext {} {
  #
  # PURPOSE: Adds the following row to the present selection
  #
  # RETURN: The index of the following row
  # 
  #///////////////////////////////////////////////////////////////////////////
    set all [$self get]
    set selected [$self selection get]
    if {$selected == ""} {
      return -1     
    } else  {
      set lastSelected [lindex $selected [expr [llength $selected] - 1] ]
      set last [lindex $all [expr [llength $all] - 1]]
      if { $last == $lastSelected } {
        return [lindex $all [expr [llength $all] - 1]]
      } else  {
        set allPosition [lsearch -exact $all $lastSelected]
        set lastSelected [lindex $all [expr $allPosition + 1]]
        $self selection set $lastSelected
        return $lastSelected
      } 
    }   
  }


  #///////////////////////////////////////////////////////////////////////////
  method previous {} {
  #
  # PURPOSE: selects the row previous to the present selection
  #
  # RETURN: The index of the row previous
  #
  #///////////////////////////////////////////////////////////////////////////
    set all [$self get]
    set selected [$self selection get]
    if {$selected == ""} {
      return -1
    } else  {
      foreach item $selected {
        $self selection clear $item
      }
      set firstSelected [lindex $selected 0]
      set first [lindex $all 0]
      if { $first == $firstSelected } {
        $self selection set $firstSelected
        return [lindex $all 0]
      } else  {
        set allPosition [lsearch -exact $all $firstSelected]
        set firstSelected [lindex $all [expr $allPosition - 1]]
        $self selection set $firstSelected
        return $firstSelected
      }
    }
  }
    

  #///////////////////////////////////////////////////////////////////////////
  method next {} {
  #
  # PURPOSE: selects the following row to the present selection
  #
  # RETURN: The index of the following row
  # 
  #///////////////////////////////////////////////////////////////////////////
    set all [$self get]
    set selected [$self selection get]
    if {$selected == ""} {
      return -1
    } else  {
      foreach item $selected {
        $self selection clear $item
      }
      set lastSelected [lindex $selected [expr [llength $selected] - 1] ]
      set last [lindex $all [expr [llength $all] - 1]]
      if { $last == $lastSelected } {
        $self selection set $lastSelected
        return [lindex $all [expr [llength $all] - 1]]
      } else  {
        set allPosition [lsearch -exact $all $lastSelected]
        set lastSelected [lindex $all [expr $allPosition + 1]]
        $self selection set $lastSelected
        return $lastSelected
      }
    }
  }
    

  #///////////////////////////////////////////////////////////////////////////
  method addAllPrevious {} {
  #
  # PURPOSE: Selects all the previous rows
  #
  # RETURN: The index of the first row
  #
  #///////////////////////////////////////////////////////////////////////////
    set all [$self get]
    set selected [$self selection get]
    if {$selected == ""} {
      return -1
    } else  {
      set firstSelected [lindex $selected 0]
      set first [lindex $all 0]
      set allPosition [lsearch -exact $all $firstSelected]
      while { $allPosition != 0 } {
        $self addPrevious
        #$self selection set $firstSelected
        set selected [$self selection get]
        set allPosition [lsearch -exact $all $firstSelected]
        if { $allPosition != 0 } {
          set firstSelected [lindex $all [expr $allPosition - 1]]
        }
      }  
      return [lindex $firstSelected]
    }
  }
    

  #///////////////////////////////////////////////////////////////////////////
  method addAllNext {} {
  #
  # PURPOSE: Selects all the following rows
  #
  # RETURN: The index of the last row
  #
  #///////////////////////////////////////////////////////////////////////////
    set all [$self get]
    set selected [$self selection get]
    if {$selected == ""} {
      return -1
    } else  {
      set lastSelected [lindex $selected [expr [llength $selected] - 1] ]
      set last [lindex $all [expr [llength $all] - 1]]  
      set allPosition [lsearch -exact $all $lastSelected]
      while {$allPosition != [expr [llength $all] -1]} {
        $self addNext
        set selected [$self selection get]
        set lastSelected [lindex $selected [expr [llength $selected] - 1] ]
        set allPosition [lsearch -exact $all $lastSelected]
      }
      return $lastSelected
    }
  }
    

  #///////////////////////////////////////////////////////////////////////////
  method clear {} {
  #
  # PURPOSE: deletes all nodes
  #
  #///////////////////////////////////////////////////////////////////////////
    #foreach i [$self get] {
    #  if { 0 eq $i } { continue }
    #  $self delete $i
    #}
    $self delete 0
  }


  #///////////////////////////////////////////////////////////////////////////
  method size {} {
  #
  # PURPOSE: calculates the treeview size
  #
  #///////////////////////////////////////////////////////////////////////////
    return [llength [$self get]]
  }


  #///////////////////////////////////////////////////////////////////////////
  method column { args } {
  #
  # PURPOSE: modifies a particular case of column -> column names
  #
  #///////////////////////////////////////////////////////////////////////////
    if { [lindex $args 0] eq "names" } {
      return [lrange [$_treeview column names] 1 end]
    }
    return [eval $_treeview column $args]
  }
  

  #///////////////////////////////////////////////////////////////////////////
  method filter { filters } {
  #
  # PURPOSE: filters the treeview elements matching filters for each col in cols
  #
  # PARAMETERS:
  #  filters: list of filters for the columns above
  #
  #///////////////////////////////////////////////////////////////////////////
    foreach inx [$self get] {
      set datos [$self get $inx $options(-filtercols)]
      set match 1
      for { set i 0 } { $i < [llength $filters] } { incr i } {
        set match [expr $match & [regexp [lindex $filters $i] [lindex $datos $i]]]
      }
      if { ! $match } {
        $_treeview hide $inx
      } else {
        $_treeview show $inx
      }
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method update { } {
  #
  # PURPOSE: method to solve bug when a expanded node of tree
  #          is collapsed and back to expand it.
  #          This bug happens when the option -opencommand is used in an
  #          entry of BLT treeview.
  #
  #///////////////////////////////////////////////////////////////////////////
    $_treeview configure -width [expr [$_treeview cget -width] - 1]
    $_treeview configure -width [expr [$_treeview cget -width] + 1]
    update
  }

}

#pedro g - DND event handler
#///////////////////////////////////////////////////////////////////////////////
proc dnd_event { w x y} {
# PURPOSE: Handles the Drop Event.
# ARGS:    w = Widget window path,
#          x, y = Mouse Coords over Drop-Target, this are used to get the parent
#
# Ojo:     El handler no puede realizar desplazamientos hacia el nodo
#          raiz del arbol por una limitacion del BLT 2.4.
#          Los nodos que se quieran mover al root deben ser recreados directamente
#          al root a partir de la cola de hijos del nodo a mover.
# Warning: Handler won't move nodes towards the Node 0 of the tree because
#          of a BLT restriction. If a node was to be re-parented to the
#          Tree Root node, it'd have to be recreated by gathering a recursive
#          Child list from the node and then doing an insert with its elements
#          directly into the Tree Root.
# TODOs:   Pendiente hacer override a evento B1 para que mantenga la selección 
#          antes de iniciar DND.
#          El bindtarget se aplica a toda el area editable del widget, por tanto
#          aceptará información drop de cualquier fuente, hay que validar 
#          que el numero obtenido exista dentro de los elementos del tree.
#          De lo contrario da error.
#///////////////////////////////////////////////////////////////////////////////

# Obtenemos Origen y Destino desde los elementos seleccionados y coordenadas del
# Drop. 
  set newHijo   [$w curselection]
  set newParent [$w nearest $x $y]
  #Prohibimos Drop Hacia si mismo.
  if { $newHijo != $newParent } {
   # Añadimos soporte para drag&Drop cuando existe mas de un elemento seleccionado.
   foreach element $newHijo {
     # Obtenemos nombre de nuevo Padre e Hijo.
     set name  [$w get -full $element]
     set nameP [$w get -full $newParent]
     # Pop-up, pregunta de "seguridad"...(..es un coñazo..remover cuanto antes)
     set answer [tk_messageBox -message "Mover el Elemento '$name' A '$nameP'" \
                 -type yesno -icon question -title "Bys TreeView - Pregunta"]
     if { $answer == "yes"} {
       # Evitamos liarla con la limitacion del BLT, pues devuelve "exit 1"
       if { $newParent != "0" } {
         if { [catch "$w move $element into $newParent" fid] } {
           # Mostramos mensaje de error en caso de que ocurra alguno. 
	   tk_messageBox -message "Error: !! $fid !!" -icon "error" -title "Bys TreeView - Error"
	   return ""
          }
         return ""
        } else {
         #Mensaje en caso de usuario Animal.
         tk_messageBox -message "Opcion no permitida !! No puede desplazar a la raiz del Arbol"\
                    -title "Bys TreeView - Error" -icon "error"
         return ""
        }
      } else {
        return ""
      }
    }
  } 
}
#pedro - end

package provide btreeviewplus 1.0

# ------- TEST CODE ----

#/////////////////////////////////////////////////////////////////////////////
proc test_btreeviewplus {} {
#
# PURPOSE: Funcion de test de un btreeviewplus
#
#/////////////////////////////////////////////////////////////////////////////
  global tcl_platform

  if { $tcl_platform(platform) != "unix" } {
    SendInit
    SendVerify
  }
  
  # defining the widgets
  variable data
  set data(-selected) 1
  
  destroy .top
  toplevel .top -width 200 -height 100
    
  TitleFrame .top.f1 -text "BTreeViewPlus test"
  set f1 [ .top.f1 getframe ]

  #Añadida la opcion Dragable (boolean 1 o 0), activa o desactiva el DND.
  btreeviewplus $f1.blb -cols { col1 col2 col3 col4 } \
                    -height 150 -width 500 -orientation vertical \
                    -hiddencols {col4} -dragable 1
  puts "WIDGET btreeviewplus: $f1.blb"
  $f1.blb insert end numeros {1 2 3 4}
  $f1.blb insert end alpha   {one two three four}
  $f1.blb insert -at 2 end alfa    {uno dos tres cuatro}
  $f1.blb insert end kaka    {k kk kkk kkkk}
  $f1.blb insert -at 1 end zero    {0 0 0 0}
  $f1.blb insert end simbols {@ # $ %}

  # packing all   
  grid .top.f1 -row 0 -column 0 -sticky "new" -padx 10 -pady 10
  #grid .top.f2 -row 1 -column 0 -sticky "new" -padx 10 -pady 10
  grid columnconfigure .top 0 -weight 1
  grid rowconfigure .top 0 -weight 0
  grid rowconfigure .top 1 -weight 1

  grid $f1.blb -row 0 -column 0 -sticky "new"
  grid columnconfigure $f1 0 -weight 1
  grid rowconfigure $f1 0 -weight 1

}