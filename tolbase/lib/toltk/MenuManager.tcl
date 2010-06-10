namespace eval ::MenuManager {
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

  set SOA [ TclList2SetOfText $objSelection ]
  set rvar "__invokeCommand__"
  set tolExpr [ string map [ list %RV $rvar %N $name %S $SOA %G $group ] {
    Real %RV = {
      Set objAddress = %S;
      Anything target = If( %G, {
        EvalSet( objAddress, Anything( Text addr ) { 
          GetObjectFromAddress( addr )
        } )
      }, GetObjectFromAddress( objAddress[ 1 ] ) );
      Real GuiTools::MenuManager::invokeEntry( "%N", target )
    }
  } ]
  tol::console eval $tolExpr
  set info [ tol::info variable [ list Real $rvar ] ]
  set value [ lindex $info 2 ]
  tol::console stack release $rvar
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
  set path0 [ split $name "/" ]
  set mm $targetMenu
  set nn ""
  foreach n [ lrange $path0 0 end-1 ] {
    set mm0 ${mm}.mm$n
    if { [ winfo exists $mm0 ] } {
      $mm0 delete 0 end
    } else {
      menu $mm0 -tearoff 0
    }
    set nn [ expr { $nn eq "" ? $n : "/$n" } ]
    set labelInfo [ getLabelInfoForEntry $nn ]
    foreach { name label image translate } $labelInfo break
    set image0 [ ::ImageManager::getImageResourceId $image ]
    if { $image0 ne "" } {
      set image $image0
    }
    $mm add cascade -menu $mm0 \
        -label [ expr { $translate ? [ msgcat::mc $label ] : $label } ] \
        -image $image -compound [ expr { $image eq "" ? "none" : "left" } ]
    set mm $mm0
  }
  return $mm
}

proc ::MenuManager::insertEntries { menuWidget objSelection entriesInfo } {
  set multiple [ expr { [ llength $objSelection ] > 1 } ]
  foreach entryInfo $entriesInfo {
    foreach {name label image translate group} $entryInfo break
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
  insertEntries $menuWidget $selection $typesSelected(globalOptions)
  # inserto las opciones especificas de cada tipo si es que hay mas de
  # un tipo
  if { [ llength $typesSelected(types) ] > 1 } {
    foreach type $typesSelected(types) {
      set menuType $menuWidget.mtype$type
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
