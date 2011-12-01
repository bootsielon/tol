  proc lshift {listvar} {
      upvar $listvar args
      
      if {[llength $args] == 0} {
          set arg ""
      } elseif {[llength $args] == 1} {
          set arg [lindex $args 0]
          set args ""
      } else {
          set arg [lindex $args 0]
          set args [lrange $args 1 end]
      }
      
      return $arg
  }