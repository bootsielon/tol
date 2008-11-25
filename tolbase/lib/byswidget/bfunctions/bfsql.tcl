#/////////////////////////////////////////////////////////////////////////////
# FILE    : bfsql.tcl -> Sql
# PURPOSE : Bayes Function SQL
#/////////////////////////////////////////////////////////////////////////////

#///////////////////////////////////////////////////////////////////////////
proc Sql2Tol { typ } { 
#
# PURPOSE: For a SQL type returns its equivalent in TOL
#
#///////////////////////////////////////////////////////////////////////////
  set typ [string tolower $typ]
    
  switch $typ {
    "char" -
    "varchar" -
    "varchar2" -
    "bpchar" -
    "long" {
      return "Text"
    }
    "int" -
    "number" -
    "decimal" -
    "numeric" -
    "float" -
    "real" -
    "bit" -
    "integer" -
    "money" -
    "smallmoney" -
    "tinyint" -
    "double" {
      return "Real"
    }
    "date" -
    "datetime" -
    "smalldatetime" {
      return "Date"
    }
    default {
      return "Unknown"
    }
  }
}    

#///////////////////////////////////////////////////////////////////////////
proc NumSql2Tol { typ } { 
#
# PURPOSE: For a numerical SQL type returns its equivalent in TOL
#
#///////////////////////////////////////////////////////////////////////////
  switch {$typ} {
    1 - 12 {
      return "Text"
    }
    2 - 3 - 4 - 5 - 6 - 7 - 8 {
      return "Real"
    }
    9 - 10 - 11 {
      return "Date"
    }
    default {
      return "Text"
    }
  }
}    

#///////////////////////////////////////////////////////////////////////////
proc SqlListBfilter { db table } {
#
# PURPOSE: Creates the list to fill bfilter with all the columns of the
#          given table.
# 
#///////////////////////////////////////////////////////////////////////////
  set cols [$db columns $table]
  set res {}
  foreach col $cols {
    set name [lindex $col 3]
    set typ [Sql2Tol [lindex $col 5]]
    set func [list "TolGetFieldValues $table $name $db"]
    set mcname [mc $name]
    lappend res "$name $typ {} $func $mcname"
  }
  return $res
}


#///////////////////////////////////////////////////////////////////////////
proc SqlGetDriver {alias} {
#
# PURPOSE: Devuelve el driver utilizado por una conexión tclODBC dado el 
#          alias (datasource name).
# 
#///////////////////////////////////////////////////////////////////////////
  set driver ""
  set lst [database datasources]
  foreach ite $lst {
    set name [lindex $ite 0]
    if {$name eq $alias} {
      set driver [lindex $ite 1]
      break 
    }
  }
  return $driver
}

#///////////////////////////////////////////////////////////////////////////
proc SqlGetGestor {driver} {
#
# PURPOSE: Devuelve el gestor de una conexión tclODBC dado el driver
#          TODO: Verificar y ampliar con más drivers
#
# RETURNS: Devuelve un identificador de Gestor de Base de Datos o vacio
#    Mic -> Sql Server
#    Ora -> Oracle
#
#///////////////////////////////////////////////////////////////////////////
  set gestor ""

  switch -regexp $driver  {
    SQL.Server   { set gestor Mic }
    Oracle*      { set gestor Ora }
    MySql -       
    MySQL        { set gestor Mys }
    Postgre*     { set gestor Pos }
    TDS          { set gestor Mic } 
  }
  return $gestor
}

#///////////////////////////////////////////////////////////////////////////
proc SqlGetTolGestor {gestor} {
#
# PURPOSE: Devuelve el gestor asociado a TOL equivalente a gestor TCL
#          devuelto por la función SqlGetGestor
#
#///////////////////////////////////////////////////////////////////////////
  set tolGestor -1
  switch $gestor {
    Mic {set tolGestor 0}
    Ora {set tolGestor 1}
    Mys {set tolGestor 2}
    Acc {set tolGestor 3}
    Pos {set tolGestor 4}
  }
  return $tolGestor
}

#///////////////////////////////////////////////////////////////////////////
proc SqlValidateData {P typFie} {
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
  #Tolcon_Trace "SqlValidateData: Valor: $P   Tipo: $typFie"
  set ok 1
  # si no hay ningun caracter no comprobamos
  if {![string length $P]} {
    return $ok
  }
  # ralizamos las comprobacines
  set sepDec {.}
  set typeField [string tolower [lindex $typFie 0]]
  set lonField  [string tolower [lindex $typFie 1]]
  #Tolcon_Trace "SqlValidateData: typeField: $typeField   lonField: $lonField"  
  switch $typeField {
    # Integers
    "bigint" {
      # Datos enteros (números enteros) comprendidos entre -2^63
      # (-9223372036854775808) y 2^63 -1 (9223372036854775807).
      set ok [expr {([string match {[-+]} $P] || [string is int $P]) && \
                    $P>=[expr pow(-2,63)] && $P<=[expr pow(2,63)]}]
    }
    "int" {
      #Datos enteros (números enteros) comprendidos entre -2^31
      # (-2.147.483.648) y 2^31 - 1 (2.147.483.647).
      set ok [expr {([string match {[-+]} $P] || [string is int $P]) && \
                     $P>=[expr pow(-2,31)] && $P<=[expr pow(2,31)]}]
    }
    "smallint" {
      #Datos enteros comprendidos entre 215 (-32.768) y 215 - 1 (32.767).
      set ok [expr {([string match {[-+]} $P] || [string is int $P]) && \
                     $P>=-32768 && $P<=32767}]
    }
    "tinyint" {
      #Datos enteros comprendidos 0 y 255.
      set ok [expr {([string is int $P]) && $P>=0 && $P<=255}]
    } 
    "bit" {
      #Datos enteros con valor 1 ó 0
      set ok [expr {$P==0 || $P==1}]
    }
    # Decimal y numeric
    "number"   -
    "decimal"  -
    "numeric" {
      #Datos de precisión y escala numérica fijas comprendidos 
      # entre -10^38 +1 y 10^38 - 1. Hay que tener encuante la escala y la
      # precision
      if {[string length $lonField]} {
        set lst [split $lonField ,]
        set sca [expr [lindex $lst 1] + 0]
        set pre [expr [lindex $lst 0] - $sca]
        set lst [split $P $sepDec]
        set np  [string length [lindex $lst 0]]
        set ns  [expr [llength $lst] == 1 ? 0 : [string length [lindex $lst 1]]]
        set ok [expr {[string is double $P] && $pre>=$np && $sca>=$ns}]
      } else  {
        set ok [expr {[string is double $P] && \
                      $P>=[expr pow(-10,37) + 1] && \
                      $P<=[expr pow(10,37)  - 1]}]
      }
    }
    # money y smallmoney
    "money" {
      #Valores de moneda comprendidos entre -263 (-922.337.203.685.477,5808) 
      # y 263 - 1 (+922.337.203.685.477,5807), con una precisión de una 
      # diezmilésima de la unidad monetaria. El tamaño es de 8 bytes.
      set lst [split $P $sepDec]
      set np  [string length [lindex $lst 0]]
      set ns  [expr [llength $lst] == 1 ? 0 : [string length [lindex $lst 1]]]
      set ok [expr ([string is double $P] && 19>=$np && 4>=$ns)]
    }
    "smallmoney" {
      #Valores de moneda comprendidos entre - 214.748,3648 y +214.748,3647,
      # con una precisión de una diezmilésima de la unidad monetaria.
      # El tamaño de almacenamiento es 4 bytes.
      set lst [split $P $sepDec]
      set np  [string length [lindex $lst 0]]
      set ns  [expr [llength $lst] == 1 ? 0 : [string length [lindex $lst 1]]]
      set ok [expr ([string is double $P] && 10>=$np && 4>=$ns)]
    }
    # Numéricos con aproximación
    "float" {
      #Un número de punto flotante es un valor comprendido entre 
      # -1.79E + 308 y 1.79E + 308. n es el número de bits que se
      # utilizan para almacenar la mantisa del número float en notación 
      # científica y por tanto dicta su precisión y el tamaño de almacenamiento.
      # n tiene que ser un valor entre 1 y 53.
      set ok [expr {([string match {[-+]} $P] || [string is double $P]) && \
                     $P>=[expr -1.79 * pow(10,308)] && \
                     $P<=[expr  1.79 * pow(10,308)]}]
    }
    "real" {
      #Datos numéricos de punto flotante entre –3.40E + 38 y 3.40E + 38.
      # El tamaño de almacenamiento es 4 bytes. En SQL Server, el sinónimo
      # de real es float(24).
      set ok [expr {([string match {[-+]} $P] || [string is double $P]) && \
                     $P>=[expr -3.40 * pow(10,38)] && \
                     $P<=[expr  3.40 * pow(10,38)]}]
    }
    # datetime y smalldatetime
    "date" -
    "datetime"  -
    "smalldatetime" {
      #TODO: Tienen su propio componente
    }
    # Cadenas de caracteres
    "char"     -
    "varchar"  -
    "varchar2" -
    "bpchar"   -
    "text" {
      if {[string length $lonField]} {
        set ok [expr ([string len $P]<=$lonField)]
      }
    }
    "nchar"     -
    "nvarchar"  -
    "ntext" {
    # Cadenas de caracteres Unicode
      if {[string length $lonField]} {
        set ok [expr ([string len $P]<=$lonField)]
      }
    }
    "binary"     -
    "varbinary"  -
    "image" {
    # Cadenas binarias
      # TODO: de momento no es posible editar estos campos
    }
    default {puts "SqlValidateData:\n\
                   WARNING: Tipo no reconocido $typeField"} 
  }
  return $ok
}


#///////////////////////////////////////////////////////////////////////////
  proc SqlFormatField {value type gestor {format ""}} {
# PURPUSE: Formatea un campo field, en función del tipo de dato que sea
#          para su uso en un gestor de BBDD dado por gestor 
#
# PARAMETERS:
#   value  -> Valor a formatear
#   type   -> Tipo de datos (BBDD) del campo cuyo valor se va a formatear
#   gestor -> Gestor de BBDD para el que se formatea. 
#                Ora -> Oracle, Mic -> SqlServer
#   format -> Especificador de formato especial. Por ejemplo para los
#             campos de tipo fecha, pueden utilizarse formatos especiales.
#             No implementado por el momento
#
# RETURNS: Valor del campo formateado   
#
#///////////////////////////////////////////////////////////////////////////
  set valFor ""
  if {[string tolower $value] eq "null"} {
    set valFor $value
  } else {
    set typeField [string tolower [lindex $type 0]]
    Tolcon_Trace "typeField --> $typeField"
    Tolcon_Trace "type --> $type"
    switch $typeField {
      "bigint"     -
      "int"        -
      "smallint"   -
      "tinyint"    -
      "int4"   -
      "bit"        -
      "number"     -
      "decimal"    -
      "numeric"    -
      "money"      -
      "smallmoney" -
      "float"      -
      "double"     -
      "real"       { set valFor $value }
      "date"       -
      "datetime"   -
      "smalldatetime" {
        switch $gestor {
          "Mic" { set valFor "convert(datetime,'${value}',121)"            }
          "Ora" { set valFor "to_date('${value}','YYYY-MM-DD HH24:MI:SS')" }
          "Pos" { set valFor "to_date('${value}','YYYYMMDD HH24:MI:SS')" }
          "Mys" { set valFor "str_to_date('${value}','%Y%m%d %H:%i:%s')" }
          default { error "Gestor no implementado $gestor"  }
        }
      }
      "char"       -
      "varchar"    -
      "bpchar"     -
      "varchar2"   -
      "text"       -
      "nchar"      -
      "nvarchar"   -
      "ntext"      { set valFor '${value}' }
      "binary"     -
      "varbinary"  -
      "image"      { set valFor $value  }
      default      { error "SqlFormatField:\n\ERROR: Tipo no reconocido $type"} 
    }
  }
  
  Tolcon_Trace "valFor --> $valFor"  
  return $valFor
}

#///////////////////////////////////////////////////////////////////////////
  proc SqlGetDelimeterField {type} {
# PURPUSE: Comprobamos el tipo de un campo e indicamos si sus valores deben
#          ir precedidos por algun caracter especial. Por ejemplo los textos
#          van entre comillas, los numeros no cesesitan comillas
#          TODO: Igual hay que usar las las funciones de TOL 
#
# RETURN: El texto a concatenar a al valor del campo
#
#///////////////////////////////////////////////////////////////////////////
#puts "SqlGetDelimeterField: $type"
  set delimeter {}
  set typeField [string tolower [lindex $type 0]]
  switch $typeField {
    # Integers
    "bigint"   -
    "int"      -
    "smallint" -
    "int4" -
    "tinyint" {
      # no necesita ningun delimitador
    } 
    # bit
    "bit" {
      # no necesita ningun delimitador
    }
    # Decimal y numeric
    "number"   -
    "decimal"  -
    "numeric" {
      # no necesita ningun delimitador
    }
    # money y smallmoney
    "money"  -
    "smallmoney" {
      # no necesita ningun delimitador
    }
    # Numéricos con aproximación
    "float"  -
    "real" {
      # no necesita ningun delimitador
    }
    # datetime y smalldatetime
    "date" -
    "datetime"  -
    "smalldatetime" {
      set delimeter {' '}
      set gestor [string trim [TclGetVar Text DBA] {"}];#"
      switch $gestor {
        "SqlServer" {
          set delimeter {{convert(datetime,'} {',121)}}        
        }
        "Oracle" {
          set delimeter {{to_date('} {','YYYY-MM-DD HH24:MI:SS')}}        
        }
        "Pos" {
          set delimeter {{to_date('} {','YYYYMMDD HH24:MI:SS')}}        
        }
        default { error "Variable DBA (Gestor) not defined"  }
      }
     }
    # Cadenas de caracteres
    "char"     -
    "varchar"  -
    "bpchar"   -
    "varchar2" -
    "bpchar"   -
    "text" {
      set delimeter {' '}
    }
    # Cadenas de caracteres Unicode
    "nchar"     -
    "nvarchar"  -
    "ntext" {
      set delimeter {' '}
    }
    # Cadenas binarias
    "binary"     -
    "varbinary"  -
    "image" {
      # TODO: de momento no es posible editar estos campos
    }
    default { error "SqlGetDelimeterField:\n\
                   WARNING: Tipo no reconocido $typeField"} 
  }
  #puts "SqlGetDelimeterField: $delimeter"
  return $delimeter
}


#///////////////////////////////////////////////////////////////////////////
  proc DBQuery {bodbc args} {
# PURPUSE: Execute query witch bodbc module
# PARAMETERS:
#   bodbc -> name of bodbc module
#   args  -> arguments of database query

#
#///////////////////////////////////////////////////////////////////////////
  # name connection TCL
  set connection [$bodbc cget -connection]
  # connection ?
  $::project::data(bodbc) validateconnection [$bodbc cget -name]  
  # run
  eval DBQueryConnection $connection $args
}  


#///////////////////////////////////////////////////////////////////////////
  proc DBQueryConnection {connection args} {
# PURPUSE: Execute query witch database connection
# PARAMETERS:
#   connection -> name of database connection
#   args       -> arguments of database query
#
#///////////////////////////////////////////////////////////////////////////
  #NOTE:  move traces to "DBQuery"
  . configure -cursor watch
  ::blt::busy hold .
  set lst {}
  Tolcon_Trace "IniQuery: \
    [clock format [clock seconds] -format "%Y%m%d %T"]:\n[lindex $args 0]"
  if {([info exists ::project::data(SqlTra)]) && \
      ($::project::data(SqlTra))} {
    ::TolConsole::HciWriter \
      "\nIniQuery. [clock format [clock seconds] -format "%Y%m%d %T"]:\n\
      [lindex $args 0]"
  }
  # eval
  if { [catch { set lst [eval $connection $args] } result ] } {
    ShowMessage E $result .
  }  
  # trace
  if {([info exists ::project::data(SqlTra)]) && \
      ($::project::data(SqlTra))} {
    ::TolConsole::HciWriter \
      "\nEndQuery. [clock format [clock seconds] -format "%Y%m%d %T"]"
  }
  Tolcon_Trace "EndQuery: [clock format [clock seconds] -format "%Y%m%d %T"]"
  . configure -cursor ""
  ::blt::busy release .
  update
  return $lst
}


