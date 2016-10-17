#
# Este archivo contiene la implementacion de ImageManager por omision
# que necesita tolinspe.tcl desde el GuiTools
#

package require BWidget

namespace eval ::ImageManager {
}

proc ::ImageManager::getIconForInstance { objAddress } {
  return [ ::Bitmap::get "NameBlock" ]
}
