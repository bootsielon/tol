#/////////////////////////////////////////////////////////////////////////////
# FILE    : iniparseBys.tcl
# PURPOSE : This file has functions to parse and create a .ini file, it 
#           doesn't make modifications directly on file, but on memory.
#           It has been rewritten in Bayes because original was inefficient.
#/////////////////////////////////////////////////////////////////////////////

package provide iniparse 1.4

#/////////////////////////////////////////////////////////////////////////////
#
# iniparse is a global array used to store information about ini file:
#
#   freeid                  -> next free id for ini file
#   idlist                  -> list of ids of ini files
#   $id,fname               -> filename
#   $id,data                -> each entry of the file are read into an item
#                              in this list as
#                              iniparse($id,data,keyname,itemname)
#   $iniparse($id,sections) -> this is a list containing the different
#                              sections of the ini file
#   $iniparse($id,flags)    -> this is a list containing these flags:
#                              RDONLY : If this flag is set, iniparse:flush
#                                       will not write anything over file, but
#                                       still writeini will work (only in
#                                       memory)
#
#/////////////////////////////////////////////////////////////////////////////

set iniparse(freeid) 0
set iniparse(idlist) {}


#/////////////////////////////////////////////////////////////////////////////
  proc iniparse:openfile {file args} {
#
# PURPOSE: Reads an ini file into memory, for later access (such as read or
#          write), and return an id which can be later used for
#          writing/reading data, and flushing/closing file.
#          Notes:
#          - It doesn't leave the file open, just reads into memory, and
#          returns an id.
#          - If RDONLY is specified (case-sensetive), then calls to writeini
#          will work only in memory, and changes won't be saved with
#          iniparse:flush, nor iniparse:closefile.
#
# PARAMETERS:
#   file: file's name
#   args: RDONLY flag, optional
#
# RETURN: The file's id
#
#/////////////////////////////////////////////////////////////////////////////
  global iniparse
  set fileId [open $file {RDONLY CREAT}]
  set sections {}
  set keyName ""

  set id $iniparse(freeid)
  incr iniparse(freeid)

  while {![eof $fileId]} {
    set input [gets $fileId]
    if {[regexp {\[(.*)\]} $input --> keyName]} {
      if {[set idx [lsearch $sections $keyName]] == -1} {
        lappend sections $keyName
      }
    } elseif {[regexp {(.*)=(.*)} $input --> itemName value] } {
      set iniparse($id,data,$keyName,$itemName) $value
    }
  }
  close $fileId

  set iniparse($id,fname) $file
  set iniparse($id,sections) $sections
  set iniparse($id,flags) $args
  lappend iniparse(idlist) $id
  return $id
}


#/////////////////////////////////////////////////////////////////////////////
  proc iniparse:flushfile {id} {
#
# PURPOSE: Writes an ini file in memory into its file, without removing data 
#          from memory.
#
# PARAMETERS:
#   id: file id
#
#/////////////////////////////////////////////////////////////////////////////
  global iniparse

  if {[lsearch $iniparse(idlist) $id] == -1} return     ;# No such Id

  if {[lsearch $iniparse($id,flags) RDONLY] != -1} return ;# ReadOnly-Flag

  set fileId [open $iniparse($id,fname) {WRONLY CREAT TRUNC}]
  
  foreach section $iniparse($id,sections) {
    #puts $fileId ""
    puts $fileId "\[$section\]"
    regsub -all $id,data,$section, [array names iniparse $id,data,$section,*] "" itemNames
    foreach it [lsort $itemNames] {
      puts $fileId "$it=$iniparse($id,data,$section,$it)"
    }
  }

  close $fileId

  set iniparse($id,flags) [lreplace $iniparse($id,flags) [set idx [lsearch $iniparse($id,flags) CHANGED]] $idx]
}


#/////////////////////////////////////////////////////////////////////////////
  proc iniparse:closefile {id} {
#   
# PURPOSE: Flushes an ini file in memory into its file, and then removes data 
#          from memory.
#
# PARAMETERS:
#   id: file id
#
#/////////////////////////////////////////////////////////////////////////////
  global iniparse
  if {[lsearch $iniparse(idlist) $id] == -1} return     ;# No such Id

  iniparse:flushfile $id
  unset iniparse($id,fname)
  foreach section $iniparse($id,sections) {
    regsub -all $id,data,$section, [array names iniparse $id,data,$section,*] "" itemNames
    foreach it $itemNames {
      unset iniparse($id,data,$section,$it)
    }
  }
  unset iniparse($id,sections)
  unset iniparse($id,flags)
  set iniparse(idlist) [lreplace $iniparse(idlist) [set idx [lsearch $iniparse(idlist) $id]] $idx]
}


#/////////////////////////////////////////////////////////////////////////////
  proc readini {id args} {
#
# PURPOSE: Usage:  readini <fileId> [ [<keyname>] [<itemname>] ]
#          Reads <itemname> under <keyname> from <fileId>. If it cannot find
#          file, <itemname>, or <keyname>, returns an empty string.
#          If itemname is absent, returns list of all items under keyname.
#          Notes:
#          - This procedure doesn't read directly from the file, but from the
#          memory-version of the file. If directly the file (not memory
#          version) is modified after the call to iniparse:openfile, this
#          procedure will not see the changes in the file, but only changes
#          in memory-version.
#          See: writeini iniparse:openfile
#
# PARAMETERS:
#   id: file id
#   args: [[<keyname>] [<itemname>]]
#
#/////////////////////////////////////////////////////////////////////////////
  global iniparse
  set keyfound 0 ; set itemfound 0
  set key [lindex $args 0]
  set item [lindex $args 1]

  if {[lsearch $iniparse(idlist) $id] == -1} {return ""}   ;# No such Id

  if {[llength $args] != 2} {
    regsub -all $id,data,$key, [array names iniparse $id,data,$key,*] "" itemNames
    return $itemNames
  }
  if {![info exist iniparse($id,data,$key,$item)]} {
    return ""
  }
  return $iniparse($id,data,$key,$item)
}


#/////////////////////////////////////////////////////////////////////////////
  proc writeini {id key item value} {
#
# PURPOSE:	Writes/modifies <item> under <key> in <id> with <value>. If cannot
#         	find <item>, or <key>, creates a new one.
#          Notes:
#          - This procedure doesn't write directly to the file, but to the
#          memory-version of the file. If directly the file (not memory
#          version) is modified after the call to iniparse:openfile, this
#          procedure will not see the changes in the file, but only changes
#          in memory-version.
#          See: readini iniparse:openfile
#
# PARAMETERS:
#   id:    file id
#   key:   key name
#   item:  item name
#   value: item value
#
#/////////////////////////////////////////////////////////////////////////////
  global iniparse

  if {[lsearch $iniparse(idlist) $id] == -1} return   ;# No such Id

  set iniparse($id,data,$key,$item) $value

  if {[set idx [lsearch $iniparse($id,sections) $key]] == -1} {
     lappend iniparse($id,sections) $key
  }
}

