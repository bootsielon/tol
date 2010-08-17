namespace eval ::MenuManager {
  variable observers
}

proc ::MenuManager::addObserver { event when script } {
  variable observers

  if { $when ne "Before" && $when ne "After" } {
    error "::MenuManager::addObserver -- valor invalido '$when' para el argumento when, debe ser Before o After"
  }
  lappend observers($event,$when) $script
}

proc ::MenuManager::invokeObservers { event when } {
  variable observers

  if { [ info exists observers($event,$when) ] } { 
    foreach script $observers($event,$when) {
      set result [ eval uplevel \#0 $script $when ]
      if { !$result } {
        return 0
      }
    }
  }
  return 1
}

proc ::MenuManager::getTypeOfObject { addr } {
  set rvar "__getTypeOfObject__"
  set tolExpr [ string map [ list %A $addr %RV $rvar ] {
    Text %RV = {
      Anything obj = GetObjectFromAddress( "%A" );
      Text theGrammar = Grammar( obj );
      Text theType = If( theGrammar == "NameBlock", {
        Text theClass = ClassOf( obj );
        If( theClass == "", "NameBlock", theClass )
      }, theGrammar )  
    }
  } ]
  tol::console eval $tolExpr
  set info [ tol::info variable [ list Text $rvar ] ]
  set value [ string trim [ lindex $info 2 ] \" ]
  tol::console stack release $rvar
  return $value
}

proc ::MenuManager::getLabelInfoForType { type } {
  set rvar "__getLabelForType__"
  set tolExpr [ string map [ list %RV $rvar %T $type ] {
    Set %RV = {
      Set aux = GuiTools::MenuManager::getTypeLabelInfo( "%T" );
      Set If( Card( aux ),
              aux, GuiTools::MenuManager::getDefaultTypeLabelInfo( "%T" ) )
    }
  } ]
  tol::console eval $tolExpr
  set value [ TolObj2TclObj [ list Set $rvar ] ]
  tol::console stack release $rvar
  return $value
}

proc ::MenuManager::getLabelInfoForEntry { entryName } {
  set rvar "__getLabelInfoForEntry__"
  set tolExpr [ string map [ list %RV $rvar %N $entryName ] {
    Set %RV = {
      Set aux = GuiTools::MenuManager::getEntryInfo( "%N" );
      Set If( Card( aux ),
              aux, GuiTools::MenuManager::getDefaultEntryInfo( "%N" ) )
    }
  } ]
  tol::console eval $tolExpr
  set value [ TolObj2TclObj [ list Set $rvar ] ]
  tol::console stack release $rvar
  return $value
}

proc ::MenuManager::getMenuOptionsForType { type } {
  set rvar "__getMenuOptionsForType__"
  set tolExpr [ string map [ list %RV $rvar %T $type ] {
    Set %RV = GuiTools::MenuManager::getTypeOptionsInfo( "%T" );
  } ]
  tol::console eval $tolExpr
  set value [ TolObj2TclObj [ list Set $rvar ] ]
  tol::console stack release $rvar
  return $value
}

proc ::MenuManager::checkEntryState { name objSelection group } {
  variable typesSelected

  set SOA [ TclList2SetOfText $objSelection ]
  set rvar "__checkEntryState__"
  set tolExpr [ string map [ list %RV $rvar %N $name %S $SOA %G $group ] {
    Real %RV = {
      Set objAddress = %S;
      Anything target = If( %G, {
        EvalSet( objAddress, Anything( Text addr ) { 
          GetObjectFromAddress( addr )
        } )
      }, GetObjectFromAddress( objAddress[ 1 ] ) );
      Real GuiTools::MenuManager::checkStateEntry( "%N", target )
    }
  } ]
  tol::console eval $tolExpr
  set info [ tol::info variable [ list Real $rvar ] ]
  set value [ lindex $info 2 ]
  tol::console stack release $rvar
  return $value
}

proc ::MenuManager::invokeCommand { name objSelection group } {
  variable typesSelected

  if { ![ invokeObservers $name "Before" ] } {
    return 0
  }
  set SOA [ TclList2SetOfText $objSelection ]
  set rvar "__invokeCommand__"
  set tolExpr [ string map [ list %RV $rvar %N $name %S $SOA %G $group ] {
    Real %RV = {
      Set objAddress = %S;
      Real GuiTools::MenuManager::invokeEntry( "%N", If( %G, {
        EvalSet( objAddress, Anything( Text addr ) { 
          GetObjectFromAddress( addr )
        } )
      }, GetObjectFromAddress( objAddress[ 1 ] ) ) )
    }
  } ]
  tol::console eval $tolExpr
  set info [ tol::info variable [ list Real $rvar ] ]
  set value [ lindex $info 2 ]
  tol::console stack release $rvar
  invokeObservers $name "After"
  return $value
}

proc ::MenuManager::checkIfMenuManagerIsLoaded { } {
  set rvar "__checkIfMenuManagerIsLoaded__"
  set tolExpr [ string map [ list %RV $rvar  ] {
    Real %RV = ObjectExist( "NameBlock", "GuiTools::MenuManager" )
  } ]
  tol::console eval $tolExpr
  set info [ tol::info variable [ list Real $rvar ] ]
  set value [ lindex $info 2 ]
  tol::console stack release $rvar
  return [ expr { $value!=0 } ]
}

proc ::MenuManager::initTypesInfoFromSelection { selection } {
  variable typesSelected

  array unset typesSelected
  array set typesSelected {
    types {}
  }
  if { ![ checkIfMenuManagerIsLoaded ] } return
  foreach addr $selection {
    set type [ getTypeOfObject $addr ]
    set typeOptionInfo [ getMenuOptionsForType $type ]
    # registro el tipo solo si tiene informacion de menu asociada
    if { [ llength $typeOptionInfo ] } {
      if { ![ info exists typesSelected(objects,$type) ] } {
        lappend typesSelected(types) $type
        set typesSelected(labelInfo,$type) [ getLabelInfoForType $type ]
        set typesSelected(entriesInfo,$type) $typeOptionInfo
        foreach option $typeOptionInfo {
          set name [ lindex $option 0 ]
          set typesSelected(optionInfo,$name) $option
          if { [ catch { lsearch $typesSelected(types,$name) $type } idx ] } {
            set typesSelected(types,$name) [ list $type ]
          } elseif { $idx == -1 } {
            lappend typesSelected(types,$name) $type
          }
        }
      }
      lappend typesSelected(objects) $addr
      lappend typesSelected(objects,$type) $addr
    }
  }
  # busco las opciones que se aplican a toda la seleccion
  set numberOfTypes [ llength $typesSelected(types) ]
  set typesSelected(globalOptions) [ list ]
  foreach optionIndex [ array names typesSelected types,* ] {
    set optionName [ string range $optionIndex 6 end ]
    if { [ llength $typesSelected(types,$optionName) ] == $numberOfTypes } {
      lappend typesSelected(globalOptions) $typesSelected(optionInfo,$optionName)
    }
  }
  return $typesSelected(types)
}

proc ::MenuManager::createSubMenuIfNeeded { targetMenu name } {
  variable typesSelected

  set path0 [ split $name "/" ]
  set mm $targetMenu
  set nn ""
  set mapdot [ list . - ]
  foreach n [ lrange $path0 0 end-1 ] {
    #puts "n = $n"
    # evito el caracter . en el nombre de widget
    set n0 [ string map $mapdot $n ]
    set mm0 ${mm}.mm$n0
    #puts "mm0 = $mm0"
    if { ![ info exists typesSelected(subMenu,inserted,$mm0) ] } {
      if { [ winfo exists $mm0 ] } {
        #puts "$mm0 existe"
        $mm0 delete 0 end
      } else {
        menu $mm0 -tearoff 0
      }
      set nn [ expr { $nn eq "" ? $n : "${nn}/${n}" } ]
      set labelInfo [ getLabelInfoForEntry $nn ]
      foreach { name label image translate } $labelInfo break
      set image0 [ ::ImageManager::getImageResourceId $image ]
      if { $image0 ne "" } {
        set image $image0
      }
      #puts "insertando menu $mm0 en $mm"
      $mm add cascade -menu $mm0 \
          -label [ expr { $translate ? [ msgcat::mc $label ] : $label } ] \
          -image $image -compound [ expr { $image eq "" ? "none" : "left" } ]
      set typesSelected(subMenu,inserted,$mm0) 1
    }
    set mm $mm0
  }
  #puts "retornando con mm  = $mm"
  return $mm
}

proc ::MenuManager::compareMenuEntries { entry1 entry2 } {
  # cache del rank de las entries, se pide a TOL solo una vez por
  # ordenamiento
  variable entriesRank

  set name1 [ lindex $entry1 0 ]
  set name2 [ lindex $entry2 0 ]
  set path1 [ lrange [ split $name1 "/" ] 0 end-1 ]
  set path2 [ lrange [ split $name2 "/" ] 0 end-1 ]
  set submenu1 ""
  set submenu2 ""
  foreach subm1 $path1 subm2 $path2 {
    if { $subm1 eq "" } {
      # path1 es mas corto que path2, la parte comun es igual, asumo
      # que es <
      return -1
    }
    if { $subm2 eq "" } {
      # path2 es mas corto que path2, la parte comun es igual, asumo
      # que es >
      return 1
    }
    append submenu1 [ expr { $submenu1 eq "" ? "" : "/" } ] $subm1
    append submenu2 [ expr { $submenu2 eq "" ? "" : "/" } ] $subm2
    if { ![ info exist entriesRank($submenu1) ] } {
      set submenuInfo [ getLabelInfoForEntry $submenu1 ]
      set entriesRank($submenu1,rank) [ lindex $submenuInfo 4 ]
    }
    if { ![ info exist entriesRank($submenu2) ] } {
      set submenuInfo [ getLabelInfoForEntry $submenu2 ]
      set entriesRank($submenu2,rank) [ lindex $submenuInfo 4 ]
    }
    set cmp \
        [ expr { $entriesRank($submenu1,rank) - $entriesRank($submenu2,rank) } ]
    if { $cmp != 0 } {
      return $cmp
    }
  }
  # hasta este punto los caminos son iguales hay que usar el rank de la entry
  return [ expr { [ lindex $entry1 4 ] - [ lindex $entry2 4 ] } ]
}

proc ::MenuManager::insertEntries { menuWidget objSelection entriesInfo } {
  variable entriesRank

  set multiple [ expr { [ llength $objSelection ] > 1 } ]
  # inserto las entries segun el orden inducido por el campo rank
  if { [ llength [ lindex $entriesInfo 0 ] ] > 4 } {
    # rank aparecio en la version 2.2 de GuiTools
    array unset entriesRank

    #set sortedEntriesInfo [ lsort -real -index 4 $entriesInfo ]
    set sortedEntriesInfo [ lsort \
                                -command ::MenuManager::compareMenuEntries \
                                $entriesInfo ]
  } else {
    # se asume rank 0 para todos, el orden no cambia
    set sortedEntriesInfo $entriesInfo
  }
  foreach entryInfo $sortedEntriesInfo {
    foreach {name label image translate rank group} $entryInfo break
    if { $group eq "" } {
      # rank aparecio en la version 2.2 de GuiTools
      set group $rank
    }
    if { $group ^ $multiple } continue
    set mm [ createSubMenuIfNeeded $menuWidget $name ]
    set state [ checkEntryState $name $objSelection $group ]
    set entryLabel [ expr { $translate ? [ msgcat::mc $label ] : $label } ]
    set image0 [ ::ImageManager::getImageResourceId $image ]
    if { $image0 ne "" } {
      set image $image0
    }
    $mm add command -label $entryLabel \
        -command [ list ::MenuManager::invokeCommand $name $objSelection $group ] \
        -state [ expr { $state ? "normal" : "disabled" } ] \
        -image $image -compound [ expr { $image eq "" ? "none" : "left" } ]
  }
}

proc ::MenuManager::insertEntriesForSelection { menuWidget selection } {
  variable typesSelected

  if { ![ llength [ initTypesInfoFromSelection $selection ] ] } return
  set lenghtValidSelection [ llength $typesSelected(objects) ]
  set lengthSelection [ llength $selection ]
  if { $lengthSelection == $lenghtValidSelection } {
    insertEntries $menuWidget \
        $typesSelected(objects) $typesSelected(globalOptions)
  }
  # inserto las opciones especificas de cada tipo si es que hay mas de
  # un tipo
  if { ( $lengthSelection != $lenghtValidSelection ) ||
       [ llength $typesSelected(types) ] > 1 } {
    set mapdot [ list . - ]
    foreach type $typesSelected(types) {
      # type puede contener . por eso hay que reemplazarlo
      set type0 [ string map $mapdot $type ]
      set menuType $menuWidget.mtype$type0
      if { ![ winfo exists $menuType ] } {
        menu $menuType -tearoff 0
      } else {
        $menuType delete 0 end
      }
      foreach {label image translate} $typesSelected(labelInfo,$type) break  
      set labelCascade [ expr { $translate ? [ msgcat::mc $label ] : $label } ]
      set image0 [ ::ImageManager::getImageResourceId $image ]
      if { $image0 ne "" } {
        set image $image0
      }
      $menuWidget add cascade -label $labelCascade -menu $menuType \
          -image $image -compound [ expr { $image eq "" ? "none" : "left" } ]
      insertEntries $menuType \
          $typesSelected(objects,$type) $typesSelected(entriesInfo,$type)
    }
  }
}
