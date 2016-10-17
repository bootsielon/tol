#/////////////////////////////////////////////////////////////////////////////
# FILE    : bcolor.tcl
# PURPOSE : Bayes Color selector is a widget that provides an interface
#           to select a color.
#/////////////////////////////////////////////////////////////////////////////

package require byscommon
package require snit
package require BWidget

#/////////////////////////////////////////////////////////////////////////////
::snit::widget bcolor {
#
# Specific options:
# -showlabel-> shows a label next to the button
# -variable-> the color
# -label-> text for the label
# -optmenu-> Optional menu:
#            0 -> Without menu
#            1 -> Menu with option without color
#            2 -> Menu with option default color
#            3 -> Menu with options without color and default color
#
# variables:
# widget-> widgets path
# data-> widgets data
# created-> created flag
#/////////////////////////////////////////////////////////////////////////////
  widgetclass Bcolor
  
  option -variable       {}
  option -showlabel      0
  option -label          ""
  option -optmenu        0
 
  variable widget
  variable data
  variable created

  #///////////////////////////////////////////////////////////////////////////
  constructor { args } {
  #
  # PURPOSE: bcolor constructor. An instance of bcolor is created.
  #
  # PARAMETERS:
  #   args -> options
  #
  #///////////////////////////////////////////////////////////////////////////
    set created 0
    #Label
    set widget(label) [label $self.lb -text [$self cget -label]]
    #Frame that contains button
    set widget(frame) [frame $self.fr -bd 1 -relief sunken]
    #Little frame with the color under the button
    set widget(fcolor) [frame $widget(frame).fcolor -height 4]
    bind $widget(fcolor) <Button-1> [list $self _ColorChange]
    
    set widget(optmenu) [ArrowButton $widget(frame).boptm -width 14 -height 16\
                         -dir bottom -command  [list $self _ShowMenu]]

    #Button
    install color using Button $widget(frame).bt -image [::Bitmap::get pen] \
                  -command [list $self _ColorChange]
    
    set data(tracecmd) [list $self _ColorUpdate]
    
    $self configure -variable  [from args -variable ""]
    $self configure -optmenu   [from args -optmenu ""]

    $self createwidget
    
    $self configure -showlabel [from args -showlabel ""]
    $self configure -label     [from args -label ""]

    $self configurelist $args

    # The widget is created
    set created 1
  }
  
  #///////////////////////////////////////////////////////////////////////////
  destructor { 
  #
  # PURPOSE:
  #
  #///////////////////////////////////////////////////////////////////////////
    trace vdelete [$self cget -variable] w $data(tracecmd)
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
    if { $options(-variable) != "" } {
      trace vdelete $options(-variable) w $data(tracecmd)
    }
    set options(-variable) $var
    trace variable $options(-variable) w $data(tracecmd)
    $self _ColorUpdate
  }

  onconfigure -optmenu { lst } {
    set options(-optmenu) $lst
    #if {$lst == 0} {
    #  grid remove $widget(optmenu)
    #} else {
    #  grid $widget(optmenu)
    #}
  }

  onconfigure -label { lst } {
    $widget(label) configure -text $lst
    set options(-label) $lst
  }

  #///////////////////////////////////////////////////////////////////////////
  method createwidget { } {
  #
  # PURPOSE: The widget and the popup menu are created and packed
  #
  #///////////////////////////////////////////////////////////////////////////  
    set optmenu [$self cget -optmenu]

    if {$optmenu} {
      set menu [menu $self.m -tearoff 0]
      bind $widget(fcolor) <Button-3>  [list $self _ShowMenu]
      bind $color          <Button-3>  [list $self _ShowMenu]
    }
    #Menu with option Without color
    if {[string equal $optmenu 1]} {
      $menu add command -label [mc "Without color"] \
                        -command [list set [$self cget -variable] {} ]
    }
    #Menu with option Default color
    if {[string equal $optmenu 2]} {
      $menu add command -label [mc "Default color"] \
            -command [list set [$self cget -variable] defcolor]
    }
    #Menu with options Without color and Default color   
    if {[string equal $optmenu 3]} {
      $menu add command -label [mc "Without color"] \
            -command [list set [$self cget -variable] {} ]
      $menu add command -label [mc "Default color"] \
            -command [list set [$self cget -variable] defcolor]
    }

    if {$optmenu} {
      grid $color          $widget(optmenu) -sticky news
      grid $widget(fcolor) ^                -sticky news
    } else {
      grid $color          -sticky news
      grid $widget(fcolor) -sticky news
      
    }
    grid $widget(label) $widget(frame)
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method _ShowMenu { } {
  #
  # PURPOSE: Shows popup menu to choose between default color or empty color
  #
  #//////////////////////////////////////////////// ///////////////////////////  
    set x [winfo rootx $self]
    set y [expr [winfo rooty $self] + [winfo height $self]]
    tk_popup $self.m $x $y
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method _ColorChange { } {
  #
  # PURPOSE: Changes the color when OK button is pressed. If CANCEL is pressed
  #          leaves the previous color.
  #
  #//////////////////////////////////////////////// ///////////////////////////
    eval set col $[$self cget -variable]
    if {![string equal $col ""] && ![string equal $col "defcolor"]} {
      set newcolor [tk_chooseColor -parent $self  \
        -title [mc "Color selection"] -initialcolor $col]
    } else  {
      set newcolor [tk_chooseColor -parent $self  \
                                   -title [mc "Color selection"]]
    }
    
    if { $newcolor != "" } {
      set [$self cget -variable] $newcolor
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method _ColorUpdate { args } {
  #
  # PURPOSE: Updates the color of the frame when the color is changed.
  #
  #//////////////////////////////////////////////// ///////////////////////////
    eval set col $[$self cget -variable]
    if { ($col == "") || ($col == "defcolor") } {
      $widget(fcolor) configure -bg [$self cget -bg]
    } else {
      $widget(fcolor) configure -bg $col
    }
  }

  # delegation of methods and options
  delegate method * to color
  delegate option * to color
}

package provide bcolor 1.0

  # ------- TEST CODE ----

#/////////////////////////////////////////////////////////////////////////////
proc test_bcolor {} {
#
# PURPOSE: Test function bcolor
#
#/////////////////////////////////////////////////////////////////////////////
  destroy .top
  toplevel .top
  global micolor
  set micolor "\#000000"

  set bColor [bcolor .top.bColor -label {Text Color:} -variable ::micolor \
                                 -optmenu 3 -showlabel 1]
  
  pack $bColor
}