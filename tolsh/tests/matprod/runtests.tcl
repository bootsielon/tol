#!/usr/bin/env tclsh

if {$argc != 1} {
  puts "usage: [lindex $argv 0] num"
} else {
  set num [lindex $argv 0]
  if {![string is integer $num] || $num <= 0} {
    puts "invalid number of configurations '$num': must be positive integer"
  } else {
    file delete partime.bbm seqtime.bbm norm.bbm
    for {set i 1} {$i <= $num} {incr i} {
      file delete A.bbm B.bbm
      set sz [expr {$i*100}]
      set out [exec tolsh -v \
                   -c "Real M=${sz};K=${sz};N=${sz}" \
                   partest.tol]
      puts $out
      set out [exec tolsh -v \
                   -c "Real M=${sz};K=${sz};N=${sz}" \
                   seqtest.tol]
      puts $out
      set out [exec tolsh -v check.tol]
      puts $out
    }
  }
}
