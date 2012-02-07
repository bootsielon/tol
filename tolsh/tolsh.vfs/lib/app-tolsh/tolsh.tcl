#!/usr/bin/env tclsh

###############################################################################
#
# tolsh.tcl -- 
#
###############################################################################

package provide tolsh 1.0

###############################################################################
#
#  Tol_HciWriter --
#
#  This file will be invoked by TOL if HCI output is enabled
#
###############################################################################
proc Tol_HciWriter { msg } {
  puts [string trim $msg]
}

###############################################################################
#
#  namespace tolsh -- encapsulate tolsh functionality
#
###############################################################################
namespace eval ::tolsh {
  variable options

  # By default load initLibrary
  #
  set options(lang)    0
  set options(initlib) 1
  set options(iniproject) 1
  set options(compile) ""
  set options(vmode)   ""
  set options(out,enabled) no
  set options(runmode) ""
  set options(tolversion) ""
}

###############################################################################
#
#  getoptions -- process the command line
#
#  The command line accepted by tolsh is:
#
#    -i       --> do not include initLibrary
#    -np      --> do not load _iniproject.tol
#    -c"..."  --> compile a tol expression
#    -c "..." --> compile a tol expression
#    -v?A?    --> enable all output
#    -vE      --> enable error output
#    -vS      --> enable system output
#    -vU      --> enable user output
#    -vT      --> enable trace output
#    -m?A?    --> disable all output
#    -mE      --> disable error output
#    -mS      --> disable system output
#    -mU      --> disable user output
#    -mT      --> disable trace output
#    -d       --> start in dialog mode
#    -e?n?    --> use english language
#    -s?p?    --> use spanish language
#    -tolversion ver --> specify an strict tol version to start with
#    -server [port]  --> start tol as a server tolsvc.<pid>.log will be the log
#             [port] is the port number where the serve will be listening.
#
#  Any other value will be considered a file to compile
#
#  Arguments:
#
###############################################################################
proc ::tolsh::getoptions { cmdline } {
  variable options

  logtmp "entering ::tolsh::getoptions $cmdline"

  # cmdline could be a bad list so the next sentence could fail.
  # try to test it in front of bugs situations.
  set num [llength $cmdline]
  set delayed_msg ""
  set i 0
  while {$i < $num} {
    set item [lindex $cmdline $i]
    if {[string index $item 0] eq "-"} {
      set opt [string range $item 1 end]
      if {$opt eq "e" || $opt eq "en"} {
        set $options(lang) 0
      } elseif {$opt eq "s" || $opt eq "sp"} {
        set $options(lang) 1
      } elseif {$opt eq "i"} {
        # initLibrary should not be loaded
        # 
        set options(initlib) 0
      } elseif {$opt eq "np"} {
        set $options(iniproject) 0
      } elseif {[string index $opt 0] eq "c"} {
        if {[string length $opt]>1} {
          # The tol expression is part of the option
          # 
          set expr [string range $opt 1 end]
        } else {
          # Next argument is the expression to compile
          #
          incr i
          set expr [lindex $cmdline $i]
        }
        # Put expr in the list of "things" to compile
        #
        lappend options(compile) [list expr $expr]
      } elseif {[string index $opt 0] eq "v"} {
        if {[regexp {^[A|S|U|T|E|W]*$} [string range $opt 1 end] mode]} {
          if {$mode eq ""} {
            set mode A
          }
          if {[regexp {[A|S]} $mode]} {
            set options(out,enabled) yes
          }
          append options(vmode) "v$mode "
        } else {
          append delayed_msg "\nignoring unknown verbose mode: '$opt'"
        }
      } elseif {[string index $opt 0] eq "m"} {
        if {[regexp {^[A|S|U|T|E|W]*$} [string range $opt 1 end] mode]} {
          if {$mode eq ""} {
            set mode A
          }
          if {[regexp {[A|S]} $mode]} {
            set options(out,enabled) no
          }
          append options(vmode) "m$mode "
        } else {
          append delayed_msg "\nignoring unknown mute mode: '$opt'"
        }
      } elseif {$opt eq "d"}  {
        if {$options(runmode) ne "" && $options(runmode) ne "interactive"} {
          puts "Replacing previous run mode '$options(runmode)' with 'interactive'"
        }
        set options(runmode) "interactive"
      } elseif {$opt eq "tolversion"} {
        incr i 
        set options(tolversion) [lindex $cmdline $i]
      } elseif {$opt eq "server"} {
        # check if next argument is the port number
        set port [lindex $cmdline [expr {$i+1}]]
        if {[string is integer $port]} {
          # $i+1 is an integer, should be considered as a port number
          incr i
          if {$port <= 0} {
            puts "Could not start as a server: invalid port number '${port}'"
            exit
          }
          if {$options(runmode) ne "" && $options(runmode) ne "server"} {
            puts "Replacing previous run mode '$options(runmode)' with 'server'"
          }
          set options(runmode) "server"
          set options(server,port) $port
        } else {
          puts "Could not start as a server: invalid port number '${port}'"
          exit
        }
      } elseif {$opt eq "slave"} {
        # the next argument is the port number
        set options(server,port) [lindex $cmdline [expr {$i+1}]]
        incr i
        if {$options(runmode) ne "" && $options(runmode) ne "slave"} {
          puts "Replacing previous run mode '$options(runmode)' with 'slave'"
        }
        set options(runmode) "slave"
      } elseif {$opt eq "shared"} {
        # the next argument is the port number
        set options(shared,port) [lindex $cmdline [expr {$i+1}]]
        incr i
        if {$options(runmode) ne "" && $options(runmode) ne "shared"} {
          puts "Replacing previous run mode '$options(runmode)' with 'shared'"
        }
        set options(runmode) "shared"
      }  elseif {$opt eq "shport"} {
        # the next argument is the port number
        set options(shared,port) [lindex $cmdline [expr {$i+1}]]
        incr i
      } else {
        append delayed_msg "\nignoring unknown option: '$item'"
      }
    } else {
      # Consider this option as a tol file to compile.
      #
      if {![file exists $item] || ![file readable $item]} {
        puts "file $item does not exist or is not readable"
      } else {
        lappend options(compile) [list file $item]
      }
    }
    incr i
  }
  if {$delayed_msg ne ""} {
    std $delayed_msg
  }
}

###############################################################################
#
#  something_to_compile -- tell us if there is something to do
#
#  Depending on the command line processing some tol expression or
#  files needs to be compiled. All this is stored in options(compile)
#
#  Return:
#
#    true value if there is something to compile in $options(compile)
#    false if $options(compile) is empty
#
###############################################################################
proc ::tolsh::something_to_compile { } {
  variable options

  llength $options(compile)
}

###############################################################################
#
#  show_usage -- show how to invoke tolsh
#
#  This proc print into stdout a message that list the valid options accepted.
#
###############################################################################
proc ::tolsh::show_usage { } {
  puts "[get_app_name] \[options] filename ...\noptions:"
  puts {
    -i file  --> include a file but not initLibrary
    -np      --> do not load _iniproject.tol
    -c "..." --> compile a tol expression
    -v?A?    --> enable all output
    -vE      --> enable error output
    -vS      --> enable standard output
    -vU      --> enable user output
    -vT      --> enable trace output
    -m?A?    --> disable all output
    -mE      --> disable error output
    -mS      --> disable standard output
    -mU      --> disable user output
    -mT      --> disable trace output
    -d       --> start in dialog mode
    -e?n?    --> use english language
    -s?p?    --> use spanish language
    -tolversion --> specify an strict tol version to start with
    -server [port] --> start tol as a server tolsvc.<pid>.log will be the log
            [port] is the port number where the serve will be listening.
 
  Multiple verbose mode is accepted in one option, for instance,

    -vUE
  }
}

###############################################################################
#
#  getArgv0 -- return the name of the app
#
###############################################################################
proc ::tolsh::get_app_name {} {
    global argv0

    set name [file tail $argv0]
    return [file rootname $name]
}

###############################################################################
#
#  append_message -- save a message 
#
###############################################################################
proc ::tolsh::std { msg } {
  variable options

  if {$options(out,enabled)} {
    puts $msg
  }
}

#puts "starkit = [namespace exist ::starkit]"
#puts "topdir = [file dir $starkit::topdir]"
#puts "info script = [info script]"
#puts "starkit::startup = [starkit::startup]"
#puts "[ info commands starkit::* ]"
#puts "argv0 = $argv0"

proc ::tolsh::setup_pkg { pkg } {
  #puts ":tolsh::setup_pkg $pkg [namespace exist ::starkit] "
  if {[lsearch [package names] $pkg]==-1} {
    if {[namespace exist ::starkit]} {
      set bindir [file dir $starkit::topdir]
      set libdir [file join $bindir .. lib]
      set pkg_dirs [glob -nocomplain -dir $libdir ${pkg}*]
      if {[llength $pkg_dirs]} {
        lappend ::auto_path [file normalize $libdir]
      } elseif { $::tcl_platform(platform) eq "unix" } {
          set activeTcl "/opt/ActiveTcl-8.4/lib"
          if { [ file exists $activeTcl ] &&
	       [ lsearch $::auto_path $activeTcl ] == -1 } {
            lappend ::auto_path $activeTcl
          }
          if { [ lsearch $::auto_path "/usr/lib" ] == -1 } {
            lappend ::auto_path "/usr/lib"
          }
      } else {
        set tclsh [auto_execok tclsh]
        if {![catch {
          exec echo puts {$::auto_path} | $tclsh
        } ext_auto_path]} {
          foreach p $ext_auto_path {
            if {[lsearch $::auto_path $p]==-1} {
              lappend ::auto_path $p
            }
          }
        }
      }
      tclPkgUnknown ${pkg}+1 ""
    } else {
      set pkg_dev_dir [file normalize [file join .. $pkg]]
      if {[file exists $pkg_dev_dir] && [file isdir $pkg_dev_dir]} {
        lappend ::auto_path $pkg_dev_dir
      }
    } 
  } 
}

###############################################################################
#
#  setup_autopath --
#
#  Set auto_path in order to find correctly the following packages:
#
#    - toltcl
#    - rmtps_client
#
###############################################################################
proc ::tolsh::setup_autopath { } {
  logtmp "entering ::tolsh::setup_autopath"

  setup_pkg toltcl
  setup_pkg tolcomm
  setup_pkg tlogger
  setup_pkg tequila
  setup_pkg tequilas

  logtmp "auto_path = $::auto_path"
  logtmp "leaving ::tolsh::setup_autopath"  
}

proc GetHomeDir { } {
  global env 
  # Check if we're using windows 
  if { $::tcl_platform(platform) eq "windows" } {
    package require registry 1.0
    set key1 {HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\User Shell Folders}
    set key2 {Personal}
    set env_home [ registry get $key1 $key2 ]
    if {[string match %USERPROFILE%* $env_home]} {
      set env_home [string replace $env_home 0 12 $env(USERPROFILE)]
    } 
  } else {
    set env_home $env(HOME)
  }
  return $env_home
}

###############################################################################
#
#  main -- entry point for tolsh application
#
###############################################################################
proc ::tolsh::run { cmdline } {
  variable options

  #logtmp "user : [ exec whoami ]"
  logtmp "entering run with cmdline = $cmdline"
  # Process command line options
  #
  getoptions $cmdline
  logtmp "options(compile): $options(compile)"
  logtmp "options(runmode): $options(runmode)"
  if {![something_to_compile] && ($options(runmode) eq "")} {
    show_usage
    return
  }
  
  logtmp "pase getoptions"

  # Ensure Toltcl is found
  #
  setup_autopath

  # Load Toltcl (+tol) and initLibrary if required
  #
  if {$options(runmode) != "server" && $options(runmode) != "shared"} {
    if {[info exist ::env(USE_TOLTCL_202)]} {
      puts "requested to load Toltcl from $::env(USE_TOLTCL_202)"
      load $::env(USE_TOLTCL_202)
    } else {
      package require -exact Toltcl 3.1
    }
    tol::initkernel $options(lang) $options(vmode)

    if {$options(initlib)} {
      tol::initlibrary $options(iniproject)
    }

    set appdata [string trim \
               [lindex [::tol::info var [list Text TolAppDataPath]] 2] \"]
    logtmp "appdata = $appdata"
  }

  set tolcomm_dir [file join [GetHomeDir] ".tol" "tolcomm"] 
  file mkdir $tolcomm_dir

  # load rmtps_client and tolclient if required
  #
  if {$options(runmode) != "server" && $options(runmode) != "shared"} {
  
    if {[catch {package require rmtps_client} msg]} {
      std "could not load rmtps_client: $msg"
    } else {
      ::rmtps_client::init [file join $tolcomm_dir "rmtps_client.log"]
      if {[catch rmtps_client::define_TOLAPI msg]} {
        std "could not load tol code for rmtps_client: $msg"
      }
    }
  
    if {[catch {package require tolclient} msg]} {
      std "could not load tolclient: $msg"
    } else {
      ::tolclient::init [file join $tolcomm_dir "tolclient.log"]
      if {[catch ::tolclient::define_TOLAPI msg]} {
        std "could not load tol code for rmtps_client: $msg"
      }
    }
    
  }

  # Compile expression and tol file provided in the command line if required
  #
  if {$options(runmode) != "server" && $options(runmode) != "shared" \
                                    && $options(runmode) != "slave" } {

    foreach item $options(compile) {
      if {[lindex $item 0] eq "expr"} {
        tol::console eval [lindex $item 1]
      } elseif {[lindex $item 0] eq "file"} {
        tol::include [lindex $item 1]
      }
    }

  }
  
  if {$options(runmode) eq "interactive"} {
    # assign our event handler for stdin
    fileevent stdin readable ::tolsh::stdin_handler
    # send a startup message and initial prompt
    puts stdout "TOL interactive shell activated..."
    console_prompt
    # enter the tcl event loop
    vwait __forever__
    
  } elseif {$options(runmode) eq "server"} {
    puts "runmode=server"
    run_as_server $tolcomm_dir
    
  } elseif {$options(runmode) eq "shared"} {
    puts "runmode=shared"
    run_as_shared $tolcomm_dir
  
  } elseif {$options(runmode) eq "slave"} {
    puts "runmode=slave"
    run_as_slave $tolcomm_dir   
  }
}

proc ::tolsh::run_as_server {tolcomm_dir} {
  variable options
  package require tolserver

  set shared_port [ clone ]
  tolserver::start -port $options(server,port) \
    -shport $shared_port \
    -logfile [file join $tolcomm_dir "tolserver.log"] \
    -compile $options(compile)
  vwait __forever__
}   

proc ::tolsh::find_shared_port { port } {
  puts "::tolsh::find_shared_port $port"
  set i 0
  while { $i < 100 } {
    if { [ catch { socket localhost $port } fd ] } {
      puts "find_shared_port OK = $port"
      return $port
    }
    incr port
    incr i
  }
  puts "find_shared_port FAIL!"
  return 0
}

proc ::tolsh::clone {} {
  variable options

  puts "::tolsh::clone"
  set tolsh [list [info nameofexecutable] ]
  if { $starkit::tolsh_mode ne "starpack" } {
    lappend tolsh $::argv0
  }
  set shared_port [ find_shared_port 20458 ]
  if { !$shared_port } {
    puts "::tolsh::clone: could not find a valid port for shared data"
    exit
  }
  eval exec $tolsh -shared $shared_port &
  after 5000            ;# wait for data sharing are created by 'shared'
  puts "exec slave"
  eval exec $tolsh -slave $options(server,port) -shport $shared_port &
  return $shared_port
}

proc ::tolsh::run_as_shared { tolcomm_dir } {
  package require tolshared
  variable options

  tolshared::start $options(shared,port) $tolcomm_dir
}   

proc ::tolsh::run_as_slave { tolcomm_dir } {
  variable options
  package require tolslave

  tolslave::start -port $options(server,port) -shport $options(shared,port) \
    -logfile [file join $tolcomm_dir "tolslave.log"]
  vwait __forever__
}   

proc ::tolsh::console_cmd { cmd } {
  if {$cmd eq "END" || $cmd eq "QUIT" || $cmd eq "EXIT"} {
    exit
  } else {
    puts stdout "unknown console command: '$cmd'"
  }
}

proc ::tolsh::console_eval { tol_stms } { 
  set con_objs0 [tol::console stack list]
  tol::console eval $tol_stms
  set con_objs1 [tol::console stack list]
  if {[llength $con_objs1]>[llength $con_objs0]} {
    puts stdout [lindex [lindex $con_objs1 end] 2]
  }
}

proc ::tolsh::console_prompt {} {
    puts -nonewline stdout \
        "[clock format [clock seconds] -format %H:%M:%S] TOL> "
    flush stdout
}

proc ::tolsh::stdin_handler {} {
  # get the data from stdin
  #
  gets stdin data
  if {[string index $data 0] eq "\\"} {
    console_cmd [string toupper [string range $data 1 end]]
  } else {
    console_eval $data
  }
  # return a new pseudo-prompt
  console_prompt
  return
}

proc logtmp { msg } {
  return 
  set user $::tcl_platform(user)
  set fd [ open "/tmp/tolsh.${user}.log" a ]
  puts $fd $msg
  flush $fd
  close $fd
}

::tolsh::run $argv
