#/////////////////////////////////////////////////////////////////////////////
# FILE    : bdialog.tcl
# PURPOSE : Bayes Dialog. Dialog con opciones
#/////////////////////////////////////////////////////////////////////////////


package require byscommon
package require BWidget


#/////////////////////////////////////////////////////////////////////////////
::snit::widgetadaptor bdialog {
#
# PURPOSE: Sobrecargamos un dialog para que tenga nuevas funcionalidades
#
#/////////////////////////////////////////////////////////////////////////////
# opciones especificas  
# iniconfig -> Nombre que se asigna al dialog para identificar sus opciones
#           en el archivo de configuración .ini por defecto "" en ese caso
#           no se guardaran las opciones
#/////////////////////////////////////////////////////////////////////////////
  option -iniconfig    ""
  option -closecmd    ""


  #///////////////////////////////////////////////////////////////////////////
  constructor {args} {
  #
  # PURPOSE: Constructor de un bdialog. Crea una instancia de un Dialog
  #
  # PARAMETERS:
  #   args -> Lista de opciones especificas de un Dialog, mas las propias de
  #           un Dialog
  #
  #///////////////////////////////////////////////////////////////////////////
    # Widget que va a servir de contenedor para bdialog
    installhull [Dialog $self]
    # Apply all arguments given:
    $self configurelist $args
    # Destroy
    wm protocol $self WM_DELETE_WINDOW [list [codename Destroy] $self] 
    # .ini
    ReadIni $self
  }
  
  #///////////////////////////////////////////////////////////////////////////  
  destructor {
  #
  # PURPOSE: Destructor o bdialog.
  #
  #///////////////////////////////////////////////////////////////////////////    
    Destroy $self
  }
  
  #///////////////////////////////////////////////////////////////////////////
  proc Destroy { self } {
  #
  # PURPOSE: Funciones a evaluar al cerrar un toplevel. Si no tiene
  #          procedimento propio se asume que se desea destruir el toplevel
  #
  #
  #///////////////////////////////////////////////////////////////////////////
    # Save .ini
    WriteIni $self
    # destroy 
    # destroy 
    set cmd [$self cget -closecmd]
    if {[string length $cmd]} {
       eval $cmd
    } else  {
      destroy $self
    }
  }
    


  #///////////////////////////////////////////////////////////////////////////
  proc WriteIni { self } {
  #
  # PURPOSE: Guarda en el .ini informacion referente al dialog
  #
  #///////////////////////////////////////////////////////////////////////////
    if { [$self cget -iniconfig] ne "" } {
      # comprobamos cargado namespace y variable de .ini
      if {[namespace exists ::iniFile]} {
        set wini ::iniFile::Write
        set nm [$self cget -iniconfig]
        if {![string length $nm]} {
          set nm $self
        }
        $wini $nm geometry [wm geometry $self]
      }
    }
  }


  #///////////////////////////////////////////////////////////////////////////
  proc ReadIni { self } {
  #
  # PURPOSE: Recupera del .ini informacion referente al dialog
  #
  #///////////////////////////////////////////////////////////////////////////
    if { [$self cget -iniconfig] ne "" } {
      # comprobamos cargado namespace y variable de .ini
      if {[namespace exists ::iniFile]} {
        set rini ::iniFile::Read
        set nm [$self cget -iniconfig]
        if {![string length $nm]} {
          set nm $self
        }
        set stt [::iniFile::ReadWMState $rini $nm]
        set gem [$rini $nm geometry ""]        
        if {[string length $gem]} {
             $self configure -geometry $gem
             wm geometry $self $gem
        }
      }
    }
  }  
  
  # delegacion de metodos y opciones
  delegate method * to hull
  delegate option * to hull
}

package provide bdialog 1.0
