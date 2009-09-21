#/////////////////////////////////////////////////////////////////////////////
# FILE    : bftoltcl.tcl -> BfT
# PURPOSE : Bayes Function TolTcl. Functions for easy Tol-Tcl comunication
#/////////////////////////////////////////////////////////////////////////////

variable TclData

#///////////////////////////////////////////////////////////////////////////
  proc TolDateFormat2TclDateFormat {format} {
# PURPUSE: Translates TolDateFormat to TclDateFomat
#
# PARAMETERS:
#   format : TOL date format
# RETURN: Tcl date format
#
#///////////////////////////////////////////////////////////////////////////
  regsub  %u $format "" format
  regsub  %c $format "" format
  regsub  %W $format "%A" format
  regsub  %N $format "%B" format
  regsub  %w $format "%a" format
  regsub  %n $format "%b" format
  regsub  %h $format "%H" format
  regsub  %i $format "%M" format
  regsub  %s $format "%S" format
  return $format
}

#/////////////////////////////////////////////////////////////////////////////
  proc GetUniqueName {gra {seed _TOLTK}} {
#
# PURPOSE: Gets a unique Name for a given grammar. Optionally user can give
#          the initial part of the name
#
# PARAMETERS:
#   gra : TOL Grammar 
#   seed: initial part of the name
#
#/////////////////////////////////////////////////////////////////////////////
  set i 1
  set end 0
  set name $seed
  while {[string equal $end 0]} {
    if [catch {::tol::info variable $gra $name}] {
      set end 1
    } else {
      set name ${seed}_$i
      incr i
    }
  }
  return $name
}

#/////////////////////////////////////////////////////////////////////////////
  proc GetGlobalUniqueName {{seed _TOLTK}} {
#
# PURPOSE: Gets a unique Name independent of grammar. Optionally user can give
#          the initial part of the name
#
# PARAMETERS:
#   seed: initial part of the name
#
#/////////////////////////////////////////////////////////////////////////////
  set i 1
  set end 0
  set name $seed
  # list of grammars
  set lstGra [::tol::info grammars]

  while {[string equal $end 0]} {
    set end 1
    foreach gra $lstGra {
      set inf [TclInfoVar $gra $name]
      if {[string length $inf]} {
        set end 0
      }
    }
    if { !$end } {
      set name ${seed}_$i
    }
    incr i
  }
  return $name
}

#/////////////////////////////////////////////////////////////////////////////
proc TclInfoVar {gra obj} {
#
# PURPOSE: Get the value of variable TOL
#
# PARAMETERS:
#   gra: grammar of variable TOL
#   obj: name of variable TOL
#
# RETURN: output [::tol::info var obg] information in "array get"
#
#/////////////////////////////////////////////////////////////////////////////
  # eval
  if {![catch {set lst [::tol::info variable [list $gra $obj]]}]} {
    set lstNam [list CONTENT PATH DESC INDEXES ISFILE\
                     HASSUBSET SUBTYPE STRUCT]
    for {set i 0} {$i < [llength $lstNam]} {incr i} {
      set inf([lindex $lstNam $i]) [lindex $lst [expr $i+2]]
    }
    #parray inf
    return [array get inf]
  }
  # error
  return {}
}


#/////////////////////////////////////////////////////////////////////////////
proc TclInfoRef {obj args} {
#
# PURPOSE: Get the value for Set TOL
# 
# PARAMETERS:
#   obj:  reference of TCL
#   args: index of set
# 
# RETURN: output [::tol::info var ref] information in "array get"
#
#/////////////////////////////////////////////////////////////////////////////
  # eval
  set cmd "::tol::info variable {$obj $args}"
  #puts "TclInfoRef: cmd=$cmd"
  if {![catch {set lst [eval $cmd]}]} {
    set lstNam [list GRAMMAR NAME CONTENT PATH DESC ISFILE\
                     HASSUBSET SUBTYPE STRUCT]
    for {set i 0} {$i < [llength $lstNam]} {incr i} {
      set inf([lindex $lstNam $i]) [lindex $lst [expr $i]]
    }
    #parray inf
    return [array get inf]
  }
  return {}
}


#/////////////////////////////////////////////////////////////////////////////
proc TclInfoFun {gra fun} {
#
# PURPOSE: Get the value of Set TOL
#
# PARAMETERS:
#   gra: grammars of functions TOL
#   fun: name of function
#
# RETURN: output [::tol::info functions gre fun] information in "array get"
#
#/////////////////////////////////////////////////////////////////////////////
  # eval
  set cmd "::tol::info functions $gra $fun"
  #puts "cmd: $cmd"
  if {![catch {set lst [eval $cmd]}]} {
    set lstNam [list CONTENT PATH DESC]
    for {set i 0} {$i < [llength $lstNam]} {incr i} {
      set inf([lindex $lstNam $i]) [lindex $lst $i]
    }
    #parray inf
    return [array get inf]
  }
  return {}
}

#/////////////////////////////////////////////////////////////////////////////
proc TclInfoInc {fil} {
#
# PURPOSE: Get the value of file included in TOL
#
# PARAMETERS:
#   fil: name of file included en TOL
#
# RETURN: output [::tol::info included fil] information in "array get"
#
#/////////////////////////////////////////////////////////////////////////////
# eval
  set cmd "::tol::info included $fil"
  #puts "cmd: $cmd"
  if {![catch {set lst [eval $cmd]}]} {
    set lstNam [list CONTENT PATH DESC HASSUBSET SUBTYPE STRUCT]
    for {set i 0} {$i < [llength $lstNam]} {incr i} {
      set inf([lindex $lstNam $i]) [lindex $lst $i]
    }
    #parray inf
    return [array get inf]
  }
  return {}
}

proc TclFindObjectRoot { ref } {
  set type [ lindex $ref 0 ]
  if { $type eq "File" } {
    set ref_root [ lrange $ref 0 2 ]
  } else {
    set ref_root [ lrange $ref 0 1 ]
  }
  # verifico si ref_root es un objeto con nombre
  set info_root [ ::tol::info var $root ]
  set name_root [ lindex $info_root 1 ]
  if { $name_root eq "" } {
    # el objeto raiz no tiene nombre, solo nos salvamos si la raiz es
    # un File y bajando por el camino encontramos algun global con
    # nombre
    if { $type eq "File" } {
      ...
    }
  } else {
    # bingo! la raiz de ref tiene nombre
    return $name_root
  }
}

proc TclRefToNameTol_ { ref } {
  set type [ lindex $ref 0 ]
  switch $type {
    Console {
      set root [ lrange $ref 0 1 ]
      set info_root [ ::tol::info var $root ]
      # verifico si tiene nombre
      set name_root [ lindex $info_root 1 ]
      if { $name_root eq "" } {
        # el objeto raiz no tiene nombre, solo nos salvamos si el
        # objeto referenciado es global y tiene nombre
        set info_all [ ::tol::info var $ref ]
        set grammar_ref [ lindex $info_all 0 ]
        set name_ref [ lindex $info_all 1 ]
        if {  $name_ref eq "" } {
          # no podemos hacer nada, no existe expresion de tol que
          # pueda recuperar el valor de la referencia ref
          return ""
        } else {
          # chequeamo si es un objeto global con nombre $name.
          if { [ catch { ::tol::info var [ list $grammar_ref $name_ref ] } ] } {
            # no se ha podido encontrar $name_ref como variable global en
            # $grammar_ref
            return ""
          } else {
            # hemos encontrado un objeto en $grammar_ref con nombre
            # $name_ref. Dado el uso interno de PutName, eso no
            # asegura que el objeto referenciado en $ref sea el
            # mismo. Ya seria mala pata!!!!!
            return $name_ref
          }
        }
      } else {
        # el objeto raiz tiene nombre, ya vamos mejorando ...
        set name $name_root
        foreach i [ lrange $ref 2 end ] {
          append name "\[$i\]"
        }
        return $name
      }
    }
    File {
    }
    Set {
    }
    NameBlock {
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc TclRefToNameTol { ref } {
#
# PURPOSE: Obtain the name of an object TOL, of the form, name of the joint
#          father, if it has, and indices of the set, if it has
#
# PARAMETERS:
#   ref: reference to object TOL
#
# RETURN: name in format: namaVariable (variable without father)
#                         namVarFat[x][y][z] (variable with father)
#/////////////////////////////////////////////////////////////////////////////
  set ref [lindex $ref 0]
  #puts "TclRefToNameTol: ref=$ref"
  # the object belongs to a set?
  if {[expr [llength $ref] == 1]} {
    set namTol $ref
  } else  {
    #array set infRef [TclInfoRef [lrange $ref 0 1]]
    array set infRef [TclInfoRef $ref]
    set namTol $infRef(NAME)
    #puts "TclRefToNameTol: namTol=$namTol"
    # name ?
  #if {[string length $namTol]} {
      # ckeck parameters
      #if {[expr [llength $ref] > 2]} {
        #set lstInx [lrange $ref 2 end]
        #foreach ind $lstInx {
          #set namTol $namTol\[$ind\]
        #}
      #}
    #} else  {
      #set namTol {}
    #}
  #}
  #puts "TclRefToNameTol: namTol=$namTol"
  return $namTol
}


#/////////////////////////////////////////////////////////////////////////////
proc TclGetVar {gra obj {property CONTENT}} {
#
# PURPOSE: Get a property of a variable TOL (default: returns value)
#
# PARAMETERS:
#   gra: grammar of variable TOL
#   obj: name of variable TOL
#   pro: property to return
#
# RETURN: the property value requested of the TOL object
#
#/////////////////////////////////////////////////////////////////////////////
  array set tmparray [TclInfoVar $gra $obj]
  set property [string toupper $property]
  if { [lsearch [array names tmparray] $property] != -1 } {
    return $tmparray($property)
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc TclGetRef {obj args {property CONTENT}} {
#
# PURPOSE: Get a property of a TOL reference (default: returns value)
#
# PARAMETERS:
#   obj: name of TOL object
#   args: list with reference indexes
#   property: property to return
#
# RETURN: the property value requested of the TOL object
#
#/////////////////////////////////////////////////////////////////////////////
  array set tmparray [eval TclInfoRef $obj $args]
  set property [string toupper $property]
  if { [lsearch [array names tmparray] $property] != -1 } {
    return $tmparray($property)
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc TclGetFromSet { type set args } {  
#
# PURPOSE: returns the value of a reference in a set that is a real date or
#          serie element
# ARGUMETS:
#  type: grammar of the element beeing accesed in the set
#  set:  name of the tol set
#  args: names and indices for indexing the set
#
#/////////////////////////////////////////////////////////////////////////////
  switch -regexp type {
    [Rr]eal { set type Real }
    [Tt]ext -
    [Tt]exto { set type Text }
    [Dd]ate -
    [Ff]echa { set type Date }
  }

  set name [GetGlobalUniqueName _TclGetFromSet]
  set str "$type $name = $set"
  foreach arg $args {
    if { ! [regexp "^\[0-9\]*$" $arg] } {
      set str [concat $str "\[\"$arg\"\]"]
    } else {
      set str [concat $str "\[$arg\]"]
    }
  }

  ::tol::console eval $str
  set result [lindex [::tol::info var [ list $type $name ] ] 2]
  ::tol::console stack release $name
  return $result
}


#/////////////////////////////////////////////////////////////////////////////
proc TclLst2TolSet {lst args} {
#
# PURPOSE: returns a TOL code that when is evaluated creates a set equivalent
#          to a tcl list.
# ARGUMETS:
#   lst  -> List to convert to TOL set (code)
#   args  -> Function's arguments as pairs "-key value"
#    -level  : Number of sublist levels to transform in TOL Set
#    -grammar: Gramatica de los elementos del ultimo nivel
#    -struct : Name of struct
#
#/////////////////////////////////////////////////////////////////////////////
  array set opt [list \
    -level   1 \
    -grammar Text \
    -struct  ""
  ]
  array set opt $args
  
  # name of struct ?
  if {[string length $opt(-struct)]} {
    set struct $opt(-struct)
  } else {
    switch -- $opt(-grammar) {
      Real     { set struct SetOfReal }
      Date     { set struct SetOfDate }
      default  { set struct SetOfText }
    }
  }  
  if {![llength $lst]} {
    return "Copy(Empty)"
  }
  if [expr $opt(-level) <= 1] {
    switch -- $opt(-grammar) {
      Real     { set code "${struct}([join $lst ,])" }
      Date     { set code "${struct}([join $lst {,}])" }
      default  { set code "${struct}(\"[join $lst \",\"]\")" }
    }
  } else {
    set sSet ""
    foreach ite $lst {
      lappend sSet [TclLst2TolSet $ite \
        -level [expr $opt(-level)-1] \
        -grammar $opt(-grammar)      \
        -struct  $opt(-struct) ]
    }
    set code "SetOfSet([join $sSet ,])"
  }
  return $code
}

#/////////////////////////////////////////////////////////////////////////////
proc TolSet2TclLst {settol} {
#
# PURPOSE: returns a TCL code that when is evaluated creates a set equivalent
#          to a tol set.
# ARGUMETS:
#   settol  -> set tol
#
#/////////////////////////////////////////////////////////////////////////////  
  set name [GetGlobalUniqueName __TolSet2TclLst__]
  set cmd  "Text $name = TolSet2TclLst($settol);"
  ::tol::console eval $cmd
  set result [TclGetVar Text $name]
  ::tol::console stack release $name
  if {[string length $result]} {
    eval set result $result
  }
  return $result
}

#/////////////////////////////////////////////////////////////////////////////
proc TclGetIdFromName { name cjto {refs ""}} {
#
# PURPOSE: returns the access index equivalent to name in the set cjto
# ARGUMETS:
#  name: a text indexing the set that should be converted to index
#  cjto: the TOL set
#  refs: names and indices indexing the set
#
#/////////////////////////////////////////////////////////////////////////////
  if { $refs ne "" } {
    if { [TclGetRef $cjto $refs GRAMMAR] != "Set" } { return }
    set n [lindex [TclGetRef $cjto $refs] 0]
  } else {
    set n [lindex [TclGetVar Set $cjto] 0]
  }
  
  for {set i 1} { $i <= $n } { incr i } {
    if { [TclGetRef $cjto [linsert $refs end $i] NAME] == $name } {
      return $i
    }
  } 
}

#/////////////////////////////////////////////////////////////////////////////
proc TclInfoExists {gra obj} {
#
# PURPOSE: Get the exits of variable TOL
#
# PARAMETERS:
#   gra: grammar of variable TOL
#   obj: name of variable TOL
#
# RETURN: 1 -> Exists
#         0 -> No
#
#/////////////////////////////////////////////////////////////////////////////
  # eval
  expr { ! [ catch { tol::info var [ list $gra $obj ] } ] }
}


#/////////////////////////////////////////////////////////////////////////////
proc TclSetVar {gra obj value} {
#
# PURPOSE: Get the exits of variable TOL
#
# PARAMETERS:
#   gra: grammar of variable TOL
#   obj: name of variable TOL
#   value: value of variable
#
# RETURN: 1 -> Exists
#         0 -> No
#
#/////////////////////////////////////////////////////////////////////////////
  # exists ?
  if {[TclInfoExists $gra $obj]} {
    set cmd "$gra $obj := $value ;"
  } else {
    set cmd "$gra $obj = $value ;"
  }
  # eval
  ::tol::console eval $cmd
}


#/////////////////////////////////////////////////////////////////////////////
proc TclTolEval {cmd} {
#
# PURPOSE: Execute sentence TOL
#
# PARAMETERS:
#   cmd: sentenc
#
#/////////////////////////////////////////////////////////////////////////////
  # trace
  if {([info exists ::project::data(EvalTra)]) && \
      ($::project::data(EvalTra))} {
    ::TolConsole::HciWriter "$cmd \n"
  }  
  ::tol::console eval $cmd
}



#///////////////////////////////////////////////////////////////////////////
proc TolValidateData {P typFie} {
#
# PURPOSE: Verifies that a given value observes the rules of a data type
#
# PARAMETERS:
#   P      : value to verify
#   typFie : data type
#
# RETURN: 1 -> OK, 0 -> NO
#
#///////////////////////////////////////////////////////////////////////////
  set ok 1
  # si no hay ningun caracter no comprobamos
  if {![string length $P]} {
    return $ok
  }
  # ralizamos las comprobacines
  set sepDec {.}
  set typeField [string tolower [lindex $typFie 0]]
  set lonField  [string tolower [lindex $typFie 1]]
 
  switch $typeField {    
    "real" {
      #Datos numéricos de punto flotante entre –3.40E + 38 y 3.40E + 38.
      # El tamaño de almacenamiento es 4 bytes. En SQL Server, el sinónimo
      # de real es float(24).
      set ok [expr {([string match {[-+]} $P] || [string is double $P]) && \
                     $P>=[expr -3.40 * pow(10,38)] && \
                     $P<=[expr  3.40 * pow(10,38)]}]
    }
    
    "text" {
      # Todo tipo de cadenas de caracteres
      
      set ok 1
    }
    
    "date" {
      # Todo tipo de cadenas de caracteres
      
      set ok 1
    }
    
    default {puts "TolValidateData:\n\
                   WARNING: Tipo no reconocido $typeField"} 
  }
}

  proc TolGetFieldValues { tableName field getConnection} {
    
    set txtSql \
    "select distinct $field \n \
     from $tableName \n \
     order by $field"
    # se ejecuta la query 
    if { [catch { set lstInfoValues [$getConnection $txtSql] } result ] } {         
        ShowMessage E "$result" "." ;# si ha habido error, se muestra
    } else { ;# devolvemos el valor
      # tratamos la lista de valores para no tener { } en valores con espacios
      set lstInfoVal {}
      foreach it $lstInfoValues {
        lappend lstInfoVal [lindex $it 0]
      }
      return $lstInfoVal
      #return $lstInfoValues
    }   
  }

#///////////////////////////////////////////////////////////////////////////
  proc TolGetDelimeterField {type} {
# PURPUSE: Comprobamos el tipo de un campo e indicamos si sus valores deben
#          ir precedidos por algun caracter especial. Por ejemplo los textos
#          van entre comillas, los numeros no cesesitan comillas
#          TODO: Igual hay que usar las las funciones de TOL 
#
# RETURN: El texto a concatenar a al valor del campo
#
#///////////////////////////////////////////////////////////////////////////

  set delimeter {}
  set typeField [string tolower [lindex $type 0]]
  switch $typeField {
    "real" {
      # no necesita ningun delimitador
    }
    # datetime y smalldatetime
    "date" {
      set delimeter {' '}
      set gestor [string trim [TclGetVar Text DBA] {"}]; #"

      switch $gestor {
        "SqlServer" {
          set delimeter {{convert(datetime,'} {',121)}}        
        }
        "Oracle" - "Pos" {
          set delimeter {{to_date('} {','yyyy/mm/dd')}}        
        }
      }
     }
    # Cadenas de caracteres
    "text" {
      set delimeter {' '}
    }
    
    
    default {puts "TolGetDelimeterField:\n\
                   WARNING: Tipo no reconocido $typeField"} 
  }
  #puts "TolGetDelimeterField: $delimeter"
  return $delimeter
}

#///////////////////////////////////////////////////////////////////////////
proc EvalTolExp {grammar expression {seed _tempName}} {
# PURPOSE: Evaluates a tol expression $expression from grammar $grammar
# RETURNS: Content of evaluated expression
#
#///////////////////////////////////////////////////////////////////////////
  set nRes [GetUniqueName $grammar $seed]
  set cmd "$grammar $nRes = $expression;"
  ::tol::console eval $cmd
  array set info [TclInfoVar $grammar $nRes]
  eval "set lstRes $info(CONTENT)"
  ::tol::console stack release $nRes
  return $lstRes
}

#///////////////////////////////////////////////////////////////////////////
proc TolObj2TclObj { obj } {
# PURPUSE: Dado una referencia a un objeto TOL retorna una representacion en
#          TCL equivalente. Esta funcion debe reemplazar a TolSet2TclLst.
#
# RETURN: Un objeto Tcl que representa el contenido del objeto TOL
#
#///////////////////////////////////////////////////////////////////////////
 #puts "obj = $obj"
  set vinfo [ tol::info variable $obj ]
  set grammar [ lindex $vinfo 0 ]
  if { $grammar eq "Set" || $grammar eq "NameBlock" } {
    set result [ list ]
    set idx 1
    tol::foreach it $obj {
      array set it_info $it
      #puts "$it_info(grammar) $it_info(name) $it_info(content) "
      set G $it_info(grammar)
      if { $G eq "Set" || $G eq "NameBlock" } {
        set value [ TolObj2TclObj [ eval list $obj $idx ] ]
      } else {
        set value $it_info(content)
      }
      lappend result $value
      incr idx
    }
    set result
  } else {
    lindex $vinfo 2
  }
}

#///////////////////////////////////////////////////////////////////////////
proc TolObj2TclObjNamed { obj } {
# PURPUSE: Dado una referencia a un objeto TOL retorna una representacion en
#          TCL equivalente manteniendo los nombres de los objetos TOL.
#          Si alguno de los objetos no tiene nombre se le asigna el nombre "".
#          Esta funcion puede reemplazar a TolSet2TclLst.
#
# RETURN: Un lista Tcl de pares <nombre,valor> que representa el contenido
#         con nombredel objeto TOL
#
#///////////////////////////////////////////////////////////////////////////
  #puts "obj = $obj"
  set vinfo [ tol::info variable $obj ]
  set grammar [ lindex $vinfo 0 ]
  if { $grammar eq "Set" || $grammar eq "NameBlock" } {
    set result [ list ]
    set idx 1
    tol::foreach it $obj {
      array set it_info $it
      #puts "$it_info(grammar) $it_info(name) $it_info(content) "
      set G $it_info(grammar)
      if { $G eq "Set" || $G eq "NameBlock" } {
        set value [ TolObj2TclObjNamed [ eval list $obj $idx ] ] 
      } else {
         set value $it_info(content)
      }
      lappend result $it_info(name) $value
      incr idx
    }
    set result
  } else {
    lrange $vinfo 1 2
  }
}

proc loadtt { } {
  package require Toltcl
  tol::initkernel
  tol::initlibrary
}

proc test1.1 { } {
  TolObj2TclObj {Set BdtChartMet}
}

proc test1.2 { } {
  TolObj2TclObjNamed {Set BdtChartMet}
}

proc test2.1 { } {
  tol::console eval {
    Set {[[ Set kk = Empty ]]}
  }
  set result [ TolObj2TclObj {Console 1} ]
  tol::console stack release
  return $result
}

proc test2.2 { } {
  tol::console eval {
    Set {[[ Set kk = Empty ]]}
  }
  set result [ TolObj2TclObjNamed {Console 1} ]
  tol::console stack release
  return $result
}

proc test3.1 { } {
  tol::console eval {
    Set {[[ 1, 2, Set kk = [[ 3, 4 ]] ]]}
  }
  set result [ TolObj2TclObj {Console 1} ]
  tol::console stack release
  return $result
}

proc test3.2 { } {
  tol::console eval {
    Set {[[ 1, 2, Set kk = [[ 3, 4 ]] ]]}
  }
  set result [ TolObj2TclObjNamed {Console 1} ]
  tol::console stack release
  return $result
}
