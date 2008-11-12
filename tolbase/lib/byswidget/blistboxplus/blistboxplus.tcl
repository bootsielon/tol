#/////////////////////////////////////////////////////////////////////////////
# FILE    : blistboxplus.tcl
# PURPOSE : Bayes List Box. ListBox con opciones sobre su elementos y varias
#           columnas
#/////////////////////////////////////////////////////////////////////////////

package require byscommon
package require BLT
package require bfunctions
package require bfieldsdialog

proc debug_init { } {
  catch {
    file delete "/tmp/debug_tcl.out"
  }
}

proc debug_msg { msg } {
  catch {
    set fd [open "/tmp/debug_tcl.out" "a"]
    puts $fd $msg
    close $fd
  }
}

#/////////////////////////////////////////////////////////////////////////////
::snit::widget blistboxplus {
#
# PURPOSE: ListBox con columnas, menu contextual y caracteristicas adicionales
#
#/////////////////////////////////////////////////////////////////////////////
# opciones especificas
# readonly -> componente en modo de lectura o lectura/escritura
# keypress -> Activa la busqueda de filas mediante la pulsacion de teclas
# colkeypress -> Indica por que columna se busca al pulsar una tecla
# parent -> indica el padre donde se ubica el blistbox
# cols -> columnas del listbox
# hiddencols -> columnas del listbox que no se mostraran
# filtercols -> columnas del listbox que por las que puede realizarse filtrado
# initialdir -> directorio inicial para los dialogos de lec/esc de archivos,...
# orientation -> orientation for de add dialog
# icon -> icono que se mostrará como primera columna, si no hay icono la
#  columna no se mostrará
# nomenu -> Si es 1, no se mostrará el menu contextual
# titles -> Lista con los titulos a mostrar en cada una de las columnas.
# values -> List of initial values to load in widget
# valuesinsert -> List of values to selection element
# iniconfig -> Nombre que se asigna al listboxplus para identificar sus
#           opciones en el archivo de configuración .ini por defecto "" en
#           ese caso no se guardaran las opciones
#
# variables
# listbox -> el treeview que hace de listbox propiamente
# menu -> el menu contextual del listbox
# xbar -> horizontal scrollbar
# ybar -> vertical scrollbar
# afterTime -> tiempo desde que se pulso la ultima tecla
# listKey -> lista de teclas pulsadas
#/////////////////////////////////////////////////////////////////////////////
  widgetclass Blistboxplus

  option -readonly  0
  option -keypress    true
  option -colkeypress ""   
  option -parent    {}
  option -cols {}
  option -hiddencols {}
  option -filtercols {}
  option -initialdir {}
  option -orientation vertical
  option -showtitles true
  option -icon ""
  option -nomenu 0
  option -titles {}
  option -values {}
  option -valuesinsert {}
  option -iniconfig ""
  
  variable _listbox
  variable menu
  variable xbar
  variable ybar
  
  variable afterTime 0
  variable listKey   ""
  variable currentId 0
  variable created   0

  #///////////////////////////////////////////////////////////////////////////
  constructor {args} {
  #
  # PURPOSE: Constructor de un blistboxplus. Crea una instancia de un Listbox
  #
  # PARAMETERS:
  #   args -> opciones
  #
  #///////////////////////////////////////////////////////////////////////////

    debug_init

    debug_msg "install listbox ..."

    if {0} {
      install _listbox using ::blt::treeview $win.lb -hideroot yes \
          -activeicons "" -icons {} -selectmode multiple \
          -selectbackground grey -selectforeground white \
          -highlightthickness 0 -borderwidth 1 -selectborderwidth 1 \
          -nofocusselectbackground grey -font {Arial 9} -flat yes
    } else {
      set _listbox [::blt::treeview $win.lb -hideroot yes \
                        -activeicons "" -icons {} -selectmode multiple \
                        -selectbackground grey -selectforeground white \
                        -highlightthickness 0 -borderwidth 1 \
                        -selectborderwidth 1 \
                        -nofocusselectbackground grey -font {Arial 9} -flat yes]
    }

    debug_msg "despues de install listbox"

    install menu using menu $win.m -tearoff 0
    install xbar using scrollbar $win.sbh -orient horizontal \
                                           -command "$win xview"
    install ybar using scrollbar $win.sbv -orient vertical \
                                           -command "$win yview"

    #bindtags is modified to make possible to associate binds to self
    bindtags $_listbox [linsert [bindtags $_listbox] 2 $win]

    $_listbox style  configure text     -bg white

    $_listbox column configure treeView -bg white

    $self configure -readonly [from args -readonly]
    $self _CreateMenu  
    
    # Apply all arguments given:
    $self configurelist $args

    $_listbox configure -xscrollcommand "$xbar set"
    $_listbox configure -yscrollcommand "$ybar set"
    if { $options(-parent) eq "" } {
      $self configure -parent [winfo parent $win]
    }
    
    # bind
    $_listbox bind all <KeyPress> +[list $self _OnKeyPress %A]
    
    $self configure -nomenu [from args -nomenu]
    $self configure -filtercols [from args -filtercols]

    #pack
    grid $_listbox -row 0 -column 0 -sticky news
    grid $ybar -row 0 -column 1 -sticky ns
    grid $xbar -row 1 -column 0 -sticky ew
    grid columnconfigure $self 0 -weight 1
    grid rowconfigure    $self 0 -weight 1

    ::autoscroll::autoscroll $ybar
    ::autoscroll::autoscroll $xbar
    debug_msg "voy a pillarte: antes de _ReadIni"
    $self _ReadIni 
    debug_msg "pase?"
    set created 1
  }

  #///////////////////////////////////////////////////////////////////////////
  destructor {
  #
  # PURPOSE: Funciones a evaluar al eliminar la instancia de blistboxplus. 
  #
  # PARAMETERS:
  #   self: toplevel
  #
  #///////////////////////////////////////////////////////////////////////////
    # Save .ini
    catch [list $self _WriteIni ]
  }
    
  #///////////////////////////////////////////////////////////////////////////
  #
  # PURPOSE: Metodos cget y configure
  #
  #///////////////////////////////////////////////////////////////////////////

    onconfigure -cols { lst } {
     set options(-cols) $lst
       foreach col [$self column names] {
         $_listbox column delete $col
       }
       foreach col $options(-cols) {
         $_listbox column insert end $col -justify left
	 $_listbox column configure $col -command "$self _SortByColumn [list $col]" \
	    -titleborderwidth 1
       }
       if { $options(-icon) == "" } {
         $_listbox column configure "BLT TreeView $_listbox" -hide 1
       } else {
         $_lisbox column configure "BLT TreeView $_listbox" -hide 0
       }
  }

  onconfigure -hiddencols { lst } {
    set options(-hiddencols) $lst
    foreach col [$self column names] {
      if { "-1" eq [lsearch $options(-hiddencols) $col] } {
        $_listbox column configure $col -hide 0
      } else {
        $_listbox column configure $col -hide 1
      }
    }
  }

  onconfigure -showtitles { bool } {
    set options(-showtitles) $bool
    $_listbox configure -showtitles $bool
    $_listbox show
  }

  onconfigure -icon { icon } {
    set options(-icon) $icon
    if { $icon != "" } {
      $_listbox column configure "BLT TreeView $_listbox" -min 16 -max 16 \
        -hide 0 -titleborderwidth 1
      $_listbox configure -icons [list [Bitmap::get $icon] [Bitmap::get $icon]]
    } else {
      $_listbox column configure "BLT TreeView $_listbox" -hide 1
    }
  }

  onconfigure -titles { titles } {
    set options(-titles) $titles
    set c 0
    set cols [$self column names]
    foreach col $cols {
     $self column configure $col -title [mc [lindex $titles $c] ]
     incr c 1     
    } 
  }
  
  onconfigure -values { lst } {
    $self clear
    eval $self insert end $lst
  }

  onconfigure -nomenu { bool } {
    set options(-nomenu) $bool
    if {$bool} {
      # no mostrar menu
      bind $self  <Button-3>   ""
    } else {
      # mostrar menu
      bind $self  <Button-3>   "tk_popup $menu %X %Y"
    }
  }
  
  onconfigure -readonly { bool } {
    if {!($created)} {
      set options(-readonly) $bool        
    } else {
      puts "Blistboxplus widget error: option -readonly only can be set on \
            creation time"
    }
  }
  
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
  # PURPOSE: Append menu to listbox. 
  #
  #///////////////////////////////////////////////////////////////////////////
    # read option
    $menu add command -label [mc "Select all"] \
      -command "$self _SelectAll"  -accelerator CTRL+E
    $menu add command -label [mc "Unselect all"] \
      -command "$self selection clearall" -accelerator CTRL+U
    $menu add command -label [mc "Toggle selection"] \
      -command "$self _ToggleSelection" -accelerator CTRL+I
    # read/write option
    if {!([$self cget -readonly])} {
      # delete 
      $menu add separator
      $menu add command -label [mc "Insert"] \
                -command "$self _OpenWR" -accelerator Ins
      $menu add command -label [mc "Delete all"]  \
        -command "$self clear"  -accelerator CTRL+D
      $menu add command -label [mc "Delete selected"] \
        -command "lbDeleteSelection $self"
    }
    # read option
    # copy
    $menu add separator
    $menu add command -label [mc "Filter"] -command "$self _OpenFD"
    $menu add command -label [mc "Unfilter"] -command "$self _RemoveFilter"
    $menu add separator
    set mc [menu $menu.copy -tearoff 0]
    $menu add cascade -label [mc "Copy"] -menu $mc
    $mc add command -label [mc "Copy all"] \
      -command "$self _CopyToClipBoard 1 1" -accelerator CTRL+A
    $mc add command -label [mc "Copy selected"] \
      -command "$self _CopyToClipBoard 1 0" -accelerator CTRL+C
    $mc add command -label [mc "Copy unselect"] \
      -command "$self _CopyToClipBoard 0 1"
    # count
    $menu add separator
    $menu add command -label [mc "Count elements"] \
      -command "$self _ShowCountItems" -accelerator CTRL+N
    # file
    $menu add separator
    set mf [menu $menu.file -tearoff 0]
    $menu add cascade -label [mc "File_lbp"] -menu $mf
    $mf add command -label "[mc "Save to file"] ..." \
      -command "$self _SaveToFile 1 1"
    if {![$self cget -readonly]} {
      $mf add command -label "[mc "Load form file"] ..." \
        -command "$self _LoadFromFile"
    }
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
  # PURPOSE: Devuelve el menu del blistboxplus para poder modificarlo y
  #          ampliarlo
  #
  # RETURN: La variable de instancia que apunta al menu
  #
  #///////////////////////////////////////////////////////////////////////////  
    return $menu
  }


  #///////////////////////////////////////////////////////////////////////////
  method _OnKeyPress {key} {
  #
  # PURPOSE: Activa la fila que coincide con las teclas pulsadas. Distinge
  #          entre muysuculas y minusculas
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
      
      A { $self _CopyToClipBoard 1 1 }
      D { $self clear }
      I { $self _ToggleSelection } 
      U { $self selection clearall }
      N { $self _ShowCountItems }
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
    if {[string length [$self cget -parent]]} {
      set par $self
    } else {
      set par [$self cget -parent] 
    }
    tk_messageBox -default ok \
                  -icon info  \
                  -message $msg \
                  -type ok \
                  -parent $par \
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
  # PURPOSE: Save the content from listbox in a file
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
      set options(-initialdir) [file dirname $filNam]    
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
  # PURPOSE: Load the content from listbox in a file
  #
  #/////////////////////////////////////////////////////////////////////////////
    # select file    
    set filNam [tk_getOpenFile -parent $self -title [mc "File selector"] \
                               -initialdir $options(-initialdir)]    

    if {$filNam != ""} {
      set options(-initialdir) [file dirname $filNam]
      # open file to read
      if { ![ catch { set fd [open $filNam r] } msgerr] } {
         while { [gets $fd line ] >= 0} {
		   Tolcon_Trace "adding $line"
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
  # PARAMETERS:
  #  col -> the column which the sort is performed
  #
  #///////////////////////////////////////////////////////////////////////////

	set numofel [$self size]
	set lst [list]
	set fulllst [$self get]

	foreach i $fulllst {
	   lappend lst [$self get $i ]
    }
# lst:    contains a list of values
	set coln 0
	set cols [$self cget -cols]
	set hidcols [$self cget -hiddencols]
	for { set j 0 } { $j < [llength $cols] } { incr j } {
		if {[lindex $cols $j] eq $col} {
			set coln $j
		}
	}
# coln:   contains the index of the column to be ordered
	set lst [lsort -index $coln $lst]
	$self clear
    for { set i 0 } { $i < $numofel } { incr i } {
    	set el [lindex $lst $i ]
	   	$self insert end $el
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
    foreach ele $elem {
      $father insert 0 $ele
    }
  }


  #//////////////////////////////////////////////////////////////////////////
  method _OpenWR {} {
  #
  # PURPOSE: Calls (and creates) an add dialog for insert by hand elements
  #
  #///////////////////////////////////////////////////////////////////////////
    destroy $self.dw
    
    if {![catch {eval $options(-valuesinsert)} res]} {
      set valuesinsert $res
    } else {
      set valuesinsert $options(-valuesinsert)
    }
    
    if {$valuesinsert == ""} {
      set dialog  [bfieldsdialog $self.dw -title [mc "Insert element"] \
                  -parent $self -orientation $options(-orientation) \
                  -fields $options(-cols) -procedure [codename _AddWR] \
                  -buttons { add exit } -titles $options(-titles) \
                  -list $valuesinsert]
    } else {        
      set dialog  [bfieldsdialog $self.dw -title [mc "Insert element"] \
                  -parent $self -orientation $options(-orientation) \
                  -fields $options(-cols) -procedure [codename _AddWR] \
                  -buttons { add addall exit } -titles $options(-titles) \
                  -list $valuesinsert]
    }
    $dialog draw
  }
  
  
  #///////////////////////////////////////////////////////////////////////////
  proc _Filter { elem father } {
  #
  # PURPOSE: Filters the listbox with the filter suministred
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
		-buttons { info apply accept cancel } \
                -titles $options(-titles)] 
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
  method _WriteIni { } {
  #
  # PURPOSE: Guarda en el .ini informacion referente al blistboxplus
  #
  #
  #///////////////////////////////////////////////////////////////////////////
    if { [$self cget -iniconfig] ne "" } {
      # comprobamos cargado namespace y variable de .ini
      if {[namespace exists ::iniFile]} {
        set wini ::iniFile::Write
        set nm [$self cget -iniconfig]
        if {![string length $nm]} {
          set nm $self
        }
        $wini $nm initialdir $options(-initialdir)
      }
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method _ReadIni { } {
  #
  # PURPOSE: Recupera del .ini informacion referente al blistboxplus
  #
  #
  #///////////////////////////////////////////////////////////////////////////
    if { [$self cget -iniconfig] ne "" } {
      # comprobamos cargado namespace y variable de .ini
      if {[namespace exists ::iniFile]} {
        set rini ::iniFile::Read
        set nm [$self cget -iniconfig]
        if {![string length $nm]} {
          set nm $self
        }
        set stt [::iniFile::ReadWMState $rini $nm]
        set inid [$rini $nm initialdir ""]        
        if {[string length $inid]} {
            set options(-initialdir) $inid
        }
      }
    }
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
      return [eval $_listbox curselection]
     }
    }

    eval $_listbox selection $args
  }


  #///////////////////////////////////////////////////////////////////////////
  method insert { pos args } {
  #
  # PURPOSE: inserts an element into de listbox
  #
  # PARAMETERS:
  #   args: a list of elements to insert, each element is a list with column
  #   data
  #
  #///////////////////////////////////////////////////////////////////////////   
    if { [lindex $args 0] eq "cols" } {
      set cols [lindex $args 1]
      set args [lrange $args 2 end]
    } else {
      set cols $options(-cols)
    }
    
    switch [llength $cols] {
      1 {
        # mas de una columna
        foreach dats $args {
          set datos [list [lindex $cols 0] $dats]
          $_listbox insert $pos $currentId -data $datos
          incr currentId
        }        
      }
      default {
        # mas de una columna
        foreach dats $args {
          set i 0
          set datos {}
          foreach col $cols {
            lappend datos $col [lindex $dats $i]
	    incr i
          }
          $_listbox insert $pos $currentId -data $datos
          incr currentId
        }
      }
    }
  }


  #///////////////////////////////////////////////////////////////////////////
  method add { args } {
  #
  # PURPOSE: appends elements to dthe listbox
  #
  # PARAMETERS:
  #   args: a list of elements to insert, each element is a list with column
  #   data
  #
  #///////////////////////////////////////////////////////////////////////////
    eval $self insert end $args
  }
  

  #///////////////////////////////////////////////////////////////////////////
  method delete { args } {
  #
  # PURPOSE: deletes elements from the listbox
  #
  # PARAMETERS:
  #   args: the tags or ids of the items to be deleted
  #   data
  #
  #///////////////////////////////////////////////////////////////////////////
    if { "-1" eq [lsearch $args 0] } {
      eval $_listbox delete $args
    } else {
      puts "Root node can't be deleted"
    }
  }
  
  
  #///////////////////////////////////////////////////////////////////////////  
  method getlb {} {
  #
  # PURPOSE: Get path of blistboxplus
  #
  #///////////////////////////////////////////////////////////////////////////  
    return $_listbox
  }


  #///////////////////////////////////////////////////////////////////////////
  method get { {ids ""} {cols ""}} {
  #
  # PURPOSE: retrieves data from an entry identified by id
  #
  # PARAMETERS:
  #  id: id list of nodes
  #  cols: name of the cols from which retrieve data
  #
  #///////////////////////////////////////////////////////////////////////////
    if { $ids eq "" } { return [lrange [$self find] 1 end] }
  
    set result {}
	  
	if { "$cols" eq "" } {
	  set cols [$self column names]
	} 

    foreach id $ids {
    
      set elem {}
    
      set data [$_listbox entry cget $id -data]    
    
      foreach col $cols {
        set inx [lsearch $data $col]
        if { "-1" ne $inx } {
          incr inx
          lappend elem [lindex $data $inx]
        }
      }
      
      lappend result $elem
    }
    if { [llength $ids] eq 1 } { return [lindex $result 0] }
    return $result
  }


  #///////////////////////////////////////////////////////////////////////////
  method set { id data {cols ""}} {
  #
  # PURPOSE: inserts data to an entry identified by id
  #
  # PARAMETERS:
  #  id: id of the node
  #  data: data to insert
  #  cols: name of the cols from which retrieve data
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
    return [$_listbox entry configure $id -data $datos]
  }


  #///////////////////////////////////////////////////////////////////////////
  method first {} {
  #
  # PURPOSE: Selects and returns the first row of blistboxplus
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
    if { !([llength [$self get]] eq 0)} {
      $self selection set [lindex $all 0]
    }
    return [lindex $all 0]
  }
    
    
  #///////////////////////////////////////////////////////////////////////////
  method last {} {
  #
  # PURPOSE: Selects and returns the last row of blistboxplus
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
    if { !([llength [$self get]] eq 0)} {
      $self selection set [lindex $all [expr [llength $all] - 1]]
    }
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
    foreach i [$self get] {
      if { 0 eq $i } { continue }
      $self delete $i
    }
  }


  #///////////////////////////////////////////////////////////////////////////
  method size {} {
  #
  # PURPOSE: calculates the listbox size
  #
  #///////////////////////////////////////////////////////////////////////////
    return [$_listbox entry size root]
  }


  #///////////////////////////////////////////////////////////////////////////
  method column { args } {
  #
  # PURPOSE: modifies a particular case of column -> column names
  #
  #///////////////////////////////////////////////////////////////////////////
    if { [lindex $args 0] eq "names" } {
      return [lrange [$_listbox column names] 1 end]
    }
    return [eval $_listbox column $args]
  }
  

  #///////////////////////////////////////////////////////////////////////////
  method filter { filters } {
  #
  # PURPOSE: filters the listbox elements matching filters for each col in cols
  #
  # PARAMETERS:
  #  filters: list of filters for the columns above
  #
  #///////////////////////////////////////////////////////////////////////////
    $self configure -flat no
    foreach inx [$self get] {
      set datos [$self get $inx $options(-filtercols)]
      set match 1
      for { set i 0 } { $i < [llength $filters] } { incr i } {
        set match [expr $match & [regexp [lindex $filters $i] [lindex $datos $i]]]
      }
      if { ! $match } {
        $_listbox hide $inx
      } else {
        $_listbox show $inx
      }
    }
    $self configure -flat yes

  }

  #///////////////////////////////////////////////////////////////////////////
  method _RemoveFilter { } {
  #
  # PURPOSE: filters the listbox elements matching filters for each col in cols
  #
  # PARAMETERS:
  #  filters: list of filters for the columns above
  #
  #///////////////////////////////////////////////////////////////////////////
    $_listbox configure -flat no
    $_listbox show all
    $_listbox configure -flat yes
  }

  #///////////////////////////////////////////////////////////////////////////
  method icon { id icon } {
  #
  # PURPOSE: changes the icon for an entry identified by id
  #
  # PARAMETERS:
  #  id: the identificator of the entry
  #  icon: the icon image (must be 16x16
  #
  #///////////////////////////////////////////////////////////////////////////
    $_listbox entry configure $id \
      -icons [list [Bitmap::get $icon] [Bitmap::get $icon]]
  }
	
  # delegacion de metodos y opciones
  delegate method * to _listbox
  delegate option * to _listbox
}

package provide blistboxplus 1.0

# ------- TEST CODE ----

#/////////////////////////////////////////////////////////////////////////////
proc test_blistboxplus {} {
#
# PURPOSE: Funcion de test de un blistcontrol
#
#/////////////////////////////////////////////////////////////////////////////  
  # defining the widgets
  variable data
  set data(-selected) 1
  
  destroy .top
  toplevel .top -width 200 -height 100 
    
  TitleFrame .top.f1 -text "BListBoxPlus test"
  set f1 [ .top.f1 getframe ]
  blistboxplus $f1.blb -cols { col1 col2 col3 col4 } -parent $f1 \
                    -height 150 -width 500 -orientation vertical \
                    -hiddencols {col4}
  $f1.blb insert end {1 2 3 4} {one two three four} {uno dos tres cuatro} \
                     {k kk kkk kkkk} {0 0 0 0} {@ # $ %}
  
  TitleFrame .top.f2 -text "Configure"
  set f2 [ .top.f2 getframe ]
  LabelEntry $f2.lei -width 5 -label Index -text 1
  LabelEntry $f2.lee -width 5 -label End
  LabelEntry $f2.lec -width 50 -label Content -text "{hay que introducir cada} \
     {fila entre corchetes y} {entre estos los items}"
  checkbutton $f2.cb -text "Allow multiple selection" \
    -command { toggleSelectModeCmd }
  $f2.cb select
  button $f2.bi -text "Insert" -command { insertCmd }
  button $f2.bd -text "Delete" -command { deleteCmd }
  button $f2.br -text "Reset" -command { resetCmd }
  $f2.bi configure -width 20
  $f2.bd configure -width 20
  # packing all 
  
  grid .top.f1 -row 0 -column 0 -sticky "new" -padx 10 -pady 10
  grid .top.f2 -row 1 -column 0 -sticky "new" -padx 10 -pady 10
  grid columnconfigure .top 0 -weight 1
  grid rowconfigure .top 0 -weight 0
  grid rowconfigure .top 1 -weight 1
  
  grid $f1.blb -row 0 -column 0 -sticky "new"
  grid columnconfigure $f1 0 -weight 1
  grid rowconfigure $f1 0 -weight 1

  grid $f2.lec -row 0 -column 0 -sticky "new" -columnspan 3
  grid $f2.lei -row 1 -column 0 -sticky "new"
  grid $f2.lee -row 1 -column 1 -sticky "new"
    
  grid $f2.cb  -row 1 -column 2 -sticky "new"
  grid $f2.bi  -row 2 -column 0 -sticky "new"
  grid $f2.bd  -row 2 -column 1 -sticky "new"
  grid $f2.br  -row 2 -column 2 -sticky "new"
  grid columnconfigure $f2 0 -weight 0
  grid columnconfigure $f2 1 -weight 0
  grid columnconfigure $f2 2 -weight 1
  grid rowconfigure $f2 0 -weight 0
  grid rowconfigure $f2 1 -weight 0
  grid rowconfigure $f2 2 -weight 0

  puts "WIDGET blistboxplus: $f1.blb"
  
  # commands attached to the widgets 
  proc insertCmd {} {
    eval .top.f1.f.blb insert [.top.f2.f.lei cget -text] \
                              [.top.f2.f.lec cget -text]
  }
  
  proc deleteCmd {} {
    .top.f1.f.blb delete [.top.f2.f.lei cget -text] \
                         [.top.f2.f.lee cget -text]
  }
  
  proc resetCmd {} {
    .top.f1.f.blb clear
    .top.f1.f.blb insert end {1 2 3 4} {one two three four} \
	  {uno dos tres cuatro} {k kk kkk kkkk} {0 0 0 0} {@ # $ %}
  }
  
  proc toggleSelectModeCmd {} {
    if { [.top.f1.f.blb cget -selectmode] eq "multiple" } {
	  .top.f1.f.blb configure -selectmode single
    } else {
	  .top.f1.f.blb configure -selectmode multiple
    }
  }
}

