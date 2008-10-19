# FILE    : newToolbarDiag.tcl
# PURPOSE : Formulario destinado a los parámetros necesarios para visualizar
#           Anómalos en el proyecto ForNews.
#
# Data Variables para controles:
#
#/////////////////////////////////////////////////////////////////////////////

#/////////////////////////////////////////////////////////////////////////////
namespace eval ::newToolbarDiag {
#
# PURPOSE: Namespace principal del visualizador
#
#/////////////////////////////////////////////////////////////////////////////
  variable widget
  variable data
  
}

#/////////////////////////////////////////////////////////////////////////////
proc ::newToolbarDiag::Show { args } {
#
# PURPOSE: Muestra el formulario, de no existir lo crea
#
#/////////////////////////////////////////////////////////////////////////////
  variable widget
  variable data

  if { [info exist widget(document)] && [winfo exists $widget(document)] } {
    if { [$widget(document) cget -state] eq "minimized" } {
      $widget(document) restore
    }
    $widget(document) raise
  } else {
    if [llength $args] {
      eval Create $args
    } else {
      Create
    }
  }
  
}

#/////////////////////////////////////////////////////////////////////////////
proc ::newToolbarDiag::Create { args } {
#
# PURPOSE: Crea un formulario destinado a la Tirada de ForNews
#
#/////////////////////////////////////////////////////////////////////////////
  variable widget
  variable data
  
  array set options {
    -toolbartype ""
    -toolbarparent ""
  }
  
  array set options  $args

  if {[llength $args]} {
    puts "TIENE ARGS"
    #read ini
    ::newToolbarDiag::ReadIni
    
    set data(tooldig,toolbartype)   $options(-toolbartype)
    set data(tooldig,toolbarparent) $options(-toolbarparent)
    puts "data(tooldig,toolbartype) -> $data(tooldig,toolbartype)"
    puts "data(tooldig,toolbarparent) -> $data(tooldig,toolbarparent)"
  } else {
    puts "no tiene parametros"
  }

  set widget(document) [::project::CreateForm \
    -type [mc {For News}] -iniconfig FORNEWSnewToolbarDiag \
    -modal 1 \
    -height 385 \
    -width 310 \
    -resizable 1 \
    -title [mc {New Toolbar Wizard}] \
    -closecmd [list ::newToolbarDiag::Destroy]]
  
  set tl $widget(document).work
  
  set widget(tfwizard)  [CreateTFWizard   $tl]
  set widget(bbox)      [CreateBBox       $tl]

  grid $widget(tfwizard) -sticky news
  grid $widget(bbox)     -sticky nes
  
  grid rowconfigure    $tl 0  -weight 1
  grid rowconfigure    $tl 1  -weight 0 

  grid columnconfigure $tl 0 -weight 1

}

#/////////////////////////////////////////////////////////////////////////////
proc ::newToolbarDiag::CreateTFWizard {tl} {
#
# PURPOSE: Creamos el frame con las opciones comunes de la ventana
# PARAMETERS: tl -> path donde creamos el componente
# RETURN: devolvemos el path del objeto creado
#
#/////////////////////////////////////////////////////////////////////////////
  variable widget
  variable data

  # titulo
  set tf [TitleFrame $tl.params \
    -text [mc {Options}] -bd 1 -side left]
  set f [$tf getframe]
  
  set f2 [frame $f.f2 -bd 0]
  
  Label $f2.leToolname  -text [mc {ToolName}]  \
    -justify left -anchor w
  
  set widget(tooldig,toolname) \
    [Entry $f2.etoolname -state normal -width 20 \
      -textvariable ::newToolbarDiag::data(tooldig,toolname)]
    
  grid $f2.leToolname $widget(tooldig,toolname)  -sticky news
  
  grid columnconfigure $f2 0 -weight 0
  grid columnconfigure $f2 1 -weight 1
  grid rowconfigure    $f2 0 -weight 0

  grid $f2                     -sticky news
  
  grid rowconfigure $f 0  -weight 0
  grid rowconfigure $f 10 -weight 1
  grid columnconfigure $f 0  -weight 1
  
  return $tf
}


#/////////////////////////////////////////////////////////////////////////////
proc ::newToolbarDiag::CreateBBox {tl} {
#
# PURPOSE: Creamos el frame con las opciones comunes de la ventana
# PARAMETERS: tl -> path donde creamos el componente
# RETURN: devolvemos el path del objeto creado
#
#/////////////////////////////////////////////////////////////////////////////
  set bbox \
    [ButtonBox $tl.bb -orient horizontal -spacing 10 -padx 0 -pady 0 \
      -homogeneous false]
  
  $bbox add -text [mc "Create"] -relief link \
    -command [list ::newToolbarDiag::Create_command] \
    -helptext [mc "Create button"] -disabledforeground ""\
    -image [Bitmap::get bexec] -compound left -padx 3 -pady 1
  $bbox add -text [mc "Exit"] -relief link \
    -command [list ::newToolbarDiag::Destroy] \
    -helptext [mc "Close window"] -disabledforeground ""\
    -image [Bitmap::get exit] -compound left -padx 3 -pady 1
     
  return $bbox  
}


#/////////////////////////////////////////////////////////////////////////////
proc ::newToolbarDiag::ReadIni { } {
#
# PURPOSE: Lee del ini 
#
#/////////////////////////////////////////////////////////////////////////////
  variable data
  variable widget

  Tolcon_Trace "Leyendo ini"
  set rini ::iniFile::Read
  set key FORNEWSnewToolbarDiag
  
}

#/////////////////////////////////////////////////////////////////////////////
proc ::newToolbarDiag::WriteIni { } {
#
# PURPOSE: Escribe en el ini
#
#/////////////////////////////////////////////////////////////////////////////
  variable data
  variable widget
   
  set wini ::iniFile::Write
  set key FORNEWSnewToolbarDiag
  Tolcon_Trace "Guardando ini"

  return 1
}

#/////////////////////////////////////////////////////////////////////////////
proc ::newToolbarDiag::Destroy { } {
#
# PURPOSE: Cerramos la ventana y destruimos toda las variables temporales
#
#/////////////////////////////////////////////////////////////////////////////
  variable data
  variable widget
   
  destroy $widget(document)
  array unset data
  array unset widget
  return 1
}

#/////////////////////////////////////////////////////////////////////////////
proc ::newToolbarDiag::Create_command {} {
#
# PURPOSE: Creamos el frame con las opciones comunes de la ventana
# PARAMETERS: tl -> path donde creamos el componente
# RETURN: devolvemos el path del objeto creado
#
#/////////////////////////////////////////////////////////////////////////////
  variable widget
  variable data
  
  set ok 0
   
  switch $data(tooldig,toolbartype) {
    "toolbar" {
      set winfoAux "${data(tooldig,toolbarparent)}.tb_${data(tooldig,toolname)}"
    }
    "toolbarframe"  {
      set winfoAux "$::project::widgets(mainframe).toolbarFrame_${data(tooldig,toolname)}"
    }
  }
  
  puts "WINFOAUX -> $winfoAux"
  
  if {[llength $data(tooldig,toolname)]} {
    set ok 1
    puts "INFORMACION ----> winfo exists $winfoAux"
    if {[winfo exists $winfoAux]} {
      set ok 0
      set msg [mc {Toolbar Name already exists}]
      ShowMessage W $msg $widget(document)
    } else {
      set ok 1 
    }
  } else {
    set msg [mc {Toolbar Name is required}]
    ShowMessage W $msg $widget(document)
    set ok 0
  }
  
  if {$ok} {
    
    switch $data(tooldig,toolbartype) {
      "toolbar" {
        #data(tooldig,toolbarparent)
        ::TolConsole::NewToolBarB $data(tooldig,toolbarparent) $data(tooldig,toolname)
        ::newToolbarDiag::Destroy
      }
      "toolbarframe"  {
        ::TolConsole::NewToolFrame $data(tooldig,toolname) 8
        ::newToolbarDiag::Destroy
      }
    }
  }
}

