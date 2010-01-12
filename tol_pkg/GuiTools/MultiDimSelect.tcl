package require snit
package require BWidget
package require autoscroll
package require tablelist


snit::widgetadaptor DimSelector {

  option -addr -readonly yes -validatemethod _checkAddr

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

  typemethod _invokeButton { obj_addr method dims } {
    set try [ catch {
      tol::console eval [ string map [ list %A $obj_addr %M $method %D $dims ] {
        NameBlock __aux_instance__ = GetObjectFromAddress("%A");
        Real __aux_result__ = __aux_instance__::%M(%D)
      } ] } msg ]
    if { $try } {
      puts "ERROR ${type}::_invokeButton : $msg"
    }
    tol::console stack release __aux_instance__
    tol::console stack release __aux_result__
  }

  typemethod createTableDim { w } {
    frame $w
    
    set tbl [ ::tablelist::tablelist $w.tbl \
                  -columns { 0 "Selected" center 0 "Dimension Value" left } \
                  -editendcommand [ mytypemethod editEndCmd ] \
                  -editstartcommand [ mytypemethod editStartCmd ] \
                  -selectmode extended ]
    $tbl columnconfigure 0 -name selected -editable yes \
        -editwindow checkbutton \
        -formatcommand [ mytypemethod emptyStr ]
    if { [ $tbl cget -selectborderwidth ] == 0 } {
      $tbl configure -spacing 1
    }
    set hs [ scrollbar $w.hs -orient horizontal \
                 -command [ list $tbl xview ] ]
    set vs [ scrollbar $w.vs -orient vertical \
                 -command [list $tbl yview ] ]
    $tbl configure \
        -xscrollcommand "$hs set" \
        -yscrollcommand "$vs set"
    grid $tbl -row 0 -column 0 -sticky snew
    grid $vs -row 0 -column 1 -sticky ns
    grid $hs -row 1 -column 0 -sticky ew
    grid rowconfigure $w 0 -weight 1
    grid columnconfigure $w 0 -weight 1
    ::autoscroll::autoscroll $vs
    ::autoscroll::autoscroll $hs
    set w
  }

  typemethod getDimSelected { w } {
    $w finishediting
    set info [ $w get 0 end ]
    puts "getDimSelected, info : $info"
    set result [ list ]
    foreach r $info {
      if { [ lindex $r 0 ] } {
        lappend result [ lindex $r 1 ]
      }
    }
    set result
  }
  
  typemethod emptyStr { val } {
    return ""
  }

  typemethod editEndCmd { w row col text } {
    if { [ $w columncget $col -name ] eq "selected" } {
      set img [ expr { $text ? $DimSelector::checkedImg : $DimSelector::uncheckedImg } ]
      $w cellconfigure $row,$col -image $img
    }
    return $text
  }
  
  typemethod editStartCmd { w row col text } {
    return $text
  }

  typemethod appendRow { w dimValue } {
    $w insert end [ list 0 $dimValue ]
    $w cellconfigure end,selected -image $DimSelector::uncheckedImg
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
    installhull using Dialog -modal none -geometry 200x200
    $win add -text Accept -command [ mymethod _onAccept ]
    $win add -text Close -command "destroy $win"
    $self configurelist $args
    set widgets [ list ]
    set f [ $win getframe ]
    set main [ frame $f.main ]
    set dimensions [ $self _getDimensions ]
    foreach dim $dimensions {
      $self _createDimWidget $dim
    }
    $self _packWidgets
    grid rowconfigure $f 0 -weight 1
    grid columnconfigure $f 0 -weight 1
  }

  method _onAccept { } {
    set dims_selected [ $self _getDimSelected ]
    puts "dims_selected $dims_selected"
    set level1 [ list ]
    foreach d $dims_selected {
      set level2 [ list ]
      foreach v $d {
        lappend level2 \"$v\"
      }
      if { [ llength $level2 ] } {
        lappend level1 "SetOfText([ join $level2 , ])"
      } else {
        lappend level1 "Copy(Empty)"
      }
    }
    $type _invokeButton $options(-addr) onAccept "SetOfSet([ join $level1 , ])"
    after idle destroy $win
  }

  method _getDimSelected { } {
    set result [ list ]
    foreach w $widgets d $dimensions {
      set ff [ $w getframe ]
      lappend result [ $type getDimSelected $ff.tl.tbl ]
    }
    set result
  }

  method _getDimensions { } {
    puts "_getDimensions"
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
      puts "ERROR DimSelector::_getDimensions : $msg"
      set result ""
    } else {
      set result [ TolObj2TclObj [ list Set $id_dims ] ]
    }
    tol::console stack release $id_dims
    tol::console stack release $id_obj
    puts "_getDimensions: DONE"
    set result
  }
  
  method _getDimValues { dim } {
    puts "_getDimValues $dim"
    puts "*** [ tol::console stack list ] ***"
    incr ids
    set id_obj __aux_dsel__$ids
    set id_values __dimvalues__$ids
    set try [ catch {
      tol::console eval [ string map [ list %A $options(-addr) %D $dim %I1 $id_obj %I2 $id_values ] {
        @MultiDimSelect %I1 = GetObjectFromAddress("%A");
        Set %I2 = %I1::getDimValues("%D");
      } ] } msg ]
    if { $try } {
      puts "ERROR DimSelector::_getDimValues : $msg"
      set result ""
    } else {
      set result [ TolObj2TclObj [ list Set $id_values ] ]
    }
    tol::console stack release $id_values
    tol::console stack release $id_obj
    puts "_getDimValues: DONE"
    set result
  }

  method _createDimWidget { dim } {
    puts "_createDimWidget $dim"
    set f [ $win getframe ]
    set l [ llength $widgets ]
    set w [ LabelFrame $f.dimf${l} -text $dim -side top ]
    set ff [ $w getframe ]
    set tl [ $type createTableDim $ff.tl ]
    foreach dv [ $self _getDimValues $dim ] {
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
  DimSelector .dimsel -addr $addr -geometry 200x200 -parent .
  set result [ .dimsel draw ]
  puts "sali con $result"
}

#set addr [tol::info address {NameBlock guidim_test}]
#ShowDimSelector $addr