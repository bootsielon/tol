namespace eval toltcl {

  proc init { } {
    variable idcounter

    foreach g [ tol::info grammars ] {
      set idcounter($g) 0
    }
  }

  if { [ info commands initkernel ] eq "" } {
    rename ::tol::initkernel initkernel
  }
  
  proc ::tol::initkernel { args } {
    eval toltcl::initkernel $args
    toltcl::init
  }  
}

proc toltcl::genid { Type } {
  variable idcounter

  return "__tmp_${Type}_[ incr idcounter($Type) ]__"
}

proc toltcl::eval { TolScript args } {
  set id [ genid Anything ]
  set script [ string map \
                   [ list %s% [ string trim $TolScript " \t\n;" ] %v% $id ] { Set %v% = { Set tmp = { [[ %s% ]] }; Set [[ tmp[Card(tmp)] ]] } } ]
  tol::console eval $script
  set idx [ llength [ tol::console stack list ] ]
  #puts [ tol::info var [ list "Console" $idx ] ]
  #set result [ ::eval tolist [ list [ list $_Type $id ] ] $args ]
  set result [ ::eval tolist [ list [ list "Console" $idx ] ] $args ]
  tol::console stack release $id
  array set opts {
    -named 0
  }
  return $result
  array set opts $args
  if { $Type eq "Set" || $Type eq "NameBlock" } {
    return $result
  } elseif { $opts(-named) } {
    return [ lrange $result end-1 end ]
  } else {
    return [ lindex $result end ]
  }
}

proc toltcl::__tolist { tolobj named recursive } {
  set vinfo [ tol::info variable $tolobj ]
  set grammar [ lindex $vinfo 0 ]
  if { $grammar eq "Set" || $grammar eq "NameBlock" } {
    set result [ list ]
    set idx 1
    tol::foreach it $tolobj {
      array set it_info $it
      if { $named } {
        lappend result $it_info(name)
      }
      lappend result [ __tolist [ linsert $tolobj end $idx ] $named $recursive ]
      incr idx
    }
    return $result
  } else {
    if { $grammar eq "Text" } {
      set contents [ lindex $vinfo 2 ]
      regexp {^"(.*)"$} $contents ==> retval
    } else {
      set retval [ lindex $vinfo 2 ]
    }
    if { $named && [ llength $tolobj ] < 3 } {
      return [ list [ lindex $vinfo 1 ] $retval ]
    } else {
      return $retval
    }
  }  
}

proc toltcl::tolist { tolobj args } {
  array set opts {
    -named 0
    -recursive 1
  }
  array set opts $args
  set result [ __tolist $tolobj $opts(-named) $opts(-recursive) ]
}

proc toltcl::test_00 { } {
  toltcl::eval Set {
    Set TolConfigManager::Config::Upgrading::TolPackage::Repositories;
  }
}

proc toltcl::test_01 { } {
  toltcl::eval Set {
    TolPackage::Client::_.versSyncInfo
  }
}

proc toltcl::test_03 { } {
  puts toltcl::eval {Real a = 1; Set kk = [[ a ]]; Real b = Card(kk)+1} -named 1
}
