#/////////////////////////////////////////////////////////////////////////////
# FILE    : bfSql.tcl
# PURPOSE : 
#/////////////////////////////////////////////////////////////////////////////

#/////////////////////////////////////////////////////////////////////////////
proc bfSqlDistinctTable {tb cp whe dist {desc false}} {
# PURPOSE:
#   Construye y ejecuta una consulta a la base de datos para una tabla y
#   muestra los campos que le indicamos
# PARAMETERS:
#   tb -> nombre de la tabla
#   cp -> lista de los campos avisualizar
#   whe -> condicion where
#   dist -> TRUE: hace un distinct, FALSE: no
#   desc -> TRUE: order by descendente, FALSE: por defecto orden ascendente 
# RETURN:
#   El resultado de la query
#/////////////////////////////////////////////////////////////////////////////
  error "Esta funcion no debe usarse"
  variable db
  set fields   [ListToStr $cp ", "] 
  if {$dist} {
    set distinct "distinct"
  }
  if {$desc} {  
    set ordBy "order by $fields desc"
  } else  {
    set ordBy "order by $fields"
  }
  
  set strSql \
    "select $distinct $fields \
     from $tb \
     $whe \
     $ordBy"

  set x [db $strSql]
  return $x
}