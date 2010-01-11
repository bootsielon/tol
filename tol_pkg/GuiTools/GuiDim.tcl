package require snit
package require BWidget

snit::widgetadaptor DimSelector {

  option -addr -readonly yes -validatemethod _checkAddr

  method _checkAddr { option value } {
    if { ![ Tol_ObjIsClassOf $value "@MultiDimSelect" ] } {
      puts "$value is not a valid @MultiDimSelect object"
      error "$value is not a valid @MultiDimSelect object"
    }
  }

  delegate option * to hull
  delegate method * to hull

  variable widgets

  constructor { args } {
    installhull using Dialog
    $win add -text Accept
    $win add -text Cancel
    $self configurelist $args
    set widgets [ list ]
    foreach dim [ $self _getDimensions ] {
      $self _createDimWidget $dim
    }
    $self _packWidgets
  }

  method _getDimensions { } {
    set try [ catch {
      tol::console eval [ string map [ list %A $options(-addr) ] {
        @MultiDimSelect __aux_dsel__ = GetObjectFromAddress("%A");
        Set __dimensions__ = __aux_dsel__::getDimensions(?)
      } ] } msg ]
    if { $try } {
      puts "ERROR DimSelector::_getDimensions : $msg"
      set result ""
    } else {
      set result [ TolObj2TclObj {Set __dimensions__} ]
    }
    tol::console stack release __aux_dsel__
    tol::console stack release __dimensions__
    set result
  }
  
  method _getDimValues { dim } {
    set try [ catch {
      tol::console eval [ string map [ list %A $options(-addr) %D $dim ] {
        @MultiDimSelect __aux_dsel__ = GetObjectFromAddress("%A");
        Set __dimvalues__ = __aux_dsel__::getDimValues("%D")
      } ] } msg ]
    if { $try } {
      puts "ERROR DimSelector::_getDimValues : $msg"
      set result ""
    } else {
      set result [ TolObj2TclObj {Set __dimvalues__} ]
    }
    tol::console stack release __aux_dsel__
    tol::console stack release __dimvalues__
    set result
  }

  method _createDimWidget { dim } {
    puts "_createDimWidget $dim"
    set f [ $win getframe ]
    set l [ llength $widgets ]
    set w [ LabelFrame $f.dimf${l} -text $dim -side top ]
    set ff [ $w getframe ]
    listbox $ff.lb -bd 1 -relief solid -selectmode extended
    eval $ff.lb insert end [ $self _getDimValues $dim ]
    grid $ff.lb -row 0 -column 0 -sticky snew
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
  DimSelector .dimsel -addr $addr
  .dimsel draw
  destroy .dimsel
}