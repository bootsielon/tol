package provide tlogger 0.5

namespace eval ::tlogger {
  variable services [list]
  variable data

  array set data {
    logfile ""
    channel stdout
    level notice
    levels { debug info notice warn error critical alert emergency }
  }
  set data(lvidx) [lsearch $data(levels) $data(level)]
}

proc ::level_str { level } {
  string toupper $level
}

proc ::tlogger::init { svc {logfile ""}} {

  variable services
  variable data
  upvar \#0 ::tlogger::data_${svc} mydata

  array set mydata [array get data]
  if {[lsearch $services $svc]==-1} {
    lappend services $svc
  }

  set logfile [file normalize $logfile]
  if {$mydata(logfile) ne $logfile} {
    if {$mydata(channel) ne "stdout"} {
      catch {
        close $mydata(channel)
      }
    }
    set mydata(logfile) $logfile
    if {$logfile eq ""} {
      set mydata(channel) "stdout"
    } else {
      if {[catch {
        open $logfile {WRONLY CREAT APPEND}
      } result]} {
        puts "::tlogger::init : $result"
      } else {
        set mydata(channel) $result
      }
    }
  }
}

proc ::tlogger::close { svc } {

  upvar \#0 ::tlogger::data_${svc} mydata

  flush $mydata(channel)
  if {$mydata(channel) ne "stdout"} {
    catch {
      close $mydata(channel)
    }
  }
  array unset mydata
}

proc ::tlogger::apache_time { } {
  # APACHE
  # Tue Nov 28 11:14:04 2006
  clock format [clock seconds] -format "%a %b %d %H:%M:%S %Y"
}

proc ::tlogger::level { svc {level ""} } {

  upvar \#0 ::tlogger::data_${svc} mydata

  if {$level eq ""} {
    set mydata(level)
  } else {
    set lvidx [lsearch $mydata(levels) $level]
    if {$lvidx==-1} {
      return ""
    } else {
      set cur $mydata(level)
      set mydata(level) $level
      set mydata(lvidx) $lvidx
      set cur
    }
  }
}

proc ::tlogger::log { svc level msg } {

  variable services

  if {[lsearch $services $svc]!=-1} {
    upvar \#0 ::tlogger::data_${svc} mydata
    
    set lvidx [lsearch $mydata(levels) $level]
    if {$lvidx >= $mydata(lvidx)} {
      puts $mydata(channel) "[apache_time], $svc\[[pid]\], [level_str $level], $msg"
      flush $mydata(channel)
    }
  }
}
