###############################################################################
#
#   tolclient.tcl -- Remote client side.
#
#   Implement the client communication routines with tolcomm via
#   sockets.
#
###############################################################################

package provide tolclient 1.0

package require tlogger

###############################################################################
#
#   tolclient - encapsulate the client code and variable in a
#   namespace
#
###############################################################################
namespace eval ::tolclient {

  variable data

  array set data {
    timeout 10
    error_reason ""
    mydir ""
    task,global ""
    counter,ask 0
    counter,batch 0
    cluster,queue ""
    cluster,launching 0
  }

  # data(timeout) := number of seconds to wait before canceling a
  # server's request request, by default it is set to 3 seconds. If
  # the maximun time is reached without answer from the server, the
  # server channel is closed and the request results in an error with
  # a timeout reason
  #

  #
  # data(timeout,id,$idrq) := timeout to be used when waiting response
  # from the server
  #

  #
  # data(result,$idrq) := variable to store the result read from the
  # server for ask $idrq
  #

  #
  # data(error_reason) := reason of the last error
  #
  
  #
  # Directory where I'm located. Will be used later to evaluate the
  # client part written in TOL
  #
  set data(mydir) [file normalize [file dir [info script]]]

  #
  # data(task,global) := # global list of pending tasks
  #

  #
  # data(task,chan,$chan) := list of pending tasks on '$chan'
  #

  #
  # data(conn,$chan) := map a connected channel to a server with
  # its IP,port this information is used to reconnect to the server in
  # case the connection is lost
  #

  #
  # data(conn,$host,$port) := map a host,port to an opened
  # channel. It's the inverse mapping of data(conn,$chan)
  #
}

proc ::tolclient::init { {logfile ""} } {
  ::tlogger::init tolclient $logfile
  ::tlogger::level tolclient "error"
}

proc ::tolclient::log { level msg } {
  
  ::tlogger::log tolclient $level $msg
}

###############################################################################
#
#   get_error_reason --
#
#
###############################################################################
proc ::tolclient::get_error_reason { } {

  variable data  
  set data(error_reason)
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
proc ::tolclient::set_server_timeout { timeout } {
  variable data

  old_timeout $data(timeout)
  set data(timeout) $timeout
  set old_timeout
}

###############################################################################
#
#   read_channel -- event to read result from server channel
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
#     TAG size data_for_tag
#
###############################################################################
proc ::tolclient::read_channel { chan } {
  variable data

  log "debug" "triggered read_channel $chan"

  if {![eof $chan]} {
    if {[gets $chan buffer] == -1} {
      # only handle complete lines
      log "debug" "read_channel only handle complete lines"
    } else {
      process_data $chan $buffer
    }
  } else {
    on_eof_channel $chan
  }
  log "debug" "leave : read_channel $chan"
}

proc ::tolclient::reconnect { host port tasks } {

  variable data

  log "debug" "reconnect $host $port $tasks"

  foreach idtask $tasks {
    if {[is_waiting $idtask] } {
      ask_server $host $port "RECONN" "$idtask"
    }
  }

  log "debug" "leave reconnect"
}

#############################################################################
# process_data --
#
#  Process a data 'buffer' received from channel 'chan'. Normally used
#  to process the end of a time consuming task previously requested to
#  a 'tol server'. It could process a synchronous event or an
#  asynchornous one.
# 
#############################################################################
proc  ::tolclient::process_data { chan buffer } {

  variable data

  log "debug" "process_data $chan $buffer"

  foreach {host port} $data(conn,$chan) break
  if {![regexp {([^\s]+)\s(\d+)\s(.+)$} $buffer ==> \
            cmd size str]} {
    log "emergency" \
        "process_data : from server ${host}:${port} : invalid answer format in '$buffer'"
    return
  }
  set rsize [string length $str]
  if {$size != $rsize} {
    log "emergency" \
        "process_data : from server ${host}:${port} : invalid data length : it was $size and should be $rsize"
    return
  }
  switch -- [string toupper $cmd] {
    CRITICAL {
      log critical "ERROR from server : $str"
    }
    ACKPING {
      # answer from a PING
      on_ackping $chan $str
    }

    RECONN {
      # asnwer from a reconnection
      log "debug" "process_data : RECONN $chan $str"
      if {[regexp {([^\s]+)\s(.+)$} $str ==> idrq idtask]} {
        on_reconnect $chan $idrq $idtask
      } else {
        log "emergency" \
            "process_data : from server ${host}:${port} : invalid RECONN arguments '$str'"
      }
    }
    
    TSTART {
      log "debug" "process_data : TSTART $chan $str"
      if {[regexp {([^\s]+)\s(.+)$} $str ==> idrq idtask]} {
        on_start_task $chan $idrq $idtask
      } else {
        log "emergency" \
            "process_data : from server ${host}:${port} : invalid TSTART arguments '$str'"
      }
    }

    TFINISH {
      log "debug" "process_data : TFINISH $chan $str"
      on_finish_task $chan $str
    }
  }
  log "debug" "leave process_data"
}

proc ::tolclient::get_conn_channel { host port } {
  
  variable data

  set data(conn,$host,$port)
}

proc ::tolclient::is_waiting { idrq } {

  variable data

  expr {[lsearch $data(task,global) $idrq]!=-1}
}

proc ::tolclient::get_tasks { chan } {

  variable data

  if {[info exists data(task,chan,$chan)]} {
    set data(task,chan,$chan)
  } else {
    list
  }
}

proc ::tolclient::has_pending { chan } {

  variable data

  if {[info exists data(task,chan,$chan)]} {
    llength $data(task,chan,$chan)
  } else {
    return 0
  }
}

proc ::tolclient::get_pending { chan } {

  variable data

  if {[info exists data(task,chan,$chan)]} {
    set data(task,chan,$chan)
  } else {
    list
  }
}

proc ::tolclient::on_eof_channel { chan } {

  variable data

  log "notice" "enter on_eof_channel $chan"

  # clear the channel state
  #
  if {[file channels $chan] ne ""} {
    fileevent $chan readable {}
    catch { close $chan }
  }

  cancel_asking $chan "EOC"

  launch_pending $chan

  clear_channel $chan

  log "notice" "leave on_eof_channel $chan"
}

proc ::tolclient::on_ackping { chan idrq } {

  variable data

  cancel_timeout $idrq

  # Cancel vwait
  #
  set data(vwait,ask,$idrq) ACKPING
}

#
# - insert a reconnected task into the list local to the channel
#
# - this proc is triggered by the event RECONN received on $chan
#
# - the RECONN event is in response to an ask_server generated by
#   this client, so a vwait variable should be canceled.
#
# revisar  todo lo relacionado con el reconnect.
#
proc ::tolclient::on_reconnect { chan idrq idtask } {
  
  variable data

  log "debug" "enter append_reconnect $chan $idrq $idtask"

  after cancel $data(timeout,id,$idrq)

  # if the task is still waiting
  if {[is_waiting $idtask]} {
    # add to the local channel list
    lappend data(task,chan,$chan) $idtask
    
    log "debug" "leave append_reconnect $chan $idrq $idtask"
    
    # Cancel vwait
    #
    # por aqui no esta pasando ya que esta sentencia tiene un error
    #
    set data(vwait,ask,$idrq) $result    
  } else {
    set data(vwait,ask,$idrq) ""
  }
}

proc ::tolclient::cancel_timeout { idrq } {

  variable data

  after cancel $data(timeout,id,$idrq)
  array unset data "timeout,id,$idrq"
}

#
# 
#
proc ::tolclient::on_start_task { chan idrq idtask } {
  
  variable data

  log "debug" "enter : on_start_task $chan $idrq $idtask"

  cancel_timeout $idrq

  # add the task to the global list of active tasks
  lappend data(task,global) $idtask
  
  # add the task to the local (to this channel) list of active tasks
  lappend data(task,chan,$chan) $idtask

  cluster_host_busy $chan

  log "debug" "leave : on_start_task $chan $idrq $idtask"

  # Cancel vwait
  #
  set data(vwait,ask,$idrq) $idtask
}

proc ::tolclient::cluster_host_busy { chan } {

  variable data

  foreach {host port} $data(conn,$chan) break
  set data(cluster,busy,$host,$port) 1
}

proc ::tolclient::cluster_host_finish { chan } {

  variable data

  foreach {host port} $data(conn,$chan) break

  puts "cluster_host_finish $host $port"

  set data(cluster,busy,$host,$port) 0
  if {[llength $data(cluster,queue)]} {
      after idle ::tolclient::cluster_launch
  }
}

proc ::tolclient::cluster_try_eval { tolExpr } {
  
  variable data

  foreach host_info $data(cluster,hosts) {
    foreach {host port} $host_info break
    if {!$data(cluster,busy,$host,$port)} {
      if {[cluster_try_eval1 $tolExpr $host $port]} {
        return 1
      }
    }
  }
  return 0
}

proc ::tolclient::cluster_try_eval1 { tolExpr host port } {
  
  variable data

  puts "cluster_try_eval1 $tolExpr $host $port"
  set task_id [ask_server $host $port "TOLEVAL" $tolExpr]

  puts "==> $task_id"

  expr {$task_id ne "TIMEOUT"}
}

#
# on_finish_task is triggered in response to the asyncronous event TFINISH
#
# - should cancel data(vwait,task) in case Wait is running
#
proc ::tolclient::on_finish_task { chan idtask } {

  variable data

  log "debug" "enter remove_task $chan $idtask"

  set idx [lsearch $data(task,chan,$chan) $idtask]
  if {$idx == -1} {
    log "emergency" "remove_task : task '$idtask' was not pending on channel $chan"
    set data(vwait,task) ""
  }
  set data(task,chan,$chan) [lreplace $data(task,chan,$chan) $idx $idx]
  set idx [lsearch $data(task,global) $idtask]
  if {$idx != -1} {
    set data(task,global) [lreplace $data(task,global) $idx $idx]
  }

  cluster_host_finish $chan

  log "debug" "leave remove_pending $chan $idtask"

  set data(vwait,task) $idtask
}

###############################################################################
#
#   on_timeout --
#
#   Process a timeout event generated while waiting for a an answer
#   from the server. See ask_server.
#
###############################################################################
proc ::tolclient::on_timeout { chan idrq } {
  
  variable data

  log debug "tiggered on_timeout $chan while waiting for $idrq"

  set result [catch {
    puts $chan "TCANCEL [string length $idrq] VOID $idrq"
    flush $chan
  } msgerr]
  if {$result} {
    log critical "on_timeout : $msgerr"
  }
  # Set result from server to "TIMEOUT" to return from vwait
  #
  array unset data "timeout,id,$idrq"
  set data(vwait,ask,$idrq) "TIMEOUT"
}

proc ::tolclient::clear_channel { chan } {

  variable data

  foreach {host port} $data(conn,$chan) break
  
  array unset data conn,$host,$port
  array unset data conn,$chan
  array unset data task,chan,$chan
  array unset data asking,$chan
}

proc ::tolclient::launch_pending { chan } {

  variable data

  foreach {host port} $data(conn,$chan) break
  set pending [get_pending $chan]
  if {[llength $pending]} {
    after idle [namespace code [list reconnect $host $port $pending]]
  }
}

proc ::tolclient::cancel_asking { chan result } {

  variable data

  if {[llength $data(asking,$chan)]} {
    set idreqs $data(asking,$chan)
    log "error" "on_eof_channel while waiting for answer for requests '$idreqs' from server"
    # Cancel vwait and timeouts
    #
    foreach idrq $idreqs {
      cancel_timeout $idrq
      set data(vwait,ask,$idrq) $result
    }
  }
}

proc ::tolclient::open_socket {host port} {
  variable data

  log "debug" "enter open_socket $host $port"

  set chan ""
  if {[info exists data(conn,$host,$port)]} {
    set chan $data(conn,$host,$port)
    if {[file channels $chan] ne ""} {
      log "debug" "open_socket : reusing channel $chan"
      set data(conn,$chan) [list $host $port]
    } else {
      log "debug" "open_socket : could not reuse channel $chan"
      set chan ""
    }
  }
  if {$chan eq ""} {
    log "debug" "open_socket : openning new channel"
    if {[catch {socket $host $port} chan]} {
      log "error" $chan
      set chan ""
    } else {
      set data(conn,$host,$port) $chan
      set data(conn,$chan) [list $host $port]
      set data(asking,$chan) [list]
      
      # buffer processing is line-oriented
      #
      fconfigure $chan -blocking 0 -buffering line
      
      log "debug" [fconfigure $chan]
      
      # Set up a fileevent to handle input from server
      # 
      fileevent $chan readable [namespace code "read_channel $chan"]
    }
  }
  log "debug" "leave : open_socket : '$chan'"
  return $chan
}

# no se usa
proc ::tolclient::close_conn {host port} {
  variable data

  if {[info exists $data(conn,$host,$port)]} {
    if {[file channels $data(conn,$host,$port)] ne ""} {
      set chan data(conn,$host,$port)
      if {[has_pending $chan]} {
        return 0
      } else {
        close $chan
      }
    }
  } else {
    set chan ""
  }
  array unset data conn,$host,$port
  array unset data conn,$chan
  array unset data asking,$chan
  return 1
}

proc ::tolclient::new_id_ask {host port chan} {
  variable data
  
  incr data(counter,ask)
  set idrq "${host}:${port}:$data(counter,ask)"
  lappend data(asking,$chan) $idrq
  return $idrq
}

###############################################################################
#
#   ask_server --
#
#   Send a request to the server 'host' which is listening at
#   'port'. It waits for and inmediate answer.
#
###############################################################################
proc ::tolclient::ask_server { host port TAG {str ""} } {
  
  variable data

  log "debug" "enter ask_server $host $port $TAG $str"

  # open channel to server
  #
  set chan [open_socket $host $port]

  if {$chan eq ""} {
    log "debug" "could not open socket ${host}:${port}"
    return ""
  }
  # Send the request to the server
  #
  set idrq [new_id_ask $host $port $chan]

  set request [string trimright "$TAG [string length $str] $idrq [string trim $str]"]
  puts $chan $request

  # Force buffer to be sent to server
  #
  flush $chan

  # Set up timeout handler
  #
  prepare_timeout $chan $idrq
  
  # Wait for result or timeout
  #
  vwait ::tolclient::data(vwait,ask,$idrq)

  set result $data(vwait,ask,$idrq)
  array unset data vwait,ask,$idrq
  set idx [lsearch $data(asking,$chan) $idrq]
  set data(asking,$chan) [lreplace $data(asking,$chan) $idx $idx]

  log "debug" "leave ask_server $result"

  set result
}

proc ::tolclient::prepare_timeout { chan idrq } {
  variable data

  set data(timeout,id,$idrq) [after [expr {1000*$data(timeout)}] \
                                  [namespace code "on_timeout $chan $idrq"]]
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
proc ::tolclient::server_ping { host port } {

  log "debug" "enter server_ping $host $port"

  # send request to server
  #
  set ans [ask_server $host $port "PING"]
  set result [regexp "ACKPING" $ans]
  
  log "debug" "leave server_ping : $result"

  set result
}

proc ::tolclient::server_eval { host port tolexpr } {
  
  variable data

  log "debug" "enter, server_eval $host $port $tolexpr"

  set answer [ask_server $host $port "TOLEVAL" $tolexpr]

  log "debug" "leave, server_eval : $answer"

  set answer
}

proc ::tolclient::cluster_eval { args } {
  log "debug" "enter, cluster_eval [llength $args]"

  set id_batch [cluster_schedule_tasks $args]

  log "debug" "leave, cluster_eval : $id_batch"

  set id_batch
}

proc ::tolclient::cluster_schedule_tasks { list_expr } {
  
  variable data

  incr data(counter,batch)
  eval lappend data(cluster,queue) $list_expr
  cluster_launch
  return "cbatch$data(counter,batch)"
}

proc ::tolclient::cluster_launch { } {

  variable data

  if {$data(cluster,launching)} return
  set data(cluster,launching) 1
  set idx_expr 0
  foreach tolExpr $data(cluster,queue) {
    if {![cluster_try_eval $tolExpr]} {
      break
    }
    incr idx_expr
  }
  if {$idx_expr} {
    set data(cluster,queue) [lrange $data(cluster,queue) $idx_expr end]
  }
  if {[llength $data(cluster,queue)]} {
    after idle ::tolclient::cluster_launch
  }
  set data(cluster,launching) 0
}

proc ::tolclient::cluster_new_host { host port } {

  variable data

  if {![info exists data(cluster,busy,$host,$port)]} {
    lappend data(cluster,hosts) [list $host $port]
    set data(cluster,busy,$host,$port) 0
  }
  puts $data(cluster,hosts)
}

proc ::tolclient::server_kill { host port } {
}

proc ::tolclient::server_wait { args } {

  variable data

  log "debug" "enter, server_wait $args"

  if {![llength $args]} {
    set waiting $data(task,global)
  } else {
    set waiting [list]
    foreach rq $args {
      if {[lsearch $data(task,global) $rq] != -1} {
        lappend waiting $rq
      }
    }
  }

  log "debug" "waiting for requests '$waiting'"

  while {[llength $waiting]} {
    vwait ::tolclient::data(vwait,task)
    set idx [lsearch $waiting $data(vwait,task)]
    if {$idx != -1} {
      set waiting [lreplace $waiting $idx $idx]
    }
  }

  log "debug" "leave, server_wait"
}

###############################################################################
#
#  define_TOLAPI --
#
#  This proc will evaluate in the protected stack the TOL code
#  corresponding to the client part of tolclient.
#
###############################################################################
proc ::tolclient::define_TOLAPI { } {
  variable data

  # check before if tolclient.tol has being loaded
  #
  if {[catch {::tol::info variable Code TolClientInitTcl}]} {
    set fd [open [file join $data(mydir) "tolclient.tol"]]
    set tol_code [read $fd]
    close $fd
    ::tol::console eval -hidden $tol_code
  }
}
