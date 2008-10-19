#/////////////////////////////////////////////////////////////////////////////
# FILE    : drawarima.tcl
# PURPOSE : This file contains a namespace called drawArima with its
#           functions. It's for creating a form that allows to define an
#           ARIMA model and plot its Auto Correlation Function and its Partial
#           Auto Correlation Function.
#/////////////////////////////////////////////////////////////////////////////
package require barima
package require bentrydate


#/////////////////////////////////////////////////////////////////////////////
namespace eval drawArima {
#
# PURPOSE: The purpose of this namespace is organize code and functions.
#          It also stotres a special namespace is created for each instance 
#          of drawArima.
# VARIABLES:
#   data -> array with information functions search
#     x -> p
#   widget -> contains the widget
#   sensitive -> has a value 1 if the user has selected case sensitive search
#
#/////////////////////////////////////////////////////////////////////////////
  variable data
  variable widget
}


#/////////////////////////////////////////////////////////////////////////////
proc ::drawArima::Show {} {
#
# PURPOSE: Shows the form to show ACFs and PACFs of an ARIMA Model
#
#/////////////////////////////////////////////////////////////////////////////
  variable widget   
  if { [info exist widget(document)] && [winfo exists $widget(document)] } {
    if { [$widget(document) cget -state] eq "minimized" } {
      $widget(document) restore
    }
    $widget(document) raise
  } else {
    Create
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::drawArima::Create {} {
#
# PURPOSE: Creates the draw ARIMA dialog
#
# PARAMETERS:
#   path: path of dialgo's parent
#
#/////////////////////////////////////////////////////////////////////////////
  global toltk_options
  variable data
  variable widget
    
  # Create the widget
  set widget(document) [::project::CreateForm \
    -iniconfig FunctionSearch \
    -title [mc "Draw ACF and PACF of ARIMA Models"] \
    -resizable 0 -width 680 -height 230 -helpkey "Tb4HanAriGra"] 
  set f0 [$widget(document) getframe]
  
  set tfm [TitleFrame $f0.tfm -text [mc "Parameters of the graph"]]
  set f [$tfm getframe]
  
  set widget(ba) [barima $f.ba -parent $widget(document) -coderequired 1 \
                               -codeeditable 1 -timesetrequired 1]
  set widget(tfs) [TitleFrame $f.tfs -text [mc "Date Selector"] ]
  set widget(frs) [$widget(tfs) getframe]  
  set widget(ledf) [Label $widget(frs).ledf -text [mc "Date first"]]
  set widget(lrf)  [Label $widget(frs).lrf -image [::Bitmap::get brequired] \
                                       -helptext [mc "Date first required"] ]
  set ::drawArima::data(bedf) ""
  set widget(bedf) [bentrydate $widget(frs).bedf \
                                         -bdate [::drawArima::TxtDef first] ]
  set widget(ledl) [Label $widget(frs).ledl -text [mc "Date last"]]
  set widget(lrl)  [Label $widget(frs).lrl -image [::Bitmap::get brequired] \
                         -helptext [mc "Date last required"] ]
  set ::drawArima::data(bedl) ""
  set widget(bedl) [bentrydate $widget(frs).bedl \
                         -bdate [::drawArima::TxtDef last] ]
  set datetmp [::drawArima::TxtDef last]
  $widget(bedl) Set [lindex $datetmp 0] [lindex $datetmp 1] \
                    [lindex $datetmp 2]
  set widget(bbox) \
          [ButtonBox $f.bbox -orient horizontal -spacing 10 -padx 0 -pady 0 \
          -homogeneous false]

  $widget(bbox) add -relief link \
          -helptext [mc "Draw ACF and PACF of ARIMA Model"] \
          -text [mc "Draw"] \
          -image [::Bitmap::get graphbar] -compound left -padx 5 -pady 1 \
          -command [list ::drawArima::Draw ]
  $widget(bbox) add -relief link -helptext [mc "Exit"] -text [mc "Exit"]\
          -image [::Bitmap::get exit] -compound left -padx 5 -pady 1 \
          -command [list ::drawArima::Destroy]

  pack  $tfm          -side top -fill both -expand yes
  pack  $widget(bbox) -side bottom -fill both -expand yes
  pack  $widget(ba)   -side left   -fill both -expand yes
  pack  $widget(tfs)  -side top    -fill both -expand yes

  grid $widget(ledf) -             -sticky nw 
  grid $widget(lrf)  $widget(bedf) -sticky nw  
  grid $widget(ledl) -             -sticky nw 
  grid $widget(lrl)  $widget(bedl) -sticky nw 

  grid columnconfigure $f0 0 -weight 1
  grid rowconfigure    $f0 0 -weight 1
  grid columnconfigure $f  0 -weight 1
  grid rowconfigure    $f  0 -weight 1
  
  grid columnconfigure $widget(frs) 0 -weight 0
  grid columnconfigure $widget(frs) 1 -weight 1
}


#/////////////////////////////////////////////////////////////////////////////
proc ::drawArima::Destroy {} {
#
# PURPOSE: Hides draw ARIMA document.
#
#/////////////////////////////////////////////////////////////////////////////
  variable widget
  variable data
  array unset data
  destroy $widget(document)
}

#/////////////////////////////////////////////////////////////////////////////
proc ::drawArima::Draw {} {
#
# PURPOSE: It verifies if all the required fields are filled and draws.
#
# RETURN: 1 if the graph has been generated, 0 otherwise
#////////////////////////////////////////////////////////////////////////////
  variable widget
  variable data
  set ret 0
  set val [::drawArima::Validate]
  switch -- $val {
    1 {
      # all the required fields are filled
      ::drawArima::DrawCFs [$widget(ba) cget -value] \
                           [$widget(bedf) cget -btext] \
                           [$widget(bedl) cget -btext] \
                           [$widget(ba) get "TimeSet"]
      set ret 1
    }
    "-1" {
      # error: all the required fields are not filled
      ShowMessage E [mc "All the required parameters must be filled"] \
                $widget(document)
    }
    "-2" {
      # error: the initial date is greater than the end date
      ShowMessage E [mc "The date last must be higher than the date fist"] \
                $widget(document)
    }
    "-3" {
      # error: the dates do not have the correct format
      ShowMessage E [mc "Dates do not have the correct format"] \
                $widget(document)
    }
  }
  return $ret
}

#/////////////////////////////////////////////////////////////////////////////
proc ::drawArima::Validate {} {
#
# PURPOSE: Verifies the fields are filled correctly
#
# RETURN: 0 if all its ok,
#        -1 if all the required fields are not filled
#        -2 if the initial date is greater than the end date
#        -3 the dates do not have the correct format
#
#/////////////////////////////////////////////////////////////////////////////
  variable widget
  variable data
  set tarima  [$widget(ba) cget -value]
  set tfirst  [$widget(bedf) cget -btext]
  set tlast   [$widget(bedl) cget -btext]
  set timeset [$widget(ba) get "TimeSet"]
  set ok 0
  if { !($timeset eq "") } { ;# timeset filled
    if { !($tarima eq "") } { ;# code filled
      if { !($tfirst eq "") } { ;# date first filled
        if { !($tlast eq "") } { ;# date last filled
          set ok 1
        } else {
          set ok -1
        }
      } else {
        set ok -1
      }
    } else {
      set ok -1
    }
  } else {
    set ok -1
  }
  if { ($ok eq 1) && ( $tfirst > $tlast ) } {
    # date first is higher than date last
    set ok -2
  }
  if { ($ok eq 1) && ( ( [CheckDateTime $tfirst] eq 0 ) ||
                       ( [CheckDateTime $tlast] eq 0 ) ) } {
    # dates aren't the correct format
    set ok -3
  }
  return $ok
}

#/////////////////////////////////////////////////////////////////////////////
proc ::drawArima::DrawCFs {arima tfirst tlast timeset} {
#
# PURPOSE: Draws the graph of ACFs and PACFs of ARIMA Model
#
# PARAMETERS:
#   arima -> string that defines the ARIMA model
#   tfirst -> text of first date
#   tlast -> text of last date
#   timeset -> text of timeset
#/////////////////////////////////////////////////////////////////////////////
  set dfirst "y[string range $tfirst 0 3]m[string range $tfirst 4 5]d[string range $tfirst 6 7]"
  set dlast  "y[string range $tlast 0 3]m[string range $tlast 4 5]d[string range $tlast 6 7]"
  set graphTitle1 [mc "Graph of ACFs and PACFs of ARIMA Model"]
  set graphTitle "${graphTitle1}: $arima"  
  set sentence \
    "Anything ARIMADrawACFs( Serie (ARIMAGetGaussian(Text \"$arima\", Date $dfirst, Date $dlast, TimeSet $timeset)), Text (\"$graphTitle\"));"
  ::tol::console eval $sentence
}


#/////////////////////////////////////////////////////////////////////////////
proc ::drawArima::TxtDef {args} {
#
# PURPOSE: Returns the correct value of date defined in Tol DefFirst or
#          DefLast to give it to bentrydate
#
#/////////////////////////////////////////////////////////////////////////////ç
  set res ""
  if {$args eq "first"} {
    set res [TclGetVar Date DefFirst CONTENT]
  }
  if {$args eq "last"} {
    set res [TclGetVar Date DefLast CONTENT]
  }
  set tmp $res
  if {!($res eq "")} {
    set res [list [list [string range $tmp 1 4]] \
                  [list [string range $tmp 6 7]] \
                  [list [string range $tmp 9 10]]]
  }
  return $res
}
  