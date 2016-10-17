#/////////////////////////////////////////////////////////////////////////////
# FILE    : iniFile.tcl
# PURPOSE : This file contains a namespace called ::iniFile with its
#           functions. It's for managing ini files, adding an abstraction 
#           layer over iniparseBys.tcl.
#/////////////////////////////////////////////////////////////////////////////


#/////////////////////////////////////////////////////////////////////////////
  namespace eval ::iniFile {
#
# PURPOSE: The purpose of this namespace is organize code and functions.
#
# VARIABLES:
#   idIni -> identifier of ini file
#   idGCF -> array of identifiers of graphics configuration files
#
#/////////////////////////////////////////////////////////////////////////////
  variable idIni
  variable idGCF
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::iniFile::Create {fileName} {
#
# PURPOSE:
#
# PARAMETERS:
#
#/////////////////////////////////////////////////////////////////////////////
  variable idIni
  #Tolcon_Trace "::iniFile::Create fileName: $fileName"
  set idIni [iniparse:openfile $fileName]
  #Tolcon_Trace "idIni: $idIni"
  #bind . <Destroy> [list ::iniFile::Destroy %W]
}  


#/////////////////////////////////////////////////////////////////////////////
  proc ::iniFile::CloseAll {} {
#
# PURPOSE:
#
# PARAMETERS:
#
#/////////////////////////////////////////////////////////////////////////////
  variable idIni
  variable idGCF

  iniparse:closefile $idIni
  foreach it [array names idGCF] {
    iniparse:closefile $idGCF($it)
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::iniFile::SetGCF {key fileName} {
#
# PURPOSE:
#
# PARAMETERS:
#
#/////////////////////////////////////////////////////////////////////////////
  #Tolcon_Trace "::iniFile::SetGCF key: $key, fileName: $fileName"
  variable idIni
  variable idGCF
  if [string equal $fileName ""] {
    set idGCF($key)  $idIni
  } else {
    if { [info exist idGCF($key)] && ![string equal $idGCF($key) $idIni] } {
      iniparse:closefile $idGCF($key)
    }
    set idGCF($key) [iniparse:openfile $fileName]
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::iniFile::SaveGCF {key fileName} {
#
# PURPOSE:
#
# PARAMETERS:
#
#/////////////////////////////////////////////////////////////////////////////
  #Tolcon_Trace "::iniFile::SaveGCF key: $key, fileName: $fileName"
  variable idGCF
  ::iniFile::SetGCF $key $fileName

  if {![string equal $fileName ""]} {
    iniparse:closefile $idGCF($key)
    set idGCF($key) [iniparse:openfile $fileName]
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::iniFile::Read {key item {valueini {}}} {
#
# PURPOSE:
#
# PARAMETERS:
#
#/////////////////////////////////////////////////////////////////////////////
  #Tolcon_Trace "::iniFile::Read key: $key, item: $item, valueini: $valueini"
  variable idIni
  set value [readini $idIni $key $item]
  if {$value == "" } {
    set value $valueini
  }
  return $value
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::iniFile::ReadGCF {key item {valueini {}}} {
#
# PURPOSE:
#
# PARAMETERS:
#
#/////////////////////////////////////////////////////////////////////////////
  #Tolcon_Trace "::iniFile::ReadGCF key: $key, item: $item, valueini: $valueini"
  variable idGCF
  set value [readini $idGCF($key) $key $item]
  if {$value == "" } {
    set value $valueini
  }
  return $value
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::iniFile::Write {key item value} {
#
# PURPOSE:
#
# PARAMETERS:
#
#/////////////////////////////////////////////////////////////////////////////
  #Tolcon_Trace "::iniFile::Write key: $key, item: $item, value: $value"
  variable idIni
  writeini $idIni $key $item $value
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::iniFile::WriteGCF {key item value} {
#
# PURPOSE:
#
# PARAMETERS:
#
#/////////////////////////////////////////////////////////////////////////////
  #Tolcon_Trace "::iniFile::WriteGCF key: $key, item: $item, value: $value"
  variable idGCF
  writeini $idGCF($key) $key $item $value
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::iniFile::Flush {} {
#
# PURPOSE:
#
# PARAMETERS:
#
#/////////////////////////////////////////////////////////////////////////////
  variable idIni
  iniparse:flushfile $idIni
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::iniFile::FlushGCF {key} {
#
# PURPOSE:
#
# PARAMETERS:
#
#/////////////////////////////////////////////////////////////////////////////
  variable idGCF
  after idle iniparse:flushfile $idGCF($key)
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::iniFile::ReadWMState {iniReader section} {
#
# PURPOSE:
#
# PARAMETERS:
#   iniReader ->
#   section   ->
#
#/////////////////////////////////////////////////////////////////////////////
  set state [$iniReader $section wm,state normal]
  set state [expr {$state eq "zoomed" && $::tcl_platform(platform) eq "windows" ? \
             $state : "normal" }]
  expr {$state eq "withdrawn" ? "normal" : $state}
}