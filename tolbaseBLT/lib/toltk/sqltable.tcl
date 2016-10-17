#/////////////////////////////////////////////////////////////////////////////
# FILE    : sqltable.tcl
# PURPOSE : This file
#           It's a child of BayesTable so inherites all its functionalities.
#/////////////////////////////////////////////////////////////////////////////

package require bcombobox

# load the helper tcl files
catch {
  source [file join [file join $::toltk_library tclodbc] tclIndex]
}

namespace eval ::SQLTable {
  variable num_SQL 0
  variable tmpdata
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::SQLTable::TableBayesSQL { {qry ""} } {
#
# PURPOSE: 
#
#/////////////////////////////////////////////////////////////////////////////
  variable num_SQL
  variable tmpdata
    
  incr num_SQL
  # connection
  set tmpdata(bodbc$num_SQL) [$::project::data(bodbc) connection \
      -name BayesSQL_$num_SQL \
      -stdtolcon 0  \
      -stdtclcon 1  \
      -iniconfig SQLTable \
      -savealias 1  \
      -saveuser  1]
  Tolcon_Trace "SQLTable.TableBayesSQL : \
  tmpdata(bodbc$num_SQL)=$tmpdata(bodbc$num_SQL)"
  # create
  set tl [::project::CreateForm \
    -width 900 -height 600 \
    -title [mc "Bayes SQL ($num_SQL)"] \
    -iniconfig SQLTable \
    -closecmd [list ::SQLTable::Destroy $tmpdata(bodbc$num_SQL)] \
    -type SQL -helpkey "Tb4Sql" ]
  set tlf [$tl getframe]
  set this [::SQLTable::Create $tlf.t $qry]
  $tl bind <Control-F4> [list $tl kill]
  pack $tlf.t -fill both -expand yes
  
  # query ?
  if {[string length $qry]} {
    set w [::BayesTable::GetWidget $this txt]
    $w insert end $qry
    ::BayesText::Refresh $w
  }

  # Fill TreView
  $tmpdata(bodbc$num_SQL) configure \
    -procconnect [list ::SQLTable::FillTreeView $this]

  #???::BayesTable::SetData $this toplevel $tl
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::SQLTable::Create {path {query ""}} {
#
# PURPOSE: 
#
# PARAMETERS:
#   path  ->
#   query ->
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpdata
  variable num_SQL

  set this [::BayesTable::Create $path \
    -clientproc "::SQLTable::CreateWidget" -child SQLTable \
    -OnWriteIni "::SQLTable::WriteIni"]
  ReadIni   $this
  Configure $this

  ::BayesTable::SetOption $this "db" $tmpdata(bodbc$num_SQL)

  #Inicializamos id_after a "" (para que al salir sin ejecutar no de error)
  ::BayesTable::SetData $this id_after ""
  #Añadimos opciones específicas de BayesSQL
  ::BayesTable::AddOptionCmd $this "sepSQL1" "--" 
  ::BayesTable::AddOptionCmd $this "sepSQL2" "--"
  ::BayesTable::AddOptionCmd $this "db" \
    "::SQLTable::OnConnect" \
    -where tool -icon db -hint [mc "Connect Options"] -notparamthis 1
  ::BayesTable::AddOptionCmd $this "open"    "::SQLTable::OpenQuery"\
    -where tool -icon bopen -hint [mc "Open query"]
  ::BayesTable::AddOptionCmd $this "save"    "::SQLTable::SaveQuery"\
    -where tool -icon bsave -hint [mc "Save query"]
  ::BayesTable::AddOptionCmd $this "exec"    "::SQLTable::ExecQuery"\
    -where tool -icon play -hint [mc "Execute query"]
  ::BayesTable::AddOptionCmd $this "object"  "::SQLTable::ObjectInspector"\
    -where tool -icon objectexplorer -hint [mc "Object inspector"]

  return $this
}

#/////////////////////////////////////////////////////////////////////////////
proc ::SQLTable::OnConnect {this} {
#/////////////////////////////////////////////////////////////////////////////
  set module [::BayesTable::GetOption $this "db"]
Tolcon_Trace "Nombre conexion SQL: $module"
  $::project::data(bodbc) showdialog [$module cget -name]
}

#/////////////////////////////////////////////////////////////////////////////
proc ::SQLTable::Destroy {connect} {
#/////////////////////////////////////////////////////////////////////////////
  # unregister connection ODBC
  $::project::data(bodbc) unregister $connect
  return 1
}
#/////////////////////////////////////////////////////////////////////////////
  proc ::SQLTable::OnDelete {this} {
#
# PURPOSE: 
#
# PARAMETERS:
#   this  -> Instance of BayesTable
#
#/////////////////////////////////////////////////////////////////////////////
  set w [::BayesTable::GetWidget $this txt]
  after cancel [::BayesTable::GetData $this id_after]
  ::BayesTable::SetOption $this txt,font [$w cget -font]
  destroy [::BayesTable::GetData $this toplevel]
  return 1
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::SQLTable::CreateWidget {this} {
#
# PURPOSE: 
#
# PARAMETERS:
#   this  -> Instance of BayesTable
#
#/////////////////////////////////////////////////////////////////////////////
  global toltk_options
  set w [::BayesTable::GetClientArea $this]

  set pw [PanedWindow $w.pw1 -pad 0 -side top]
  ::BayesTable::SetWidget $this PW $pw
  set pan0 [$pw add -weight 1]  
  set pan1 [$pw add -weight 5]
  
  set pw2 [PanedWindow $pan1.pw -pad 0 -side right]
  set pane1 [$pw2 add -weight 3]
  set pane2 [$pw2 add -weight 1]

  #En el primer panel del pan1 creamos la tabla
  ::BayesTable::CreateWidget $this $pane1

  #En el segundo panel del pan1 creamos el text para editar las SQL
  set sw [ScrolledWindow $pane2.sw -ipad 0 -borderwidth 0 -scrollbar vertical ]

  #Creamos un text con resaltado SQL
  set w_text [::BayesText::CreateHLText $sw.t sql]

  set m [::BayesText::MenuCreate $w_text]

  $m insert 0 separator
  $m insert 0 command -label [mc Refresh]   -accelerator F5\
     -command [list ::BayesText::Refresh $w_text]
  $m insert 0 command -label [mc "Execute query"] -accelerator F9\
            -command [list ::SQLTable::ExecQuery $this]
  
  bind $w_text <F5> [list ::BayesText::Refresh $w_text]

  bind $w_text <F9> [list ::SQLTable::ExecQuery $this]
  ::BayesTable::SetWidget $this txt $sw.t

  $sw setwidget $sw.t
  pack $sw  -fill both -expand yes
  pack $pw2 -fill both -expand yes

    
  #En el pan0 creamos un TreeView para visualizar informacion de las tablas
  # treeview y scroll
  set swtv [ScrolledWindow $pan0.sw -ipad 0 -borderwidth 0 -scrollbar both ]
  set tv   [blt::treeview $swtv.tv -hideroot no -activeicons ""\
            -selectborderwidth 0\
            -highlightthickness 0 -font $toltk_options(fonts,Label)]

  bind $tv  <Button-1> "::focus $tv"
  ::BayesTable::SetWidget $this tv $tv
  $tv style configure text -bg white     
  $tv entry configure root -label [mc "Not connected"]
  $swtv setwidget $swtv.tv
  pack $swtv  -fill both -expand yes
  pack $pw -fill both -expand yes
}



#/////////////////////////////////////////////////////////////////////////////
  proc ::SQLTable::OpenQuery {this} {
#
# PURPOSE: Loads a query from a file to the text widget of Bayes SQL
#
# PARAMETERS:
#   this  -> Instance of BayesTable
#
#/////////////////////////////////////////////////////////////////////////////
  set txt [::BayesTable::GetWidget $this txt]
  set typelist {{"SQL Files" {".sql"}}}
  set path [tk_getOpenFile -title [mc "Open"] \
                           -parent $txt \
                           -defaultextension .sql -filetypes $typelist]
  if {$path != ""} {
    $txt delete 1.0 end
    if [::BayesText::Open $txt $path] {
    } else {
    }
    tk::TextSetCursor $txt 1.0
  }  
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::SQLTable::SaveQuery {this} {
#
# PURPOSE: Saves text of text widget of Bayes SQL.
#
# PARAMETERS:
#   this  -> Instance of BayesTable
#
#/////////////////////////////////////////////////////////////////////////////
  set txt [::BayesTable::GetWidget $this txt]

  set typelist {{"SQL Files" {".sql"}}}
  set path [tk_getSaveFile -title [mc "Save as"] -parent $txt \
                           -defaultextension .sql -filetypes $typelist]
  if {![string equal $path ""] } {
    set ok [::BayesText::Save $txt $path]
  } else {
    set ok 0
  }
  return $ok
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::SQLTable::ExecQuery {this} {
#
# PURPOSE: Executes a query of BayesSQL
#
# PARAMETERS:
#   this  -> Instance of BayesTable
#
#/////////////////////////////////////////////////////////////////////////////
  set t [::BayesTable::GetTkTable $this]

  set w [::BayesTable::GetWidget $this txt]
  set txt [::BayesText::GetSelectionOrAll $w]
  # unselected table
  $t selection clear all

  set dbConnect [::BayesTable::GetOption $this db]
  set status [$::project::data(bodbc) checkconnectiontcl $dbConnect]
  if {!$status} {
    tk_messageBox -type ok -icon warning -title [mc BayesSQL] -parent $t \
          -message [mc "You must set connection first"]
    ::SQLTable::OnConnect $this
    return
  }

  [$dbConnect cget -connection] statement stmt $txt
  stmt execute

  set rowCount [stmt rowcount]
  puts "rowCount=$rowCount"
  if {$rowCount == -1} {
    ::BayesTable::WriteInfo $this "Consulta ejecutada correctamente"
  } else  {
    ::BayesTable::WriteInfo $this "Completado proceso. $rowCount filas afectadas"
  }
  
  # Borrar mensaje.Cancelamos el comando anterior (si lo hubiese)
  after cancel [::BayesTable::GetData $this id_after]
  set id_after [after 5000 [list ::BayesTable::WriteInfo $this ""]]
  ::BayesTable::SetData $this id_after $id_after


  $t configure -state normal
  ::BayesTable::UnPackTkTable $this
  tclodbc::TkTableInit $t stmt
  # Si la instruccion sql es select se llena la tabla
  if {$rowCount == -1} {
    tclodbc::TkTableRead $t stmt
  }
  # TODO: Se puede hacer q se muestre un mensaje mas visible que la 
  # barra de estado para insert,delete, update
  ::BayesTable::AdjustTable $this
  ::BayesTable::PackTkTable $this
  $t configure -state disabled
  ::BayesTable::UpdateInfoRows $this
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::SQLTable::ObjectInspector {this} {
#
# PURPOSE: 
#
# PARAMETERS:
#
#/////////////////////////////////////////////////////////////////////////////
  set pw [::BayesTable::GetWidget $this PW]

  set dbConnect [::BayesTable::GetOption $this db]
  set status    [$::project::data(bodbc) checkconnectiontcl $dbConnect]
 
  if {$status} {
    ::PanedWindow::MoveSash $pw 1 x middle
  } else {
    ::PanedWindow::MoveSash $pw 1 x min
  }
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::SQLTable::ConnectOptions_ {this} {
#
# PURPOSE: 
#
# PARAMETERS:
#   this  -> Instance of BayesTable
#
# TODO: ESTE PROCEDIMIENTO NO SE DEBE USAR, UTILIZAR EL BODBC
#/////////////////////////////////////////////////////////////////////////////
  variable tmpdata

  set table [::BayesTable::GetTkTable $this]
  set dlg [Dialog ._cnnopt -title [mc "Connect options"] \
               -parent $table -default 0 -cancel 1 -modal local]
  
  $dlg add -image [::Bitmap::get accept] -helptext [mc "Accept"] \
      -relief link -command [list ::SQLTable::OnConnectOk $this]
  $dlg add -image [::Bitmap::get cancelx] -helptext "Cancelar" -relief link\
      -command "$dlg enddialog cancel"
  
  set w [$dlg getframe]
  wm resizable $dlg 0 0
  catch {
      array unset tmpdata
  }
  set tmpdata(alias)    [::BayesTable::GetOption $this alias]
  set tmpdata(user)     [::BayesTable::GetOption $this user]
  set tmpdata(password) [::BayesTable::GetOption $this password]
  set tmpdata(dlg)      $dlg
  Label $w.lbalias -text "[mc Alias]:"

  #bcombobox $w.cbalias -editable false \
  #   -textvariable ::SQLTable::tmpdata(alias) \
  #   -bkeypress true
  ComboBox $w.cbalias -editable false -textvariable ::SQLTable::tmpdata(alias)
  set tmpdata(cbalias) $w.cbalias
  Label $w.lbuser -text "[mc User]:"
  Entry $w.euser  -textvariable ::SQLTable::tmpdata(user)
  Label $w.lbpass -text "[mc Password]:"
  Entry $w.epass  -show "*" -textvariable ::SQLTable::tmpdata(password) 

  set tmpdata(datasources) [lsort -index 0 [database datasources]] 
  #Tolcon_Trace "DataSources : $tmpdata(datasources)"
  set all_alias ""
  foreach alias $tmpdata(datasources) {
    lappend all_alias [lindex $alias 0]
  }
  $w.cbalias configure -values $all_alias
  grid $w.lbalias -row 0 -column 0 -sticky e -padx 4
  grid $w.cbalias -row 0 -column 1 -sticky w -padx 2
  grid $w.lbuser  -row 1 -column 0 -sticky e -padx 4
  grid $w.euser   -row 1 -column 1 -sticky w -padx 2
  grid $w.lbpass  -row 2 -column 0 -sticky e -padx 4
  grid $w.epass   -row 2 -column 1 -sticky w -padx 2
  grid rowconfigure $w 1 -pad 6

  set result [$dlg draw]
  #Tolcon_Trace "draw == $result"
  if { [string equal $result "ok"] } {
    ::BayesTable::SetOption $this alias    $tmpdata(alias)
    ::BayesTable::SetOption $this user     $tmpdata(user)
    ::BayesTable::SetOption $this password $tmpdata(password)
    array unset tmpdata
  }
  destroy $dlg
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::SQLTable::OnConnectOk {this} {
#
# PURPOSE: 
#
# PARAMETERS:
#   this  -> Instance of BayesTable
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpdata

  blt::busy hold $tmpdata(dlg)
  update
  if [catch "database dbtest $tmpdata(alias) $tmpdata(user) $tmpdata(password)" msg] {
    MessageDlg ._msgerr -type ok -icon error -message $msg
  } else {
    dbtest disconnect
    set idx [$tmpdata(cbalias) getvalue]
    #Tolcon_Trace "idx= $idx, [lindex $tmpdata(datasources) $idx]"
    set issql [string equal "SQL Server" [lindex [lindex $tmpdata(datasources) $idx] 1]]
    database connect db $tmpdata(alias) $tmpdata(user) $tmpdata(password)
    ::BayesTable::SetOption $this "db" db 
    $tmpdata(dlg) enddialog ok
    #Llenar treeview
    FillTreeView $this
  }
  blt::busy release $tmpdata(dlg)
}

#/////////////////////////////////////////////////////////////////////////////
proc ::SQLTable::FillTreeView {this} {
#
# PURPOSE:
#
# PARAMETERS:
#   this  -> Instance of BayesTable
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpdata
  set tv        [::BayesTable::GetWidget $this tv]
  set dbConnect [::BayesTable::GetOption $this db]

  set img [::Bitmap::get bd]
  $tv delete root
  $tv entry configure root -label [$dbConnect cget -alias] -icons [list $img $img]
  
  set idxUsrTbl   [$tv insert -at root end "User_tables"   -label [mc "User Tables"]]
  set idxSysTbl   [$tv insert -at root end "System_tables" -label [mc "System Tables"]]
  set idxView     [$tv insert -at root end "Views"         -label [mc "Views"]]
  set idxSynonym  [$tv insert -at root end "Synonyms"      -label [mc "Synonyms"]]
  set idxSequence [$tv insert -at root end "Sequence"      -label [mc "Sequence"]]
  #set idxOther    [$tv insert -at root end "Other"         -label [mc "Other"]]

  set imgTbl  [::Bitmap::get table]
  set imgView [::Bitmap::get view]
  set lstTables [[$dbConnect cget -connection] tables]
  set nTable 0
  foreach tb_info $lstTables {
    set tb "[lindex $tb_info 1].[lindex $tb_info 2]"
    switch [lindex $tb_info 3] {
      "TABLE"        {
        set idx [$tv insert -at $idxUsrTbl  end $nTable]
        $tv entry configure $idx -label $tb -icons [list $imgTbl $imgTbl]\
          -opencommand "::SQLTable::FillTable $this %#" -button yes
      }
      "SYSTEM TABLE" {
        set idx [$tv insert -at $idxSysTbl  end $nTable]
        $tv entry configure $idx -label $tb -icons [list $imgTbl $imgTbl]\
          -opencommand "::SQLTable::FillTable $this %#" -button yes
      }
      "VIEW"         {
        set idx [$tv insert -at $idxView    end $nTable]
        $tv entry configure $idx -label $tb -icons [list $imgView $imgView]\
          -opencommand "::SQLTable::FillTable $this %#" -button yes
      }
      "SYNONYM"      {
        set idx [$tv insert -at $idxSynonym end $nTable]
        $tv entry configure $idx -label $tb -icons [list $imgTbl $imgTbl]\
          -opencommand "::SQLTable::FillTable $this %#" -button yes
      }
      "SEQUENCE"      {
        set idx [$tv insert -at $idxSequence end $nTable]
        $tv entry configure $idx -label $tb -icons [list $imgTbl $imgTbl]\
          -opencommand "::SQLTable::FillTable $this %#" -button yes
      }
      default        {
        # set idx [$tv insert -at $idxOther   end $nTable]
        # $tv entry configure $idx -label $tb -icons [list $imgTbl $imgTbl]\
          # -opencommand "::SQLTable::FillTable $this %#" -button yes
      }
    }
    incr nTable
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::SQLTable::GetColumnTxt {colinfo} {
#
#/////////////////////////////////////////////////////////////////////////////

  # TODO: Repasar la parte que une nombres y valores en array y llevarlo a un
  # procedimiento comun (que devuelva lista [array get $inf])
  # Hacerlo tb para indices y tablas
  set lst_names [list TABLE_QUALIFIER\
                      TABLE_OWNER \
                      TABLE_NAME\
                      COLUMN_NAME\
                      DATA_TYPE\
                      TYPE_NAME\
                      PRECISION \
                      LENGTH\
                      SCALE\
                      RADIX\
                      NULLABLE\
                      REMARKS]
  #En colinfo hay mas informacion q estos 12 campos (al menos para SQLServer)
  #puts "ln=[llength $lst_names], lv=[llength $colinfo]"
  for  {set i 0} {$i < [llength $lst_names]} {incr i} {
    set inf([lindex $lst_names $i]) [lindex $colinfo $i]
  }
  #parray inf

  switch -- $inf(DATA_TYPE) {
	2 - 3 - 4 - 5 - 6 - 7 - 8 - {-5} - {-6} {
	  # numerical datatypes
	  set type_name $inf(TYPE_NAME)
	}
	1 - 12 - {-1} {
	  # string types
	  set type_name "$inf(TYPE_NAME) ($inf(LENGTH))"
	}
	default {
	  # logical, date and time types
	  set type_name $inf(TYPE_NAME)
	}
  }
 if $inf(NULLABLE) {
   set null "Null"
 } else  {
   set null "Not Null"
 }

 return "$inf(COLUMN_NAME) ($type_name,$null)"
}

#/////////////////////////////////////////////////////////////////////////////
proc ::SQLTable::FillTable {this index} {
#
# PURPOSE: Script called each time a node of a table in treeview is opened.
#
# PARAMETERS:
#   this : Instance of BayesTable
#   index : The id of the node opened
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpdata
  set tv [::BayesTable::GetWidget $this tv]
  set dbConnect [::BayesTable::GetOption $this db]

  set path [$tv get -full $index]
  puts "FillTable. index=$index, tv=$tv"
  set imgCol  [::Bitmap::get column]
  set imgIdx  [::Bitmap::get index]
  
  if  ![llength [$tv entry children $index]] {
      set tbl [$tv entry cget $index -label]
      set idx $index
      set nTable [lindex $path end]
      set tblName [lindex [split $tbl .] end]
      if ![catch {set lstColumns [[$dbConnect cget -connection] columns $tblName]}] {
          set idxColumns [$tv insert -at $idx end c$nTable]
          $tv entry configure $idxColumns -label [mc "Columns"]
          set nCol 0
          foreach col_info $lstColumns {
              
              set idxCol [$tv insert -at $idxColumns end c_${nTable}_$nCol]
              set col_txt [GetColumnTxt $col_info]
              $tv entry configure $idxCol -label $col_txt -icons [list $imgCol $imgCol]
              incr nCol
          }
      }
      if ![catch {set lstIndexes [[$dbConnection cget -connection] indexes $tblName]}] {
          set idxIndexes [$tv insert -at $idx end i$nTable]
          $tv entry configure $idxIndexes -label [mc "Indexes"]
          set nIdx 0
          foreach idx_info $lstIndexes {
              set idxIdx [$tv insert -at $idxIndexes end i_${nTable}_$nIdx]
              set idx_txt [lindex $idx_info 4]
              $tv entry configure $idxIdx -label $idx_txt -icons [list $imgIdx $imgIdx]
              incr nIdx
          }
      }
  }
}


#/////////////////////////////////////////////////////////////////////////////
# INIFILE
#/////////////////////////////////////////////////////////////////////////////

#/////////////////////////////////////////////////////////////////////////////
  proc ::SQLTable::ReadIni {this} {
#
# PURPOSE: Reads specific options from .ini file. First of all, reads options
#          from BayesTable. 
#
# PARAMETERS:
#   this  -> Instance of BayesTable
#
#/////////////////////////////////////////////////////////////////////////////
  global toltk_options
  # General options (parent)
  ::BayesTable::ReadIni $this
  # Specific options
  set rini ::iniFile::Read

  ::BayesTable::SetOption $this txt,font  \
        [$rini SQLTable txt,font   $toltk_options(fonts,FixSmall)]
}

    
#/////////////////////////////////////////////////////////////////////////////
  proc ::SQLTable::WriteIni {this} {
#
# PURPOSE: Writes specific options to .ini file. This procedure is called by
#          parent BayesGraph. 
#
# PARAMETERS:
#   this  -> Instance of BayesTable
#
#/////////////////////////////////////////////////////////////////////////////
  set wini ::iniFile::Write

  $wini SQLTable txt,font [::BayesTable::GetOption $this txt,font]
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::SQLTable::Configure {this} {
#
# PURPOSE: Configures some options of SQL Table. First of all, configures
#          options from BayesTable. 
#
# PARAMETERS:
#   this  -> Instance of BayesTable
#
#/////////////////////////////////////////////////////////////////////////////
  # General options (parent)
  ::BayesTable::Configure $this
  # Specific options
  set w [::BayesTable::GetWidget $this txt]
  $w configure -font [::BayesTable::GetOption $this txt,font]
}
