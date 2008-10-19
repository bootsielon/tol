#/////////////////////////////////////////////////////////////////////////////
# FILE    : bflist.tcl
# PURPOSE : This file 
#/////////////////////////////////////////////////////////////////////////////

#/////////////////////////////////////////////////////////////////////////////
proc ListReplace {list value newValue} {
# PURPOSE: 
#
# PARAMETERS:
#   lst -> 
#   value ->
#   newValue ->
#
# RETURN:
#
#/////////////////////////////////////////////////////////////////////////////
  set idx -1;
  if [string equal $value $newValue] {
    return $list
  }
  while { [set idx [lsearch $list $value]] != -1} {
    set list [lreplace $list $idx $idx $newValue]
  }
  return $list
}

#/////////////////////////////////////////////////////////////////////////////
proc blremove {_L args} {
# PURPOSE: 
#
# PARAMETERS:
#   _L ->    nombre completo de la lista
#   args ->  argumentos a borrar
#
# RETURN:
#
#/////////////////////////////////////////////////////////////////////////////
  upvar $_L L
  foreach i $args {
    set pos [lsearch $L $i] ;# might be -1 if not found...
    set L [lreplace $L $pos $pos] ;# ... but that's fine
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc lunion {args} {
# PURPOSE: 
#
# PARAMETERS:
#   args ->
#
# RETURN:
#
#/////////////////////////////////////////////////////////////////////////////
  switch [llength $args] {
    0 {
      return {}
    }
    1 {
      return [lindex $args 0]
    }
    default {
      foreach set $args {
        foreach e $set {
          set tmp($e) .
        }
      }
      return [array names tmp]
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc lreverse {_L} {
# PURPOSE: 
#
# PARAMETERS:
#   _L ->
#
# RETURN:
#
#/////////////////////////////////////////////////////////////////////////////
  set res {}
  set i [llength $_L]
  while {$i} {lappend res [lindex $_L [incr i -1]]} 
  set res
}

#/////////////////////////////////////////////////////////////////////////////
proc ljoin {_L1 _L2 sep {empty 0} {tab 0}} {
#
# PURPOSE: Junta las dos variables en un elemento separandolos por los
#          caracteres que se le indique
#
# PARAMETERS:
#   _L1 -> Primera lista
#   _L2 -> Segunda lista
#   sep -> Caracteres de separacion
#   empty -> Introducimos un primer elemento vacio. 0:False, 1:True
#   tab -> Tabulamos los elementos. 0:False, 1:True
#
# RETURN:
#
#///////////////////////////////////////////////////////////////////////////
  set lista {}
  if {$empty} {
    set lista {""}
  }

  foreach i $_L1 j $_L2 {
    if {$tab} {
      set len [expr 10 - [string length $i]]
      set elem [format "%-*s %s %s" $len $i $sep $j]
    } else  {
      set elem "$i$sep$j"
    }
    lappend lista $elem
  }
  return $lista
}

#/////////////////////////////////////////////////////////////////////////////
proc lsub {_L1 _L2} {
#
# PURPOSE: Devuelve el resultado de restar a _L1 _L2
#
# PARAMETERS:
#   _L1 -> Primera lista
#   _L2 -> Segunda lista
#
# RETURN: La listta resultado de la resta
#
#/////////////////////////////////////////////////////////////////////////////
  
  foreach i $_L2 {
    set ini [lsearch $_L1 $i]
    if { $ini != -1 } {
	  set _L1 [lreplace $_L1 $ini $ini ]
    }
  }

  return $_L1
}

#/////////////////////////////////////////////////////////////////////////////
proc ltraspose {_L} {
#
# PURPOSE: transpone los valores de una lista. 
#   NOTA. SOLO FUNCIONA SI LAS LISTAS SON TODAS DE LA MISMA DIMENSION O LA 
#   PRIMERA ES MAYOR O IGUAL QUE LAS DEMAS
#
# PARAMETERS:
#   _L -> lista original
#
# RETURN: la lista transpuesta
#
#/////////////////////////////////////////////////////////////////////////// 
  # lista a devolver
  set lst ""
  # comprobamos que la lista no esta vacia, numero de columnas resultado
  set r [llength $_L]
  if {$r} {
    #set c [llength [lindex $_L $row]] 
    set c [llength [lindex $_L 0]]
    if {$c} {
      for {set col 0} {$col < $c} {incr col} {
        set laux ""
        for {set row 0} {$row < $r} {incr row} {
          set dt [lindex [lindex $_L $row] $col]
          lappend laux $dt
          #append [lindex $lst $row] [lindex [lindex $_L $row] $col]
	    }
        lappend lst $laux
      }
    }
  }
  return $lst
}

#/////////////////////////////////////////////////////////////////////////////
proc lfilter {_L first skip} {
#
# PURPOSE: Filtra por indice los valores de una lista:
#
# PARAMETERS:
#   _L -> lista original
#   first -> Primer valor que se devuelve, a partir de 1.
#   skip -> Número de valores que no se muestran en cada salto
#
# RETURN: la lista transpuesta
#
#/////////////////////////////////////////////////////////////////////////// 
  # lista a devolver
  set lst ""
  incr skip
  # comprobamos que la lista no esta vacia, numero de columnas resultado
  set r [llength $_L]
  if {$r} {
    for {set i $first} {$i < $r} {incr i $skip} {
      lappend lst [lindex $_L $i]
    }
  }
  return $lst
}

#/////////////////////////////////////////////////////////////////////////////
proc lmaximo {_L} {
#
# PURPOSE: Busca el elemento mayor de la lista
#
# PARAMETERS:
#   _L -> lista original
#
# RETURN: el elemento mayor
#
#/////////////////////////////////////////////////////////////////////////// 
  # elemento
  set e [lindex $_L 0]
  # comprobamos que la lista no esta vacia, numero de columnas resultado
  foreach ele $_L {
    if {[expr $ele>$e]} {
      set e $ele 
    }
  }
  return $e
}

#/////////////////////////////////////////////////////////////////////////////
proc lminimo {_L} {
#
# PURPOSE: Busca el elemento menor de la lista
#
# PARAMETERS:
#   _L -> lista original
#
# RETURN: el elemento menor
#
#/////////////////////////////////////////////////////////////////////////// 
  # elemento
  set e [lindex $_L 0]
  # comprobamos que la lista no esta vacia, numero de columnas resultado
  foreach ele $_L {
    if {[expr $ele<$e]} {
      set e $ele 
    }
  }
  return $e
}
