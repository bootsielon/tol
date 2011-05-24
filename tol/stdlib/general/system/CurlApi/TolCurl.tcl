package require TclCurl

namespace eval ::TolCurl {
  # infoType specifies what kind of information it is (0 text, 1
  # incoming header, 2 outgoing header, 3 incoming data, 4 outgoing
  # data, 5 incoming SSL data, 6 outgoing SSL data).
  array set infoTypeConf {
    0 "**"
    0,PRINT 1
    0,DESCRIPTION "text"
    1 <-
    1,PRINT 1
    1,DESCRIPTION "incoming header"
    2 ->
    2,PRINT 1
    2,DESCRIPTION "outgoing header"
    3 <*
    3,PRINT 0
    3,DESCRIPTION "incoming data"
    4 *>
    4,PRINT 0
    4,DESCRIPTION "outgoing data"
    5 <*
    5,PRINT 0
    5,DESCRIPTION "incoming SSL data"
    6 *>
    6,PRINT 0
    6,DESCRIPTION "outgoing SSL data"
  }
}

proc ::TolCurl::upload { sendFile url args } {
  set curlHandle [ ::curl::init ]
  
  if { ![ file exists $sendFile ] || ![ file readable $sendFile ] } {
    set failStatus -2
    set explain "file $sendFile does not exists or is not readable"
    return [ list failStatus $failStatus explain $explain ]
  }

  set fileSize [ file size $sendFile ]

  $curlHandle configure  -url               $url \
                         -verbose           1 \
			 -errorbuffer       errorBuffer \
                         -upload            1 \
			 -infile            $sendFile \
                         -infilesize        $fileSize \
			 -failonerror       1
  if { [ llength $args ] } {
    eval $curlHandle configure $args
  }

  set r 0
  if { [ catch { $curlHandle perform } r ] } {    
    $curlHandle cleanup
    #puts "'$r -- $errorBuffer'"
    #return -code error "$r $errorBuffer"
    if { [ string is integer $r ] } {
      set status $r
    } else {
      set status -1
      set errorBuffer $r
    }
    #return -code error $errorBuffer
    return [ list failStatus $status explain $errorBuffer ]
  }
  
  set totalTime    [ $curlHandle getinfo totaltime     ] 
  set connectTime  [ $curlHandle getinfo connecttime   ] 

  set details \
      [ list failStatus $r totalTime $totalTime connectTime $connectTime ]
  
  $curlHandle cleanup 
  
  return $details
}

#::TolCurl::upload TolCurl.tcl scp://packages.localbayes.es/var/www/packages/TolCurl.tcl -userpwd "toldevel:toldevelbys"


proc ::TolCurl::getUrl0 { url args } {
  set curlHandle [ ::curl::init ]
  
  array set options {
    -file ""
    -verbose 0
    -timeout 0
    -connecttimeout 5
    -debugproc ""
  }

  array set options $args

  set bodyData ""
  if { $options(-file) eq "" } {
    set optionDest "-bodyvar"
    set nameDest "bodyData"
    set valueDest bodyData
  } else {
    set optionDest "-file"
    set nameDest "file"
    set valueDest $options(-file)
  }
  $curlHandle configure \
      -url               $url \
      -verbose           $options(-verbose) \
      -errorbuffer       errorBuffer \
      $optionDest        $valueDest \
      -failonerror       1 \
      -followlocation    1 \
      -filetime          1 \
      -connecttimeout    $options(-connecttimeout) \
      -timeout           $options(-timeout) \
      -debugproc         $options(-debugproc)
  
  #-userpwd           $userid:$password \
  #-proxy             $proxyHost \
  #-proxyport         $proxyport \
  #-proxytype         http \
      
  set r 0
  if { [ catch { $curlHandle perform } r ] } {    
    $curlHandle cleanup
    #puts "'$r -- $errorBuffer'"
    #return -code error "$r $errorBuffer"
    if { [ string is integer $r ] } {
      set status $r
    } else {
      set status -1
      set errorBuffer $r
    }
    #return -code error $errorBuffer
    return [ list failStatus $status explain $errorBuffer ]
  }
  
  set fileTime     [ $curlHandle getinfo filetime     ] 
  set totalTime    [ $curlHandle getinfo totaltime     ] 
  set connectTime  [ $curlHandle getinfo connecttime   ] 
  set sizeDownload [ $curlHandle getinfo sizedownload  ] 
  set speedDownload [ $curlHandle getinfo speeddownload ]
  
  #puts " totalTime    = $totalTime    "
  #puts " connectTime  = $connectTime  "
  #puts " sizeDownload = $sizeDownload "
  #puts " speedDownoad = $speedDownoad "
  
  set details [ list failStatus $r fileTime $fileTime totalTime $totalTime \
                    connectTime $connectTime sizeDownload $sizeDownload \
                    speedDownload $speedDownload nameData $nameDest \
                    $nameDest [expr { $nameDest eq "file" ?  
                                      $options(-file) : $bodyData } ] ]
  
  $curlHandle cleanup 
  
  return $details
}

#::TolCurl::getUrl0 "http://packages.tol-project.org/OfficialTolArchiveNetwork/repository.php?tol_package_version=1.1&tol_version=v2.0.1%20b.4&action=ping&key=658184943"

# infoType specifies what kind of information it is (0 text, 1
# incoming header, 2 outgoing header, 3 incoming data, 4 outgoing
# data, 5 incoming SSL data, 6 outgoing SSL data).
proc ::TolCurl::debugProc { infoType data } {
  variable infoTypeConf

  
  if { $infoTypeConf($infoType,PRINT) } {
    set _data "[string trim $data]\n"
  } else {
    set _data "$infoTypeConf($infoType,DESCRIPTION)\n"
  }
  Tol_HciWriter "$infoTypeConf($infoType) $_data"
}

proc ::TolCurl::getUrl { url args } {
  set details [ eval getUrl0 [list $url] $args -debugproc ::TolCurl::debugProc ]
  set textItems [list]
  foreach {key value} $details {
    set item [ expr { [string is double $value] ?
                      "Real $key=$value" : "Text $key=\"$value\""} ]
    lappend textItems $item
  }
  set joinedItems [ join $textItems "," ]
  return "\[\[${joinedItems}\]\]"
}

#puts [TolCurl::getUrl "http://packages.tol-project.org/tol_pkg/v2_0_1/GuiTools.oza" -file "/tmp1/GuiTools.oza"]
#puts [TolCurl::getUrl "http://packages.tol-project.org/tol_pkg/v2_0_1" -file "/tmp/v2_0_1"]
