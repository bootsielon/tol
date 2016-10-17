###############################################################################
#
#   rmtps_client.tcl -- Remote process control client side.
#
#   Implement the client communication routines with RmtPS_Server via
#   sockets.
#
###############################################################################

package provide rmtps_client 1.0

package require base64

if { [ catch { package require tlogger } ] } {
  namespace eval tlogger {
    variable _level "error"

    proc init { svc {logfile ""} } {
    }
    
    proc level { svc l } {
      variable _level $l
    }

    proc log { svc l msg } {
      variable _level 

      if { $l eq $_level } {
        puts $msg
      }
    }
  }
}

###############################################################################
#
#   rmtps_client - encapsulate the client code and variable in a
#   namespace
#
###############################################################################
namespace eval ::rmtps_client {
  # timeout to be used when waiting response from the server
  variable timeout_id ""

  # number of seconds to wait before canceling a server's request
  # request, by default it is set to 3 seconds. If the maximun time is
  # reached without answer from the server, the server channel is
  # closed and the request results in an error with a timeout reason.
  variable server_timeout 10

  # variable to store the result read from the server
  variable server_result

  # reason of the last error
  variable error_reason ""

  variable asking_error 0

  # Directory where I'm located. Will be used later to evaluate the
  # client part written in TOL
  #
  variable mydir [file normalize [file dir [info script]]]

  
}

proc ::rmtps_client::init { {logfile ""} } {
  ::tlogger::init  rmtps_client $logfile
  ::tlogger::level rmtps_client "error"
}

proc ::rmtps_client::log { level msg } {
  
  ::tlogger::log rmtps_client $level $msg
}

###############################################################################
#
#   get_error_reason --
#
#
###############################################################################
proc ::rmtps_client::get_error_reason { } {

  variable error_reason
  
  set error_reason
}

###############################################################################
#
#   set_server_timeout -- change the current server's request timeout
#
#   Arguments --
#
#     timeout - maximun number of seconds to wait a request result from
#     the server.
#
#   Return --
#
#     the previous timeout value
#
###############################################################################
proc ::rmtps_client::set_server_timeout { timeout } {
  variable server_timeout

  old_timeout $server_timeout
  set server_timeout $timeout
  set old_timeout
}

###############################################################################
#
#   wait_result -- event to read result from server channel
# 
#     This procedure is invoked whenever an input is available on the
#     server channel. Every routine must use this event handler in
#     order to
#
#   Arguments --
#
#     chan - channel from which we are waiting a line response
#
#   Return --
#
###############################################################################
proc ::rmtps_client::wait_result { chan } {
  variable timeout_id
  variable server_result
  variable server_timeout 
  variable error_reason

  log "debug" "wait_result $chan"

  if {![eof $chan]} {
    if {[gets $chan data] == -1} {
      log "debug" "wait_result : incomplete data"
      return;			# only handle complete lines
    }
  } else {
    # Close the channel.
    #
    after cancel $timeout_id
    fileevent $chan readable {}
    set error_reason "end of channel from server"
    log "warn" "wait_result : end of channel from server"
    # Cancel vwait
    #
    set server_result ""
    return
  }
  after cancel $timeout_id
  fileevent $chan readable {}
  set error_reason ""
  # Return the data via the vwaited variable
  #
  log "debug" "wait_result read : $data"
  set server_result $data
}

###############################################################################
#
#   handle_timeout --
#
#
###############################################################################
proc ::rmtps_client::handle_timeout { chan } {
  variable server_result
  variable error_reason

  log "debug" "handle_timeout $chan" 

  # Set result from server to empty to return from vwait
  #
  set server_result ""
  
  # Set error reason to timeout
  #
  set error_reason "server timeout"
}

proc ask_server_channel { chan request } {

  usado sobre todo para pedir la razon del fallo

  el timeout quizas deba cerrar el channel, esa peticion se cancela

  file channels puedo usarlo para verificar si un socket existe.
}

###############################################################################
#
#   ask_server --
#
#
###############################################################################
proc ::rmtps_client::ask_server { host port request } {

  variable server_result
  variable server_timeout
  variable timeout_id
  variable error_reason
  variable asking_error

  log "debug" "enter ask_server $host $port $request"

  # open channel to server
  #
  set chan [socket $host $port]

  # We want whole lines
  #
  fconfigure $chan -blocking 0 -buffering line

  # Send the request to the server
  #
  puts $chan $request

  # Remember that server is line oriented
  #
  flush $chan

  # Set up a fileevent to handle input from server
  # 
  fileevent $chan readable [namespace code "wait_result $chan"]

  # Set up timeout handler
  #
  set timeout_id [after [expr {1000*$server_timeout}] \
                      [namespace code "handle_timeout $chan"]]
  
  # Wait for result or timeout
  #
  vwait ::rmtps_client::server_result
  
  catch {close $chan}

  log "debug" "enter ask_server $host $port $server_result"
  
  set server_result
}

###############################################################################
#
#   server_ping -- Check if a rmtps_server is listening on a remote
#   host
#
#   Arguments --
#
#     host - computer where RmtPS_Server is running
#     port - port where the server socket should be listening
#
#   Return --
#
#     1 if rmtps_server is running and listening
#     0 if rmtps_server does not answer or give back the wrong answer
#
###############################################################################
proc ::rmtps_client::server_ping { host port } {

  log "debug" "enter server_ping $host $port"

  # send request to server
  #
  set ans [ask_server $host $port "PING"]
  set result [expr {$ans eq "RMTPS_SERVER WORKING"}]

  log "debug" "leave server_ping $host $port"

  set result
}

###############################################################################
#
#   ps_is_active -- Check if a process is active on a remote host
#
#   Arguments --
#
#     host - computer where RmtPS_Server is running
#     port - port where the server socket is listening
#     PID  - process identifier to check for activity
#
#   Return --
#
#     1 if process given by PID is active
#     0 if process given by PID is not active
#    -1 there was an error during the operation, don't know if its alive
#
###############################################################################
proc ::rmtps_client::ps_is_active { host port PID } {

  log "debug" "enter ps_is_active $host $port $PID"

  # send request to server
  #
  set result [ask_server $host $port "ALIVE $PID"]

  log "debug" "from ask_server = '$result'"

  # Answer from server must be an integer value
  #
  if { $result eq "" || ![string is integer $result] ||
      ![string is boolean $result] } {
    set result -1
  } else {
    # Make sure 0 or 1 is returned
    #
    set result [expr {!!$result}]
  }

  log "debug" "leave ps_is_active $host $port $PID : $result"

  set result
}

###############################################################################
#
#  ps_kill -- terminate a given process on a remote host
#
#   Arguments --
#
#     host - computer where RmtPS_Server is running
#     port - port where the server socket is listening
#     PID  - process identifier to terminate
#
#   Return --
#
#     1 if process given by PID could be terminated
#     0 if process given by PID could not be terminated
#
#    -1 if there was an error during the kill, in any case don't know
#       is the process was killed
#
###############################################################################
proc ::rmtps_client::ps_kill { host port PID } {

  log "debug" "enter ps_kill $host $port $PID"

  # send request to server
  #
  set result [ask_server $host $port "KILL $PID"]
  # Answer from server must be an integer or boolean value
  #
  if {![string is integer $result]} {
    set result -1
  } else {
    # Make sure 0, 1 or -1 is returned
    #
    set result [expr {$result==-1?-1:$result!=0}]
  }

  log "debug" "leave ps_kill $host $port $PID : $result"

  set result
}

###############################################################################
#
#  ps_run -- execute a program on a remote host
#
#   Arguments --
#
#     host     - computer (remote host) where RmtPS_Server is running
#     port     - port where the server socket is listening
#     cmdline  - command line (program + arguments) to run on a remote host
#
#   Return --
#
#     PID of the process if cmdline could be executed
#     0 if the command line could not be executed
#
###############################################################################
proc ::rmtps_client::ps_run { host port cmdline } {

  log "debug" "enter ps_run $host $port $cmdline"

  # send request to server
  #
  set result [ask_server $host $port "RUN $cmdline"]
  # Make sure that 0 or PID>0 is returned
  #
  if {![string is integer $result] || $result<0} {
    set result 0
  }

  log "debug" "leave ps_run $host $port $cmdline : $result"

  set result
}


###############################################################################
#
#  ps_runtol -- execute a TOL process on a remote host
#
#   Arguments --
#
#     host        - computer (remote host) where RmtPS_Server is running
#     port        - port where the server socket is listening
#     cmdln_args  - TOL arguments
#
#   Return --
#
#     PID of the process if cmdline could be executed
#     0 if the command line could not be executed
#
###############################################################################
proc ::rmtps_client::ps_runtol { host port cmdln_args } {

  log "debug" "enter ps_runtol $host $port $cmdln_args"

  # send request to server
  #
  set cmdln64 [ base64::encode -wrapchar "" $cmdln_args ]
  #set cmdln64 $cmdln_args
  set result [ask_server $host $port "RUNTOL $cmdln64"]
  # Make sure that 0 or PID>0 is returned
  #
  if {![string is integer $result] || $result<0} {
    set result 0
  }

  log "debug" "leave ps_runtol $host $port $cmdln_args : $result"

  set result
}

###############################################################################
#
#  define_TOLAPI --
#
#  This proc will evaluate in the protected stack the TOL code
#  corresponding to the client part of rmtps.
#
###############################################################################
proc ::rmtps_client::define_TOLAPI { } {
  variable mydir

  # check before if rmtps_client.tol has being loaded
  #
  if {[catch {::tol::info variable Code RemoteProcessInitTcl}]} {
    set fd [open [file join $mydir "rmtps_client.tol"]]
    set tol_code [read $fd]
    close $fd
    tol::console eval -hidden $tol_code
  }
}