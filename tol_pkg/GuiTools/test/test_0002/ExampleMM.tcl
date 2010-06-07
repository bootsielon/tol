set mydir [ file normalize [ file dir [ info script ] ] ]

if { [ llength [ info command tol::info ] ] } {
    set exampleTol [ file join $mydir "ExampleMM.tol" ]
    set menuManagerTol [ file join $mydir "MenuManager.tol" ]
    if { ![ catch { tol::info included $exampleTol } ] } {
	tol::decompile $exampleTol
    }
    if { ![ catch { tol::info included $menuManagerTol } ] } {
	tol::decompile $menuManagerTol
    }
    tol::include $menuManagerTol
    tol::include $exampleTol
}

source [ file join $mydir "MenuManager.tcl" ]

set selection [ list ]
foreach o { ej11 ej12 ej13 ej21 ej22 } {
  lappend selection [ tol::info address [ list NameBlock $o ] ]
}

destroy .exampleMM

set menuRoot [ menu .exampleMM -tearoff 0 ]

MenuManager::initTypesInfoFromSelection $selection

MenuManager::insertEntriesForSelection $menuRoot $selection