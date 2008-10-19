#/////////////////////////////////////////////////////////////////////////////
# FILE    : bfobjects.tcl -> bfo
# PURPOSE : Bayes Function Objects. Funciones sobre objetos
#/////////////////////////////////////////////////////////////////////////////

#/////////////////////////////////////////////////////////////////////////////
proc GetOptionValue {lst option {default ""}} {
#
# PURPOSE: Obtener el valor para un argumento de un cojunto de opciones
#
# PARAMETERS:
#   lst     -> Lista de opciones
#   option  -> Opcion a buscar el valor
#   default -> Valor por defecto por si no esta especificada la opcion
#
# RETURN: El valor de la opcion especificada. De no existir devuelve ""
#
#/////////////////////////////////////////////////////////////////////////////
  set value $default
  set posi  [lsearch -exact $lst $option]
  if {$posi >= 0} {
    set value [lindex $lst [expr $posi + 1]]
  } 
  return $value
}

#/////////////////////////////////////////////////////////////////////////////
proc DeleteOption {lst option} {
#
# PURPOSE: Elimina una opción de una lista de argumentos
#
# PARAMETERS:
#   lst -> Lista de opciones
#   option -> Opcion a eleiminar
#
# RETURN: La lista de argumentos sin la opcion espcificada
#
#/////////////////////////////////////////////////////////////////////////////
  set posi [lsearch -exact $lst $option]
  if {$posi >= 0} {
    set arg  [lreplace $lst $posi [expr $posi + 1]]
  } else  {
    set arg $lst
  }
  return $arg
}

#/////////////////////////////////////////////////////////////////////////////
proc BfoCleanPath {path} {
#
# PURPOSE: Limpia el contenido de todos los widget de un path
#
# PARAMETERS:
#   path -> contenedor de widget a limpiar
#
#/////////////////////////////////////////////////////////////////////////////
  foreach w [winfo children $path] {
    BfoCleanWidget $w
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc BfoCleanWidget {widget} {
#
# PURPOSE: Borra el contenido de un widget
#
# PARAMETERS:
#   widget -> objeto a limpiar
#
#/////////////////////////////////////////////////////////////////////////////
  # capturamos el tipo
  set type [winfo class $widget]
  # comprobamos
  switch -exact $type {
    "BSelectFields"
    {
      $widget SetValue ""
    }
    "BComboBox"
    {
      #$widget BClear
      $widget list clear
    }
    "Bcomboboxplus"
    {
      $widget key clean
      $widget values clean
    }
    "BEditMaster"
    {
      BfoCleanPath $widget  
    }
    "BEntryDate" {
      $widget ClearDate
    }
    "Frame" {
        BfoCleanPath $widget
    }
    "Mclistbox" {
      # Estudiar como limpiar los datos
      BfoCleanPath $widget
    }
    "ComboBox" {
      $widget configure -text ""
    }
    "Entry" {
      set stt [$widget cget -state]
      $widget configure -state normal
      $widget delete 0 end
      $widget configure -state $stt
    }
    "LabelEntry" {
      set stt [$widget cget -state]
      $widget configure -state normal 
      $widget.e delete 0 end
      $widget configure -state $stt
    }
    "Listbox" {
      set stt [$widget cget -state]
      $widget configure -state normal      
      $widget delete 0 end
      $widget configure -state $stt
    }
    "Text" { 
      set stt [$widget cget -state]
      $widget configure -state normal      
      $widget delete 0.0 end
      $widget configure -state $stt
    }
    "Label" {
    }
    "Button" {
    }
    "Checkbutton" { 
      set stt [$widget cget -state]
      $widget configure -state normal      
      $widget deselect
      $widget configure -state $stt
    }
    default
    {
      puts "BfoCleanWidget. Widget type not recognized: $type"
      if [llength [winfo children $widget]] {
        BfoCleanPath $widget
      }
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc BfoActivateWidget {widget stt} {
#
# PURPOSE: Borra el contenido de un widget esecificos disennados por 
#          Bayes Inference
#
# PARAMETERS:
#   widget -> objeto a limpiar
#   stt    -> estado aplicar [normal | active | select | disabled | notactive]
#             Nota: Si el widget no admite todas las opciones se cambia
#               select -> normal
#               disabled, notactive -> disabled
#
#/////////////////////////////////////////////////////////////////////////////
  # capturamos el tipo
  set type [winfo class $widget]
  # comprobamos
  switch -exact $type {
    BSelectFields {
      $widget Activate $stt
    }
    Bcomboboxplus {
      $widget activate $stt
    }
    BEntryDate {
      switch $stt {
        readonly    -
        notactive   {
          set stt disabled
        }
        select {
          set stt normal
        }
      }
      $widget configure -bstate $stt
    }
    Frame {
#        BfoCleanPath $widget
    }
    Mclistbox {
#      BfoCleanPath $widget
    }
    ComboBox { 
      switch $stt {
        active -
        select {
          set stt normal
        }
        readonly -   
        notactive {
          set stt disabled
        }
      }
      $widget configure -state $stt
    }
    Entry {
      switch $stt {
        active - 
        select {
          set stt normal
        }
        notactive {
          set stt disabled
        }
      } 
      $widget configure -state $stt
    }
    LabelEntry  {
      switch $stt {
        active {
          set edi true
          set stt normal
        }
        normal -
        select {
          set edi true
          set stt normal
        }
        disabled -
        notactive {
          set edi false
          set stt disabled
        }
      }
      $widget configure -state    $stt
      $widget configure -editable $edi
    }
    Listbox
    {
      switch $stt {
        active -
        select {
          set stt normal
        }
        notactive {
          set stt disabled
        }
      }
      $widget configure -state $stt
    }
    Text
    {
      switch $stt {
        active -
        select {
          set stt normal
        }
        notactive {
          set stt disabled
        }
      }
      $widget configure -state $stt
    }
    Label {
      switch $stt {
        active -
        select {
          set stt normal
        }
        notactive {
          set stt disabled
        }
      }
      $widget configure -state $stt
    }
    Button {
      switch $stt {
        notactive {
          set stt disabled
        }
        select {
          set stt normal
        } 
      }
      $widget configure -state $stt
    }
    ArrowButton {
      switch $stt {
        notactive {
          set stt disabled
        }
        select {
          set stt normal
        }
      }
      $widget configure -state $stt
    }
    Checkbutton {
      switch $stt {
        notactive -
        disabled 
        {
          set stt disabled
        }
        select -
        active 
        {
          set stt normal
        }
      }
      $widget configure -state $stt
    }

    default
    {
      puts "BfoActivateWidget. Widget type not recognized: $type"
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc BfoGetText {widget} {
#
# PURPOSE: Obtiene el texto introducido en un objeto
#
# PARAMETERS:
#   widget -> objeto
#
# RETURN: el texto introducido
#
#/////////////////////////////////////////////////////////////////////////////
#Tolcon_Trace "BfoGetText widget: $widget , tipo:[winfo class $widget]"
  set txt ""
  # capturamos el tipo
  set type [winfo class $widget]
  # comprobamos
  switch -exact $type {
    "BSelectFields" {
      set txt [$widget GetCode]
    }
    "BEntryDate" {
      set txt [$widget cget -btext]
    }
    "ComboBox" { 
      set txt [$widget cget -text]
    }
    "Entry" {
      set txt [$widget cget -text]
    }
    "LabelEntry" {
      set txt [$widget cget -text]
    }
    "Checkbutton" {
      set var [$widget cget -variable]
      eval set txt $$var
    }
    "Text" {
      set txt2 [$widget get 1.0 end]
      set txt [string replace $txt2 end end {}]
    }
    "Bcomboboxplus" {
      set txt [$widget key get]
    }
    default
    {
      puts "BfoGetText. Widget type not recognized: $type"
    }
  }
  return $txt
}


#/////////////////////////////////////////////////////////////////////////////
proc BfoSetText {widget txt} {
#
# PURPOSE: Set the input text on a widget depending its type.
#
# PARAMETERS:
#   widget -> object
#   txt -> text
#
#/////////////////////////////////////////////////////////////////////////////
  # capturing the type
  set type [winfo class $widget]
  # set the text on the widget
  switch -exact $type {
    "BEntryDate" {
      $widget configure -btext $txt
    }
    "ComboBox" {
      $widget configure -text $txt      
    }
    "Entry" {
      $widget configure -text $txt            
    }
    "LabelEntry" {
      $widget configure -text $txt            
    }
    "Bcomboboxplus" {
      $widget configure -text $txt
    }
    default
    {
      puts "BfoSetText. Widget type not recognized: $type"
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
  proc SetAttribute {w attribute value} {
#
# PURPOSE: Applies recursively an attribute value to all childs of widget w.
#          If a child doesn't have this attribute, doesn't produce an error.A  
#
# PARAMETERS:
#   w         -> widget
#   attribute -> attribute to modify
#   value     -> value of attribute
#
#/////////////////////////////////////////////////////////////////////////////
  foreach ch [winfo children $w] {
    SetAttribute $ch $attribute $value
  }
  catch {$w configure -$attribute $value}
}