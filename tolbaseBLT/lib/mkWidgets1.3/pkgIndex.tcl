package ifneeded Metawidget 1.3 "
  [list source [file join $dir metawidget.tcl]]
"

package ifneeded mkWidgets 1.3 "
  [list source [file join $dir Tooltip.tcl]]
  [list source [file join $dir Calendar.tcl]]
  [list source [file join $dir Aclock.tcl]]
  [list source [file join $dir Combobox.tcl]]
  [list source [file join $dir Scrollbox.tcl]]
  [list source [file join $dir Toolbar.tcl]]
  [list source [file join $dir Statusbar.tcl]]
  [list source [file join $dir Textframe.tcl]]
  [list source [file join $dir Window.tcl]]
  [list source [file join $dir Document.tcl]]
  [list source [file join $dir Pane.tcl]]
  [list source [file join $dir Tabcontrol.tcl]]
  [list source [file join $dir Progressbar.tcl]]
  [list source [file join $dir Gridcontrol.tcl]]
  [list source [file join $dir Listcontrol.tcl]]
  [list source [file join $dir Iconbox.tcl]]
  [list source [file join $dir Treecontrol.tcl]]
  [list source [file join $dir Ibutton.tcl]]
  [list source [file join $dir Spinentry.tcl]]

  package provide mkWidgets 1.3
"