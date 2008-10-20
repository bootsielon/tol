# This is a test file to verify the report
# http://bugs.tol-project.org/show_bug.cgi?id=470
#

tol::console eval {
  Real k = 2;
  Real k := k+1;
}

tol::console stack release
if {[llength [tol::console stack list]]} {
  set quality 0.0
  set partial "console stack should be empty"
} else {
  set quality 1.0
  set partial "all objects were successfully removed from console stack"
}

