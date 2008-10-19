#/////////////////////////////////////////////////////////////////////////////
# FILE    : btoplevel.tcl
# PURPOSE : Bayes Top Level. TopLevel con opciones
#/////////////////////////////////////////////////////////////////////////////


package require byscommon


#/////////////////////////////////////////////////////////////////////////////
::snit::widgetadaptor btoplevel {
#
# PURPOSE: Sobrecargamos un toplevel para que tenga nuevas funcionalidades
#
#/////////////////////////////////////////////////////////////////////////////
  # opciones especificas
  # title -> titulo del toplevel
  # closecmd -> comando a ejecutar al cerrar el toplevel
  # iniconfig -> Nombre que se asigna al toplevel para identificar sus opciones
  #           en el archivo de configuración .ini por defecto "" en ese caso
  #           no se guardaran las opciones
  # geometry -> Geometria inicial, antes de que exista en el .ini
#/////////////////////////////////////////////////////////////////////////////
  option -title    {}
  option -closecmd 
  option -iniconfig    ""
  option -geometry 780x580+10+10
#/////////////////////////////////////////////////////////////////////////////


  #///////////////////////////////////////////////////////////////////////////
  constructor {args} {
  #
  # PURPOSE: Constructor de un btoplevel. Crea una instancia de un toplevel
  #
  # PARAMETERS:
  #   args -> Lista de opciones especificas de un toplevel, mas las propias de
  #           un btoplevel
  #
  #///////////////////////////////////////////////////////////////////////////
    # Widget que va a servir de contenedor para btoplevel
    installhull [toplevel $self]
    # Apply all arguments given:
    $self configurelist $args
    # Destroy
#    bind $self <Destroy> +[list [codename Destroy] $self]
    wm protocol $self WM_DELETE_WINDOW [list [codename Destroy] $self] 
    # .ini
    ReadIni $self
  }


  #///////////////////////////////////////////////////////////////////////////
  proc Destroy {self} {
  #
  # PURPOSE: Funciones a evaluar al cerrar un toplevel. Si no tiene
  #          procedimento propio se asume que se desea destruir el toplevel
  #
  # PARAMETERS:
  #   self: toplevel
  #
  #///////////////////////////////////////////////////////////////////////////
    # Save .ini
    WriteIni $self
    # destroy 
    set cmd [$self cget -closecmd]
    if {[string length $cmd]} {
       eval $cmd
    } else  {
      destroy $self
    }
  }


  #///////////////////////////////////////////////////////////////////////////
  proc WriteIni {self} {
  #
  # PURPOSE: Guarda en el .ini informacion referente al toplevel
  #
  # PARAMETERS:
  #   self: toplevel
  #
  #///////////////////////////////////////////////////////////////////////////
    if { [$self cget -iniconfig] ne "" } {
      # comprobamos cargado namespace y variable de .ini
#&& [info exists idIni]
      if {[namespace exists ::iniFile]} {
        set wini ::iniFile::Write
        set nm [$self cget -iniconfig]
        if {![string length $nm]} {
          set nm $self
        }
        $wini $nm wm,state [wm state    $self]
        $wini $nm geometry [wm geometry $self]
      }
    }
  }


  #///////////////////////////////////////////////////////////////////////////
  proc ReadIni {self} {
  #
  # PURPOSE: Recupera del .ini informacion referente al toplevel
  #
  # PARAMETERS:
  #   self: toplevel
  #
  #///////////////////////////////////////////////////////////////////////////
    if { [$self cget -iniconfig] ne "" } {
      # comprobamos cargado namespace y variable de .ini
#&& [info exists idIni]
      if {[namespace exists ::iniFile]} {

        set rini ::iniFile::Read
        set nm [$self cget -iniconfig]
        if {![string length $nm]} {
          set nm $self
        }
        set stt [::iniFile::ReadWMState $rini $nm]
        set geo [$rini $nm geometry [$self cget -geometry]]
        if {[string length $geo]} {
          wm geometry $self $geo
        }
        wm state $self $stt
      }
    }
  }


  #///////////////////////////////////////////////////////////////////////////
  # cget and configure
  #///////////////////////////////////////////////////////////////////////////
  oncget -title {
    return [wm title $self]
  }
  onconfigure -title {value} {
    wm title $self $value
  }

  # delegacion de metodos y opciones
  delegate method * to hull
  delegate option * to hull
}

package provide btoplevel 1.0
