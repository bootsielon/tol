#/////////////////////////////////////////////////////////////////////////////
# FILE    : odbc.tcl
# PURPOSE : This file contains a namespace called ODBCDialog with its
#           functions. It's for creating a ODBC connect options dialog and
#           managing its events.
#/////////////////////////////////////////////////////////////////////////////


#/////////////////////////////////////////////////////////////////////////////
# TESTING
#/////////////////////////////////////////////////////////////////////////////
# Put if {1} for testing this file.
if {0} {
  package require Tk
  package require BWidget
  package require BLT
  # force BWidget autoload
  Bitmap::use
  set toltk_script_path [file dirname [info script]]
  set toltk_images_path [file join ${toltk_script_path} images]
  if { [lsearch $Bitmap::path $toltk_images_path] == -1 } {
      lappend Bitmap::path [file join $toltk_script_path images]
  }
  package require msgcat
  namespace import msgcat::*
  ::msgcat::mclocale es
  ::msgcat::mcload [file join $toltk_script_path msgs]
  if { ![info exists toltk_library] } {
    set toltk_library [file join $toltk_script_path ..]
  }
  if { [string equal $tcl_platform(platform) "windows"] } {
    load [file join $::toltk_library tclodbc tclodbc.dll]
  } else {
    load [file join $::toltk_library tclodbc libtclodbc.so]
  }

}

package require bcombobox

#/////////////////////////////////////////////////////////////////////////////
  namespace eval ODBCDialog {
#
# PURPOSE: The purpose of this namespace is organize code and functions.
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpdata
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::ODBCDialog::Create {parent {alias ""} {user ""} {pass ""}
                             {connectProc ""} {disconnectProc ""}} {
#
# PURPOSE: Creates a instance of ODBCDialog. If it already exists calls to
#          ConnectOptions.
#
# PARAMETERS:
#   parent         -> widget's parent path
#   alias          -> ODBC alias (DSN)
#   user           -> ODBC user
#   pass           -> ODBC password
#   connectProc    -> user procedure to call when connect
#   disconnectProc -> user procedure to call when disconnect
#
#/////////////////////////////////////////////////////////////////////////////
  set Instance ::ODBCDialog::${parent}ODBC
  if { [lsearch [namespace children ::ODBCDialog] $Instance] != -1 } {
    #error "$Instance already exists as a children of ::ODBCDialog"
  } else {
    #  A namespace is created for each ODBCDialog widget.
    namespace eval $Instance {
      # data -> array with information about the instance:
      #   parent   -> widget's parent path
      #   alias    -> ODBC alias (DSN)
      #   user     -> ODBC user
      #   password -> ODBC password
      #   status   -> Connection status
      #   OnConnnec    -> user procedure to call when connect
      #   OnDisconnect -> user procedure to call when disconnect
      variable data
    }
    upvar #0 ${Instance}::data data
    set data(alias)        $alias
    set data(user)         $user
    set data(pass)         $pass
    set data(parent)       $parent
    set data(status)       0
    set data(OnConnect)    $connectProc
    set data(OnDisconnect) $disconnectProc
    bind $data(parent) <Destroy> [list ::ODBCDialog::Destroy $Instance]
  }
  set result [ConnectOptions $Instance]
  return $result
    
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::ODBCDialog::Destroy {this} {
#
# PURPOSE: Destroyes the instance given by this.
#
#
# PARAMETERS:
#   this -> Instance of ODBCDialog
#
#/////////////////////////////////////////////////////////////////////////////
  namespace delete $this
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::ODBCDialog::ConnectOptions {this} {
#
# PURPOSE: Shows the connect options dialog
#
# PARAMETERS:
#   this -> Instance of ODBCDialog
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpdata
  upvar #0 ${this}::data data
     
  set tmpdata(alias)    $data(alias)
  set tmpdata(user)     $data(user)
  set tmpdata(password) $data(pass)

  set dlg [Dialog $data(parent)._cnnopt -title [mc "Connect options"] \
          -parent $data(parent) -default 0 -cancel 1 -modal local]
  if $data(status) {
    $dlg add -text [mc "Disconnect"] -image [::Bitmap::get stop] \
             -compound right -helptext [mc "Disconnect"] \
             -relief link -command [list ::ODBCDialog::Disconnect $this]
  } else  {
    $dlg add -text [mc "Connect"] -image [::Bitmap::get play]\
             -compound right -helptext [mc "Connect"] \
             -relief link -command [list ::ODBCDialog::Connect $this]
  }

  $dlg add -text [mc "Cancel"] -image [::Bitmap::get cancel]\
           -compound right -helptext [mc "Cancel"]\
           -relief link -command "$dlg enddialog cancel"
    
  set w [$dlg getframe]
  wm resizable $dlg 0 0

  set tmpdata(dlg)      $dlg
  Label $w.lbalias -text "[mc Alias]:"
  ComboBox $w.cbalias -editable false -textvariable ::ODBCDialog::tmpdata(alias)
#  bcombobox $w.cbalias -editable false \
#            -textvariable ::ODBCDialog::tmpdata(alias) \
#            -bkeypress true
  set tmpdata(cbalias) $w.cbalias
  Label $w.lbuser -text "[mc User]:"
  Entry $w.euser  -textvariable ::ODBCDialog::tmpdata(user)
  Label $w.lbpass -text "[mc Password]:"
  Entry $w.epass  -show "*" -textvariable ::ODBCDialog::tmpdata(password) 
  set tmpdata(datasources) [lsort -index 0 [database datasources]] 
#  puts "DataSources : $tmpdata(datasources)"
  set all_alias ""
  foreach al $tmpdata(datasources) {
    lappend all_alias [lindex $al 0]
  }
#  puts "ALL_alias: $all_alias"
  SetControlsStatus $this $data(status)
  $w.cbalias configure -values $all_alias
  grid $w.lbalias      -row 0 -column 0 -sticky e -padx 4
  grid $w.cbalias      -row 0 -column 1 -sticky w -padx 2
  grid $w.lbuser       -row 1 -column 0 -sticky e -padx 4
  grid $w.euser        -row 1 -column 1 -sticky w -padx 2
  grid $w.lbpass       -row 2 -column 0 -sticky e -padx 4
  grid $w.epass        -row 2 -column 1 -sticky w -padx 2
  grid rowconfigure $w 1 -pad 6

  set result [$dlg draw]
  puts "draw == $result"
  if { [string equal $result "ok"] } {
    #::BayesTable::SetOption $this alias    $tmpdata(alias)
    #::BayesTable::SetOption $this user     $tmpdata(user)
    #::BayesTable::SetOption $this password $tmpdata(password)
    array unset tmpdata
  }
  array unset tmpdata
  destroy $dlg
  return $result
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::ODBCDialog::SetControlsStatus {this status} {
#
# PURPOSE: Sets controls state to normal/disabled according to connection 
#          status.
#
# PARAMETERS:
#   this   -> Instance of ODBCDialog
#   status -> connection status (1 connected, 0 disconnected)
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpdata
  set w [$tmpdata(dlg) getframe]
  if $status {
   set newStat disabled
  } else  {
   set newStat normal
  }
  $w.cbalias configure -state $newStat
  $w.euser   configure -state $newStat
  $w.epass   configure -state $newStat
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::ODBCDialog::Connect {this} {
#
# PURPOSE: Procedure that is called when Connect button is pushed. Checks if
#          connection can be established and calls user connection procedure.
#
# PARAMETERS:
#   this -> Instance of ODBCDialog
#
#/////////////////////////////////////////////////////////////////////////////
  upvar #0 ${this}::data data
  variable tmpdata

  rbc::busy hold $tmpdata(dlg)
  update
  if [catch "database dbtest $tmpdata(alias) $tmpdata(user) $tmpdata(password)" msg] {
    set data(status) 0
    MessageDlg ._msgerr -type ok -icon error -message $msg
  } else {
    dbtest disconnect
    set data(status) 1
    set data(alias) $tmpdata(alias)
    set data(user)  $tmpdata(user)
    set data(pass)  $tmpdata(password)
    set idx [$tmpdata(cbalias) getvalue]
    set datasource [lindex $tmpdata(datasources) $idx]
    # write .ini
    ::ODBCDialog::WriteIni $this $tmpdata(alias) $tmpdata(user)
    #User procedure
    set args [list $tmpdata(alias) $tmpdata(user) $tmpdata(password) $datasource]
    if { [string length $data(OnConnect)] } {
      eval $data(OnConnect) $args
    }
    #database connect db $tmpdata(alias) $tmpdata(user) $tmpdata(password)
    #::BayesTable::SetOption $this "db" db
    $tmpdata(dlg) enddialog ok
  }
  rbc::busy release $tmpdata(dlg)
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::ODBCDialog::Disconnect {this} {
#
# PURPOSE: Procedure that is called when Disconnect button is pushed.
#          Configures ODBC dialog and calls user disconnect procedure.
#
# PARAMETERS: 
#   this -> Instance of ODBCDialog
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpdata
  upvar #0 ${this}::data data  
  set data(status) 0
  SetControlsStatus $this $data(status)
  set idx [$tmpdata(cbalias) getvalue]
  set datasource [lindex $tmpdata(datasources) $idx]
  # write .ini
  ::ODBCDialog::WriteIni $this
  #User procedure
  set args [list $tmpdata(alias) $tmpdata(user) $tmpdata(password) datasource]
  if { [string length $data(OnDisconnect)] } {
    eval $data(OnDisconnect) $args
  }
  $tmpdata(dlg) itemconfigure 0 -text [mc "Connect"] -image [::Bitmap::get play]\
             -compound right -helptext [mc "Connect"] \
             -relief link -command [list ::ODBCDialog::Connect $this]
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::ODBCDialog::ReadIni {} {
#
# PURPOSE: Read configuration file
#
# RETURN: Values configuration connection ODBC. Alias User
#
#/////////////////////////////////////////////////////////////////////////////
  # cargamos variables de conexion a la BD
  set alias  [::iniFile::Read TolBaseODBC DataSource  ""]
  set user   [::iniFile::Read TolBaseODBC UserId      ""]
  return [list $alias $user]
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::ODBCDialog::WriteIni {this {alias ""} {user ""}} {
#
# PURPOSE: Write configuration file
#
# PARAMETERS: 
#   this -> Instance of ODBCDialog
#   alias -> data sourece
#   user  -> user connection
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpdata
  ::iniFile::Write TolBaseODBC DataSource $alias
  ::iniFile::Write TolBaseODBC UserId     $user
}

#/////////////////////////////////////////////////////////////////////////////
# TESTING
#/////////////////////////////////////////////////////////////////////////////
#
#proc UserConnect {alias user pass datasource} {
#  global galias guser gpass
#  database connect db $alias $user $pass
#  puts "galias=$galias, guser=$guser, gpass=$gpass"
#  puts "alias=$alias, user=$user, pass=$pass, datasource=$datasource"
#  puts "db= db" 
#}
#
#     set galias ICIPP
#     set guser icipp
#     set gpass icipp
#     toplevel .t
#     set this [::ODBCDialog::Create .t  $galias $guser $gpass ::UserConnect]
#     ::ODBCDialog::ConnectOptions $this
#/////////////////////////////////////////////////////////////////////////////
