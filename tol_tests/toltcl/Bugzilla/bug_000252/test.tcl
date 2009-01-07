tol::include binormal_MH.tol 
tol::decompile binormal_MH.tol
tol::include binormal_MH.tol
set d1 [lindex [tol::info variable [list Real d1] ] 0]
set d2 [lindex [tol::info variable [list Real d2] ] 0]
set quality 1.0
set partial "d1 = $d1, d2 = $d2"