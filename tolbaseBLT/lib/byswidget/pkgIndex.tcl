#Tcl package index file, version 1.1

# Extend the auto_path to make tcllib packages available
if {[lsearch -exact $::auto_path $dir] == -1} {
  lappend ::auto_path $dir
}

set maindir $dir
foreach d {
  bfunctions
  bcombobox
  bcomboboxplus
  beditmaster
  bentrydate
  bfilter
  blistbox
  blistboxplus
  bselectfields
  btoplevel
  btreeview
  btreeviewplus
  bguifunction
  bmultiselect
  beditselector
  bodbc
  bsersel
  btable
  barima
  bdialog
  btab
} {
  set dir [file join $maindir $d]
  source [file join $dir pkgIndex.tcl]
}
package ifneeded byswidget 1.0 [list source [file join $maindir loadByswidget.tcl]]
unset maindir
