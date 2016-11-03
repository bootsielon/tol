
#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::_InsertChild { args } {
#
# PURPOSE: este proc es para ancapsular desc en una lista
#
#/////////////////////////////////////////////////////////////////////////////
    eval InsertChild [lrange $args 0 3] [list [list [lindex $args 4]]] [lrange $args 5 end]
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::InsertChild { args } {
#
# PURPOSE: Inserts a node in the corresponding treeview of the right panel.
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
#    args[end] -> "Reference {reference}" end > 4
#
#/////////////////////////////////////////////////////////////////////////////
  variable ht_vars
  variable blt_vartree
  variable ht_funcs
  variable blt_functree
  variable gra_parent

  #puts "InsertChild: args=$args [info level 1]"
  set grammar [lindex $args 0]
  set name [ lindex $args 1 ]
  set content [lindex $args 2]
  set path    [lindex $args 3]
  set desc    [lindex [lindex $args 4] 0]


  if {[llength [lindex $args end]] >= 2} {
    #set ref [list [lindex [lindex $args end] 0] [lrange [lindex $args end] 1 end]]
    set ref [concat [lindex [lindex $args end] 0] [lrange [lindex $args end] 1 end]]
  } else {
    set ref ""      
  }
  #puts "InsertChild: ref=$ref"
  switch $grammar {
    Code {
      set isvar [lindex $args 5]
      if { [string equal $isvar "1"] } {
        Insert_HTItem $ht_vars $blt_vartree $grammar $name $content $path $desc $ref
      } else {
        Insert_HTItem $ht_funcs $blt_functree $grammar $name $content $path $desc $ref
      }
    }
    NameBlock -
    Set {
      #puts "HTItem, ref = '$ref' '$args'"
      # args[5] is the index of the set object
      # args[6] is the flag file
      # args[8] is the subtype
      Insert_HTItem $ht_vars $blt_vartree $grammar $name $content $path $desc \
        [lindex $args 5] [lindex $args 6] [lindex $args 8] $ref
    }
    default {
      Insert_HTItem $ht_vars $blt_vartree $grammar $name $content $path $desc $ref
    }
  }
}

# si grammar == "Set"
#   args[0] -> indexes
#   args[1] -> ¿is a file?
#   args[2] -> subtype

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::Insert_HTItem {ht tree grammar name content path desc args} {
#/////////////////////////////////////////////////////////////////////////////
  variable ht_tree
  variable tolset
  variable tolindex
  variable iterSet
  variable knownReference
  variable node_prefix
  variable item_data
  variable item_id
  variable gra_parent

  #upvar \#0 ::TolTk::Images Images

  #puts "ENTER: ::TolInspector::Insert_HTItem"
  if { $knownReference eq "" } {
    set objRef $iterSet
    lappend objRef $item_id
  } else {
    #puts "knownReference = $knownReference"
    set objRef $knownReference
  }

  #puts "\tobjRef = $objRef"
  if { $gra_parent eq "NameBlock" } {
    array set info_member [ FilterNameBlockMember $name ]
    if { !$info_member(-show) } {
      incr item_id
      return
    }
    set fcolor $info_member(-fcolor)
  } else {
    set fcolor "black"
  }

  set brief_cont [TrimContent $grammar $content]
  set brief_desc [TrimContent DONTCARE $desc]
  #puts "Insert_HTItem: args=$args"
  if {[llength [lindex $args end]] >= 2} {
    set ref_value [lindex [lindex $args end] 1]
  } elseif {[regexp -- "console" $node_prefix]} {
    set ref_value [list Console]
    eval lappend ref_value $tolindex $item_id
  } else {
    set ref_value ""
  }

  #puts "Insert_HTItem: ref_value=$ref_value tolindex=$tolindex item_id=$item_id node_prefix=$node_prefix"
  #puts "\ttolset=$tolset"
  set data [list Index      $item_id \
                Content     $brief_cont \
                Path        $path \
                Description $brief_desc \
                Reference   $ref_value]

  if {$grammar eq "Set" || $grammar eq "NameBlock"} {
    if {[string length $tolset] || [regexp "console" $node_prefix]} {
      # OJO: si tolset == "" no implica que no estemos en un Set
      # por ejemplo un Set de la consola.
      set item [GetItemName $node_prefix [lindex $args 0]]
      #Tolcon_Trace "InsertHTItem. item(if): $item"
    } else {
      switch [lindex [$ht_tree get -full anchor] end] {
        root-files {
          set item file$item_id
        }
        root-console {
          set item console$item_id
        }
        root-pool {
          set item pool$item_id
        }
        default {
          set item item$item_id
        }
      }
    }
    # aqui hay que pedir el bitmap en funcion del subtype: args[2]
    if {$grammar eq "Set"} {
      #puts "*******Set $objRef *******"
      set icon_grammar [SubTypeImage [lindex $args 2]]
      set data [lreplace $data 9 9 $objRef]  
    } else {
      #(pgea) se reemplaza el valor de la referencia en data
      set data [lreplace $data 9 9 $objRef]  
      #(pgea) se utiliza la informacon de instancia
      #puts "Tol_ClassOfFromReference $objRef"
      set classOf [Tol_ClassOfFromReference $objRef]
      #puts "Tol_InstanceContentFromReference $objRef"
      set insCont [Tol_InstanceContentFromReference $objRef]
      if {$insCont ne ""} {
        set data [lreplace $data 3 3 [TrimContent $grammar $insCont]]
      } elseif {$classOf ne ""} {
        set data [lreplace $data 3 3 $classOf]
      }
      set objAddress [ tol::info address $objRef ]
      set icon_grammar [ ::ImageManager::getIconForInstance $objAddress ]
    }
  } elseif { $grammar eq "Anything" } {
    set item "item$item_id"
    set anyType [ tol::info anything $objRef ]
    set icon_grammar [ ::Bitmap::get $anyType ]
  } else {
    set item "item$item_id"
    set icon_grammar [ ::Bitmap::get $grammar ]
  }
  set idnew [$tree insert 0 -label $item]
  set label [OneLine $name]
  if {$label eq ""} {
    if {[set label $brief_desc] eq ""} {
      set label " "
    }
  }
  $ht entry configure $idnew -label $label \
      -icons [list $icon_grammar $icon_grammar] -data $data \
       -foreground $fcolor
  set item_data($item_id) [list $content $desc $grammar]
  incr item_id
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::ClearHiertables { } {
#
# PURPOSE:
#
#/////////////////////////////////////////////////////////////////////////////
  variable w_tabset
  variable ht_vars
  variable ht_funcs
  variable item_data
  variable OnSelectItem

  $ht_vars  entry delete root 0 end
  $ht_funcs entry delete root 0 end
  $ht_vars column  configure Index -hide no
  $ht_funcs column configure Index -hide no
  catch {
    unset item_data
  }
  $w_tabset tab configure Variables -text [mc Variables] -state normal
  $w_tabset tab configure Functions -state normal

  if { [string length $OnSelectItem] } {
    $OnSelectItem
  }
}

# este proc debe pasar a ser un getContainerReference
#
proc ::TolInspector::getPackageReference { treePath } {
  variable ht_tree

  #puts "::TolInspector::getPackageReference $treePath"
  catch {
    set typeContainer "NameBlock"
    set order [ lindex  [ split [ lindex $treePath 2 ] "-" ] end ]
    incr order -1
    #puts "path,2 = [ lindex $treePath 2 ]"
    #puts "order = $order"
    set idx [lindex [ $ht_tree entry children "root-packages"] $order]
    set nameContainer [ $ht_tree entry cget $idx -label ]
    #puts "en root-package:"
    #puts "\torder=$order"
    #puts "\tidx=$idx"
    #puts "\tnameContainer=$nameContainer"
    set container [ list $typeContainer $nameContainer ]
    if { [ llength $treePath ] > 3 } {
      foreach c [ lrange $treePath 3 end-1 ] {
        set orderItem [ lindex  [ split $c "-" ] end ]
        lappend container $orderItem
      }
      set tolindex [ lindex  [ split [ lindex $treePath end ] "-" ] end ]
    } else {
      set tolindex [ list ]
    }
  } msg
  #puts "::TolInspector::getPackageReference: $msg"
  #puts "::TolInspector::getPackageReference: [ list $container $tolindex ]"
  return [ list $container $tolindex ]
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::OpenObject { index } {
#
# PURPOSE: Script called each time a node in ht_tree (left treeview) is
#          opened.
#
# PARAMETERS:
#   index : The id of the node opened
#
#/////////////////////////////////////////////////////////////////////////////
  variable tolset
  variable tolindex
  variable iterSet
  variable at_set
  variable node_prefix
  variable ht_tree
  variable data

  #puts "OpenObject: index=$index"
  set path [$ht_tree get -full $index] 
  #puts "path = $path"
  if { $index } {
    set parent1 [lindex $path 1]
    if { [set ispackage [string equal $parent1 "root-packages"]] ||
         [set isfile [string equal $parent1 "root-files"]] ||
         [set isconsole [string equal $parent1 "root-console"]] ||
         [string equal $parent1 "root-pool"] } {
      set lpath [llength $path]
      #puts "lpath = $lpath"
      if { $lpath == 2 } {
        if { $ispackage } {
          InsertPackages
        } elseif { $isfile } {
          InsertFiles
        } elseif { $isconsole } {
          InsertConsoleObj
        } else {
          InsertPoolObj
        }
      } else {
        #puts "OpenObject: path = $path"
        set leaf [lindex $path end]
        set leaf_info [split $leaf "-"]
        set node_prefix [lindex $leaf_info 0]
        set _tolset $tolset
        if { $parent1 eq "root-packages" } {
          foreach {tolset tolindex} [ getPackageReference $path ] break
        } elseif { [string equal $parent1 "root-pool"] } {
          #Tolcon_Trace "OpenObject. tolset=$tolset"
          set tolset $data(pool,reference,[lindex $leaf_info 0])
          #puts "OpenObject: Spool: tolset=$tolset, leaf_info=$leaf_info"
          if { $lpath == 3 } {
            set tolindex [lrange $tolset 1 end]
            set tolset [lindex $tolset 0]
          } else {
            set tolindex [lrange $leaf_info 1 end]
		    if {[lindex $tolset 0] eq "Console"} {
		      set tolset [lindex $tolset 0]  
		    } else {
		      set tolset [lrange $tolset 0 1]
		    }
            #set tolindex [lrange $tolset 1 end]
            #set tolset [lindex $tolset 0]
            #lappend tolindex [lindex $leaf_info end]
          }
          #Tolcon_Trace "OpenObject. tolset=$tolset"
        } elseif {[regexp -- "console(.+)" $node_prefix --> idx_con]} {
          set tolset "Console"
          #puts "OpenObject: Console: leaf_info = $leaf_info"
          if {[llength $leaf_info] == 1} {
            set tolindex $idx_con
          } else {
            #set tolindex [list $idx_con [lrange $leaf_info 1 end]]
            set tolindex [lrange $leaf_info 1 end]
          }
        } else {
          set idx [$ht_tree find -name $node_prefix]
          array set aryData [$ht_tree entry cget $idx -data]
          set tolset [list File $aryData(FileName)]
          #set tolset [$ht_tree entry cget [$ht_tree find -name $node_prefix] -label]
          if { $lpath == 3 } {
            set tolindex ""
          } else {
            set tolindex [lrange $leaf_info 1 end]
          }
        }
        Busy
        set at_set $index
        set iterSet [eval list $tolset $tolindex]
        variable gra_parent [ GetParentGrammar $iterSet ]
        ::tol::forallchild $iterSet ::TolInspector::InsertSubset
        set tolset $_tolset
        NotBusy
      }
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::InsertPackages {} {
#
# PURPOSE: Inserts a node in ht_tree (left ::blt::treeview) for each TOL
#          packages
#
#/////////////////////////////////////////////////////////////////////////////
  variable ht_tree
  #upvar \#0 ::TolTk::Images Images
    
  set order 1
  foreach pkg [::tol::info packages] {
    # takes an icon with the same name that the grammar
    set img [ ::Bitmap::get NameBlock ]
    $ht_tree insert -at "root-packages" end package-$order -label $pkg\
          -icons [list $img $img] -button yes
    incr order
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::InsertGrammars {} {
#
# PURPOSE: Inserts a node in ht_tree (left ::blt::treeview) for each TOL
#          grammar
#
#/////////////////////////////////////////////////////////////////////////////
  variable ht_tree
  #upvar \#0 ::TolTk::Images Images
    
  set id 1
  foreach g [::tol::info grammars] {
    # takes an icon with the same name that the grammar
    set img [::Bitmap::get $g]
    $ht_tree insert -at "root-grammars" end grammar-$id -label $g\
          -icons [list $img $img]
    incr id
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::InsertFiles { } {
#
# PURPOSE: Inserts a node in ht_tree (left treeview) for each Included file
#
#/////////////////////////////////////////////////////////////////////////////
  variable ht_tree
  variable blt_tree

  set index 1
  #set img [::Bitmap::get "File"]
  foreach f [lreverse [::tol::info included]] {
    set finfo   [::tol::info included $f]
    set button  [lindex $finfo 3]
    set subtype [lindex $finfo 4]
    # tomar la imagen en funcion del subtype
    set img [SubTypeImage $subtype]
    set tag_file "file$index"
    set blt_idx [$blt_tree insert "root-files" \
             -label $tag_file -tags $tag_file]
    # Tail del fichero. Podría hacerse configurable por el usuario
    set label [file tail $f]
    $ht_tree entry configure $blt_idx -label $label\
    -icons  [list $img $img] -button $button -data [list "FileName" $f]
    incr index
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::InsertConsoleObj { } {
#
# PURPOSE: Inserts a node in ht_tree (left treeview) for each Console object
#
#/////////////////////////////////////////////////////////////////////////////
  variable ht_tree
  variable has_button 0
    
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
      #tol::forallchild $name HasSubset
      set has_button [lindex $co 6]
      if {$name eq ""} {
        if {[set name [TrimContent DONTCARE [lindex $co 4]]] eq ""} {
          set name " "
        }
      }
      $ht_tree insert -at root-console end "console$index" -label $name \
          -icons  [list $icon $icon] -button $has_button -data [list "FileName" $index]
    }
    incr index
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::InsertPoolObj { } {
#
# PURPOSE: Inserts a node in ht_tree (left treeview) for each object in Spool
#
#/////////////////////////////////////////////////////////////////////////////
  variable data
  variable ht_tree
  array unset data pool,reference,*
  set index 1

  foreach _obj $data(pool,objects) {
    #Tolcon_Trace "InsertPoolObj. _obj=$_obj"
    array set obj $_obj
    if { [string equal $obj(grammar) "Set"] || [string equal $obj(grammar) "NameBlock"] } {
      #Si la referencia del objeto tiene espacios en la ruta, el 
      # llength me daría > 1, por eso añado la 2ª parte de la condicion
      if { ([llength $obj(reference)] > 1) && \
            ![string equal $obj(reference) $obj(objName)]} {
        set infoObj [::tol::info variable $obj(reference)]
      } else {
        if {[catch { set infoObj [concat [list "Set" $obj(reference)] \
                            [::tol::info included $obj(reference)]] }]} {
          set infoObj [concat [list "Set" $obj(reference)] \
                 [::tol::info variable "Set" $obj(reference)]]
        } else {
          # Insertamos el 1 de isFile
          set infoObj [linsert $infoObj 5 1]
        }
      }
      #Tolcon_Trace "infoObj=$infoObj"
      if {[set name [file tail [lindex $infoObj 1]]] eq ""} {
        if {[set name [TrimContent DONTCARE [lindex $infoObj 4]]] eq ""} {
          set name " "
        }
      }
      set has_button [lindex $infoObj 6]
      #set icon [SubTypeImage [lindex $infoObj 7]]
      if {$obj(grammar) eq "Set"} {
        set icon [SubTypeImage [lindex $infoObj 7]]
      } else  {
        #puts "en pool = $obj(reference)"
        set objAddress [ tol::info address $objRef ]
        set icon [ ::ImageManager::getIconForInstance $objAddress ]
      }

      $ht_tree insert -at root-pool end "pool$index" -label $name \
          -icon [list $icon $icon] -button $has_button
      #Tolcon_Trace "InsertPoolObj. name=$name"
      if { ([llength $obj(reference)] > 1) && \
            [string equal $obj(reference) $obj(objName)]} {
        set obj(reference) [list $obj(reference)]
      }
      set data(pool,reference,pool$index) $obj(reference)
    }
    incr index
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::InsertSubset { args } {
#
# PURPOSE: Inserts a node in ht_tree (left treeview) for each child of a Set
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
  variable ht_tree
  variable at_set
  variable tolset
  variable iterSet
  variable node_prefix
  variable has_button 0
  variable blt_tree
  variable gra_parent
  catch {
  #puts "InsertSubset, args=$args"
  set grammar [lindex $args 0]
  if {$grammar eq "Set" || $grammar eq "NameBlock"} {
    #puts "InsertSubset, args=$args"
    if { $gra_parent eq "NameBlock" } {
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
    # idx es el indice completo desde el Set root
    set idx     [lindex $args 5]
    set isfile  [lindex $args 6]
    set has_button [lindex $args 7]
    set subtype [lindex $args 8]
    #tol::forallchild $tolset $idx HasSubset
    if {$grammar eq "Set"} {
      set icon [SubTypeImage $subtype]
    } else {
      set objRef $iterSet
      lappend objRef [ lindex $idx end ]
      set objAddress [ tol::info address $objRef ]
      set icon [ ::ImageManager::getIconForInstance $objAddress ]
    }
    # Tail por si es un fichero. Podría hacerse configurable por el usuario
    set label [file tail $name]
    #puts [list InsertSubset, idx {lindex $args 5} = $idx]
    set idnew [ $blt_tree insert $at_set \
                   -label [ GetItemName $node_prefix $idx ] ]

    #puts "InsertSubset, node_prefix = $node_prefix"
    #puts "InsertSubset, idnew = $idnew"
    #puts "InsertSubset, -label = [GetItemName $node_prefix $idx]"

    if {$label eq ""} {
      if {[set label [TrimContent DONTCARE [lindex $args 4]]] eq ""} {
        set label " "
      }
    }
    $ht_tree entry configure $idnew -label $label -button $has_button \
      -icons [list $icon $icon] -foreground $fcolor 
  }
} msg
#puts "InsertSubSet: $msg"
}



#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::SelectObject { } {
#
# PURPOSE: Script called each time a node in ht_tree (left treeview) is
#          selected.
#
#/////////////////////////////////////////////////////////////////////////////

  #Tolcon_Trace "LLAMADA A SELECTOBJECT"
  variable w_tabset
  variable ht_tree
  #variable set_type ""
  variable tolset
  variable tolindex
  #variable closing
  variable node_prefix ""
  variable data
  variable knownReference ""

  set index [$ht_tree index anchor]
    
  if { ![string length $index] } {
    ClearHiertables
    return
  }
  ClearHiertables

  set path [ $ht_tree get -full $index ] 
  set tolset ""
  if { $index } {
    #puts "SelectObject index = $index"
    #puts "SelectObject: full = $path"
    set info_node [split [lindex $path end] "-"] ;# ojo si el path tiene -
    set node_type [lindex $info_node 0]
    set node_name [lindex $info_node 1]
    #puts "SelectObject: info_node = $info_node"
  } else {
    set node_type root
    set node_name root
  }

  Busy
  #puts "SelectObject: node_type = $node_type, node_name = $node_name"
  switch -regexp $node_type  {
    root {
      switch $node_name {
        root -
        grammars {
          ClearHiertables
          #$w_tabset tab configure Variables -state disabled
          #$w_tabset tab configure Functions -state disabled
        }
        package {
          puts "implementa el llenado del panel derecho"
        }
        files {
          SelectFileRoot
        }
        console {
          SelectConsoleRoot
        }
        pool {
          SelectPoolRoot
        }
      }
    }
    grammar {
      SelectGrammar
    }
    default {
      set node_prefix $node_type
      #puts "SelectObject: node_prefix = $node_prefix"
      if { $node_type eq "package" } {
        foreach {tolset tolindex} [ getPackageReference $path ] break
      } elseif {[regexp -- "pool" $node_type]} {
        set tolset $data(pool,reference,[lindex $node_type 0])
        #puts "SelectObject: Spool: tolset=$tolset, info_node=$info_node"
        if { [llength $info_node] == 1 } {
          set tolindex [lrange $tolset 1 end]
          set tolset [lindex $tolset 0]
        } else {
          #set tolindex [lrange $tolset 1 end]
          #set tolset [lindex $tolset 0]
          #lappend tolindex [lindex $info_node end]
          set tolindex [lrange $info_node 1 end]
          if {[lindex $tolset 0] eq "Console"} {
            set tolset [lindex $tolset 0]  
          } else {
            set tolset [lrange $tolset 0 1]
          }
        }
      } elseif {[regexp -- "console(.+)" $node_type --> idx_con]} {
        #puts "SelectObject: Console: tolset=$tolset info_node=$info_node idx_con=$idx_con"
        set tolset "Console"
        #set idx [$ht_tree find -name $node_type]
        #array set aryData [$ht_tree entry cget $idx -data]
        #set tolindex [eval list $aryData(FileName) [lrange $info_node 1 end]]
        if {[llength $info_node] == 1} {
          set tolindex $idx_con
        } else {
          set tolindex [lrange $info_node 1 end]
          #set tolindex [concat [list $idx_con] $tolindex]
        }
      } else {
        set idx [$ht_tree find -name $node_type]
        array set aryData [$ht_tree entry cget $idx -data]
        #puts "SelectObject: FileName=$aryData(FileName) info_node=$info_node"
        set tolset [list File $aryData(FileName)]
        set tolindex [lrange $info_node 1 end]
        #set tolindex [lrange $info_node 2 end]
      }
      #puts "SelectObject end: tolset=$tolset, tolindex=$tolindex"
      SelectSet
    }
  }
  NotBusy
}


#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::SelectFileRoot { } {
#
# PURPOSE: Script called each time 'Included Files' node in ht_tree is
#          selected. Gets information about each node that must be in
#          included files, and inserts it in the right treeview.
#
#/////////////////////////////////////////////////////////////////////////////
  variable item_id 1
  variable w_tabset
  variable knownReference ""

  ClearHiertables
  $w_tabset tab configure Variables -text [mc Files]
  #  $w_tabset tab configure Functions -state disabled
  $w_tabset select 0
  foreach f [lreverse [::tol::info included]] {
    # this catch is needed because info include could return files not
    # realy included.
    if { [ catch { ::tol::info included $f } fi ] } continue
    # tener en cuenta el subtype cuando lo retorne
    set knownReference [ list "File" $f ] 
    InsertChild Set $f \
        [lindex $fi 0] [lindex $fi 1] [list [lindex $fi 2]] $item_id 1 [lindex $fi 3] [lindex $fi 4] 
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::SelectConsoleRoot { } {
#
# PURPOSE: Script called each time 'Console Objects' node in ht_tree is
#          selected. Gets information about each node that must be in console,
#          and inserts it in the right treeview.
#
#/////////////////////////////////////////////////////////////////////////////
  variable w_tabset
  variable ht_vars
  variable ht_funcs
  variable item_id 1
  variable iterSet "Console"
  variable knownReference ""

  ClearHiertables
#(pgea) se quita la linea que oculta la columna del indice y que
#(pgea) el procedimimento anterior ClearHiertables muestra
#  $ht_vars column  configure Index -hide yes
#  $w_tabset tab configure Functions -state disabled
  
  foreach co [::tol::console stack list] {
   #puts "SelectConsoleRoot: co = $co"
   set gra  [lindex $co 0]
   set name [lindex $co 1]
   set cont [lindex $co 2]
   set path [lindex $co 3]
   set desc [lindex $co 4]
   set knownReference [ list Console $item_id ]
    switch [lindex $co 0 ] {
      Code {
        InsertChild $gra $name $cont $path [list $desc] 0
      }
      NameBlock -
      Set {
        #puts "switch: SelectConsoleRoot $gra"
        InsertChild $gra $name $cont $path [list $desc] \
            $item_id [lindex $co 5] [lindex $co 6] [lindex $co 7] \
            [list Reference [list "Console" $item_id]]
      }
      default {
        InsertChild $gra $name $cont $path [list $desc] \
            [list Reference [list "Console" $item_id]]
      }
    }
  }
  set knownReference ""
}


#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::SelectPoolRoot { } {
#
# PURPOSE: Script called each time 'Spool Objects' node in ht_tree is
#          selected. Gets information about each node that must be in pool,
#          and inserts it in the right treeview.
# 
#/////////////////////////////////////////////////////////////////////////////
  variable w_tabset
  variable ht_vars
  variable data
  variable item_id 1
  variable knownReference ""

  ClearHiertables
  $ht_vars column  configure Index -hide yes

  array unset data pool,variables,*
  # Para cada uno de los objetos que debe haber en el spool
  # busco la información del nodo y lo inserto en el treeview de la derecha
  foreach _obj $data(pool,objects) {
    array set obj $_obj
    #Si la referencia del objeto tiene espacios en la ruta, el
    # llength me daría > 1, por eso añado la 2ª parte de la condicion
    #puts "SelectPoolRoot: obj(grammar)=$obj(grammar) obj(objName)=$obj(objName) obj(reference)=$obj(reference)"
    if { ([llength $obj(reference)] > 1) && ![string equal $obj(reference) $obj(objName)]} {
      if {$obj(grammar) eq "Code"} {
        set infoObj [concat [list $obj(grammar) $obj(objName)] \
            [::tol::info function $obj(reference)]]
      } else  {
        set infoObj [::tol::info variable $obj(reference)]
      }
      #Tolcon_Trace "tol info (var|function) reference"
    } else {
      if {[catch { set infoObj [concat [list "Set" $obj(reference)] \
                          [::tol::info included $obj(reference)]] }]} {
        if {$obj(grammar) eq "Code"} {
          set infoObj [concat [list $obj(grammar) $obj(reference)] \
               [::tol::info function $obj(graApply) $obj(reference)]]
        } else  {
          #set infoObj [concat [list $obj(grammar) $obj(reference)] \
          #     [::tol::info variable $obj(grammar) $obj(reference)]]
          set infoObj [::tol::info variable [list $obj(grammar) $obj(reference)]]
        }
        #Tolcon_Trace "tol info (var|function) Grammar nameGlobal"
      } else {
        # Insertamos el 1 de isFile
        set infoObj [linsert $infoObj 5 1]
        #Tolcon_Trace "tol info included"
      }
    }
    #Tolcon_Trace "INFOOBJ: $infoObj"
    set gra  [lindex $infoObj 0]
    set name [lindex $infoObj 1]
    set cont [lindex $infoObj 2]
    set path [lindex $infoObj 3]
    set desc [lindex $infoObj 4]
    set data(pool,variables,$item_id) $obj(reference)

    switch $gra {
      Code {
        InsertChild $gra $name $cont $path [list $desc] 0 "Reference $obj(reference)"
      }
      Set -
      NameBlock {
        set knownReference $obj(reference)
        InsertChild $gra $name $cont $path [list $desc] \
            $item_id [lindex $infoObj 5] [lindex $infoObj 6] [lindex $infoObj 7] "Reference $obj(reference)"
        set knownReference ""
      }
      default {
        InsertChild $gra $name $cont $path [list $desc] "Reference $obj(reference)"
      }
    }
  }  
}


#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::SelectGrammar { } {
#
# PURPOSE: Script called each time a Grammar node in ht_tree is selected. Gets
#          information about each var and function of this grammar, and
#          inserts them in the right treeview.
#
#/////////////////////////////////////////////////////////////////////////////
  variable w_tabset
  variable ht_tree
  variable ht_vars
  variable ht_funcs
  variable item_id 1
  variable OnSelectItem
  variable knownReference

  set knownReference ""
  ClearHiertables
  $ht_vars column  configure Index -hide yes
  $ht_funcs column configure Index -hide yes
  set grammar [$ht_tree entry cget anchor -label]
  if { [string compare $grammar "Anything"] } {
    if { [string length $OnSelectItem] } {
      $OnSelectItem [::tol::info grammars $grammar]
    }
    foreach var  [::tol::info variables $grammar] {
      if {[catch {
        set vinfo [::tol::info variables [list $grammar $var]]
      } msg]} {
        Tol_HciWriter "<E>in ::TolInspector::SelectGrammar : $msg\n</E>"
        continue
      }
      if { [string equal $grammar "Code"] } {
        #Tolcon_Trace "Code : -- $vinfo --"
        InsertChild $grammar $var [lindex $vinfo 2] [lindex $vinfo 3] [list [lindex $vinfo 4]] 1  
      } elseif { $grammar eq "Set" || $grammar eq "NameBlock" } {
        #Tolcon_Trace "Set : -- $vinfo --"
        set knownReference [ list $grammar $var ]
        InsertChild $grammar $var [lindex $vinfo 2] [lindex $vinfo 3] [list [lindex $vinfo 4]] \
          $item_id [lindex $vinfo 5] [lindex $vinfo 6] [lindex $vinfo 7]
      } else {
        InsertChild $grammar $var [lindex $vinfo 2] [lindex $vinfo 3] [list [lindex $vinfo 4]] $item_id 0
      }  
    }
  } else {
#    $w_tabset tab configure Variables -state disabled
#    $w_tabset select 1
  }
  foreach fun  [::tol::info functions $grammar] {
    set finfo [::tol::info functions $grammar $fun]
    InsertChild Code $fun [lindex $finfo 0] [lindex $finfo 1] [list [lindex $finfo 2]] 0    
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::SelectSet { } {
#
# PURPOSE: Script called each time a Set node in ht_tree is selected. Gets
#          information about each child of this set, and inserts it in the
#          right treeview.
#
#/////////////////////////////////////////////////////////////////////////////
  variable ht_tree
  variable ht_vars
  variable ht_funcs
  
  variable tolset
  variable tolindex
  variable iterSet
  variable var_id 1
  variable fun_id 1
  variable item_id
  #variable selecting_set
  
  #  if { $selecting_set } {
  #puts "eoooo ..."
  #    return
  #  }
  ClearHiertables
  #  set selecting_set 1
  set item_id 1
  
  #puts "SelectSet: tolset=$tolset tolindex=$tolindex"
  set iterSet [eval list $tolset $tolindex]
  variable gra_parent [ GetParentGrammar $iterSet ]
  ::tol::forallchild $iterSet ::TolInspector::_InsertChild
  #  set selecting_set 0
}


#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::CloseObject { index } {
#
# PURPOSE: Script called each time a node in ht_tree (left treeview) is
#          closed.
#
# PARAMETERS:
#   index : The id of the node closed
#
#/////////////////////////////////////////////////////////////////////////////
  variable ht_tree
  #variable closing 1
  set path [$ht_tree get -full $index]
  if { $index } {
    set parent1 [lindex $path 1]
    if { [string equal $parent1 "root-packages"] ||
         [string equal $parent1 "root-files"] ||
         [string equal $parent1 "root-console"] || 
         [string equal $parent1 "root-pool"] } {
      $ht_tree entry delete $index 0 end
    }
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::OpenVariable { index } {
#
# PURPOSE: ????
#
# PARAMETERS:
#   index -> 
#
#/////////////////////////////////////////////////////////////////////////////
  variable tolset     ;# Name of the current TOL set variable
  variable ht_vars    ;# ::blt::treeview of variables
  variable ht_tree    ;# ::blt::treeview left
  variable blt_tree   ;# ::blt::tree left
  variable item_data  ;# Array to hold the full content & description of items
 
  if { ![string length $index] } {
    return
  }
  set node_anchor [lindex [$ht_tree get -full anchor] end]
  
  set tree_index -1
  #puts "OpenVariable = $item_data([lindex [$ht_vars entry cget $index -data] end])"
  if { ![regexp -- "grammar-" $node_anchor] } {
    set grammar [lindex $item_data([lindex [$ht_vars entry cget $index -data] end]) 2]
    if {$grammar eq "Set" || $grammar eq "NameBlock"} {
      $ht_tree open anchor
      #puts "full = [$ht_vars get -full $index]"
      set local_name [lindex [$ht_vars get -full $index] end]
      set idx_anchor [$ht_tree index anchor]
      #puts "a buscar $local_name dentro de $idx_anchor"
      if {[set tree_index [$blt_tree findchild $idx_anchor $local_name]] == -1} {
        foreach idx [$ht_tree entry children anchor] {
          if { [string equal $local_name [lindex [$ht_tree get -full $idx] end]] } {
            set tree_index $idx
            break
          }
        }
      }
    }
  }
  if { $tree_index != -1 } {
    update
    blt::tv::SetSelectionAnchor $ht_tree $tree_index
    set blt::tv::scroll 1
    #$ht_tree selection clearall
    #$ht_tree see $tree_index
    #$ht_tree focus $tree_index
    #$ht_tree selection set $tree_index
    #$ht_tree selection anchor $tree_index
    #set blt::Hiertable::scroll 1
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::SelectItem { ht } {
#
# PURPOSE: Procedure that is executed when the user selects an element of
#          the trees of variables orfunctions.
#          First it take all the arguments necessary of the node of tree to
#          execute the procedure stored in the variable OnSelectItem, and
#          later executes it passing those arguments.
#
# PARAMETERS: path of ht_* (::blt::treeview of variables or of functions)
#
#/////////////////////////////////////////////////////////////////////////////
 # variable w_text       ;# TODO: ????
  variable OnSelectItem ;#Command to invoke on SelectItem in ht_*
  variable item_data    ;# Array to hold the full content & description of items
  
  set entry [$ht index anchor] ;# id of entry selected  
  if { [string length $OnSelectItem] } {
    # there are associated command
    if { [string length $entry] } {
      # almost one entry of ht_* selected
      array set aryData [$ht entry cget $entry -data]
      set name [$ht entry cget $entry -label] ;# name of item
      set itemid $aryData(Index)              ;# id of item
      set path   $aryData(Path)               ;# path of file of item
      #(pgea) se anyade la propiedad Reference
      set objRef $aryData(Reference)          ;# info of item
      set full_data $item_data($itemid) ;# {value} {desc} {grammar}       
      set value [lindex $full_data 0]   ;# value of item PE:
                                         # Anything Field(Set s, Text nombre)

      if { [string equal \"\n\" $value] } {
        # hasn't value        
        set value {"\n"}
      } else {
        set value [string trim $value]
      }
      set desc [string trim [lindex $full_data 1]]
      set grammar [lindex $full_data 2]
      set icon [lindex [$ht entry cget $entry -icons] 0]
      #(pgea) lista de argumentos que recibe ::TolConsole::OnInfo
      set args [list $icon $grammar $name $value $path $desc $objRef]
    } else {
      set args ""
    }
    eval "$OnSelectItem $args"
  }
  return
#;# TODO: ????
#  if { [string length $entry] } {
#    FillText $ht $entry
#  } else {
#    $w_text delete 1.0 end
#  }
}

#/////////////////////////////////////////////////////////////////////////////
#(pgea) se usa?
proc ::TolInspector::FillText { ht entry } {
#/////////////////////////////////////////////////////////////////////////////
  variable w_text
  variable item_data
  #upvar #0 ::TolTk::Images Images

  $w_text delete 1.0 end
  array set aryData [$ht entry cget $entry -data]
  set name [$ht entry cget $entry -label]
  set itemid $aryData(Index)
  set full_data $item_data($itemid)
  
  set value [lindex $full_data 0]
  if { [string equal \"\n\" $value] } {
    set value {"\n"}
  }
  set desc [string trim [lindex $full_data 1]]
  set grammar [lindex $full_data 2]
  set icon [lindex [$ht entry cget $entry -icons] 0]
  foreach {prop val} [list "Grammar: " $grammar "Name: " $name "Value: " [string trim $value]] {
    set idx_start [$w_text index "end -1 chars"]
    $w_text insert end $prop
    set idx_end   [$w_text index "end -1 chars"]
    $w_text tag add tagLabel $idx_start $idx_end
    if {[string equal $prop "Grammar: "]} {
      $w_text image create end -image $icon
      $w_text insert end "  "
    }
    $w_text insert end "$val\n"
    $w_text tag add tagValue $idx_end [$w_text index "end -1 chars"]
  }
  $w_text tag configure tagLabel -font fnBold
  $w_text tag configure tagValue -wrap none
  if { [string length $desc] } {
    set idx_start [$w_text index "end -1 chars"]
    $w_text insert end "Description\n"
    set idx_end [$w_text index "end -1 chars"]
    $w_text tag add tagPropLabel $idx_start $idx_end
    $w_text insert end $desc
    $w_text tag add tagPropValue $idx_end [$w_text index "end -1 chars"]
    $w_text tag configure tagPropLabel -font fnBold -underline 1
    $w_text tag configure tagPropValue -wrap word
  }
}

#proc ::TolInspector::GetIndexes { nodeid } {
#  variable ht_tree
#  # No se utiliza
#  # name is prefix-i1-i2-i3-...-in
#
#  if { $nodeid } {
#    lrange [split [$ht_tree get $nodeid] "-"] 1 end
#  } else list
#}