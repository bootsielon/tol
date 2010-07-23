package require BWidget

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

proc ::ImageManager::getIconForInstance { objAddress } {
  set rvar "__getIconForInstance__"
  set tolExpr [ string map [ list %A $objAddress %RV $rvar ] {
    Text %RV = {
      Real test = ObjectExist( "Code", "GuiTools::ImageManager::getIconForInstance" );
      // obtengo el nombre del imagen
      Text imageName = If( test, {
                           NameBlock obj = GetObjectFromAddress( "%A" );
                           Text GuiTools::ImageManager::getIconForInstance( obj )
                          }, "" );
      // obtengo el id del recurso que es el id de la imagen Tk
      Text GuiTools::ImageManager::getImageResourceId( imageName )
    }
  } ]
  tol::console eval $tolExpr
  set info [ tol::info variable [ list Text $rvar ] ]
  set value [ string trim [ lindex $info 2 ] \" ]
  tol::console stack release $rvar
  return [ expr {$value eq "" ? [::Bitmap::get "NameBlock"] : $value} ]
}
