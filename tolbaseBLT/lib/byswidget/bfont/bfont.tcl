#/////////////////////////////////////////////////////////////////////////////
# FILE    : bfont.tcl
# PURPOSE : Bayes Font selector is a widget that provides an interface
#           to select an available font.
#/////////////////////////////////////////////////////////////////////////////

package require byscommon
package require snit
package require BWidget

#/////////////////////////////////////////////////////////////////////////////
::snit::widget bfont {
#
# Specific options:
# -showlabel-> shows a label next to the button
# -variable-> the font
# -label-> text for the label
#
# variables:
# widget-> widgets path
# data-> widgets data
# created-> created flag
#/////////////////////////////////////////////////////////////////////////////
  widgetclass Bfont
  
  option -variable       {}
  option -showlabel      0
  option -label          ""

  variable widget
  variable data
  variable created

  #///////////////////////////////////////////////////////////////////////////
  constructor {args} {
  #
  # PURPOSE: bfont constructor. An instance of bfont is created.
  #
  # PARAMETERS:
  #   args -> options
  #
  #///////////////////////////////////////////////////////////////////////////
    set created 0
    #Label
    set widget(label) [label $self.lb -text [$self cget -label]]
    #Button
    install font using Button $self.bt -image [::Bitmap::get font] \
                  -command [list $self _FontChange]

    set data(tracecmd) [list $self _FontUpdate]
    
    $self configure -variable  [from args -variable ""]

    $self createwidget
    
    $self configure -showlabel [from args -showlabel ""]
    $self configure -label     [from args -label ""]    

    $self configurelist $args

    # The widget is created
    set created 1
  }

  #///////////////////////////////////////////////////////////////////////////
  #
  # PURPOSE: Methods cget & configure
  #
  #///////////////////////////////////////////////////////////////////////////    
  onconfigure -showlabel { lst } {
    set options(-showlabel) $lst
    if {$lst == 0} {
      grid remove $widget(label)
    } else {
      grid $widget(label)
    }
  }

  onconfigure -variable { var } {
    set options(-variable) $var
  }

  onconfigure -label { lst } {
    $widget(label) configure -text $lst
    set options(-label) $lst
  }

  #///////////////////////////////////////////////////////////////////////////
  method createwidget { } {
  #
  # PURPOSE: The widget is created and packed
  #
  #///////////////////////////////////////////////////////////////////////////  
    grid $widget(label) $font
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method _FontChange { } {
  #
  # PURPOSE: Changes the font when OK button is pressed. If CANCEL is pressed
  #          leaves the previous font.
  #
  #//////////////////////////////////////////////// ///////////////////////////  
    eval set fon $[$self cget -variable]
    set newfont [SelectFont .sf -parent $self \
                               -title [mc "Font selection"] \
                               -sampletext [mc "Sample text"] \
                               -font $fon]
    
    if { $newfont != "" } {
      set [$self cget -variable] $newfont
    }
  }

  # delegation of methods and options
  delegate method * to font
  delegate option * to font
}

package provide bfont 1.0

  # ------- TEST CODE ----

#/////////////////////////////////////////////////////////////////////////////
proc test_bfont {} {
#
# PURPOSE: Test function bfont
#
#/////////////////////////////////////////////////////////////////////////////
  destroy .top
  toplevel .top
  global mifont
  set mifont {lucida 9 bold}
  
  set bfont [bfont .top.font -variable ::mifont -showlabel 1 -label [mc Font]:]
  
  pack $bfont
}