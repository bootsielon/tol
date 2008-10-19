#/////////////////////////////////////////////////////////////////////////////
# FILE    : bchilddetail.tcl
# PURPOSE : Bayes Editor Parametros. Editor de registros de una tabla hija,
#           dependientes de un valor de una tabla padre.
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
# The module that calls bchilddetail must define this variables correctly
# BEFORE creating the widget (That is, before calling bchilddetail).
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
# Variables internas de bchilddetail
#/////////////////////////////////////////////////////////////////////////////
#
#  Marca los campos que aparecerán por columnas. El index determina la columna
#    del campo marcadol
#
# fieldInfo($index,$tbName,field,name)      # nombre del campo              
# fieldInfo($index,$tbName,field,type)      # tipo y tamaño, en formato lista
# fieldInfo($index,$tbName,field,iskey)     # indica si es clave
# fieldInfo($index,$tbName,field,isFK)      # indica si es FK de alguna tabla             
# fieldInfo($index,$tbName,field,label)     # texto a mostrar 
#                                   #  (de no proporcinarse se iguala al name)              
# fieldInfo($index,$tbName,field,notnull)   # indica si es obligatorio o no 
#                                   #  (si es clave, es obligatorio)
# fieldInfo($index,$tbName,field,fkMRes)    # foreing key en la que mas restrictiva
#                                   #  del campo
# fieldInfo($index,$tbName,field,edit)      # Indica si el campo es editable o no
# fieldInfo($index,$tbName,field,ptr)       # path del objeto en el que se visualiza
# fieldInfo($index,field,links)     # lista de enlaces a otros formularios ediMaestro
#                                   # (se pasa el procedimiento Show)
# uno por enlace
# fieldInfo($index,field,$i,link)   # procedimiento a llamar al pulsar el boton
#                                   # del enlace
#
# informacion de los campos descriptivos sobre el campo
# fieldInfo($index,$tbName,desc,table)      # tabla en la que se encuentra la descripcion
# fieldInfo($index,$tbName,desc,join)       # nombre del campo de esta tabla al que hace
#                                   #  referencia, de no proporcionrse es igual a
#                                   #  $fieldInfo($index,$tbName,field,name)   
# fieldInfo($index,desc,show)       # indica si el campo tiene descripcion
# fieldInfo($index,$tbName,desc,lstfields)  # lista completa con los campos a mostrar en la selecion
# fieldInfo($index,$tbName,desc,lstshow)    # lista de campos descripcion a mostrar
# fieldInfo($index,$tbName,desc,lstselect)  # lista de campos a mostrar en una seleccion + 
#                                   #  fieldInfo($index,$tbName,desc,lstshow)
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
# fieldFkInfo($indexFk,$tbName,table)       # tabla a la que hace referencia
# fieldFkInfo($indexFk,$tbName,indFie)      # lista de campos a los que hace referencia
# fieldFkInfo($indexFk,$tbName,fields)      # nombre de los campos en la tabla referenciada
#
#
#/////////////////////////////////////////////////////////////////////////////
# Informacion proporcionada
#/////////////////////////////////////////////////////////////////////////////
#
# regFields($index,$tbName,field,name)      # nombre del campo (*)
# regFields($index,$tbName,field,type)      # tipo y tamaño, en formato lista (*)
# regFields($index,$tbName,field,iskey)     # indica si es clave (*)
# regFields($index,$tbName,field,label)     # texto a mostrar
# regFields($index,$tbName,field,notnull)   # indica si es obligatorio o no 
#                                   #  (si es clave, es obligatorio)
# regFields($index,$tbName,field,fkMRes)    # foreing key en la que mas restrictiva
#                                   #  del campo
# regFields($index,$tbName,field,links)     # lista de enlaces a otros formularios ediMaestro
#                                   # (se pasa el procedimiento Show)
#
## informacion de los campos descripcion sobre el campo, solo se proporciona de existir
# regFields($index,$tbName,desc,table)      # tabla en la que se encuentra la descripcion (*)
# regFields($index,$tbName,desc,join)       # nombre del campo de esta tabla al que 
#                                   #  hace referencia
# regFields($index,$tbName,desc,lstshow)    # lista de campos descripcion a mostrar 
# regFields($index,$tbName,desc,lstselect)  # lista de campos a mostrar en la seleccion + 
#                                   #  regFields($index,$tbName,desc,lstshow)
# regFields($index,$tbName,desc,lstedit)    # lista que indica si los campos de
#                                   #   "lstshow" son editables o no
# regFields($index,$tbName,desc,lsttype)    # si los campos son editables, 
#                                   # se especifica su tipo en formato lista de listas
# regFields($index,$tbName,desc,lstnotnull) # si los campos son editables, 
#                                   # se especifica si son obligatorios o no en formato lista
#
# informacion de las claves externas, solo se proporciona de existir
# regFk($indexFk,$tbName,table)             # tabla a la que hace referencia (*)
# regFk($indexFk,$tbName,indFie)            # lista de campos a los que hace referencia (*)
# regFk($indexFk,$tbName,fields)            # nombre de los campos en la tabla referenciada (*)
#
#
#/////////////////////////////////////////////////////////////////////////////
# Estructura de tablas hijas
#/////////////////////////////////////////////////////////////////////////////
#
# widget(tabset)                    # tabset de las tablas hija
# widget($tableName,tab)            # tab de cada tabla hija
# widget(tabParent)                 # Frame parent para el tabset
#
# child(numTables)                  # numero de tablas hija
# child($index,tableName)           # nombre de las tablas hija
# child(lstColNames)                # lista de campos que son FK de alguna hija
# child(selectedField)              # campo padre modificado
# child(selectedValue)              # Valor del campo padre modificado
# child(actualTbName)               # Nombre de la tabla cuyo tab está visualizado.
#
# child($parentColName,tableNames)  # nombre de las tablas hija, asociadas a un campo
# child($parentColName,$tableName,FKName) # Nombre de la columna hija que hace 
                                          #  referencia a la columna padre dada
# child($index,ptr)                 # puntero al widget asociado
# child($index,$tableName,field)    # nombre del campo de la tabla hija relacionado
# child($colName,$tableName,FKName) # Nombre de la col que es FK hacia la col padre dada 

##
# child($tbName,maxFields)          # Numero maximo de campos de cada tabla.
# child($tbName,fieldInfo)          # Información sobre los campos de la tabla.
# child($tbName,fieldFkInfo)        # Información sobre las claves ajenas de la tabla.
#




#/////////////////////////////////////////////////////////////////////////////
::snit::widget bchilddetail {
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
  widgetclass Bchilddetail

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
  option -child {}

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
  # Array de valores de tablas hijas.
  variable child
  # Tabla hija actual
  variable childName
  #Don't use it, but will in the future:
  variable oldStatus
 
  variable data
  variable step 0
  
  #///////////////////////////////////////////////////////////////////////////
  typeconstructor {
  #
  # PURPOSE: bchilddetail typeconstructor
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
  # PURPOSE: bchilddetail constructor
  #
  # PARAMETERS:
  #   args ->
  #
  #///////////////////////////////////////////////////////////////////////////
#puts "bchilddetail: args : $args"
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
    set childName     $options(-child)
    Tolcon_Trace "BODBCMODULE = $bodbcmodule, CONECTION = $connection, GESTOR = $gestor"
    # autoload ?
    if { $autoload } {
      set lstInfo [$self InitTable $tablename] 
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
          -title [mc "Child Detail"] \
          -type ok
        return 0
      }
    } else {
      array set fieldInfo    $options(-regfields)
      array set fieldFkInfo  $options(-regfk)
      set maxFields          $options(-maxfield)
    }

    ##############################
    set tbName $tablename

    #
    #ScrolledWindow $self.sw -relief sunken -borderwidth 2 -auto both
    #ScrollableFrame $self.sw.fr 
    #$self.sw setwidget $self.sw.fr
    #set widget(frc) [$self.sw.fr getframe]
  
    set widget(frc) [frame $self.sw -relief sunken]
      
    # Configuracion del formulario:
    # 1.- Zona de información de la tabla padre -> widget(tf,pk,fr)
    $self createParentZone

    # 2.- rejilla de datos: Tablas hija -> widget(tf,data)
    $self createChildZone

    # 3.- extra grid
    set widget(fr,extra) [frame $widget(frc).extra]
    
    # 3.- barra de botones: widget(bbox)
    $self createBB $tbName $parent

    # Empacar
    #pack $widget(bbox)  -side bottom -fill x    -expand no
    #pack $widget(tf,data) -side top   -fill both -expand yes 
    #pack $self.sw       -side top    -fill both -expand yes
      
    #grid $widget(tf,pk,fr) -sticky news
    #grid $widget(tf,data) -sticky news
    #grid $widget(bbox)    -sticky news
      
    #set status "sel"
    set status "sel"
    $self setStatus "sel"
    # Fill out the form with all database values:
#    $self fillForm
    # Show the form to the user:
    $self showForm
  }

  #/////////////////////////////////////////////////////////////////////////////
  onconfigure -regfields {value } {
  #/////////////////////////////////////////////////////////////////////////////
    variable child

    # Recuperamos las tablas hijas.
    $self getDependentTables
    
    array set fieldInfo $value
    if {[info exists $childName] && ([string length $childName]!=0)} {
       set tbName $childName
    } else {
       set tbName $tablename    
    }
    
    set child($tbName,maxFields) [$self cget -maxfield]
    for {set x 1} {[expr $x <= [$self cget -maxfield]]} {incr x} {
      
      # comprobamos label
      if {![info exists fieldInfo($x,$tbName,field,label)]} {
        set fieldInfo($x,$tbName,field,label) $fieldInfo($x,$tbName,field,name)
      }
      # comprobamos los botones de edicion especificados
      if {[info exists fieldInfo($x,field,links)]} {
        for {set idx 0} {$idx < [llength $fieldInfo($x,field,links)]} {incr idx} {
          set fieldInfo($x,link,[expr $idx+1],proc) [lindex $fieldInfo($x,field,links) $idx]
        }
      }
      # comprobamos si el codigo es editable
      if {![info exists fieldInfo($x,$tbName,field,edit)]} {
        if {[info exists fieldInfo($x,$tbName,field,fkMRes)]} {
          set fieldInfo($x,$tbName,field,edit) false
        } else {
          set fieldInfo($x,$tbName,field,edit) true
        }
      }
      # comprobamos que exista descripcion
      if {[info exists fieldInfo($x,$tbName,desc,table)]} {
        set lstfields {}
        # comprobamos join
        if {![info exists fieldInfo($x,$tbName,desc,join)]} {
          set fieldInfo($x,$tbName,desc,join) $fieldInfo($x,$tbName,field,name)
        }
        # lista de campos a mostar al seleccionar, primero es el codigo
        lappend lstfields $fieldInfo($x,$tbName,desc,join)
        # indicamos que tiene descripcion
        set fieldInfo($x,$tbName,desc,show) true
        # guardamos informacion sobre los campos descripcion a mostrar
        # nombres
        set i 1
        #if {[info exists fieldInfo($x,$tbName,desc,lstshow)]} 
        if {[info exists fieldInfo($x,$tbName,desc,lstshow)] && \
            [llength $fieldInfo($x,$tbName,desc,lstshow)]} {
          foreach n $fieldInfo($x,$tbName,desc,lstshow) {
            lappend lstfields $n
            set fieldInfo($x,$tbName,desc,$i,name) $n
            # por defecto edit -> false
            set fieldInfo($x,$tbName,desc,$i,edit) false
            incr i
          } 
        } else {
          set fieldInfo($x,$tbName,desc,lstshow) {}
          # comrpbamos que tenga valor 'true', si es asi lo dejamos con su valor          
          if {[info exists fieldInfo($x,$tbName,desc,show)] && \
              [string length $fieldInfo($x,$tbName,desc,show)] && \
              !($fieldInfo($x,$tbName,desc,show))} {
            set fieldInfo($x,$tbName,desc,show) false
          }
        }
        # edit
        set i 1
        if {[info exists fieldInfo($x,$tbName,desc,lstedit)]} {
          foreach e $fieldInfo($x,$tbName,desc,lstedit) {
            set fieldInfo($x,$tbName,desc,$i,edit) $e
            incr i
          } 
        }        
        # tipo
        if {[info exists fieldInfo($x,$tbName,desc,lsttype)]} {
          set i 1
          foreach t $fieldInfo($x,$tbName,desc,lsttype) {
            set fieldInfo($x,$tbName,desc,$i,type) $t
            incr i
          }
        }
        # notnull   
        if {[info exists fieldInfo($x,$tbName,desc,lstnotnull)]} {
          set i 1
          foreach nn $fieldInfo($x,$tbName,desc,lstnotnull) {
            set fieldInfo($x,$tbName,desc,$i,notnull) $nn
            incr i
          }
        }
        # lista de campos para la selecciona
        if {[info exists fieldInfo($x,$tbName,desc,lstselect)]} {
          foreach n $fieldInfo($x,$tbName,desc,lstselect) {
            lappend lstfields $n
          }    
        }
        set fieldInfo($x,$tbName,desc,lstfields) $lstfields
      } else  {
        set fieldInfo($x,$tbName,desc,show) false
      }
    }
   
#   Carga de todos los valores de las tablas: 
   for {set k 1} {[expr $k <= $child(numTables)]} {incr k} {
    set tbName $child($k,tableName)
    # Llamamos a InitTable para recuperar la información de cada tabla hija.
    set lstInfo [$self InitTable $tbName] 
   if {[llength $lstInfo]} {
      array set fieldInfo   [lindex $lstInfo 0]
      array set fieldFkInfo [lindex $lstInfo 1]
      set maxFields         [lindex $lstInfo 2]
      set child($tbName,maxFields)   $maxFields
      set child($tbName,fieldInfo)   [lindex $lstInfo 0]
      set child($tbName,fieldFkInfo) [lindex $lstInfo 1]
    } else {
      tk_messageBox -icon error \
        -message [mc "An error has taken place during the creation of the form, reason why this it could not be visualized"] \
        -parent $parent \
        -title [mc "Child Detail"] \
        -type ok
      return 0
    }        

    for {set x 1} {[expr $x <= $child($tbName,maxFields)]} {incr x} {
      # comprobamos label
      if {![info exists fieldInfo($x,$tbName,field,label)]} {
        set fieldInfo($x,$tbName,field,label) $fieldInfo($x,$tbName,field,name)
      }
      # comprobamos los botones de edicion especificados
      if {[info exists fieldInfo($x,field,links)]} {
        for {set idx 0} {$idx < [llength $fieldInfo($x,field,links)]} {incr idx} {
          set fieldInfo($x,link,[expr $idx+1],proc) [lindex $fieldInfo($x,field,links) $idx]
        }
      }
      # comprobamos si el codigo es editable
      if {![info exists fieldInfo($x,$tbName,field,edit)]} {
        if {[info exists fieldInfo($x,$tbName,field,fkMRes)]} {
          set fieldInfo($x,$tbName,field,edit) false
        } else {
          set fieldInfo($x,$tbName,field,edit) true
        }
      }
      # comprobamos que exista descripcion
      if {[info exists fieldInfo($x,$tbName,desc,table)]} {
        set lstfields {}
        # comprobamos join
        if {![info exists fieldInfo($x,$tbName,desc,join)]} {
          set fieldInfo($x,$tbName,desc,join) $fieldInfo($x,$tbName,field,name)
        }
        # lista de campos a mostar al seleccionar, primero es el codigo
        lappend lstfields $fieldInfo($x,$tbName,desc,join)
        # indicamos que tiene descripcion
        set fieldInfo($x,$tbName,desc,show) true
        # guardamos informacion sobre los campos descripcion a mostrar
        # nombres
        set i 1
#        if {[info exists fieldInfo($x,$tbName,desc,lstshow)]} 
        if {[info exists fieldInfo($x,$tbName,desc,lstshow)] && \
            [llength $fieldInfo($x,$tbName,desc,lstshow)]} {
          foreach n $fieldInfo($x,$tbName,desc,lstshow) {
            lappend lstfields $n
            set fieldInfo($x,$tbName,desc,$i,name) $n
            # por defecto edit -> false
            set fieldInfo($x,$tbName,desc,$i,edit) false
            incr i
          } 
        } else {
          set fieldInfo($x,$tbName,desc,lstshow) {}
          # comrpbamos que tenga valor 'true', si es asi lo dejamos con su valor          
          if {[info exists fieldInfo($x,$tbName,desc,show)] && \
              [string length $fieldInfo($x,$tbName,desc,show)] && \
              !($fieldInfo($x,$tbName,desc,show))} {
            set fieldInfo($x,$tbName,desc,show) false
          }
        }
        # edit
        set i 1
        if {[info exists fieldInfo($x,$tbName,desc,lstedit)]} {
          foreach e $fieldInfo($x,$tbName,desc,lstedit) {
            set fieldInfo($x,$tbName,desc,$i,edit) $e
            incr i
          } 
        }        
        # tipo
        if {[info exists fieldInfo($x,$tbName,desc,lsttype)]} {
          set i 1
          foreach t $fieldInfo($x,$tbName,desc,lsttype) {
            set fieldInfo($x,$tbName,desc,$i,type) $t
            incr i
          }
        }
        # notnull   
        if {[info exists fieldInfo($x,$tbName,desc,lstnotnull)]} {
          set i 1
          foreach nn $fieldInfo($x,$tbName,desc,lstnotnull) {
            set fieldInfo($x,$tbName,desc,$i,notnull) $nn
            incr i
          }
        }
        # lista de campos para la selecciona
        if {[info exists fieldInfo($x,$tbName,desc,lstselect)]} {
          foreach n $fieldInfo($x,$tbName,desc,lstselect) {
            lappend lstfields $n
          }    
        }
        set fieldInfo($x,$tbName,desc,lstfields) $lstfields
#puts "...........Es FK: $x es: fieldInfo($x,$tbName,desc,lstfields)-> $fieldInfo($x,$tbName,desc,lstfields)"
      } else  {
        set fieldInfo($x,$tbName,desc,show) false
      }
    }
   }
  }

  method createParentZone {} {
  #
  # PURPOSE: Create parent table widget ( widget(tf,pk,fr) ).
  #
  # TODO: Cargar la información de los campos que sean FK de alguna tabla.
  #       Crear la zona ocultable de campos que no sean FK.
  #
  # PARAMETERS: 
  #
  # RETURN: 
  #
  #///////////////////////////////////////////////////////////////////////////  
    set widget(tf,pk)    [TitleFrame $widget(frc).pk -text [mc "Parent Fields"]]
    set widget(tf,pk,fr) [$widget(frc).pk getframe]
    
    $widget(tf,pk,fr) configure -height 100
    
    $self showMasterTable $widget(tf,pk,fr)
    
    grid columnconfigure $widget(tf,pk,fr) 0 -weight 1
    grid rowconfigure    $widget(tf,pk,fr) 0 -weight 1
    
  }

  method createChildZone {} {
  #
  # PURPOSE: Create children tables widget ( widget(tf,data) ).
  #
  # TODO: Asociar un scroll a cada tab de forma independiente.
  #
  # PARAMETERS: parent -> widget padre.
  #
  # RETURN: 
  #
  #///////////////////////////////////////////////////////////////////////////  
    set widget(tf,data) [TitleFrame $widget(frc).data \
                         -text [mc "Children tables"]]
    set f2 [$widget(tf,data) getframe]
    
    $f2 configure -height 300
    
    #ScrolledWindow $f2.sw -relief sunken -borderwidth 2
    #ScrollableFrame $f2.sw.fr
    #$f2.sw setwidget $f2.sw.fr
    #set widget(tf,data,fr) [$f2.sw.fr getframe]
    #
    #pack $f2.sw -expand true -fill both
    
    # Generación de la zona de información de las tablas hija:
    #  crear los tabs contenedores y rellenar el formulario que contienen.
    set widget(tabParent) $f2
    $self createTabs $f2
    $self createTableForm

    #set widget(tf,data,fr) $widget(tabset)
    grid columnconfigure $f2 0 -weight 1
    grid rowconfigure $f2 0 -weight 1
  }


  #///////////////////////////////////////////////////////////////////////////
  method createBB {tbName parent} {
  #
  # PURPOSE: Create buttons for the widget ( widget(bbox) ).
  #
  # TODO: Generar una extructura de estados de cada botón asociada a cada tabla.
  #
  # PARAMETERS: parent -> widget padre.
  #
  # RETURN: 
  #
  #///////////////////////////////////////////////////////////////////////////
    set widget(bbox) \
      [ButtonBox $self.bb -orient horizontal -spacing 0 -padx 0 -pady 0 \
                 -homogeneous false]

    set widget(bt,new) \
      [$widget(bbox) add -text [mc "New"] -relief link \
         -command [list $self botnew $tbName] \
         -helptext [mc "New"] -disabledforeground ""\
         -image [Bitmap::get MODFile] -compound left -padx 1 -pady 1]
    set widget(bt,edit) \
      [$widget(bbox) add -text [mc "Edit"] -relief link \
         -state disable -command [list $self botedi $tbName] \
         -helptext [mc "Edit current"] -disabledforeground ""\
         -image [Bitmap::get edit] -compound left -padx 1 -pady 1]
    set widget(bt,save) \
      [$widget(bbox) add -text [mc "Save"] -relief link \
         -state disable -command [list $self botgua $tbName] \
         -helptext [mc "Save current"] -disabledforeground ""\
         -image [Bitmap::get bsave] -compound left -padx 1 -pady 1]
    set widget(bt,cancel) \
      [$widget(bbox) add -text [mc "Cancel"] -relief link \
         -state disable -command [list $self botcan $tbName] \
         -helptext [mc "Cancel operation"] -disabledforeground ""\
         -image [Bitmap::get cancelx] -compound left -padx 1 -pady 1]
    set widget(bt,delete) \
      [$widget(bbox) add -text [mc "Delete"] -relief link \
         -state disable -command [list $self boteli $tbName] \
         -helptext [mc "Delete current"] -disabledforeground ""\
         -image [Bitmap::get delete] -compound left -padx 1 -pady 1]
    set widget(bt,clear) \
      [$widget(bbox) add -text [mc "Clear"] -relief link \
          -command [list $self botlim ] \
          -helptext [mc "Clear form"] -disabledforeground ""\
          -image [Bitmap::get Clear] -compound left -padx 1 -pady 1]
    set widget(bt,refresh) \
      [$widget(bbox) add -text [mc  "Refresh"] -relief link \
         -command [list $self botref ] \
         -helptext [mc "Refresh form"] -disabledforeground ""\
         -image [Bitmap::get refresh] -compound left -padx 1 -pady 1]
    set widget(bt,open) \
      [$widget(bbox) add -text [mc "Open"] -relief link \
         -command [list $self botabr $tbName] -state disabled \
         -helptext [mc "Open table"] -disabledforeground ""\
         -image [Bitmap::get opentable] -compound left -padx 1 -pady 1]
    set widget(bt,exit) \
      [$widget(bbox) add -text [mc "Exit"] -relief link \
         -command [list $self botsal $parent] \
         -helptext [mc "Close window"] -disabledforeground ""\
         -image [Bitmap::get exit] -compound left -padx 1 -pady 1]  
  }


  #///////////////////////////////////////////////////////////////////////////
  method createTabs {parent} {
  #
  # PURPOSE: Create a tab for every table to be shown in the widget ( widget(tabset) )
  #          and fill it with the table.
  #
  # PARAMETERS: parent -> widget padre.
  #
  # RETURN: 
  #
  #///////////////////////////////////////////////////////////////////////////
    variable widget
    
    set w_tabset [::blt::tabset $parent.ts -relief flat -highlightthickness 0\
        -bd 0 -outerpad 0 -tier 1 -slant right -textside right -scrollincrement 20]
    grid $w_tabset -sticky news
    
    set widget(tabset) $w_tabset
    
    grid columnconfigure $widget(tabset) 0 -weight 1
    grid rowconfigure    $widget(tabset) 0 -weight 1
    
    for {set i 1} {$i<=$child(numTables)} {incr i} {
      set tbName $child($i,tableName)
      set widget($child($i,tableName),tab) [frame $w_tabset.f$i]
      $w_tabset insert end $child($i,tableName)
      $w_tabset tab configure $child($i,tableName) -text [mc $child($i,tableName)] \
        -window $widget($child($i,tableName),tab) \
        -fill both -padx 0 -selectbackground \#d9d9d9 -bg gray75
      $w_tabset tab configure $child($i,tableName) \
        -command [list $self reloadTabWidgets $child($i,tableName)]

#puts "createTabs -> $tbName"      
      set nCols $child($tbName,maxFields)
      set widget($tbName,visutable) \
      [btable $widget($tbName,tab).btable$tbName -parent $widget($tbName,tab) \
        -bg white \
        -toolbar 1 -width 1 -titlerows 1 -cols $nCols \
        -variable tab -flashmode off -cursor top_left_arrow \
        -borderwidth 1]
      $widget($tbName,visutable) configure -cols $nCols
      $widget($tbName,visutable) configure -state normal
      for {set k 1} {$k <= $nCols} { incr k} {
        $widget($tbName,visutable) set col 0,$k  $fieldInfo($k,$tbName,field,name)
      }
      
      grid columnconfigure $widget($child($i,tableName),tab) 0 -weight 1
      grid rowconfigure    $widget($child($i,tableName),tab) 0 -weight 1
      
      # Engridamos las tablas
      grid $widget($tbName,visutable) -sticky news
    }
    set child(actualTbName) $child(1,tableName)
  }

  #///////////////////////////////////////////////////////////////////////////
  method getDependentTables {} {
  #
  # PURPOSE: Obtain all the tables that depend on current table by a foreign key.
  # TODO:
  #
  # PARAMETERS:
  # RETURN: 
  #
  #///////////////////////////////////////////////////////////////////////////
  
  #Usamos la función TOL SqlInfoDependent para obtener el Set de nombres de tablas
  # relacionadas con alguna de las columnas de la tabla.
  # Devuelve un Set de estructuras StSqlInfoForeing 
  #(Table, Index, Column, ForeignTable, ForeignIndex, ForeignColumn)
  variable child
  variable fieldInfo

  set setTableNames [GetGlobalUniqueName __setTableNames]
 
  # $tablename es el nombre de la tabla padre.
  set tolCmd  \
    "Text $setTableNames = TolSet2TclLst (SqlInfoDependent(\"$tablename\", Copy(Empty), GesAct));"
  TclTolEval $tolCmd
  
  eval set lstTables [TclGetVar Text $setTableNames]
#puts "Lista de tablas hijas: $lstTables"
  set indice 1
  set child(lstColNames) {}
  foreach i $lstTables {
      set tableName [lindex $i 0]
      if { [string equal $tableName $tablename] eq 0 } {
        set child($indice,tableName) $tableName
      } else {
        # Si no es necesario añadir la tabla porque es la propia tabla padre,
        # hay que evitar que avance el índice, para evitar vacios en la estructura
        # de datos.
        set indice [expr $indice - 1]
      }
#puts "child($indice,tableName)->$child($indice,tableName)"
      # Nombre de la clave padre.
      set parentColName [lindex $i 5]
      # Nombre de la columna dependiente en la tabla hija
      set colName [lindex $i 2]
      
      # Generar la lista de columnas padre que son FK de alguna hija.
      if { ([lsearch -dictionary $child(lstColNames) $parentColName] eq -1) } {
        lappend child(lstColNames) $parentColName
      }
      
      # almacenar el nombre de columna hija
      #set child($indice,$tableName,field) $colName
      # añadir el nombre de la tabla a la lista de tablas asociadas al campo
      # que sirve de referencia a la FK
      lappend child($parentColName,tableNames) $tableName
      # Asignar el nombre de la columna hija de la tabla, para la columna padre.
      set child($parentColName,$tableName,FKName) $colName

#puts "Nombre tabla: $child($indice,tableName) y su columna: $child($parentColName,$tableName,FKName)"
#puts "-----columna hija de la FK $parentColName: $child($parentColName,$tableName,FKName)"
      set indice [expr $indice + 1]
    }
    set child(numTables) [expr $indice - 1]
#puts "-------listaTablas child($parentColName,tableNames) -> $child($parentColName,tableNames)"
#puts "Lista columnas padre: $child(lstColNames)"
  }

  method reloadTabWidgets {tbName} {
  #
  # PURPOSE: Delete widgets of actual tab and generate widgets of newly selected
  #          tab.
  # PARAMETERS:
  #        tbName -> Nombre de la tabla actualmente seleccionada.
  #///////////////////////////////////////////////////////////////////////////
  if {[info exists child(selectedField)]} {
    for {set k 1} { $k <= $child(numTables)} {incr k} {
        # Si existe dato modificado en padre-> cargar las tablas hijas.  
        set table $child($k,tableName)
        if {[info exists child(selectedField)]} {
           $self childLoad $table $child($child(selectedField),$table,FKName)
        } 
    }
  }
    # Si se cambia de tab, actualizamos:
    if { [string equal $tbName $child(actualTbName)] eq 0 } {
      set child(actualTbName) $tbName
    } 
  }

  #///////////////////////////////////////////////////////////////////////////
  method createTableForm {} {
  #
  # PURPOSE: Generate all the tabs of the widget, one for each table.
  #          It's needed to have been created tabs associated for each table.
  #
  #///////////////////////////////////////////////////////////////////////////
  
  # Rellenamos la estructura de información de las TABLAS HIJAS.
    for {set k 1} {$k<=$child(numTables)} {incr k} {
      set tbName $child($k,tableName)
      $self showTabTable $widget($child($k,tableName),tab) $tbName
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method showMasterTable {fdata} {
  #
  # PURPOSE: Create and Show tab information for master table.
  # TODO: Crear el widget ocultable con datos que no sean FK de alguna tabla.
  #       
  # PARAMETERS:
  #  fdata : path del frame de los valores (tab asociada a la tabla)
  # RETURN:
  #
  #///////////////////////////////////////////////////////////////////////////
    set tbName $tablename
    
    ScrolledWindow $fdata.swMaster -relief sunken -borderwidth 2
    ScrollableFrame $fdata.swMaster.fr
    $fdata.swMaster setwidget $fdata.swMaster.fr
    set widget(master,data,fr) [$fdata.swMaster.fr getframe]
    
    #$widget(master,data,fr) configure -height 20
    
    #pack $fdata.swMaster -expand true -fill both
    
    grid $fdata.swMaster -sticky news
    
    #grid $widget(master,data,fr) -sticky news
    
    grid columnconfigure $fdata.swMaster 0 -weight 0
    grid columnconfigure $fdata.swMaster 1 -weight 1
    
    grid rowconfigure $fdata.swMaster 0 -weight 1
  
    #set widget(frc) [frame $fdata.swMaster -relief sunken]
    
    $self createWidgetTable $widget(master,data,fr) $tbName

    for {set k 1} {$k<=$child($tbName,maxFields)} {incr k} {    
      grid $widget($k,$tbName,field,label) \
        $widget(ptr,[string tolower $fieldInfo($k,$tbName,field,name)]) \
        -sticky news
      grid columnconfigure \
           $widget(ptr,[string tolower $fieldInfo($k,$tbName,field,name)]) 1 \
           -weight 1
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method showTabTable {fdata tbName} {
  #
  # PURPOSE: Create and Show tab information for a table.
  # TODO: 
  #
  # PARAMETERS:
  #  fdata : path del frame de los valores (tab asociada a la tabla)
  #  tbName: nombre de la tabla asignada al tab.
  # RETURN:
  #
  #///////////////////////////////////////////////////////////////////////////
    ScrolledWindow $fdata.sw$tbName -relief sunken -borderwidth 2
    ScrollableFrame $fdata.sw$tbName.fr
    $fdata.sw$tbName setwidget $fdata.sw$tbName.fr
    set widget($tbName,data,fr) [$fdata.sw$tbName.fr getframe]
    
    pack $widget($tbName,data,fr) -expand true -fill both
  }


  #///////////////////////////////////////////////////////////////////////////
  method createWidgetTable {fdata tbName} {
  #
  # PURPOSE: Create widget information for each table.
  # TODO: 
  #
  # PARAMETERS:
  #  fdata : path del frame de los valores (tab asociada a la tabla)
  #  tbName: nombre de la tabla asignada al tab.
  # RETURN:
  #
  #///////////////////////////////////////////////////////////////////////////
    set lstFields {}
    for {set x 1} {$x<=$child($tbName,maxFields)} {incr x} {
      if {$fieldInfo($x,$tbName,field,iskey)} {
        # campos claves
        label $fdata.label$x$tbName -text $fieldInfo($x,$tbName,field,label) -fg blue
        set fieldInfo($x,$tbName,field,ptr) $fdata.cb$x$tbName
        set widget(ptr,[string tolower $fieldInfo($x,$tbName,field,name)]) $fieldInfo($x,$tbName,field,ptr)
        set outValue $fieldInfo($x,$tbName,field,label)
        set fieldInfo($x,$tbName,header,name) $outValue

        # Si la tabla es la padre, solo mostrar select de los campos que sean FK
        # de alguna tabla hija --> estan en child(lstColNames)
        if { [string match $tablename $tbName] } {
          if { ([lsearch -dictionary $child(lstColNames) $fieldInfo($x,$tbName,field,name)] != -1) } {
            $self CreateSelectFields $x $tbName
          }
        } else {
          $self CreateSelectFields $x $tbName
        }
        set widget($x,$tbName,field,label) $fdata.label$x$tbName
      } else {
        # campos NO claves
        if {$fieldInfo($x,$tbName,field,notnull)} {
          set clr blue
        } else  {
          set clr black
        }
        label $fdata.label$x$tbName -text $fieldInfo($x,$tbName,field,label) -fg $clr
        if {$fieldInfo($x,$tbName,desc,show)} {
          set fieldInfo($x,$tbName,field,ptr) $fdata.cb$x$tbName
          set widget(ptr,[string tolower $fieldInfo($x,$tbName,field,name)]) \
            $fieldInfo($x,$tbName,field,ptr)          
          $self CreateSelectFields $x $tbName
        } else {
          set fieldInfo($x,$tbName,field,ptr) [$self CreateWidget $fdata $x $tbName]
          set widget(ptr,[string tolower $fieldInfo($x,$tbName,field,name)]) \
            $fieldInfo($x,$tbName,field,ptr)                    
        }
        set widget($x,$tbName,field,label) $fdata.label$x$tbName
      }
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method CreateSelectFields {x tbName} {
  # PURPOSE:
  # PARAMETERS:
  #    x -> indice del campo select dentro del widget contenedor.
  #    tbName -> nombre de la tabla que contiene el campo.
  # RETURN:
  #///////////////////////////////////////////////////////////////////////////
    variable child
    
#Tolcon_Trace "SELFNS=$selfns"
    lappend lst_help "$fieldInfo($x,$tbName,field,name) ($fieldInfo($x,$tbName,field,type))"
#Tolcon_Trace "CreateSelectFields $x"
    for {set i 0} {$i < [llength $fieldInfo($x,$tbName,desc,lstshow)]} {incr i} {
      set nam [lindex $fieldInfo($x,$tbName,desc,lstshow) $i]
      if {[info exists fieldInfo($x,$tbName,desc,lsttype)]} {
        set typ " ([lindex $fieldInfo($x,$tbName,desc,lsttype) $i])"
      } else  {
        set typ ""
      }
      lappend lst_help "$nam $typ"
    }
    
#Tolcon_Trace ""
#Tolcon_Trace "lst_help=$lst_help"
    if {([string length $fieldInfo($x,$tbName,desc,lstshow)] eq 0)} {
      set fields [list $fieldInfo($x,$tbName,field,name)]  
    } else {
      set fields [list $fieldInfo($x,$tbName,field,name) $fieldInfo($x,$tbName,desc,lstshow)]
    }
    update
    set combo \
      "bcomboboxplus   $fieldInfo($x,$tbName,field,ptr) \
      -cols        \"$fieldInfo($x,$tbName,field,name) $fieldInfo($x,$tbName,desc,lstshow)\" \
      -colslb      \"$fieldInfo($x,$tbName,field,name) $fieldInfo($x,$tbName,desc,lstshow)\" \
      -titles      \"$fieldInfo($x,$tbName,desc,lstfields)\" \
      -postcommand [list [list $self GetValuesShow $x $tbName]] \
      -modifycmd   [list [list $self botsel $tbName $x]] \
      -helptexts   [list $lst_help] \
      -showlabels  0 \
      -widthlb     600 \
      -font        {Arial 9} \
      -nrows       15"
    switch [string tolower [lindex $fieldInfo($x,$tbName,field,type) 0]] {
      # datetime y smalldatetime
      "date" -
      "datetime"  -
      "smalldatetime" {
        set combo "$combo  -keydate     1"
      }
    }
    eval $combo
    # guardamos los nombres de los objetos de las descripciones
    if {$fieldInfo($x,$tbName,desc,show)} {
      set i 1          
      set allObj [$fieldInfo($x,$tbName,field,ptr) getwidget ]
      if {$fieldInfo($x,$tbName,field,edit)} {
#Tolcon_Trace "CreateSelectFields: configurando [lindex $allObj 0]"
        [lindex $allObj 0] configure -validate key -vcmd \
          [list SqlValidateData %P $fieldInfo($x,$tbName,field,type)]
      }  
      set objs [lrange $allObj 1 end]
      foreach o $objs {
        set fieldInfo($x,$tbName,desc,$i,ptr) $o
        # comprobamos si es editable, de serlo le activamos el control del
        # texto introducido
        if {[info exists fieldInfo($x,$tbName,desc,$i,edit)] && \
            [info exists fieldInfo($x,$tbName,desc,$i,type)]} { 
          if {$fieldInfo($x,$tbName,desc,$i,edit)} {
            $o configure -validate key -vcmd \
              [list SqlValidateData %P $fieldInfo($x,$tbName,desc,$i,type)]
          }    
        }
        incr i
      }
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method CreateWidget {path index tbName} {
  # PURPOSE:
  # PARAMETERS:
  # RETURN:
  #///////////////////////////////////////////////////////////////////////////
    global toltk_options

    #TODO: controlar el tipo de objeto que hay que empacar
    set typeField [string tolower [lindex $fieldInfo($index,$tbName,field,type) 0]]
    set ptr $path.entry$index$tbName
    set lab $fieldInfo($index,$tbName,field,name)
    set typ $fieldInfo($index,$tbName,field,type)

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
        set longVar [lindex $fieldInfo($index,$tbName,field,type) 1]
        set fc [string first , $longVar]
        if {[expr $fc ne -1]} {
         set longVar [string range $longVar 0 [expr $fc-1]] 
        }
        if { ![string length $longVar] || [expr $longVar <= 255] } {
          Entry $ptr -editable false \
            -helptext "$lab ($typ)" -validate key \
            -background $toltk_options(colors,SystemButtonFace) \
            -vcmd [list SqlValidateData %P $fieldInfo($index,$tbName,field,type)]
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
  method botsel {tbName args} {
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
      if {[$self FillFieldKey $tbName]} {
        # construimos sentencia
        set select "select [join [$self GetLstFieldsTable $tbName] ,]"
        set from   "from $tbName"
        set where  "where [$self GetSqlWhere "" -1 $tbName ]"
        set txtSql "$select \n $from \n $where"

        # mostramos valores en el formulario
        $self SetValuesFields $txtSql $tbName
        # mostramos descripciones de los campos
        $self SetValuesFieldsDesc $tbName
        # cambiamos de estado  
        $self setStatus "vis"
        
        # Recargamos la informacion de los tabs.
        $self reloadTabWidgets $child(actualTbName)
        
      } 
    } elseif {$status=="ins" || $status=="edi"} {
#TODO: comprobar si en este estado hay que hacer algo
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method childLoad {tbName fieldName args} {
  #
  # PURPOSE: Load children widgets with info associated to value selected in
  #          parent.
  #
  # PARAMETERS:
  #   tbName -> Nombre de la tabla hija
  #   fieldName -> Nombre de la columna hija asociada al valor FK de la padre.
  #   current_index -> Index to the RegComboBox "matrix".
  #///////////////////////////////////////////////////////////////////////////
    set current_index [lindex $args 0]
    # construimos sentencia
    set select "select [join [$self GetLstFieldsTable $tbName] ,]"
    set from   "from $tbName"
    set where  "where $fieldName='$child(selectedValue)'" 
    set txtSql "$select \n $from \n $where"
    # mostramos valores en el formulario
    $self SetValuesFieldsChild $txtSql $tbName
    # mostramos descripciones de los campos
    $self SetValuesFieldsDesc $tbName
    # cambiamos de estado  
    #$self setStatus "vis"
  }

  #///////////////////////////////////////////////////////////////////////////
  method SetValuesFieldsChild {txtSql tbName} {
  # PURPUSE: Rellenamos los objetos del tab de tbName con sus valores
  #
  # PARAMETERS:
  #      txtSql -> sentencia SQL que rellena la informacion del btable .
  #      tbName -> Nombre de la tabla hija cuya informacion hay que rellenar.
  #///////////////////////////////////////////////////////////////////////////
 
    set db [[$self cget -bodbcmodule] cget -connection]
    $db statement stmt $txtSql
    $widget($tbName,visutable) fillSQL stmt    
  }



  #///////////////////////////////////////////////////////////////////////////
  method GetValuesShow {index tbName} {
  # PURPUSE: funcion a previa a ejecutar al mostrar la lista de valores de un 
  #          bcomboboxplus
  # PARAMETERS:
  #       index -> registro activo
  #       tbName -> nombre de la tabla a la que se aplica
  #///////////////////////////////////////////////////////////////////////////
    
    # para visualizacion de datos de la propia tabla
    if {$status=="sel" || $status=="vis"} {
      set where ""
      set whe [$self GetSqlWhere ori. $index $tbName]
      if {[string length $whe]} {
        set where "where $whe \n"
      }
      if {[string equal $tbName $fieldInfo($index,$tbName,desc,table)]} {
#Tolcon_Trace "GetValuesShow (1)"
        # informacion de la misma tabla
        set txtSql \
          "select distinct [join $fieldInfo($index,$tbName,desc,lstfields) ,] \n\
           from   $tbName ori \n\
           $where \
           order by [lindex $fieldInfo($index,$tbName,desc,lstfields) 0]"
      } else  {
        # informacion de la tabla referenciada
        foreach f $fieldInfo($index,$tbName,desc,lstfields) {
          lappend fie "des.$f"
        }
        # unimos las tablas pro sus foreing key y campos descripcion
        
        set wheJoin [$self GetJoin $index \
                                   $fieldInfo($index,$tbName,desc,table) \
                                   ori. \
                                   des. \
                                   $tbName]
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
          from $fieldInfo($index,$tbName,desc,table) des, $tbName ori \n\
          $where \
          order by des.[lindex $fieldInfo($index,$tbName,desc,lstfields) 0]"
      }
    } elseif {$status=="edi" || $status=="ins"} {
      # valores de las tablas realcionadas
#Tolcon_Trace "GetValuesShow (3)"      
      set txtSql [$self GetSelectFk $index $tbName]
    }
    # asignamos valor al bselectfield correspondiente
#Tolcon_Trace "GetValuesShow: Consulta:\n $txtSql"
    set lst [$connection $txtSql]
    $fieldInfo($index,$tbName,field,ptr) configure -values $lst
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
  method GetSqlWhere {prefTable {index -1} tbName} {
  # PURPUSE: Construct the WHERE clause
  # PARAMETERS: index -> indice a excluir de la seleccion
  #             prefTable -> nombre de la tabla a concatenar al campo
  # RETURN:
  #
  #///////////////////////////////////////////////////////////////////////////
  #  $fieldInfo($x,$tbName,field,ptr)
    variable child

    set where {}
    for {set x 1} {$x<=$child($tbName,maxFields)} {incr x} { 
      if {$fieldInfo($x,$tbName,field,iskey)} {
        if {([info exists fieldInfo($x,$tbName,field,ptr)])} {
          if {[set code [$fieldInfo($x,$tbName,field,ptr) key get]] != "" && $x != $index} {
            if { [string match $tablename $tbName] } {
              set child(selectedField) $fieldInfo($x,$tbName,field,name)
              set child(selectedValue) $code
            }
            lappend where "$prefTable$fieldInfo($x,$tbName,field,name) = \
               [SqlFormatField $code $fieldInfo($x,$tbName,field,type) $gestor]\n"
          }  
        }
      }  
    }
    return [join $where { and }]
  }

  #///////////////////////////////////////////////////////////////////////////
  method GetSelectFk {index tbName} {
  # PURPUSE:
  # PARAMETERS: index -> indice selecionado
  #             tbName-> Nombre de la tabla
  # RETURN: la sentencia sql a ejecutar para obtener los resultados a mostrar
  #
  #///////////////////////////////////////////////////////////////////////////

    set txtSql ""
    # control de foreing key existente
    if {[info exists fieldInfo($index,$tbName,field,fkMRes)]} {
      set inxFk $fieldInfo($index,$tbName,field,fkMRes)
      # control de foreing key a la tabla de la descripcion
      if {[string equal $fieldInfo($index,$tbName,desc,table) $fieldFkInfo($inxFk,table)]} {
        set where [$self GetSqlWhereFk $index "" and $tbName]
        if {[string length $where]} {
          set where "\n where $where"
        }
        set txtSql \
          "select distinct [join $fieldInfo($index,$tbName,desc,lstfields) ,] \n\
           from $fieldFkInfo($inxFk,$tbName,table) \
           $where \
           order by [lindex $fieldInfo($index,$tbName,desc,lstfields) 0]"
      } else  {
        set where {}
        # informacion de la tabla referenciada
        foreach f $fieldInfo($index,$tbName,desc,lstfields) {
          lappend fie "des.$f"
        }
        # claupsula where
        set join {}
        set lon [llength $fieldFkInfo($inxFk,$tbName,indFie)]
        for {set x 0} {$x<$lon} {incr x} {
          if {[lindex $fieldFkInfo($inxFk,$tbName,indFie) $x]==$index} {
            set join "des.$fieldInfo($index,$tbName,desc,join) = \
                      ori.[lindex $fieldFkInfo($inxFk,$tbName,fields) $x]"
          }
        }
        if {[string length $join]} {
          set where "\n where $join"
        }
        set whe {}
        set whe [$self GetSqlWhereFk $index des. and $tbName]
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
           from $fieldInfo($index,$tbName,desc,table) des, \
                $fieldFkInfo($inxFk,$tbName,table) ori \
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
  method GetSqlWhereFk {index prefTable {opeLog and} tbName} {
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
    if {[info exists fieldInfo($index,$tbName,field,fkMRes)]} {
      set lstId $fieldFkInfo($fieldInfo($index,$tbName,field,fkMRes),$tbName,indFie)
      if {[llength $lstId]>1} {
        set i 0
        foreach id $lstId {
#Tolcon_Trace "GetSqlWhereFk: If $id <> $index"
          if {[expr $id != $index] && \
              [string length [set code [$fieldInfo($index,$tbName,desc,join) key get]]]} {
#Tolcon_Trace "GetSqlWhereFk: I: $i"
#Tolcon_Trace "Lista de campos referenciados:\
      $fieldFkInfo($fieldInfo($index,$tbName,field,fkMRes),fields)"
            lappend where \
              "$prefTable\
               [lindex $fieldFkInfo($fieldInfo($index,$tbName,field,fkMRes),$tbName,fields) $i] = \
               [SqlFormatField $code $fieldInfo($id,$tbName,field,type) $gestor] \n"
#Tolcon_Trace "GetSqlWhereFk: Campo en el where: [lindex $fieldFkInfo($fieldInfo($index,$tbName,field,fkMRes),$tbName,fields) $i]";#"
          }
          incr i
        }
      }
    }
    return [join $where " $opeLog "]
  }

  #///////////////////////////////////////////////////////////////////////////
  method FillFieldKey {tbName} {
  # PURPUSE: comprobamos que esten rellenos todos los campos clave
  #
  # RETURN: true -> SI, false -> NO
  #
  #///////////////////////////////////////////////////////////////////////////
    set fill true
    set x    1
    while {$fill && [expr $x <=$child($tbName,maxFields)]} {
      if {$fieldInfo($x,$tbName,field,iskey)} {
#Tolcon_Trace "FillFieldKey $fieldInfo($x,$tbName,field,ptr) = [$fieldInfo($x,$tbName,field,ptr) GetCode]"
        if {[string equal [$fieldInfo($x,$tbName,field,ptr) key get] ""]} {
          set fill false
#Tolcon_Trace "FillFieldKey: Por rellenar el campo: $fieldInfo($x,$tbName,field,name)"
        }
      }
      incr x
    }
    return $fill
  }

  #///////////////////////////////////////////////////////////////////////////
  method GetLstFieldsTable {tbName} {
  # PURPUSE:
  #
  # RETURN:
  #
  #///////////////////////////////////////////////////////////////////////////
#Tolcon_Trace "GetLstFieldsTable ..."
    set lst {}
    for {set x 1} {$x<=$child($tbName,maxFields)} {incr x} {
      lappend lst $fieldInfo($x,$tbName,field,name)
      if {$fieldInfo($x,$tbName,desc,show)} { 
        for {set i 1} {$i<=[llength $fieldInfo($x,$tbName,desc,lstshow)]} {incr i} {
          if {$fieldInfo($x,$tbName,desc,$i,edit)} {
            lappend lst $fieldInfo($x,$tbName,desc,$i,name)
          }
        }
      }
    }
    return $lst
  }

  #///////////////////////////////////////////////////////////////////////////
  method boteli {tbName} {
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
      if {[$self FillFieldKey $tbName]} {
        set where [$self GetSqlWhere "" -1 $tbName]
        set txtSql "delete from $tbName \n\
                    where $where"
#Tolcon_Trace "boteli: Sentencia de borrado: $txtSql"
        #TODO: comprobamos el valor de OK
        set ok [$connection $txtSql]
        #call cancel:
        $self botcan $tbName
      }
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method botedi {tbName} {
  # PURPOSE:
  #///////////////////////////////////////////////////////////////////////////
    # Guardamos los valores previos a modificaciones
    $self SetOldValues $tbName
    set childName $tbName
    set child(actualTbName) $tbName
    $self setStatus "edi"
  }

  #///////////////////////////////////////////////////////////////////////////
  method botcan {tbName} {
  # PURPOSE:
  #///////////////////////////////////////////////////////////////////////////
    #Simply clean up and return to "select" status.
    BfoCleanPath $widget(tf,pk)
    BfoCleanPath $widget(tf,data)
    # load old data
    $self SetDataFomOldValues
    $self SetValuesFieldsDesc $tbName
    # load old values
    set childName $tbName
    set child(actualTbName) $tbName
    $self setStatus "sel"
    $self botsel $tbName   
  }

  #///////////////////////////////////////////////////////////////////////////
  method botnew {tbName} {
  #
  # PURPOSE: Actions to do when the "New" button is pressed.
  #
  #///////////////////////////////////////////////////////////////////////////
    $self SetOldValues $tbName
    #Clear the form:
    BfoCleanPath $widget(tf,pk)
    BfoCleanPath $widget(tf,data)
    #Change status:
    set childName $tbName
    set child(actualTbName) $tbName
    $self setStatus "ins"
  }

  #///////////////////////////////////////////////////////////////////////////
  method botgua {tbName} {
  # PURPOSE:
  #///////////////////////////////////////////////////////////////////////////
    if {$status=="edi" || $status=="ins"} {
      set ok 1
      # comprobamos rellenos todos los campos key y notnull
      if {[$self FillFieldKey $tbName]} {
        if {![$self FillFieldNotNull $tbName]} {
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
            set txtSql [$self GetSqlInsert $tbName]
          }
          "edi" {
            set txtSql [$self GetSqlUpdate $tbName]
          }
        }

        # ejecutamos sentencia
#Tolcon_Trace "botgua: Sentencia de insercion o actualizacion:\n $txtSql"
        set result [$connection $txtSql]
        if {$result>=0} {
          set childName $tbName
          set child(actualTbName) $tbName
          $self setStatus "vis"
        } else  {
          #TODO: mensaje de error
        }
      }
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method GetSqlInsert {tbName} {
  # PURPOSE: construimos la sentencia de insercion
  # RETURN: Sentencia de insercion 
  #///////////////////////////////////////////////////////////////////////////
    set lstField {}
    set lstValues {}
    #Read each combo and text value, and add it to the query:
    for {set x 1} {$x<=$maxFields} {incr x} {
      lappend lstField $fieldInfo($x,$tbName,field,name)
      set txt [BfoGetText $fieldInfo($x,$tbName,field,ptr)]
      if {[string length $txt]} {
        set txt [StringReplace $txt ' ''] 
      } else {
        set txt null
      }
      lappend lstValues [SqlFormatField $txt $fieldInfo($x,$tbName,field,type) $gestor]
      if {$fieldInfo($x,$tbName,desc,show)} {
        for {set i 1} {$i<=[llength $fieldInfo($x,$tbName,desc,lstshow)]} {incr i} {
          if {$fieldInfo($x,$tbName,desc,$i,edit)} {
            lappend lstField $fieldInfo($x,$tbName,desc,$i,name)
            set obj $fieldInfo($x,$tbName,desc,$i,ptr)
            set txt [BfoGetText $obj]
            if {![string length $txt]} {
              set txt null
            }
            lappend lstValues [SqlFormatField $txt $fieldInfo($x,$tbName,desc,$i,type) $gestor]
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
  method SetDataFomOldValues {} {
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
  method GetJoin {index tableDes prefOri prefDes tbName} {
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
    if {[info exists fieldInfo($index,$tbName,field,fkMRes)]} {
      set fkMRes $fieldInfo($index,$tbName,field,fkMRes)
      if {[string equal $tableDes $fieldFkInfo($fkMRes,table)]} {
        set i 0
        foreach id $fieldFkInfo($fkMRes,$tbName,indFie) {
          lappend wheJoin "$prefOri$fieldInfo($id,$tbName,field,name) = \
                           $prefDes[lindex $fieldFkInfo($fkMRes,$tbName,fields) $i]"
          incr i
        }
      } else {
        set ok 1
      }
    } else {
      set ok 1 
    }  
    if {$ok} {
      lappend wheJoin "$prefOri$fieldInfo($index,$tbName,field,name) = \
                     $prefDes$fieldInfo($index,$tbName,desc,join)" 
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
    variable child

    # Definimos el valor de tbName, si tenemos tabla hija, lo cargamos con ella.    
    if {![info exists childName] || ([string length $childName] == 0)} {
        set tbName $tablename  
    } else {
        set tbName $childName      
    }
    
    #Set widget status:
    switch $status {
      "sel" {
        switch $new {
          "sel" {
            #Gets this on every launch:
            #Tolcon_Trace "SetStatus: Change from select to select status."
            # solo se pueden seleccionar los campos con lista de la primary key
            for {set x 1} {$x<=$child($tbName,maxFields)} {incr x} {
              $self ActivateWidget $x $new $tbName
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
            for {set x 1} {$x<=$child($tbName,maxFields)} {incr x} {
              $self ActivateWidget $x $new $tbName
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
            for {set x 1} {$x<=$child($tbName,maxFields)} {incr x} {
                $self ActivateWidget $x $new $tbName
            }
          }
          "ins" {
            #set editable keys combos
            #enable entries
            for {set x 1} {$x<=$child($tbName,maxFields)} {incr x} {
                $self ActivateWidget $x $new $tbName
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
            for {set x 1} {$x<=$child($tbName,maxFields)} {incr x} {
               $self ActivateWidget $x $new $tbName
            }
          }
          "vis" {
            #enable keys combos
            #disable entries
            focus -force $self
            for {set x 1} {$x<=$child($tbName,maxFields)} {incr x} {
               $self ActivateWidget $x $new $tbName
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
            for {set x 1} {$x<=$child($tbName,maxFields)} {incr x} {
               $self ActivateWidget $x $new $tbName
            }
          }
          "vis" {
            focus -force $self
            #"select" keys combos
            #disable entries (set to readonly)
            for {set x 1} {$x<=$child($tbName,maxFields)} {incr x} {
              $self ActivateWidget $x $new $tbName
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
    #    $widget(bt,duplicate) configure -state disable
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
    #    $widget(bt,duplicate) configure -state normal
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
    #    $widget(bt,duplicate) configure -state normal
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
    #    $widget(bt,duplicate) configure -state normal
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
    #pack $widget(bbox)    -side bottom -fill x -expand no
    #pack $widget(tf,pk)   -side top    -fill x -expand yes
    #pack $widget(tf,data) -side top    -fill x -expand yes 
    #pack $self.sw         -side top    -fill x -expand yes


    #grid $self.sw         -sticky news
   
    grid $widget(tf,pk)    -sticky news
    grid $widget(tf,data)  -sticky news
    grid $widget(fr,extra) -sticky news

    grid columnconfigure $widget(frc) 0  -weight 1
    grid columnconfigure $widget(frc) 10 -weight 0
    grid rowconfigure    $widget(frc) 0  -weight 1
    grid rowconfigure    $widget(frc) 1  -weight 6
    grid rowconfigure    $widget(frc) 3  -weight 0
    
    grid $widget(frc)     -sticky news    
    
    grid $widget(bbox)    -sticky ew
    
    #grid columnconfigure $self.sw 0  -weight 1
    #
    #grid rowconfigure    $self.sw 0 -weight 1
    #grid rowconfigure    $self.sw 2 -weight 1
    #grid rowconfigure    $self.sw 3 -weight 1
    #grid rowconfigure    $self.sw 10 -weight 0
    #
    grid columnconfigure $widget(tf,pk) 0 -weight 1
    grid columnconfigure $widget(tf,pk) 1 -weight 1
    grid columnconfigure $widget(tf,pk) 10 -weight 0
    grid rowconfigure    $widget(tf,pk) 0 -weight 1
    grid rowconfigure    $widget(tf,pk) 10 -weight 0
    #
    grid columnconfigure $widget(tf,data) 0 -weight 1
    grid columnconfigure $widget(tf,data) 10 -weight 0
    grid rowconfigure    $widget(tf,data) 0 -weight 1
    grid rowconfigure    $widget(tf,data) 10 -weight 0
    

    #pack $widget(tf,pk)    -fill both -expand yes
    #pack $widget(tf,data)  -fill both -expand yes
    #pack $widget(fr,extra) -fill both -expand yes
    #pack $widget(bbox)  -side bottom -fill x    -expand no
    
    #grid $widget(frc) -sticky news

    grid columnconfigure $self 0 -weight 1
    grid columnconfigure $self 10 -weight 0
    grid rowconfigure    $self 0 -weight 1
    grid rowconfigure    $self 10 -weight 0
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
  method FillFieldNotNull {tbName} {
  # PURPUSE: comprobamos que esten rellenos todos los campos obligatorios
  #
  # RETURN: true -> SI, false -> NO
  #
  #///////////////////////////////////////////////////////////////////////////
#Tolcon_Trace "FillFieldNotNull ..."  
    set fill true
    set x    1
    while {$fill && [expr $x<=$child($tbName,maxFields)]} {
      if {$fieldInfo($x,$tbName,field,notnull)} {
        if {$fieldInfo($x,$tbName,desc,show)} {
          for {set i 1} {$i<=[llength $fieldInfo($x,$tbName,desc,lstshow)]} {incr i} {
            if {[info exists fieldInfo($x,$tbName,desc,$i,notnull)] && \
                $fieldInfo($x,$tbName,desc,$i,notnull)} {
              set txt  [BfoGetText $fieldInfo($x,$tbName,desc,$i,ptr)]
              if {![string length $txt]} {
                set fill false
              }
            }
          }
        } else {
          set txt  [BfoGetText $fieldInfo($x,$tbName,field,ptr)]
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
  method SetValuesFields {txtSql tbName} {
  # PURPUSE: Rellenamos los objetos del formulario con sus valores
  #
  # PARAMETERS: 
  #
  #///////////////////////////////////////////////////////////////////////////
  global tcl_platform
#Tolcon_Trace "SetValuesFields: Consulta: $txtSql"
    set values [$connection $txtSql]
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
      set index [$self GetIndexArray field,name $nc $tbName]
      if {[expr $index != -1]} {
        if {!$fieldInfo($index,$tbName,field,iskey)} {
          set val [lindex [lindex $values 0] $c]
          $self SetValue $fieldInfo($index,$tbName,field,ptr) $val
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
  method SetValuesFieldsDesc {tbName} {
  #
  # PURPOSE: Cargar las descripciones de los campos.
  #
  #///////////////////////////////////////////////////////////////////////////
  
   # Solo se muestran los controles para la tabla padre.
   if { [string equal $tbName $tablename] } {
    for {set x 1} {$x<=$child($tbName,maxFields)} {incr x} {
      # si no es clave y tiene descripcion
      if {$fieldInfo($x,$tbName,desc,show)} {
        if {[llength $fieldInfo($x,$tbName,desc,lstshow)]} {                
          set txt [BfoGetText $fieldInfo($x,$tbName,field,ptr)]
          if {[string length $txt]} {
            # construimos sentencia
            set txtSql \
              "select [join $fieldInfo($x,$tbName,desc,lstshow) ,] \n\
               from $fieldInfo($x,$tbName,desc,table) \n\
               where $fieldInfo($x,$tbName,desc,join) = \
                 [SqlFormatField $txt $fieldInfo($x,$tbName,field,type) $gestor]"
            # ejecutamos sentencia
              set values [$connection $txtSql]
            # rellenamos valores
            for {set i 1} {$i<=[llength $fieldInfo($x,$tbName,desc,lstshow)]} {incr i} {
              $self SetValue $fieldInfo($x,$tbName,desc,[expr $i +1],ptr) \
               [lindex [lindex $values 0] [expr $i-1]]
            }
          }
        }
      }
    }
   }
  }

  #///////////////////////////////////////////////////////////////////////////
  method GetIndexArray {key value tbName} {
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
    while {[expr $index == -1] && [expr $x <= $child($tbName,maxFields)]} {
      if {[info exists fieldInfo($x,$tbName,$key)]} {
        if {[string equal [string toupper $fieldInfo($x,$tbName,$key)] [string toupper $value]]} {
          set index $x
        }
      }
      incr x
    }
#Tolcon_Trace "GetIndexArray. result: $index"    
    return $index
  }

  #///////////////////////////////////////////////////////////////////////////
  method GetSqlUpdate {tbName} {
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
    for {set x 1} {$x<=$child($tbName,maxFields)} {incr x} {
      set txt [BfoGetText $fieldInfo($x,$tbName,field,ptr)]
      if {[string length $txt]} {
        set txt [StringReplace $txt ' '']
      } else {
        set txt null
      }
      lappend lstValues "$fieldInfo($x,$tbName,field,name) = \
        [SqlFormatField $txt $fieldInfo($x,$tbName,field,type) $gestor]"
      if {$fieldInfo($x,$tbName,desc,show)} {
        for {set i 1} {$i<=[llength $fieldInfo($x,$tbName,desc,lstshow)]} {incr i} {
          if {$fieldInfo($x,$tbName,desc,$i,edit)} {
            set obj $fieldInfo($x,$tbName,desc,$i,ptr)
            set txt [BfoGetText $obj]
            if {![string length $txt]} {
              set txt null
            }
            lappend lstValues "$fieldInfo($x,$tbName,desc,$i,name) = \
              [SqlFormatField $txt $fieldInfo($x,$tbName,desc,$i,type) $gestor]"
          }  
        }
      }
    }
    # construir sentencia sql
    set txtSql \
      "update $tbName set \n\
       [join $lstValues ,] \n\
       where $where" 
    return $txtSql
  }

  #///////////////////////////////////////////////////////////////////////////
  method SetOldValues {tbName} {
  # PURPUSE: Almacenamos en una variable los valores actuales, antes de 
  #          modificarlos
  #
  #///////////////////////////////////////////////////////////////////////////
    set n 0
    for {set x 1} {$x<=$child($tbName,maxFields)} {incr x} {
      if {$fieldInfo($x,$tbName,field,iskey)} {
        incr n
        set oldValues($n,name)  $fieldInfo($x,$tbName,field,name)
        set oldValues($n,type)   $fieldInfo($x,$tbName,field,type)
        set oldValues($n,value) [BfoGetText $fieldInfo($x,$tbName,field,ptr)]
      }
    }
    set oldValues(numReg) $n
  }

  #///////////////////////////////////////////////////////////////////////////
  method ActivateWidget {index state tbName} {
  #
  # PURPOSE: Activa o desactiva los widgets según el estado que se lea,
  #          en base a las directrices de los estados de bchilddetail.
  #
  # PARAMETERS:
  #   widget -> objeto a activar/desactivar
  #   state   -> Estado.
  #
  #///////////////////////////////////////////////////////////////////////////
    variable child

    global toltk_options
    #set w $fieldInfo($index,$tbName,field,ptr)
    #set type [winfo class $w]

    switch $type {
      "BSelectFields" {
        switch $state {
          "sel" -
          "vis" {
            if {$fieldInfo($index,$tbName,field,iskey)} {
              set stt "select"
            } else {
              set stt "notactive"
            }
     #       $w Activate $stt
          }
          "edi" -
          "ins" {
#TODO: entiendo que es igual. Hay que decidir si en edicion se puede cambiar
#      el code de un campo (este es el codigo que se especifica en la tabla)
#      En estos momentos se pueden aztualizar todos los campos
            if {$fieldInfo($index,$tbName,field,iskey)} {
              if {$fieldInfo($index,$tbName,field,edit)} {
                set stt "normal"
              } else  {
                set stt "select"
              }
    #          $w Activate $stt
              if {$fieldInfo($index,desc,show)} {
                for {set i 1} {$i<=[llength $fieldInfo($index,$tbName,desc,lstshow)]} {incr i} {
                  if {$fieldInfo($index,desc,$i,edit)} {
                    set stt "active"
                    BfoActivateWidget $fieldInfo($index,desc,$i,ptr) $stt
                  }
                }
              }
            } else {
              set stt "select"
      #        $w Activate $stt
            }
          }
        }
      }
      "Bcomboboxplus"
      {
        switch $state {
          "sel" -
          "vis" {
            if {$fieldInfo($index,$tbName,field,iskey)} {
              set stt "select"
            } else {
              set stt "notactive"
            }
        #    $w activate $stt
          }
          "edi" -
          "ins" {
            if {$fieldInfo($index,$tbName,field,iskey)} {
              if {$fieldInfo($index,$tbName,field,edit)} {
                set stt "normal"
              } else  {
                set stt "select"
              }
#              $w activate $stt
              if {$fieldInfo($index,desc,show)} {
                for {set i 1} {$i<=[llength $fieldInfo($index,$tbName,desc,lstshow)]} {incr i} {
                  if {$fieldInfo($index,desc,$i,edit)} {
                    set stt "active"
                    BfoActivateWidget $fieldInfo($index,desc,$i,ptr) $stt
                  }
                }
              }
            } else {
              set stt "select"
       #       $w activate $stt
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
#        $w configure -editable $stt
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
    #        $w configure -background $toltk_options(colors,SystemButtonFace)
          }
          "edi" -
          "ins" {
            set stt "true"
#            $w configure -background white
          }
        }
    #    $w configure -editable $stt
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
#        $w configure -bstate $stt
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
#        $w configure -state $stt
      }
      "Text"
      {
        #Tolcon_Trace "prueba nueva ActivateWidget TEXT"
        switch $state {
          "sel" -
          "vis" {
            set stt "disabled"            
    #        $w configure -background $toltk_options(colors,SystemButtonFace) \
                         -foreground darkgray
          }
          "edi" -
          "ins" {
            set stt "normal"
    #        $w configure -background white -foreground black
          }
        }
#        $w configure -state $stt
      }
      default
      {
        Tolcon_Trace "ActivateWidget: Widget type not recognized: $type"
      }
    }
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
  method botabr {tbName} {
  #
  # PURPOSE: Shows table
  #
  #///////////////////////////////////////////////////////////////////////////
    set table $tbName
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
    
    
    #grid $tlf.t -sticky news
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
      set tbName $child($x,tableName)
      return $fieldInfo($x,$tbName,field,ptr)
    } else {
      set inx [lsearch -exact [$self GetLstFieldsTable $tablename] $x]
      if { $inx != -1 } {
        incr inx
        return $fieldInfo($inx,$tbName,field,ptr)
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
  method showvalue { tbName args } {
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
    $self botedi $tbName
    $self botcan $tbName
    $self botref    
    set ok true
    switch $opts(-status) {
      sel -
      vis     {  }
      edi     { $self botedi $tbName}
      ins     { $self botnew $tbName}
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
      $self SetValuesFieldsDesc $tbName
      
      # status end
      set ok true
      switch $opts(-status) {
        sel     { $self botsel $tbName }
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
  
  
  #///////////////////////////////////////////////////////////////////////////
  method InitTable { tbName } {
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
    set table $tbName
    set tolGes [SqlGetTolGestor $gestor]

    # Obtenemos informacion de los campos
    set infoColumns [GetGlobalUniqueName __infoColumns[clock second]]
    set cmdIC infoColums[clock clicks]
    set cmdTIC "Set $infoColumns = SqlInfoColumns(\"$table\", $tolGes);"
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
    set cmdTIF "Set $infoForeignKeys = SqlInfoForeign(\"$table\", $tolGes);"
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
      set regFields($index,$tbName,field,name) \
        [string trim [$cmdIC data get $fila 1] {"}] ;#"
      set regFields($index,$tbName,field,label) [mc "$regFields($index,$tbName,field,name)"]
    
      set type   [string trim [$cmdIC data get $fila 2] {"}];#"
    
      # Longitud. Si el campo "Precision" es igual a "?" nos quedamos con el
      # valor del campo "Length"
      set length [$cmdIC data get $fila 4]
      if {[string eq $length ?]} {
        set length [$cmdIC data get $fila 3]
      }
   
      set scale  [$cmdIC data get $fila 5]
    
      if {[string equal $scale ?] || ![string length $scale]} {
        set typeCol [list $type $length]
      } else {
        set typeCol [list $type $length,$scale]
      }
      set regFields($index,$tbName,field,type)     $typeCol
      set regFields($index,$tbName,field,iskey)    [$cmdIC data get $fila 7]

      set regFields($index,$tbName,field,notnull)  [expr ![$cmdIC data get $fila 6]]
    
      # Informacion por defecto para campos clave
      if {$regFields($index,$tbName,field,iskey)} {
        set regFields($index,$tbName,desc,table)    $table
        set regFields($index,$tbName,desc,lstshow)  {}
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
      set regFields($indCode,$tbName,desc,table) $table
    
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
        set regFields($indCode,$tbName,desc,lstselect) $auxDescCode
        set data(field,description) $regFields($indCode,$tbName,desc,lstselect)        
#Tolcon_Trace "EditMasterUtils::InitTable. regFields($indCode,$tbName,desc,lstselect) : $regFields($indCode,$tbName,desc,lstselect)"
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
      
        set namCA [string trim [$cmdIF data get $fila 2] {"}];#"
        # buscamos el campo en la tabla
        set indCol [lsearch $lstInfTabFil $namCA]  
        set rowCol \
          [lindex [split [lindex $lstInfTab [expr $indCol * 2]] ,] 0]
#Tolcon_Trace "EditMasterUtils::InitTable. rowCol: $rowCol"      
        # Informacion de la tabla de los campos ajenos  
        set regFields($rowCol,$tbName,field,fkMRes)   1
      
        set namTab [string trim [$cmdIF data get $fila 3] {"}];#"
        set regFields($rowCol,$tbName,field,links) [$self GetLinks $table $namTab]
        
#OJO. AÑADIMOS INFORMACION SOBRE LOS CAMPOS EXTERNOS DE LA TABLA, QUE NO SON
#CLAVES EXTERNAS DE PRIMER NIVEL
        set lstTableFieldForeing \
          [$self _GetTableParent $namTab [string trim [$cmdIF data get $fila 5] {"}]];#"
        set regFields($rowCol,$tbName,desc,table) [lindex [lindex $lstTableFieldForeing 0] 0]
        set regFields($rowCol,$tbName,desc,join)  [lindex [lindex $lstTableFieldForeing 0] 1]
#Tolcon_Trace "EditMasterUtils::InitTable. lstTableFieldForeing: $lstTableFieldForeing"                                         

        set regFields($rowCol,$tbName,desc,show) true                                                
        set regFields($rowCol,$tbName,desc,lstshow) \
          [$self GetFieldDescForeignKey \
            $table \
            $regFields($rowCol,$tbName,desc,table) \
            $regFields($rowCol,$tbName,desc,join)]
      }      
    }
    
#Tolcon_Trace "valor dado a regFields(4,desc,lstshow): $regFields(4,desc,lstshow)"

    # construimos las referencias externas
    if {[expr $nRegFk > 0]} {
      # valores iniciales
      set fila     0
      set index    0
      # lectura adelantada
      set namFk    [string trim [$cmdIF data get $fila 1] {"}];#"
      set namFkFij {}
      while {[expr $fila < $nRegFk]} {
        if {![string eq [string tolower $namFk] [string tolower $namFkFij]]} {  
          incr index
          # introducimos tabla
          set regFk($index,table) [string trim [$cmdIF data get $fila 3] {"}];#"
          set namFkFij $namFk
        }  
        # buscamos el indice del campo al que asociar esta restricion
        set valFie [string trim [$cmdIF data get $fila 2] {"}];#"
        set id [lsearch $lstInfTabFil $valFie]
        if {[expr $id > -1]} {
          # buscamos el indice del campo codigo
          set inx [lindex [split [lindex $lstInfTab [expr $id * 2]] ,] 0]
          lappend regFk($index,$tbName,indFie)     $inx
          set regFields($inx,$tbName,field,fkMRes) $index
        }
        # guardamos valores
        lappend regFk($index,$tbName,fields) \
            [string trim [$cmdIF data get $fila 5] {"}];#"
           
        # preparamos siguiente paso
        incr fila
        if {[expr $fila < $nRegFk]} {
          set namFk [string trim [$cmdIF data get $fila 1] {"}];#"
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
      set cmdTIC "Set $infoColumns = SqlInfoColumns(\"$tableFk\", $tolGes);"
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

package provide bchilddetail 1.0