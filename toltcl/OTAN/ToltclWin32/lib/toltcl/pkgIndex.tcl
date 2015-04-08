# Tcl package index file, version 1.1

proc LoadToltcl { version dir } {
  global tcl_platform
 
  set _version [ string map [ list "." "" ] $version ]
  if {[info exist ::env(USE_TOLTCL_$_version)]} {
    puts "requested to load Toltcl from $::env(USE_TOLTCL_$_version)"
    load $::env(USE_TOLTCL_$_version)
  } else {
    set suffix [info sharedlibextension]
    if {$tcl_platform(platform) eq "unix"} {
      set name libtoltcl[info sharedlibextension]
    } else  {
      set name toltcl.dll
    }
    load [file join $dir {libtoltcl.dll}] Toltcl
  }
}

package ifneeded Toltcl 3.2 \
    [ string map [list %d $dir] {
      LoadToltcl 3.2 {%d}
      source [ file join {%d} toltcl.tcl ]
    } ]

package ifneeded TolshApp 3.2 \
    [ string map [list %d $dir] {
      source [ file join {%d} tolsh.tcl ]
    } ]
