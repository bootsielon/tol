package require TclCurl

namespace eval ::TolCurl {
}

proc ::TolCurl::getUrl0 { url receiveFile args } {
  set curlHandle [ ::curl::init ]
  
  $curlHandle configure -url               $url \
                         -verbose           0 \
			 -errorbuffer       errorBuffer \
			 -file              $receiveFile \
			 -failonerror       1 \
         	         -followlocation    1 \
                         -filetime          1

         	         #-userpwd           $userid:$password \
         	         #-proxy             $proxyHost \
         	         #-proxyport         $proxyport \
         	         #-proxytype         http \
                         # -verbose           1 \

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
    return [ list status $status explain $errorBuffer ]
  }
  
  set fileTime     [ $curlHandle getinfo filetime     ] 
  set totalTime    [ $curlHandle getinfo totaltime     ] 
  set connectTime  [ $curlHandle getinfo connecttime   ] 
  set sizeDownload [ $curlHandle getinfo sizedownload  ] 
  set speedDownoad [ $curlHandle getinfo speeddownload ] 
  
  #puts " totalTime    = $totalTime    "
  #puts " connectTime  = $connectTime  "
  #puts " sizeDownload = $sizeDownload "
  #puts " speedDownoad = $speedDownoad "
  
  set details [ list failStatus $r fileTime $fileTime totalTime $totalTime connectTime $connectTime sizeDownload $sizeDownload speedDownload $speedDownoad ]
  
  $curlHandle cleanup 
  
  return $details
}

proc ::TolCurl::getUrl { url receiveFile args } {
  set details [ eval getUrl0 [list $url] [list $receiveFile] $args ]
  set textItems [list]
  foreach {key value} $details {
    set item [ expr { $key eq "explain" ?
                      "Text $key=\"$value\"" : "Real $key=$value"} ]
    lappend textItems $item
  }
  set joinedItems [ join $textItems "," ]
  return "\[\[${joinedItems}\]\]"
}

#puts [TolCurl::getUrl "http://packages.tol-project.org/tol_pkg/v2_0_1/GuiTools.oza" "/tmp1/GuiTools.oza"]
#puts [TolCurl::getUrl "http://packages.tol-project.org/tol_pkg/v2_0_1" "/tmp/v2_0_1"]