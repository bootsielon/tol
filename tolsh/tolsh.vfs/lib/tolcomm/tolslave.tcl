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

global shared_state
#set shared_data(ack) 0
set shared_state(slave) "busy"

global shared_tasks

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
  global shared_state
  global shared_tasks

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
  if {$shared_tasks(server_init) == 0} {
    vwait shared_tasks(server_init)
  }
  foreach item $shared_tasks(init) {
    if {[lindex $item 0] eq "expr"} {
      log "debug" "tol::console eval,item=$item"
      tol::console eval [lindex $item 1]
    } elseif {[lindex $item 0] eq "file"} {
      log "debug" "tol::include,item=$item"
      tol::include [lindex $item 1]
    }
  }

  #set shared_data(ack) 0
  set shared_state(slave) "ready"
  log "debug" "leave start"
}

proc ::tolslave::slave_attach_data {} {
  package require tequila
  global shared_state
  global shared_tasks

  tequila::open localhost 20458
  tequila::attach shared_state
  tequila::attach shared_tasks
  
  #trace add var shared_tasks(request) write tolslave::slave_listen_tasks
  trace add var shared_tasks write tolslave::slave_listen_tasks
  
  # this trace is just for debbuging, it's not necesary actually
  trace add var shared_state write tolslave::slave_listen_state
}    

proc tolslave::slave_listen_tasks {name index op} {
  global shared_tasks

  log "debug" "enter slave_listen_tasks:index=$index"
  log "debug" "[array get ::shared_tasks]"
  #set shared_data(ack) 0
  if {$index eq "request"} {
    schedule_queue
  }
  log "debug" "leave slave_listen_tasks"
}

proc tolslave::slave_listen_state {name index op} {
  global shared_state

  log "debug" "slave_listen_state:index=$index"
  log "debug" "[array get shared_state]"
}

proc ::tolslave::schedule_queue { } {
  #global shared_data

  log "debug" "enter, schedule_queue"
  set t_list [get_queue_shared]
  set_queue_tasks $t_list
  after idle [process_queue]
  #set shared_data(ack) 1
  log "debug" "leave, schedule_queue"
}

proc ::tolslave::get_queue_shared {} {
  global shared_tasks

  set t_list $shared_tasks(request)
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
  global shared_state

  log "debug" "enter, process_queue"

  set shared_state(slave) "busy"
  
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
  set shared_state(slave) "ready"
  #tequila::do GetAll shared_data
  log "debug" "leave, process_queue"
}
