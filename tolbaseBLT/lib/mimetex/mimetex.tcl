package require base64

namespace eval mimetex {
  variable cache
  variable bin ""
  variable defaultBin

  array set cache {}

  array set defaultBin {
    Linux,4        linux32/mimetex
    Linux,8        linux32/mimetex
    Windows\ NT,4  win32/mimetex.exe
    Windows\ NT,8  win32/mimetex.exe
  }

  proc SetBinPath { path } {
    variable bin $path
  }

  proc RenderImage { latex { name "" } } {
    variable cache
    variable bin

    puts "RenderImage $latex"
    if { [ info exists cache($latex) ] } {
      return $cache($latex)
    }
    if { [ file executable $bin ] } {
      set p [open [list |$bin -d $latex] r]
      fconfigure $p -translation binary -encoding binary
      set rawData [base64::encode [read $p]]
      close $p
      if { $name eq "" } {
        set cache($latex) [ image create photo -data $rawData ]
      } else {
        set cache($latex) [ image create photo $name -data $rawData ]
      }
      return $cache($latex)
    } else {
      error "could not execute mimetex, $bin is not executable"
    }
  }

  
  set os $::tcl_platform(os)
  set wordSize $::tcl_platform(wordSize)
  if { [ info exists defaultBin($os,$wordSize) ] } {
    set _cwd [ file normalize [ file dir [ info script ] ] ]
    SetBinPath [ file join $_cwd $defaultBin($os,$wordSize) ]
  } else {
    puts "There is no default mimetex.cgi for os = $os and wordSize = $wordSize. You should set it up with SetBinPath"
  }
}

package provide mimetex 1.0
