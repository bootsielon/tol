#//////////////////////////////////////////////////////////////////////////////
# FILE    : byswidget.tcl
# PURPOSE : setup file for package byswidget. When version chabes the correct
#           version numbre must be written in the variable version, then create
#           pkgIndex.tcl invoking the proc byswidget::pkgCreate.
#
#//////////////////////////////////////////////////////////////////////////////

namespace eval byswidget {
  variable version 1.0
  variable dir [file dirname [info script]]
}

package provide byswidget $byswidget::version

proc byswidget::pkgCreate { } {
  variable version
  variable dir

  foreach f [glob -tails -dir $dir *.tcl] {
    if {$f ne "pkgIndex.tcl"} {
      append source "-source $f "
    }
  }
  set fd [open [file join $dir pkgIndex.tcl] w]
  puts $fd "# Tcl package index file, version 1.1\n"
  puts $fd [eval pkg::create -name byswidget -version $version $source]
  close $fd
}

proc crt_pkg { } {
  foreach d [glob -types d *] {
    puts "processing $d"
    if {[llength [glob -nocomplain -dir $d *.tcl]]} {
      cd $d
      pkg_mkIndex -verbose . *.tcl
      cd ..
    }
  }
}