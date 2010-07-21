namespace eval ::ImageManager {
}

proc ::ImageManager::getImageResourceId { imageName } {
  set rvar "__getImageResourceId__"
  set tolExpr [ string map [ list %N $imageName %RV $rvar ] {
    Text %RV = {
      Real test = ObjectExist( "Code", "GuiTools::ImageManager::getImageResourceId" );
      Text If( test, GuiTools::ImageManager::getImageResourceId( "%N" ), "" )
    }
  } ]
  tol::console eval $tolExpr
  set info [ tol::info variable [ list Text $rvar ] ]
  set value [ string trim [ lindex $info 2 ] \" ]
  tol::console stack release $rvar
  return $value
}

proc ::ImageManager::getIconForClass { className } {
  set rvar "__getIconForClass__"
  set tolExpr [ string map [ list %N $className %RV $rvar ] {
    Text %RV = {
      Real test = ObjectExist( "Code", "GuiTools::ImageManager::getIconForClass" );
      Text imageName = If( test, 
                           GuiTools::ImageManager::getIconForClass( "%N" ), 
                           "" );
      Text GuiTools::ImageManager::getImageResourceId( imageName )
    }
  } ]
  tol::console eval $tolExpr
  set info [ tol::info variable [ list Text $rvar ] ]
  set value [ string trim [ lindex $info 2 ] \" ]
  tol::console stack release $rvar
  return $value
}
