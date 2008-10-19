#/////////////////////////////////////////////////////////////////////////////
# FILE    : bfMessages
# PURPOSE : Functions of messages of TolBase to users or external applications
#
#/////////////////////////////////////////////////////////////////////////////

#/////////////////////////////////////////////////////////////////////////////
proc ShowMessage { type msg parent {useMessageBox 1} } {
#
# PURPOSE: Shows a message with an pop-up window and in the HciWriter 
#          depending on its type
# 
# PARAMETERS:
#   type -> Type of the message ("E": Error, "W": Warning, o "M": Message)
#   msg -> Text to show
#   parent -> Widget that generates the message
#   useMessageBox -> If has a value 0, don't show the messageBox. 1 by default
#
#/////////////////////////////////////////////////////////////////////////////
  switch $type {
    E {
      if {$useMessageBox} {
        tk_messageBox -type ok -icon error\
          -message [mc $msg]\
          -parent $parent -title [mc Error]
      }
      set msg [concat "<E>[mc ERROR]:" [mc $msg] "</E>"]
      ::TolConsole::HciWriter $msg
    }
    W {
      if {$useMessageBox} {      
        tk_messageBox -type ok -icon warning\
           -message [mc $msg]\
           -parent $parent -title [mc Warning]
      }
      set msg [concat "<W>[mc WARNING]:" [mc $msg] "</W>"]
      ::TolConsole::HciWriter $msg
    }
    M {
      if {$useMessageBox} {
        tk_messageBox -type ok -icon info\
          -message [mc $msg]\
          -parent $parent -title [mc Message]
      }
      set msg [concat "[mc MESSAGE]:" [mc $msg] ]
      ::TolConsole::HciWriter $msg
    }
  } ;# switch
}

#/////////////////////////////////////////////////////////////////////////////
proc ShowBrowserPage { page {section ""} } {
#
# PURPOSE: Shows a web page on the internet browser. If this option isn't
#          established it uses the default browser under windows and allows to
#          select it in linux.
#
# PARAMETERS:
#   page -> Page to open
#   section -> Section of the page (optional)
#
#/////////////////////////////////////////////////////////////////////////////
  set rini ::iniFile::Read
  set pathBrowser  [$rini TolConsole pathBrowser {}]
  if {[llength $pathBrowser] eq 0 } { ;#pathBrowser isn't established
    if { $::tcl_platform(platform) == "windows" } {
      ShowMessage "W" \
        [mc "The path of the internet browser isn't established, using the default Windows browser"] \
        "." 0
      eval exec [auto_execok start] [list $page] &
    } else { ;# linux
      ShowMessage "E" \
        [mc "The path of the internet browser isn't established"] \
        "." 0
      ::TolConsole::OptionsCreate "Paths"
    }
  } else {
    if {$section eq ""} { ;# this page hasn't section
      exec $pathBrowser $page &
    } else {
      exec $pathBrowser $page $section &
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ShowPDFDocument { path } {
#
# PURPOSE: Shows a PDF document on the internet browser. If this option isn't
#          established allows to select it in linux.
#
# PARAMETERS:
#   path -> path to PDF document
#
#/////////////////////////////////////////////////////////////////////////////
  set rini ::iniFile::Read
  set pathPDFViewer  [$rini TolConsole pathPDFViewer {}]
  if {[llength $pathPDFViewer] eq 0 } { ;#pathPDFViewer isn't established
    ShowMessage "E" [mc "The path of the PDF wiewer isn't established"] "." 0
    ::TolConsole::OptionsCreate "Paths"
  } else {
    exec $pathPDFViewer $path &
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ShowHelp { {section "Tb4Mai"} } {
#
# PURPOSE: Constructs the absolute path of the help and launch the browser.
#
# PARAMETERS:
#  section -> section of the help. By default "Tb4Mai" (Tolbase4 Main)
#
#/////////////////////////////////////////////////////////////////////////////
  global toltk_options

  set rini ::iniFile::Read
  set pathHelp  [$rini TolConsole pathHelp {}]
  if { ([file isdirectory $pathHelp]) && \
       ([file isfile [file join $pathHelp "pathhelp.bst"]]) } {
    set page [GetUniqueName Text _helpPage]
    set cmd "Text $page = GetDirHelpSection(\"$pathHelp\",\"$section\");"
    ::tol::console eval $cmd
    array set info [TclInfoVar Text $page]
    eval "set path $info(CONTENT)"
    ::tol::console stack release $page
    ShowBrowserPage $path
  } else {
    ShowMessage "E" \
        [mc "The path of the help isn't correctly established"] \
        "." 0
      ::TolConsole::OptionsCreate "Paths"
  }
}