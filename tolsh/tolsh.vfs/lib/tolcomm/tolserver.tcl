###############################################################################
#
#  tolserver.tcl -- communication layer of tolsh when acting as a server.
#
#  Originally taken from:
#
#    - Network server application template: http://wiki.tcl.tk/8301
#    - fconfigure : http://wiki.tcl.tk/1469 
#
###############################################################################

package provide tolserver 1.0

package require tlogger

###############################################################################
#
#  tolserver - namespace to encapsulate the server state and behaviour
#
###############################################################################
namespace eval ::tolserver {

  variable data
  
  array set data {
    socket       ""
    queue,id     0
    queue,tasks  ""
    queue,cancel ""
  }
}

global shared_tasks
set shared_tasks(request) ""
set shared_tasks(init) ""
set shared_tasks(server_init) 0

global shared_state
global shared_finish

###############################################################################
#
#  is_implemented -- check is a given proc is implemented in the
#  server namespace.
#
#  Arguments:
#
#    command - a possible command name
#
#  Return:
#
#    a true value if command is implemented in for the server
#    namespace a false value in other case.
#
###############################################################################
proc ::tolserver::is_implemented { command } {
  llength [info command [namespace current]::$command]
}

###############################################################################
#
#  get_client_id -- return the client host identification host:port
# 
#  Arguments:
#
#    chan - the client socket channel for which host identification is requested
#
#  Return:
#
#    A string "host:port" identifying the client host for which 'chan'
#    is connected.
#
###############################################################################
proc ::tolserver::get_client_id { chan } {

  upvar \#0 ::tolserver::${chan}_data channel_data
  
  return "$channel_data(host):$channel_data(port)"
}

###############################################################################
#
#  log -- write a log message into the server log file
#
#  This proc write a message into the server log channel which is
#  handled by tlogger
#
#  Arguments:
#
#    msg - the message to be written
#
###############################################################################
proc ::tolserver::log { level msg } {
  
  ::tlogger::log "tolserver" $level $msg
}

###############################################################################
# 
#  register_client -- Configure a client channel
#
#    This proc is the server listener (see server socket creation),
#    and is invoked whenever a client open a socket connection to this
#    server.
#
#  Arguments:
#
#    chan - channel assigned to the client just conneted
#    host - host address from where the channel was connected
#    port - port from where the channel was connected in the host
#  
###############################################################################
proc ::tolserver::register_client {chan host port} {

  # We want to read whole lines
  #
  fconfigure $chan -blocking 0 -buffering line

  log debug [fconfigure $chan]

  # Invoke a proc to notify us that a client has registered.
  #
  on_client_connected $chan $host $port
  
  # Set up a fileevent to handle input
  #
  fileevent $chan readable \
      [namespace code "read_from_client $chan"]
}

###############################################################################
# 
#  read_from_client -- Read a complete data buffer from client
#
#  This proc is an event handler triggered whenever some data is
#  available from the client channel.
#
#  Arguments:
#
#    chan - client channel
#
###############################################################################
proc ::tolserver::read_from_client { chan } {

  if {![eof $chan]} {
    # Something could be read
    #
    if {[gets $chan buffer] == -1} {
      # Only handle complete lines
      #
      return		
    }
  } else {
    # Close up the channel and notify us that a client was disconnected.
    #
    on_eof_channel $chan
    return
  }

  # process the buffer of data
  #
  on_client_request $chan $buffer
}

###############################################################################
# 
#  on_client_request -- Process a buffer of data read from client.
#
#  Given a request 'buffer' just read from a client channel 'chan' the
#  corresponding command is invoked. A request buffer is a word command
#  followed by its arguments. The word commands accepted are:
#
#    - PING       : check if I'm alive
#    - TOLEVAL    : evaluate a tol expression.
#    - RECONN     : reconnection request
#    - TCANCEL    : the client wants to cancel a task due to timeout
#    - SHUTDOWN   : shutdown this process server.
#
#  Arguments --
#
###############################################################################
proc ::tolserver::on_client_request {chan buffer} {

  # Log this request
  #
  log "notice" "on_client_request ([get_client_id $chan]) : $buffer"
  set idrq "NOID"
  if {![regexp {([^\s]+)\s(\d+)\s([^\s]+)(.*)$} $buffer ==> cmd size idrq str]} {
    set reason "invalid request format in '$buffer'"
    log critical $reason
    on_bad_request $chan $idrq $reason
    return
  }
  set str [string trim $str]
  set rsize [string length $str]
  if {$rsize!=$size} {
    set reason "invalid argument size '$str'. Should be $size and it was $rsize"
    log critical $reason
    on_bad_request $chan $idrq $reason
    return
  }
  switch -- [string toupper $cmd] {
    PING {
      on_ping $chan $idrq
    } 
    TOLEVAL {
        on_toleval $chan $idrq $str
    }
    RECONN {
        on_reconnect $chan $idrq $str
    }
    SHUTDOWN {
      on_shutdown $chan
    }
    TCANCEL {
      on_cancel $chan $str
    }
    default {
      on_bad_request $chan $idrq "unknown request '$cmd' in '$buffer'"
    }
  }
}

proc ::tolserver::on_bad_request { chan idrq reason } {
  log notice "bad request '$idrq' '$reason'"
  send_to_client $chan "CRITICAL" $reason
}

###############################################################################
# 
#  send_to_client -- send data to the client channel
#
#  Arguments:
#
#    chan - client socket channel who should be waiting for a server answer.
#    TAG   - identification of the data
#    data - data to be send to client.
#
###############################################################################
proc ::tolserver::send_to_client { chan TAG {data ""} } {

  log "debug" "enter : send_to_client $chan $TAG '$data'"

  if {[catch {
    puts $chan [string trimright "$TAG [string length $data] $data"]
    flush $chan
  } msg]} {
    log emergency $msg
  }

  log "debug" "leave : send_to_client" 
}

###############################################################################
# 
#  on_ping -- check is the server is responding
#
#  Arguments:
#
#    chan - client socket channel who should be waiting for a server answer.
#
###############################################################################
proc ::tolserver::on_ping { chan idrq } {

  log "debug" "enter : on_ping $chan $idrq"

  send_to_client $chan ACKPING $idrq 

  log "debug" "leave : on_ping $chan $idrq"
}

###############################################################################
#
#  on_toleval -- Invoked in response to a TOLEVAL request
#
#  This proc evaluate a tol expression requested from client connected
#  at the oder end of channel $chan. The tol expression is contained
#  in argument cmd. First the expression is inserted into a queue and
#  a requestd ID is returned to the client. When idle the server will
#  process the tol expressions waiting for being evaluated. After and
#  expression evaluation is done a TFINISH is sent to the
#  corresponding client.
#
#  Arguments:
#
#    chan -- client channel from where the RUN request was sent.
#    idreq -- request ID, must be send back when evaluation is finished
#    texpr  -- the tol expression to be executed.
#
###############################################################################
proc ::tolserver::on_toleval { chan idrq cmd }  {

  log "debug" "enter, on_toleval $chan $idrq $cmd"

  log "notice" "TOLEVAL $cmd"
  set idtask [insert_expr $chan $idrq $cmd]
  send_to_client $chan "TSTART" "$idrq $idtask"
  after idle ::tolserver::dispatch_to_slave
  
  log "debug" "leave, on_toleval"
}

proc ::tolserver::on_reconnect { chan idrq idtask } {
  # NO ESTA IMPLEMENTADO
}

###############################################################################
# 
#  on_shutdown -- Handler for shutdown request
#
#  This proc perfom will shutdown this tol server. Before dying it
#  will send an abort signal to all clients waiting for a request to
#  finish.
#
#  Arguments:
#
#    chan - client socket channel from where the KILL request was sent.
#    PID  - process ID to kill.
#
#  Return:
#
###############################################################################
proc ::tolserver::on_shutdown { chan }  {

  upvar \#0 ::tolserver::${chan}_data channel_data

  log "debug" "enter, on_shutdown $chan"

  log information "shutdown from $chan [get_client_id $chan]"
  broadcast $chan "ACKSD"

  after idle shutdown

  log "debug" "leave, on_shutdown $chan"
}

###########################################################################
#
#
###########################################################################
proc ::tolserver::on_cancel { chan idrq } {

  variable data

  log notice "TCANCEL [get_client_id $chan] $idrq"

  lappend data(queue,cancel) $idrq
}

###############################################################################
# 
#  on_eof_channel -- close a client channel normally after an eof received.
#
#  This proc perform the following action:
#
#    - File event associated to the channel is cleared.
#    - The client channel is closed.
#    - The event is logged.
#    - The channel specific data are cleared.
#
###############################################################################
proc ::tolserver::on_eof_channel { chan } {
  upvar \#0 ::tolserver::${chan}_data channel_data

  log "debug" "enter, on_eof_channel $chan"
  # Clear the file event.
  #
  fileevent $chan readable {}

  # Close the client channel.
  #
  catch {close $chan}

  # Log the situation.
  #
  log "notice" "$chan attached to [get_client_id $chan] has being closed"

  # Clear the channel specific data.
  #
  array unset channel_data
}

###############################################################################
#
#  on_client_connected -- Invoked whenever the a client a connected.
#
#  Arguments:
#
#    chan - client channel just connected
#    host - host address from where the channel was connected
#    port - port from where the channel was connected in the host
#
###############################################################################
proc ::tolserver::on_client_connected { chan host port } {
  # Connection has been accepted. Log connection from client
  #
  log "debug" "enter, on_client_connected $chan $host $port"

  log "notice" "Connection accepted from client ${host}:${port} through channel ${chan}"

  # Create a variable for storing client channel specific data.
  # 
  upvar \#0 ::tolserver::${chan}_data channel_data

  set channel_data(host) $host
  set channel_data(port) $port

  log "debug" "leave, on_client_connected"
}

###############################################################################
#
#  open_socket -- Open the server socket
#
#  Arguments:
#
#    port - the port where this server will attend request from clients.
#
###############################################################################
proc ::tolserver::open_socket { port } {
  variable data

  set data(socket) [socket -server [namespace code register_client] $port]
}

###############################################################################
#
#  close_socket -- Closes the server socket
#
###############################################################################
proc ::tolserver::close_socket { } {

  variable data

  log "notice" "closing server socket ...."
  catch {close $data(socket)}
  set data(socket) ""
}

###############################################################################
#
#  start -- Start the server
#
#  Arguments:
#
#    port - the port where this server will attend request from clients.
#
###############################################################################
proc ::tolserver::start { args } {
  global shared_tasks

  array set options {
    -port 6669
    -shport 20458
    -logfile ""
    -compile ""
  }
  array set options $args

  ::tlogger::init tolserver $options(-logfile)
  ::tlogger::level tolserver "debug"

  if {[catch {open_socket $options(-port)} msgerr]} {
    log emergency "::tolserver::start '$msgerr'"
    exit
  } else {
    log "notice" "started"
  }
  
  server_attach_data $options(-shport)

  log "debug" "shared_tasks(init)=$options(-compile)"
  set shared_tasks(init) $options(-compile)  
  set shared_tasks(server_init) 1
}

proc ::tolserver::server_attach_data { shared_port } {
  package require tequila
  global shared_tasks
  global shared_state
  global shared_finish

  tequila::open localhost $shared_port
  tequila::attach shared_tasks
  tequila::attach shared_state
  tequila::attach shared_finish
  
  #trace add var shared_finish(tasks) write tolserver::server_listen_finish
  trace add var shared_finish write tolserver::server_listen_finish

  # this trace is just for debbuging, it's not necesary actually
  trace add var shared_tasks write tolserver::server_listen_tasks
  
  # this trace is just for debbuging, it's not necesary actually
  trace add var shared_state write tolserver::server_listen_state
}    

proc tolserver::server_listen_finish {name index op} {
  global shared_finish

  log "debug" "enter server_listen_finish:index=$index"
  log "debug" "[array get ::shared_finish]"
  
  # inform client this request has being finished
  foreach {chan idtask} $shared_finish(tasks) break
  after idle [namespace code "send_to_client $chan TFINISH $idtask"]

  log "debug" "leave server_listen_finish"
}

proc tolserver::server_listen_tasks {name index op} {
  global shared_tasks

  log "debug" "server_listen_tasks:index=$index"
  log "debug" "[array get shared_tasks]"
}

proc tolserver::server_listen_state {name index op} {
  global shared_state

  log "debug" "server_listen_state:index=$index"
  log "debug" "[array get shared_state]"
}

###############################################################################
#
#  shutdown -- Perform shutdown task for the server.
#
###############################################################################
proc ::tolserver::shutdown { } {
  log "notice" "shutting down immediately"
  close_socket
  exit
}

###############################################################################
#
#  bgerror -- handle unxepected error detected in background
#
###############################################################################
proc _bgerror {msg} {
  # Handle unexpected errors here.
  ::tolserver::log "emergency" "from _bgerror --> $msg"
}

proc ::tolserver::broadcast {chan what} {
  set client [get_client_id $chan]
  log "notice" "broadcasting $what requested from $client"
  log "warn" "TO BE IMPLEMENTED"
  # TO BE IMPLEMENTED
}

###########################################################################
#
#
###########################################################################
proc ::tolserver::insert_expr { chan idrq tol_expr } {
  upvar \#0 ::tolserver::${chan}_data channel_data
  variable data
  
  log "debug" "enter, insert_expr $chan $idrq $tol_expr"

  incr data(queue,id)
  set client [get_client_id $chan]
  set idtask "${client}:$data(queue,id)"
  push_queue_tasks [list $idtask $idrq $chan $tol_expr]

  log "debug" "leave, insert_expr, $idtask"
  set idtask
}

proc ::tolserver::push_queue_tasks {item} {
  variable data

  log "debug" "push_queue_tasks:item=$item"
  lappend data(queue,tasks) $item
}

proc ::tolserver::dispatch_to_slave {} {
  global shared_state

  log "debug" "enter dispatch_to_slave"
  if {$shared_state(slave) ne "ready"} {
    log "debug" "dispatch_to_slave: vwait shared_state(slave)"
    vwait shared_state(slave)
  }
  send_tasks_to_slave
  log "debug" "leave dispatch_to_slave"
}

proc ::tolserver::send_tasks_to_slave {} {

  log "debug" "enter send_tasks_to_slave"
  set t_list [get_queue_tasks]
  if {[llength $t_list] != 0} {
    set_queue_shared $t_list
  }
  log "debug" "leave send_tasks_to_slave"
}

proc ::tolserver::get_queue_tasks {} {
  variable data

  set t_list $data(queue,tasks)
  set data(queue,tasks) [list]
  log "debug" "get_queue_tasks:t_list=$t_list"
  return $t_list
}

proc ::tolserver::set_queue_shared {t_list} {
  global shared_tasks
  
  log "debug" "set_queue_shared:t_list=$t_list"
  set shared_tasks(request) $t_list
}
