#############################################

proc setini {file chap name val} {
if {![file exists $file]} {
   puts "INI::setini no such file \"$file\""
   retun
   }
set ifile [open $file]
set ofile [open $file.tmp w]
set chapfound [set found 0]
while {![eof $ifile]} {
      gets $ifile str
      if {$str != "" } {puts "w1 ofile <$str>"}
      if [regexp -- \\\[$chap\\\] [string trim $str]] {set chapfound 1; break}
      }
while {![eof $ifile]} {
      gets $ifile str
      if {[string trim [lindex [split $str =] 0]] == "$name"} {set found 1; puts $ofile $name=$val
         } else {
         if [regexp -- {(\[)([[:alnum:]])*(\])} [string trim $str]] {set found 1; puts $ofile $name=$val}
         if {$str != "" } {puts $ofile $str}
         }
      }
if {[eof $ifile] && !$found} {
   if {!$chapfound} {puts $ofile \[$chap\]}
   puts -nonewline $ofile $name=$val
   }
close $ifile
close $ofile
file rename -force ${file}.tmp $file
}

proc getini {file chap name} {
if {![file exists $file]} {
   puts "INI::getini no such file \"$file\""
   retun
   }
set ifile [open $file]
set chapfound 0
set res ""
while {![eof $ifile]} {
      gets $ifile str
      if [regexp -- \\\[$chap\\\] [string trim $str]] {set chapfound 1
         } else {
         if [regexp -- {(\[)([[:alnum:]])*(\])} [string trim $str]] {set chapfound 0}      
         }
      if {($chapfound) && ([string trim [lindex [split $str =] 0]] == "$name")} {
         set res [string trim [lindex [split $str =] 1]]
         break
         }           
      }
close $ifile
return $res
}

proc getchaps {file {chap ""}} {
if {![file exists $file]} {
   puts "INI::getini no such file \"$file\""
   retun
   }
set ifile [open $file]
set res ""
while {![eof $ifile]} {
      gets $ifile str
      if {$chap == ""} {
         if [regexp -- {(\[)([[:alnum:]])*(\])} [string trim $str]] {lappend res "[lindex [split $str \] ] 0]\]"}      
         } else {
         if [regexp -- \\\[$chap\\\] [string trim $str]] {lappend res "[lindex [split $str \] ] 0]\]"}
         }
      } 
return $res
}