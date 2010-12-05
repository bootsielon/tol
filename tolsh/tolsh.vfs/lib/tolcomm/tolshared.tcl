###############################################################################
#
#  tolshared.tcl -- communication layer of tolsh when acting as a shared.
#
###############################################################################

package provide tolshared 1.0

###############################################################################
#
#  tolshared - namespace to encapsulate the shared state and behaviour
#
###############################################################################

namespace eval ::tolshared {

}

###############################################################################
#
#  start -- Start the shared
#
###############################################################################
proc ::tolshared::start {} {
  global tqs_info
  package require Mk4tcl
  package require tequilas

  puts "::tolshared::start"
  mk::file open tqs tequilas.dat -nocommit
  set tqs_info(verbose) 0     ;# default logging is off
  TqsTimer ""                 ;# default commit timer is 30 seconds
  tqsRun 20458                ;# default port is 20458
}
