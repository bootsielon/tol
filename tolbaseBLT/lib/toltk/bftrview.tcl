#/////////////////////////////////////////////////////////////////////////////
# FILE    : bftrview.tcl
# PURPOSE : This file contains some utility functions for treeviews
#           manipulation or generation.
#/////////////////////////////////////////////////////////////////////////////


#/////////////////////////////////////////////////////////////////////////////
  namespace eval ::BayesTreeview {
# 
# PURPOSE: The purpose of this namespace is organize code and functions and
#          provides functions to manage treeviews.
#
#/////////////////////////////////////////////////////////////////////////////
}

#/////////////////////////////////////////////////////////////////////////////
# Functions that convert between treeviews and especification files.
#/////////////////////////////////////////////////////////////////////////////
#   File is similar to a .ini file. There are two special items: ('children'
#  and 'label') for each node.  Here's an example:
#         [Root]
#           Children=1
#           Label=Nodo raiz
#           Type=
#           Data1=bla,bla,...
#           DataN=bla,bla,...
#         [1]
#           Children=
#           Label=Label del nodo 1
#           Type=
#           Data1=bla,bla,...
#           DataN=bla,bla,...
#
#/////////////////////////////////////////////////////////////////////////////

#/////////////////////////////////////////////////////////////////////////////
  proc ::BayesTreeview::TreeviewFromFile {tv path {proc ""}} {
#
# PURPOSE: Loads a treeview info data from a text especification file of a
#          treeview. 
#
# PARAMETERS:
#   tv   -> treeview's path
#   path -> name of file
#   proc -> procedure that converts type node to the path of the image of node
#
# RETURN: Returns 1 if treeview has been loaded correctly or 0 if not.
#
#/////////////////////////////////////////////////////////////////////////////
  if ![file exists $path] {
    return -code error "File $path doesn't exist"
  }
  Tolcon_Trace "Llamada a TreeviewFromFile"
  set fd [iniparse:openfile $path] 
  NodeFromFile $tv root "" $fd $proc
  iniparse:closefile $fd
  return 1
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::BayesTreeview::TreeviewToFile {tv path} {
#
# PURPOSE: Saves a treeview info data into a text especification file of a
#          treeview.
#
# PARAMETERS:
#   tv   -> treeview's path
#   path -> name of file
#
# RETURN: Returns 1 if file has been saved correctly or 0 if not.
#
#/////////////////////////////////////////////////////////////////////////////
  #borrar archivo pq iniparse:openfile abre para solo lectura si existe
  file delete $path
  set fd [iniparse:openfile $path] 
  NodeToFile $tv root $fd
  iniparse:closefile $fd
  return 1
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::BayesTreeview::NodeToFile {tv index fd} {
#
# PURPOSE: Saves a treeview's node info data into a text file.
#
# PARAMETERS:
#   tv    -> treeview's path
#   index -> treeview node index
#   fd    -> channel identifier of text file
#
#/////////////////////////////////////////////////////////////////////////////
  set key $index

  # Guardar data del nodo actual
  array set aryData [$tv entry cget $index -data]
  foreach item [array names aryData] {
    writeini $fd $key $item $aryData($item)
  }
  
  writeini $fd $key Label [$tv entry cget $index -label]
  # Guardar sus hijos
  writeini $fd $key Children [$tv entry children $index]

  # Llamar recursivamente para los hijos
  foreach child [$tv entry children $index] {
    NodeToFile $tv $child $fd
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::BayesTreeview::NodeFromFile {tv index parent fd {imgFromType ""}} {
#
# PURPOSE: Loads a treeview's node info data from a text file.
#
# PARAMETERS:
#   tv     -> treeview's path
#   index  -> treeview node index
#   parent -> treeview node's parent index
#   fd     -> channel identifier of text file
#
#/////////////////////////////////////////////////////////////////////////////
  set key $index

  if ![string equal $parent ""] {
    set idx [$tv insert -at $parent end $index ]
  } else {
    set idx root
  }
  # Configurar data del nodo actual
  foreach item [readini $fd $key] {
    set value [readini $fd $key $item]
    if {[lsearch -exact [$tv column names] $item] != -1} {
      $tv entry configure $idx -data [list $item $value]    
    }
    if [string equal $item "Label"] {
      $tv entry configure $idx -label $value
    }
    if {[string equal $item "Type"] && ![string equal $imgFromType ""]} {
      if ![string equal [set img [$imgFromType $value]] ""] {
        $tv entry configure $idx -icons [list $img $img]
      }
    }
  }
  # Llamar recursivamente para los hijos
  foreach child [readini $fd $key Children] {
    NodeFromFile $tv $child $idx $fd $imgFromType
  }
}
