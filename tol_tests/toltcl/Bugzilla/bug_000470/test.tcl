# This is a test file to verify the report
# http://bugs.tol-project.org/show_bug.cgi?id=470
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
  Real k = 2;
  Real n = 10;
  Set all = [[k,n]];
}

tol::console stack release
if {[llength [tol::console stack list]]} {
  set quality 0.0
  set partial "console stack should be empty"
} else {
  set quality 1.0
  set partial "all objects were successfully removed from console stack"
}

