# This is a test file to verify the report
# http://bugs.tol-project.org/show_bug.cgi?id=433
#
# - a timeset expression is created in the console
# - a timeset iterator is created for that timeset
# - try to release the console stack
# - verify that the imeset is still in the stack
# - destroy the timeset iterator
# - try again to release the console stack
# - check that the stack is empty and the timeset does not exists
#

proc on_stack { name } {
  foreach obj  [tol::console stack list] {
    if {[lindex $obj 1] eq "tms1"} {
      return 1
    }
  }
  return 0
}

tol::console eval {
  TimeSet tms1 = D(1);
  TimeSet tms2 = D(2);
}
tol::timeset create iter_tms1 tms1
tol::console stack release stack
if {![on_stack "tms1"]} {
  set quality 0.0
  set partial "tms1 was released from the console stack unexpectly"
} else {
  tol::timeset destroy iter_tms1
  tol::console stack release
  if {[on_stack "tms1"]} {
    set quality 0.0
    set partial "tms1 is still on console stack"
  } else {
    set quality 1.0
    set partial "all timeset successfully removed from console stack"
  }
}
