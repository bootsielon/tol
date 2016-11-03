
#/////////////////////////////////////////////////////////////////////////////
proc TclList2SetOfText { lst } {
#/////////////////////////////////////////////////////////////////////////////
  set result [ list ]
  foreach i $lst {
    lappend result \"[string map {\" \\\"} $i]\"
  }
  return "SetOfText([join $result ,])"
}

#/////////////////////////////////////////////////////////////////////////////
proc Tol_GetViewPrivateMembers { } {
#/////////////////////////////////////////////////////////////////////////////
  set viewOpt 0
  catch {
    # no funciona 
    tol::console eval {
      Real __getViewPrivateMembers_tmp__ =
      TolConfigManager::Config::Various::View::PrivateMembers;
    }
    set x [ lindex [ tol::info variable { Real __getViewPrivateMembers_tmp__ } ] 2 ]
    tol::console stack release __getViewPrivateMembers_tmp__
    set viewOpt [ expr { int( $x ) } ]
    
  } msg
  #puts "Tol_GetViewPrivateMembers: $msg"
  return $viewOpt
}

#/////////////////////////////////////////////////////////////////////////////
proc Tol_ObjIsClassOf { obj_addr cls_name } {
#/////////////////////////////////////////////////////////////////////////////
  tol::console eval [ string map [ list %A $obj_addr %C $cls_name ] {
    Real __gui_check__ = IsInstanceOf( GetObjectFromAddress( "%A" ),"%C" )
  } ]
  set info [ tol::info variable {Real __gui_check__} ]
  set x [ lindex $info 2 ]
  tol::console stack release __gui_check__
  expr { round($x) }
}

#/////////////////////////////////////////////////////////////////////////////
proc Tol_ClassOf { obj_addr } {
#/////////////////////////////////////////////////////////////////////////////
  tol::console eval [ string map [ list %A $obj_addr ] {
    Text __gui_classof__ = {
      Anything obj = GetObjectFromAddress( "%A" );
      // WriteLn( "Tol_ClassOf " << Grammar( obj ) );
      Text If( Grammar( obj ) == "NameBlock", ClassOf( obj ), "" )
    }
  } ]
  set info [ tol::info variable {Text __gui_classof__} ]
  set x [ string trim [ lindex $info 2 ]  \" ]
  tol::console stack release __gui_classof__
  set x
}

#/////////////////////////////////////////////////////////////////////////////
proc Tol_ClassOfFromReference { objReference } {
#/////////////////////////////////////////////////////////////////////////////
  set addr [ ::tol::info address $objReference ]
  return [ Tol_ClassOf $addr ]
}

#/////////////////////////////////////////////////////////////////////////////
proc Tol_StructOf { obj_addr } {
#/////////////////////////////////////////////////////////////////////////////
  set tolcode [ string map [ list %A $obj_addr ]  {
    Text {
      Anything obj = GetObjectFromAddress( "%A" );
      // WriteLn( "Tol_StructOf " << Grammar( obj ) );
      Text If( Grammar( obj ) == "Set", StructName( obj ), "" )
    }
  } ]
  return [ lindex [ toltcl::eval $tolcode ] 0 ]
}

#/////////////////////////////////////////////////////////////////////////////
proc Tol_StructOfFromReference { objReference } {
#/////////////////////////////////////////////////////////////////////////////
  # WATCH!!!!
  # when Tol_StructOfFromReference is invoken on {File xxx} xxx is
  # decompiled!!!!
  if { [ llength $objReference ] == 2 && 
       [ lindex $objReference 0 ] eq "File" } {
    return ""
  }
  set addr [ ::tol::info address $objReference ]
  return [ Tol_StructOf $addr ]
}

#/////////////////////////////////////////////////////////////////////////////
#(pgea) nueva funcion para obtener la informacion de instancia
proc Tol_InstanceInfo { obj_addr } {
#/////////////////////////////////////////////////////////////////////////////
  tol::console eval [ string map [ list %A $obj_addr ] {
    Text __gui_instanceinfo__ = {
      Anything __intance__ = GetObjectFromAddress("%A");
      Text If(ObjectExist("Code", "__intance__::_.get.autodoc.info"), 
        __intance__::_.get.autodoc.info(?), "")
    }
  } ]
  set info [ tol::info variable {Text __gui_instanceinfo__} ]
  set x [ string trim [ lindex $info 2 ]  \" ]
  tol::console stack release __gui_instanceinfo__
  set x
}

#/////////////////////////////////////////////////////////////////////////////
#(pgea) nueva funcion para obtener la informacion de instancia
proc Tol_InstanceInfoFromReference { objReference } {
#/////////////////////////////////////////////////////////////////////////////
  set addr [ ::tol::info address $objReference ]
  return [ Tol_InstanceInfo $addr ]
}

#/////////////////////////////////////////////////////////////////////////////
#(pgea) nueva funcion para obtener el contenido de la instancia
proc Tol_InstanceContent { obj_addr } {
#/////////////////////////////////////////////////////////////////////////////
  tol::console eval [ string map [ list %A $obj_addr ] {
    Text __gui_instancecontent__ = {
      Anything __intance__ = GetObjectFromAddress("%A");
      Text If(ObjectExist("Code", "__intance__::_.get.autodoc.content"), 
        __intance__::_.get.autodoc.content(?), "")
    }
  } ]
  set info [ tol::info variable {Text __gui_instancecontent__} ]
  set x [ string trim [ lindex $info 2 ]  \" ]
  tol::console stack release __gui_instancecontent__
  set x
}

#/////////////////////////////////////////////////////////////////////////////
#(pgea) nueva funcion para obtener el contenido de la instancia
proc Tol_InstanceContentFromReference { objReference } {
#/////////////////////////////////////////////////////////////////////////////
  set addr [ ::tol::info address $objReference ]
  return [ Tol_InstanceContent $addr ]
}

#/////////////////////////////////////////////////////////////////////////////
proc TolGui_GetMenuEntries { selection idx } {
#/////////////////////////////////////////////////////////////////////////////
  #puts "TolGui_GetMenuEntries $selection $idx"
  array set instances {}
  array set menu_class {}
  set i 0
  foreach obj_info $selection {
    set tcl_ref [ lindex $obj_info 0 ]
    set obj_addr [ ::tol::info address $tcl_ref ]
    if { [ Tol_ObjIsClassOf $obj_addr "GuiTools::@MenuDesc" ] } {
      set class [ Tol_ClassOf $obj_addr ]
      lappend instances($class) $obj_addr
      set name_inst [ lindex $obj_info 1 ]
      set menu_items [ TolGui_GetObjMenu $obj_addr ]
      # esto hace que estemos guardando la ultima instancia, asumimos
      # tambien que todas las instancia tienen la misma espcificacion
      # de menu
      if { ![ llength [ array names menu_class $class ] ] || $i == $idx } {
        set menu_class($class) \
            [ list $name_inst $obj_addr $menu_items ]
      }
    }
    incr i
  }
  set menu_info [ list ]
  foreach c [ array names instances ] {
    lappend menu_info [ list $c $menu_class($c) $instances($c) ]
  }
  #puts "$menu_info"
  set menu_info
}

#/////////////////////////////////////////////////////////////////////////////
proc TolGui_StoreSetElementsInOZA { object } {
#/////////////////////////////////////////////////////////////////////////////
  set saveIn [ tk_getSaveFile -defaultextension .oza \
                   -parent . -title [ mc "Store OZA" ] \
                   -filetypes {{oza {.oza}}} ]

  if { $saveIn ne "" } {
    set addr [ tol::info address $object ]
    set tolexpr [ string map [ list %a $addr %f $saveIn ] {
      Real Ois.Store( GetObjectFromAddress( "%a" ), "%f" );
    } ]
    toltcl::eval $tolexpr
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc TolGui_StoreSelectionInOZA { selection } {
#/////////////////////////////////////////////////////////////////////////////
  set saveIn [ tk_getSaveFile -defaultextension .oza \
                   -parent . -title [ mc "Store OZA" ] \
                   -filetypes {{oza {.oza}}} ]
  if { $saveIn ne "" } {
    set lst_addr {}
    foreach r $selection {
      set a [ tol::info address $r ]
      lappend lst_addr \"$a\"
    }
    set set_addr "\[\[ [join $lst_addr ,] \]\]"
    set tolexpr [ string map [ list %L $set_addr %f $saveIn ] {
      Real {
        Set items = EvalSet( %L, Anything( Text addr ) {
          Anything GetObjectFromAddress( addr )
        } );
        Real Ois.Store( items, "%f" )
      }
    } ]
    toltcl::eval $tolexpr
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc TolGui_InsertEntriesMatrixChart { mm selection } {
#/////////////////////////////////////////////////////////////////////////////
  if {  ![ llength $selection ] } {
    return 0
  }
  set lst_addr {}
  foreach r $selection {
    set a [ tol::info address $r ]
    lappend lst_addr \"$a\"
  }
  set set_addr "\[\[ [join $lst_addr ,] \]\]"
  set tolexpr [ string map [ list %L $set_addr ] {
    Real {
      Set all_mat = EvalSet( %L, Matrix( Text addr ) {
        GetObjectFromAddress( addr )
      } );
      Real R = Rows( all_mat[1] );
      Set st = EvalSet( all_mat, Real( Matrix it ) {
        Real EQ( R, Rows( it ) )
      } );
      Real BinGroup( "And", st )
    }
  } ]
  set couldChart [ toltcl::eval $tolexpr ]
  if { $couldChart } {
    $mm add command -label [ mc "Draw matrix selection (natural)" ] \
        -command [ list TolGui_ChartMatrix $selection ]
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc TolGui_ChartMatrix { selection } {
#/////////////////////////////////////////////////////////////////////////////
  set lst_addr {}
  foreach r $selection {
    set a [ tol::info address $r ]
    lappend lst_addr \"$a\"
  }
  set set_addr "\[\[ [join $lst_addr ,] \]\]"
  set tolexpr [ string map [ list %L $set_addr ] {
    Real {
      Set all_mat = EvalSet( %L, Matrix( Text addr ) {
        GetObjectFromAddress( addr )
      } );
      Set names = Copy( Empty );
      Set For( 1, Card( all_mat ), Real( Real k ) {
        Text _n = Name( all_mat[ k ] );
        Text n = If( _n == "", "unamed", _n );
        Set Append( names, If( Columns( all_mat[ k ] ) == 1, [[ n ]], {
          Set For( 1, Columns( all_mat[ k ] ), Text( Real k ) {
            Text n << "[" << k << "]"
          } )
        } ) );
        Real 1
      } );
      Matrix groupMat = BinGroup( "|", all_mat );
      Text TclChartMatrix( groupMat, 
                           SetOfSet (
                                     @TclArgSt("-title",    "Matrix Chart" ),
                                     @TclArgSt("-type"    , "2"            ),
                                     @TclArgSt("-names",    TxtListTcl(names) ) ) )
    }
  } ]
  toltcl::eval $tolexpr
}

#/////////////////////////////////////////////////////////////////////////////
proc TolGui_InsertEntriesFromMenuManager { targetMenu selection } {
#/////////////////////////////////////////////////////////////////////////////
  set addrList [ list ]
  foreach obj_info $selection {
    set tcl_ref [ lindex $obj_info 0 ]
    lappend addrList [ ::tol::info address $tcl_ref ]
  }
  ::MenuManager::insertEntriesForSelection $targetMenu $addrList
}

#/////////////////////////////////////////////////////////////////////////////
proc TolGui_GetObjMenu { obj_addr } {
#/////////////////////////////////////////////////////////////////////////////
  set try [ catch {
    tol::console eval [ string map [ list %A $obj_addr ] {
      GuiTools::@MenuDesc __aux_menu__ = GetObjectFromAddress("%A");
      Set __gui_menu_entries__ = __aux_menu__::getMenuEntries(?)
    } ] } msg ]
  if { $try } {
    puts "ERROR TolGui_GetObjMenu : $msg"
    set result ""
  } else {
    set result [ TolObj2TclObj {Set __gui_menu_entries__} ]
  }
  tol::console stack release __aux_menu__
  tol::console stack release __gui_menu_entries__
  set result
}

#/////////////////////////////////////////////////////////////////////////////
proc TolGui_InvokeMethod { method obj_addr } {
#/////////////////////////////////////////////////////////////////////////////
  #puts "TolGui_InvokeMethod $method $obj_addr"
  set try [ catch {
    tol::console eval [ string map [ list %A $obj_addr %M $method ] {
      NameBlock __aux_instance__ = GetObjectFromAddress("%A");
      Real __aux_result__ = __aux_instance__::%M(?)
    } ] } msg ]
  if { $try } {
    puts "ERROR TolGui_InvokeMethod : $msg"
  }
  tol::console stack release __aux_instance__
  tol::console stack release __aux_result__
}

#/////////////////////////////////////////////////////////////////////////////
proc TolGui_InvokeGroup { cname function group } {
#/////////////////////////////////////////////////////////////////////////////
  set SOA [ TclList2SetOfText $group ]
  set try [ catch {
    tol::console eval \
        [ string map [ list %C $cname %F $function %S $SOA ] {
          Real __aux_result__ = {
            Set aux = EvalSet( %S, NameBlock( Text addr ) {
              NameBlock GetObjectFromAddress( addr )
            } );
            Real %C::%F( aux )
          }
        } ] } msg ]
  if { $try } {
    puts "ERROR TolGui_InvokeGroup : $msg"
  }
  tol::console stack release __aux_result__
}
