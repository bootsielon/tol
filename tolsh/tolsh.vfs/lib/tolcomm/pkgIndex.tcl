
  package ifneeded tolserver 1.0 [list source [file join $dir tolserver.tcl]]
  package ifneeded tolshared 1.0 [list source [file join $dir tolshared.tcl]]
  package ifneeded tolslave 1.0 [list source [file join $dir tolslave.tcl]]
  package ifneeded tolclient 1.0 [list source [file join $dir tolclient.tcl]]
  package ifneeded rmtps_client 1.0 [list source [file join $dir rmtps_client.tcl]]

