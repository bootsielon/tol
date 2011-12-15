if { [ string compare $::tcl_version "8.5" ] >= 0 } {
  package require tile
}
package require -exact treectrl 2.2.9
package require autoscroll
package require snit

# 
# TODO:
#
#  - filtrado: una entry y filtrar tipo google
#
#     buscar mientras se escribe puede ser lento, podemos encolar la
#     busqueda para que sea atendida en un after idle. Para eso el
#     loop de busqueda debe entrar en el loop de eventos de tcl
#
#  - especificar columnas de filtrado
#
#  - parametrizar el contenido de las columnas: usar -tags, sino se da
#    tag se asume el tag column${i}
#
#  - propiedad "-globalcheck" para indicar si el campo check debe
#    sincronizar entre todos los items. Esto implica que sea de tipo
#    "tristate" para poder reflejar un nodo con hijos parcialmente
#    seleccionados. Si un campo check no esta marcado como
#    "globalcheck" entonces es de tipo "on/off".
#
#  - la columna 1 puede llevar dos imagenes para open y close que
#    pueden estar dadas por imision.
#
#  - open_image, close_image: dar unas por omision. No tener en cuenta
#    si es formato tabla.
#
#  - en una columna puedo tener "image", "text" o check
#

#
# Ejemplo de especificacion:
if { 0 } {
  wtree .t01.t \
      -columns [ list \
                     { -tags ID -label "Identifier" \
                           {-type image} {-type text } } \
                     { -tags DESC -label "Description" {-type text} } ]

 # en la definicion de las elementos de columnas se puede dar un unico
 # elemento en cuyo caso se dice el tipo o se puede dar una lista de
 # pares uno de los cuales espcifica el tipo del elemento y el resto
 # define propiedades del elemento.

  .t01.t insert { {"" "node1"} {"Desc. node1"} }
  .t01.t insert [ list { -image "" -text "node 1" } { -text "Desc. node 1" } ]

 # en la insercion de un item las columnas se pueden dar con un solo
 # elemento si la columna contiene solo un elemento o una lista de
 # pares indicando para cada tipo el valor de la propiedad
}
  
snit::widget wtree {
  typevariable SystemButtonFace
  typevariable SystemHighlight
  typevariable SystemHighlightText
  typevariable ButtonImageConf

  option -columns \
      -default "" -configuremethod "_conf-columns"
  option -buttonstyle \
      -default "" -configuremethod "_conf-buttonstyle"
  option -table \
      -default no -configuremethod "_conf-table"
  option -contextmenu  \
      -default "" -configuremethod "_conf-contextmenu"

  option -filter \
      -default 0 -configuremethod "_conf-filter"

  option -columnfilter \
      -default "" -configuremethod "_conf-columnfilter"
  
  variable tree
  variable vscrollbar
  variable hscrollbar

  variable elements
  variable styles
  variable column_info

  variable textFilter
  variable columnsToFilter -array {-1 0}
  variable idIdleFilter ""
  variable idWaitFilter ""
  variable breakFilter 0
  variable numItemsFiltered 0
  variable listFilterOn  ""
  variable listFilterOff ""
  variable widgets

  variable sortedColumn 0

  method _getTrace_ { } {
    trace info variable [ myvar textFilter ]
  }

  delegate method * to tree
  delegate option * to tree
  delegate option -borderwidth to hull
  delegate option -bd to hull
  delegate option -relief to hull
  
  typeconstructor {
    
    set thisPlatform $::tcl_platform(platform)
    set w [ listbox .___lb___ ]
    set SystemButtonFace [$w cget -highlightbackground]
    set SystemHighlight [$w cget -selectbackground]
    set SystemHighlightText [$w cget -selectforeground]
    destroy $w

    if {$thisPlatform == "unix"} {
      # I hate that gray selection color
      set SystemHighlight #316ac5
      set SystemHighlightText White
    }

    if { [ lsearch -exact [ image names ] mac-collapse ] == -1 } {
      image create photo mac-collapse  -data {
        R0lGODlhEAAQALIAAAAAAAAAMwAAZgAAmQAAzAAA/wAzAAAzMyH5BAUAAAYA
        LAAAAAAQABAAggAAAGZmzIiIiLu7u5mZ/8zM/////wAAAAMlaLrc/jDKSRm4
        OAMHiv8EIAwcYRKBSD6AmY4S8K4xXNFVru9SAgAh/oBUaGlzIGFuaW1hdGVk
        IEdJRiBmaWxlIHdhcyBjb25zdHJ1Y3RlZCB1c2luZyBVbGVhZCBHSUYgQW5p
        bWF0b3IgTGl0ZSwgdmlzaXQgdXMgYXQgaHR0cDovL3d3dy51bGVhZC5jb20g
        dG8gZmluZCBvdXQgbW9yZS4BVVNTUENNVAAh/wtQSUFOWUdJRjIuMAdJbWFn
        ZQEBADs=
      }
    }

    if { [ lsearch -exact [ image names ] mac-expand ] == -1 } {
      image create photo mac-expand -data {
        R0lGODlhEAAQALIAAAAAAAAAMwAAZgAAmQAAzAAA/wAzAAAzMyH5BAUAAAYA
        LAAAAAAQABAAggAAAGZmzIiIiLu7u5mZ/8zM/////wAAAAMnaLrc/lCB6MCk
        C5SLNeGR93UFQQRgVaLCEBasG35tB9Qdjhny7vsJACH+gFRoaXMgYW5pbWF0
        ZWQgR0lGIGZpbGUgd2FzIGNvbnN0cnVjdGVkIHVzaW5nIFVsZWFkIEdJRiBB
        bmltYXRvciBMaXRlLCB2aXNpdCB1cyBhdCBodHRwOi8vd3d3LnVsZWFkLmNv
        bSB0byBmaW5kIG91dCBtb3JlLgFVU1NQQ01UACH/C1BJQU5ZR0lGMi4wB0lt
        YWdlAQEAOw==
      }
    }

    if { [ lsearch -exact [ image names ] folder-open ] == -1 } {
      image create photo folder-open -data {
        R0lGODlhEAANANIAAAAAAISEhMbGxv//AP////8AAAAAAAAAACH5BAkZAAUA
        LAAAAAAQAA0AAAM8WBrM+rAEQmmIb4qxBWnNQnCkV32ARHRlGQBgDA7vdN6v
        UK8tC78qlrCWmvRKsJTquHkpZTKAsiCtWq0JAAA7
      }
    }
    
    if { [ lsearch -exact [ image names ] folder-closed ] == -1 } {
      image create photo folder-closed -data {
        R0lGODlhDwANANIAAAAAAISEhMbGxv//AP////8AAAAAAAAAACH5BAkZAAUA
        LAAAAAAPAA0AAAMzWBXM+jCIMUWAT9JtmwtEKI5hAIBcOplgpVIs8bqxa8On
        fNP5zsWzDctD9AAKgKRyuUwAAAA7
      }
    }

    if { [ lsearch -exact [ image names ] actcross16 ] == -1 } {
      image create photo actcross16 -data {
        R0lGODlhEAAQAIIAAASC/PwCBMQCBEQCBIQCBAAAAAAAAAAAACH5BAEAAAAA
        LAAAAAAQABAAAAMuCLrc/hCGFyYLQjQsquLDQ2ScEEJjZkYfyQKlJa2j7AQn
        MM7NfucLze1FLD78CQAh/mhDcmVhdGVkIGJ5IEJNUFRvR0lGIFBybyB2ZXJz
        aW9uIDIuNQ0KqSBEZXZlbENvciAxOTk3LDE5OTguIEFsbCByaWdodHMgcmVz
        ZXJ2ZWQuDQpodHRwOi8vd3d3LmRldmVsY29yLmNvbQA7
      }
    }

    array set ButtonImageConf {
      ""       ""
      "mac"    { mac-collapse open mac-expand    !open }
      "folder" { folder-open  open folder-closed !open }
    }
  }

  method DoubleButton1 {w x y} {

    set id [$w identify $x $y]
    if {$id eq ""} {
      return
    }
    if {[lindex $id 0] eq "item"} {
      #lassign $id where item arg1 arg2
      foreach  {where item arg1 arg2} $id break
      
      if {[llength [$w item descendants $item]] == 0} {
        $self notify generate <<ItemSelected>> [list T $w c $item]
      } else {
        $w item toggle $item
      }
      
    } else {
      TreeCtrl::DoubleButton1 $w $x $y
    }
  }

  method KeyPressReturn {w} {

    if {[llength [$w item descendants active]] == 0} {
      $self notify generate <<ItemSelected>> [list T $w c active]
    } else {
      $w item toggle active
    }
  }
  
  method Button3 {w x y rootX rootY} {

    set cmenu [$self cget -contextmenu]
    if {$cmenu eq ""} {
      return
    }
    set id [$w identify $x $y]

    #if {$id eq ""} {
    #  return
    #}
    if {[lindex $id 0] == "item"} {
      foreach {where item arg1 arg2} $id break
      
      if {[$w selection includes $item]==0} {
        $w selection clear
        $w selection add $item
      }
      $w activate $item
    }
    tk_popup $cmenu $rootX $rootY
  }
  
  method KeyPressEscape {} {

    $self notify generate <<Escape>>
  }
  
  method SortbyColumn {t column} {

	#puts "SortbyColumn: column=$column, tree=$t"
	if {[$t column compare $column == $sortedColumn]} {
	  if {[$t column cget $sortedColumn -arrow] eq "up"} {
		set order -decreasing
		set arrow "down"
	  } else {
		set order -increasing
		set arrow "up"
	  }
	} else {
	  if {[$t column cget $sortedColumn -arrow] eq "down"} {
		set order -decreasing
		set arrow "down"
	  } else {
		set order -increasing
		set arrow "up"
	  }
	  $t column configure $sortedColumn -arrow none
	  set sortedColumn $column
	}
	$t column configure $column -arrow $arrow
	
	$t item sort root $order -column $column -dictionary
  }

  constructor { args } {
    # frame para el filtro de nodos
    frame $win.frameFilter
    label $win.frameFilter.l -text "[ msgcat::mc Search ]:"
    menubutton $win.frameFilter.b -image mac-collapse \
        -menu $win.frameFilter.b.m
    menu $win.frameFilter.b.m \
        -postcommand [ mymethod _postFilterMenu $win.frameFilter.b.m ] \
        -tearoff no
    frame $win.frameFilter.fentry -relief sunken -bd 1 -highlightthickness 1
    set widgets(filter,entry) \
        [ entry $win.frameFilter.fentry.e \
              -bd 0 -highlightthickness 0 \
              -textvar [ myvar textFilter ] ]
    set widgets(filter,bcancel) \
        [ button $win.frameFilter.fentry.b \
              -bd 0 -highlightthickness 0 \
              -image actcross16 \
              -command [ mymethod _cancelFilter ] ]
    grid $widgets(filter,entry) -row 0 -column 0 -sticky snew
    grid $widgets(filter,bcancel) -row 0 -column 1 -sticky ew
    grid rowconfigure $win.frameFilter.fentry 0 -weight 1
    grid columnconfigure $win.frameFilter.fentry 0 -weight 1
    grid remove $widgets(filter,bcancel)
    
    bind $widgets(filter,entry) <Key-Return> [ mymethod _applyFilter ]
    grid $win.frameFilter.l -row 0 -column 1 -sticky "e"
    grid $win.frameFilter.b -row 0 -column 2 -sticky "e"
    grid $win.frameFilter.fentry -row 0 -column 3 -sticky "ew"
    grid columnconfigure $win.frameFilter 0 -weight 1
    grid columnconfigure $win.frameFilter 3 -weight 1
    grid $win.frameFilter -row 0 -column 0 -sticky "snew"

    trace add variable [ myvar textFilter ] write [ mymethod _onChangeFilter ]

    # frame para tree widget
    frame $win.frameTree
    grid $win.frameTree -row 1 -column 0 -sticky "snew"
    grid rowconfigure $win.frameTree 0 -weight 1
    grid columnconfigure $win.frameTree 0 -weight 1
    
    install tree as treectrl $win.frameTree.t \
        -highlightthickness 0 -borderwidth 0 \
        -xscrollincrement 20 -showheader 1 -indent 19 \
        -selectmode extended -showroot no \
        -showrootbutton no -showbuttons yes -showlines yes \
        -scrollmargin 16 -xscrolldelay "500 50" -yscrolldelay "500 50"


    $tree notify install <<ItemSelected>>
    $tree notify install <<Escape>>

    $tree notify install <Header-invoke>

    $tree notify install <Edit-begin>
    $tree notify install <Edit-end>
    $tree notify install <Edit-accept>
    
    $tree notify bind $tree <Edit-begin> \
        [ mymethod EditCallback begin %I %C %E %t ]
    $tree notify bind $tree <Edit-end> \
        [ mymethod EditCallback end %I %C %E %t ]
    $tree notify bind $tree <Edit-accept>  \
        [ mymethod EditCallback accept %I %C %E %t ]
    
    bind $tree <ButtonPress-1> [ mymethod OnButtonPress1 %x %y ]
    bind $tree <Control-ButtonPress-1> { } 
    bind $tree <Shift-ButtonPress-1> { } 

    set height [font metrics [$tree cget -font] -linespace]
    if {$height < 18} {
      set height 18
    }
    $tree configure -itemheight $height

    # create the elements & styles

    set elements(image) [ $tree element create eIMG image ]
    set elements(text) [ $tree element create eTXT text \
                           -fill [list $wtree::SystemHighlightText \
                                      {selected focus}] ]
    set elements(check) [ $tree element create eCHK window ]
    
    set elements(rect) [ $tree element create eRECT rect \
                             -showfocus yes -open ew \
                             -fill [ list \
                                         $wtree::SystemHighlight \
                                         {selected focus} \
                                         gray {selected !focus} ] ]


    # vertical scrollbar
    if { [ string compare $::tcl_version "8.5" ] >= 0 } {
      set wscroll ttk::scrollbar
    } else {
      set wscroll scrollbar
    }
    install vscrollbar as $wscroll $win.frameTree.sv \
        -orient vertical -command [list $win.frameTree.t yview]

    # horizontal scrollbar
    install hscrollbar as $wscroll $win.frameTree.sh \
        -orient horizontal -command [list $win.frameTree.t xview]

    grid $tree       -row 0 -column 0 -sticky nsew
    grid $vscrollbar -row 0 -column 1 -sticky ns
    grid $hscrollbar -row 1 -column 0 -sticky ew
    grid columnconfigure $win 0 -weight 1
    grid rowconfigure    $win 1 -weight 1

    focus $tree
    
    $self _configure_bindings
    $self _conf-columns -columns $options(-columns)
    $self _conf-contextmenu -contextmenu $options(-contextmenu)
    $self configurelist $args
    if { !$options(-filter) } {
      grid remove $win.frameFilter
    }
    #return $win
  }
  
  method getItemText { item } {
    set itemText ""
    # buscar las columnas que tiene elemento eTXT
    foreach c [ $tree column list ] {
      if { !$columnsToFilter($c) } {
        continue
      }
      # extraigo el style asociado a la columna
      set columnStyle [ $tree column cget $c -itemstyle ]
      # extraigo los elementos del style
      set styleElements [ $tree style elements $columnStyle ]
      # busco si eTXT es uno de esos elementos
      if { [ lsearch $styleElements "eTXT" ] != -1 } {
        # el root no tiene styles
        catch {
          append itemText [ $tree item element cget $item $c eTXT -text ] " "
        }
      }
    }
    return $itemText
  }

  method getColumnLabels { args } {
    array set opts {
      -onlyvisible yes
      -hastext no
    }
    array set opts $args
    set labels [ list ]
    foreach c [ $tree column list ] {
      if { $opts(-hastext) } {
        # extraigo el style asociado a la columna
        set columnStyle [ $tree column cget $c -itemstyle ]
        # extraigo los elementos del style
        set styleElements [ $tree style elements $columnStyle ]
        # busco si eTXT es uno de esos elementos
        if { [ lsearch $styleElements "eTXT" ] == -1 } {
          # no tiene elemento Text
          continue
        }
      }
      if { $opts(-onlyvisible) && ![ $tree column cget $c -visible ] } {
        # no es visible
        continue
      }
      lappend labels [ list -idx $c -text [ $tree column cget $c -text ] ]
    }
    return $labels
  }

  method _postFilterMenu { m } {
    $m delete 0 end
    foreach c [ $self getColumnLabels ] {
      array set cinfo $c
      $m add checkbutton -label $cinfo(-text) -onvalue 1 -offvalue 0 \
          -variable [ myvar columnsToFilter($cinfo(-idx)) ]
    }
  }

  method _setWaitCursor { } {
    $win configure -cursor watch
    $widgets(filter,entry) configure -cursor watch
    update
  }

  method _setNormalCursor { } {
    $win configure -cursor {}
    $widgets(filter,entry) configure -cursor {}
    update
  }

  method _cancelFilter { } {
    set textFilter ""
    grid remove $widgets(filter,bcancel)
  }

  method filterItems { text } {
    #puts "filterItems $text"
    $self _setWaitCursor
    set negativeWords [ list ]
    set positiveWords [ list ]
    set negating 0
    set wordList [regexp -inline -all -- {\S+} $text]
    foreach word $wordList {
      if { $negating } {
        lappend negative $word
        set negating 0
      } elseif { [ string index $word 0 ] eq "-" } {
        # se pide negar la palabra
        set _word [ string range $word 1 end ]
        if { $_word eq "" } {
          set negating 1
        } else {
          lappend negativeWords $_word
        }
      } else {
        lappend positiveWords $word
      }
    }
    # regexp {^(?!(.*(foo|bar).*))} atttcokebar
    if { [ llength $negativeWords ] } {
      set negativeRegExp "^(?!(.*([ join $negativeWords | ]).*))"
    } else {
      set negativeRegExp ""
    }
    set breakFilter 0
    set numItemsFiltered 0
    set listFilterOn [ list ]
    set listFilterOff [ list ]
    $self _filterNodeSet [ $tree item children root ] \
        $positiveWords $negativeRegExp
    $self _setNormalCursor
  }
 
  method _filterNodesMatching { } {
    if { [ llength $listFilterOff ] } {
      $tree item configure [ list list $listFilterOff ] -visible 0
      set listFilterOff [ list ]
    }
    if { [ llength $listFilterOn ] } {
      $tree item configure [ list list $listFilterOn ] -visible 1
      if { !$options(-table) } {
        foreach nid $listFilterOn {
          $tree item configure \
              [ list list [ $tree item ancestors $nid ] ] -visible 1
        }
      }
      set listFilterOn [ list ]
    }
  }

  method _filterNodeSet { nodes positiveWords negativeRegExp } {
    #puts "filterNodeSet '$nodes' '$positiveWords'"
    # recorro cada uno de los nodos
    foreach n $nodes {
      # filtro el nodo
      
      #esto es para el filtrado automatico incremental
      if { $breakFilter } {
        return 0
      }

      $self _filterNode $n $positiveWords $negativeRegExp
      # desciendo a los hijos
      $self _filterNodeSet [ $tree item children $n ] \
          $positiveWords $negativeRegExp
      
      if { 0 } {
      if { [ $self _filterNode $n $positiveWords $negativeRegExp ] } {
        # si el resultado es visible desciendo y filtro a los hijos
        $self _filterNodeSet [ $tree item children $n ] \
            $positiveWords $negativeRegExp
      }
      }

      # esto es para el filtrado automatico incremental
      incr numItemsFiltered
      if { !$breakFilter && !( $numItemsFiltered % 1000 ) } {
        # refresco la cola de eventos
        $self _filterNodesMatching
        update
      }
    }
    $self _filterNodesMatching
    return 1
  }

  method _filterNode { node positiveWords negativeRegExp } {
    #puts "filterNode '$node' '$positiveWords' '$negativeRegExp'"
    set itemText [ $self getItemText $node ]
    #puts "itemText $itemText"
    # hago match, luego hago algo mas sofisticado con regexp
    # set visible [ string match "*${text}*" $itemText ]
    if { [ string length $negativeRegExp ] && 
         ![ regexp -nocase $negativeRegExp $itemText ] } {
      # itemText contiene alguna de las palabra prohibidas
      #puts "itemText contiene alguna de las palabra prohibidas"
      #$tree item configure $node -visible 0
      lappend listFilterOff $node
      return 0
    }
    foreach word $positiveWords {
      if { ![ string match -nocase "*${word}*" $itemText ] } {
        # itemText no contiene una de las palabras deseadas
        #puts "itemText no contiene una de las palabras deseadas: $word"
        #puts "string match \"*${word}*\" $itemText"
        #$tree item configure $node -visible 0
        lappend listFilterOff $node
        return 0
      }
    }
    # finalmente todas las palabras deseadas y niguna de las
    # prohibidas estan.
    #$tree item configure $node -visible 1
    lappend listFilterOn $node
    return 1
  }

  method _applyFilter { } {
    set columnFound -1
    foreach c [ array name columnsToFilter ] {
      if { $columnsToFilter($c) } {
        set columnFound $c
        break
      }
    }
    if { $columnFound == -1 } {
      return
    }
    if { $textFilter eq "" } {
      $tree item configure all -visible 1
    } else {
      $self filterItems $textFilter
    }
  }

  method _onChangeFilter { args } {
    if { $textFilter eq "" } {
      grid remove $widgets(filter,bcancel)
      $tree item configure all -visible 1
    } else {
      #puts "filtering on $textFilter"
      grid $widgets(filter,bcancel)
      update
      after cancel $idWaitFilter
      set breakFilter 1
      set idWaitFilter [ after 500 $self _scheduleFilter ]
    }
  }
  
  method _scheduleFilter { } {
    after cancel $idIdleFilter
    set idIdleFilter [ after idle $self _applyFilter ]
  }

  method _configure_bindings { } {
    
    bind $tree <Double-ButtonPress-1> "[mymethod DoubleButton1 %W %x %y] ; break"
    bind $tree <Return> "[mymethod KeyPressReturn %W] ; break"

    bind $tree <ButtonPress-3> [ mymethod Button3 %W %x %y %X %Y]

    bind $tree <Escape> "[mymethod KeyPressEscape] ; break"
    
    $tree notify bind $tree <Header-invoke> [mymethod SortbyColumn %W %C]

    $tree notify bind $vscrollbar <Scroll-y> { %W set %l %u }
    bind $vscrollbar <ButtonPress-1> [list focus $tree]
    
    $tree notify bind $hscrollbar <Scroll-x> { %W set %l %u }
    bind $hscrollbar <ButtonPress-1> [list focus $tree]
    
    ::autoscroll::autoscroll $vscrollbar
    ::autoscroll::autoscroll $hscrollbar
  }

  method _conf-table { _ yesno } {
    if { $yesno } {
      set options(-table) 1
      $tree configure -showbuttons 0 -showlines 0
    } else {
      set options(-table) 0
      $tree configure -showbuttons 1 -showlines 1
    }
  }

  method _conf-buttonstyle { _ bs } {
    set valid_bstyles [ array names wtree::ButtonImageConf ]
    if { [ lsearch $valid_bstyles $bs ] == -1 } { 
      error "invalid button style $bs should be one of $valid_bstyles"
    } else {
      set options(-buttonstyle) $bs
      $tree configure -buttonimage $wtree::ButtonImageConf($bs)
    }
  }

  method _conf-contextmenu { _ cmenu } {
    set options(-contextmenu) $cmenu 
  }

  method _conf-filter { _ value } {
    if { [ string is boolean $value ] } {
      if { ( $options(-filter)  && $value ) || 
           ( !$options(-filter) && !$value ) } {
        return
      }
      set options(-filter) $value
      if { $value } {
        grid $win.frameFilter
      } else {
        grid remove $win.frameFilter
      }
      set textFilter ""
    } else {
      error "invalid boolean value $value"
    }
  }

  method _conf-columnfilter { _ value } {
    # hago el chequeo en el postcommand del menu que despliegua las
    # opciones y tambien a la hora de filtrar
    set options(-columnfilter) $value
	foreach {idx v} $value {
	  set columnsToFilter($idx) $v
    }
  }

  #
  # columnsSpec es una lista de "columnas" y una columna puede ser
  # 
  #  - un unico elemento con valor: image, text o check
  #
  #  - una lista de elementos, donde cada elemento puede estar dado
  #    por su tipo.
  #
  #    { image text }
  #
  #    en un futuro se le puede adjuntar propiedades de layout.
  #
  #    cada columna crea un stilo y el layout esa definido por el
  #    orden en que se describen los elementos de la columna.
  #
  method _conf-columns { _ columnsSpec } {
    if { $columnsSpec eq "" } {
      set columnsSpec [ list { text -tags C0 -label "Column 0" -editable 0 } ]
    }
    set options(-columns) $columnsSpec
    # elimino columnas
    $tree column delete all
    # elimino estilos
    eval $tree style delete [ $tree style names ]

    # limpio la informacion interna de las columnas
    array unset column_info
    array unset style_info
    # crear las columnas y estilos
    set idx 0
    foreach col_spec $columnsSpec {
      set col_elements [ lindex $col_spec 0 ]
      # opciones por omision.
      array set opts [ list -tags C${idx} -label "Column $idx" -image "" \
                           -editable 0 ]
      # opciones dadas en la llamada.
      array set opts [ lrange $col_spec 1 end ]
      set colID [ $tree column create \
                      -text [ msgcat::mc $opts(-label) ] \
                      -image $opts(-image) \
                      -itembackground "\#e0e8f0 {}" \
                      -tags $opts(-tags) ]
      set column_info($idx,ID) $colID
      set column_info(MAPIDX,$colID) $idx
      set column_info($idx,elements) $col_elements
      set column_info($idx,-editable) $opts(-editable)
      # creamos el estilo para esta columna
      set s [ $tree style create style$colID ]
      set column_info($idx,style) $s
      set eList [ list ]
      # selecciono los id's de los elementos de esta columna
      set eIMG ""
      foreach e $col_elements {
        lappend eList $elements($e)
        if { $e eq "image" } {
          set eIMG $elements($e)
        }
      }
      # asocio los elementos al style
      $tree style elements $s [ eval list eRECT $eList ]

      # defino el layout de los elementos
      $tree style layout   $s eRECT -union $eList -iexpand nswe -ipadx 2
      if { $eIMG ne "" } {
        $tree style layout   $s $eIMG -iexpand ns -padx 2
      }
      # asocio el style a la columna
      $tree column configure $colID -itemstyle $s
      incr idx
    }
    # Specifies a column description that determines which column
    # displays the expand/collapse buttons and connecting lines between
    # items. The default is unspecified.
    $tree configure -treecolumn $column_info(0,ID)
    # extraigo las columnas filtrables
    set first ""
    set hasCheck 0
    foreach c [ $self getColumnLabels -onlyvisible yes -hastext yes ] {
      array set cinfo $c
      if { $first eq "" } {
        set first $cinfo(-idx)
      }
      if { [ info exists columnsToFilter($cinfo(-idx)) ] } {
        set _columnsToFilter($cinfo(-idx)) $columnsToFilter($cinfo(-idx))
      } else {
        set _columnsToFilter($cinfo(-idx)) 0
      }
      if { $_columnsToFilter($cinfo(-idx)) } {
        set hasCheck 1
      }
    }
    if { !$hasCheck } {
      set _columnsToFilter($first) 1
    }
    array unset columnsToFilter
    array set columnsToFilter [ array get _columnsToFilter ]
  }

  method insert { data args } {
    array set insopt {
      -at end
      -relative root
      -open 0
      -button 1
      -tags ""
    }
    array set insopt $args
    # creamos el item, -button yes debe depender si el widget no es
    # tipo tabla
    set item [ $tree item create \
                   -button $insopt(-button) \
                   -tags insopt(-tags) ]
    set idx 0
    set idx_last  [ expr [ llength $options(-columns) ] - 1 ]
    # 'data' es una lista con informacion para cada columna. Lo que
    # venga de mas en data no lo considero.
    foreach col_data [ lrange $data 0 $idx_last ] {
      # los elementos de esta columna
      set col_elements $column_info($idx,elements)
      # col_data debe ser una lista de la misma cantidad de elementos
      set e_count [ llength $col_elements ]
      # esta lista es para usar la sintaxis de configurar + de un
      # elemento al estilo
      #
      # $tree item element configure $item $columnID($idx) \
      #      $elements(IMG) -image $opts(-image) + \
      #      $elements(TXT) -text  $opts(-text)
      #
      set elements_config [ list ]
      for { set i 0 } { $i < $e_count } { incr i } {
        set e_data [ lindex $col_data $i ]
        set e_conf [ $self _build_element_config \
                         [ lindex $col_elements $i ] $e_data ]
        eval $tree item element configure $item $column_info($idx,ID) $e_conf
      }
      incr idx
    }
    # insertamos el item en el arbol
    $tree item collapse $item
    switch $insopt(-at) {
      end - child {
        $tree item lastchild $insopt(-relative) $item
      }
      next {
        $tree item nextsibling $insopt(-relative) $item
      }
      prev {
        $tree item prevsibling $insopt(-relative) $item
      }
      default {
        error "invalid index, should be: end (child), prev or next"
      }
    }
    return $item
  }

  method _build_element_config { etype data } {
    array set def_prop {
      text -text
      image -image
    }
    if { [ catch { llength $data } ] } {
      set data [ list $data ]
    }
    if { [ llength $data ] } {
      if { [ llength $data ] == 1 } {
        return [ list $elements($etype) $def_prop($etype) [ string trim [ lindex $data 0 ] ] ]
      } elseif { [ string index [ lindex $data 0 ] 0 ] ne "-" } {
        return [ list $elements($etype) $def_prop($etype) [ string trim $data ] ]
      } else {
        return [ linsert $data 0 $elements($etype) ]
      }
    } else {
      return [ list $elements($etype) $def_prop($etype) "" ]
    } 
  }

  method EditCallback { callback item column elem t } {
    if { $callback eq "accept" } {
      $tree item element configure $item $column $elem -text $t
    }
  }

  method OnButtonPress1 { x y } {
    
    set treeInfo [ $tree identify $x $y ]
    if { $treeInfo eq "" } {
      $tree selection clear
      return
    }
    
    if { [ llength $treeInfo ] != 6 } {
      return
    }
    
    foreach { item itemNum column columnNum elem elemName } $treeInfo { }
    
    #puts "OnButtonPress1: $treeInfo"
    if { $item == "item" && \
             $column == "column" && \
             $elem == "elem" && \
             [ $tree selection includes $itemNum ] } {

      #puts "$itemNum = [$tree selection includes $itemNum ]"

      set idx_info $column_info(MAPIDX,$columnNum)

      if { $column_info($idx_info,-editable) && $elemName eq "eTXT"} {
        ::TreeCtrl::ButtonPress1 $tree $x $y
        update
        ::TreeCtrl::EntryOpen $tree $itemNum $columnNum $elemName
        return -code break
      }
    }
  }
  
  proc test_GetTextContents { T item } {
    set textDump ""
    # buscar las columnas que tiene elemento eTXT
    foreach c [ $T column list ] {
      # extraigo el style asociado al item
      set style [ $T column cget $c -itemstyle ]
      # extraigo los elementos del style
      set elements [ $T style elements $style ]
      # busco si eTXT es uno de esos elementos
      if { [ lsearch $elements "eTXT" ] != -1 } {
        append textDump [ $T item element cget $item $c eTXT -text ] " "
      }
    }
    return $textDump
  }

  proc test_ListenSearch { t args } {
    if { $::wtreeSearch eq "" } {
      $t item configure all -visible 1
    } else {
      #puts "filtering on $::wtreeSearch"
      #$t filterNodeSet [ $t item children root ] $::wtreeSearch
      catch {
        $t filterItems $::wtreeSearch
      } msg
      #puts "msg = $msg"
    }
  }

  proc test_01 { w { istable 0 } } {
    destroy $w
    toplevel $w
    if { 0 } {
      frame $w.fsearch
      label $w.fsearch.l -text "Search:"
      entry $w.fsearch.e -textvar ::wtreeSearch
      grid $w.fsearch.l -row 0 -column 1 -sticky "e"
      grid $w.fsearch.e -row 0 -column 2 -sticky "ew"
      grid columnconfigure $w.fsearch 0 -weight 1
      grid $w.fsearch -row 0 -column 0 -sticky "snew"
    }
    
    set T [ wtree $w.t -table $istable -filter yes -buttonstyle mac \
                -columns [ list \
                               { {image text} -tags ID   -label Identifier } \
                               { text -tags DESC -label Description } ] ]
    
    $T notify bind $T <Expand-before> {
      #puts "before openning %I"
    }
    $T notify bind $T <Collapse-after> {
      #puts "after closing %I"
    }
    $T notify bind $T <ActiveItem> {
      #puts "now active item is %c the previous was %p"
    }
    grid $T -row 1 -column 0 -sticky "snew"
    grid rowconfigure $w 1 -weight 1
    grid columnconfigure $w 0 -weight 1

    if { 0 } {
      trace remove variable ::wtreeSearch write "::wtree::test_ListenSearch $T"
      trace add    variable ::wtreeSearch write "::wtree::test_ListenSearch $T"
    }
    
    set bugValue {
Serie AlgGetData(
 "Lla_MetCon.Atendi.Ser_TNuXXX_GHLAll.Ind.ALL_TLlTLl.XXX_RotXXX.XXX.XXX.XXX_EspEsp.XXX_SegSeg.XXX_PerMHo.XXX_FecDia",
 AlgProj1004)
}
    set id [ $T insert [list  [list folder-open "node bug"] [ list $bugValue ] ] ]
    set id [ $T insert [list [list folder-open "node 1"] [list "Desc node\n 1"]] ]
    set id [ $T insert { { folder-closed "node 2" } { "Desc node 2" } } \
                 -at end -relative [ expr { $istable ? "root" : $id } ] ]
    return .t01.t
  }

  proc TestSearch { w } {
    destroy $w
    toplevel $w

    set T [ wtree $w.t -table 1 -filter yes -buttonstyle mac \
                -columns [ list \
                               { {image text} -tags ID   -label Identifier } \
                               { text -tags DESC -label Description -editable yes} ] ]
    grid $T -row 1 -column 0 -sticky "snew"
    grid rowconfigure $w 1 -weight 1
    grid columnconfigure $w 0 -weight 1
    
    for { set r 0 } { $r < 100 } { incr r } {
      $T insert [list  [list folder-open "node $r"] [ list "Description for node $r" ] ]
    }
    return $T
  }
}

package provide wtree 1.0