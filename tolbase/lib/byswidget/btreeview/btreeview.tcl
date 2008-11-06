#/////////////////////////////////////////////////////////////////////////////
# FILE    : btreeview.tcl
# PURPOSE : Bayes Tree View. Treeview especial que permite la selección de
#         nodos en jerarquía, es decir, si un padre tiene todos sus hijos
#         seleccionados atomaticamente pasa a estar seleccionado. 
#/////////////////////////////////////////////////////////////////////////////

# include
if {0} {
#  set toltk_script_path [file dirname [info script]]

  package require snit 0.91
  package require Tk
  package require BLT

  package require msgcat
  namespace import msgcat::*
}

package require Tk
package require BLT
package require byscommon


#/////////////////////////////////////////////////////////////////////////////
::snit::widgetadaptor btreeview {
#
# PURPOSE: Sobrecargamos un TreeView para que tenga nuevas funcionalidades
#
#/////////////////////////////////////////////////////////////////////////////
  # opciones especificas
  # breadonly    -> componente en modo de lectura o lectura/escritura
  # bmultiselect -> si se pueden seleccionar varios nodos a la vez
  # bicosel      -> si se muestra el icono del checkbutton
  # bonselcmd    -> comando a ejecutar cuando se selecciona un nodo
  # bhelptext    -> columna que se muestra como helptext
  # btogglehier  -> cambiar jerárquicamente si un item está o no seleccionado
#/////////////////////////////////////////////////////////////////////////////
  variable items_selected
  variable imgs 
  variable types
  option -breadonly    false
  option -bmultiselect true
  option -bicosel      true
  option -bonselcmd    ""
  option -bhelptext 
  option -bmenu
  option -btogglehier true
  #///////////////////////////////////////////////////////////////////////////
  constructor {args} {
  #
  # PURPOSE: Constructor de un btreeview. Crea una instancia de un TreeView
  #
  # PARAMETERS:
  #   args -> Lista de opciones especificas de un TreeView, mas las propias de
  #           un btreeview
  #
  #///////////////////////////////////////////////////////////////////////////
    global toltk_options
    variable imgs

    set imgs [list ON [::Bitmap::get on] OFF [::Bitmap::get off] MED [::Bitmap::get med]]
    # Widget que va a servir de contenedor para blistbox

    installhull using ::blt::treeview

    # Apply default arguments
    $self configure -hideroot yes -linewidth 0 -activeicons "" \
      -selectbackground gray90 -selectforeground black \
      -highlightthickness 1 -borderwidth 1 -selectborderwidth 1 \
      -font $toltk_options(fonts,Label) -icons {}

    # Apply all arguments given:
    $self configurelist $args

    # creamos menu
    # No estan definidas las opciones
    $self CreateMenu 

    $self style  configure text     -bg white
    $self column configure treeView -bg white

    eval $self column insert end type state childsON childsMED \
      parent [$self cget -bhelptext] -hide true

    bind $self  <Button-1> +[list focus -force %W]
    #$self bind all <Enter> +[list $self HelpText enter]
    #$self bind all <Leave> +[list $self HelpText leave]
    if [llength [$self cget -bhelptext]] {
      DynamicHelp::add $self -command [list $self HelpText]
      #set cmd [bind BwHelpVariable <Leave>]
      #puts "cmd=$cmd"
      # Habria que hacer que al salir el cursor del nodo desapareciese el helptext
      #$self bind all <Leave>   "DynamicHelp::_leave_info $self"
    }
#variable ${selfns}::helptext 
    $self configure -selectcommand [list $self OnSelectCommand]
    $self entry configure root -data {state NSE} -label "ROOT"
    #bind $self <ButtonPress-1> [list $self OnSelectCommand]
  }

  #///////////////////////////////////////////////////////////////////////////
  method OnSelectCommand  {} {
  #///////////////////////////////////////////////////////////////////////////
    Tolcon_Trace "Llamando a OnSelectCommand"
    set index [$self index current]
    if [$self cget -bmultiselect] {
      $self ToggleNode $index
    } else {
      $self SelectAll OFF
      $self ToggleNode $index
    }
    # Evaluo el comando del usuario
    set cmd [$self cget -bonselcmd]
    if {[llength $cmd]} {
      eval $cmd
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method HelpText  {} {
  #
  # PURPOSE: Returns the helptext related to the column specified in
  #  -bhelptext to the current node
  #
  #///////////////////////////////////////////////////////////////////////////
    set index [$self index current]

    set txt ""
    if [llength $index] {
      if [llength [$self cget -bhelptext]] {
        array set aryData [$self entry cget $index -data]
        set txt $aryData([$self cget -bhelptext])
      }
    }
    return $txt
  }

  #///////////////////////////////////////////////////////////////////////////
  method CreateMenu {args} {
  #
  # PURPOSE: Append menu to treeview. 
  #
  #///////////////////////////////////////////////////////////////////////////
    set m [menu $self.m -tearoff 0]
    # read option
    if [$self cget -bmultiselect] {
      $m add command -label [mc "Select all"]       -command [list $self SelectAll ON]
      $m add command -label [mc "Unselect all"]     -command [list $self SelectAll OFF]
      $m add command -label [mc "Toggle selection"] -command [list $self ToggleSelect]
      $m add separator
    }
    # read/write option
    if {![$self cget -breadonly]} {
      # delete 
      $m add command -label [mc "Delete all"]
      $m add command -label [mc "Delete selected"]
      $m add separator
    }
    # read option
    # copy
    set mc [menu $m.copy -tearoff 0]
    $m add cascade -label [mc "Copy"] -menu $mc
    $mc add command -label [mc "Copy all"]
    $mc add command -label [mc "Copy selected"]
    $mc add command -label [mc "Copy unselect"]
    # count
    $m add separator
    $m add command -label [mc "Count elements"]
    # file
    $m add separator
    set mf [menu $m.file -tearoff 0]
    $m add cascade -label [mc "File"] -menu $mf
    $mf add command -label "[mc "Save to file"] ..."
    if {![$self cget -breadonly]} {
      $mf add command -label "[mc "Load from file"] ..."
    }
    bind $self <Button-3> "tk_popup $m %X %Y"
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method bColumnInsert {args} {
  #///////////////////////////////////////////////////////////////////////////
    eval $self column insert $args
    # Asocio a la nueva columna (y a todas de momento) el
    # procedimiento de ordenar la columna
    # Coger de args los nombres de las columnas insertadas y 
    # asociar solo a estas (son del elto 1 de args hasta la 
    # aparición de la primera opcion (va con - deltante))
    foreach column [$self column names] {
      $self column configure $column \
	    -command [list [codename SortColumn] $self $column]
    }
  }

  # delegacion de metodos y opciones
  delegate method * to hull
  delegate option * to hull

  #///////////////////////////////////////////////////////////////////////////
  proc SortColumn {ht column} {
  #///////////////////////////////////////////////////////////////////////////
    set old [$ht sort cget -column] 
    set decreasing 0
    if { "$old" == "$column" } {
      set decreasing [$ht sort cget -decreasing]
      set decreasing [expr !$decreasing]
    }
    $ht sort configure -decreasing $decreasing -column $column -mode integer
    $ht configure -flat no
    $ht sort auto yes
  
    blt::busy hold $ht
    update
    blt::busy release $ht
  }

  #///////////////////////////////////////////////////////////////////////////
  method getSelected {{column ""} {from root}} {
  # PURPOSE: Devuelve un dato correspondiente a una columna de los nodos
  #          seleccionados (con state=ON) a partir de un nodo dado
  # PARAMETERS:
  #   column : Columna previamente definida cuyo dato se devuelve. Si no se
  #        especifica ninguna se devuelve el identificador del nodo
  #   from   : Identificador del nodo desde el que se obtienen los nodos 
  #        seleccionados. Por defecto root
  #///////////////////////////////////////////////////////////////////////////
    variable items_selected

    if [$self cget -bmultiselect] {
      set items_selected ""
      $self _GetSelected $column $from
    } else  {      
       #Solo puede estar seleccionado uno o ninguno:
       set it [$self curselection]
       puts "it=$it"
       if {$it ne ""} {
         if {$column ne ""} {
           array set aryData [$self entry cget $it -data]         
           set items_selected $aryData($column)
         } else {
           set items_selected [$self entry cget $it -label]
         }
       } else {
         #Sin selección válida. Devolvemos vacío.
         set items_selected ""
       }
    }
    return $items_selected
  }

  #///////////////////////////////////////////////////////////////////////////
  method _GetSelected {column {from root}} {
  #///////////////////////////////////////////////////////////////////////////
    variable items_selected
    foreach it [$self entry children $from] {
      array set aryData [$self entry cget $it -data]
      set state $aryData(state)
      if {[string equal $state "ON"]}  {
        if {$column ne ""} {
          lappend items_selected $aryData($column)
        } else  {
          lappend items_selected [$self entry cget $it -label]
        }
      }
      $self _GetSelected $column $it
    }
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method SetNodeState {index new_state} {
  #///////////////////////////////////////////////////////////////////////////
    #Tolcon_Trace "SetNodeState. index=$index, new_state=$new_state"
    array set aryData [$self entry cget $index -data]
    set state $aryData(state)
    if { [string equal $state NSE] }        return
    if { [string equal $state $new_state] } {
      $self _SetNodeState $index $new_state
    } else {
      $self ToggleNode $index
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method _SetNodeState {index state} {
  #///////////////////////////////////////////////////////////////////////////
    array set aryData [$self entry cget $index -data]
    
    $self entry configure $index -data [list state $state]
    set icons [$self GetImages $aryData(type) $state]
    $self entry configure $index -icons $icons
  }

  #///////////////////////////////////////////////////////////////////////////
  method ToggleNode {index} { 
  #///////////////////////////////////////////////////////////////////////////
    array set aryData [$self entry cget $index -data]
    set state $aryData(state)
	
	set ctogglehier [$self cget -btogglehier]

    if { [string equal $state NSE] } return
    set parent_idx $aryData(parent)
    
    if { [string equal $state ON] } {
      $self _SetNodeState $index OFF
	  if $ctogglehier {
        $self ToggleUp $parent_idx OFF ON
        $self entry configure $index -data [list childsON 0 childsMED 0]
        foreach it [$self entry children $index] {
          $self ToggleDown $it OFF
        }
	  }
    } else {
      $self _SetNodeState $index ON
	  if $ctogglehier {
        $self ToggleUp $parent_idx ON $state
        $self entry configure $index\
             -data [list childsON [$self entry size $index] childsMED 0]
        foreach it [$self entry children $index] {
          $self ToggleDown $it ON
        }
	  }
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method ToggleUp {index state from} {
  #///////////////////////////////////////////////////////////////////////////
    # state es el estado al que ha pasado el hijo que me ha llamado
    # from  es el estado en el que estaba el hijo que me ha llamado
    array set aryData [$self entry cget $index -data]
    set stateOld $aryData(state)
    if { [string equal $stateOld NSE] } return
    set childsON  $aryData(childsON)  
    set childsMED $aryData(childsMED)
    set childs [llength [$self entry children $index]]  
    switch -- $state {
      ON {
        incr childsON
        if {[string equal $from MED]} { incr childsMED -1 }
        if {$childsON == $childs} {
          set stateNew ON
        } else {
          set stateNew MED
        }
      }
      OFF {
        if {[string equal $from ON]} {
          incr childsON -1
        } else  {
          incr childsMED -1
        }
        if {($childsON == 0) && ($childsMED == 0)} {
          set stateNew OFF
        } else  {
          set stateNew MED
        }
      }
      MED  {
        incr childsMED
        if {[string equal $from ON]} { incr childsON -1 }
        set stateNew MED
      }
    }
  
    $self entry configure $index -data [list childsON $childsON childsMED $childsMED]
    $self _SetNodeState $index $stateNew
    if {$stateNew != $stateOld} {
      #set list [lindex [$self get -full $index] 0]
      #Para la versión 8.4.1 cambiado por
      set list [$self get -full $index]
      set parent_idx [$self find -name [lindex $list end-1]]
      $self ToggleUp $parent_idx $stateNew $stateOld
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method ToggleDown {index state} {
  #///////////////////////////////////////////////////////////////////////////
    set childs [$self entry children $index]
    if {[string equal $state OFF]} {
      $self entry configure $index -data [list childsON 0 childsMED 0]
    } else  {
      if {[string equal $state ON]} {
        $self entry configure $index -data [list childsON [llength $childs] childsMED 0]
      }
    }
    $self _SetNodeState $index $state
    foreach it $childs {
      $self ToggleDown $it $state
    }
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method GetImages {tipo state} {
  #///////////////////////////////////////////////////////////////////////////
    variable imgs
    array set aryImg $imgs
    if [$self cget -bicosel] {
      set icos [list $aryImg($state) $aryImg($state)]
    } else  {
      set icos {}
    }
    return $icos
  }

  #///////////////////////////////////////////////////////////////////////////
  method insertNode {parentId tipo state path {label {}} {others {}} } {
  # PURPOSE: Inserta un nodo en el arbol, inicializando los atributos 
  #       de tipo, estado y los relativos a la jerarquia del btreeview
  #
  # PARAMETERS:
  #   parentId: identificador del nodo padre
  #   tipo    :
  #   state   : ON,OFF indica si el checkbutton asociado estara o no activado
  #   path    :
  #   label   :
  #   others  : list of pairs {column value} for columns previously defined
  #///////////////////////////////////////////////////////////////////////////
    if {[string equal $label ""]} {
      set label $path
    }
    set lstData [list type $tipo state $state childsON 0 childsMED 0 parent $parentId]
    set idx [$self insert -at $parentId end $path -label $label \
         -data [concat $lstData $others]]
    $self SetNodeState $idx $state
    return $idx
  }
  

  #///////////////////////////////////////////////////////////////////////////
  method ToggleSelect {} {
  # PURPOSE: Cambia el estado de los nodos seleccionados (marcados)
  #///////////////////////////////////////////////////////////////////////////
    set lst [$self GetAllChildren]
    foreach l $lst {
      Tolcon_Trace "llamo a ToggleNode con l = $l"
      $self ToggleNode $l 
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method SelectAll {state} {
  # PURPOSE: Marca todos los nodos hoja a un estado
  #
  # PARAMETERS:
  #   state : ON  -> seleccionados
  #           OFF -> no seleccionados 
  #///////////////////////////////////////////////////////////////////////////
    set lst [$self GetAllChildren]
    foreach l $lst {
      $self SetNodeState $l $state
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method GetAllChildren {{id root}} {
  # PURPOSE: Obtenemos todos los nodos hoja o terminales. Esto es, que no
  #          tienen hijos.
  # RETURN: Devuelve una lista con los nodos que no tienen hijos
  #
  #///////////////////////////////////////////////////////////////////////////
    set lst [$self entry children $id]
    if {[llength $lst]} {
      foreach e $lst {
        if {[info exists lstid]} {
          set lstid [concat $lstid [$self GetAllChildren $e]]
        } else {
          set lstid [concat [$self GetAllChildren $e]] 
        }
      }
    } else {
      return $id
    }
    if {[info exists lstid]} {
      return $lstid
    } else {
      return {}
    }
  }

  oncget -bmenu {
    return $self.m
  }

}

package provide btreeview 1.0

#/////////////////////////////////////////////////////////////////////////////
proc test_btreeview {} {
#
# PURPOSE: Funcion de test de un BTreeView. Para poder ejecutarlo hay que
#          situarse en el directorio tolbase/lib
#
#/////////////////////////////////////////////////////////////////////////////
 
 
  # commands attached to the widgets

  proc ReCreateTree { readonly multiselect icosel } {
    destroy .top.tf1.f.btv
    btreeview .top.tf1.f.btv -height 260 -width 500 \
       -breadonly $readonly -bmultiselect $multiselect -bicosel $icosel\
       -bhelptext Size
    #.top.tf1.f.btv bColumnInsert end Type Permissions Owner Group Size
    .top.tf1.f.btv bColumnInsert end Type
    #RellenaBTreeView root [file join [info dir script] testdir]
    #RellenaBTreeView root "./byswidget/btreeview/testdir" [jym: se cambia para que no falle]
    RellenaBTreeView root "c:/UnxUtils"
    grid .top.tf1.f.btv -row 0 -column 0 -sticky new
    grid columnconfigure .top.tf1.f 0 -weight 1
    grid rowconfigure .top.tf1.f 0 -weight 1
  }
  
  proc RellenaBTreeView { idParent path } {
  puts "Path=$path"
    foreach i [ glob $path/* ] {
      set id [ lindex [file split $i] end ]
      if { [ file isdirectory $i ] } {
        set type "directory"
      } else {
        set type "file"
      }
      set atributos [file attributes $i]
      
      #set permissions [lindex $atributos [expr [lsearch -exact $atributos -permissions] + 1]]
      #set owner [lindex $atributos       [expr [lsearch -exact $atributos -owner] + 1]]
      #set group [lindex $atributos       [expr [lsearch -exact $atributos -group] + 1]]
      set size [file size $i]
      #set infCols [ list  Type $type Permissions $permissions Owner $owner Group $group Size $size ]
      set infCols [ list  Type $type Size $size ]
      set childid [ .top.tf1.f.btv insertNode $idParent Group OFF $id "" $infCols]
      if { [ file isdirectory $i ] } {
        RellenaBTreeView $childid $i
      }
    }
   }

  proc ToggleBTreeView {} {
    if { $::cb1 } {
      set readonly true
    } else {
      set readonly false
    } 
    if { $::cb2 } {
      set multiselect true
    } else {
      set multiselect false
    }
    if { $::cb3 } {
      set icosel true
    } else {
      set icosel false
    }
    ReCreateTree $readonly $multiselect $icosel
  }

  # defining the widgets
  
  global toltk_options
  set toltk_options(fonts,Label) {Arial 9}
  
  destroy .top
  toplevel .top
 
  TitleFrame .top.tf1 -text "BTreeView test"
  set f [.top.tf1 getframe]
  #puts "constructor. ant, f=$f"
  #btreeview $f.btv -height 260 -width 500 -bhelptext Size
  #puts "constructor. des"
   #$f.btv bColumnInsert end Type
  #RellenaBTreeView root "./byswidget/btreeview/testdir"
  ReCreateTree false false true

  TitleFrame .top.tf2 -text "Configure"
  checkbutton .top.tf2.f.cb1 -text "Read Only"           -command ToggleBTreeView
  checkbutton .top.tf2.f.cb2 -text "Multiselect"         -command ToggleBTreeView
  checkbutton .top.tf2.f.cb3 -text "Show selection icon" -command ToggleBTreeView
  .top.tf2.f.cb1 deselect
  .top.tf2.f.cb2 deselect
  .top.tf2.f.cb3 select
  

  # packing all 
 
  grid .top.tf1 -row 0 -column 0 -sticky "new" -padx 10 -pady 10
  grid .top.tf2 -row 1 -column 0 -sticky "new" -padx 10 -pady 10
  grid columnconfigure .top 0 -weight 1
  grid rowconfigure .top 0 -weight 0
  grid rowconfigure .top 1 -weight 1
  
  grid .top.tf1.f.btv -row 0 -column 0 -sticky new
  grid columnconfigure .top.tf1.f 0 -weight 1
  grid rowconfigure .top.tf1.f 0 -weight 1

  grid .top.tf2.f.cb1 -row 0 -column 0 -sticky nw
  grid .top.tf2.f.cb2 -row 1 -column 0 -sticky nw
  grid .top.tf2.f.cb3 -row 2 -column 0 -sticky nw
  grid columnconfigure .top.tf2.f 0 -weight 1
}
