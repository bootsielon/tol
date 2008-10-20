# This is a test file to verify the report
# http://bugs.tol-project.org/show_bug.cgi?id=433
#

for {set i 0} {$i <= 100} {incr i} {
  tol::console eval {
    Real _TOLTK = 1.0
  }
  tol::console stack release _TOLTK
}

set quality 1.0
set partial "all _TOLTK objects could be created & destroyed"
