proc GetTolValue {N {G Real}} {
  lindex [tol::info variable $G $N] 0
}

proc HowM {N C} {
  return "$N $C[expr {$N==1?{}:{s}}]"
}

proc HowE {N} {
  HowM $N error
}

proc HowW {W} {
  HowM $W warning
}

proc WriteTestBst {bst quality desc {E 0} {W 0}} {
  set fd [open $bst "w"]
  puts $fd "strToltclStatus;Errors;Warnings;quality_;partial_;"
  set desc [regsub -all "\"" $desc "\\\""]
  puts $fd "tt_result;$E;$W;$quality;\"$desc\";"
  close $fd
  #puts "toltcl test ended with: [HowE $E], [HowW $W] and quality [expr {$quality*100}] %"
}

if {$argc != 2} {
  error "invalid arguments should be 'runttt path-to-test path-to-result'"
}

foreach {test_file result_bst} $argv break

set result_bst [file normalize $result_bst]
set dir_result [file dir $result_bst]
set test_file [file normalize $test_file]

if {!([file isdir $dir_result] && [file writable $dir_result])} {
  error "result path $dir_result is nor a directory or is not writable"
}
if {![file exists $test_file]} {
  WriteTestBst $result_bst 0 "test file $test_file does not exist"
  return
}

set version "1.1.7"

#puts "Loading Toltcl $version ... "
if {[catch {
  package require -exact Toltcl $version
  tol::initkernel
  tol::console eval [string map [list %L [file join $dir_result "tt_test.log"]] {
    Text PutDumpFile("%L")
  }]
  tol::initlibrary

  proc Tol_HciWriter {msg} {puts $msg}
} msg]} {
  WriteTestBst $result_bst 0 "could not initialize toltcl $version : $msg"
  return
}
#puts "Toltcl $version loaded"

set tol_E0 [GetTolValue NError]
set tol_W0 [GetTolValue NWarning]
namespace eval _toltcl_test {
  #puts "Running Toltcl test $test_file ...."
  if {[catch {source $test_file} msg]} {
    WriteTestBst $result_bst 0 "fail while sourcing $test_file : $msg"
    return
  }
  if {![info exist quality]} {
    WriteTestBst $result_bst 0 "variable quality not defined in $test_file"
    return
  }
  if {!([string is double $quality]  && 
        [expr {$quality>=0 && $quality <=1}])} {
    WriteTestBst $result_bst 0 "variable quality is not a number or is out of range \[0..1\] in test_file"
    return
    
  }
  if {![info exist partial]} {
    WriteTestBst $result_bst 0 "variable partial not defined in $test_file"
    return
  }
  set tol_E [expr {[GetTolValue NError]-$tol_E0}]
  set tol_W [expr {[GetTolValue NWarning]-$tol_W0}]

  WriteTestBst $result_bst $quality $partial $tol_E $tol_W
}