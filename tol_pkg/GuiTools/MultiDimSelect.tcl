package require snit
package require BWidget
package require autoscroll
package require tablelist

snit::widgetadaptor DimTable {

  typevariable checkedImg
  typevariable uncheckedImg

  typeconstructor {
    option add *Tablelist*Checkbutton.background		white
    option add *Tablelist*Checkbutton.activeBackground	white
    
    if {[catch {tk windowingsystem} winSys] != 0} {
      switch $::tcl_platform(platform) {
	unix      { set winSys x11 }
	windows   { set winSys win32 }
	macintosh { set winSys classic }
      }
    }
    
    #
    # Add some entries to the Tk option database
    #
    switch $winSys {
      x11 {
	#
	# Create the font TkDefaultFont if not yet present
	#
	catch {font create TkDefaultFont -family Helvetica -size -12}
        
	option add *Font		TkDefaultFont
	option add *selectBackground	#678db2
	option add *selectForeground	white
      }
      classic {
	option add *background		#dedede
      }
    }
    option add *Tablelist.background	gray98
    option add *Tablelist.stripeBackground	#e0e8f0
    option add *Tablelist.setGrid		yes
    option add *Tablelist.movableColumns	yes
    option add *Tablelist.labelCommand	::tablelist::sortByColumn
    option add *Tablelist.labelCommand2	::tablelist::addToSortColumns

    set checkedImgData {
      R0lGODdhCwALAJEAAH9/f////wAAAP///ywAAAAACwALAAACLISPRvEPAE8o
      AMUXCYAgJSEiAYRIQkSCAgTJjgiAoEgSEQGEJIRiA9wdwUcrADs=
    }
    
    set uncheckedImgData {
      R0lGODdhCwALAJEAAH9/f////////////ywAAAAACwALAAACH4SPRvEPADEI
      YPwDQAwCGP8AEIMAxj8AxCCA8Y/goxUAOw==
    }
    set checkedImg   [ image create photo -data $checkedImgData ]
    set uncheckedImg [ image create photo -data $uncheckedImgData ]  
  }

  typemethod emptyStr { val } {
    return ""
  }

  typemethod editEndCmd { w row col text } {
    if { [ $w columncget $col -name ] eq "selected" } {
      set img [ expr { $text ? $DimTable::checkedImg : $DimTable::uncheckedImg } ]
      $w cellconfigure $row,$col -image $img
    }
    return $text
  }
  
  typemethod editStartCmd { w row col text } {
    return $text
  }

  component table -public table

  delegate option * to hull
  delegate method * to hull

  constructor { args } {
    eval installhull using LabelFrame $args
    set f [ $win getframe ]
    install table using ::tablelist::tablelist $f.tbl \
                  -columns { 0 "Selected" center 0 "Dimension Value" left } \
                  -editendcommand [ mytypemethod editEndCmd ] \
                  -editstartcommand [ mytypemethod editStartCmd ] \
                  -selectmode extended
    $table columnconfigure 0 -name selected -editable yes \
        -editwindow checkbutton \
        -formatcommand [ mytypemethod emptyStr ]
    if { [ $table cget -selectborderwidth ] == 0 } {
      $table configure -spacing 1
    }
    set hs [ scrollbar $f.hs -orient horizontal \
                 -command [ list $table xview ] ]
    set vs [ scrollbar $f.vs -orient vertical \
                 -command [list $table yview ] ]
    $table configure \
        -xscrollcommand "$hs set" \
        -yscrollcommand "$vs set"
    grid $table -row 0 -column 0 -sticky snew
    grid $vs -row 0 -column 1 -sticky ns
    grid $hs -row 1 -column 0 -sticky ew
    grid rowconfigure $f 0 -weight 1
    grid columnconfigure $f 0 -weight 1
    ::autoscroll::autoscroll $vs
    ::autoscroll::autoscroll $hs
  }

  method getChecked { } {
    $table finishediting
    set info [ $table get 0 end ]
    puts "getChecked, info : $info"
    set result [ list ]
    set idx 1
    foreach r $info {
      if { [ lindex $r 0 ] } {
        lappend result $idx
      }
      incr idx
    }
    set result
  }

  method appendRow { dimValue } {
    $table insert end [ list 0 $dimValue ]
    $table cellconfigure end,selected -image $DimTable::uncheckedImg
  }

};

snit::widgetadaptor DimSelector {

  option -addr -readonly yes -validatemethod _checkAddr

  method _invokeButton { method dims } {
    set addr $options(-addr)
    puts "_invokeButton $addr $method $dims"
    set try [ catch {
      tol::console eval [ string map [ list %A $addr %M $method %D $dims ] {
        NameBlock __aux_instance__ = GetObjectFromAddress("%A");
        Real __aux_result__ = __aux_instance__::%M(%D)
      } ] } msg ]
    if { $try } {
      puts "ERROR ${type}::_invokeButton : $msg"
    }
    tol::console stack release __aux_instance__
    tol::console stack release __aux_result__
  }  

  method _checkAddr { option value } {
    if { ![ Tol_ObjIsClassOf $value "@MultiDimSelect" ] } {
      puts "$value is not a valid @MultiDimSelect object"
      error "$value is not a valid @MultiDimSelect object"
    }
  }

  delegate option * to hull
  delegate method * to hull

  variable widgets
  variable dimensions
  variable ids 0

  constructor { args } {
    installhull using Dialog -modal none
    $win add -text Accept -command [ mymethod invokeButton onAccept yes ]
    $win add -text Close -command "destroy $win"
    $self configurelist $args
    set widgets [ list ]
    $self _createDimTables
    $self _createButtons
    set f [ $win getframe ]
    grid rowconfigure $f 0 -weight 1
    grid columnconfigure $f 0 -weight 1
  }

  method _createDimTables { } {
    set f [ $win getframe ]
    set main [ frame $f.main ]
    set dimensions [ $self _getObjDimensions ]
    set widgets [ list ]
    grid $main -row 0 -column 0 -sticky snew
    grid rowconfigure $main 0 -weight 1
    foreach dim $dimensions {
      set idx [ llength $widgets ]
      set dt [ DimTable $main.dt$idx -text $dim -side top ]
      foreach dv [ $self _getObjDimValues $dim ] {
        $dt appendRow $dv
      }
      grid $dt -row 0 -column $idx -sticky snew
      grid columnconfigure $main $idx -weight 1
      lappend widgets $dt
    }
  }

  method _createButtons { } {
    set f [ $win getframe ]
    set but_info [ $self _getObjButtons ]
    if { [ llength $but_info ] } {
      set sep [ Separator $f.sep -orient vertical ]
      grid $sep -row 0 -column 1 -sticky sn
      set fbut [ frame $f.fbut ]
      grid $fbut -row 0 -column 2 -sticky sn
      array set but_opts {
        -image ""
        -exit "no"
        -method ""
      }
      set idx 0
      foreach but $but_info {
        array set but_opts {
          -image ""
          -exit "no"
          -method ""
        }
        array set but_opts $but
        button $fbut.but$idx -text $but_opts(-label) \
            -image $but_opts(-image) \
            -command [ mymethod invokeButton $but_opts(-method) $but_opts(-exit) ]
        grid $fbut.but$idx -row $idx -column 0 -sticky ew
        incr idx
      }
    }
  }

  method invokeButton { met ex } {
    set dims_selected [ $self getDimSelected ]
    puts "dims_selected $dims_selected"
    set level1 [ list ]
    foreach d $dims_selected {
      set level2 [ list ]
      foreach v $d {
        lappend level2 $v
      }
      if { [ llength $level2 ] } {
        lappend level1 "SetOfReal([ join $level2 , ])"
      } else {
        lappend level1 "Copy(Empty)"
      }
    }
    $self _invokeButton $met "SetOfSet([ join $level1 , ])"
    if { $ex } {
      after idle destroy $win
    }
  }

  method getDimSelected { } {
    set result [ list ]
    foreach w $widgets d $dimensions {
      lappend result [ $w getChecked ]
    }
    set result
  }

  method _getObjDimensions { } {
    puts "_getObjDimensions"
    puts "*** [ tol::console stack list ] ***"
    incr ids
    set id_obj __aux_dsel__$ids
    set id_dims __dimensions__$ids
    set try [ catch {
      tol::console eval [ string map [ list %A $options(-addr) %I1 $id_obj %I2 $id_dims ] {
        @MultiDimSelect %I1 = GetObjectFromAddress("%A");
        Set %I2 = %I1::getDimensions(?)
      } ] } msg ]
    if { $try } {
      puts "ERROR DimSelector::_getObjDimensions : $msg"
      set result ""
    } else {
      set result [ TolObj2TclObj [ list Set $id_dims ] ]
    }
    tol::console stack release $id_dims
    tol::console stack release $id_obj
    puts "_getObjDimensions: DONE"
    set result
  }
  
  method _getObjDimValues { dim } {
    incr ids
    set id_obj __aux_dsel__$ids
    set id_values __dimvalues__$ids
    set try [ catch {
      tol::console eval [ string map [ list %A $options(-addr) %D $dim %I1 $id_obj %I2 $id_values ] {
        @MultiDimSelect %I1 = GetObjectFromAddress("%A");
        Set %I2 = %I1::getDimValues("%D");
      } ] } msg ]
    if { $try } {
      puts "ERROR DimSelector::_getObjDimValues : $msg"
      set result ""
    } else {
      set result [ TolObj2TclObj [ list Set $id_values ] ]
    }
    tol::console stack release $id_values
    tol::console stack release $id_obj
    set result
  }

  method _getObjButtons { } {
    incr ids
    set id_obj __aux_dsel__$ids
    set id_buttons __buttons__$ids
    set try [ catch {
      tol::console eval [ string map [ list %A $options(-addr) %I1 $id_obj %I2 $id_buttons ] {
        @MultiDimSelect %I1 = GetObjectFromAddress("%A");
        Set %I2 = %I1::getButtonEntries(?);
      } ] } msg ]
    if { $try } {
      puts "ERROR DimSelector::_getObjButtons : $msg"
      set result ""
    } else {
      set result [ TolObj2TclObj [ list Set $id_buttons ] ]
    }
    tol::console stack release $id_buttons
    tol::console stack release $id_obj
    set result
  }

  method _createDimWidget { dim } {
    puts "_createDimWidget $dim"
    set f [ $win getframe ]
    set l [ llength $widgets ]
    set w [ LabelFrame $f.dimf${l} -text $dim -side top ]
    set ff [ $w getframe ]
    set tl [ $type createTableDim $ff.tl ]
    foreach dv [ $self _getObjDimValues $dim ] {
      $type appendRow $tl.tbl $dv
    }
    grid $tl -row 0 -column 0 -sticky snew
    grid rowconfigure $ff 0 -weight 1
    grid columnconfigure $ff 0 -weight 1
    lappend widgets $w
    puts "_createDimWidget : DONE"
  }

  method _packWidgets { } {
    puts "_packWidget $widgets"
    set c 0
    set f [ $win getframe ]
    foreach w $widgets {
      grid $w -row 0 -column $c -sticky snew
      grid columnconfigure $f $c -weight 1
      incr c
    }
  }
}

proc ShowDimSelector { addr } {
  puts "ShowDimSelector $addr"
  DimSelector .dimsel -addr $addr -geometry 200x200 -parent .
  puts "[ .dimsel cget -addr]"
  set result [ .dimsel draw ]
  puts "sali con $result"
}

#set addr [tol::info address {NameBlock guidim_test}]
#ShowDimSelector $addr