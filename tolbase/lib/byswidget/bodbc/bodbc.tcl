#/////////////////////////////////////////////////////////////////////////////
# FILE    : bodbc.tcl
# PURPOSE : This file contains a namespace called BODBC with its
#           functions. It's for creating a ODBC connect options dialog and
#           managing its events.
#/////////////////////////////////////////////////////////////////////////////

# INCLUDES
package require byscommon
package require bcombobox
package require bfunctions


#/////////////////////////////////////////////////////////////////////////////
snit::type bodbcmodule {
#
# PURPOSE: BODBCModule widget class definition
#
#/////////////////////////////////////////////////////////////////////////////
#/////////////////////////////////////////////////////////////////////////////
#
# options:
#   module        -> Name of modules to connect
#   stdtolcon     -> Connection TOL : true
#   stdtclcon     -> Connection TCL : true
#   procconnect   -> Function connect : {}
#   prodisconnect -> Function disconnect : {}
#   iniconfig     -> Header in .ini file : $modele
#   savealias     -> Save "alias" in .ini : true
#   saveuser      -> Save "user" in .ini : true
#   savepwd       -> Save "pwd" in .ini : false
#   alias         -> Alias by defect
#   user          -> User by defect
#   pwd           -> Password by defect 
#   connection    -> name of connection TCL
#   autoconnect   -> connects automatically with Data Base
#   
#/////////////////////////////////////////////////////////////////////////////
  delegate option * to hull

  option -stdtolcon     true
  option -stdtclcon     true
  option -procconnect   {}
  option -prodisconnect {}
  option -iniconfig     {}
  option -savealias     true
  option -saveuser      true
  option -savepwd       false
  option -alias         {}
  option -user          {}
  option -pwd           {}
  option -gestor        {}
  option -name          {}
  option -connection    {}
  option -autoconnect   false

  #///////////////////////////////////////////////////////////////////////////
  constructor { args } {
  #
  # PURPOSE: beditselector constructor
  #
  # PARAMETERS:
  #   args -> options
  #
  #///////////////////////////////////////////////////////////////////////////
    # Apply all options passed at creation time.
    $self configurelist $args
    # load parameters .ini
    $self _LoadParamIni
  }

  #///////////////////////////////////////////////////////////////////////////
  method _LoadParamIni {} {
  #
  # PURPOSE: To recover values of the configuration file
  #
  #///////////////////////////////////////////////////////////////////////////
    # load parameters from ini
    if {[string length [$self cget -iniconfig]]} {
Tolcon_Trace "_LoadParamIni ..."
      set key [$self cget -iniconfig]
      if {[$self cget -savealias]} {
        $self configure -alias  \
          [::iniFile::Read $key DataSource [$self cget -alias] ]
      }
      if {[$self cget -saveuser]} {
        $self configure -user \
          [::iniFile::Read $key Userid     [$self cget -user]]
      }
      if {[$self cget -savepwd]} {
        $self configure -pwd \
          [::iniFile::Read $key Password   [$self cget -pwd]]
      }     
    }
  }


  #///////////////////////////////////////////////////////////////////////////
  method get { var} {
  #
  # PURPOSE: Obtain the value of a variable
  #
  # PARAMETERS: var -> name of variable
  #
  # RETURN: value of variable
  #
  #///////////////////////////////////////////////////////////////////////////
    return [subst $[subst "$var"]]
  }
    
  #///////////////////////////////////////////////////////////////////////////
  method set { var value } {
  #
  # PURPOSE: Establish the value of a variable
  #
  # PARAMETERS: var -> name of variable
  #             value -> values of variable
  #
  #///////////////////////////////////////////////////////////////////////////
    set [subst $var] $value
  }
}


#/////////////////////////////////////////////////////////////////////////////
snit::type bodbc {
#
# PURPOSE: BODBC type class definition
#
#/////////////////////////////////////////////////////////////////////////////
#/////////////////////////////////////////////////////////////////////////////
# variables:
#   lstConnection -> list of list. Name module  and  variable connection
#   widget        -> array of name of widget
#   counter       -> variable accountant 
#
#/////////////////////////////////////////////////////////////////////////////
  delegate option * to hull

  variable lstConnection     {}
  variable widget
  variable counter 0
  
  #///////////////////////////////////////////////////////////////////////////
  method showdialog { { namMod {} } } {
  #
  # PURPUSE: Show the dialog
  #
  # PARAMETERS: 
  #   namMod -> if it is specified I name of module, it asks for its information.
  #             ESLE
  #             it shows box of selection of module
  #
  #///////////////////////////////////////////////////////////////////////////
    set module {}
    # one connection ?
    if {[string length $namMod]} {
      set module [$self _GetModuleToName $namMod]
    } else {
      switch -exact [llength $lstConnection] {
        0 { return 0 }
        1 { set module [lindex [lindex $lstConnection 0] 1] }
      }
    }
    # create dialog
    if {[string length $module]} {
      $self _CreateDialog $module
    } else  {
      $self _CreateDialog
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method validateconnection { namMod } {
  #
  # PURPUSE: it verifies if we are connected to the data base, else, show
  #          selection dialog   
  # 
  # PARAMETERS:
  #   namMod -> if it is specified I name of module, it asks for its information.
  # 
  # RETURN: 1 -> connection OK
  #         0 -> error
  #        
  #///////////////////////////////////////////////////////////////////////////
    # get module
    set module [$self _GetModuleToName $namMod]
    if {![string length module]} {
#Tolcon_Trace "ValidateConnection. El modulo $namMod no existe"
      return 0
    }  
    
    # connection TCL && TCL
    set status [$self checkconnection $module]
    # activate connection TOL
    if {($status) && ([$module cget -stdtolcon])} {
      set op [GetUniqueName Real _ActTOLTK]
      set alias  [$module cget -alias]
      set cmd "Real $op = DBActivate(\"$alias\");"
      ::tol::console eval $cmd
      ::tol::console stack release $op
    }
    # Auto connect 
    if {[expr !$status && [$module cget -autoconnect]]} {
      set alias  [$module cget -alias]
      set user   [$module cget -user]
      set pwd    [$module cget -pwd]
      set status [$self _Connect $module $alias $user $pwd]
    }
        
    # Dialogo
    if {!$status} {
      set status [$self _CreateDialog $module]
    } 
    # return
#Tolcon_Trace "validateconnection $status"
    return $status
  }


  #///////////////////////////////////////////////////////////////////////////
  method checkconnection {module} {
  #
  # PURPUSE: Verifies if its are connected to the data base in TCL and TOL
  # 
  # PARAMETERS:
  #   module: name of connection (bodbcmodule)
  # 
  # RETURN: 1 -> connection
  #         0 -> no
  #        
  #///////////////////////////////////////////////////////////////////////////
    # connection TCL 
    set statusTcl [$self checkconnectiontcl $module]
    # connection TOL
    set statusTol [$self checkconnectiontol $module]
#Tolcon_Trace "CheckConnection [expr $statusTcl && $statusTol]"
    return [expr $statusTcl && $statusTol]
  }

  #///////////////////////////////////////////////////////////////////////////
  method checkconnectiontcl {module} {
  #
  # PURPUSE: Verifies if is are connected to the data base in TCL
  # 
  # PARAMETERS:
  #   module: name of connection (bodbcmodule)
  # 
  # RETURN: 1 -> connection
  #         0 -> no
  #        
  #///////////////////////////////////////////////////////////////////////////
#Tolcon_Trace "checkconnectiontcl $module"
    set result 1

    # connection TCL ?
    if {[$module cget -stdtclcon]} {
      if {[string length [$module cget -connection]]} {
        if {[catch "[$module cget -connection] statement get tables" msg]} {
          # no connection
          #Tolcon_Trace "No conectado a TCL"
          set result 0
        }
      } else {
        #Tolcon_Trace "No conectado a TCL"
        set result 0
      }
    }
#Tolcon_Trace "checkconnectiontcl $result"
    return $result
  }

  #///////////////////////////////////////////////////////////////////////////
  method checkconnectiontol {module} {
  #
  # PURPUSE: Verifies if is are connected to the data base in TOL
  # 
  # PARAMETERS:
  #   module: name of connection (bodbcmodule)
  # 
  # RETURN: 1 -> connection
  #         0 -> no
  #        
  #///////////////////////////////////////////////////////////////////////////
#Tolcon_Trace "checkconnectiontol ... module: $module"
    set status 1
    # connection TOL
    if {[$module cget -stdtolcon]} {
      # parameters
      set alias [$module cget -alias]
      # connect tol ?
      set status [GetAliasOpen $alias]
    }
#Tolcon_Trace "CheckConnectionTol.end = $status"
    return $status
  }

  #///////////////////////////////////////////////////////////////////////////
  method _ConnectDlg {} {
  #
  # PURPUSE: Makes connection a TOL and TCL with dialog
  #
  #///////////////////////////////////////////////////////////////////////////
#Tolcon_Trace "ConnectDlg  ..."
#    variable widget
    # wait
    blt::busy hold $widget(dlg)
    update
    # _Connect
    set mamMod [BfoGetText $widget(module)]
    # get module
    set module {}
    set lst [ltraspose $lstConnection]
    set inx [lsearch [lindex $lst 0] $mamMod]
    if {[expr $inx != -1]} {
      set module [lindex [lindex $lst 1] $inx]
    }
#Tolcon_Trace "Nombre del module: mamMod ; Modulo a cargar: $module"
    set alias [BfoGetText $widget(alias)]
    set user  [BfoGetText $widget(user)]
    set pwd   [BfoGetText $widget(pwd)]
    set status [$self _Connect $module $alias $user $pwd]
    # OK -> close dialog ?
    if {$status} {
      $widget(dlg) enddialog ok
    }
    # end wait
    blt::busy release $widget(dlg)
  }

  #///////////////////////////////////////////////////////////////////////////
  method _Connect {module alias user pwd} {
  #
  # PURPUSE: Makes connection a TOL and TCL
  #
  # PARAMETERS:
  #   module: name of connection (bodbcmodule)
  #   alias: name od bridge
  #   user: name of user
  #   pwd: password
  #
  #///////////////////////////////////////////////////////////////////////////
    # parameters

Tolcon_Trace "_Connect: module: $module  alias: $alias  user: $user  pwd: $pwd"
    # connection TCL
    set statusTcl [$self _ConnectTcl $module $alias $user $pwd]
    # connection TOL
    set statusTol [$self _ConnectTol $module $alias $user $pwd]
#Tolcon_Trace "Connect: [expr $statusTcl && $statusTol]"
    # .ini ?
    if {[string length [$module cget -iniconfig]]} {
#Tolcon_Trace ".ini"
      if {[$module cget -savealias]} {
        ::iniFile::Write [$module cget -iniconfig] DataSource $alias
      }
      if {[$module cget -saveuser]} {
        ::iniFile::Write [$module cget -iniconfig] Userid     $user
      }
      if {[$module cget -savepwd]} {
        ::iniFile::Write [$module cget -iniconfig] Password   $pwd
      }
    }
    # update parameters
    $module configure -alias $alias
    $module configure -user  $user
    $module configure -pwd   $pwd
  
    set gestor [SqlGetGestor [SqlGetDriver $alias]]
    $module configure -gestor $gestor
 
    # execute proc ?
    if {[string length [$module cget -procconnect]]} {
#Tolcon_Trace "execute proc"
      eval [$module cget -procconnect]
    }

    # OK ?
    return [expr $statusTcl && $statusTol]
  }

  #///////////////////////////////////////////////////////////////////////////
  method _ConnectTcl {module alias user pwd} {
  #
  # PURPUSE: Makes connection a TCL
  #
  # PARAMETERS:
  #   module: name of connection (bodbcmodule)
  #   alias: name od bridge
  #   user: name of user
  #   pwd: password
  #
  # RETURN:
  # RETURN: 1 -> connection
  #         0 -> no    
  #
  #///////////////////////////////////////////////////////////////////////////
#Tolcon_Trace "_ConnectTcl: module: $module  alias: $alias  user: $user  pwd: $pwd Connect: [$module cget -stdtclcon]"
    set status 1
    # connection TCL
    if {[$module cget -stdtclcon]} {
      if {[$self checkconnectiontcl $module]} {
        set status [$self _DisconnectTcl $module]
      }
      if {$status} {
        if {[string length [$module cget -connection]]} {
          set con [$module cget -connection]
        } else  {
          set con [$self _UniqueName [$module cget -name]]
        }
        if [catch "database $con $alias $user $pwd" msg] {
          set status 0
          $module configure -connection {}
          MessageDlg ._msgerr -type ok -icon error -message $msg
        } else  {
          $module configure -connection $con
        }
      }
#Tolcon_Trace "ConnectTcl = $status"
    }
    # return
    return $status
  }

  #///////////////////////////////////////////////////////////////////////////
  method _ConnectTol {module alias user pass} {
  #
  # PURPUSE: Makes connection a TOL
  #
  # PARAMETERS:
  #   module: name of connection (bodbcmodule)
  #   alias: name od bridge
  #   user: name of user
  #   pwd: password
  #
  # RETURN:
  # RETURN: 1 -> connection
  #         0 -> no  
  #
  #///////////////////////////////////////////////////////////////////////////
#Tolcon_Trace "_ConnectTol: module: $module  alias: $alias  user: $user  pwd: $pwd Connect: [$module cget -stdtolcon]" 
    if {[$module cget -stdtolcon]} {
      if {[$self checkconnectiontol $module]} {
        # intento activarla
        #
        if {[$self _ActivateAlias $alias]} {
          # el alias ha sido activado correctamente
          #
          return 1
        } else {
          # Algo ha ido mal, me dijeron que el alias estaba abierto
          # pero no pude activarlo.
          #
          $self _CloseAlias $alias
        }
      }
      return [$self _OpenAlias $alias $user $pass]
    }
    return 1
  }

  method _OpenAlias { alias user pass } {
    tol::console eval [string map "%A $alias %U $user %P $pass" {
      Real __dbopen_status__ = DBOpen("%A","%U","%P");
    }]
    set status [lindex [tol::info variable Real __dbopen_status__] 0]
    tol::console stack release __dbopen_status__
    expr {$status>0.0}
  }

  method _ActivateAlias { alias } {
    tol::console eval [string map "%A $alias" {
      Real __dbact_status__ = DBActivate("%A");
    }]
    set status [lindex [tol::info variable Real __dbact_status__] 0]
    tol::console stack release __dbact_status__
    expr {$status>0.0}
  }

  method _CloseAlias { alias } {
    tol::console eval [string map "%A $alias" {
      Real __dbclose_status__ = DBClose("%A");
    }]
    set status [lindex [tol::info variable Real __dbclose_status__] 0]
    tol::console stack release __dbclose_status__
    expr {$status>0.0}
  }

  #///////////////////////////////////////////////////////////////////////////
  method _Disconnect { } {
  #
  # PURPUSE: Makes disconnect of TOL and TCL
  #
  # RETURN: 1 -> disconnect
  #         0 -> error   
  #
  #///////////////////////////////////////////////////////////////////////////
#Tolcon_Trace "Disconnect ..."
    # get module
    set lst [ltraspose $lstConnection]
    set inx [lsearch [lindex $lst 0] [BfoGetText $widget(module)]]
    if {[expr $inx != -1]} {
        set module [lindex [lindex $lst 1] $inx]
    }
    # parameters
    set alias [BfoGetText $widget(alias)]
    # disconnect TOL
    set statusTol [$self _DisconnectTol $module $alias]

    # disconnect TCL
    set statusTcl [$self _DisconnectTcl $module]

    # control status 
    SetControlsStatus $self $widget(dlg) [expr !($statusTol && $statusTcl)]

    # execute proc ?
    if {[string length [$module cget -prodisconnect]]} {
#Tolcon_Trace "Disconnect ... proc"
      eval [$module cget -prodisconnect]
    }
    # return
#Tolcon_Trace "Disconnect = [expr $statusTol && $statusTcl]"
    return [expr $statusTol && $statusTcl]
  }

  #///////////////////////////////////////////////////////////////////////////
  method _DisconnectTol {module alias} {
  #
  # PURPUSE: Makes disconnect of TOL
  #
  # PARAMETERS:
  #   module: name of connection (bodbcmodule)
  #   alias: name od bridge
  #
  # RETURN:
  # RETURN: 1 -> disconnect
  #         0 -> no  
  #
  #///////////////////////////////////////////////////////////////////////////
#Tolcon_Trace "_DisconnectTol. module:  $module alias: $alias Disconnect: [$module cget -stdtolcon]"
    set status 1
    # disconnect TOL
    if {[$module cget -stdtolcon]} {
      # connecition TOL ?
      set connect [GetAliasOpen $alias]
      if {$connect} {
        set op [GetUniqueName Real]
        set cmd "Real $op =  DBClose(\"$alias\");"
        ::tol::console eval $cmd
        array set aryInfo [TclInfoVar Real $op]
        #> 1 -> OK, 0 -> ERROR
        set status [lindex $aryInfo(CONTENT) 0]
        unset aryInfo
        ::tol::console stack release $op
      }
#Tolcon_Trace "Disconnect: $status"
    }
    # return 
    return $status
  }

  #///////////////////////////////////////////////////////////////////////////
  method _DisconnectTcl {module} {
  #
  # PURPUSE: Makes disconnect of TCL
  #
  # PARAMETERS:
  #   module: name of connection (bodbcmodule)
  #   alias: name od bridge
  #
  # RETURN:
  # RETURN: 1 -> disconnect
  #         0 -> no  
  #
  #///////////////////////////////////////////////////////////////////////////
#Tolcon_Trace "DisconnectTcl .... module: $module  Disconnect: [$module cget -stdtclcon]"
    set status 1
    # disconnect TCL
    if {[$module cget -stdtclcon]} {
      set con [$module cget -connection]
      if {[string length $con]} {
        $con disconnect
      }
#Tolcon_Trace "DisconnectTcl = $status"
    }
    # return 
    return $status
  }

  #///////////////////////////////////////////////////////////////////////////
  method _CreateDialog { {module {}} } {
  #
  # PURPOSE: Shows the connect options dialog
  #
  # PARAMETERS:
  #   module: name of connection (bodbcmodule)
  #
  #///////////////////////////////////////////////////////////////////////////
Tolcon_Trace "CreateDialog: module: $module"   
    set status 0
    # exists module ? 
    set alias    {}
    set user     {}
    set password {}
    if {[string length $module]} {
      # parameters
      set alias    [$module cget -alias]
      set user     [$module cget -user]
      set password [$module cget -pwd]
      # connection
      set status [$self checkconnection $module]
    }

    set widget(dlg) [set dlg \
      [Dialog ._cnnopt -title [mc "Connect options"] \
               -parent . -default 0 -cancel 1 -modal local]]
    if {$status} {
      $dlg add -text [mc "Disconnect"] -image [::Bitmap::get stop] \
               -compound right -helptext [mc "Disconnect"] \
               -relief link -command [list $self _Disconnect]
    } else  {
      $dlg add -text [mc "Connect"] -image [::Bitmap::get play]\
               -compound right -helptext [mc "Connect"] \
               -relief link -command [list $self _ConnectDlg]
    }
    
    $dlg add -text [mc "Cancel"] -image [::Bitmap::get cancel]\
             -compound right -helptext [mc "Cancel"]\
             -relief link -command "$dlg enddialog cancel"
    
    set w [$dlg getframe]
    wm resizable $dlg 0 0
    bind $widget(dlg) <F1> "ShowHelp Tb4Cbd"

    # one module ?
    Label $w.lbmodule -text "[mc Connection]:"
    if {[string length $module]} {
      set widget(module) [Entry $w.ecbmodule -editable false \
                                             -text [$module cget -name]]
    } else {
      set widget(module) \
        [ComboBox $w.ecbmodule -editable false \
           -values [lindex [ltraspose $lstConnection] 0] \
           -modifycmd [list $self _ModifyLstConnection]]
# BComboBox (desactivado hasta que se realize un nuevo BComboBox)
#        [bcombobox $w.ecbmodule -editable false \
                   -values [lindex [ltraspose $lstConnection] 0] \
                   -bkeypress true \
                   -modifycmd [list $self _ModifyLstConnection]]
    }
    Label $w.lbalias -text "[mc Alias]:"
    #ComboBox $w.cbalias 
    #  -editable false -textvariable $alias
    set widget(alias) \
      [ComboBox $w.cbalias -editable false \
         -text $alias]
# BComboBox (desactivado hasta que se realize un nuevo BComboBox)
#      [bcombobox $w.cbalias -editable false \
                 -text $alias \
                 -bkeypress true]
    Label $w.lbuser -text "[mc User]:"
    set widget(user) [Entry $w.euser -text $user]
    Label $w.lbpass -text "[mc Password]:"
    set widget(pwd) [Entry $w.epass  -show "*" -text $password]
    set datasources [lsort -index 0 [database datasources]]
    #  puts "DataSources : $datasources"
    set all_alias ""
    foreach al $datasources {
      lappend all_alias [lindex $al 0]
    }
    #  puts "ALL_alias: $all_alias"
    SetControlsStatus $self $dlg $status
    $w.cbalias configure -values $all_alias

    grid $w.lbmodule     -row 0 -column 0 -sticky e -padx 4
    grid $w.ecbmodule    -row 0 -column 1 -sticky w -padx 2
    grid $w.lbalias      -row 1 -column 0 -sticky e -padx 4
    grid $w.cbalias      -row 1 -column 1 -sticky w -padx 2
    grid $w.lbuser       -row 2 -column 0 -sticky e -padx 4
    grid $w.euser        -row 2 -column 1 -sticky w -padx 2
    grid $w.lbpass       -row 3 -column 0 -sticky e -padx 4
    grid $w.epass        -row 3 -column 1 -sticky w -padx 2
    grid rowconfigure $w 1 -pad 6
    
    set result [$dlg draw]
    # puts "draw == $result"
    

    destroy $dlg
Tolcon_Trace "_CreateDialog = $result"
    return $result
  }

  
  #///////////////////////////////////////////////////////////////////////////
  method _ModifyLstConnection {} {
  #
  # PURPOSE: Updates the values after a selection
  #
  #///////////////////////////////////////////////////////////////////////////
    # module
    set namMod [BfoGetText $widget(module)]
    # load values
    set module [$self _GetModuleToName $namMod]
    if {[string length $module]} {
      $widget(alias) configure -text [$module cget -alias]
      $widget(user)  configure -text [$module cget -user]
      $widget(pwd)   configure -text [$module cget -pwd]
      # control state
      SetControlsStatus $self $widget(dlg) [$self checkconnection $module]
    } else {
      $widget(alias) configure -text {}
      $widget(user)  configure -text {}
      $widget(pwd)   configure -text {}
    }
    
    # set the focus in the first field not filled
    if {!([$module cget -alias] eq "")} {
      if {!([$module cget -user] eq "")} {
        if {!([$module cget -pwd] eq "")} {
          focus $widget(module)
        } else {
          focus $widget(pwd)
        }          
      } else {
        focus $widget(user)
      }        
    } else {
      focus $widget(alias)
    }
  }

  #/////////////////////////////////////////////////////////////////////////////
  proc SetControlsStatus {self dlg status} {
  #
  # PURPOSE: Sets controls state to normal/disabled according to connection
  #          status.
  #
  # PARAMETERS:
  #   self   -> instancia of bodbc
  #   dlg    -> Instance of ODBCDialog
  #   status -> connection status (1 connected, 0 disconnected)
  #
  #/////////////////////////////////////////////////////////////////////////////
    set w [$dlg getframe]
    if {$status} {
      set newStat disabled
      # button disconnect
      $dlg itemconfigure 0 -text [mc "Disconnect"] -image [::Bitmap::get stop]\
             -compound right -helptext [mc "Disconnect"] \
             -relief link -command [list $self _Disconnect]
    } else  {
      set newStat normal
      #button disconnect
      $dlg itemconfigure 0 -text [mc "Connect"] -image [::Bitmap::get play]\
             -compound right -helptext [mc "Connect"] \
             -relief link -command [list $self _ConnectDlg]
      
    }
    $w.cbalias configure -state $newStat
    $w.euser   configure -state $newStat
    $w.epass   configure -state $newStat
    
    if {[$w.epass cget -text] eq ""} {
      focus $w.epass
    }
    # set the focus in the first field not filled
    if {!([$w.cbalias cget -text] eq "")} {
      if {!([$w.euser cget -text] eq "")} {
        if {!([$w.epass cget -text] eq "")} {
          focus $dlg
        } else {
          focus $w.epass
        }          
      } else {
        focus $w.euser
      }        
    } else {
      focus $w.cbalias
    }
    
  }


  #///////////////////////////////////////////////////////////////////////////
  method connection {args} {
  #
  # PURPOSE: Create a new module
  #
  # PARAMETERS: 
  #   args: parameters of bodbcmodule
  #
  #///////////////////////////////////////////////////////////////////////////
    # parameters default
    array set opts [list \
                      -name   {} \
                   ]
    array set opts $args   
    # exists $mod ?
    if {[string length $opts(-name)]} {
      if {[info exists lstConnection]} {
        set lst [lindex [ltraspose $lstConnection] 0]
        if {[expr [lsearch $lst $opts(-name)] != -1]} {
          puts "Error. Modulo ya asignado."
          return {}
        }
      }
    } else  {
      puts "Error. No se ha especificado un nombre al modulo"
      return {}
    } 
    # create a new module
    set namCon [$self _UniqueName $opts(-name)]
    set namMod [eval ::bodbcmodule $namCon $args]
    lappend lstConnection "$opts(-name) $namMod"
    return $namMod
  }

  #///////////////////////////////////////////////////////////////////////////
  method unregister {module} {
  #
  # PURPOSE: Eliminated one module register
  #
  # PARAMETERS:
  #   module: name of connection (bodbcmodule)
  #
  #///////////////////////////////////////////////////////////////////////////
    # eliminate register
    set lst [ltraspose $lstConnection]
    set inx [lsearch [lindex $lst 1] $module]
    if {[expr $inx != -1]} {
      set lstConnection [lreplace $lstConnection $inx $inx]
      # disconnect
      if {[$self checkconnectiontcl $module]} {
        $self _DisconnectTcl $module
      }
      if {[$self checkconnectiontol $module]} {
        $self _DisconnectTol $module [$module cget -alias]
      }
      # destroy
      $module destroy
    }
  } 
 

  #///////////////////////////////////////////////////////////////////////////
  method get { var } {
  #
  # PURPOSE: Obtain the value of a variable
  #
  # PARAMETERS: var -> name of variable
  #
  # RETURN: value of variable
  #
  #///////////////////////////////////////////////////////////////////////////
    return [subst $[subst "$var"]]
  }

  #///////////////////////////////////////////////////////////////////////////
  method _UniqueName { namMod } {
  #
  # PURPOSE: Obtain a unique name
  #
  # RETURN: name of bodbcmodule
  #
  #///////////////////////////////////////////////////////////////////////////
    set lst [lindex [ltraspose $lstConnection] 1]
    while 1 {
      incr counter
      if {$counter > 2147483647} {
        set counter 0
      }
      set candidate db${namMod}${counter}
      if {[expr [lsearch $lst $candidate] == -1]} {
        return $candidate
      }
    }
  }

  #/////////////////////////////////////////////////////////////////////////// 
  method _GetModuleToName { namMod } {
  #
  # PURPOSE: From the name of I modulate, we obtain the name of connection TCL
  #
  # PARAMETERS:
  #   namMod -> name of module
  #
  # RETURN: name of connection TCL
  #
  #///////////////////////////////////////////////////////////////////////////
    set module {}
    if {[string length $namMod]} {
      set lst [ltraspose $lstConnection]
      set inx [lsearch [lindex $lst 0] $namMod]
      if {[expr $inx != -1]} {
        set module [lindex [lindex $lst 1] $inx]
      } else {
        set module {}
      } 
    }
    return $module
  }

  #///////////////////////////////////////////////////////////////////////////
  proc GetAliasOpen {alias} {
  #
  # PURPOSE: It verifies that connection TOL this open one
  #
  # PARAMETERS: alias -> name of bridget
  #
  # RETURN: 1 -> open
  #         0 -> no 
  #
  #///////////////////////////////////////////////////////////////////////////

    set result 0
    # command
    set op [GetUniqueName Set]
    set cmd "Set $op = DBGetOpened(\"\");"
    # evaluation
    ::tol::console eval $cmd
    #Capturar el valor devuelto por TOL
    array set aryInfo [TclInfoVar Set $op]
    set lenAry [lindex $aryInfo(CONTENT) 0]
    set inx 1
    while {!($result) && ([expr $inx <= $lenAry])} {
      array set infoReg [TclInfoRef $op $inx 1]
      set aliasConn [lindex $infoReg(CONTENT) 0]
      if {[string equal $alias $aliasConn]} {
        set result 1
      }
      incr inx
    }
    ::tol::console stack release $op
    # return
#Tolcon_Trace "GetAliasOpen $result"
    return $result
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc test_bodbc {} {
#
# PURPOSE: Test function of a bodbc
#
#/////////////////////////////////////////////////////////////////////////////
#  package require msgcat
#  namespace import msgcat::*
#  ::msgcat::mclocale es
  
  package require tclodbc
  package require BWidget
  package require BLT
  Bitmap::use
  #lappend Bitmap::path ../images

  if {[winfo exists ._cnnopt]} {
    destroy ._cnnopt
  }

  set b [bodbc b]

  set telesp [$b connection -name telesp -alias ddw_ld -user dbo -pwd dbo]
  set sadd   [$b connection -name sadd   -alias prueba -user bayes -pwd bayes]

  set latam   [$b connection -name latam  -alias TfnLatam -user bayes -pwd bayes \
     -stdtolcon 0 -stdtclcon 1 -procconnect [list puts "Conexion realizada"] \
     -prodisconnect [list puts "Deconexión realizada"] \
     -iniconfig     TflLatamODBC \
     -savealias     1 \
     -saveuser      1 \
     -savepwd       0]

  set prbini  [$b connection -name prbini  \
     -procconnect [list puts "Conexion realizada"] \
     -prodisconnect [list puts "Deconexión realizada"] \
     -iniconfig     PruebaIniODBC \
     -savealias     1 \
     -saveuser      1 \
     -savepwd       1]


  $b connection -name sadd

  puts "Modulos cargados: [$b get lstConnection]"

  #$b showdialog
  $b showdialog
}


package provide bodbc 1.0
