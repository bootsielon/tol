###############################################################################
#
#  tolslave.tcl -- communication layer of tolsh when acting as a slave.
#
###############################################################################

package provide tolslave 1.0

package require tlogger

###############################################################################
#
#  tolslave - namespace to encapsulate the slave state and behaviour
#
###############################################################################

namespace eval ::tolslave {

  variable data
  
  array set data {
    queue,tasks  ""
    queue,cancel ""
  }
}

global shared_data
#set shared_data(ack) 0
set shared_data(slave_status) "busy"

###############################################################################
#
#  log -- write a log message into the slave log file
#
#  This proc write a message into the server log channel which is
#  handled by tlogger
#
#  Arguments:
#
#    msg - the message to be written
#
###############################################################################
proc ::tolslave::log { level msg } {
  
  ::tlogger::log "tolslave" $level $msg
}

###############################################################################
#
#  start -- Start the slave
#
###############################################################################
proc ::tolslave::start {args} {
  global shared_data

  array set options {
    -port 6669
    -logfile ""
  }
  array set options $args

  ::tlogger::init tolslave $options(-logfile)
  ::tlogger::level tolslave "debug"
  
  # redirect the log
  ::tol::console eval [string map "%P $options(-port)" {
    Text {
      Text srv_root = TolAppDataPath+"/server";
      Real OSDirMake(srv_root);
      Text PutDumpFile(srv_root+"/start.%P.log")
    }
  }]

  slave_attach_data

  # Compile expression and tol file provided in the command line
  #
  if {$shared_data(server_init) == 0} {
    vwait shared_data(server_init)
  }
  foreach item $shared_data(init) {
    if {[lindex $item 0] eq "expr"} {
      log "debug" "tol::console eval,item=$item"
      tol::console eval [lindex $item 1]
    } elseif {[lindex $item 0] eq "file"} {
      log "debug" "tol::include,item=$item"
      tol::include [lindex $item 1]
    }
  }

  #set shared_data(ack) 0
  set shared_data(slave_status) "ready"
  log "debug" "leave start"
}

proc ::tolslave::slave_attach_data {} {
  global shared_data
  package require tequila

  tequila::open localhost 20458
  tequila::attach shared_data
  #trace add var shared_data(request) write tolslave::slave_listen
  trace add var shared_data write tolslave::slave_listen
}    

proc tolslave::slave_listen {name index op} {
  global shared_data

  log "debug" "enter slave_listen:index=$index"
  log "debug" "[array get ::shared_data]"
  #set shared_data(ack) 0
  if {$index eq "request"} {
    schedule_queue
  }
  log "debug" "leave slave_listen"
}

proc ::tolslave::schedule_queue { } {
  global shared_data

  log "debug" "enter, schedule_queue"
  set t_list [get_queue_shared]
  set_queue_tasks $t_list
  after idle [process_queue]
  #set shared_data(ack) 1
  log "debug" "leave, schedule_queue"
}

proc ::tolslave::get_queue_shared {} {
  global shared_data

  set t_list $shared_data(request)
  log "debug" "get_queue_shared:t_list=$t_list"
  return $t_list
}

proc ::tolslave::set_queue_tasks {t_list} {
  variable data

  log "debug" "set_queue_tasks:t_list=$t_list"
  #lappend data(queue,tasks) {*}$t_list
  set data(queue,tasks) $t_list
}

proc ::tolslave::process_queue { } {
  variable data
  global shared_data

  log "debug" "enter, process_queue"

  set shared_data(slave_status) "busy"
  
  foreach item $data(queue,tasks) {
    foreach {idtask idrq chan tol_expr} $item break
    set idx [lsearch $data(queue,cancel) $idrq]
    log "debug" "process_queue idx=$idx, idtask=$idtask"
    if {$idx != -1} {
      log debug "canceling task $idtask"
      # remove idrq from the cancel list
      set data(queue,cancel) [lreplace $data(queue,cancel) $idx $idx]
    } else {
      log debug "processing task $idtask"
      log "debug" "::tol::console eval tol_expr=$tol_expr"
      ::tol::console eval $tol_expr
      # inform client this request has being finished
      #after idle [namespace code "send_to_client $chan TFINISH $idtask"]
    }
  }
  set shared_data(slave_status) "ready"
  #tequila::do GetAll shared_data
  log "debug" "leave, process_queue"
}
