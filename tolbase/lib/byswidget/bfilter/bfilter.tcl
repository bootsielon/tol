#/////////////////////////////////////////////////////////////////////////////
# FILE    : bfilter.tcl
# PURPOSE : Bayes Filtro. Componente para generar expresiones de filtrado
#           sobre una estructura.
#
#           La estructura es una lista de listas con el siguiente formato:
#
#              Campo:       Code referencia del campo
#              Tipo:        Tipo de campo (Date, Text, Real)
#              Descripcion: Descripción del campo
#              Lenguaje:    Tipo de lenguaje (tol, sql, tcl)
#              Funcion:     Función de carga de valores (lista ó procedure)
#
# example:
# [list [list code1 type1  {val1 val2} {funtion1 param1} {description1}] \
#       [list code2 type2  {val1 val2} {funtion2 param1} {description2}]]
#
# Nota --> Si le pasamos una lista de valores y una función la función no se
#          evaluará utilizando como valores posibles la lista que recibe
#
#/////////////////////////////////////////////////////////////////////////////

package require byscommon
package require BLT
package require blistboxplus
package require btreeviewplus
package require bcomboboxplus
package require bfunctions
package require bentrydate
package require bcombobox
package require beditselector
package require Toltcl

#/////////////////////////////////////////////////////////////////////////////
::snit::widget bfilter {
#
# PURPOSE: Componente para construir filtros
#
# -values   ->   List of initial values to load in widget
# -language ->   Lenguaje para el que se realiza el bfilter <tol / sql>
# -geometry ->   Lista con separación de cada uno de los paneles desde la
#                parte izquierda de cada panel. Debe haber 2 elementos
# -showbuttons -> Muestra una barra de botones con Ok/Cancel
# -okcommand -> Procedimiento a ejecutar cuando se pulse el boton OK
# -cancelcommand -> Procedimiento a ejecutar cuando se pulse el boton Cancel
#/////////////////////////////////////////////////////////////////////////////

  widgetclass Bfilter
  
  option -values {}
  option -language {}
  option -geometry {250 350}
  option -showbuttons 0
  option -okcommand ""
  option -cancelcommand ""

  variable lstOpeLogic
  variable isEntry
  variable widget
  variable date
  variable fVal1
  variable fVal2
  variable lstFinal
  variable lstValuesComp
  variable language
  variable defaultOperator
  
  
  
  #///////////////////////////////////////////////////////////////////////////
  constructor {args} {
  #
  # PURPOSE: Constructor de un bfilter. Crea una instancia de un Bayes Filtro
  #
  # PARAMETERS:
  #   args -> Lista de opciones especificas de un Bayes Filtro
  #
  #///////////////////////////////////////////////////////////////////////////
    set widget(pwM) [PanedWindow $self.pwt]
    set f0 [$widget(pwM) add -weight 1]
    set f1 [$widget(pwM) add -weight 2]
    
    # Create Fields ListBox  
    set widget(fields)   [$self CreateLBF     $f0]
    # Create Options Title Frame
    set widget(filters)  [$self CreateFilters $f1]
    # Create Filter ButtonBox
    set widget(buttonbox)  [$self CreateBB]
    
    grid $widget(fields)  -sticky news
    grid $widget(filters) -sticky news
    
    grid rowconfigure    $f0 0 -weight 1
    grid columnconfigure $f0 0 -weight 1
    grid rowconfigure    $f1 0 -weight 1
    grid columnconfigure $f1 0 -weight 1
    
    # engridamos la paned window principal y la barra de botones
    grid $widget(pwM)       -sticky news
    grid $widget(buttonbox) -sticky ew
    
    grid columnconfigure $self 0 -weight 1
    grid rowconfigure    $self 0 -weight 1
    
    # Apply all options passed at creation time.
    $self configure -values         [from args -values ""]
    $self configure -language       [from args -language ""]
    $self configure -geometry       [from args -geometry ""]
    $self configure -showbuttons    [from args -showbuttons ""]
    $self configure -okcommand      [from args -okcommand ""]
    $self configure -cancelcommand  [from args -cancelcommand ""]

    $self configurelist $args
  }

  #///////////////////////////////////////////////////////////////////////////
  #
  # PURPOSE: Metodos cget y configure
  #
  #///////////////////////////////////////////////////////////////////////////

    onconfigure -values { lstValues } {
      $self ParseLstValues $lstValues
  }
  
    onconfigure -language { languagetype } {
      set language $languagetype
  }
  
  onconfigure -geometry { lst } {
    # formato; lista de numeros:
    # widget(pwM) -> widget principal
    # {widget(fields) widget(options) widget(filter)}
    # widget(fields)  ->  Separación del panel 1 del paned window principal
    # widget(options) ->  Separación del panel 2 del paned window principal
    # widget(filter)  ->  Separación del panel 3 del paned window principal

    if { !([llength $lst] eq 2) } {
      puts "BFilter error:\
            onconfigure -geometry, num params no valido"
    } else {
      set options(-geometry) $lst      
      # el primer panel aparece oculto
      #rcsoto
      #$widget(pwM) sash place 0 [lindex $lst 0] 0
      #$widget(pwM) sash place 1 [lindex $lst 1] 0
    }  
  }

  onconfigure -showbuttons { lst } {
    set options(-showbuttons) $lst
    #Los botones se muestran/ocultan
    if {$lst == 0} {
      grid remove $widget(buttonbox)
    } else {
      grid $widget(buttonbox)
    }
  }
  
  onconfigure -okcommand { lst } {
    set options(-okcommand) $lst
  }

  onconfigure -cancelcommand { lst } {
    set options(-cancelcommand) $lst
  }

  oncget -geometry {
    set lst {}
    # se añade a la lista el desplazamiento de los dos paneles horizontales
    #rcsoto
    #  lappend lst [lindex [$widget(pwM) sash coord 0] 0] \
                  [lindex [$widget(pwM) sash coord 1] 0]
    
    return $lst
  }

  #///////////////////////////////////////////////////////////////////////////
  method getFilter { } { 
  #
  # PURPOSE: Devuelve el path del listboxplus que contiene los filtros
  #
  #///////////////////////////////////////////////////////////////////////////
    return $widget(lbFilters)
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method getLastFilter { } { 
  #
  # PURPOSE: Devuelve el ultimo filtro de la lista de filtros
  #
  #///////////////////////////////////////////////////////////////////////////
    #Lista de indices de filtros
    set lstFiltersinx [$widget(lbFilters) get]
    #Ultimo indice de filtro
    set lastFilterinx [lindex $lstFiltersinx end]
    #Ultimo indice de filtro
    set lastFilter [$widget(lbFilters) get $lastFilterinx]
    #Campo code del ultimo filtro
    set lastFilterCod [lindex $lastFilter 1]
    
    return $lastFilterCod
  }
 
  #///////////////////////////////////////////////////////////////////////////
  method getButtons { } { 
  #
  # PURPOSE: Returns the path that contains the ButtonBox
  #
  #///////////////////////////////////////////////////////////////////////////
    return $widget(buttonbox)
  }

  #///////////////////////////////////////////////////////////////////////////
  method ParseLstValues { lst } { 
  #
  # PURPOSE: Tratamos la lista de valores que se le pasa al widget como opcion
  #
  #///////////////////////////////////////////////////////////////////////////
    set description {}
    foreach ite $lst {
      if {[string length [lindex $ite 4]]} {
        lappend description [lindex $ite 4]        
        lappend lstFinal [list [lindex $ite 4] [lindex $ite 0] [lindex $ite 1] [lindex $ite 2] [lindex $ite 3]] 
      } else {
        lappend description [lindex $ite 0]        
        lappend lstFinal [list [lindex $ite 0] [lindex $ite 0] [lindex $ite 1] [lindex $ite 2] [lindex $ite 3]]  
      }
    }
    #$widget(lbFields) configure -values $lstFinal
    $widget(lbFields) configure -values $description
  }
 
  #///////////////////////////////////////////////////////////////////////////
  method CreateLBF { path } { 
  #
  # PURPOSE: Crea el ListBoxPlus de los campos
  #
  #///////////////////////////////////////////////////////////////////////////
    # TitleFrame
    set tf [TitleFrame $path.filterField -text [mc Fields] -bd 1]
    set f  [$tf getframe]
 
    set widget(lbFields) [blistboxplus $f.lbFields \
                            -cols [mc "Fields"] \
                            -parent $tf \
                            -selectmode single\
                            -orientation vertical]

    bind $widget(lbFields).lb <ButtonPress-1>  [list $self LbFieldsOnClick]
     
    grid $widget(lbFields)  -sticky news -padx 1 -pady 1
      
    grid columnconfigure $f 0  -weight 1
    grid rowconfigure    $f 0  -weight 1

    return $tf
  }

  #///////////////////////////////////////////////////////////////////////////
  method CreateFilters { path } { 
  #
  # PURPOSE: Crea el Title Frame de las OPCIONES
  #
  #///////////////////////////////////////////////////////////////////////////  
    set f [frame $path.f]
    
    # Create Options Title Frame
    set widget(options)    [$self CreateTFO $f]
    # Create Filter Title Frame
    set widget(filter)     [$self CreateTFF $f]
    
    grid $widget(options) -sticky news
    grid $widget(filter)  -sticky news
    
    grid columnconfigure $f 0  -weight 1
    grid rowconfigure    $f 0  -weight 0
    grid rowconfigure    $f 1  -weight 1
    
    return $f
  }  
  
  
  #///////////////////////////////////////////////////////////////////////////
  method CreateTFO { path } { 
  #
  # PURPOSE: Crea el Title Frame de las OPCIONES
  #
  #///////////////////////////////////////////////////////////////////////////
    # TitleFrame
    set tf [TitleFrame $path.filterOption -text [mc "Filtrate restrictions"] \
             -bd 1]
    set f  [$tf getframe]
 
    # Create lista de operadores
    set lstOpeLogic [CreateLstOpeLogic]

    #list box de operadores
    Label $f.lOpe -text [mc Operators]
    set widget(cbOperator) [bcomboboxplus $f.cbOperator \
                              -cols "idOperator operators" \
                              -hiddencols "operators" \
                              -values $lstOpeLogic \
                              -titles [mc "Operators"] \
                              -showlabels 1 \
                              -width 120 -widthlb auto -nrows 7 \
                              -showlabelkey 0 \
                              -variable [myvar defaultOperator]]
    
    # initialization
    set defaultOperator "LIKE"
    $widget(cbOperator) configure -text [mc "Similar than 'Filter Value A'"]
    
    $widget(cbOperator) configure -modifycmd "$self OnChooseOpeLog"
    
    set fVal1  [frame $f.fVal1]
    #list box de valores Desde
    Label $f.lVal1 -text [mc "Filter Values A"] -anchor w -justify left
    set widget(cbValIni)   [bcomboboxplus $fVal1.cbValIni \
                              -cols [mc "Values"] \
                              -hiddencols "" \
                              -titles [mc "Values"] \
                              -showlabels 1 \
                              -widthlb auto -nrows 7 \
                              -showlabelkey 0]
    
    set fVal2  [frame $f.fVal2]
    #list box de valores Hasta
    Label $f.lVal2 -text [mc "Filter Values B"] -anchor w -justify left
    set widget(cbValEnd)   [bcomboboxplus $fVal2.cbValEnd \
                              -cols [mc "values"] \
                              -titles [mc "Values"] \
                              -showlabels 1 \
                              -widthlb auto -nrows 7 \
                              -showlabelkey 0]

    #boton de añadir
    set widget(bAdd) \
          [ButtonBox $f.bAdd -orient horizontal -spacing 5 -padx 0 -pady 0 \
          -homogeneous false]
    
    $widget(bAdd) add -relief link \
          -helptext [mc "Add Options selected to Filter Frame"] \
          -text  [mc "Add"]\
          -image [::Bitmap::get add] -compound bottom -padx 5 -pady 1 \
          -command "$self AddOnClick"
    
    grid $widget(cbValIni) -sticky news
    grid $widget(cbValEnd) -sticky news

    grid columnconfigure $fVal1 0  -weight 1
    grid columnconfigure $fVal2 0  -weight 1
    grid rowconfigure    $fVal1 0  -weight 1
    grid rowconfigure    $fVal2 0  -weight 1
       
    grid $f.lOpe  $widget(cbOperator)               -sticky nw  -padx 0 -pady 2
    grid $f.lVal1 $fVal1              $widget(bAdd) -sticky new -padx 0 -pady 2
    grid $f.lVal2 $fVal2              ^             -sticky new -padx 0 -pady 2    

    grid columnconfigure $f  0  -weight 0
    grid columnconfigure $f  1  -weight 1
    grid columnconfigure $f  0  -weight 0    
    grid rowconfigure    $f 10  -weight 1

    
    return $tf
  } 

  #///////////////////////////////////////////////////////////////////////////
  method CreateTFF { path } { 
  #
  # PURPOSE: Crea el Title Frame de los FILTER
  #
  #///////////////////////////////////////////////////////////////////////////
    # TitleFrame
    set tf [TitleFrame $path.filterFilter -text [mc Filters] -bd 1]
    set f  [$tf getframe]
 
    frame $f.f
    radiobutton $f.f.bAnd -text [mc {AND}] -value and \
      -variable [varname data(opt,operator)] 
    radiobutton $f.f.bOr  -text [mc {OR}]  -value or \
      -variable [varname data(opt,operator)]
    radiobutton $f.f.bNot -text [mc {NOT}] -value not \
      -variable [varname data(opt,operator)]
    
    #boton de aplicar
    Button $f.f.bApply -relief link \
      -helptext [mc "Apply Filter"] \
      -text [mc "Apply Filter"]\
      -image [::Bitmap::get accept] -compound left -padx 5 -pady 1 \
      -command "$self OnClickOpeLog"
    
    #list box de Filtros
    set widget(lbFilters) [blistboxplus $f.lbFilters \
                            -cols {Filters CodeFilter} \
                            -hiddencols {CodeFilter} \
                            -parent $tf \
                            -orientation vertical]

    grid $f.f.bAnd $f.f.bOr $f.f.bNot $f.f.bApply -sticky nw

    grid columnconfigure $f.f 0   -weight 0
    grid rowconfigure    $f.f 10  -weight 1

    grid $f.f               -sticky nw
    grid $widget(lbFilters) -sticky news -padx 1 -pady 1

    grid columnconfigure $f 0   -weight 1
    grid rowconfigure    $f 0   -weight 0
    grid rowconfigure    $f 1   -weight 1
   
    return $tf
  }
  

  #///////////////////////////////////////////////////////////////////////////
  method CreateBB {} { 
  #
  # PURPOSE: Crea el ButtonBox del filter
  #
  #///////////////////////////////////////////////////////////////////////////
    set widget(bbox) \
    [ButtonBox $self.bb -orient horizontal -spacing 10 -padx 0 -pady 0 \
       -homogeneous false]
    $widget(bbox) add -text [mc "Accept"] -relief link -helptext [mc "Accept"] \
        -image [::Bitmap::get accept] -compound left -padx 5 -pady 1 \
        -command [list $self accept]
    $widget(bbox) add -text [mc "Exit"] -relief link \
      -helptext [mc "Exit"]\
      -image [::Bitmap::get exit] -compound left -padx 5 -pady 1 \
      -command [list $self cancel]
    
    return $widget(bbox)
  }

  #///////////////////////////////////////////////////////////////////////////
  method accept {} { 
  #
  # PURPOSE: Ejecuta el procedimiento que se pasa en -okcommand
  #
  #///////////////////////////////////////////////////////////////////////////
    set cmd [$self cget -okcommand]
    if {[string length $cmd]} {
       eval $cmd
    } else {
      return [$self getLastFilter]
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method cancel {} { 
  #
  # PURPOSE: Ejecuta el procedimiento que se pasa en -cancelcommand
  #
  #///////////////////////////////////////////////////////////////////////////
    set cmd [$self cget -cancelcommand]
    if {[string length $cmd]} {
       eval $cmd
    } else {
      destroy [winfo parent [winfo parent [winfo parent $self]]]
    }
  }
 
  #///////////////////////////////////////////////////////////////////////////  
  proc CreateLstOpeLogic {} {
  #
  # PURPOSE: Crea la lista de operadores
  #
  #///////////////////////////////////////////////////////////////////////////
    set lst "{=    {[mc "Equal to 'Filter Value A'"]}} \
             {<>   {[mc "Not equal to 'Filter Value A'"]}} \
             {<    {[mc "Less than 'Filter Value A'"]}} \
             {>    {[mc "Greater than 'Filter Value A'"]}} \
             {>=   {[mc "Greater than or equal to 'Filter Value A'"]}} \
             {<=   {[mc "Less than or equal to 'Filter Value A'"]}} \
             {\[\] {[mc "Greater than or equal to 'Filter Value A' and Less than or equal to 'Filter Value B'"]}} \
             {(\]  {[mc "Greater than 'Filter Value A' and Less than or equal to 'Filter Value B'"]}} \
             {\[)  {[mc "Greater than or equal to 'Filter Value A' and Less than 'Filter Value B'"]}} \
             {()   {[mc "Greater than 'Filter Value A' and Less than 'Filter Value B'"]}} \
             {IN   {[mc "Any element in set of 'Filter Value A'"]}} \
             {{NOT IN} {[mc "None element in set of 'Filter Value A'"]}} \
             {LIKE {[mc "Similar than 'Filter Value A'"]}}"
    return $lst
  }

  #/////////////////////////////////////////////////////////////////////////////
  method OnChooseOpeLog { } {
  #
  # PURPOSE: Acción que se ejecuta al cambiar / elegir tipo de operador
  #
  # PARAMETERS:
  #
  #/////////////////////////////////////////////////////////////////////////////
    set i 1
    # elementos seleccionados
    set idFieldSelect [lindex [$widget(lbFields) curselection]]
    
    if {[llength $idFieldSelect]} {
      foreach ite $lstFinal {
        if {$i eq $idFieldSelect} {
          set myTypeAux  [lindex $ite 2]
          set myValueAux [lindex $ite 3]
        }
        incr i 1
      }

      set myTypeAux [string tolower [lindex $myTypeAux 0]]
      # destroy widget exists		
      if {[info exist widget(val1)]} {
        destroy $widget(val1)
      }
      if {[info exist widget(cbFecha1)]} {
        destroy $widget(cbFecha1)
      }
      if {[info exist widget(cbFecha2)]} {
        destroy $widget(cbFecha2)
      }
      grid forget $widget(cbValIni)
      
      set idFieldSelect [lindex [$widget(lbFields) curselection]]
      set id [$widget(cbOperator) selection get]
      
      switch $id {
        1 {
          set defaultOperator "="
          $widget(cbOperator) configure -text [mc "Equal to 'Filter Value A'"]
        }
        2 {
          set defaultOperator "<>"
          $widget(cbOperator) configure -text [mc "Not equal to 'Filter Value A'"]
        }
        3 {
          set defaultOperator "<"
          $widget(cbOperator) configure -text [mc "Less than 'Filter Value A'"]
        }
        4 {
          set defaultOperator ">"
          $widget(cbOperator) configure -text [mc "Greater than 'Filter Value A'"]
        }
        5 {
          set defaultOperator ">="
          $widget(cbOperator) configure -text [mc "Greater than or equal to 'Filter Value A'"]
        }
        6 {
          set defaultOperator "<="
          $widget(cbOperator) configure -text [mc "Less than or equal to 'Filter Value A'"]
        }
        7 {
          set defaultOperator "\[\]"
          $widget(cbOperator) configure -text [mc "Greater than or equal to 'Filter Value A' and Less than or equal to 'Filter Value B'"]
        }
        8 {
          set defaultOperator "(\]"
          $widget(cbOperator) configure -text [mc "Greater than 'Filter Value A' and Less than or equal to 'Filter Value B'"]
        }
        9 {
          set defaultOperator "\[)"
          $widget(cbOperator) configure -text [mc "Greater than or equal to 'Filter Value A' and Less than 'Filter Value B'"]
        }
        10 {
          set defaultOperator "()"
          $widget(cbOperator) configure -text [mc "Greater than 'Filter Value A' and Less than 'Filter Value B'"]
        }
        11 {
          set defaultOperator "IN"
          $widget(cbOperator) configure -text [mc "Any element in set of 'Filter Value A'"]
        }
        12 {
          set defaultOperator "{NOT IN}"
          $widget(cbOperator) configure -text [mc "None element in set of 'Filter Value A'"]
        }
        13 {
          set defaultOperator "LIKE"
          $widget(cbOperator) configure -text [mc "Similar than 'Filter Value A'"]
        }
      }
      
      #operador simple
      if {($id == 1) || ($id == 2) || ($id == 3) || \
          ($id == 4) || ($id == 5) || ($id == 6) || ($id == 13)} {
        if {$myTypeAux == "date"} {
          set widget(cbFecha1) \
            [bcomboboxplus $fVal1.cbFecha1 \
              -cols [mc "Values"] \
              -titles [mc "Values"] \
              -showlabels 1 \
              -widthlb auto -nrows 7 \
              -showlabelkey 0 \
              -keydate 1 ]
            
          $widget(cbFecha1) configure -postcommand "$self OnLstFecha 1"
          grid $widget(cbFecha1) -sticky news
          
          set widget(cbFecha2) \
            [bcomboboxplus $fVal2.cbFecha2 \
              -cols [mc "Values"] \
              -titles [mc "Values"] \
              -showlabels 1 \
              -widthlb auto -nrows 7 \
              -showlabelkey 0 \
              -keydate 1 ]
            
          $widget(cbFecha2) configure -postcommand "$self OnLstFecha 2" 
          grid $widget(cbFecha2) -sticky news
          
          #activación y desactivación
          $widget(cbFecha1) activate "allactive"
          $widget(cbFecha2) activate "disabled"
        } else {
          $widget(cbValIni) configure -postcommand "$self OnLstVal 1"
          $widget(cbValEnd) configure -postcommand "$self OnLstVal 2"
          grid $widget(cbValIni) -sticky news
          grid $widget(cbValEnd) -sticky news
          
          #activación y desactivación
          $widget(cbValIni) activate "allactive"
          $widget(cbValEnd) activate "disabled"
        }
      }
    
      # operador multiple
      if {($id == 7) || ($id == 8) || ($id == 9) || ($id == 10)} {
        if {$myTypeAux == "date"} {
          set widget(cbFecha1) \
            [bcomboboxplus $fVal1.cbFecha1 \
              -cols [mc "Values"] \
              -titles [mc "Values"] -showlabels 1 \
              -widthlb auto -nrows 7 \
              -showlabelkey 0 \
              -keydate 1 ]
            
          $widget(cbFecha1) configure -postcommand "$self OnLstFecha 1"
          grid $widget(cbFecha1) -sticky news
          
          set widget(cbFecha2) \
            [bcomboboxplus $fVal2.cbFecha2 \
              -cols [mc "Values"] \
              -titles [mc "Values"] -showlabels 1 \
              -widthlb auto -nrows 7 \
              -showlabelkey 0 \
              -keydate 1 ]
            
          $widget(cbFecha2) configure -postcommand "$self OnLstFecha 2"
          grid $widget(cbFecha2) -sticky news
          
          #activación y desactivación
          $widget(cbFecha1) activate "allactive"
          $widget(cbFecha2) activate "allactive"
        } else {
          $widget(cbValIni) configure -postcommand "$self OnLstVal 1"
          $widget(cbValEnd) configure -postcommand "$self OnLstVal 2"
          grid $widget(cbValIni) -sticky news
          grid $widget(cbValEnd) -sticky news
          #activación y desactivación
          $widget(cbValIni) activate "allactive"
          $widget(cbValEnd) activate "allactive"
        }
      }
      
      # operador de agrupación (IN / NOT IN)
      if {($id == 11) || ($id == 12)} {
        if {($myTypeAux == "date")} {
          set widget(val1) \
            [beditselector $fVal1.val1  \
              -cols "[mc Values]" \
              -selcols "[mc Values]" \
              -unique false \
              -procedure [list $self OnLstMultiSelec] ]
          
          grid $widget(val1) -sticky news
           
          set widget(cbFecha2) \
            [bcomboboxplus $fVal2.cbFecha2 \
              -cols [mc "Values"] \
              -titles [mc "Values"] -showlabels 1 \
              -widthlb auto -nrows 7 \
              -showlabelkey 0 \
              -keydate 1 ]
          grid $widget(cbFecha2) -sticky news
          
          #activación y desactivación
          $widget(cbFecha2) activate "disabled"
        } else {
          set widget(val1) \
            [beditselector $fVal1.val1 \
              -cols "[mc Values]" \
              -selcols "[mc Values]" \
              -unique false \
              -procedure [list $self OnLstMultiSelec] ]

          grid $widget(val1)     -sticky news
          grid $widget(cbValEnd) -sticky news
          
          #activación y desactivación
          $widget(cbValEnd) activate "disabled"
        }
      }
    } else {
      $widget(cbOperator) key clean
      set msg [mc {"To select an operator select a Field is needed"}]
        ::TolConsole::HciWriter "<W>${msg}</W>"
        return {} 
    }
  }


  #/////////////////////////////////////////////////////////////////////////////
  method LbFieldsOnClick { } {
  #
  # PURPOSE: Evento OnClick del ListBox de Campos
  #
  #/////////////////////////////////////////////////////////////////////////////
    # elementos seleccionados
    set idFieldSelect [lindex [$widget(lbFields) curselection]]
    
    if {[llength $idFieldSelect]} {
      set lstValuesComp ""
      set i 1    
      
      foreach ite $lstFinal {
        if {$i eq $idFieldSelect} {
          set myType        [lindex $ite 2]
          set myValues      [lindex $ite 3] 
          set myFunction    [lindex $ite 4]
        }
        incr i 1
      }
  
      set myType [string tolower [lindex $myType 0]]
  
      if {![llength $myValues]} {
        set myValues ""
      }
      if {[info exist widget(val1)]} {
        destroy $widget(val1)
      }
      switch $myType {
        "date" {
          if {[info exist widget(cbValIni)]} {
            grid forget $widget(cbValIni)
          }
          if {[info exist widget(cbValEnd)]} {
            grid forget $widget(cbValEnd)
          }
          if {[info exist widget(cbFecha1)]} {
            destroy $widget(cbFecha1)
          }
          set widget(cbFecha1) [bcomboboxplus $fVal1.cbFecha1 \
            -cols [mc "Values"] -titles [mc "Values"] \
            -showlabels 1       -widthlb auto -nrows 7 \
            -showlabelkey 0     -keydate 1 \
            -postcommand "list $self OnLstFecha 1" ]
          grid $widget(cbFecha1) -sticky news
          
          if {[info exist widget(cbFecha2)]} {
            destroy $widget(cbFecha2)
          }
          set widget(cbFecha2) [bcomboboxplus $fVal2.cbFecha2 \
            -cols [mc "Values"] -titles [mc "Values"] \
            -showlabels 1       -widthlb auto -nrows 7 \
            -showlabelkey 0     -keydate 1]
          grid $widget(cbFecha2) -sticky news
        }
        
        default {
          if {[info exist widget(cbFecha1)]} {
            destroy $widget(cbFecha1)
          }
          $widget(cbValIni) key clean
          grid $widget(cbValIni)  -sticky news

          if {[info exist widget(cbFecha2)]} {
            destroy $widget(cbFecha2)
          }
          $widget(cbValEnd) key clean
          grid $widget(cbValEnd) -sticky news
        }
      }
    }
  }
  
  #/////////////////////////////////////////////////////////////////////////////
  method OnClickOpeLog { } {
  #
  # PURPOSE: Comprobamos que el operador logico seleccionado cumple con los
  #   requisitos de la "logica", para "Not" un solo elemento, para "And" y "Or"
  #   mas de un elemento. De ser asi introducimos la expresion generada en el
  #  repositorio de expresiones
  #
  #/////////////////////////////////////////////////////////////////////////////               
    variable widget
    variable data
    # elementos seleccinados
    set lst {}
    set lstAux {}
    foreach it [$widget(lbFilters) selection get] {
      lappend lst    [lindex [$widget(lbFilters) get $it Filters] 0]
      lappend lstAux [lindex [$widget(lbFilters) get $it CodeFilter] 0]
    }
    
    set typeLanguage [string tolower [lindex $language 0]]
    
    switch $typeLanguage {
      "sql" {
        set txt   {}
        set txtid {}
        set lstid [$widget(lbFilters) selection get]
        set opeLog [string toupper $data(opt,operator) 0 0]
        if {[expr [llength $lst] == 1] && [string equal ${data(opt,operator)} not]} {
          set txt    "${opeLog}([lindex $lst    0])"
          set txtAux "${opeLog}([lindex $lstAux 0])"
          set txtid  "${opeLog}([lindex $lstid  0])"
        } elseif {[expr [llength $lst] > 1] && 
                  ([string equal $data(opt,operator) and] || 
                   [string equal $data(opt,operator) or]) } {
          set txt    "([join $lst    " $opeLog "])"
          set txtAux "([join $lstAux " $opeLog "])"
          set txtid  "${opeLog}([join $lstid ,])"
        }
      }
      "tol" {
        set txt   {}
        set txtid {}
        set lstid [$widget(lbFilters) selection get]
        set opeLog [string toupper $data(opt,operator) 0 0]
        if {[expr [llength $lst] == 1] && [string equal ${data(opt,operator)} not]} {
          set txt    "${opeLog}([lindex $lst    0])"
          set txtAux "${opeLog}([lindex $lstAux 0])"
          set txtid  "${opeLog}([lindex $lstid  0])"
        } elseif {[expr [llength $lst] > 1] && 
                  ([string equal $data(opt,operator) and] || 
                   [string equal $data(opt,operator) or]) } {
          set txt    "Real ${opeLog}([join $lst    ", "])"
          set txtAux "Real ${opeLog}([join $lstAux ", "])"
          set txtid  "${opeLog}([join $lstid ,])"
        } 
      }      
    }

    # insertamos txt  
    if {[string length $txt]} {
      $widget(lbFilters) insert end [list $txt $txtAux]
    }
  }
  
  #/////////////////////////////////////////////////////////////////////////////
  method AddOnClick { } {
  #
  # PURPOSE: Evento Click del botón Add expresión de filtrado
  #
  #/////////////////////////////////////////////////////////////////////////////
    set i 1
        
    set idFieldSelect [lindex [$widget(lbFields) curselection]]
    
    if { [llength $idFieldSelect]} {   
      foreach ite $lstFinal {
        if {$i eq $idFieldSelect} {
          set myType        [lindex $ite 2]
          set myDescription [lindex $ite 0]
          set myCode        [lindex $ite 1]
        }
        incr i 1
      }
      
      set idOperator   [$widget(cbOperator) selection get]
      set myTypeAux    [string tolower [lindex $myType 0]]
      set typeLanguage [string tolower [lindex $language 0]]
  
      if {[llength $idOperator]} {
        switch $typeLanguage {
          "sql" {
            if { $idOperator == 1 } { ;#operator =
              if {$myTypeAux != "date"} {
                set valueAux [$widget(cbValIni) key get]
              } else {
                set valueAux [$widget(cbFecha1) key get]
              }
              if {[TolValidateData $valueAux $myTypeAux] == 1} {
                set sep [TolGetDelimeterField $myTypeAux]
                set txt "($myDescription = [lindex $sep 0]$valueAux[lindex $sep 1])"
                set txtAux "($myCode = [lindex $sep 0]$valueAux[lindex $sep 1])"
                $widget(lbFilters) insert end [list $txt $txtAux]
              }
            }
            
            if { $idOperator == 2 } { ;#operator <>
              if {$myTypeAux != "date"} {
                set valueAux [$widget(cbValIni) key get]
              } else {
                set valueAux [$widget(cbFecha1) key get]
              }
              if {[TolValidateData $valueAux $myTypeAux] == 1} {
                set sep [TolGetDelimeterField $myTypeAux]
                set txt "($myDescription <> [lindex $sep 0]$valueAux[lindex $sep 1])"
                set txtAux "($myCode <> [lindex $sep 0]$valueAux[lindex $sep 1])"
                $widget(lbFilters) insert end [list $txt $txtAux]
              }
            }
            
            if { $idOperator == 3 } { ;#operator <
              if {$myTypeAux != "date"} {
                set valueAux [$widget(cbValIni) key get]
              } else {
                set valueAux [$widget(cbFecha1) key get]
              }
              if {[TolValidateData $valueAux $myTypeAux] == 1} {
                set sep [TolGetDelimeterField $myTypeAux]
                set txt "($myDescription < [lindex $sep 0]$valueAux[lindex $sep 1])"
                set txtAux "($myCode < [lindex $sep 0]$valueAux[lindex $sep 1])"
                $widget(lbFilters) insert end [list $txt $txtAux]
              }
            }
            
            if { $idOperator == 4 } { ;#operator >
              if {$myTypeAux != "date"} {
                set valueAux [$widget(cbValIni) key get] 
              } else {
                set valueAux [$widget(cbFecha1) key get]
              }
              if {[TolValidateData $valueAux $myTypeAux] == 1} {
                set sep [TolGetDelimeterField $myTypeAux]
                set txt "($myDescription > [lindex $sep 0]$valueAux[lindex $sep 1])"
                set txtAux "($myCode > [lindex $sep 0]$valueAux[lindex $sep 1])"
                $widget(lbFilters) insert end [list $txt $txtAux]
              }
            }
            
            if { $idOperator == 5 } { ;#operator >=
              if {$myTypeAux != "date"} {
                set valueAux [$widget(cbValIni) key get]
              } else {
                set valueAux [$widget(cbFecha1) key get]
              }
              if {[TolValidateData $valueAux $myTypeAux] == 1} {
                set sep [TolGetDelimeterField $myTypeAux]
                set txt "($myDescription >= [lindex $sep 0]$valueAux[lindex $sep 1])"
                set txtAux "($myCode >= [lindex $sep 0]$valueAux[lindex $sep 1])"
                $widget(lbFilters) insert end [list $txt $txtAux]
              }
            }
            
            if { $idOperator == 6 } { ;#operator <=
              if {$myTypeAux != "date"} {
                set valueAux [$widget(cbValIni) key get]
              } else {
                set valueAux [$widget(cbFecha1) key get]
              }
              if {[TolValidateData $valueAux $myTypeAux] == 1} {
                set sep [TolGetDelimeterField $myTypeAux]
                set txt "($myDescription <= [lindex $sep 0]$valueAux[lindex $sep 1])"
                set txtAux "($myCode <= [lindex $sep 0]$valueAux[lindex $sep 1])"
                $widget(lbFilters) insert end [list $txt $txtAux]
              }
            }
            
            if { $idOperator == 7 } { ;#operator []
              if {$myTypeAux != "date"} {
                set valueAux1 [$widget(cbValIni) key get]
                set valueAux2 [$widget(cbValEnd) key get]
              } else {
                set valueAux1 [$widget(cbFecha1) key get]
                set valueAux2 [$widget(cbFecha2) key get]
              }
              if {([TolValidateData $valueAux1 $myTypeAux] == 1) && ([TolValidateData $valueAux2 $myTypeAux] == 1)} {
                set sep [TolGetDelimeterField $myTypeAux]
                set txt "($myDescription >= [lindex $sep 0]$valueAux1[lindex $sep 1]) and ($myDescription <= [lindex $sep 0]$valueAux2[lindex $sep 1])"
                set txtAux "($myCode >= [lindex $sep 0]$valueAux1[lindex $sep 1]) and ($myCode <= [lindex $sep 0]$valueAux2[lindex $sep 1])"
                $widget(lbFilters) insert end [list $txt $txtAux]
              }
            }
            
            if { $idOperator == 8 } { ;#operator (]
              if {$myTypeAux != "date"} {
                set valueAux1 [$widget(cbValIni) key get]
                set valueAux2 [$widget(cbValEnd) key get]
              } else {
                set valueAux1 [$widget(cbFecha1) key get]
                set valueAux2 [$widget(cbFecha2) key get]
              }
              if {([TolValidateData $valueAux1 $myTypeAux] == 1) && ([TolValidateData $valueAux2 $myTypeAux] == 1)} {
                set sep [TolGetDelimeterField $myTypeAux]
                set txt "($myDescription > [lindex $sep 0]$valueAux1[lindex $sep 1]) and ($myDescription <= [lindex $sep 0]$valueAux2[lindex $sep 1])"
                set txtAux "($myCode > [lindex $sep 0]$valueAux1[lindex $sep 1]) and ($myCode <= [lindex $sep 0]$valueAux2[lindex $sep 1])"
                $widget(lbFilters) insert end [list $txt $txtAux]
              }
            }
            
            if { $idOperator == 9 } { ;#operator [)
              if {$myTypeAux != "date"} {
                set valueAux1 [$widget(cbValIni) key get]
                set valueAux2 [$widget(cbValEnd) key get]
              } else {
                set valueAux1 [$widget(cbFecha1) key get]
                set valueAux2 [$widget(cbFecha2) key get]
              }
              if {([TolValidateData $valueAux1 $myTypeAux] == 1) && ([TolValidateData $valueAux2 $myTypeAux] == 1)} {
                set sep [TolGetDelimeterField $myTypeAux]
                set txt "($myDescription >= [lindex $sep 0]$valueAux1[lindex $sep 1]) and ($myDescription < [lindex $sep 0]$valueAux2[lindex $sep 1])"
                set txtAux "($myCode >= [lindex $sep 0]$valueAux1[lindex $sep 1]) and ($myCode < [lindex $sep 0]$valueAux2[lindex $sep 1])"
                $widget(lbFilters) insert end [list $txt $txtAux]
              }
            }
            
            if { $idOperator == 10 } { ;#operator ()
              if {$myTypeAux != "date"} {
                set valueAux1 [$widget(cbValIni) key get]
                set valueAux2 [$widget(cbValEnd) key get]
              } else {
                set valueAux1 [$widget(cbFecha1) key get]
                set valueAux2 [$widget(cbFecha2) key get]
              }
              if {([TolValidateData $valueAux1 $myTypeAux] == 1) && ([TolValidateData $valueAux2 $myTypeAux] == 1)} {
                set sep [TolGetDelimeterField $myTypeAux]
                set txt "($myDescription > [lindex $sep 0]$valueAux1[lindex $sep 1]) and ($myDescription < [lindex $sep 0]$valueAux2[lindex $sep 1])"
                set txtAux "($myCode > [lindex $sep 0]$valueAux1[lindex $sep 1]) and ($myCode < [lindex $sep 0]$valueAux2[lindex $sep 1])"
                $widget(lbFilters) insert end [list $txt $txtAux]
              }
            }
            
            if { $idOperator == 11 } { ;#operator IN
                # lista de valores seleccionados
                set lstValSel [$widget(val1) selection get selected [mc "Values"]]
                foreach val $lstValSel {
                  if {[TolValidateData $val $myTypeAux] == 1} {
                    set sep [TolGetDelimeterField $myTypeAux]
                    lappend lstValAux "[lindex $sep 0]$val[lindex $sep 1]" 
                  }
                }
                set myLstValAux [ListToStr $lstValAux { , } {} {}]
                set txt "($myDescription IN ($myLstValAux))"
                set txtAux "($myCode IN ($myLstValAux))"
                $widget(lbFilters) insert end [list $txt $txtAux]
            }
            
            if { $idOperator == 12 } { ;#operator NOT IN
                # lista de valores seleccionados
                set lstValSel [$widget(val1) selection get selected [mc "Values"]]
                foreach val $lstValSel {
                  if {[TolValidateData $val $myTypeAux] == 1} {
                    set sep [TolGetDelimeterField $myTypeAux]
                    lappend lstValAux "[lindex $sep 0]$val[lindex $sep 1]" 
                  }
                }
                set myLstValAux [ListToStr $lstValAux { , } {} {}]
                set txt "($myDescription NOT IN ($myLstValAux))"
                set txtAux "($myCode NOT IN ($myLstValAux))"
                $widget(lbFilters) insert end [list $txt $txtAux]
            }
            
            if { $idOperator == 13 } { ;#operator LIKE
              if {$myTypeAux != "date"} {
                set valueAux [$widget(cbValIni) key get]
              } else {
                set valueAux [$widget(cbFecha1) key get]
              }
              if {[TolValidateData $valueAux $myTypeAux] == 1} {
                set sep [TolGetDelimeterField $myTypeAux]
                set txt "(UPPER($myDescription) LIKE UPPER([lindex $sep 0]%$valueAux%[lindex $sep 1]))"
                set txtAux "(UPPER($myCode) LIKE UPPER([lindex $sep 0]%$valueAux%[lindex $sep 1]))"
                $widget(lbFilters) insert end [list $txt $txtAux]
              }
            }
          }    
          "tol" {
            switch $idOperator {
              1 - 2 - 3 -
              4 - 5 - 6 {
                if {$myTypeAux != "date"} {
                  set valueAux [$widget(cbValIni) key get]
                } else {
                  set valueAux [$widget(cbFecha1) key get]
                }
                
                if { $idOperator == 1 } { ;#operator =
                  if {$myTypeAux == "date"} {
                    if {[CheckDateTime $valueAux]} {
                      set valueAux [DateToIso8601 $valueAux y%Ym%md%d]
                      set txt "(Real $myDescription == $valueAux)"
                      set txtAux "(Real $myCode == $valueAux)"
                      $widget(lbFilters) insert end [list $txt $txtAux]
                    } else {
                      set msg [mc {Invalid selected DateTime}]
                      ::TolConsole::HciWriter "<W>${msg}</W>"
                      return {} 
                    }
                  } 
                  
                  if {$myTypeAux == "text"} {
                    if {[TolValidateData $valueAux $myTypeAux] == 1} {
                      set sep "\""
                      set txt "(Real $myDescription == $sep$valueAux$sep)"
                      set txtAux "(Real $myCode = $sep$valueAux$sep)"
                      $widget(lbFilters) insert end [list $txt $txtAux]
                    } 
                  }
                  
                  if {$myTypeAux == "real"} {
                    if {[TolValidateData $valueAux $myTypeAux] == 1} {
                      set sep [TolGetDelimeterField $myTypeAux]
                      set txt "(Real Eq($myDescription, [lindex $sep 0]$valueAux[lindex $sep 1]))"
                      set txtAux "(Real Eq($myCode, [lindex $sep 0]$valueAux[lindex $sep 1]))"
                      $widget(lbFilters) insert end [list $txt $txtAux]
                    } 
                  }
                }
                
                if { $idOperator == 2 } { ;#operator !=
                  set myOp "!="
                  if {$myTypeAux == "date"} {
                    if {[CheckDateTime $valueAux]} {
                      set valueAux [DateToIso8601 $valueAux y%Ym%md%d]
                      set txt "(Real $myDescription != $valueAux)"
                      set txtAux "(Real $myCode != $valueAux)"
                      $widget(lbFilters) insert end [list $txt $txtAux]
                    } else {
                      set msg [mc {Invalid selected DateTime}]
                      ::TolConsole::HciWriter "<W>${msg}</W>"
                      return {} 
                    }
                  } else {
                    if {$myTypeAux == "text"} {
                      set sep "\""
                    }
                    if {$myTypeAux == "real"} {
                      set sep ""
                    }
                    if {[TolValidateData $valueAux $myTypeAux] == 1} {
                      set txt "(Real $myDescription != $sep$valueAux$sep)"
                      set txtAux "(Real $myCode != $sep$valueAux$sep)"
                      $widget(lbFilters) insert end [list $txt $txtAux]
                    } 
                  }
                }
                if { $idOperator == 3 } { ;#operator <
                  if {$myTypeAux == "date"} {
                    if {[CheckDateTime $valueAux]} {
                      set valueAux [DateToIso8601 $valueAux y%Ym%md%d]
                      set txt "(Real $myDescription < $valueAux)"
                      set txtAux "(Real $myCode < $valueAux)"
                      $widget(lbFilters) insert end [list $txt $txtAux]
                    } else {
                      set msg [mc {Invalid selected DateTime}]
                      ::TolConsole::HciWriter "<W>${msg}</W>"
                      return {} 
                    }
                  } else {
                    if {$myTypeAux == "text"} {
                      set sep "\""
                      if {[TolValidateData $valueAux $myTypeAux] == 1} {
                        set txt "(Real $myDescription < $sep$valueAux$sep)"
                        set txtAux "(Real $myCode < $sep$valueAux$sep)"
                        $widget(lbFilters) insert end [list $txt $txtAux]
                      }
                    } 
                    if {$myTypeAux == "real"} {
                      set sep ""
                      if {[TolValidateData $valueAux $myTypeAux] == 1} {
                        set txt "(Real LT($myDescription, $sep$valueAux$sep))"
                        set txtAux "(Real LT($myCode, $sep$valueAux$sep))"
                        $widget(lbFilters) insert end [list $txt $txtAux]
                      }
                    }
                  }
                }
                if { $idOperator == 4 } { ;#operator >
                  if {$myTypeAux == "date"} {
                    if {[CheckDateTime $valueAux]} {
                      set valueAux [DateToIso8601 $valueAux y%Ym%md%d]
                      set txt "(Real $myDescription > $valueAux)"
                      set txtAux "(Real $myCode > $valueAux)"
                      $widget(lbFilters) insert end [list $txt $txtAux]
                    } else {
                      set msg [mc {Invalid selected DateTime}]
                      ::TolConsole::HciWriter "<W>${msg}</W>"
                      return {} 
                    }
                  } else {
                    if {$myTypeAux == "text"} {
                      set sep "\""
                      if {[TolValidateData $valueAux $myTypeAux] == 1} {
                        set txt "(Real $myDescription > $sep$valueAux$sep)"
                        set txtAux "(Real $myCode > $sep$valueAux$sep)"
                        $widget(lbFilters) insert end [list $txt $txtAux]
                      }
                    } 
                    if {$myTypeAux == "real"} {
                      set sep ""
                      if {[TolValidateData $valueAux $myTypeAux] == 1} {
                        set txt "(Real GT($myDescription, $sep$valueAux$sep))"
                        set txtAux "(Real GT($myCode, $sep$valueAux$sep))"
                        $widget(lbFilters) insert end [list $txt $txtAux]
                      }
                    }
                  }
                }
                if { $idOperator == 5 } { ;#operator >=
                  if {$myTypeAux == "date"} {
                    if {[CheckDateTime $valueAux]} {
                      set valueAux [DateToIso8601 $valueAux y%Ym%md%d]
                      set txt "(Real $myDescription >= $valueAux)"
                      set txtAux "(Real $myCode < $valueAux)"
                      $widget(lbFilters) insert end [list $txt $txtAux]
                    } else {
                      set msg [mc {Invalid selected DateTime}]
                      ::TolConsole::HciWriter "<W>${msg}</W>"
                      return {} 
                    }
                  } else {
                    if {$myTypeAux == "text"} {
                      set sep "\""
                      if {[TolValidateData $valueAux $myTypeAux] == 1} {
                        set txt "(Real $myDescription >= $sep$valueAux$sep)"
                        set txtAux "(Real $myCode >= $sep$valueAux$sep)"
                        $widget(lbFilters) insert end [list $txt $txtAux]
                      }
                    } 
                    if {$myTypeAux == "real"} {
                      set sep ""
                      if {[TolValidateData $valueAux $myTypeAux] == 1} {
                        set txt "(Real GE($myDescription, $sep$valueAux$sep))"
                        set txtAux "(Real GE($myCode, $sep$valueAux$sep))"
                        $widget(lbFilters) insert end [list $txt $txtAux]
                      }
                    }
                  }
                }
                if { $idOperator == 6 } { ;#operator <=
                  if {$myTypeAux == "date"} {
                    if {[CheckDateTime $valueAux]} {
                      set valueAux [DateToIso8601 $valueAux y%Ym%md%d]
                      set txt "(Real $myDescription <= $valueAux)"
                      set txtAux "(Real $myCode <= $valueAux)"
                      $widget(lbFilters) insert end [list $txt $txtAux]
                    } else {
                      set msg [mc {Invalid selected DateTime}]
                      ::TolConsole::HciWriter "<W>${msg}</W>"
                      return {} 
                    }
                  } else {
                    if {$myTypeAux == "text"} {
                      set sep "\""
                      if {[TolValidateData $valueAux $myTypeAux] == 1} {
                        set txt "(Real $myDescription <= $sep$valueAux$sep)"
                        set txtAux "(Real $myCode <= $sep$valueAux$sep)"
                        $widget(lbFilters) insert end [list $txt $txtAux]
                      }
                    } 
                    if {$myTypeAux == "real"} {
                      set sep ""
                      if {[TolValidateData $valueAux $myTypeAux] == 1} {
                        set txt "(Real LE($myDescription, $sep$valueAux$sep))"
                        set txtAux "(Real LE($myCode, $sep$valueAux$sep))"
                        $widget(lbFilters) insert end [list $txt $txtAux]
                      }
                    }
                  }
                }
                
              }
              7 - 8 -
              9 - 10 {
                if {$myTypeAux != "date"} {
                  set valueAux1 [$widget(cbValIni) key get]
                  set valueAux2 [$widget(cbValEnd) key get]
                } else {
                  set valueAux1 [$widget(cbFecha1) key get]
                  set valueAux2 [$widget(cbFecha2) key get]
                }
                
                if { $idOperator == 7 } { ;#operator []
                  if {$myTypeAux == "date"} {
                    if {([CheckDateTime $valueAux1]) && ([CheckDateTime $valueAux2])} {
                      set valueAux1 [DateToIso8601 $valueAux1 y%Ym%md%d]
                      set valueAux2 [DateToIso8601 $valueAux2 y%Ym%md%d]
                      set txt "(Real $myDescription >= $valueAux1 & Real $myDescription <= $valueAux2)"
                      set txtAux "(Real $myCode >= $valueAux1 & Real $myCode <= $valueAux2)"
                      $widget(lbFilters) insert end [list $txt $txtAux]
                    } else {
                      set msg [mc {Invalid selected DateTime}]
                      ::TolConsole::HciWriter "<W>${msg}</W>"
                      return {} 
                    }
                  } else {
                    if {$myTypeAux == "text"} {
                      set sep "\""
                      if {([TolValidateData $valueAux1 $myTypeAux] == 1) && ([TolValidateData $valueAux2 $myTypeAux] == 1)} {
                        set txt "(Real $myDescription >= $sep$valueAux1$sep & Real $myDescription <= $sep$valueAux2$sep)"
                        set txtAux "(Real $myCode >= $sep$valueAux1$sep & Real $myCode <= $sep$valueAux2$sep)"
                        $widget(lbFilters) insert end [list $txt $txtAux]
                      }
                    } 
                    if {$myTypeAux == "real"} {
                      set sep ""
                      if {([TolValidateData $valueAux1 $myTypeAux] == 1) && ([TolValidateData $valueAux2 $myTypeAux] == 1)} {
                        set txt "(Real GE($myDescription, $sep$valueAux1$sep) & LE($myDescription, $sep$valueAux2$sep))"
                        set txtAux "(Real GE($myCode, $sep$valueAux1$sep) & LE($myCode, $sep$valueAux2$sep))"
                        $widget(lbFilters) insert end [list $txt $txtAux]
                      }
                    }
                  }
                }
                if { $idOperator == 8 } { ;#operator (]
                  if {$myTypeAux == "date"} {
                    if {([CheckDateTime $valueAux1]) && ([CheckDateTime $valueAux2])} {
                      set valueAux1 [DateToIso8601 $valueAux1 y%Ym%md%d]
                      set valueAux2 [DateToIso8601 $valueAux2 y%Ym%md%d]
                      set txt "(Real $myDescription > $valueAux1 & Real $myDescription <= $valueAux2)"
                      set txtAux "(Real $myCode > $valueAux1 & Real $myCode <= $valueAux2)"
                      $widget(lbFilters) insert end [list $txt $txtAux]
                    } else {
                      set msg [mc {Invalid selected DateTime}]
                      ::TolConsole::HciWriter "<W>${msg}</W>"
                      return {} 
                    }
                  } else {
                    if {$myTypeAux == "text"} {
                      set sep "\""
                      if {([TolValidateData $valueAux1 $myTypeAux] == 1) && ([TolValidateData $valueAux2 $myTypeAux] == 1)} {
                        set txt "(Real $myDescription > $sep$valueAux1$sep & Real $myDescription <= $sep$valueAux2$sep)"
                        set txtAux "(Real $myCode > $sep$valueAux1$sep & Real $myCode <= $sep$valueAux2$sep)"
                        $widget(lbFilters) insert end [list $txt $txtAux]
                      }
                    } 
                    if {$myTypeAux == "real"} {
                      set sep ""
                      if {([TolValidateData $valueAux1 $myTypeAux] == 1) && ([TolValidateData $valueAux2 $myTypeAux] == 1)} {
                        set txt "(Real GT($myDescription, $sep$valueAux1$sep) & LE($myDescription, $sep$valueAux2$sep))"
                        set txtAux "(Real GT($myCode, $sep$valueAux1$sep) & LE($myCode, $sep$valueAux2$sep))"
                        $widget(lbFilters) insert end [list $txt $txtAux]
                      }
                    }
                  }
                }
                if { $idOperator == 9 } { ;#operator [)
                  if {$myTypeAux == "date"} {
                    if {([CheckDateTime $valueAux1]) && ([CheckDateTime $valueAux2])} {
                      set valueAux1 [DateToIso8601 $valueAux1 y%Ym%md%d]
                      set valueAux2 [DateToIso8601 $valueAux2 y%Ym%md%d]
                      set txt "(Real $myDescription >= $valueAux1 & Real $myDescription < $valueAux2)"
                      set txtAux "(Real $myCode >= $valueAux1 & Real $myCode < $valueAux2)"
                      $widget(lbFilters) insert end [list $txt $txtAux]
                    } else {
                      set msg [mc {Invalid selected DateTime}]
                      ::TolConsole::HciWriter "<W>${msg}</W>"
                      return {} 
                    }
                  } else {
                    if {$myTypeAux == "text"} {
                      set sep "\""
                      if {([TolValidateData $valueAux1 $myTypeAux] == 1) && ([TolValidateData $valueAux2 $myTypeAux] == 1)} {
                        set txt "(Real $myDescription >= $sep$valueAux1$sep & Real $myDescription < $sep$valueAux2$sep)"
                        set txtAux "(Real $myCode >= $sep$valueAux1$sep & Real $myCode < $sep$valueAux2$sep)"
                        $widget(lbFilters) insert end [list $txt $txtAux]
                      }
                    } 
                    if {$myTypeAux == "real"} {
                      set sep ""
                      if {([TolValidateData $valueAux1 $myTypeAux] == 1) && ([TolValidateData $valueAux2 $myTypeAux] == 1)} {
                        set txt "(Real GE($myDescription, $sep$valueAux1$sep) & LT($myDescription, $sep$valueAux2$sep))"
                        set txtAux "(Real GE($myCode, $sep$valueAux1$sep) & LT($myCode, $sep$valueAux2$sep))"
                        $widget(lbFilters) insert end [list $txt $txtAux]
                      }
                    }
                  }
                }
                if { $idOperator == 10 } { ;#operator ()
                  if {$myTypeAux == "date"} {
                    if {([CheckDateTime $valueAux1]) && ([CheckDateTime $valueAux2])} {
                      set valueAux1 [DateToIso8601 $valueAux1 y%Ym%md%d]
                      set valueAux2 [DateToIso8601 $valueAux2 y%Ym%md%d]
                      set txt "(Real $myDescription > $valueAux1 & Real $myDescription < $valueAux2)"
                      set txtAux "(Real $myCode > $valueAux1 & Real $myCode < $valueAux2)"
                      $widget(lbFilters) insert end [list $txt $txtAux]
                    } else {
                      set msg [mc {Invalid selected DateTime}]
                      ::TolConsole::HciWriter "<W>${msg}</W>"
                      return {} 
                    }
                  } else {
                    if {$myTypeAux == "text"} {
                      set sep "\""
                      if {([TolValidateData $valueAux1 $myTypeAux] == 1) && ([TolValidateData $valueAux2 $myTypeAux] == 1)} {
                        set txt "(Real $myDescription > $sep$valueAux1$sep & Real $myDescription < $sep$valueAux2$sep)"
                        set txtAux "(Real $myCode > $sep$valueAux1$sep & Real $myCode < $sep$valueAux2$sep)"
                        $widget(lbFilters) insert end [list $txt $txtAux]
                      }
                    } 
                    if {$myTypeAux == "real"} {
                      set sep ""
                      if {([TolValidateData $valueAux1 $myTypeAux] == 1) && ([TolValidateData $valueAux2 $myTypeAux] == 1)} {
                        set txt "(Real GT($myDescription, $sep$valueAux1$sep) & LT($myDescription, $sep$valueAux2$sep))"
                        set txtAux "(Real GT($myCode, $sep$valueAux1$sep) & LT($myCode, $sep$valueAux2$sep))"
                        $widget(lbFilters) insert end [list $txt $txtAux]
                      }
                    }
                  }
                }
              }
              11 - 12 {
                # lista de valores seleccionados
                set lstValSel [$widget(val1) selection get selected [mc "Values"]]
                
                if {$myTypeAux == "date"} {
                  foreach val $lstValSel {
                    if {[CheckDateTime $val]} {
                      lappend lstValAux [DateToIso8601 $val y%Ym%md%d]
                    } else {
                      set msg [mc {Invalid selected DateTime}]
                      ::TolConsole::HciWriter "<W>${msg}</W>"
                      return {} 
                    }
                  }
                  set myLstValAux [ListToStr $lstValAux]
                } else {
                  if {$myTypeAux == "text"} {
                    set sep "\""
                  }
                  if {$myTypeAux == "real"} {
                    set sep ""
                  }
                  foreach val $lstValSel {
                    if {[TolValidateData $val $myTypeAux] == 1} {
                      lappend lstValAux "$sep$val$sep" 
                    }
                  }
                  set myLstValAux [ListToStr $lstValAux {, } {} {}]
                }
                if { $idOperator == 11 } { ;#operator IN
                  set txt "(Real $myDescription <: \[\[$myLstValAux\]\])"
                  set txtAux "(Real $myCode <: \[\[$myLstValAux\]\])"
                  $widget(lbFilters) insert end [list $txt $txtAux]
                } else {
                  set txt "(Real (Not ($myDescription <: \[\[$myLstValAux\]\])))"
                  set txtAux "(Real (Not ($myCode <: \[\[$myLstValAux\]\])))"
                  $widget(lbFilters) insert end [list $txt $txtAux] 
                }
              }
            }
          }      
          default {
            set msg [mc {You must type a language}]
            ::TolConsole::HciWriter "<W>${msg}</W>"
            return {} 
          }
        }
      } else {
        set msg [mc "You must select an operator"]
            ::TolConsole::HciWriter "<W>${msg}</W>"
            return {}
      }
    } else {
      set msg [mc "You must select any Field"]
      ::TolConsole::HciWriter "<W>${msg}</W>"
      return {}  
    }
  }

  

  #///////////////////////////////////////////////////////////////////////////
  method getfilterselected {colName} { 
  #
  # PURPOSE: Devuelve el filtro seleccionado de la lista de filtros
  #
  # PARAMETERS:
  #   colName -> Nombre de la columna que quieres recuperar: filter ó code
  #
  #///////////////////////////////////////////////////////////////////////////
    set idFilterSelect [lindex [$widget(lbFilters) curselection]]
    
    set colNameAux [string tolower [lindex $colName 0]]
    
    if {[llength $idFilterSelect]} {
      if {$colNameAux == "filter"} {
        set lst    [lbGetItems $widget(lbFilters) 1 0]
        set txt    [lindex $lst   0 0]
      }
      if {$colNameAux == "code"} {
        set lst    [lbGetItems $widget(lbFilters) 1 0]
        set txt    [lindex $lst   0 1]
      }
    }
    
    return $txt
   
  }

  #/////////////////////////////////////////////////////////////////////////////
  method OnLstFecha { sender } {
  #
  # PURPOSE: Acción que se ejecuta para cargar los valores en los comboboxes de
  #          fechas al pulsar la flecha de listar valores
  #
  # PARAMETERS:   sender --> 1 :  cbValIni
  #                          2 :  cbValEnd
  #
  #/////////////////////////////////////////////////////////////////////////////
    set i 1
		
    # elementos seleccionados
		set idFieldSelect [lindex [$widget(lbFields) curselection]]
    
    if {[llength $idFieldSelect]} {
	
      foreach ite $lstFinal {
        if {$i eq $idFieldSelect} {
          set myType     [lindex $ite 2]
          set myValue    [lindex $ite 3]
          set myFunction [lindex $ite 4]
        }
        incr i 1
      }
    }

    if { [llength $myValue] } {
      set listValAux $myValue
    } else {
      set listValAux "[eval $myFunction]"
    }
    
    if {$sender == "1"} {
      $widget(cbFecha1) configure -values $listValAux
    } else {
      $widget(cbFecha2) configure -values $listValAux
    }
  }
  
  #/////////////////////////////////////////////////////////////////////////////
  method OnLstVal { sender } {
  #
  # PURPOSE: Acción que se ejecuta para cargar los valores en los comboboxes de
  #          valores tipo texto al pulsar la flecha de listar valores
  #
  # PARAMETERS:   sender --> 1 :  cbValIni
  #                          2 :  cbValEnd
  #
  #/////////////////////////////////////////////////////////////////////////////
    set i 1
		
    # elementos seleccionados
		set idFieldSelect [lindex [$widget(lbFields) curselection]]
    
    if {[llength $idFieldSelect]} {
	
      foreach ite $lstFinal {
        if {$i eq $idFieldSelect} {
          set myType     [lindex $ite 2]
          set myValue    [lindex $ite 3]
          set myFunction [lindex $ite 4]
        }
        incr i 1
      }
    }

    if { [llength $myValue] } {
      set listValAux $myValue
    } else {
      set listValAux "[eval $myFunction]"
    }
    
    if {$sender == "1"} {
      $widget(cbValIni) configure -values $listValAux
    } else {
      $widget(cbValEnd) configure -values $listValAux
    }
  }
   
   
  #/////////////////////////////////////////////////////////////////////////////
  method OnLstMultiSelec { args } {
  #
  # PURPOSE: Acción que se ejecuta para cargar los valores en el selector
  #          múltiple de valores al pulsar los tres puntitos
  #
  # PARAMETERS:   
  #
  #/////////////////////////////////////////////////////////////////////////////
    set i 1
		
    # elementos seleccionados
		set idFieldSelect [lindex [$widget(lbFields) curselection]]
    
    if {[llength $idFieldSelect]} {
	
      foreach ite $lstFinal {
        if {$i eq $idFieldSelect} {
          set myType     [lindex $ite 2]
          set myValue    [lindex $ite 3]
          set myFunction [lindex $ite 4]
        }
        incr i 1
      }
    }

    if { [llength $myValue] } {
      set listValAux $myValue
    } else {
      set listValAux "[eval $myFunction]"
    }

    return $listValAux
  }
 
}

package provide bfilter 1.0

# ------- TEST CODE ----
#/////////////////////////////////////////////////////////////////////////////
proc ok_command { path } {
#
# PURPOSE: Funcion de test de un blistcontrol
#
#/////////////////////////////////////////////////////////////////////////////
  puts [$path getLastFilter]
}

#/////////////////////////////////////////////////////////////////////////////
proc test_bfilter {} {
#
# PURPOSE: Funcion de test de un blistcontrol
#
#/////////////////////////////////////////////////////////////////////////////
  
  # defining the widgets
  #variable data
  

  destroy .top
  toplevel .top -width 400 -height 400 
    
  TitleFrame .top.f1 -text "BFilter test"
  set f1 [ .top.f1 getframe ]
  
  set db [[$self cget -bodbcmodule] cget -connection]
  
  bfilter $f1.bfilter \
    -values [list [list newsstand Text {} {TolGetFieldValues TSubstitution newsstand ::sadd::GetDB} {Punto de Venta}] \
                  [list subst     Text {1 2 3 4} {TolGetFieldValues TSubstitution subst ::sadd::GetDB} {Substituto}]     \
                  [list dateOn    Date {} {TolGetFieldValues TSubstitution dateon    ::sadd::GetDB} {Fecha Inicio}]   \
                  [list dateOff   Date {} {TolGetFieldValues TSubstitution dateoff   ::sadd::GetDB} {Fecha Fin}]]     \
    -language "SQL" \
    -showbuttons 1 -okcommand "ok_command $f1.bfilter"
  # packing all 

  grid .top.f1 -row 0 -column 0 -sticky "new" -padx 10 -pady 10
  grid columnconfigure .top 0 -weight 1
  grid rowconfigure .top 0 -weight 0
  
  grid $f1.bfilter -row 0 -column 0 -sticky "news"
  grid columnconfigure $f1 0 -weight 1
  grid rowconfigure $f1 0 -weight 1

  puts "WIDGET bfilter: $f1.bfilter"
  
}
