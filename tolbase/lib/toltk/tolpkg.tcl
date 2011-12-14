package require Toltcl

if { ![ llength [ info commands Tol_HciWriter ] ] } {
  tol::initkernel

  proc Tol_HciWriter { msg } {
    puts [ string trim $msg ]
  }

  tol::initlibrary 1
}

namespace eval ::TolPkg {
  variable cwd [ file normalize [ file dir [ info script ] ] ]
}

proc ::TolPkg::UpdateRepositoryInfo { } {
  tol::console eval {
    Real TolPackage::Client::RemoteUpdatePackSyncInfo(False);
    Real TolPackage::Client::RemoteUpdateVersSyncInfo(False);
  }
  return
}

if { 0 } {
0 -- http://packages.localbayes.es/BayesPrivateTolPackages/repository.php
1 -- SSCC_P18_Maps
2 -- Maps for the sscc in Granada province.
3 -- 
4 -- v2.0.1 b.1
5 -- v999999999999999999
6 -- SSCC_P18_Maps.1.0
7 -- SSCC_P18_Maps.1.0
8 -- y2011m11d26h15i11s00.000
9 -- y2011m11d26h15i11s00.000
10 -- 1154572
}

proc ::TolPkg::GetPkgSyncInfo { } {
  set localRoot [ file normalize \
                      [ toltcl::eval { Text TolPackage::_.localRoot } ] ]
  set ozaSyncInfo [ file join $localRoot "Client/PackSyncInfo.oza" ]
  set tolScript [ string map [ list "%O" $ozaSyncInfo ] {
    Set { Ois.Load( "%O" )[1] }
  } ]
  # return a dictionary
  set dict {}
  foreach p [ lindex [ toltcl::eval $tolScript ] 0 ] {
    set url        [ lindex $p 0 ]
    set name       [ lindex $p 1 ]
    set brief      [ lindex $p 2 ]
    set mintol     [ lindex $p 4 ]
    set maxtol     [ lindex $p 5 ]
    set lastremote [ lindex $p 6 ]
    set lastlocal  [ lindex $p 7]
    lappend dict [ list $name \
                       [ list url $url brief $brief mintol $mintol \
                             maxtol $maxtol lastremote $lastremote \
                             lastlocal $lastlocal ] ]
  }
  return $dict
}

if { 0 } {
0 -- http://packages.localbayes.es/BayesPrivateTolPackages/repository.php
1 -- MMS
2 -- MMS.0.6040
3 -- 
4 -- v2.0.1 b.4
5 -- v999999999999999999
6 -- GuiTools.3.2,BysMcmc.4.8,TolIpopt.2.1,IpoptCore.3.9,QltvRespModel.4.1,GrzLinModel.3.1,MatQuery.3.2,NonLinGloOpt.5.1,TolCint.2.1,BysPrior.2.1,MatAlg.1.1,BysInfDiag.2.1,BysSampler.4.1,BabelTool.1.4,ExtLib.1.5,RandVar.2.104,DecoTools.1.6
7 -- y2011m11d07h10i45s00.000
8 -- y2011m11d07h10i45s00.000
9 -- 1474442
}

proc ::TolPkg::GetVersSyncInfo { } {
  set localRoot [ file normalize \
                      [ toltcl::eval { Text TolPackage::_.localRoot } ] ]
  set ozaSyncInfo [ file join $localRoot "Client/VersSyncInfo.oza" ]
  set tolScript [ string map [ list "%O" $ozaSyncInfo ] {
    Set { Ois.Load( "%O" )[1] }
  } ]
  # return a dictionary
  set dict {}
  foreach p [ lindex [ toltcl::eval $tolScript ] 0 ] {
    set url          [ lindex $p 0 ]
    set name         [ lindex $p 1 ]
    set version      [ lindex $p 2 ]
    set mintol       [ lindex $p 4 ]
    set maxtol       [ lindex $p 5 ]
    set dependencies [ split [ lindex $p 6 ] "," ]
    set dateremote   [ lindex $p 7 ]
    set datelocal    [ lindex $p 8 ]
    lappend dict [ list $version \
                       [ list url $url name $name mintol $mintol \
                             maxtol $maxtol dateremote $dateremote \
                             datelocal $datelocal \
                             dependencies $dependencies ] ]
  }
  return $dict
}

proc ::TolPkg::GetLocalPackages { } {
  set localRoot [ file normalize \
                      [ toltcl::eval { Text TolPackage::_.localRoot } ] ]
  set clientRoot [ file join $localRoot Client ]
  set dirs [ glob -tail -dir $clientRoot -types {d r} * ]
  set pkgsInfo {}
  foreach d $dirs {
    set check [ file join $clientRoot $d ${d}.oza ]
    if { [ file exists $check ] } {
      lappend pkgsInfo [ GetPackageInfoFromOZA $check ]
    }
  }
  return $pkgsInfo
}

proc ::TolPkg::GetPackageInfoFromOZA { oza } {
  puts "::TolPkg::GetPackageInfoFromOZA $oza"
  set tolScript [ string map [ list "%O" $oza ] {
    Set {
      Set oza = Include( "%O" );
      WriteLn( "%O" );
      Set If( Grammar( oza[1] ) == "NameBlock", {
        NameBlock pkg = oza[ 1 ];
        Set If( And( ObjectExist( "Text", "pkg::_.autodoc.name" ),
                     ObjectExist( "Text", "pkg::_.autodoc.brief" ),
                     ObjectExist( "Text", "pkg::_.autodoc.description" ),
                     ObjectExist( "Text", "pkg::_.autodoc.url" ),
                     ObjectExist( "Real", "pkg::_.autodoc.version.high" ),
                     ObjectExist( "Real", "pkg::_.autodoc.version.low"),
                     ObjectExist( "Set",  "pkg::_.autodoc.authors" ),
                     ObjectExist( "Text", "pkg::_.autodoc.minTolVersion" ) ), {
                       Set aux = [[ Text name           = pkg::_.autodoc.name,
                                    Text brief          = pkg::_.autodoc.brief,
                                    Text description    = pkg::_.autodoc.description,
                                    Text url            = pkg::_.autodoc.url,
                                    Real version.high   = pkg::_.autodoc.version.high,
                                    Real version.low    = pkg::_.autodoc.version.low,
                                    Set dependencies    = pkg::_.autodoc.dependencies,
                                    Set authors         = pkg::_.autodoc.authors,
                                    Text versionControl = If( ObjectExist( "Text", "pkg::_.autodoc.versionControl" ),
                                                              pkg::_.autodoc.versionControl, "" );
                                    Text minTolVersion  = pkg::_.autodoc.minTolVersion,
                                    Text maxTolVersion  = If( ObjectExist( "Text", "pkg::_.autodoc.maxTolVersion" ),
                                                              pkg::_.autodoc.axTolVersion, "" ) ]];
                       Text pkgName = pkg::_.autodoc.name << "." << pkg::_.autodoc.version.high
                                      << "." << pkg::_.autodoc.version.low ;
                       Set PutName( pkgName, aux )                       
                     }, { WriteLn( "Empty 2" ), Copy( Empty )} ) }, { WriteLn( "Empty 1" ); Copy( Empty )}  )
    };
  } ]
  #puts $tolScript
  set result [ toltcl::eval $tolScript -named 1 ]
  puts "::TolPkg::GetPackageInfoFromOZA END $oza"
  return $result
}


proc ::TolPkg::GetRepositoryInfo { repo } {
  set ask "${repo}?action=general_information"
  set result [ toltcl::eval [ string map [ list %A $ask ] {
    Set TolPackage::Client::GetRepositoryInfo( "%A" )
  } ] ]
  return [ lindex $result 0 ]
}

proc ::TolPkg::GetKnownRepositories { } {
  return [ lindex [ toltcl::eval \
                        {Set TolConfigManager::Config::Upgrading::TolPackage::Repositories} ] 0 ]
}

proc ::TolPkg::GetPackagesInfo { } {
  return [ lindex [ toltcl::eval { Set TolPackage::Client::_.packSyncInfo } ] 0 ]
}

proc ::TolPkg::test_00 { } {
  GetRepositoryInfo http://packages.tol-project.org/OfficialTolArchiveNetwork/repository.php
}

proc ::TolPkg::test_01 { } {
  set oza /home/jsperez/.tol/TolPackage/1.1/Client/DecoTools.1.8/DecoTools.1.8.oza
  ::TolPkg::GetPackageInfoFromOZA $oza
}

proc ::TolPkg::test_02 { } {
  return [ llength [ ::TolPkg::GetLocalPackages ] ]
}