
#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::InsertChild { args } {
#
# PURPOSE: Inserts a node in the corresponding wtree of the right panel.
#
# PARAMETERS:
#   args[0] -> grammar
#   args[1] -> name
#   args[2] -> content
#   args[3] -> path
#   args[4] -> desc
#   Si grammar == "Code"
#    args[5] -> ¿es una variable?
#   Si grammar == "Set" || "NameBlock"
#    args[5] -> indexes of the set
#    args[6] -> is file?
#    args[7] -> has subset?
#    args[8] -> subtype
#
#/////////////////////////////////////////////////////////////////////////////
  variable wt_vars
  variable wt_funcs

  set grammar [lindex $args 0]
  set name    [lindex $args 1]
  set content [lindex $args 2]
  set path    [lindex $args 3]
  set desc    [lindex $args 4]

  switch $grammar {
    Code {
      set isvar [lindex $args 5]
      if { [string equal $isvar "1"] } {
        InsertItem $wt_vars $grammar $name $content $path $desc {}
      } else {
        InsertItem $wt_funcs $grammar $name $content $path $desc {}
      }
    }
    NameBlock -
    Set {
      #puts "HTItem, ref = '$ref' '$args'"
      # args[5] is the index of the set object
      # args[6] is the flag file
      # args[8] is the subtype
      InsertItem $wt_vars $grammar $name $content $path $desc [lindex $args 8]
      # [lindex $args 5] [lindex $args 6] [lindex $args 8] $ref
    }
    default {
      InsertItem $wt_vars $grammar $name $content $path $desc {}
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::InsertItem {wt grammar name content path desc subtype} {
#/////////////////////////////////////////////////////////////////////////////
  variable wt_tree
  variable arguments

  #upvar \#0 ::TolTk::Images Images

  set parent_grammar $arguments(parent_grammar)  
  
  #@ Si la llamada a InsertChild viene de SelectSet el argumento tolref 
  #@ no está definido y se crea a partir de parent_tolref
  set tolref $arguments(tolref)
  if { $tolref eq "" } {
    set tolref $arguments(parent_tolref)
    lappend tolref $arguments(index) 
  }
  
  if { $parent_grammar eq "NameBlock" } {
    array set info_member [ FilterNameBlockMember $name ]
    if { !$info_member(-show) } {
      incr arguments(index)
      return
    }
    set fcolor $info_member(-fcolor)
  } else {
    set fcolor "black"
  }
  
  set cont $content
  if {$grammar eq "NameBlock"} {
    # se utiliza la información de instancia
    set classOf [Tol_ClassOfFromReference $tolref]
    if {$classOf ne ""} {
      set insCont [Tol_InstanceContentFromReference $tolref]
      if {$insCont ne ""} {
        set cont $insCont
      } else {
        set cont $classOf
      }
    }
  }

  #@ set brief_cont [TrimContent $grammar $cont]
  #@ set brief_desc [TrimContent DONTCARE $desc]

  #@ Se localiza el icono según la gramática
  if {$grammar eq "Set" || $grammar eq "NameBlock"} {
    if {$grammar eq "Set"} {
      set icon_grammar [SubTypeImage $subtype]
    } else {
      set objAddress [tol::info address $tolref]
      set icon_grammar [::ImageManager::getIconForInstance $objAddress]
    }
  } elseif { $grammar eq "Anything" } {
    set anyType [ tol::info Anything $tolref ]
    set icon_grammar [::Bitmap::get $anyType]
  } else {
    set icon_grammar [::Bitmap::get $grammar]
  }
  
  #@ No se usa el método insert de wtree porque es más lento
  #@ Crea y completa los datos de un nodo
  set idx [$wt item create -button 1 -open 0 -parent root]
  $wt item image $idx Name $icon_grammar
  $wt item text $idx Index $arguments(index) Grammar $grammar \
    Name $name Content $cont Description $desc Path $path Reference $tolref
  if { $fcolor ne "black" } {
    $wt item-color $idx $fcolor
  }
  incr arguments(index)
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::ClearHiertables { } {
#
# PURPOSE:
#
#/////////////////////////////////////////////////////////////////////////////
  variable w_tabset
  variable wt_vars
  variable wt_funcs
  variable OnSelectItem

  $wt_vars selection clear
  $wt_vars item delete all
  $wt_funcs selection clear
  $wt_funcs item delete all
  
  $w_tabset itemconfigure Variables -text [mc Variables] -state normal 
  $w_tabset itemconfigure Functions -state normal

  if { [string length $OnSelectItem] } {
    $OnSelectItem
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::OpenObject { node } {
#
# PURPOSE: Script called each time a node in wt_tree (left wtree) is opened.
#
# PARAMETERS:
#   node : The id of the node opened
#
#/////////////////////////////////////////////////////////////////////////////
  variable wt_tree
  variable arguments
  
  #@ No es necesario llenar ni el nodo root (0) ni root-grammars (1)
  if { $node == 0 } return
  if { $node == 1 } return  

  #@ Argumentos pasados por atributo
  #@  + parent_node es usado por InsertSubset para saber dónde insertar
  set arguments(parent_node) $node
  set tolReference [$wt_tree item text $node Reference]
  
  if { [llength $tolReference] == 1 } {
    #@ Aquí llegan los nodos raíz y las gramáticas
    switch $tolReference {
      Package InsertPackages
      File    InsertFiles
      Console InsertConsoleObj
      default return
    }
  } elseif { [lindex $tolReference 0] == "Grammar" } {
    return
  } else {
    Busy
    #@ Argumentos pasados por atributo
    #@  + parent_grammar es usado por InsertSubset para abrir o no un nodo
    #@  + parent_tolref es usado por InsertSubset para determinar tolref
    set arguments(parent_grammar) [$wt_tree item text $node Grammar]
    set arguments(parent_tolref) $tolReference
    ::tol::forallchild $tolReference ::TolInspector::InsertSubset
    NotBusy
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::InsertPackages {} {
#
# PURPOSE: Inserts a node in wt_tree (left ::wtree) for each TOL packages
#
#/////////////////////////////////////////////////////////////////////////////
  variable wt_tree
  #upvar \#0 ::TolTk::Images Images
  
  set order 1
  foreach pkg [::tol::info packages] {
    # takes an icon with the same name that the grammar     
    set idnew [$wt_tree item create -parent "root-packages" -button yes -open 0 -tags "package-$order"]
    $wt_tree item image $idnew Name [::Bitmap::get NameBlock]
    $wt_tree item text $idnew Index $order Grammar "NameBlock" Name $pkg Reference "NameBlock $pkg"      
    incr order
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::InsertGrammars {} {
#
# PURPOSE: Inserts a node in wt_tree (left ::wtree) for each TOL grammar
#
#/////////////////////////////////////////////////////////////////////////////
  variable wt_tree
  #upvar \#0 ::TolTk::Images Images
  
  set id 1
  foreach g [::tol::info grammars] {
    # takes an icon with the same name that the grammar
    set idnew [$wt_tree item create -parent "root-grammars" -button no -open 0 -tags "grammar-$id"]
    $wt_tree item image $idnew Name [::Bitmap::get $g]
    $wt_tree item text $idnew Index $id Grammar "Grammar" Name $g Reference "Grammar $g"
    incr id
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::InsertFiles { } {
#
# PURPOSE: Inserts a node in wt_tree (left treeview) for each Included file
#
#/////////////////////////////////////////////////////////////////////////////
  variable wt_tree

  set index 1
  #set img [::Bitmap::get "File"]
  foreach f [lreverse [::tol::info included]] {
    set finfo   [::tol::info included $f]
    set button  [lindex $finfo 3]
    set subtype [lindex $finfo 4]
    # tomar la imagen en funcion del subtype
    set img [SubTypeImage $subtype]
    # Tail del fichero. Podría hacerse configurable por el usuario
    set label [file tail $f]
    set idnew [$wt_tree item create -parent "root-files" -button $button -open 0 -tags "file-$index"]
    $wt_tree item image $idnew Name $img
    $wt_tree item text $idnew Index $index Grammar "File" Name $label Reference [list File $f]
    incr index
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::InsertConsoleObj { } {
#
# PURPOSE: Inserts a node in wt_tree (left treeview) for each Console object
#
#/////////////////////////////////////////////////////////////////////////////
  variable wt_tree
    
  set index 1
  foreach co [::tol::console stack list] {
    set grammar [lindex $co 0]
    if {$grammar eq "Set" || $grammar eq "NameBlock"} {
      # SUBTYPE: icono en funcion de [lindex $co 7]
      if {$grammar eq "Set"} {
        set icon [SubTypeImage [lindex $co 7]]
      } else  {
        set objRef [ list "Console" $index ]
        set objAddress [ tol::info address $objRef ]
        set icon [ ::ImageManager::getIconForInstance $objAddress ]
      }
      set name [lindex $co 1]
      set has_button [lindex $co 6]
      if {$name eq ""} {
        if {[set name [TrimContent DONTCARE [lindex $co 4]]] eq ""} {
          set name " "
        }
      }
      set idnew [$wt_tree item create -parent "root-console" -button $has_button -open 0 -tags "console-$index"]
      $wt_tree item image $idnew Name $icon
      $wt_tree item text $idnew Index $index Grammar $grammar Name $name Reference "Console $index"
    }
    incr index
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::InsertSubset { args } {
#
# PURPOSE: Inserts a node in wt_tree (left treeview) for each child of a Set
#
# PARAMETERS:
#   args[0] -> grammar
#   args[1] -> name
#   args[2] -> content
#   args[3] -> path
#   args[4] -> desc
#   Si grammar == "Set"
#    args[5] -> indexes of the set
#    args[6] -> is file?
#    args[7] -> has subset?
#    args[8] -> subtype
#
#/////////////////////////////////////////////////////////////////////////////
  variable wt_tree
  variable arguments
  
  #@ para saber dónde insertar
  set parent_node $arguments(parent_node)
  ## para distinguir set de nameblocks
  set parent_grammar $arguments(parent_grammar)
  #@ para obtener el icono de las instancias
  set parent_tolref $arguments(parent_tolref)
  
  catch {
  #puts "InsertSubset, args=$args"
  set grammar [lindex $args 0]
  if {$grammar eq "Set" || $grammar eq "NameBlock"} {
    #puts "InsertSubset, args=$args"
    if { $parent_grammar eq "NameBlock" } {
      array set info_member [ FilterNameBlockMember [ lindex $args 1 ] ]
      if { !$info_member(-show) } {
        return
      }
      set name $info_member(-name)
      set fcolor $info_member(-fcolor)
    } else {
      set name [ lindex $args 1 ]
      set fcolor "black"
    } 
    set content [lindex $args 2]
    # indices es el indice completo desde el Set root
    set indices     [lindex $args 5]
    set isfile  [lindex $args 6]
    set has_button [lindex $args 7]
    set subtype [lindex $args 8]
    if {$grammar eq "Set"} {
      set icon [SubTypeImage $subtype]
    } else {
      set objRef $parent_tolref
      lappend objRef [ lindex $indices end ]
      set objAddress [ tol::info address $objRef ]
      set icon [ ::ImageManager::getIconForInstance $objAddress ]
    }
    # Tail por si es un fichero. Podría hacerse configurable por el usuario
    set label [file tail $name]
    #puts [list InsertSubset, indices {lindex $args 5} = $indices]
    
    set index [lindex $indices end]
    set tolRefe [eval list [$wt_tree item text $parent_node Reference] $index]
    if {$label eq ""} {
      if {[set label [TrimContent DONTCARE [lindex $args 4]]] eq ""} {
        set label " "
      }
    }

    set idnew [$wt_tree item create -parent $parent_node -button $has_button -open 0]
    $wt_tree item image $idnew Name $icon
    $wt_tree item text $idnew Index $index Grammar $grammar Name $label Reference $tolRefe  
    if { $fcolor ne "black" } {
      $wt_tree item-color $idnew $fcolor
    }
  }
  } msg
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::SelectObject { } {
#
# PURPOSE: Script called each time a node in wt_tree (left wtree) is selected.
#
#/////////////////////////////////////////////////////////////////////////////
  variable w_tabset
  variable wt_tree
  variable arguments
  
  set node [$wt_tree selection get 0]
  if { ![string length $node] } return
  if { $node == 0 } {
    ClearHiertables
    #@D $w_tabset itemconfigure Variables -state disabled
    #@D $w_tabset itemconfigure Functions -state disabled    
    return
  }
  
  set tolReference [$wt_tree item text $node Reference]
  
  Busy
  if { [llength $tolReference] == 1 } {
    #@ Aquí llegan los nodos raíz y las gramáticas
    #@ Argumentos pasados por atributo
    #@  + parent_grammar es usado por InsertItem para saber si es NameBlock  
    set arguments(parent_grammar) ""
    switch $tolReference {
      Package SelectPackageRoot
      File    SelectFileRoot
      Console SelectConsoleRoot
      default {
        ClearHiertables
        #@D $w_tabset itemconfigure Variables -state disabled
        #@D $w_tabset itemconfigure Functions -state disabled
      }
    }
  } elseif { [lindex $tolReference 0] == "Grammar" } {
    #@ Argumentos pasados por atributo
    #@  + parent_grammar es usado por InsertItem para saber si es NameBlock
    set arguments(parent_grammar) ""
    SelectGrammar
  } else {
    #@ Argumentos pasados por atributo
    #@  + parent_grammar es usado por InsertItem para saber si es NameBlock
    set arguments(parent_grammar) [$wt_tree item text $node Grammar]
    SelectSet $tolReference
  }
  NotBusy
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::SelectPackageRoot { } {
#
# PURPOSE: Script called each time 'Packages' node in wt_tree is
#          selected. Gets information about each node that must be in
#          packages, and inserts it in the right wtree.
#
#/////////////////////////////////////////////////////////////////////////////
  variable w_tabset
  variable arguments
  
  #@ Argumentos pasados por atributo
  #@  + index es usado e incrementado por InsertItem
  set arguments(index) 1

  ClearHiertables
  $w_tabset itemconfigure Variables -text [mc Packages]  
  $w_tabset raise Variables  
  #@D $w_tabset itemconfigure Functions -state disabled    
  
  foreach pkg [::tol::info packages] {
    set pi [::tol::info var "NameBlock $pkg"]
    #@ Argumentos pasados por atributo
    #@  + tolref es usado por InsertItem    
    set arguments(tolref) [list "NameBlock" $pkg]
    InsertChild NameBlock $pkg [lindex $pi 2] [lindex $pi 3] [lindex $pi 4] \
      $arguments(index) [lindex $pi 5] [lindex $pi 6] [lindex $pi 7]        
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::SelectFileRoot { } {
#
# PURPOSE: Script called each time 'Included Files' node in wt_tree is
#          selected. Gets information about each node that must be in
#          included files, and inserts it in the right wtree.
#
#/////////////////////////////////////////////////////////////////////////////
  variable w_tabset
  variable arguments
  
  #@ Argumentos pasados por atributo
  #@  + index es usado e incrementado por InsertItem
  set arguments(index) 1

  ClearHiertables
  $w_tabset itemconfigure Variables -text [mc Files]
  $w_tabset raise Variables
  #@D $w_tabset itemconfigure Functions -state disabled
  
  foreach f [lreverse [::tol::info included]] {
    # this catch is needed because info include could return files not really included
    if { [catch { ::tol::info included $f } fi] } continue
    #@ Argumentos pasados por atributo
    #@  + tolref es usado por InsertItem
    set arguments(tolref) [list "File" $f]
    set label [file tail $f]
    InsertChild Set $label [lindex $fi 0] [lindex $fi 2] [lindex $fi 1] \
      $arguments(index) 1 [lindex $fi 3] [lindex $fi 4]        
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::SelectConsoleRoot { } {
#
# PURPOSE: Script called each time 'Console Objects' node in wt_tree is
#          selected. Gets information about each node that must be in console,
#          and inserts it in the right wtree.
#
#/////////////////////////////////////////////////////////////////////////////
  variable wt_tree
  variable arguments
  
  #@ Argumentos pasados por atributo
  #@  + index es usado e incrementado por InsertItem
  set arguments(index) 1

  ClearHiertables

#(pgea) se quita la linea que oculta la columna del indice y que
#(pgea) el procedimimento anterior ClearHiertables muestra
#  $ht_vars column  configure Index -hide yes
  
  foreach co [::tol::console stack list] {
    #puts "SelectConsoleRoot: co = $co"
    set gra  [lindex $co 0]
    set name [lindex $co 1]
    set cont [lindex $co 2]
    set path [lindex $co 3]
    set desc [lindex $co 4]
    #@ Argumentos pasados por atributo
    #@  + tolref es usado por InsertItem
    set arguments(tolref) [list Console $arguments(index)]
    switch [lindex $co 0 ] {
      Code {
        InsertChild $gra $name $cont $path $desc 0
      }
      NameBlock -
      Set {
        #puts "switch: SelectConsoleRoot $gra"
        InsertChild $gra $name $cont $path $desc \
            $arguments(index) [lindex $co 5] [lindex $co 6] [lindex $co 7]
      }
      default {
        InsertChild $gra $name $cont $path $desc
      }
    }
  }
  set arguments(tolref) ""
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::SelectGrammar { } {
#
# PURPOSE: Script called each time a Grammar node in wt_tree is selected. Gets
#          information about each var and function of this grammar, and
#          inserts them in the right treeview.
#
#/////////////////////////////////////////////////////////////////////////////
  variable w_tabset
  variable wt_tree
  variable OnSelectItem
  variable arguments
  
  #@ Argumentos pasados por atributo
  #@  + index es usado e incrementado por InsertItem  
  set arguments(index) 1
  
  ClearHiertables
  
  set selAnchor [$wt_tree selection get 0]
  
  set grammar [$wt_tree item text $selAnchor Name ]
  if { [string compare $grammar "Anything"] } {
    #@ Por aquí si no es Anything
    if { [string length $OnSelectItem] } {
      $OnSelectItem [::tol::info grammars $grammar]
    }
    foreach var [::tol::info variables $grammar] {
      if {[catch {
        set vinfo [::tol::info variables [list $grammar $var]]
        #@ Devuelve:
        #@ Grammar Name Content Path Description [IsFile HasSubset ? Subtype?]
      } msg]} {
        Tol_HciWriter "<E>in ::TolInspector::SelectGrammar : $msg\n</E>"
        continue
      }
      #@ Argumentos pasados por atributo
      #@  + tolref es usado por InsertItem
      set arguments(tolref) [list $grammar $var]
      #@ InsertChild grammar name content path desc [Set|Nameblock: index isfile hassubset subtype] [Code: isvariable]
      if { [string equal $grammar "Code"] } {
        InsertChild $grammar $var [lindex $vinfo 2] [lindex $vinfo 3] [lindex $vinfo 4] 1  
      } elseif { $grammar eq "Set" || $grammar eq "NameBlock" } {
        InsertChild $grammar $var [lindex $vinfo 2] [lindex $vinfo 3] [lindex $vinfo 4] \
          $arguments(index) [lindex $vinfo 5] [lindex $vinfo 6] [lindex $vinfo 7]
      } else {
        InsertChild $grammar $var [lindex $vinfo 2] [lindex $vinfo 3] [lindex $vinfo 4]
      }  
    }
  } else {
    $w_tabset raise Functions  
    #@D $w_tabset itemconfigure Variables -state disabled
  }
  foreach fun  [::tol::info functions $grammar] {
    set finfo [::tol::info functions $grammar $fun]
    #@ Argumentos pasados por atributo
    #@  + tolref es usado por InsertItem   
    set arguments(tolref) [list Code $fun]
    InsertChild Code $fun [lindex $finfo 0] [lindex $finfo 1] [lindex $finfo 2] 0    
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::SelectSet { tolReference } {
#
# PURPOSE: Script called each time a Set node in wt_tree is selected. Gets
#          information about each child of this set, and inserts it in the
#          right treeview.
#
#///////////////////////////////////////////////////////////////////////////// 
  variable var_id 1
  variable fun_id 1
  variable arguments
  
  ClearHiertables
  
  #@ Argumentos pasados por atributo
  #@  + index es usado e incrementado por InsertItem
  #@  + tolref indica a InsertItem que se ha de obtener desde parent_tolref
  #@  + parent_tolref es usado por InsertItem para encontrar tolref  
  set arguments(index) 1  
  set arguments(tolref) ""
  set arguments(parent_tolref) $tolReference 
  ::tol::forallchild $tolReference ::TolInspector::InsertChild
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::CloseObject { node } {
#
# PURPOSE: Script called each time a node in wt_tree (left treeview) is closed.
#
# PARAMETERS:
#   node : The id of the node closed
#
#/////////////////////////////////////////////////////////////////////////////
  variable wt_tree
  
  #@ No es necesario vaciar ni el nodo root (0) ni root-grammars (1)
  if { $node == 0 } return
  if { $node == 1 } return
  foreach it [$wt_tree item children $node] {
    $wt_tree item delete $it
  }   
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::OpenVariable { node } {
#
# PURPOSE: Script called each time a node in wt_vars (right wtree) is opened.
#
# PARAMETERS:
#   node : The id of the node opened
#
#/////////////////////////////////////////////////////////////////////////////
  variable wt_tree
  variable wt_vars

  if { ![string length $node] } { return }
  
  set treeParent [$wt_tree selection get 0]
  set treeParent_tolRef [$wt_tree item text $treeParent Reference]
  if { [llength $treeParent_tolRef] < 1 } return
  if { [lindex $treeParent_tolRef 0] eq "Grammar" } return  
  
  set treeNode -1

  set grammar [$wt_vars item text $node Grammar]
  if {$grammar eq "Set" || $grammar eq "NameBlock"} {
    $wt_tree item expand $treeParent
    #@ Se localiza el nodo seleccionado en el árbol (wt_tree)
    set index [$wt_vars item text $node Index]   
    foreach node_i [$wt_tree item children $treeParent] {
      set index_i [$wt_tree item text $node_i Index]
      if { [string equal $index $index_i] } {
        set treeNode $node_i
        break
      }
    }
  } else {
    return
  }
  
  if { $treeNode != -1 } {
    update
    $wt_tree selection clear    
    $wt_tree selection add $treeNode
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::SelectItem { wt } {
#
# PURPOSE: Procedure that is executed when the user selects an element of
#          the trees of variables or functions.
#          First it take all the arguments necessary of the node of tree to
#          execute the procedure stored in the variable OnSelectItem, and
#          later executes it passing those arguments.
#
# PARAMETERS: path of wt_* (::wtree of variables or of functions)
#
#/////////////////////////////////////////////////////////////////////////////
# variable w_text       ;# TODO: ????
  variable OnSelectItem ;#Command to invoke on SelectItem in wt_*
 
  #@ entry era el 'selection anchor'
  set entry [$wt selection get end] ;# id of entry selected

  if { [string length $OnSelectItem] } {
    # there are associated command
    if { [string length $entry] } {
      # almost one entry of wt_* selected
      set name   [$wt item text $entry Name] ;# name of item     
      set itemid [$wt item text $entry Index] ;# id of item        
      set path   [$wt item text $entry Path] ;# path of file of item
      set objRef [$wt item text $entry Reference] ;# info of item
      set value  [$wt item text $entry Content]
      if { [string equal \"\n\" $value] } {
        # hasn't value        
        set value {"\n"}
      } else {
        set value [string trim $value]
      }
      set desc [$wt item text $entry Description]
      set grammar [$wt item text $entry Grammar]
      set icon [$wt item image $entry Name]
      #(pgea) lista de argumentos que recibe ::TolConsole::OnInfo
      set args [list $icon $grammar $name $value $path $desc $objRef]
    } else {
      set args ""
    }
    eval "$OnSelectItem $args"
  }
  return
}