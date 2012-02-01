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

proc ::TolPkg::GetPkgDirectory { p } {
  set localRoot [ file normalize \
                      [ toltcl::eval { Text TolPackage::_.localRoot } ] ]
  return [ file join $localRoot "Client" $p ]
}

proc ::TolPkg::BackupPackage { p {delete 0}} {
  # backup is not working, some problem has being reported on Windows 7
  return 1
  set path [ GetPkgDirectory $p ]
  if { [ file exists $path ] } {
    set bak "${path}.bak"
    set op [ expr { $delete ? "rename" : "copy" } ]
    puts "file $op -force $path $bak"
    return [ file $op -force $path $bak ]
  } else {
    return 0
  }
}

proc ::TolPkg::RestoreBackup { p { delete 0 } } {
  # backup is not working, some problem has being reported on Windows 7
  return 1
  set path [ GetPkgDirectory $p ]
  set bak "${path}.bak"
  if { [ file exists $bak ] } {
    set op [ expr { $delete ? "rename" : "copy" } ]
    return [ file $op -force $bak $path ]
  } else {
    return 0
  }
}

proc ::TolPkg::DeleteBackup { p } {
  # backup is not working, some problem has being reported on Windows 7
  return 1
  set path [ GetPkgDirectory $p ]
  set bak "${path}.bak"
  return [ file delete -force $bak ]
}

proc ::TolPkg::GetPkgSyncInfo { } {
  set localRoot [ file normalize \
                      [ toltcl::eval { Text TolPackage::_.localRoot } ] ]
  set oza [ file join $localRoot "Client/PackSyncInfo.oza" ]
  if { ![ file exists $oza ] || ![ file readable $oza ] } {
    return {}
  }
  set tolScript [ string map [ list "%O" $oza ] {
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
  set oza [ file join $localRoot "Client/VersSyncInfo.oza" ]
  if { ![ file exists $oza ] || ![ file readable $oza ] } {
    return {}
  }
  set tolScript [ string map [ list "%O" $oza ] {
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
    lappend pkgsInfo [ list $d [ GetLocalPackageInfo $d ] ]
  }
  return $pkgsInfo
}

proc ::TolPkg::GetLocalPackageInfo { p } {
  set tolScript [ string map [ list %p $p ] {
    Set TolPackage::Client::LocalInfo( "%p" );
  } ]
  return [ lindex [ toltcl::eval $tolScript -named 1 ] 1 ]
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

proc ::TolPkg::AddRepository { url } {
  set tolexpr [ string map [ list %u $url ] {
    Real TolPackage::Client::AddRepository( "%u" );
  } ]
  toltcl::eval $tolexpr
}

proc ::TolPkg::ZipInstall { pkg.zip } {
  puts "::TolPkg::ZipInstall ${pkg.zip}"
  set tolexpr [ string map [ list %p ${pkg.zip} ] {
    Real TolPackage::Client::LocalInstallPackage( "%p" )
  } ]
  return [ toltcl::eval $tolexpr ]
}

proc ::TolPkg::RemoteInstall { pkg repo } {
  puts "::TolPkg::RemoteInstall $pkg $repo"
  BackupPackage $pkg 1
  set tolexpr [ string map [ list %p $pkg %r $repo ] {
    Real TolPackage::Client::RemoteInstall( "%r", "%p", 1 )
  } ]
  if { [ catch  { toltcl::eval $tolexpr } result ] } {
    set result 0
  }
  if { [ string is boolean $result ] && $result } {
    DeleteBackup $pkg
  } else {
    RestoreBackup $pkg 1
  }
  return $result
}

proc ::TolPkg::ExportPackage { pkg dest } {
  puts "::TolPkg::ExportPackage $pkg to $dest"
  set tolexpr [ string map [ list %p $pkg %d $dest ] {
    Real TolPackage::Client::LocalExportPackage( "%p", "%d/" )
  } ]
  return [ toltcl::eval $tolexpr ]
}

proc ::TolPkg::CheckPackagesToInstall { pkgs } {
  set members {}
  foreach p $pkgs {
    lappend members \"${p}\"
  }
  set tolexpr [ string map [ list %s "\[\[ [ join $members , ] \]\]" ] {
    Set TolPackage::Client::GetDeepDependencies.all.last( %s );
  } ]
  return [ lindex [ toltcl::eval $tolexpr ] 0 ]
}

proc ::TolPkg::GetUpgradeConfig { } {
  set tolexpr {
    Set { 
      [[ 
        Real checkTOL = TolConfigManager::Config::Upgrading::TolVersion::CheckAllowed;
        Real localPKG = TolConfigManager::Config::Upgrading::TolPackage::LocalOnly
       ]]
    }
  }
  return [ lindex [ toltcl::eval $tolexpr -named 1 ] 1 ]
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