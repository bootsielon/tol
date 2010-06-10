namespace eval ::ImageManager {
}

proc ::ImageManager::getImageResourceId { imageName } {
  set rvar "__getImageResourceId__"
  set tolExpr [ string map [ list %N $imageName %RV $rvar ] {
    Text %RV = GuiTools::ImageManager::getImageResourceId( "%N" );
  } ]
  tol::console eval $tolExpr
  set info [ tol::info variable [ list Text $rvar ] ]
  set value [ string trim [ lindex $info 2 ] \" ]
  tol::console stack release $rvar
  return $value
}