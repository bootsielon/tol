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

package ifneeded Toltcl 3.1 \
    [ string map [list %d $dir] {
      LoadToltcl 3.1 {%d}
      source [ file join {%d} toltcl.tcl ]
    } ]

