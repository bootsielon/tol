namespace eval ::TolConfig {
  variable cwd [ file normalize [ file dir [ info script ] ] ]
}

proc ::TolConfig::GetConfig { } {
  return [ lindex [ toltcl::eval { 
    NameBlockToSet( TolConfigManager::Config )
  } -named 1 ] 1 ]
}

proc ::TolConfig::GetType { member } {
  set result [ toltcl::eval [ string map [ list %m $member ] {
    Text Grammar( %m )
  } ] ]
  if { $result eq "NameBlock" } {
    return "section"
  } else {
    return "member"
  }
}
