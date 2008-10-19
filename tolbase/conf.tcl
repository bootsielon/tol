#!/bin/sh
# -*- tcl -*- \
exec tclsh "$0" ${1+"$@"}

# --------------------------------------------------------------
# Perform various checks and operations on the distribution.

set distribution   [file dirname [info script]]
lappend auto_path  [file join $distribution lib]

source [file join $distribution tolbase_version.tcl] ; # Get version information.

catch {eval file delete -force [glob [file rootname [info script]].tmp.*]}


proc __test {} {
    global argv distribution

    set packname $argv
    if {[llength $packname] == 0} {
	set packname [packname]
    }
    
    if { [catch {set packver [package require $packname]} fid] } {
	set packver 0
    }

    if {$packver==0} {
	return [puts no]
    } else {
	return [puts yes]
    }
}

# --------------------------------------------------------------

set cmd [lindex $argv 0]
if {[llength [info procs __$cmd]] == 0} {
    puts stderr "unknown command $cmd"
    set fl {}
    foreach p [lsort [info procs __*]] {
	lappend fl [string range $p 2 end]
    }
    puts stderr "use: [join $fl ", "]"
    exit 1
}

set  argv [lrange $argv 1 end]
incr argc -1

__$cmd
exit 0
