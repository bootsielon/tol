#/////////////////////////////////////////////////////////////////////////////
# FILE    : bfstring.tcl
# PURPOSE : This file contains some utility functions for strings manipulation
#           or generation.
#/////////////////////////////////////////////////////////////////////////////


#/////////////////////////////////////////////////////////////////////////////
  proc ListToStr { list {sep {, }} {ant {}} {sig {}} } {
#
# PURPOSE: Obtiene un string concatenando los elementos de una lista
#          separándolos entre ellos por <sep>, anteponiendo <ant> y posponiendo
#          <sig> a cada uno de ellos.
# 
# RETURN: The string created.
#
#/////////////////////////////////////////////////////////////////////////////
  set result {}
  set coma {}
  foreach it $list {
    append result $coma $ant $it $sig
    set coma $sep
  }
  return $result
}


#/////////////////////////////////////////////////////////////////////////////
  proc ListToStrStruct {list structure} {
#
# PURPOSE: Obtiene un string concatenando los elementos de una lista 
#          separándolos sucesivamente según se establezca en el parámetro
#          structure.
#
# PARAMETERS: 
#  list      -> lista con los elementos a concatenar
#  structure -> lista de listas de la forma {n sep ini end} donde:
#    n   -> indice cada cuantos elementos separa con sep, ini y end
#    sep -> separador
#    ini -> string a concatenar al principio de cada grupo de elementos
#    end -> string a concatenar al final de cada grupo de elementos
#
#/////////////////////////////////////////////////////////////////////////////
  set structureLevel [lindex $structure 0]
  foreach {n sep ini end} $structureLevel {
    set result $ini
    if {[llength $structure] > 1} {
      set newStructure [lrange $structure 1 end]
      set newList [lrange $list 0 [expr $n-1]]
      append result [ListToStrStruct $newList $newStructure]
      for {set i $n} {$i < [llength $list]} {incr i $n} {
        append result $end $sep $ini
        set newList [lrange $list $i [expr $i+$n-1]]
        append result [ListToStrStruct $newList $newStructure]
      }
    } else {
      append result [lindex $list 0]
      for {set i 1} {$i < [llength $list]} {incr i} {
        if {([expr $i % $n ]==0)} {
          append result $end $sep $ini
        } else {
          append result " "
        }
        append result [lindex $list $i]
      }  
    }
    append result $end
  }
  return $result    
}


#/////////////////////////////////////////////////////////////////////////////
  proc DateTol {date} {
#
# PURPOSE: Creates a date string in tol format. e.j. y2002m11d23
#
# PARAMETERS:
#   date: list with at least 3 elements (year month day)
#
#/////////////////////////////////////////////////////////////////////////////
  set newDate [FormatDate $date]
  set y [lindex $newDate 0]
  set m [lindex $newDate 1]
  set d [lindex $newDate 2]

  return "y${y}m${m}d${d}"
}

#/////////////////////////////////////////////////////////////////////////////
  proc FormatDate {date} {
#
# PURPOSE: Returns a date (list) formated as YYYY MM DD
#
# PARAMETERS:
#   date: list with at least 3 elements (year month day)
#
#/////////////////////////////////////////////////////////////////////////////
  set year  [lindex $date 0]
  set month [lindex $date 1]
  set day   [lindex $date 2]

  if {[string range $month 0 0] eq 0} {
    set month [string range $month 1 1]
  }
  if {[string range $day 0 0] eq 0} {
    set day [string range $day 1 1]
  }
  return [list $year [format "%02d" $month] [format "%02d" $day]]
}

#/////////////////////////////////////////////////////////////////////////////
proc StringMapToWidget {txt} {
#
# PURPOSE: Converts and string into a valid name for a widget
#
# PARAMETERS:
#   txt: Text origin
#
# RETURN: the modified text
#
#/////////////////////////////////////////////////////////////////////////////
  string map {" " "" "*" "" "$" ""} [string tolower $txt]
}


#/////////////////////////////////////////////////////////////////////////////
  proc StringReplace {txt old new} {
#
# PURPOSE: It replaces in a text the characters indicated by a new text
#
# PARAMETERS:
#   txt: Text origin
#   old: Text to replace
#   new: Text to insert
#
# RETURN: the modified text
#
#/////////////////////////////////////////////////////////////////////////////
  set pos [string first $old $txt]
  while {[expr $pos != -1]} {
    set txt [string replace $txt $pos [expr $pos + [string length $old] - 1] $new]
    incr pos [string length $new]
    set pos [string first $old $txt $pos]
  }
  return $txt
}

#/////////////////////////////////////////////////////////////////////////////
  proc CheckDateTime { datetime } {
#
# PURPOSE: Check if a string has the following formats:
#          yyyy/mm/dd
#          yyyy-mm-dd
#          yyyymmdd
#          HH:MM:SS
#          yyyy/mm/dd:HH:MM:SS
#          yyyy-mm-dd:HH:MM:SS
#          yyyymmdd:HH:MM:SS
#         
# PARAMETERS:
#   datetime: Text to check
#
# RETURN: 1 if parameter is a valid datetime
#         0 if parameter is not a valid datetime
#
#/////////////////////////////////////////////////////////////////////////////
  return [regexp \
    {((^([0-9]{4})([/-]?)(0[1-9]|1[0-2])([/-]?)(0[1-9]|[12][0-9]|3[01])$)|(^([0-1][0-9]|2[0-3]):([0-5][0-9]):([0-5][0-9])$)|(^([0-9]{4})([/-]?)(0[1-9]|1[0-2])([/-]?)(0[1-9]|[12][0-9]|3[01]):([0-1][0-9]|2[0-3]):([0-5][0-9]):([0-5][0-9])$))} \
          $datetime]
}

#/////////////////////////////////////////////////////////////////////////////
  proc DateToIso8601 { datetime format } {
#
# PURPOSE: Get a valid date with / without format from a datetime
#
# PARAMETERS:
#   datetime: Text to check
#   format  : format to apply
#
# RETURN: datetime
#
#/////////////////////////////////////////////////////////////////////////////
  regsub  {[:h]} $datetime "T" dateT
    
  regsub -all {[/\.ymdis]} $datetime "" iso8601
    
  set mask 00000101T00:00:00
  set addmask [string range $mask [string length $iso8601] end]
      
  set res [join "$iso8601 $addmask" ""]
  
  if {$format == ""} {
    return $res
  } else {
    return [clock format [clock scan $res] -format $format]
  } 

  return $res
  
}


#/////////////////////////////////////////////////////////////////////////////
proc Ver2Num { version } {
#
# PURPUSE: Convert a text that represents a version (separated numbers by '.')
#          to number to be able to make comparisons between versions 
#/////////////////////////////////////////////////////////////////////////////
  set result [Ver2NumGen $version . 4]
  return $result
}

#/////////////////////////////////////////////////////////////////////////////
proc Ver2NumGen { version sepVer padVer } {
#  
# PURPUSE: It turns a chain that represents a version (separated numbers by a
#          separator) number to be able to make comparisons between versions.
#          The maximum number of digitos of the elements of the version can be
#          indicated
# PARAMETERS:
#   version -> Version in format x.y...
#   sepVer  -> Separator of elements within the version
#   padVer  -> Maximum number of digitos for a part of version
#/////////////////////////////////////////////////////////////////////////////
  set setNum [split $version $sepVer]
  set lstNum ""
  foreach item $setNum {
    set itemExt "[string repeat 0 $padVer]$item"
    lappend lstNum \
      [string range $itemExt [expr [string length $itemExt] - $padVer] \
      [string length $itemExt]]
  }
  set num [string trimleft [join $lstNum {}] 0]
  if {[string is double $num]} {
    return $num
  } else {
    return 0 
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ObtainRelativePath {path referencePath} {
#
# PURPOSE: Express an absolute path in relative terms to a referencePath.
#
# PARAMETERS:
#   path -> Absolute input path which is to be expressed in relative terms
#   referencePath -> Absolute path to which the expression to be computed
#                    refers to
#
#   If any of the input paths is not absolute an internal fatal error is
#   produced
#
# RETURN:
#          It returns a relative path to referencePath if path and
#          referencePath are in the same unit, otherwise it returns path.
#
#/////////////////////////////////////////////////////////////////////////////  
  # Check if two input paths are absolute ones
  if {[file pathtype $path] != "absolute"} {
    error "$path is not an absolute path"
  }
  if {[file pathtype $referencePath] != "absolute"} {
    error "$referencePath is not an absolute path"
  }

  # Check whether the unit is the same for both paths or not
  if {[lindex [file split $path] 0] != \
      [lindex [file split $referencePath] 0]} {
    # Just return the input path in negative case
    return $path
  } else {
    # Compute the relative way from one to the other
    set sPath [file split $path]
    set sReferencePath [file split $referencePath]
    
    set index 0
    foreach level $sPath {
      if {[lindex $sPath $index] != [lindex $sReferencePath $index]} {
        break
      } else {
        incr index
	if {$index >= [llength $sReferencePath]} {
          break
        }
      }
    }
    set levelsToClimb [expr [llength $sReferencePath] - $index]

    set computedPath {}

    if {$levelsToClimb > 0} {
      for {set i 0} {$i < $levelsToClimb} {incr i} {
        lappend computedPath ".."
      }
    }

    for {set i $index} {$i < [llength $sPath]} {incr i} {
        lappend computedPath [lindex $sPath $i]
    }

    return [eval file join $computedPath]
  }
}