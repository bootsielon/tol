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
proc ::tolshared::start { port datadir } {
  global tqs_info
  package require Mk4tcl
  package require tequilas

  puts "::tolshared::start"
  # OJO: este archivo hay abrirlo en algun directorio controlado
  mk::file open tqs [ file join $datadir tequilas.${port}.dat ] -nocommit
  set tqs_info(verbose) 0     ;# default logging is off
  TqsTimer ""                 ;# default commit timer is 30 seconds
  tqsRun $port                ;# default port is 20458
}
