package require TclCurl

namespace eval ::TolCurl {
}

proc ::TolCurl::Get { url pathDest args } {
  catch { curl::transfer -url $url -file $pathDest } result
  return $result
}