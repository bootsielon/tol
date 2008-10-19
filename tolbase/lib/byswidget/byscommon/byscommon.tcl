package require Tk
package require BWidget
package require msgcat
package require snit

Bitmap::use

set byswidget_path [file join [file dir [info script]] ..]

set dir_imgs [file normalize [file join $byswidget_path images]]
if {[lsearch -exact $Bitmap::path $dir_imgs] == -1} {
  lappend ::Bitmap::path $dir_imgs
  #Text manager:
  catch {[namespace import msgcat::*]}
  #::msgcat::mclocale es
  ::msgcat::mcload [file join $byswidget_path msgs]
}




package provide byscommon 1.0
