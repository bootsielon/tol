# Tcl package index file, version 1.1

proc LoadToltcl { version dir } {
  global tcl_platform
  
  set suffix [info sharedlibextension]
  if {$tcl_platform(platform) eq "unix"} {
    set name libtoltcl${version}[info sharedlibextension]
  } else  {
    set name toltcl.dll
  }
  load [file join $dir $name] Toltcl
}

set version "2.0.1"

package ifneeded Toltcl $version [list LoadToltcl $version $dir]

