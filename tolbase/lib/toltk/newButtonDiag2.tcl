# FILE    : newButtonDiag2.tcl
# PURPOSE : Formulario destinado a los parámetros necesarios para visualizar
#           Anómalos en el proyecto ForNews.
#
# Data Variables para controles:
#
#/////////////////////////////////////////////////////////////////////////////

#/////////////////////////////////////////////////////////////////////////////
namespace eval ::newButtonDiag2 {
#
# PURPOSE: Namespace principal del visualizador
#
#/////////////////////////////////////////////////////////////////////////////
  variable widget
  variable data
  
}

#/////////////////////////////////////////////////////////////////////////////
proc ::newButtonDiag2::Show { args } {
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
proc ::newButtonDiag2::Create { args } {
#
# PURPOSE: Crea un formulario destinado a la Tirada de ForNews
#
#/////////////////////////////////////////////////////////////////////////////
  variable widget
  variable data
  
  array set options {
    -toolbar ""
    -cmdlst {}
  }
  
  array set options  $args

  if {[llength $args]} {
    puts "TIENE ARGS"
    #read ini
    ::newButtonDiag2::ReadIni
    
    set data(butdig,toolbarfather) $options(-toolbar)
    set data(butdig,cmdlst)        $options(-cmdlst)
    puts "data(butdig,cmdlst) -> $data(butdig,cmdlst)"
  } else {
    puts "no tiene parametros"
  }

  set widget(document) [::project::CreateForm \
    -type [mc {For News}] -iniconfig FORNEWSnewButtonDiag2 \
    -modal 1 \
    -height 385 \
    -width 310 \
    -resizable 1 \
    -title [mc {New Button Wizard}] \
    -closecmd [list ::newButtonDiag2::Destroy]]
  
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
proc ::newButtonDiag2::CreateTFWizard {tl} {
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
  
  set f1 [frame $f.f1 -bd 0]
  
  Label $f1.lePathGCF  -text [mc {Image Selection}]  \
    -justify left -anchor w
  
  set widget(butdig,pathgcf) \
    [Entry $f1.epathgcf -state normal -width 20 \
      -textvariable ::newButtonDiag2::data(butdig,imagepath)]

  Button $f1.bPathGCF -compound left -padx 10 \
    -height 16 -relief link \
    -helptext [mc {Search image button file}] \
    -image [Bitmap::get bopen] \
    -command [list ::newButtonDiag2::GetImagePath]
  
  grid $f1.lePathGCF                        -sticky nw
  grid $widget(butdig,pathgcf) $f1.bPathGCF -sticky news
  
  grid columnconfigure $f1 0 -weight 1
  grid columnconfigure $f1 1 -weight 0
  grid rowconfigure    $f1 0 -weight 0
  grid rowconfigure    $f1 1 -weight 0
  
  set lstRes {}  
  set widget(butdig,command) \
    [beditselector $f.bescommand -label [mc {Form}] \
      -cols "[mc Form] [mc Commands]" \
      -selcols "[mc Form]" \
      -entcols [mc Form] -hlpcols "[mc Form]" \
      -list $data(butdig,cmdlst) \
      -readonly false -width 100 \
      -unique 1]
  
  set f2 [frame $f.f2 -bd 0]
  
  Label $f2.leTooltip  -text [mc {Tooltip}]  \
    -justify left -anchor w
  
  set widget(butdig,tooltip) \
    [Entry $f2.etooltip -state normal -width 20 \
      -textvariable ::newButtonDiag2::data(butdig,tooltip)]
    
  grid $f2.leTooltip $widget(butdig,tooltip)  -sticky news
  
  grid columnconfigure $f2 0 -weight 0
  grid columnconfigure $f2 1 -weight 1
  grid rowconfigure    $f2 0 -weight 0

  grid $f1                     -sticky news 
  grid $widget(butdig,command) -sticky news
  grid $f2                     -sticky news
  
  grid rowconfigure $f 0  -weight 0
  grid rowconfigure $f 1  -weight 0
  grid rowconfigure $f 2  -weight 0
  grid rowconfigure $f 10 -weight 1
  grid columnconfigure $f 0  -weight 1
  #grid columnconfigure $f 10 -weight 1
  
  return $tf
}


#/////////////////////////////////////////////////////////////////////////////
proc ::newButtonDiag2::CreateBBox {tl} {
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
    -command [list ::newButtonDiag2::Create_command] \
    -helptext [mc "Create button"] -disabledforeground ""\
    -image [Bitmap::get ok_txt] -compound left -padx 3 -pady 1
  $bbox add -text [mc "Exit"] -relief link \
    -command [list ::newButtonDiag2::Destroy] \
    -helptext [mc "Close window"] -disabledforeground ""\
    -image [Bitmap::get exit] -compound left -padx 3 -pady 1
     
  return $bbox  
}


#/////////////////////////////////////////////////////////////////////////////
proc ::newButtonDiag2::ReadIni { } {
#
# PURPOSE: Lee del ini 
#
#/////////////////////////////////////////////////////////////////////////////
  variable data
  variable widget

  Tolcon_Trace "Leyendo ini"
  set rini ::iniFile::Read
  set key FORNEWSnewButtonDiag2
  
}

#/////////////////////////////////////////////////////////////////////////////
proc ::newButtonDiag2::WriteIni { } {
#
# PURPOSE: Escribe en el ini
#
#/////////////////////////////////////////////////////////////////////////////
  variable data
  variable widget
   
  set wini ::iniFile::Write
  set key FORNEWSnewButtonDiag2
  Tolcon_Trace "Guardando ini"

  return 1
}

#/////////////////////////////////////////////////////////////////////////////
proc ::newButtonDiag2::Destroy { } {
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
proc ::newButtonDiag2::Create_command { } {
#
# PURPOSE: Creamos el frame con las opciones comunes de la ventana
# PARAMETERS: tl -> path donde creamos el componente
# RETURN: devolvemos el path del objeto creado
#
#/////////////////////////////////////////////////////////////////////////////
  variable widget
  variable data
  
  set data(butdig,command) \
    [$widget(butdig,command) selection get selected [mc Commands]]
    
  set ok 0
  
  if {[llength $data(butdig,imagepath)]} {
    set ok 1
    if {[llength $data(butdig,command)]} {
      set ok 1
    } else {
      set msg [mc {Button command is required}]
      ShowMessage W $msg $widget(document)
      set ok 0
    }
  } else {
    set msg [mc {Image button is required}]
    ShowMessage W $msg $widget(document)
    set ok 0
  }
  
  if {$ok} {
    puts "tb  -> $data(butdig,toolbarfather)"
    puts "pic -> $data(butdig,imagepath)"
    puts "cmd -> $data(butdig,command)"
    puts "hlp -> $data(butdig,tooltip)"
    
    set Tb  $data(butdig,toolbarfather)
    set pic $data(butdig,imagepath)
    eval eval set cmd $data(butdig,command)
    set hlp $data(butdig,tooltip)
    set parent [winfo parent $Tb]
    ::TolConsole::NewButtonB $Tb $pic $hlp $cmd $parent
    ::newButtonDiag2::Destroy
  }
  
}

#/////////////////////////////////////////////////////////////////////////////
proc ::newButtonDiag2::GetImagePath {} {
#
# PURPOSE: Llama a tk_getOpenFile para guardar el path en la variable data
#          correspondiente
#/////////////////////////////////////////////////////////////////////////////
  variable widget
  variable data
  
  set typelist {{"Gif Files" {".gif"}}}
  set defExt   .gif
  set ::newButtonDiag2::data(butdig,imagepath) \
    [tk_getOpenFile \
      -defaultextension $defExt \
      -filetypes $typelist]
}

