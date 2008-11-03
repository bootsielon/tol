#/////////////////////////////////////////////////////////////////////////////
# FILE    : beditmaster.tcl
# PURPOSE : Bayes Editor Maestro. Editor de registros de tablas de una Base
#           de datos. .ed
# TODO:
#
#/////////////////////////////////////////////////////////////////////////////
# includes

package require byscommon

#/////////////////////////////////////////////////////////////////////////////
#
# To use this source you must create a global variable containing the
# required information about the table and fields that will be shown.
# Take a look at the following information about each field:
#
#/////////////////////////////////////////////////////////////////////////////
# tablename
# regFields($index,TYPE): Structure to store all information regarding table
# columns (fields)
# Each one of the table fields has a unique index.
#
#/////////////////////////////////////////////////////////////////////////////
#
# The TYPE field is as follows:
#
# ptr -> The widget's name.
# label -> Description to print next to the widget.
# nameField -> The name of the database's field/s in a space separated list.
#
# typeField -> Type of the field "nameField" above in a space separated list.
# iskey -> It indicates if the field is PK, and therefore has influence in
#   the rest of fields in the form. It can also give information about how
#   many PK there are.
# masterTable -> Table name in case it is a child field. Otherwise DO NOT
#                define this field.
# masterTableCode -> Master table code field's name. Same as masterTable field
# masterTableDesc -> Master table description field's name. Same as above.
#
#/////////////////////////////////////////////////////////////////////////////
#
# NOTE: The way RegBox is stored is not as a matrix, but as a simple
# array with the string "N,type" as each index. Therefore, the index "N, name"
# (with a space between both "indexes") will NOT work!
#
# The module that calls beditmaster must define this variables correctly
# BEFORE creating the widget (That is, before calling beditmaster).
#
#/////////////////////////////////////////////////////////////////////////////
# Do we need a fifth state? It would be "Duplicating" state: dup.
# status: Defines the state of the form (sel,edi,vis,ins):
#         "sel" - Select
#         "edi" - Edit
#         "vis" - Visualization
#         "ins" - Insert -> Will be duplicate too.
#
#/////////////////////////////////////////////////////////////////////////////
# Variables internas de BEditMaster
#/////////////////////////////////////////////////////////////////////////////
#
# fieldInfo($index,field,name)      # nombre del campo              
# fieldInfo($index,field,type)      # tipo y tamaño, en formato lista
# fieldInfo($index,field,iskey)     # indica si es clave             
# fieldInfo($index,field,label)     # texto a mostrar 
#                                   #  (de no proporcinarse se iguala al name)              
# fieldInfo($index,field,notnull)   # indica si es obligatorio o no 
#                                   #  (si es clave, es obligatorio)
# fieldInfo($index,field,fkMRes)    # foreing key en la que mas restrictiva
#                                   #  del campo
# fieldInfo($index,field,edit)      # Indica si el campo es editable o no
# fieldInfo($index,field,ptr)       # path del objeto en el que se visualiza
# fieldInfo($index,field,links)     # lista de enlaces a otros formularios ediMaestro
#                                   # (se pasa el procedimiento Show)
# uno por enlace
# fieldInfo($index,field,$i,link)   # procedimiento a llamar al pulsar el boton
#                                   # del enlace
#
# informacion de los campos descriptivos sobre el campo
# fieldInfo($index,desc,table)      # tabla en la que se encuentra la descripcion
# fieldInfo($index,desc,join)       # nombre del campo de esta tabla al que hace
#                                   #  referencia, de no proporcionrse es igual a
#                                   #  $fieldInfo($index,field,name)   
# fieldInfo($index,desc,show)       # indica si el campo tiene descripcion
# fieldInfo($index,desc,lstfields)  # lista completa con los campos a mostrar en la selecion
# fieldInfo($index,desc,lstshow)    # lista de campos descripcion a mostrar
# fieldInfo($index,desc,lstselect)  # lista de campos a mostrar en una seleccion + 
#                                   #  fieldInfo($index,desc,lstshow)
#
# uno por campos a mostrar
# fieldInfo($index,desc,$i,name)    # nombre del campo
# fieldInfo($index,desc,$i,edit)    # indica si es editable
# fieldInfo($index,desc,$i,type)    # tipo y tamaño, en formato lista
# fieldInfo($index,desc,$i,notnull) # indica si es obligatorio o no
#                                   #  (si es clave, es obligatorio)
# fieldInfo($index,desc,$i,ptr)     # path del objeto en el que se visualiza
#                                   #  ?????????????? Hay que calcularle al 
#                                   #  crear el bcomboboxplus
#
#
# informacion de las claves externas, solo se proporciona de existir
# fieldFkInfo($indexFk,table)       # tabla a la que hace referencia
# fieldFkInfo($indexFk,indFie)      # lista de campos a los que hace referencia
# fieldFkInfo($indexFk,fields)      # nombre de los campos en la tabla referenciada
#
#
#/////////////////////////////////////////////////////////////////////////////
# Informacion proporcionada
#/////////////////////////////////////////////////////////////////////////////
#
# regFields($index,field,name)      # nombre del campo (*)
# regFields($index,field,type)      # tipo y tamaño, en formato lista (*)
# regFields($index,field,iskey)     # indica si es clave (*)
# regFields($index,field,label)     # texto a mostrar
# regFields($index,field,notnull)   # indica si es obligatorio o no 
#                                   #  (si es clave, es obligatorio)
# regFields($index,field,fkMRes)    # foreing key en la que mas restrictiva
#                                   #  del campo
# regFields($index,field,links)     # lista de enlaces a otros formularios ediMaestro
#                                   # (se pasa el procedimiento Show)
#
## informacion de los campos descripcion sobre el campo, solo se proporciona de existir
# regFields($index,desc,table)      # tabla en la que se encuentra la descripcion (*)
# regFields($index,desc,join)       # nombre del campo de esta tabla al que 
#                                   #  hace referencia
# regFields($index,desc,lstshow)    # lista de campos descripcion a mostrar 
# regFields($index,desc,lstselect)  # lista de campos a mostrar en la seleccion + 
#                                   #  regFields($index,desc,lstshow)
# regFields($index,desc,lstedit)    # lista que indica si los campos de
#                                   #   "lstshow" son editables o no
# regFields($index,desc,lsttype)    # si los campos son editables, 
#                                   # se especifica su tipo en formato lista de listas
# regFields($index,desc,lstnotnull) # si los campos son editables, 
#                                   # se especifica si son obligatorios o no en formato lista
#
# informacion de las claves externas, solo se proporciona de existir
# regFk($indexFk,table)             # tabla a la que hace referencia (*)
# regFk($indexFk,indFie)            # lista de campos a los que hace referencia (*)
# regFk($indexFk,fields)            # nombre de los campos en la tabla referenciada (*)

#/////////////////////////////////////////////////////////////////////////////
::snit::widget beditmaster {
#
# PURPOSE: Definición de las funcionalidades de un BayesEditorMaster
#
#/////////////////////////////////////////////////////////////////////////////
#/////////////////////////////////////////////////////////////////////////////
# options:
# -table       -> Primary table name.
# -parent      -> Parent widget. Used to exit gently.
# -bodbcmodule -> Object of type bodbcmodule to obtain connection and
# -connection  -> Name of connection to Data Base in TCL
# -prociscode  -> Function get 'code' of table.
#                 Parameters: -table  -> name of table
#                             -fields -> fields of table
#                 Return: index of fields that they fulfill the condition
# -procisdesc  -> Function get 'description' of table.
#                 Parameters: -table  -> name of table
#                             -fields -> fields of table
#                 Return: index of fields that they fulfill the condition
#
#/////////////////////////////////////////////////////////////////////////////
  widgetclass BEditMaster

  option -table       ""
  option -parent      ""
  option -bodbcmodule ""
  #option -connection  ""
  #In the future, it will be possible to just give this information:
  #(But that requires mysql, MSSQL, Oracle, postgree info query models)
  option -regfields
  option -regfk
  option -maxfield
  option -autoload false
  option -prociscode {}
  option -procisdesc {}

  variable tablename
  variable bodbcmodule
  # Conexion tclodbc (se obtiene de bodbcmodule)
  variable connection  
  # Gestor de BBDD: Ora (Oracle), Mic (SQLServer), Mys (MySQL)
  #  (se obtiene de bodbcmodule)
  variable gestor      
  variable maxFields
  variable fieldInfo
  variable fieldFkInfo
  variable status 
  variable widget
  variable oldValues
  variable prociscode
  variable procisdesc  
  #Don't use it, but will in the future:
  variable oldStatus
 
  variable data
  variable step 0
  
  #///////////////////////////////////////////////////////////////////////////
  typeconstructor {
  #
  # PURPOSE: beditmaster typeconstructor
  #
  #///////////////////////////////////////////////////////////////////////////
    global tcl_platform
    global toltk_options
    if { [string equal $tcl_platform(platform) "windows"] } {
      set toltk_options(colors,SystemButtonFace) SystemButtonFace
    } else {
      set toltk_options(colors,SystemButtonFace) #d4d4d4
    }
  }


  #///////////////////////////////////////////////////////////////////////////
  constructor {args} {
  #
  # PURPOSE: beditmaster constructor
  #
  # PARAMETERS:
  #   args ->
  #
  #///////////////////////////////////////////////////////////////////////////
#puts "BEditMaster: args : $args"
    # CONSTANTES
    # Tomamos como campo codigo aquel que tenga el nombre:
    #   'code' o 'codigo'
    set data(codes) {code codigo}
    # Tomamos como campo descripcion aquel que tenga el nombre:
    #   'description', 'descripcion', 'descricao'
    set data(descritions) {description descripcion descricao name nombre}   
  
    # Apply all options passed at creation time.
    $self configurelist $args

    # cargamos valores
    set tablename     $options(-table)
    set parent        $options(-parent)
    set autoload      $options(-autoload)
    #set connection    $options(-connection)
    set bodbcmodule   $options(-bodbcmodule)
    set connection    [$bodbcmodule cget -connection]
    set gestor        [$bodbcmodule cget -gestor]
    set prociscode    $options(-prociscode)
    set procisdesc    $options(-procisdesc)
    Tolcon_Trace "BODBCMODULE = $bodbcmodule, CONECTION = $connection, GESTOR = $gestor"
    # autoload ?
    if { $autoload } {
      set lstInfo [$self InitTable]
      if {[llength $lstInfo]} {
#puts "AutoLoad: $lstInfo"
        array set fieldInfo   [lindex $lstInfo 0]
        array set fieldFkInfo [lindex $lstInfo 1]
        set maxFields         [lindex $lstInfo 2]
        $self configure -maxfield $maxFields
        $self configure -regfields [lindex $lstInfo 0]
        $self configure -regfk     [lindex $lstInfo 1]
      } else {
        tk_messageBox -icon error \
          -message [mc "An error has taken place during the creation of the form, reason why this it could not be visualized"] \
          -parent $parent \
          -title [mc "Edit Master"] \
          -type ok
        return 0
      }
    } else {
      array set fieldInfo    $options(-regfields)
      array set fieldFkInfo  $options(-regfk)
      set maxFields          $options(-maxfield)
    }


    ScrolledWindow $self.sw -relief sunken -borderwidth 2
    ScrollableFrame $self.sw.fr -constrainedwidth 1
    $self.sw setwidget $self.sw.fr
    set widget(frc) [$self.sw.fr getframe]
  
    # Configuracion del formulario:
    # 1.- datos del registro
    set widget(tf,pk)    [TitleFrame $widget(frc).pk -text [mc "Key Fields"]]
    set widget(tf,pk,fr) [$widget(frc).pk getframe]
#  -highlightthickness 2
    # 2.- rejilla de datos
    set widget(tf,data)    [TitleFrame $widget(frc).data -text [mc "Non Key Fields"]]
    set widget(tf,data,fr) [$widget(frc).data getframe]
    # 3.- extra grid
    set widget(fr,extra) [frame $widget(frc).extra]
    
    #Now we create the form:
    $self createForm $widget(tf,pk,fr)  $widget(tf,data,fr)

    # 3.- barra de botones
    set widget(bbox) \
      [ButtonBox $self.bb -orient horizontal -spacing 0 -padx 0 -pady 0 \
                 -homogeneous false]
    
    set widget(bbox,inf) \
      [ButtonBox $self.bbinf -orient horizontal -spacing 0 -padx 0 -pady 0 \
                 -homogeneous false]

    set widget(bt,duplicate) \
      [$widget(bbox) add -text [mc "Duplicate"] -relief link \
         -state disable -command [list $self botdup] \
         -helptext [mc "Duplicate"] -disabledforeground ""\
         -image [::Bitmap::get duplicate] -compound left -padx 1 -pady 1]
    set widget(bt,new) \
      [$widget(bbox) add -text [mc "New"] -relief link \
         -command [list $self botnew] \
         -helptext [mc "New"] -disabledforeground ""\
         -image [Bitmap::get MODFile] -compound left -padx 1 -pady 1]
    set widget(bt,edit) \
      [$widget(bbox) add -text [mc "Edit"] -relief link \
         -state disable -command [list $self botedi] \
         -helptext [mc "Edit current"] -disabledforeground ""\
         -image [Bitmap::get edit] -compound left -padx 1 -pady 1]
    set widget(bt,save) \
      [$widget(bbox) add -text [mc "Save"] -relief link \
         -state disable -command [list $self botgua] \
         -helptext [mc "Save current"] -disabledforeground ""\
         -image [Bitmap::get bsave] -compound left -padx 1 -pady 1]
    set widget(bt,cancel) \
      [$widget(bbox) add -text [mc "Cancel"] -relief link \
         -state disable -command [list $self botcan] \
         -helptext [mc "Cancel operation"] -disabledforeground ""\
         -image [Bitmap::get cancelx] -compound left -padx 1 -pady 1]
    set widget(bt,delete) \
      [$widget(bbox,inf) add -text [mc "Delete"] -relief link \
         -state disable -command [list $self boteli] \
         -helptext [mc "Delete current"] -disabledforeground ""\
         -image [Bitmap::get delete] -compound left -padx 1 -pady 1]
    set widget(bt,clear) \
      [$widget(bbox,inf) add -text [mc "Clear"] -relief link \
          -command [list $self botlim ] \
          -helptext [mc "Clear form"] -disabledforeground ""\
          -image [Bitmap::get Clear] -compound left -padx 1 -pady 1]
    set widget(bt,refresh) \
      [$widget(bbox,inf) add -text [mc  "Refresh"] -relief link \
         -command [list $self botref ] \
         -helptext [mc "Refresh form"] -disabledforeground ""\
         -image [Bitmap::get refresh] -compound left -padx 1 -pady 1]
    set widget(bt,open) \
      [$widget(bbox,inf) add -text [mc "Open"] -relief link \
         -command [list $self botabr] -state disabled \
         -helptext [mc "Open table"] -disabledforeground ""\
         -image [Bitmap::get opentable] -compound left -padx 1 -pady 1]
    set widget(bt,exit) \
      [$widget(bbox,inf) add -text [mc "Exit"] -relief link \
         -command [list $self botsal $parent] \
         -helptext [mc "Close window"] -disabledforeground ""\
         -image [Bitmap::get exit] -compound left -padx 1 -pady 1]

    # Empacar
    pack $widget(bbox,inf)  -side bottom -fill x -expand no 
    pack $widget(bbox)      -side bottom -fill x -expand no
    
    pack $self.sw       -side top    -fill both -expand yes
      
    #set status "sel"
    set status "sel"
    $self setStatus "sel"
    # Fill out the form with all database values:
#    $self fillForm
    # Show the form to the user:
    $self showForm
  }

  #/////////////////////////////////////////////////////////////////////////////
  onconfigure -regfields {value} {
  #/////////////////////////////////////////////////////////////////////////////
    array set fieldInfo $value
    for {set x 1} {[expr $x <= [$self cget -maxfield]]} {incr x} {
      # comprobamos label 
      if {![info exists fieldInfo($x,field,label)]} {
        set fieldInfo($x,field,label) $fieldInfo($x,field,name)
      }
      # comprobamos los botones de edicion especificados
      if {[info exists fieldInfo($x,field,links)]} {
        for {set idx 0} {$idx < [llength $fieldInfo($x,field,links)]} {incr idx} {
          set fieldInfo($x,link,[expr $idx+1],proc) [lindex $fieldInfo($x,field,links) $idx]
        }
      }
      # comprobamos si el codigo es editable
      if {![info exists fieldInfo($x,field,edit)]} {
        if {[info exists fieldInfo($x,field,fkMRes)]} {
          set fieldInfo($x,field,edit) false
        } else {
          set fieldInfo($x,field,edit) true
        }
      }
      # comprobamos que exista descripcion
      if {[info exists fieldInfo($x,desc,table)]} {
        set lstfields {}
        # comprobamos join
        if {![info exists fieldInfo($x,desc,join)]} {
          set fieldInfo($x,desc,join) $fieldInfo($x,field,name)
        }
        # lista de campos a mostar al seleccionar, primero es el codigo
        lappend lstfields $fieldInfo($x,desc,join)
        # indicamos que tiene descripcion
        set fieldInfo($x,desc,show) true
        # guardamos informacion sobre los campos descripcion a mostrar
        # nombres
        set i 1
#        if {[info exists fieldInfo($x,desc,lstshow)]} 
        if {[info exists fieldInfo($x,desc,lstshow)] && \
            [llength $fieldInfo($x,desc,lstshow)]} {
          foreach n $fieldInfo($x,desc,lstshow) {
            lappend lstfields $n
            set fieldInfo($x,desc,$i,name) $n
            # por defecto edit -> false
            set fieldInfo($x,desc,$i,edit) false
            incr i
          } 
        } else {
          set fieldInfo($x,desc,lstshow) {}
          # comrpbamos que tenga valor 'true', si es asi lo dejamos con su valor          
          if {[info exists fieldInfo($x,desc,show)] && \
              [string length $fieldInfo($x,desc,show)] && \
              !($fieldInfo($x,desc,show))} {
            set fieldInfo($x,desc,show) false
          }
        }
        # edit
        set i 1
        if {[info exists fieldInfo($x,desc,lstedit)]} {
          foreach e $fieldInfo($x,desc,lstedit) {
            set fieldInfo($x,desc,$i,edit) $e
            incr i
          } 
        }        
        # tipo
        if {[info exists fieldInfo($x,desc,lsttype)]} {
          set i 1
          foreach t $fieldInfo($x,desc,lsttype) {
            set fieldInfo($x,desc,$i,type) $t
            incr i
          }
        }
        # notnull   
        if {[info exists fieldInfo($x,desc,lstnotnull)]} {
          set i 1
          foreach nn $fieldInfo($x,desc,lstnotnull) {
            set fieldInfo($x,desc,$i,notnull) $nn
            incr i
          }
        }
        # lista de campos para la selecciona
        if {[info exists fieldInfo($x,desc,lstselect)]} {
          foreach n $fieldInfo($x,desc,lstselect) {
            lappend lstfields $n
          }    
        }
        set fieldInfo($x,desc,lstfields) $lstfields
      } else  {
        set fieldInfo($x,desc,show) false
      }
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method createForm {fpk fdata} {
  #
  # PURPOSE: Create all widgets needed for the form.
  #  The widgets are created acording to the information given in fieldInfo:
  #  fieldInfo(X,iskey) -> BComboBox
  #  (!fieldInfo(X,iskey) AND fieldInfo(X,MasterTable)) -> BComboBox
  # TODO:
  #  fieldInfo(X,typefield) = datetime ->
  #::Objects::ChooseDateCreate ::TolConsole::tmpOpt(var,DefLast) $frame.cLD
  #  else -> Entry
  #
  # PARAMETERS:
  #  fpk : path del frame de las primary key
  #  fdata : path del frame de los valores
  # RETURN:
  #
  #///////////////////////////////////////////////////////////////////////////
    for {set x 1} {$x<=$maxFields} {incr x} {
      if {$fieldInfo($x,field,iskey)} {
        # campos claves
        label $fpk.label$x -text $fieldInfo($x,field,label) -fg blue
        set fieldInfo($x,field,ptr) $fpk.cb$x
        set widget(ptr,[string tolower $fieldInfo($x,field,name)]) $fieldInfo($x,field,ptr)
        $self CreateSelectFields $x
      } else {
        # campos NO claves
        if {$fieldInfo($x,field,notnull)} {
          set clr blue
        } else  {
          set clr black
        }
        label $fdata.label$x -text $fieldInfo($x,field,label) -fg $clr
        if {$fieldInfo($x,desc,show)} {
          set fieldInfo($x,field,ptr) $fdata.cb$x
          set widget(ptr,[string tolower $fieldInfo($x,field,name)]) \
            $fieldInfo($x,field,ptr)          
          $self CreateSelectFields $x
        } else {
          set fieldInfo($x,field,ptr) [$self CreateWidget $fdata $x]
          set widget(ptr,[string tolower $fieldInfo($x,field,name)]) \
            $fieldInfo($x,field,ptr)                    
        }
      }
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method CreateSelectFields {x} {
  # PURPOSE:
  # PARAMETERS:
  # RETURN:
  #///////////////////////////////////////////////////////////////////////////
#Tolcon_Trace "SELFNS=$selfns"
    lappend lst_help "$fieldInfo($x,field,name) ($fieldInfo($x,field,type))"
#Tolcon_Trace "CreateSelectFields $x"
    for {set i 0} {$i < [llength $fieldInfo($x,desc,lstshow)]} {incr i} {
      set nam [lindex $fieldInfo($x,desc,lstshow) $i]
      if {[info exists fieldInfo($x,desc,lsttype)]} {
        set typ " ([lindex $fieldInfo($x,desc,lsttype) $i])"
      } else  {
        set typ ""
      }
      lappend lst_help "$nam $typ"
    }
    
#Tolcon_Trace ""
#Tolcon_Trace "lst_help=$lst_help"
    if {([string length $fieldInfo($x,desc,lstshow)] eq 0)} {
      set fields [list $fieldInfo($x,field,name)]  
    } else {
      set fields [list $fieldInfo($x,field,name) $fieldInfo($x,desc,lstshow)]
    }
    update
    set combo \
      "bcomboboxplus   $fieldInfo($x,field,ptr) \
      -cols        \"$fieldInfo($x,field,name) $fieldInfo($x,desc,lstshow)\" \
      -colslb      \"$fieldInfo($x,desc,lstfields)\" \
      -titles      \"$fieldInfo($x,desc,lstfields)\" \
      -postcommand [list [list $self GetValuesShow $x]] \
      -modifycmd   [list [list $self botsel $x]] \
      -helptexts   [list $lst_help] \
      -showlabels  0 \
      -widthlb     600 \
      -font        {Arial 9} \
      -nrows       15 \
      -keypress    true \
      -colkeypress $fieldInfo($x,field,name)"
    switch [string tolower [lindex $fieldInfo($x,field,type) 0]] {
      # datetime y smalldatetime
      "date" -
      "datetime"  -
      "smalldatetime" {
        set combo "$combo  -keydate     1"
      }
    }
    eval $combo
    # guardamos los nombres de los objetos de las descripciones
    if {$fieldInfo($x,desc,show)} {
      set i 1          
      set allObj [$fieldInfo($x,field,ptr) getwidget]
      if {$fieldInfo($x,field,edit)} {
#Tolcon_Trace "CreateSelectFields: configurando [lindex $allObj 0]"
        [lindex $allObj 0] configure -validate key -vcmd \
          [list SqlValidateData %P $fieldInfo($x,field,type)]
      }  
      set objs [lrange $allObj 1 end]
      foreach o $objs {
        set fieldInfo($x,desc,$i,ptr) $o
        # comprobamos si es editable, de serlo le activamos el control del
        # texto introducido
        if {[info exists fieldInfo($x,desc,$i,edit)] && \
            [info exists fieldInfo($x,desc,$i,type)]} { 
          if {$fieldInfo($x,desc,$i,edit)} {
            $o configure -validate key -vcmd \
              [list SqlValidateData %P $fieldInfo($x,desc,$i,type)]
          }    
        }
        incr i
      }
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method CreateWidget {path index} {
  # PURPOSE:
  # PARAMETERS:
  # RETURN:
  #/////////////////////////////////////////////////////////////////////////// 
    global toltk_options
    #TODO: controlar el tipo de objeto que hay que empacar
    set typeField [string tolower [lindex $fieldInfo($index,field,type) 0]]
    set ptr $path.entry$index
    #set tbl $tablename
    set lab $fieldInfo($index,field,name)
    set typ $fieldInfo($index,field,type)

    switch $typeField {
      # datetime y smalldatetime
      "date" -
      "datetime"  -
      "smalldatetime" {
        bentrydate $ptr
      }
      "bit" {
        checkbutton $ptr -variable [string map {. ""} ::${ptr}var]
      }
      default {
        set longVar [lindex $fieldInfo($index,field,type) 1]
        set fc [string first , $longVar]
        if {[expr $fc ne -1]} {
         set longVar [string range $longVar 0 [expr $fc-1]] 
        }
        if { ![string length $longVar] || [expr $longVar <= 255] } {
          Entry $ptr -editable false \
            -helptext "$lab ($typ)" -validate key \
            -background $toltk_options(colors,SystemButtonFace) \
            -vcmd [list SqlValidateData %P $fieldInfo($index,field,type)]
        } else  {
          text $ptr -yscrollcommand "${ptr}sb set" -height 4 \
                    -background $toltk_options(colors,SystemButtonFace)
          scrollbar ${ptr}sb -command "${ptr} yview"
        }
      } 
    }
    return $ptr
  }


  #///////////////////////////////////////////////////////////////////////////
  method botsel {args} {
  #
  # PURPOSE: Method to call when selecting a new value in any widget.
  #
  # PARAMETERS:
  #   current_index -> Index to the RegComboBox "matrix".
  #
  #///////////////////////////////////////////////////////////////////////////
    set current_index [lindex $args 0]
    # si estamos en visualizacion
    if {$status=="sel" || $status=="vis"} {
      if {[$self FillFieldKey]} {
        # construimos sentencia
        set select "select [join [$self GetLstFieldsTable] ,]"
        set from   "from $tablename"
        set where  "where [$self GetSqlWhere {}]"
        set txtSql "$select \n $from \n $where"
#obtener los valores de statement, estan en info
#        set values [$connection $txtSql]
#        $connection statement info $txtSql
        # mostramos valores en el formulario
        $self SetValuesFields $txtSql
        # mostramos descripciones de los campos
        $self SetValuesFieldsDesc 
        # cambiamos de estado  
        $self setStatus "vis"
      } 
    } elseif {$status=="ins" || $status=="edi"} {
#TODO: comprobar si en este estado hay que hacer algo
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method GetValuesShow {index} {
  # PURPUSE: funcion a previa a ejecutar al mostrar la lista de valores de un 
  #          bcomboboxplus
  # PARAMETERS: index -> registro activo
  #
  #///////////////////////////////////////////////////////////////////////////  
    # para visualizacion de datos de la propia tabla
    if {$status=="sel" || $status=="vis"} {
      set where ""
      set whe [$self GetSqlWhere ori. $index]
      if {[string length $whe]} {
        set where "where $whe \n"
      }
      if {[string equal $tablename $fieldInfo($index,desc,table)]} {
#Tolcon_Trace "GetValuesShow (1)"
        # informacion de la misma tabla
        set txtSql \
          "select distinct [join $fieldInfo($index,desc,lstfields) ,] \n\
           from   $tablename ori \n\
           $where \
           order by [lindex $fieldInfo($index,desc,lstfields) 0]"
      } else  {
        # informacion de la tabla referenciada
        foreach f $fieldInfo($index,desc,lstfields) {
          lappend fie "des.$f"
        }
        # unimos las tablas pro sus foreing key y campos descripcion
        set wheJoin [$self GetJoin $index \
                                   $fieldInfo($index,desc,table) \
                                   ori. \
                                   des.]

        if {[string length $wheJoin]} {
          set where "where $wheJoin \n"
        }

        if {[string length $whe]} {
          if {[string length $where]} {
            #concat $where "   and $whe \n"
            set where "$where \n and $whe \n"
          } else {
            set where "where $whe \n"
          }
        }
#Tolcon_Trace "GetValuesShow (2)"        
        set txtSql \
         "select distinct [join $fie ,] \n\
          from $fieldInfo($index,desc,table) des, $tablename ori \n\
          $where \
          order by des.[lindex $fieldInfo($index,desc,lstfields) 0]"
      }
    } elseif {$status=="edi" || $status=="ins"} {
      # valores de las tablas realcionadas
#Tolcon_Trace "GetValuesShow (3)"      
      set txtSql [$self GetSelectFk $index]
    }
    # asignamos valor al bselectfield correspondiente
#Tolcon_Trace "GetValuesShow: Consulta:\n $txtSql"
    set lst [DBQuery $bodbcmodule $txtSql]
    $fieldInfo($index,field,ptr) configure -values $lst
  }
 
  #///////////////////////////////////////////////////////////////////////////
  method GetJoin {index tableDes prefOri prefDes} {
  #
  # PURPOSE:
  #
  # PARAMETERS:
  #
  # RETURN:
  #
  #///////////////////////////////////////////////////////////////////////////   
    set wheJoin {}
    set ok      0
    if {[info exists fieldInfo($index,field,fkMRes)]} {
      set fkMRes $fieldInfo($index,field,fkMRes)
      if {[string equal $tableDes $fieldFkInfo($fkMRes,table)]} {
        set i 0
        foreach id $fieldFkInfo($fkMRes,indFie) {
          lappend wheJoin "$prefOri$fieldInfo($id,field,name) = \
                           $prefDes[lindex $fieldFkInfo($fkMRes,fields) $i]"
          incr i
        }
      } else {
        set ok 1
      }
    } else {
      set ok 1 
    }  
    if {$ok} {
      lappend wheJoin "$prefOri$fieldInfo($index,field,name) = \
                     $prefDes$fieldInfo($index,desc,join)" 
    }
    return [join $wheJoin { and }]
  }


  #///////////////////////////////////////////////////////////////////////////
  method setStatus {new} {
  #
  # PURPOSE: Change button state by the status value
  #
  # PARAMETERS: new -> New state.
  #
  # COMMENTS: A bit confusing to debug. Now it uses the new function
  #           ActivateWidget, and is much more readable.
  #
  #///////////////////////////////////////////////////////////////////////////
    #Set widget status:
    switch $status {
      "sel" {
        switch $new {
          "sel" {
            #Gets this on every launch:
            #Tolcon_Trace "SetStatus: Change from select to select status."
            # solo se pueden seleccionar los campos con lista de la primary key
            for {set x 1} {$x<=$maxFields} {incr x} {
              $self ActivateWidget $x $new
            }
          }
          "edi" {
            Tolcon_Trace "SetStatus: Change from select to edit status."
          }
          "vis" {
            #Only change the buttons state.
          }
          "ins" {
            #(Pushed new or duplicate button)
            #set editable keys combos
            #enable entries
            for {set x 1} {$x<=$maxFields} {incr x} {
              $self ActivateWidget $x $new
            }
          }
          "old" {
          }
        }
      }
      "vis" {
        switch $new {
          "sel" {
          }
          "vis" {
          }
          "edi" {
            #disable keys combos
            #enable entries
            for {set x 1} {$x<=$maxFields} {incr x} {
                $self ActivateWidget $x $new
            }
          }
          "ins" {
            #set editable keys combos
            #enable entries
            for {set x 1} {$x<=$maxFields} {incr x} {
                $self ActivateWidget $x $new
            }
          }
          "old" {
          }
        }
      }
      "edi" {
        switch $new {
          "sel" {
            focus -force $self
            for {set x 1} {$x<=$maxFields} {incr x} {
               $self ActivateWidget $x $new
            }
          }
          "vis" {
            #enable keys combos
            #disable entries
            focus -force $self
            for {set x 1} {$x<=$maxFields} {incr x} {
               $self ActivateWidget $x $new
            }
          }
          "edi" {
          }
          "ins" {
          }
          "old" {
          }
        }
      }
      "ins" {
        switch $new {
          "sel" {
            focus -force $self
            for {set x 1} {$x<=$maxFields} {incr x} {
               $self ActivateWidget $x $new
            }
          }
          "vis" {
            focus -force $self
            #"select" keys combos
            #disable entries (set to readonly)
            for {set x 1} {$x<=$maxFields} {incr x} {
              $self ActivateWidget $x $new
            }
          }
          "edi" {
          }
          "ins" {
          }
          "old" {
          }
        }
      }
    }
    #Set status:
    if { $new == "old" } {
        set status $oldStatus
    } else {
      set oldStatus $status
      set status $new
    }
    #Tolcon_Trace "setStatus: NEW Status: $status"
    #Change buttons according to old and new status:
    #
    switch $status {
      "sel" {
        $widget(bt,duplicate) configure -state disable
        $widget(bt,new)       configure -state normal
        $widget(bt,edit)      configure -state disable
        $widget(bt,save)      configure -state disable
        $widget(bt,cancel)    configure -state normal
        $widget(bt,cancel)    configure -state disable
        $widget(bt,delete)    configure -state disable
        $widget(bt,clear)     configure -state normal
        $widget(bt,refresh)   configure -state normal
        $widget(bt,open)      configure -state normal
      }
      "vis" {
        $widget(bt,duplicate) configure -state normal
        $widget(bt,new)       configure -state normal
        $widget(bt,edit)      configure -state normal
        $widget(bt,save)      configure -state disable
        $widget(bt,cancel)    configure -state normal
        $widget(bt,cancel)    configure -state disable
        $widget(bt,delete)    configure -state normal
        $widget(bt,clear)     configure -state normal
        $widget(bt,refresh)   configure -state normal
        $widget(bt,open)      configure -state normal
      }
      "edi" {
        $widget(bt,duplicate) configure -state normal
        $widget(bt,new)       configure -state disable
        $widget(bt,edit)      configure -state disable
        $widget(bt,save)      configure -state normal
        $widget(bt,cancel)    configure -state normal
        $widget(bt,delete)    configure -state disable
        $widget(bt,clear)     configure -state disable
        $widget(bt,refresh)   configure -state disable
        $widget(bt,open)      configure -state normal
      }
      "ins" {
        $widget(bt,duplicate) configure -state normal
        $widget(bt,new)       configure -state disable
        $widget(bt,edit)      configure -state normal
        $widget(bt,save)      configure -state normal
        $widget(bt,cancel)    configure -state normal
        $widget(bt,cancel)    configure -state normal
        $widget(bt,delete)    configure -state normal
        $widget(bt,clear)     configure -state normal
        $widget(bt,refresh)   configure -state normal
        $widget(bt,open)      configure -state normal
      }
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method showForm {} {
  #
  # PURPOSE: Show the form.
  #
  #///////////////////////////////////////////////////////////////////////////
    # grid of objects
    for {set x 1} {$x<=$maxFields} {incr x} {    
      switch [lindex $fieldInfo($x,field,type) 0] {
          "bit" {
            grid $fieldInfo($x,field,ptr)  -sticky nw -row $x -column 1 -pady 2
          }
          "text" - 
          "varchar" {
            grid $fieldInfo($x,field,ptr)  -sticky new -row $x -column 1 -pady 2
            if { [ expr [lindex $fieldInfo($x,field,type) 1] > 255] } {
              grid $fieldInfo($x,field,ptr)sb -sticky nse -row $x -column 2 -pady 2
            }
          }
           default {
             grid $fieldInfo($x,field,ptr)  -sticky new -row $x -column 1 -pady 2
           }        
     }     

      if {$fieldInfo($x,field,iskey)} {
      set path pk
      } else {
        set path data
      }
      grid $widget(tf,$path,fr).label$x -sticky nw -row $x -column 0 -pady 2
      # buttons for links to other editmasters
      if [info exists ${selfns}::fieldInfo($x,field,links)] {
        for {set idx 1} {$idx <= [llength $fieldInfo($x,field,links)]} {incr idx} {
          button $widget(tf,$path,fr).b${x}_${idx} \
            -command $fieldInfo($x,link,$idx,proc) \
            -image [::Bitmap::get "tools"]
          grid   $widget(tf,$path,fr).b${x}_${idx} -sticky nw\
             -row $x -column [expr 2+$idx] -pady 2
        }
      }
    }
    
    grid columnconfigure $widget(tf,pk,fr)    1 -weight 1
    grid rowconfigure    $widget(tf,pk,fr) 1000 -weight 1

    grid columnconfigure $widget(tf,data,fr)    1 -weight 1
    grid rowconfigure    $widget(tf,data,fr) 1000 -weight 1    

    pack $widget(tf,pk)    -fill x -expand yes
    pack $widget(tf,data)  -fill x -expand yes
    pack $widget(fr,extra) -fill x -expand yes

  }
  
  #///////////////////////////////////////////////////////////////////////////
  method getExtraFrame { } {
  #
  # PURPOSE: Return the path of an extra frame between the title frame of
  #          data and the button box
  #
  #
  #///////////////////////////////////////////////////////////////////////////
    return $widget(fr,extra)
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method botsal {parent} {
  #
  # PURPOSE: Manages the use of the "exit" button.
  #
  # PARAMETERS: parent -> (self's parent)
  #
  #///////////////////////////////////////////////////////////////////////////
    destroy $parent
  }

  #///////////////////////////////////////////////////////////////////////////
  method GetSqlWhere {prefTable {index -1}} {
  # PURPUSE: Construct the WHERE clause
  # PARAMETERS: index -> indice a excluir de la seleccion
  #             prefTable -> nombre de la tabla a concatenar al campo
  # RETURN:
  #
  #///////////////////////////////////////////////////////////////////////////
  #$fieldInfo($x,field,ptr)
    set where {}
    for {set x 1} {$x<=$maxFields} {incr x} { 
      if {$fieldInfo($x,field,iskey)} {
        
        if {([info exists fieldInfo($x,field,ptr)])} {
          if {[set code [$fieldInfo($x,field,ptr) key get]] != "" && $x != $index} {
            lappend where "$prefTable$fieldInfo($x,field,name) = \
               [SqlFormatField $code $fieldInfo($x,field,type) $gestor]\n"
          }  
        }
      }  
    }
    return [join $where { and }]
  }

  #///////////////////////////////////////////////////////////////////////////
  method GetSelectFk {index} {
  # PURPUSE:
  # PARAMETERS: index -> indice selecionado
  # RETURN: la sentencia sql a ejecutar para obtener los resultados a mostrar
  #
  #///////////////////////////////////////////////////////////////////////////
    set txtSql ""
    # control de foreing key existente
    if {[info exists fieldInfo($index,field,fkMRes)]} {
      set inxFk $fieldInfo($index,field,fkMRes)
      # control de foreing key a la tabla de la descripcion
      if {[string equal $fieldInfo($index,desc,table) $fieldFkInfo($inxFk,table)]} {
        set where [$self GetSqlWhereFk $index ""]
        if {[string length $where]} {
          set where "\n where $where"
        }
        set txtSql \
          "select distinct [join $fieldInfo($index,desc,lstfields) ,] \n\
           from $fieldFkInfo($inxFk,table) \
           $where \
           order by [lindex $fieldInfo($index,desc,lstfields) 0]"
      } else  {
        set where {}
        # informacion de la tabla referenciada
        foreach f $fieldInfo($index,desc,lstfields) {
          lappend fie "des.$f"
        }
        # claupsula where
        set join {}
        set lon [llength $fieldFkInfo($inxFk,indFie)]
        for {set x 0} {$x<$lon} {incr x} {
          if {[lindex $fieldFkInfo($inxFk,indFie) $x]==$index} {
            set join "des.$fieldInfo($index,desc,join) = \
                      ori.[lindex $fieldFkInfo($inxFk,fields) $x]"
          }
        }
        if {[string length $join]} {
          set where "\n where $join"
        }
        set whe {}
        set whe [$self GetSqlWhereFk des. $index]
        if {[string length $whe]} {
          if {[string length $where]} {
            set where "\n where $whe" 
          } else {
            set where "\n and $whe"
          }
        }
        # sentencia
        set txtSql \
          "select distinct [join $fie ,]\n\
           from $fieldInfo($index,desc,table) des, \
                $fieldFkInfo($inxFk,table) ori \
           $where \
           order by [lindex $fie 0]"
      }
    } else  {
      # no esta asociado a ninguna tabla, no se muestra lista
    }
    # result
    return $txtSql
  }

  #///////////////////////////////////////////////////////////////////////////
  method GetSqlWhereFk {index prefTable {opeLog and}} {
  # PURPUSE: Construye una sentenia where para la seleccion mediante la tabla
  #          relaccinada
  # PARAMETERS: index -> indice selecionado
  #             prefTable -> prefijo de la tabla, ha de incluir el punto
  #             opeLog -> operador logico entre las expresiones del where.
  #                       Por defecto "and"
  # RETURN: la sentencia where
  #
  #///////////////////////////////////////////////////////////////////////////
    set where {}
    if {[info exists fieldInfo($index,field,fkMRes)]} {
      set lstId $fieldFkInfo($fieldInfo($index,field,fkMRes),indFie)
      if {[llength $lstId]>1} {
        set i 0
        foreach id $lstId {
#Tolcon_Trace "GetSqlWhereFk: If $id <> $index"
          if {[expr $id != $index] && \
              [string length [set code [$fieldInfo($id,field,ptr) key get]]]} {
#Tolcon_Trace "GetSqlWhereFk: I: $i"
#Tolcon_Trace "Lista de campos referenciados:\
      $fieldFkInfo($fieldInfo($index,field,fkMRes),fields)"
            lappend where \
              "$prefTable\
               [lindex $fieldFkInfo($fieldInfo($index,field,fkMRes),fields) $i] = \
               [SqlFormatField $code $fieldInfo($id,field,type) $gestor] \n"
#Tolcon_Trace "GetSqlWhereFk: Campo en el where: [lindex $fieldFkInfo($fieldInfo($index,field,fkMRes),fields) $i]";#"
          }
          incr i
        }
      }
    }
    return [join $where " $opeLog "]
  }

  #///////////////////////////////////////////////////////////////////////////
  method FillFieldKey {} {
  # PURPUSE: comprobamos que esten rellenos todos los campos clave
  #
  # RETURN: true -> SI, false -> NO
  #
  #///////////////////////////////////////////////////////////////////////////
    set fill true
    set x    1
    while {$fill && [expr $x <=$maxFields]} {
      if {$fieldInfo($x,field,iskey)} {
#Tolcon_Trace "FillFieldKey $fieldInfo($x,field,ptr) = [$fieldInfo($x,field,ptr) key get]"
        if {[string equal [$fieldInfo($x,field,ptr) key get] ""]} {
          set fill false
#Tolcon_Trace "FillFieldKey: Por rellenar el campo: $fieldInfo($x,field,name)"
        }
      }
      incr x
    }
    return $fill
  }

  #///////////////////////////////////////////////////////////////////////////
  method FillFieldNotNull {} {
  # PURPUSE: comprobamos que esten rellenos todos los campos obligatorios
  #
  # RETURN: true -> SI, false -> NO
  #
  #///////////////////////////////////////////////////////////////////////////
#Tolcon_Trace "FillFieldNotNull ..."  
    set fill true
    set x    1
    while {$fill && [expr $x<=$maxFields]} {
      if {$fieldInfo($x,field,notnull)} {
        if {$fieldInfo($x,desc,show)} {
          for {set i 1} {$i<=[llength $fieldInfo($x,desc,lstshow)]} {incr i} {
            if {[info exists fieldInfo($x,desc,$i,notnull)] && \
                $fieldInfo($x,desc,$i,notnull)} {
              set txt  [BfoGetText $fieldInfo($x,desc,$i,ptr)]
              if {![string length $txt]} {
                set fill false
              }
            }
          }
        } else {
          set txt  [BfoGetText $fieldInfo($x,field,ptr)]
          if {![string length $txt]} {
            set fill false
          }
        } 
      }
      incr x
    }
    return $fill
  }

  #///////////////////////////////////////////////////////////////////////////
  method GetLstFieldsTable {} {
  # PURPUSE:
  #
  # RETURN:
  #
  #///////////////////////////////////////////////////////////////////////////
#Tolcon_Trace "GetLstFieldsTable ..."
    set lst {}
    for {set x 1} {$x<=$maxFields} {incr x} {
      lappend lst $fieldInfo($x,field,name)
      if {$fieldInfo($x,desc,show)} { 
        for {set i 1} {$i<=[llength $fieldInfo($x,desc,lstshow)]} {incr i} {
          if {$fieldInfo($x,desc,$i,edit)} {
            lappend lst $fieldInfo($x,desc,$i,name)
          }
        }
      }
    }
    return $lst
  }
 
  #///////////////////////////////////////////////////////////////////////////
  method SetValuesFields {txtSql} {
  # PURPUSE: Rellenamos los objetos del formulario con sus valores
  #
  # PARAMETERS: 
  #
  #///////////////////////////////////////////////////////////////////////////
  global tcl_platform
#Tolcon_Trace "SetValuesFields: Consulta: $txtSql"
    set values [DBQuery $bodbcmodule $txtSql]
    $connection statement infor $txtSql
    if { [string equal $::tcl_platform(platform) "unix"] } {
      infor
      $connection commit
    }
    #Loop for every field:
    set nameColums [infor columns]
    set c -1
    foreach nc $nameColums {
      incr c
      # obtenemos el indice de la columna
      set index [$self GetIndexArray field,name $nc]
      if {[expr $index != -1]} {
        if {!$fieldInfo($index,field,iskey)} {
          set val [lindex [lindex $values 0] $c]
          $self SetValue $fieldInfo($index,field,ptr) $val
        }
      }
    }
#    $self setStatus "vis"
  }

  #///////////////////////////////////////////////////////////////////////////
  method SetValue {ptr value} {
  # PURPUSE: Rellenamos los objetos del formulario con sus valores
  #
  # PARAMETERS: 
  #
  #///////////////////////////////////////////////////////////////////////////
#Tolcon_Trace "SetValue ptr=$ptr, value=$value"
    set type [winfo class $ptr]
#Tolcon_Trace "SetValue type=$type"
    switch $type {
      "BSelectFields" {
        $ptr SetValueView [list $value]
      }
      "Bcomboboxplus" {
        $ptr SetValueView [list $value]
      }
      "Entry" {
        set current [$ptr cget -state]
        $ptr configure -state normal
        $ptr configure -text $value
        $ptr configure -state $current
      }      
      "BEntryDate" {
        $ptr configure -btext $value
      }
      "LabelEntry" {
        set current [$ptr cget -state]
        $ptr configure -state normal
        $ptr configure -text $value
        $ptr configure -state $current
      }
      "Text" {
        set current [$ptr cget -state]
        $ptr configure -state normal
        $ptr delete 0.0 end
        $ptr insert 0.0 $value
        $ptr configure -state $current
      }
      "Checkbutton" {
        set current [$ptr cget -state]
        $ptr configure -state normal
        if { $value == 1} {
           $ptr select
        } else  {
           $ptr deselect
        }        
        $ptr configure -state $current
      }
      default {
        Tolcon_Trace "SetValue: Widget not recognized: $type"
      }
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method SetValuesFieldsDesc {} {
  # PURPUSE:
  #
  #///////////////////////////////////////////////////////////////////////////
#Tolcon_Trace "SetValuesFieldsDesc ..."
    for {set x 1} {$x<=$maxFields} {incr x} {
      # si no es clave y tiene descripcion
#Tolcon_Trace "SetValuesFieldsDesc Show: $fieldInfo($x,desc,show)"
      if {$fieldInfo($x,desc,show)} {
#Tolcon_Trace "SetValuesFieldsDesc  lstshow$x: $fieldInfo($x,desc,lstshow)"        
        if {[llength $fieldInfo($x,desc,lstshow)]} {                
#        if {!$fieldInfo($x,field,iskey) && $fieldInfo($x,desc,show)}
          set txt [BfoGetText $fieldInfo($x,field,ptr)]
#Tolcon_Trace "SetValuesFieldsDesc Table: $fieldInfo($x,desc,table)" 
#Tolcon_Trace "SetValuesFieldsDesc txt: $txt"                  
          if {[string length $txt]} {
            # construimos sentencia
            set txtSql \
              "select [join $fieldInfo($x,desc,lstshow) ,] \n\
               from $fieldInfo($x,desc,table) \n\
               where $fieldInfo($x,desc,join) = \
                 [SqlFormatField $txt $fieldInfo($x,field,type) $gestor]"
            # ejecutamos sentencia
#Tolcon_Trace "SetValuesFieldsDesc: Consulta:\n $txtSql"
              set values [DBQuery $bodbcmodule $txtSql]
            # rellenamos valores
            for {set i 1} {$i<=[llength $fieldInfo($x,desc,lstshow)]} {incr i} {
              $self SetValue $fieldInfo($x,desc,[expr $i +1],ptr) \
               [lindex [lindex $values 0] [expr $i-1]]
            }
          }
        }
      }
#Tolcon_Trace "SetValuesFieldsDesc. ---Acabo para el campo $x  ---"
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method GetIndexArray {key value} {
  # PURPUSE: Obtenemos la posicon dentro del array para una calve y un valor
  #
  # PARAMETERS: key
  #             value
  # RETURN: la posicion dentro del array, de no existir devuelve -1
  #
  #///////////////////////////////////////////////////////////////////////////
#Tolcon_Trace "GetIndexArray. key: $key  value: $value "  
    set index -1
    set x      1
    while {[expr $index == -1] && [expr $x <= $maxFields]} {
      if {[info exists fieldInfo($x,$key)]} {
        if {[string equal [string toupper $fieldInfo($x,$key)] [string toupper $value]]} {
          set index $x
        }
      }
      incr x
    }
#Tolcon_Trace "GetIndexArray. result: $index"    
    return $index
  }

  #///////////////////////////////////////////////////////////////////////////
  method boteli {} {
  #
  # PURPOSE: Ask for confirmation and delete form data from the database
  #
  #///////////////////////////////////////////////////////////////////////////
    #First, ask the user:
    global toltk_options
    # New dialog with two buttons: Acept and Cancel
    set rq [tk_messageBox -type yesno -icon question -title [mc "Delete"] \
            -parent $self -message [mc askDeleteField]]
    if {[string equal $rq yes]} {
      # borramos el registro
      #Look for every PK field and collect its value and field name:
      if {[$self FillFieldKey]} {
        set where [$self GetSqlWhere ""]
        set txtSql "delete from $tablename \n\
                    where $where"
#Tolcon_Trace "boteli: Sentencia de borrado: $txtSql"
        #TODO: comprobamos el valor de OK
        set ok [DBQuery $bodbcmodule $txtSql]
        #call cancel:
        $self botcan
      }
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method botedi {} {
  # PURPOSE:
  #///////////////////////////////////////////////////////////////////////////
    # Guardamos los valores previos a modificaciones
    $self SetOldValues
    $self setStatus "edi"
  }

  #///////////////////////////////////////////////////////////////////////////
  method botcan {} {
  # PURPOSE:
  #///////////////////////////////////////////////////////////////////////////
    #Simply clean up and return to "select" status.
    BfoCleanPath $widget(tf,pk)
    BfoCleanPath $widget(tf,data)
    # load old data
    $self SetDataFromOldValues
    $self SetValuesFieldsDesc
    # load old values
    $self setStatus "sel"
    $self botsel    
  }

  #///////////////////////////////////////////////////////////////////////////
  method botnew {} {
  #
  # PURPOSE: Actions to do when the "New" button is pressed.
  #
  #///////////////////////////////////////////////////////////////////////////
    $self SetOldValues
    #Clear the form:
    BfoCleanPath $widget(tf,pk)
    BfoCleanPath $widget(tf,data)
    #Change status:
    $self setStatus "ins"
  }

  #///////////////////////////////////////////////////////////////////////////
  method botgua {} {
  # PURPOSE:
  #///////////////////////////////////////////////////////////////////////////
    if {$status=="edi" || $status=="ins"} {
      set ok 1
      # comprobamos rellenos todos los campos key y notnull
      if {[$self FillFieldKey]} {
        if {![$self FillFieldNotNull]} {
          set ok 0
          tk_messageBox -type ok -icon warning -title [mc "Atention"] \
            -parent $self -message [mc notFillNotNull]
        }
      } else  {
        set ok 0  
        tk_messageBox -type ok -icon warning -title [mc "Atention"] \
          -parent $self -message [mc notFillKey]
      }
      if {$ok} {
        # construimos consultas de grabado
        switch $status {
          "ins" {
            set txtSql [$self GetSqlInsert]
          }
          "edi" {
            set txtSql [$self GetSqlUpdate]
          }
        }

        # ejecutamos sentencia
        set result 0
        if {[string length $txtSql]} {
#Tolcon_Trace "botgua: Sentencia de insercion o actualizacion:\n $txtSql"
          set result [DBQuery $bodbcmodule $txtSql]
        }
        # check result ?
        if {$result>=0} {
          $self setStatus "vis"
        } else  {
          #TODO: mensaje de error
        }
      }
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method GetInsertData {} {
  # PURPOSE: Obtain the list of values that would be inserted or updated in the
  #        table.
  #
  # RETURN: Sentencia de insercion 
  #///////////////////////////////////////////////////////////////////////////
    # NOTA: Da los datos en el formato adecuado a SQL
    set lstField {}
    set lstValues {}
    #Read each combo and text value, and add it to the query:
    for {set x 1} {$x<=$maxFields} {incr x} {
      lappend lstField $fieldInfo($x,field,name)
      set txt [BfoGetText $fieldInfo($x,field,ptr)]
      if {[string length $txt]} {
        set txt [StringReplace $txt ' ''] 
      } else {
        set txt null
      }
      lappend lstValues [SqlFormatField $txt $fieldInfo($x,field,type) $gestor]
      if {$fieldInfo($x,desc,show)} {
        for {set i 1} {$i<=[llength $fieldInfo($x,desc,lstshow)]} {incr i} {
          if {$fieldInfo($x,desc,$i,edit)} {
            lappend lstField $fieldInfo($x,desc,$i,name)
            set obj $fieldInfo($x,desc,$i,ptr)
            set txt [BfoGetText $obj]
            if {![string length $txt]} {
              set txt null
            }
            lappend lstValues [SqlFormatField $txt $fieldInfo($x,desc,$i,type) $gestor]
          }  
        }
      }
    }
    set regFields(lstValues) $lstValues
    return $lstValues
  }


  #///////////////////////////////////////////////////////////////////////////
  method GetSqlInsert {} {
  # PURPOSE: construimos la sentencia de insercion
  # RETURN: Sentencia de insercion 
  #///////////////////////////////////////////////////////////////////////////
    set lstField {}
    set lstValues {}
    #Read each combo and text value, and add it to the query:
    for {set x 1} {$x<=$maxFields} {incr x} {
      lappend lstField $fieldInfo($x,field,name)
      set txt [BfoGetText $fieldInfo($x,field,ptr)]
      if {[string length $txt]} {
        set txt [StringReplace $txt ' ''] 
      } else {
        set txt null
      }
      lappend lstValues [SqlFormatField $txt $fieldInfo($x,field,type) $gestor]
      if {$fieldInfo($x,desc,show)} {
        for {set i 1} {$i<=[llength $fieldInfo($x,desc,lstshow)]} {incr i} {
          if {$fieldInfo($x,desc,$i,edit)} {
            lappend lstField $fieldInfo($x,desc,$i,name)
            set obj $fieldInfo($x,desc,$i,ptr)
            set txt [BfoGetText $obj]
            if {![string length $txt]} {
              set txt null
            }
            lappend lstValues [SqlFormatField $txt $fieldInfo($x,desc,$i,type) $gestor]
          }  
        }
      }
    }
    # construir sentencia sql
    set txtSql \
      "insert into $tablename \n\
       ([join $lstField ,]) \n\
       values \n\
       ([join $lstValues ,])"
    return $txtSql
  }

  #///////////////////////////////////////////////////////////////////////////
  method GetSqlUpdate_ {} {
  # PURPOSE: Construimos la sentencia de actualizacion del registro activo
  # RETURN: Sentencia de actualizacion
  #///////////////////////////////////////////////////////////////////////////
    set lstValues {}
    update
    # sentencia where
    set where {}
    for {set x 1} {$x<=$oldValues(numReg)} {incr x} {
      lappend where "$oldValues($x,name) = \
        [SqlFormatField $oldValues($x,value) $oldValues($x,type) $gestor]"
    }
    set where [join $where { and }]

    #Read each combo and text value, and add it to the query:
    for {set x 1} {$x<=$maxFields} {incr x} {
      set txt [BfoGetText $fieldInfo($x,field,ptr)]
      if {[string length $txt]} {
        set txt [StringReplace $txt ' '']
      } else {
        set txt null
      }
      lappend lstValues "$fieldInfo($x,field,name) = \
        [SqlFormatField $txt $fieldInfo($x,field,type) $gestor]"
      if {$fieldInfo($x,desc,show)} {
        for {set i 1} {$i<=[llength $fieldInfo($x,desc,lstshow)]} {incr i} {
          if {$fieldInfo($x,desc,$i,edit)} {
            set obj $fieldInfo($x,desc,$i,ptr)
            set txt [BfoGetText $obj]
            if {![string length $txt]} {
              set txt null
            }
            lappend lstValues "$fieldInfo($x,desc,$i,name) = \
              [SqlFormatField $txt $fieldInfo($x,desc,$i,type) $gestor]"
          }  
        }
      }
    }
    # construir sentencia sql
    set txtSql \
      "update $tablename set \n\
       [join $lstValues ,] \n\
       where $where" 
    return $txtSql
  }


  #///////////////////////////////////////////////////////////////////////////
  method GetSqlUpdate {} {
  # PURPOSE: Construimos la sentencia de actualizacion del registro activo
  # RETURN: Sentencia de actualizacion
  #///////////////////////////////////////////////////////////////////////////
    set lstValues {}
    update
    # sentencia where
    set where {}
    for {set x 1} {$x<=$oldValues(numReg)} {incr x} {
      if {$oldValues($x,iskey)} {  
        lappend where "$oldValues($x,name) = \
          [SqlFormatField $oldValues($x,value) $oldValues($x,type) $gestor]"
      }
    }
    set where [join $where { and }]

    #Read each combo and text value, and add it to the query:
    for {set x 1} {$x<=$maxFields} {incr x} {
      set txt [BfoGetText $fieldInfo($x,field,ptr)]
      # modified ?
      if {![string eq $txt $oldValues($x,value)]} {
        if {[string length $txt]} {
          set txt [StringReplace $txt ' '']
        } else {
          set txt null
        }
        lappend lstValues "$fieldInfo($x,field,name) = \
          [SqlFormatField $txt $fieldInfo($x,field,type) $gestor]"
        if {$fieldInfo($x,desc,show)} {
          for {set i 1} {$i<=[llength $fieldInfo($x,desc,lstshow)]} {incr i} {
            if {$fieldInfo($x,desc,$i,edit)} {
              set obj $fieldInfo($x,desc,$i,ptr)
              set txt [BfoGetText $obj]
              if {![string length $txt]} {
                set txt null
              }
              lappend lstValues "$fieldInfo($x,desc,$i,name) = \
                [SqlFormatField $txt $fieldInfo($x,desc,$i,type) $gestor]"
            }  
          }
        }
      }
    }
    # construir sentencia sql
    if {[llength $lstValues]} {
      set txtSql \
        "update $tablename set \n\
         [join $lstValues ,] \n\
         where $where"
    } else {
      # no se va a modificar nada  
      set txtSql ""
    }
    return $txtSql
  }


  #///////////////////////////////////////////////////////////////////////////
  method SetOldValues_ {} {
  # PURPUSE: Almacenamos en una variable los valores actuales, antes de 
  #          modificarlos
  #
  #///////////////////////////////////////////////////////////////////////////
    set n 0
    for {set x 1} {$x<=$maxFields} {incr x} {
      if {$fieldInfo($x,field,iskey)} {
        incr n
        set oldValues($n,name)  $fieldInfo($x,field,name)
        set oldValues($n,type)  $fieldInfo($x,field,type)
        set oldValues($n,value) [BfoGetText $fieldInfo($x,field,ptr)]
      }
    }
    set oldValues(numReg) $n
  }


  #///////////////////////////////////////////////////////////////////////////
  method SetOldValues {} {
  # PURPUSE: Almacenamos en una variable los valores actuales, antes de 
  #          modificarlos
  #
  #///////////////////////////////////////////////////////////////////////////
    for {set x 1} {$x<=$maxFields} {incr x} {
      set oldValues($x,name)  $fieldInfo($x,field,name)
      set oldValues($x,type)  $fieldInfo($x,field,type)
      set oldValues($x,iskey) $fieldInfo($x,field,iskey)
      set oldValues($x,value) [BfoGetText $fieldInfo($x,field,ptr)]
    }
    set oldValues(numReg) $maxFields
  }


  #///////////////////////////////////////////////////////////////////////////
  method SetDataFomOldValues_ {} {
  #
  # PURPOSE: Carga los campos calve con los valores antigüos y ejecuta la
  #          función para rellenar el resto.
  #
  #///////////////////////////////////////////////////////////////////////////  
    #
    for {set i 1} {[expr $i <= $oldValues(numReg)]} {incr i} {
      $self SetValue [$self getpathfield $oldValues($i,name)] \
               $oldValues($i,value) 
    }
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method SetDataFromOldValues {} {
  #
  # PURPOSE: Carga los campos calve con los valores antigüos y ejecuta la
  #          función para rellenar el resto.
  #
  #///////////////////////////////////////////////////////////////////////////
    # exists old values ?
    if {[info exists oldValues(numReg)]} {
      for {set i 1} {[expr $i <= $oldValues(numReg)]} {incr i} {
        if {$oldValues($i,iskey)} {  
          $self SetValue \
            [$self getpathfield $oldValues($i,name)] $oldValues($i,value)
        }
      }
    }
  }  
  
  
  #///////////////////////////////////////////////////////////////////////////
  method ActivateWidget {index state} {
  #
  # PURPOSE: Activa o desactiva los widgets según el estado que se lea,
  #          en base a las directrices de los estados de BEditMaster.
  #
  # PARAMETERS:
  #   widget -> objeto a activar/desactivar
  #   state   -> Estado.
  #
  #///////////////////////////////////////////////////////////////////////////
    global toltk_options
    set w $fieldInfo($index,field,ptr)
    set type [winfo class $w]

    switch $type {
      "BSelectFields" {
        switch $state {
          "sel" -
          "vis" {
            if {$fieldInfo($index,field,iskey)} {
              set stt "select"
            } else {
              set stt "notactive"
            }
            $w Activate $stt
          }
          "edi" -
          "ins" {
#TODO: entiendo que es igual. Hay que decidir si en edicion se puede cambiar
#      el code de un campo (este es el codigo que se especifica en la tabla)
#      En estos momentos se pueden aztualizar todos los campos
            if {$fieldInfo($index,field,iskey)} {
              if {$fieldInfo($index,field,edit)} {
                set stt "normal"
              } else  {
                set stt "select"
              }
              $w Activate $stt
              if {$fieldInfo($index,desc,show)} {
                for {set i 1} {$i<=[llength $fieldInfo($index,desc,lstshow)]} {incr i} {
                  if {$fieldInfo($index,desc,$i,edit)} {
                    set stt "active"
                    BfoActivateWidget $fieldInfo($index,desc,$i,ptr) $stt
                  }
                }
              }
            } else {
              set stt "select"
              $w Activate $stt
            }
          }
        }
      }
      "Bcomboboxplus"
      {
        switch $state {
          "sel" -
          "vis" {
            if {$fieldInfo($index,field,iskey)} {
              set stt "select"
            } else {
              set stt "notactive"
            }
            $w activate $stt
          }
          "edi" -
          "ins" {
            if {$fieldInfo($index,field,iskey)} {
              if {$fieldInfo($index,field,edit)} {
                set stt "normal"
              } else  {
                set stt "select"
              }
              $w activate $stt
              if {$fieldInfo($index,desc,show)} {
                for {set i 1} {$i<=[llength $fieldInfo($index,desc,lstshow)]} {incr i} {
                  if {$fieldInfo($index,desc,$i,edit)} {
                    set stt "active"
                    BfoActivateWidget $fieldInfo($index,desc,$i,ptr) $stt
                  }
                }
              }
            } else {
              set stt "select"
              $w activate $stt
            }
          }
        }
      }
      "LabelEntry"
      {
        switch $state {
          "sel" -
          "vis" {
            set stt "false"
          }
          "edi" -
          "ins" {
            set stt "true"
          }
        }
        $w configure -editable $stt
      }
      "Mclistbox"
      {
        #TODO
      }
      "ComboBox"
      {
        #TODO
      }
      "Entry"
      {
        switch $state {
          "sel" -
          "vis" {
            set stt "false"
            $w configure -background $toltk_options(colors,SystemButtonFace)
          }
          "edi" -
          "ins" {
            set stt "true"
            $w configure -background white
          }
        }
        $w configure -editable $stt
      }
      "BEntryDate" {
        switch $state {
          "sel" -
          "vis" {
            set stt "disabled"
          }
          "edi" -
          "ins" {
            set stt "normal"
          }
        }
        $w configure -bstate $stt
      }
      "Listbox"
      {
        #TODO
      }
      "Checkbutton"
      {
        switch $state {
          "sel" -
          "vis" {
            set stt "disabled"
          }
          "edi" -
          "ins" {
            set stt "normal"
          }
        }
        $w configure -state $stt
      }
      "Text"
      {
        #Tolcon_Trace "prueba nueva ActivateWidget TEXT"
        switch $state {
          "sel" -
          "vis" {
            set stt "disabled"            
            $w configure -background $toltk_options(colors,SystemButtonFace) \
                         -foreground darkgray
          }
          "edi" -
          "ins" {
            set stt "normal"
            $w configure -background white -foreground black
          }
        }
        $w configure -state $stt
      }
      default
      {
        Tolcon_Trace "ActivateWidget: Widget type not recognized: $type"
      }
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method botdup {} {
  #
  # PURPOSE:
  #
  #///////////////////////////////////////////////////////////////////////////
    #duplicar
    #set status "edi"
    $self setStatus "ins"
  }

  #///////////////////////////////////////////////////////////////////////////
  method botlim {} {
  #
  # PURPOSE:
  #
  #///////////////////////////////////////////////////////////////////////////
    #Istead of having a status control, we force it by disabling New and Edit
    #buttons.
    #$componente.f3.butdup configure -state disable
    #$componente.f3.butedit configure -state disable
    BfoCleanPath $widget(tf,pk)
    BfoCleanPath $widget(tf,data)
    $self setStatus "sel"
  }

  #///////////////////////////////////////////////////////////////////////////
  method botref {} {
  #
  # PURPOSE:
  #
  #///////////////////////////////////////////////////////////////////////////
#TODO: decidir que hace esta opcion, de momento limpiamos todos los valores
#      introducidos   
    BfoCleanPath $widget(tf,pk)
    BfoCleanPath $widget(tf,data)

#   $self fillForm
    #When pushing the Refresh button, no real action is taken but
    #filling up all widgets with the data. Therefore, we don't change status.
    #$self setStatus "sel"
  }

  #///////////////////////////////////////////////////////////////////////////
  method botabr {} {
  #
  # PURPOSE: Shows table
  #
  #///////////////////////////////////////////////////////////////////////////
    set table [$self cget -table]
    set db [[$self cget -bodbcmodule] cget -connection]
    $db statement stmt "select * from $table"
    
    set tl [::project::CreateForm \
        -title    "[mc "SQL table"]: $table" \
        -iniconfig SQLTable \
        -type Tables -helpkey "Tb4Edt"
    ]
    set tlf [$tl getframe]
    # config
    btable $tlf.t -parent $tl \
                  -posteditcmd [list ::TolConsole::TableRecentUpdate $tlf.t] \
                  -iniconfig btable
    $tlf.t fillSQL stmt
    pack $tlf.t -fill both -expand yes
    $tl bind <Control-F4> [list $tl kill]
    update
  }

  #///////////////////////////////////////////////////////////////////////////
  method GetWidget { x } {
  #
  # PURPOSE:Gets widget path for field indexed by x
  #
  #///////////////////////////////////////////////////////////////////////////
    # comprobar q la entrada es un nombre y no un mumero -> GetLstFieldsTable
    if { [string is integer $x] } {
      return $fieldInfo($x,field,ptr)
    } else {
      set inx [lsearch -exact [$self GetLstFieldsTable] $x]
      if { $inx != -1 } {
        incr inx
        return $fieldInfo($inx,field,ptr)
      }
    }
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method getpathfield { field } {
  #
  # PURPOSE: Gets path for field
  # PARAMETERS: field: name of field
  # RETURN: path of widget
  #
  #///////////////////////////////////////////////////////////////////////////
    # find
    set infPtr [array get widget ptr,[string tolower $field]]
    return [lindex $infPtr 1]
  }

  #///////////////////////////////////////////////////////////////////////////
  method showvalue { args } {
  #
  # PURPOSE: Loads the form with the indicated values
  # PARAMETERS:
  #   args:
  #     -status: [sel | edi | vis | ins]
  #       sel  -> mode visualization
  #       vis  -> visualizes the proportionate information
  #       edi  -> mode edition, first clean, then edit
  #       ins  -> mode insertion
  #     -values: pair of values {field value}
  #
  #///////////////////////////////////////////////////////////////////////////
    # values default
    array set opts [list \
      -status   sel \
      -values   {}  \
    ]
    array set opts $args
    
    # status initial
    $self botedi
    $self botcan
    $self botref    
    set ok true
    switch $opts(-status) {
      sel -
      vis     {  }
      edi     { $self botedi }
      ins     { $self botnew }
      default { set ok false }  
    }
    # ok ?
    if { $ok } {
      # fill values
      foreach inf $opts(-values) {
        set path [$self getpathfield [lindex $inf 0]]
        if {[string length $path]} {
          $self SetValue $path [lindex $inf 1]
        }
      }
      # show desc
      $self SetValuesFieldsDesc
      
      # status end
      set ok true
      switch $opts(-status) {
        sel     { $self botsel }
        vis     { }
        edi     { }
        ins     { }
        default { }  
      }      
    }  
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method get { var} {
  #
  # PURPOSE: Obtain the value of a variable
  # PARAMETERS: var -> name of variable
  # RETURN: value of variable
  #
  #///////////////////////////////////////////////////////////////////////////
    return [subst $[subst "$var"]]
  }
  
  proc IsTextType {type} {
    regexp {^char|varchar|varchar2|text|nchar|ntext$} $type
  }

  #///////////////////////////////////////////////////////////////////////////
  method InitTable {} {
  #
  # PURPUSE: Rellena la estructura inicial para invocar un Editor Mestro de
  #          tablas a partir de una tabla dada
  #
  # PARAMETERS:
  #   table     : nombre de la tabla
  #   connection: nombre de la conexion a usar
  #
  # RETURN: Obtenemos una lista con estructura de array con la informacion
  #         necesaria para invocar un EditMaster.
  #         1 elemento -> informacion sobre los campos
  #         2 elemento -> informacion sobre las claves ajenas
  #         3 elemento -> numero de campos de la tabla
  #
  #///////////////////////////////////////////////////////////////////////////
    set table $tablename
    set tolGes [SqlGetTolGestor $gestor]
    
    # Obtenemos informacion de los campos
    set infoColumns [GetGlobalUniqueName __infoColumns[clock second]]
    set cmdIC infoColums[clock clicks]
    
    #set cmdTIC "Set $infoColumns = SqlInfoColumns(\"$table\", $tolGes);"
    set cmdTIC "Set $infoColumns = StdLib::SqlEngine::SqlInfoColumns(\"$table\", $tolGes);"

    ::tol::console eval $cmdTIC
    # check exists variable
    if {![TclInfoExists Set $infoColumns]} {
      # ERROR
      puts "Error al ejecutar: $cmdTIC"
      return {}   
    }    
    ::tol::tableset create $cmdIC $infoColumns
  
    # Obtenemos informacion de las foregn keys
    set infoForeignKeys [GetGlobalUniqueName __infoForeignKeys[clock second]]
    set cmdIF infoForeignKeys[clock clicks]
    #set cmdTIF "Set $infoForeignKeys = SqlInfoForeign(\"$table\", $tolGes);"
    set cmdTIF "Set $infoForeignKeys = StdLib::SqlEngine::SqlInfoForeign(\"$table\", $tolGes);"
    
    ::tol::console eval $cmdTIF
    # check exists variable
    if {![TclInfoExists Set $infoForeignKeys]} {
      # ERROR  
      error "Error al ejecutar: $cmdTIF"
      return {}    
    }    
    # comprobamos que tenemos valores
    set nRegFk [lindex [TclGetFromSet Set $infoForeignKeys] 0]
    if {[expr $nRegFk > 0]} {
      ::tol::tableset create $cmdIF $infoForeignKeys
    }  
#Tolcon_Trace "EditMasterUtils::InitTable. nRegFk: $nRegFk"

    # Rellenamos informacion de los campos
    set index 0
    set nRegC [$cmdIC info datarows]
    for {set fila 0} {[expr $fila < $nRegC]} {incr fila} {
      incr index
      set regFields($index,field,name) \
        [string trim [$cmdIC data get $fila 1] \"] ;#"
      set regFields($index,field,label) [mc "$regFields($index,field,name)"]
    
      set type   [string trim [$cmdIC data get $fila 2] \"];#"
    
      # Longitud. Si el campo "Precision" es igual a "?" nos quedamos con el
      # valor del campo "Length"
      set length [$cmdIC data get $fila 4]
      if {[IsTextType $type] || [string eq $length ?]} {
        set length [$cmdIC data get $fila 3]
      }
   
      set scale  [$cmdIC data get $fila 5]
    
      if {[IsTextType $type] || [string equal $scale ?] || ![string length $scale]} {
        set typeCol [list $type $length]
      } else {
        set typeCol [list $type $length,$scale]
      }
      set regFields($index,field,type)     $typeCol
      set regFields($index,field,iskey)    [$cmdIC data get $fila 7]

      set regFields($index,field,notnull)  [expr ![$cmdIC data get $fila 6]]
    
      # Informacion por defecto para campos clave
      if {$regFields($index,field,iskey)} {
        set regFields($index,desc,table)    $table
        set regFields($index,desc,lstshow)  {}
      }  
    }
  
    # Pasamos el array a lista y nos quedamos con los pares
    # Para obtener el nombre del array multiplicaremos por dos el
    # indice resultado y quitamos las etiquetas
    set lstInfTab [array get regFields]
#Tolcon_Trace "\nlstInfTab = $lstInfTab"
    set lstIdxLab [lsearch -all -regexp [string tolower $lstInfTab] \
                   ",field,label"]
#Tolcon_Trace "\nlstIdxLab = $lstIdxLab"
    for {set j [expr [llength $lstIdxLab] - 1]} {[expr $j >= 0]} {incr j -1} {
      set ini [lindex $lstIdxLab $j] 
      set end [expr $ini + 1]  
      set lstInfTab [lreplace $lstInfTab $ini $end]        
    }
#Tolcon_Trace "\nlstInfTab = $lstInfTab"
    set lstInfTabFil [lfilter $lstInfTab 1 1]  
#Tolcon_Trace "\nlstInfTabFil = $lstInfTabFil"
    # Rellenamos informacion sobre los campos codigo de la tabla
    if {[string length $prociscode]} {
#puts "ProcIsCode 1"        
      set lstIdx [eval $prociscode -table $table -fields [list $lstInfTabFil]]
    } else {  
      set lstIdx [lsearch -all -regexp [string tolower $lstInfTabFil] \
                  "[join $data(codes) |]"]
    }      
    if {[expr [llength $lstIdx] == 1]} {
      # buscamos el indice del campo codigo
      set indCode \
        [lindex [split [lindex $lstInfTab [expr [lindex $lstIdx 0] * 2]] ,] 0]
      set regFields($indCode,desc,table) $table
    
      # buscamos el indice del campo descripcion
      if {[string length $procisdesc]} {
#puts "ProcIsDesc 1"                
        set lstDes [eval $procisdesc -table $table -fields [list $lstInfTabFil]]
      } else {  
        set lstDes [lsearch -all -regexp [string tolower $lstInfTabFil] \
                    "[join $data(descritions) |]"]
      }        
#Tolcon_Trace "EditMasterUtils::InitTable. lstDes: $lstDes"    
# NOTA: Opcion si queremos solo un campo informativo del codigo
      if {[expr [llength $lstDes] >= 1]} {
        set auxDescCode {}
        foreach des $lstDes {
          lappend auxDescCode [lindex $lstInfTabFil $des]
        }
        set regFields($indCode,desc,lstselect) $auxDescCode
        set data(field,description) $regFields($indCode,desc,lstselect)        
#Tolcon_Trace "EditMasterUtils::InitTable. regFields($indCode,desc,lstselect) : $regFields($indCode,desc,lstselect)"
      }
    } else {
      Tolcon_Trace "La tabla $table no tiene codigo"
    }

    # Informacion sobre las claves ajenas
#Tolcon_Trace "Informacion sobre las claves ajenas nRegFk = $nRegFk ..."    
    if {[expr $nRegFk > 0]} {
#Tolcon_Trace "Informacion sobre las claves ajenas nRegFk = $nRegFk ..."    
      set index 0
      for {set fila 0} {[expr $fila < $nRegFk]} {incr fila} {
        incr index
      
        set namCA [string trim [$cmdIF data get $fila 2] \"];#"
        # buscamos el campo en la tabla
        set indCol [lsearch $lstInfTabFil $namCA]  
        set rowCol \
          [lindex [split [lindex $lstInfTab [expr $indCol * 2]] ,] 0]
#Tolcon_Trace "EditMasterUtils::InitTable. rowCol: $rowCol"      
        # Informacion de la tabla de los campos ajenos  
        set regFields($rowCol,field,fkMRes)   1
      
        set namTab [string trim [$cmdIF data get $fila 3] \"];#"
        set regFields($rowCol,field,links) [$self GetLinks $table $namTab]
        
#OJO. AÑADIMOS INFORMACION SOBRE LOS CAMPOS EXTERNOS DE LA TABLA, QUE NO SON
#CLAVES EXTERNAS DE PRIMER NIVEL
        set lstTableFieldForeing \
          [$self _GetTableParent $namTab [string trim [$cmdIF data get $fila 5] \"]];#"
        set regFields($rowCol,desc,table) [lindex [lindex $lstTableFieldForeing 0] 0]
        set regFields($rowCol,desc,join)  [lindex [lindex $lstTableFieldForeing 0] 1]
#Tolcon_Trace "EditMasterUtils::InitTable. lstTableFieldForeing: $lstTableFieldForeing"                                         

        set regFields($rowCol,desc,show) true                                                
        set regFields($rowCol,desc,lstshow) \
          [$self GetFieldDescForeignKey \
            $table \
            $regFields($rowCol,desc,table) \
            $regFields($rowCol,desc,join)]
      }      
    }
    
#Tolcon_Trace "valor dado a regFields(4,desc,lstshow): $regFields(4,desc,lstshow)"

    # construimos las referencias externas
    if {[expr $nRegFk > 0]} {
      # valores iniciales
      set fila     0
      set index    0
      # lectura adelantada
      set namFk    [string trim [$cmdIF data get $fila 1] \"];#"
      set namFkFij {}
      while {[expr $fila < $nRegFk]} {
        if {![string eq [string tolower $namFk] [string tolower $namFkFij]]} {  
          incr index
          # introducimos tabla
          set regFk($index,table) [string trim [$cmdIF data get $fila 3] \"];#"
          set namFkFij $namFk
        }  
        # buscamos el indice del campo al que asociar esta restricion
        set valFie [string trim [$cmdIF data get $fila 2] \"];#"
        set id [lsearch $lstInfTabFil $valFie]
        if {[expr $id > -1]} {
          # buscamos el indice del campo codigo
          set inx [lindex [split [lindex $lstInfTab [expr $id * 2]] ,] 0]
          lappend regFk($index,indFie)     $inx
          set regFields($inx,field,fkMRes) $index
        }
        # guardamos valores
        lappend regFk($index,fields) \
            [string trim [$cmdIF data get $fila 5] \"];#"
           
        # preparamos siguiente paso
        incr fila
        if {[expr $fila < $nRegFk]} {
          set namFk [string trim [$cmdIF data get $fila 1] \"];#"
        }
      }
#Tolcon_Trace "\n\nEditMasterUtils::InitTable.\n RegFk: [array get regFk]\n\n"    
    }

    # destruimos todas las variables creadas internamente
    ::tol::tableset destroy $cmdIC
    if {[expr $nRegFk > 0]} {
      ::tol::tableset destroy $cmdIF
    }
    ::tol::console stack release $infoColumns
    ::tol::console stack release $infoForeignKeys
#Tolcon_Trace"valor dado a regFields(4,desc,lstshow): $regFields(4,desc,lstshow)"
    # return
    if {[array exists regFk]} {
      return [list [array get regFields] [array get regFk] $nRegC]
    } else {
      return [list [array get regFields] {} $nRegC]
    }
  }


  #///////////////////////////////////////////////////////////////////////////
  method GetFieldDescForeignKey {table tableFk fieldFk} {
  #
  # PURPUSE: Para una clave externa obtenemos un campo de información sobre
  #          el campo indicado
  #
  # PARAMETERS:
  #   table  : Nombre de la tabla principal
  #   tableFk: Nombre de la tabla de la relacion
  #   fieldFk: Nombre del campo de la relacion
  #
  # RETURN: Deveulve la lista de campos descripcion seleccionados para una
  #         tabla
  #
  #/////////////////////////////////////////////////////////////////////////////
    set tolGes [SqlGetTolGestor $gestor]
    set result {}

    # Comprobamos que la tabla sea la misma
    if {[string eq [string tolower $table] [string tolower $tableFk]]} {
      set result $data(field,description)
    } else {
      incr step
      # informacion de la tabla ajena
      set infoColumns [GetGlobalUniqueName __infoColumns$step[clock second]]
      set cmdIC infoColums$step[clock clicks]
      #set cmdTIC "Set $infoColumns = SqlInfoColumns(\"$tableFk\", $tolGes);"+
      set cmdTIC "Set $infoColumns = StdLib::SqlEngine::SqlInfoColumns(\"$tableFk\", $tolGes);"
   
      ::tol::console eval $cmdTIC
      # check exists variable
      if {![TclInfoExists Set $infoColumns]} {
        # ERROR
        error "Error al ejecutar: $cmdTIC"        
        return {}    
      }      
      ::tol::tableset create $cmdIC $infoColumns    
    
      # recorremos la tabla en busca de un valor coincidente
      set ok 0
      set fila 0
      set nRegC [$cmdIC info datarows]    
      while {([expr $fila < $nRegC]) && (!$ok)} {
        set valFie [string trim [$cmdIC data get $fila 1] {"}] ;#"
        if {[string length $procisdesc]} {
#puts "ProcIsDesc 2"            
          set id [lindex [eval $procisdesc -table $tableFk -fields $valFie] 0]   
        } else {
          set id [lsearch [string tolower $data(descritions)] \
                   [string tolower $valFie]]
        }
        if {([llength $id] && [expr $id >= 0])} {
          set ok 1
          set result $valFie
        }
        incr fila
      }  
  
      # destruimos todas las variables creadas internamente
      ::tol::tableset destroy $cmdIC
      ::tol::console stack release $infoColumns
    }
   
#Tolcon_Trace "GetFieldDescForeignKey. result: $result"
    return $result
  }

  #///////////////////////////////////////////////////////////////////////////
  method GetLinks {table tableFk } {
  #
  # PURPUSE: Obtenemos la función a ejecutar en las claves externas. Buscamos
  #          si existe un 'namespace' para la tabla relacionada. Para ello
  #          comparamos el nombre de la tabla con los nombres de los
  #          namespaces existentes. En caso de que coincida alguno,
  #          comprobamos si existe la funcion 'Show' para ese namespace
  #
  # PARAMETERS:
  #   table     : Nombre de la tabla principal
  #   tableFk   : Nombre de la tabla de la relacion
  #   connection: Nombre de la conexion a usar
  #
  # RETURN: Deveulve la lista de campos descripcion seleccionados para una
  #         tabla
  #
  #/////////////////////////////////////////////////////////////////////////////
    # inizialization
    set links [list "::edTableGeneric::Show -table $tableFk -bodbcmodule $bodbcmodule"]
    set prefNam {::ed}
    set lenPref [string length $prefNam]
    # check table ?
    if {[string eq [string tolower $table] [string tolower $tableFk]]} {
      set links {}
    } else {
      # check exists document ?
      set lstNam [namespace children ::]
      set lstIdx [lsearch -all -regexp $lstNam ${prefNam}*]
#Tolcon_Trace "GetLinks. lstNam: $lstNam \n rlstIdx: $lstIdx"
      # recorremos los distintos namespaces
      foreach idx $lstIdx {
        set rNam [string range [lindex $lstNam $idx] $lenPref end]
        set rTab [string range $tableFk \
          [expr [string length $tableFk] - [string length $rNam]] end]
#Tolcon_Trace "GetLinks. rNam: $rNam \n rTab: $rTab"
        # namespace equal table ?
        if {([string eq [string tolower $rNam] [string tolower $rTab]]) && \
            ([expr ([string length $table] - [string length $rTab]) <= 3])} {            
          # exists namespace 
#Tolcon_Trace "GetLinks. namespace: ::${prefNam}${rNam}::Show"
          if {[string length [info commands ${prefNam}${rNam}::Show]]} {
            set links "${prefNam}${rNam}::Show"
          }
        }
      }
    }
#Tolcon_Trace "GetLinks. links: $links"
    # return
    return $links  
  }
  
  #///////////////////////////////////////////////////////////////////////////  
  method _GetTableParent {table field} {  
  #
  # PURPUSE: Obtenemos para una tabla y un campo, la tabla en la que se crea
  #          ese campo y el nombre del campo que hace referencia.
  #
  # PARAMETERS:
  #   table     : Nombre de la tabla
  #   field     : Nombre del campo
  #   connection: Nombre de la conexion a usar
  #
  # RETURN: Deveulve una lista con el nombre de la tabla y el del campo
  #
  #/////////////////////////////////////////////////////////////////////////////
    set tolGes [SqlGetTolGestor $gestor]
    # name of Text TOL
    set namTabFie [GetGlobalUniqueName __nameTabFie__]
    set cmd "Text $namTabFie = TxtListTcl(SqlGetTableParent(\"${table}\", \"${field}\", $tolGes));"
    # run
    ::tol::console eval $cmd
    set lst [string trim [TclGetVar Text $namTabFie] {"}];#"
    if {[string length $lst]} { eval set lst $lst }
    # destroy
    ::tol::console stack release $namTabFie    
    # retunr
    return $lst
  }
}
#snit::widget

package provide beditmaster 1.0