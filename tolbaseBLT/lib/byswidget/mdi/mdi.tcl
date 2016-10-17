package require Tk 8.4
package require snit
package require fmenu

snit::widget mdidoc {
  widgetclass Mdidoc
  
  typevariable wFocus  ;# topmost window (drawn active)
  typevariable colors
  typevariable cursors
  typevariable offset
  typevariable icons
  typevariable noraise 0

  typemethod icon {name} {
    set icons($name)
  }

  typeconstructor {
    set wFocus {}
    if {$::tcl_platform(platform) == "windows"} {
      array set colors {
        gray50 SystemDisabledText
        white SystemWindow
        darkblue SystemHighlight
        gray SystemButtonFace
      }
      array set cursors {
        {} {}
        nw size_nw_se
        ne size_ne_sw
        sw size_ne_sw
        se size_nw_se
        n size_ns
        s size_ns
        w size_we
        e size_we    
      }
    } else {
      array set colors {
        gray50 gray50
        white white
        darkblue darkblue
        gray gray
      }
      array set cursors {
        {} {}
        nw top_left_corner
        ne top_right_corner
        sw bottom_left_corner
        se bottom_right_corner
        n sb_v_double_arrow
        s sb_v_double_arrow
        w sb_h_double_arrow
        e sb_h_double_arrow
      }
    }
    set offset 0
    set icons(minimize) [::image create bitmap -data {
      #define _width 8
      #define _height 8
      static char _bits[] = { 0x00,0x00,0x00,0x00,0x00,0x3e,0x3e,0x00 };
    }]
    set icons(maximize) [::image create bitmap -data {
      #define _width 8
      #define _height 8
      static char _bits[] = { 0x7f,0x7f,0x41,0x41,0x41,0x41,0x7f,0x00 };
    }]
    set icons(restore) [::image create bitmap -data {
      #define _width 8
      #define _height 8
      static char _bits[] = { 0x7c,0x7c,0x44,0x5f,0x5f,0x71,0x11,0x1f };
    }]
    set icons(close) [::image create bitmap -data {
      #define _width 8
      #define _height 8
      static char _bits[] = { 0x00,0x63,0x36,0x1c,0x1c,0x36,0x63,0x00 };
    }]

    set img_doc_data \
"R0lGODlhCwANAPcAAAAAAAEBAQICAgMDAwQEBAUFBQYGBgcHBwgICAkJCQoK
CgsLCwwMDA0NDQ4ODg8PDxAQEBERERISEhMTExQUFBUVFRYWFhcXFxgYGBkZ
GRoaGhsbGxwcHB0dHR4eHh8fHyAgICEhISIiIiMjIyQkJCUlJSYmJicnJygo
KCkpKSoqKisrKywsLC0tLS4uLi8vLzAwMDExMTIyMjMzMzQ0NDU1NTY2Njc3
Nzg4ODk5OTo6Ojs7Ozw8PD09PT4+Pj8/P0BAQEFBQUJCQkNDQ0REREVFRUZG
RkdHR0hISElJSUpKSktLS0xMTE1NTU5OTk9PT1BQUFFRUVJSUlNTU1RUVFVV
VVZWVldXV1hYWFlZWVpaWltbW1xcXF1dXV5eXl9fX2BgYGFhYWJiYmNjY2Rk
ZGVlZWZmZmdnZ2hoaGlpaWpqamtra2xsbG1tbW5ubm9vb3BwcHFxcXJycnNz
c3R0dHV1dXZ2dnd3d3h4eHl5eXp6ent7e3x8fH19fX5+fn9/f4CAgIGBgYKC
goODg4SEhIWFhYaGhoeHh4iIiImJiYqKiouLi4yMjI2NjY6Ojo+Pj5CQkJGR
kZKSkpOTk5SUlJWVlZaWlpeXl5iYmJmZmZqampubm5ycnJ2dnZ6enp+fn6Cg
oKGhoaKioqOjo6SkpKWlpaampqenp6ioqKmpqaqqqqurq6ysrK2tra6urq+v
r7CwsLGxsbKysrOzs7S0tLW1tba2tre3t7i4uLm5ubq6uru7u7y8vL29vb6+
vr+/v8DAwMHBwcLCwsPDw8TExMXFxcbGxsfHx8jIyMnJycrKysvLy8zMzM3N
zc7Ozs/Pz9DQ0NHR0dLS0tPT09TU1NXV1dbW1tfX19jY2NnZ2dra2tvb29zc
3N3d3d7e3t/f3+Dg4OHh4eLi4uPj4+Tk5OXl5ebm5ufn5+jo6Onp6erq6uvr
6+zs7O3t7e7u7u/v7/Dw8PHx8fLy8vPz8/T09PX19fb29vf39/j4+Pn5+fr6
+vv7+/z8/P39/f7+/v///yH5BAEAAKoALAAAAAALAA0AAAg5AFUJHEhQIICD
CA8OBPCvoUOGBh0+VMgw4UNVDCVerHhQIgCMGjf+S5jxY8aQJkdaHAkyJEuS
JAMCADs="

    set icons(doc) [image create photo -data $img_doc_data]

    event add <<Maximize>> <Shift-Control-M>
    event add <<Minimize>> <Shift-Control-W>
    event add <<Restore>>  <Shift-Control-R>
  }

  option -type
  option -title
  option -activeforeground 
  option -activebackground
  option -lowerforeground 
  option -lowerbackground
  option -font
  option -minsize {100 30}
  option -icontext ""
  option -state normal
  option -image ""
  option -width 250
  option -height 150
  option -x 0
  option -y 0
  option -closecmd ""
  option -iniconfig ""
  option -modal 0
  option -resizable 1
  option -helpkey ""
  
  variable NormX
  variable NormY
  variable IconX -1
  variable IconY -1
  variable Xr1
  variable Xr2
  variable Yr1
  variable Yr2
  variable Xp0
  variable Yp0
  variable WMin
  variable HMin
  variable XMax
  variable YMax
  variable XPos ""
  variable YPos ""
  variable constructed 0

  typemethod disableglobalraise {} {
    set noraise 1
  }
  
  typemethod allowglobalraise {} {
    set noraise 0
  }

  proc global_raise { w } {
    #puts "global_raise for $w"
    if { $noraise } {
      set noraise 0
      return
    }
    if {![winfo exists $w] } return
    while {[set cls [winfo class $w]] ne "Toplevel" && $w ne "."} {
      if {$cls eq "Mdidoc"} {
        #puts "voy con un Mdidoc ...."
        $w raise
        return
      }
      set w [winfo parent $w]
    }
  }

  onconfigure -x { x } {
    set options(-x) $x

    place $win -x $x
  }

  onconfigure -y { y } {
    set options(-y) $y

    place $win -y $y
  }
  
  onconfigure -width { d } {
    set options(-width) $d

    if {$options(-state) eq "normal"} {
      place $win -width $d
    }
  }

  onconfigure -height { d } {
    set options(-height) $d

    if {$options(-state) eq "normal"} {
      place $win -height $d
    }
  }
  
  onconfigure -modal { m } {
    set options(-modal) $m
    if { $options(-modal) == 1 } {
      ::grab set $self
      bind $self <FocusOut> [list $self raise]
    } else {
      ::grab release $self      
      bind $self <FocusOut> ""
    }
  }

  onconfigure -resizable { r } {
    set options(-resizable) $r
    if { $options(-resizable) && $options(-state) == "normal" } {
      bind $win <1>               [mymethod _resize1 %X %Y]
      bind $win <B1-Motion>       [mymethod _resize2 %X %Y]
      bind $win <ButtonRelease-1> [mymethod _resize3]
      bind $win <Motion>          [mymethod _motion %X %Y]
      
      grid $win.head.maxb
      $win.head.icon.m entryconf Maximize -state normal
    } else {
      bind $win <1>               {}
      bind $win <B1-Motion>       {}
      bind $win <ButtonRelease-1> {}
      bind $win <Motion>          {}
      if { $options(-state) == "maximized" } { $self restore }
      grid remove $win.head.maxb
      $win.head.icon.m entryconf Maximize -state disabled
    }
    
  }

  # name: -font
  # args: sFont: font to set
  # option handler. sets the title font.
  onconfigure -font { f } {
    set options(-font) $f
    #$win.head.text configure -font $f
    $win.head.title configure -font $f
  }

  # name: -image
  # args: sImage: image to set
  # option handler. sets the image in the title bar.
  onconfigure -image { img } {
    if {$img eq ""} {
      set img $icons(doc)
    }
    set options(-image) $img
    #$win.head.icon configure -image $img
    $win.head.icon configure -image $img
  }

  # name: -title
  # args: sTitle: title to set
  # option handler. sets title text.

  onconfigure -title { str } {
    set options(-title) $str
    #$win.head.text configure -text $str
    $win.head.title configure -text $str
  }

  # name: -state
  # args: sState: new Document state
  # option handler. sets the state for the document. the variable -state
  # is set in the respective (private) member functions.
  onconfigure -state { state } {
    switch -- [mdi::complete_name $state {normal minimized maximized withdrawn}] {
      normal    { $self restore }
      minimized { $self minimize }
      maximized { $self maximize }
      withdrawn { $self withdraw }
    }
  }

  onconfigure -helpkey { key } {
      $self bind <F1> "ShowHelp $key"      
  }
  
  method _set_defaults { } {
    set options(-title) $win
    set options(-activeforeground) $colors(white)
    set options(-activebackground) $colors(darkblue)
    set options(-lowerforeground) $colors(gray)
    set options(-lowerbackground) $colors(gray50)
    #set options(-font) [$win.head.text cget -font]
    set options(-font) [$win.head.title cget -font]
    set NormX $offset
    set NormY $offset
  }

  constructor {args} {
    # create title bar elements
    #pack [frame $win.head -border 0] -padx 2 -pady 2 -fill x
    frame $win.head -border 0

    #pack [label $win.head.icon -border 0] -side left -padx 1
    set options(-image) $icons(doc)
    menubutton $win.head.icon -border 0 -image $icons(doc) \
        -menu $win.head.icon.m 
    # define popup menu
    puts $win.head.icon
    menu $win.head.icon.m -tearoff 0
    $win.head.icon.m add command -label Restore  -command [mymethod restore]
    $win.head.icon.m add command -label Minimize -command [mymethod minimize]
    $win.head.icon.m add command -label Maximize -command [mymethod maximize]
    $win.head.icon.m add separator
    $win.head.icon.m add command -label Close -command [mymethod kill]

    #pack [label $win.head.clob -border 2 -relief raised \
    #          -image $icons(close)] -side right -pady 2 -padx 2
    button $win.head.clob -image $icons(close) -bd 1 -padx 0 -pady 0 \
        -command [mymethod kill]

    #pack [label $win.head.maxb -border 2 -relief raised \
    #          -image $icons(maximize)] -side right -pady 2
    button $win.head.maxb -image $icons(maximize) -bd 1 -padx 0 -pady 0 \
        -command [mymethod maximize]

    #pack [label $win.head.minb -border 2 -relief raised \
    #          -image $icons(minimize)] -side right -pady 2
    button $win.head.minb -image $icons(minimize) -bd 1 -padx 0 -pady 0 \
        -command [mymethod minimize]

    #pack [label $win.head.text -border 0 -anchor w] -side left \
    #    -fill x -expand 1
    label $win.head.title -border 0 -anchor w

    # work area where all inner widgets will be go
    #pack [frame $win.work -border 0] -fill both -expand 1 -padx 2 -pady 2
    frame $win.work -border 0

    grid $win.head.icon -row 0 -column 0 -sticky w
    grid $win.head.title -row 0 -column 1 -sticky "ew"
    grid $win.head.minb -row 0 -column 2 -pady 2 -sticky e
    if { $options(-resizable) } {
      grid $win.head.maxb -row 0 -column 3 -pady 2 -sticky e
    }
    grid $win.head.clob -row 0 -column 4 -pady 2 -sticky e
    grid columnconfigure $win.head 1 -weight 1

    grid $win.head -row 0 -column 0 -sticky "new"
    grid $win.work -row 1 -column 0 -sticky "snew" -padx 2 -pady 2
    grid rowconfigure $win 1 -weight 1
    grid columnconfigure $win 0 -weight 1

    # make sure mouse cursor resets when not over the boarder
    bind $win.work <Enter> "$hull configure -cursor {}"
    bind $win.head <Enter> "$hull configure -cursor {}"
    # default help
    # bind menu to icon and close button
    #bind $win.head.icon <1>        [mymethod _show_menu]
    bind $win.head.icon <Double-1> "[mymethod kill]; break"
    #bind $win.head.clob <1>        [mymethod kill]

    # increase offset for next window
    incr offset 16
    if {[expr {$offset+64}] > [winfo height [winfo parent $win]]} {
      set offset 0
    }

    set tmdi [winfo toplevel $win]
    set script_1 [bind $tmdi <1>]
    if {[lsearch $script_1 ::mdidoc::global_raise] == -1} {
      bind $tmdi <1> "+::mdidoc::global_raise %W"
    }
    $hull configure -bd 2 -relief raised
    $self _set_defaults
    $self configure -helpkey    \
                      [from args -helpkey $::project::data(defaulthelpkey)]
    $self configurelist $args
    puts $args
    $self restore
    # .ini
    $self readIni
    $self raise
    #$self _register
    
    set constructed 1
  }

  destructor {
    #$self _unregister
  }

  method kill { } {
    $self writeIni
    if {$options(-closecmd) ne ""} {
      set script [string map [list %W $win] $options(-closecmd)]
	  puts $script
      if {![uplevel 0 $script]} { return }
    }
    destroy $win
  }

  # -type is readonly
  onconfigure -type {value} {
    if {!$constructed} {
      set options(-type) $value
    }
  }

  method restore { } {
    if {$constructed && $options(-state) eq "normal"} return
    # if we come from iconized state, store the icon position
    if {$options(-state) == "minimized"} {
      set IconX $options(-x)
      set IconY $options(-y)
    }
    # morph -x and -y to contain the window position
    set options(-x) $NormX
    set options(-y) $NormY

    $self _title_bar on
    # show the working area and place Document on last known position
    #pack  $win.work -fill both -expand 1 -padx 2 -pady 2
    grid $win.work
    place $win -x $NormX -y $NormY -width $options(-width) \
      -height $options(-height) -relwidth 0 -relheight 0

    # restore icons and menu entries for all title bar elements
    $win.head.maxb configure -image $icons(maximize)
    $win.head.minb configure -image $icons(minimize)
    #$win.head.text configure -text $options(-title)
    $win.head.title configure -text $options(-title)
    $win.head.icon.m entryconf Minimize -state normal
    $win.head.icon.m entryconf Maximize -state normal
    $hull configure -border 2
    
    if { $options(-resizable) } {
      grid $win.head.maxb
      $win.head.icon.m entryconf Maximize -state normal
    } else {
      grid remove $win.head.maxb
      $win.head.icon.m entryconf Maximize -state disabled
    }

    # restore resize bindings
    if { $options(-resizable) } {
      bind $win <1>               [mymethod _resize1 %X %Y]
      bind $win <B1-Motion>       [mymethod _resize2 %X %Y]
      bind $win <ButtonRelease-1> [mymethod _resize3]
    } 
    
    # restore move bindings
    bind $win.head.title <1>               [mymethod _move1 %X %Y]
    bind $win.head.title <B1-Motion>       [mymethod _move2 %X %Y]
    bind $win.head.title <ButtonRelease-1> [mymethod _move3]

    # restore maximize and minimize bindings
#     bind $win.head.maxb <1>        [mymethod maximize]
#     bind $win.head.minb <1>        [mymethod minimize]
    if { $options(-resizable) } {
      $win.head.maxb configure -command [mymethod maximize]
      bind $win.head.title <Double-1> [mymethod maximize]
    }
    $win.head.minb configure -command [mymethod minimize]
    
    # restore binding for mouse cursor changes over borders
    if { $options(-resizable) } {
      bind $win <Motion> [mymethod _motion %X %Y]
    } else {
      bind $win <Motion> {}
    }

    set options(-state) normal
    $self raise
    event generate $win <<Restore>> -when now
 }

  typemethod mdiparent { w } {
    if {$w eq "." || $w eq ""} {
      return ""
    }
    if {[winfo class $w] eq "Mdidoc"} {
      return $w
    }
    return [mdidoc mdiparent [winfo parent $w]]
  }

  method Bind { w args } {
    eval ::bind $w $args
    foreach _w [winfo children $w] {
      eval $self Bind $_w $args 
    }
  }

  method bind { args } {
    if {[llength $args] < 2} {
      return [eval ::bind $win $args]
    }
    eval $self Bind $win $args
  }

  method raise { } {
    #puts "---- method raise para $win [array names options -activeback*]"
    catch {$wFocus lower}
    set wFocus $win
    
    # esto es lo mas raro, o algo de lo mas raro que he visto en tcl
    # para reproducir el error cambie el valor de la variable skip_bug_tcl_or_snit a 0
    set skip_bug_tcl_or_snit 1
    
    if {$skip_bug_tcl_or_snit && $::tcl_platform(platform) eq "windows"} {
      set options(-activebackground) SystemHighlight
      set options(-activeforeground) SystemWindow
    }
    #puts "dime : [array names options -activefore*]"
    #puts "y ahora : [$win cget -activeforeground]"
    #puts "dime mas : [array get options -activefore*]"
   
    
    $win.head configure -bg $options(-activebackground)
    $win.head.icon configure -bg $options(-activebackground)
    $win.head.title configure -bg $options(-activebackground) \
        -fg $options(-activeforeground)
    
    foreach w [list $win.head.minb $win.head.maxb $win.head.clob] {
      $w configure -highlightbackground $options(-activebackground)
    }
    if {[mdidoc mdiparent [focus]] ne [mdidoc mdiparent $win]} {
      focus $win
    }
    if { $options(-state) eq "maximized" } {
      set mdi [winfo parent $win]
      set mdimenu [$mdi cget -mdimenu]
      $mdimenu set_wcurrent $win
      set wtop [winfo toplevel $win]
      set titletop [$mdimenu cget -title]
      wm title $wtop "$titletop \[[$win cget -title]\]"
    }
    raise $win
  }

  # name: lower
  # args: -
  # draws the title bar in grey.
  method lower {} {
    $win.head configure -bg $options(-lowerbackground)
    $win.head.icon configure -bg $options(-lowerbackground)
    $win.head.title configure -bg $options(-lowerbackground) \
        -fg $options(-lowerforeground)
    foreach w [list $win.head.minb $win.head.maxb $win.head.clob] {
      $w configure -highlightbackground $options(-lowerforeground)
    }
  }

  method _show_menu { } {
    $win.menu post [winfo rootx $win.head.icon] \
        [expr [winfo height $win.head.icon] + [winfo rooty $win.head.icon]]
    focus -force $win.menu
    bind $win.menu <FocusOut> "$win.menu unpost"
  }

  # args: xp, yp: mouse position
  # called only when the mouse is over the document's border. returns an area
  # indicator of the mouse position, e.g. sw for south-west = bottom left.
  # used to set the mouse cursor and to determine the resize mode.
  method _get_edge {xp yp} {
    incr xp [expr {-[winfo rootx $win]}]
    incr yp [expr {-[winfo rooty $win]}]
    
    # allow max. 16 pixel for edge detection
    set iXEdge [expr ($options(-width) < 16)?$options(-width) >>1:8]
    set iYEdge [expr ($options(-height) < 16)?$options(-height)>>1:8]
    
    # detect if mouse is on one of the edges (x coordinate)
    if { $xp < $iXEdge } {
      set sPosH w
    } elseif { $xp > [expr $options(-width)-$iXEdge] } {
      set sPosH e
    } else {
      set sPosH {}
    }
    
    # detect if mouse is on one of the edges (y coordinate)
    if { $yp < $iYEdge } {
      set sPosV n
    } elseif { $yp > [expr $options(-height)-$iYEdge] } {
      set sPosV s
    } else {
      set sPosV {}
    }
    
    return $sPosV$sPosH
  }
  
  # name: _motion
  # args: iXp, iYp: mouse position (from the %X and %Y fields of an event)
  # called by a <Motion> binding over the document frame. Sets the mouse cursor
  # to show arrows depending on the region of the mouse. _getEdge returns an
  # area (e.g. ne for north-east = upper right corner), the aCursor array
  # maps this to a valid cursor name (depending on platform).
  method _motion { xp yp } {
    $hull configure -cursor $cursors([$self _get_edge $xp $yp])
  }
  
  # name: _move1
  # args: iXp, iYp: mouse position (from the %X and %Y fields of an event)
  # called on a <Button-1> event on the title bar. stores the current mouse
  # pointer position. raises the document.
  method _move1 { xp yp } {
    $self raise

    set wParent [winfo parent $win]
    set Xr1 [winfo rootx $wParent]
    set Yr1 [winfo rooty $wParent]
    set Xr2 [expr $Xr1+[winfo width  $wParent]]
    set Yr2 [expr $Yr1+[winfo height $wParent]]
    
    set Xp0 $xp
    set Yp0 $yp
  }

  # name: _move2
  # args: iXp, iYp: mouse position (from the %X and %Y fields of an event)
  # called on a <B1-Motion> event on the title bar. calculates the new
  # window position and places it there.
  method _move2 { xp yp } {
    if { $xp < $Xr1 } { set xp $Xr1 }
    if { $yp < $Yr1 } { set yp $Yr1 }
    if { $xp > $Xr2 } { set xp $Xr2 }
    if { $yp > $Yr2 } { set yp $Yr2 }
    
    place $win -x [expr $options(-x)-$Xp0+$xp] -y [expr $options(-y)-$Yp0+$yp]
  }

  # name: _move3
  # args: -
  # called on a <ButtonRelease-1> event on the title bar. stores the actual
  # window position in some private variables.
  method _move3 {} {
    set options(-x) [winfo x $win]
    set options(-y) [winfo y $win]
  }
  
  # name: _resize1
  # args: iXp, iYp: mouse position (from the %X and %Y fields of an event)
  # called on a <1> event on one of the borders. stores the current mouse
  # position. depending on the mouse position (at an edge or not), the
  # resize mode is determined.
  method _resize1 { xp yp } {
    $self raise
    
    set WMin [lindex $options(-minsize) 0]
    set HMin [lindex $options(-minsize) 1]
    
    set XMax [expr $options(-x)+$options(-width) -$WMin]
    set YMax [expr $options(-y)+$options(-height)-$HMin]
    
    set Xp0 $xp
    set Yp0 $yp
    
    set sPos [$self _get_edge $xp $yp]
    set sXPos {}; regexp {[we]} $sPos sXPos; set XPos $sXPos
    set sYPos {}; regexp {[ns]} $sPos sYPos; set YPos $sYPos
  }
  
  # name: _resize2
  # args: iXp, iYp: mouse position (from the %X and %Y fields of an event)
  # called on a <B1-Motion> event on the borders. calculates the new
  # Document sizes and position and places it depending on the resize mode.
  method  _resize2 { xp yp } {
    # sometimes the <1> event and _resize1 didn't precede this one...
    if { $XPos eq "" && $YPos eq ""} {
      return
    }
    
    if { $XPos eq "w" } {
      set X [expr $options(-x)-$Xp0+$xp]
      if { $X > $XMax } { set X $XMax }
      
      set W [expr $options(-width)+$Xp0-$xp]
      if { $W < $WMin } { set W $WMin }
      
      place $win -x $X -width $W
    } elseif { $XPos eq "e" } {
      set W [expr $options(-width)-$Xp0+$xp]
      if { $W < $WMin } { set W $WMin }
      
      place $win -width $W
    }
    
    if { $YPos eq "n" } {
      set Y [expr $options(-y)-$Yp0+$yp]
      if { $Y > $YMax } { set Y $YMax }
      
      set H [expr $options(-height)+$Yp0-$yp]
      if { $H < $HMin } { set H $HMin }
      
      place $win -y $Y -height $H
    } elseif { $YPos eq "s" } {
      set H [expr $options(-height)-$Yp0+$yp]
      if { $H < $HMin } { set H $HMin }
      
      place $win -height $H
    }
  }
  
  # name: _resize3
  # args: -
  # called on a <ButtonRelease-1> event on the borders. stores the actual
  # window size and position in some private variables.
  method _resize3 {} {
    set options(-x) [winfo x $win]
    set options(-y) [winfo y $win]
    set options(-width)  [winfo width $win]
    set options(-height) [winfo height $win]
  }
  
  # name: _withdraw
  # args: -
  # simply lets the Document disappear.
  method withdraw {} {
    place forget $win
    set options(-state) withdrawn
  }

  # name: _maximize
  # args: -
  # maximizes the document. exchanges the icon for the maximize button.
  # deletes bindings for resize and move.
  method maximize {} {
    if {$constructed && $options(-state) eq "maximized"} return
    # if it was minimized or withdrawn: set to default
    # if we come from normal state, store the window position
    if { $options(-state) eq "normal" } {
      set NormX $options(-x)
      set NormY $options(-y)
    } else {
      $self restore
    }
    
    # place it to fill entirely its master
    place $win -x 0 -y 0 -width 0 -height 0 -relwidth 1 -relheight 1
    $win.head.maxb configure -image $icons(restore)
    $win.head.icon.m entryconf Maximize -state disabled
    $hull configure -border 0
    
    # delete bindings for resizing
    bind $win <1> {}
    bind $win <B1-Motion> {}
    bind $win <ButtonRelease-1> {}
    
    # modify/delete bindings for moving
    bind $win.head.title <1> [mymethod raise]
    bind $win.head.title <B1-Motion> {}
    bind $win.head.title <ButtonRelease-1> {}
    
    # delete binding for mouse cursor changes over borders
    bind $win <Motion> {}
    
    # change bindings for maximize button and title bar
    bind $win.head.title <Double-1> [mymethod restore]
    #bind $win.head.maxb <1> [mymethod restore]
    $win.head.maxb configure -command [mymethod restore]
    
    set options(-state) maximized
    event generate $win <<Maximize>> -when now
  }
  
  method _title_bar { {status ""} } {
    if {$status eq ""} {
      return [expr {[llength [grid info $win.head]]?on:off}] 
    }
    if {$status eq "on"} {
      if {![llength [grid info $win.head]]} {
        grid $win.head
      }
    } elseif {$status eq "off"} {
      if {[llength [grid info $win.head]]} {
        grid remove $win.head
      }
    } else {
      error "$self _title_bar, invalid status value '$status': must be 'on' or 'off'"
    }
  }

  # name: _minimize
  # args: -
  # minimizes the document. exchanges the icon for the minimize button.
  # deletes bindings for resize.
  method minimize {} {
    if {$constructed && $options(-state) eq "minimized"} return
    # if it was maximized or withdrawn: set to default
    # if we come from normal state, store the window position
    if { $options(-state) == "normal" } {
      set NormX $options(-x)
      set NormY $options(-y)
    } else {
      $self restore
    }
    
    # morph -x and -y to contain the icon position
    if { $IconX != -1 } {
      set options(-x) $IconX
      set options(-y) $IconY
    } else {
      set options(-x) $NormX
      set options(-y) $NormY
    }
    
    # withdraw the document's working area
    #pack forget $win.work
    grid remove $win.work
    $win.head.minb configure -image $icons(restore)
    $win.head.icon.m entryconf Minimize -state disabled
    
    # eventually change title text
    if { $options(-icontext) ne "" } {
      $win.head.title configure -text $options(-icontext)
    }
    
    place $win -x $options(-x) -y $options(-y) \
        -width 128 -height [expr [winfo reqheight $win.head] + 8] -relwidth 0 -relheight 0
    
    # delete bindings for resizing
    bind $win <1> {}
    bind $win <B1-Motion> {}
    bind $win <ButtonRelease-1> {}
    
    # delete binding for mouse cursor changes over borders
    bind $win <Motion> {}
    
    # change bindings for minimize button and title bar
    bind $win.head.title <Double-1> [mymethod restore]
    #bind $win.head.minb <1> [mymethod restore]
    $win.head.minb configure -command [mymethod restore]
    
    set options(-state) minimized
    event generate $win <<Minimize>> -when now
  }
  
  method client { } {
    return $win.work
  }

  #///////////////////////////////////////////////////////////////////////////
  method writeIni { } {
  #
  # PURPOSE: Writes to .ini file information about document properties
  #
  # PARAMETERS:
  #   self: document
  #
  #///////////////////////////////////////////////////////////////////////////
    if {[string length [$self cget -iniconfig]]} {
      # comprobamos cargado namespace y variable de .ini
      #&& [info exists idIni]
      if {[namespace exists ::iniFile]} {
        set wini ::iniFile::Write
        set nm [$self cget -iniconfig]

        if { [$self cget -state] ne "minimized" } {
          $wini $nm mdistate [$self cget -state]
        }
        if { $options(-resizable) } {
          $wini $nm mdiwidth  [$self cget -width]
          $wini $nm mdiheight [$self cget -height]
        }
        $wini $nm mdix [$self cget -x]
        $wini $nm mdiy [$self cget -y]
      }
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method readIni { } {
  #
  # PURPOSE: Recovers from .ini file information about document properties
  #
  # PARAMETERS:
  #   self: document
  #
  #///////////////////////////////////////////////////////////////////////////
    if {[string length [$self cget -iniconfig]]} {
      # comprobamos cargado namespace y variable de .ini
      #&& [info exists idIni]
      if {[namespace exists ::iniFile]} {
        set rini ::iniFile::Read
        set nm [$self cget -iniconfig]
        set stt [$rini $nm mdistate normal]
        $self configure -state $stt
        if { $stt eq "normal" } {
	  if { $options(-resizable) } {
            set width [$rini $nm mdiwidth [$self cget -width]]
            set height [$rini $nm mdiheight [$self cget -height]]
            if {[string length $width]} { $self configure -width $width }
            if {[string length $height]} { $self configure -height $height }
	  }
	  
          set x [$rini $nm mdix [$self cget -x]]
          set y [$rini $nm mdiy [$self cget -y]]
          if {[string length $x]} { $self configure -x $x }
          if {[string length $y]} { $self configure -y $y }
        }
      }
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method getframe { } {
  #
  # PURPOSE: returns the document framework
  #
  # PARAMETERS:
  #   self: document
  #
  #///////////////////////////////////////////////////////////////////////////
    return $self.work
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method setgeometry { geometry } {
  #
  # PURPOSE: Set geometry
  #
  # PARAMETERS:
  #   geometry -> geometry specification
  #///////////////////////////////////////////////////////////////////////////
    if {[string length $geometry]} {
      set width    [lindex [split $geometry x] 0]
      set restgeom [lindex [split $geometry x] 1]
      set height   [lindex [split $restgeom +] 0]
      set x        [lindex [split $restgeom +] 1]
      set y        [lindex [split $restgeom +] 2]
      $self configure -width $width -height $height -x $x -y $y
    }
  }
}

snit::widgetadaptor mdimenu {
  # base title to preserve when adding the doc maximized titles
  option -title
  # fmenu instance
  variable menu
  # current document attached to this fmenu
  variable wcurrent ""
  variable wmaster ""
  variable save_title
  
  delegate method * to hull
  delegate option * to hull

  constructor {args} {
    installhull using fmenu

    bind MaxDoc <<Minimize>> [mymethod unlink_doc]
    bind MaxDoc <<Restore>> [mymethod unlink_doc]
    bind MaxDoc <Destroy> [mymethod unlink_doc 1]
    
    $self configurelist $args
    
    if { $options(-title) eq "" } {
      set options(-title) [wm title .]
    }
  }

  method create_buttons { } {
    # create label/buttons
    menubutton $win.icon -menu $win.icon.menu -relief flat -bd 0
    # create icon menu
    menu $win.icon.menu -tearoff 0
    grid $win.icon -row 0 -column 0 -sticky "w"
    grid remove $win.icon
    $win.icon.menu add command -label Restore -command [mymethod restore_doc]
    $win.icon.menu add command -label Minimize -command [mymethod minimize_doc]
    $win.icon.menu add command -label Maximize -state disabled
    $win.icon.menu add separator
    $win.icon.menu add command -label Close -command [mymethod kill_doc]
    button $win.minb -image [mdidoc icon minimize] -bd 1 -padx 0 -pady 0 \
        -command [mymethod minimize_doc]
    grid $win.minb -row 0 -column 20 -sticky "e"
    grid remove $win.minb
    button $win.resb -image [mdidoc icon restore] -bd 1 -padx 0 -pady 0 \
        -command [mymethod restore_doc]
    grid $win.resb -row 0 -column 21 -sticky "e"
    grid remove $win.resb
    button $win.clob -image [mdidoc icon close] -bd 1 -padx 0 -pady 0 \
        -command [mymethod kill_doc]
    grid $win.clob -row 0 -column 22 -sticky "e"
    grid remove $win.clob    
  }

  # link_doc --
  #
  # link doc to menu interface, this grid the corresponding buttons and
  # undate commands in those buttons to reference this document.

  method link_doc { wdoc } {
    if {$wdoc ne ""} {
      set wcurrent $wdoc
      set wmaster [winfo parent $wdoc]
      if {![winfo exists $win.icon]} {
        $self create_buttons
      }
      foreach w [list $win.icon $win.minb $win.resb $win.resb $win.clob] {
        if {![llength [grid info $w]]} {
          grid $w
        }
      }
      set tags [bindtags $wdoc]
      if {[lsearch $tags "MaxDoc"] == -1} {
        bindtags $wdoc [lappend tags "MaxDoc"]
      }
      $win.icon configure -image [$wdoc cget -image]
      set wtop [winfo toplevel $win]
      #set save_title [wm title $wtop]
      #wm title $wtop "$save_title \[[$wdoc cget -title]\]"
      wm title $wtop "$options(-title) \[[$wdoc cget -title]\]"
    }
  }

  # unlink_doc --
  #
  # unlink current doc, unpacking corresponding buttons.

  method unlink_doc { {killed 0} } {
    grid remove $win.icon
    grid remove $win.minb
    grid remove $win.resb
    grid remove $win.clob
    if {!$killed && $wcurrent ne ""} {
      set tags [bindtags $wcurrent]
      if {[set idx [lsearch $tags "MaxDoc"]]!=-1} {
        set tags [lreplace $tags $idx $idx]
        bindtags $wcurrent $tags
      }
      #wm title [winfo toplevel $win] $save_title
      wm title [winfo toplevel $win] $options(-title)
    }
    # check for another maximized in wmaster
    set wmax ""
    foreach w [winfo children $wmaster] {
      if {$w ne $wcurrent && [winfo class $w] eq "Mdidoc"
          && [$w cget -state] eq "maximized"} {
        set wmax $w
      }
    }
    set wcurrent ""
    set wmaster ""
    if {$wmax ne ""} {
      $self link_doc $wmax
    }
  }

  method restore_doc { } {
    if {$wcurrent ne ""} {
      $wcurrent restore
    }
  }

  method minimize_doc { } {
    if {$wcurrent ne ""} {
      $wcurrent minimize
    }
  }

  method kill_doc { } {
    if {$wcurrent ne ""} {
      $wcurrent kill
    }
  }
  
  method set_wcurrent { w } {
    set wcurrent $w
  }
}

snit::widget mdi {
  widgetclass Mdi

  delegate option * to hull

  option -mdimenu ""
  option -backimg ""

  variable counter 0
  variable allTypes ;# list of all types registered
  variable allDocs  ;# array of all document windows classified by type

  constructor { args } {

    set allTypes {}
    set allDocs() {}
    $self configurelist $args
    label $win.lback -image $options(-backimg)
    pack $win.lback -fill both -expand yes
    update
  }

  onconfigure -backimg {img} {
    set options(-backimg) $img
    if {[winfo exists $win.lback]} {
      $win.lback configure -image $img
    }
  }

  method get_types { } {
    set allTypes
  }

  method get_documents {args} {    
    set result {}
    if {[llength $args]} {
      set type_names $args
    } else {
      set type_names $allTypes
      if {[llength $allDocs()]} {
        lappend result "" $allDocs()
      }
    }
    foreach t $type_names {
      lappend result $t $allDocs($t)
    }
    set result
  }

  method slave { args } {
    set wslave [$self _uniquename]
    eval ::mdidoc $wslave $args
    $self _register $wslave
    if { [$wslave cget -state] eq "maximized"} {
       # inexplicably the following is required to maximize correctly
       update
       $wslave configure -state "normal"
       $wslave configure -state "maximized"
    }
    return $wslave
  }

  method _register { w } {
    set t [$w cget -type]
    if {$t ne "" && [lsearch $allTypes $t] == -1} {
      lappend allTypes $t
    }
    lappend allDocs($t) $w
    bind $w <Destroy> +[mymethod _unregister $w]
    bind $w <<Maximize>> [mymethod on_maximize $w]
    bind $w <<Minimize>> [mymethod on_minimize $w]
    bind $w <<Restore>>  [mymethod on_restore $w]
  }

  method _unregister { w } {
    set t [$w cget -type]
    set idx [lsearch $allDocs($t) $w]
    if {$idx != -1} {
      set allDocs($t) [lreplace $allDocs($t) $idx $idx]
      if {$t ne "" && ![llength $allDocs($t)]} {
        array unset allDocs $t
        set idx [lsearch $allTypes $t]
        if {$idx != -1} {
          set allTypes [lreplace $allTypes $idx $idx]
        }
      }
    }
  }

  method on_maximize { w } {
    if {$options(-mdimenu) ne ""} {
      $w _title_bar off
      $options(-mdimenu) link_doc $w
    }
  }

  method on_minimize { w } {
  }

  method on_restore { w } {
  }

  # name: arrange
  # args: sHow: arrange mode
  #       sParent: a valid parent widget of document windows(s).
  # static member function. resizes and repositions all Document objects
  # in one of several ways. it always arrange iconfied windows before
  # it repositions the non-iconfied windows.

  method arrange { how } {
    
    # first check if at least one document window exists
    if {[llength [array names allDocs]]==1 && ![llength $allDocs()]} {
      return
    }
    
    set how [complete_name $how \
                 {horizontally vertically cascade \
                      tile icons maximize minimize}]
    
    
    # get a list of all iconified windows and a list of all others
    set lDocs {}
    set lIcon {}
    foreach t [array names allDocs] {
      foreach doc $allDocs($t) {
	if { [$doc cget -state] != "withdrawn" } {
	  if { [$doc cget -state] == "minimized" } {
	    lappend lIcon $doc
	  } else {
	    lappend lDocs $doc
	  }
	}
      }
    }
    
    # get parent window's dimensions for later calculation
    set sParent [winfo parent $doc]
    set iW [winfo width  $sParent]
    set iH [winfo height $sParent]
    
    # if there are iconfied windows, arrange them first and reduce iH
    set iXd 0
    foreach sDoc $lIcon {
      set iWd [winfo width $sDoc]
      if { [expr $iXd+$iWd] > $iW } {
        set iXd 0
      }
      if { $iXd == 0 } {
        incr iH -[winfo height $sDoc]
      }
      $sDoc configure -x $iXd -y $iH
      incr iXd $iWd
    }
    
    # how many non-iconified windows? just exit when none
    set iDocs [llength $lDocs]
    if { $iDocs == 0 } return
    
    switch $how {
      horizontally {
        set iStep [expr $iH/$iDocs]
        for { set i 0 } { $i < $iDocs } { incr i } {
          set sDoc [lindex $lDocs $i]
          $sDoc restore
          $sDoc configure -x 0 -y [expr $i*$iStep] -width $iW -height $iStep
        }
      }
      
      vertically {
        set iStep [expr $iW/$iDocs]
        for { set i 0 } { $i < $iDocs } { incr i } {
          set sDoc [lindex $lDocs $i]
          $sDoc restore
          $sDoc configure -x [expr $i*$iStep] -y 0 -width $iStep -height $iH
        }
      }
      
      cascade {
        set iStep 24
        for { set i 0 } { $i < $iDocs } { incr i } {
          set sDoc [lindex $lDocs $i]
          $sDoc restore
          $sDoc configure -x [expr $i*$iStep] -y [expr $i*$iStep] ;#-width [expr $iW*2/3] -height [expr $iH*2/3]
          $sDoc raise
        }
      }
      
      tile {
        for { set i 0 } { $i <= 100 } { incr i } {
          set iHor [expr ($i+3)/2]
          set iVer [expr ($i+2)/2]
          if { [expr $iHor*$iVer] >= $iDocs } break
        }
        
        set iXStep [expr $iW/$iHor]
        set iYStep [expr $iH/$iVer]
        for { set i 0 } { $i < $iDocs } { incr i } {
          set sDoc [lindex $lDocs $i]
          $sDoc restore
          $sDoc configure -x [expr ($i%$iHor)*$iXStep] -y [expr ($i/$iHor)*$iYStep] -width $iXStep -height $iYStep
        }
      }
      
      maximize {
        foreach sDoc $lDocs {
          $sDoc maximize
        }
      }
      
      minimize {
        foreach sDoc $lDocs {
          $sDoc minimize
        }
      }
    }
  }

  proc complete_name {short long_values {what option}} {
    foreach v $long_values {
      set tmp($v) {}
    }
    if { [info exists tmp($short)] } {
      return $short
    } else {
      set match [array names tmp $short*]

      if { [llength $match] == 1 } {
        return $match
      } elseif { [llength $match] == 0 } {
        error "bad $what '$short': must be [join $long_values ,]"
      } else {
        error "ambiguous $what '$short': must be [join $long_values ,]"
      }
    }    
  }

  method _uniquename { } {
    while 1 {
      incr counter
      if {$counter > 2147483647} {
        set counter 0
      }
      set candidate $win.slave$counter
      if {![winfo exists $candidate]} {
        return $candidate
      }
    }
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method writeDocsIni { } {
  #
  # PURPOSE: Calls the writeIni method from each mdi document child
  #
  # PARAMETERS:
  #   self: mdi
  #
  #///////////////////////////////////////////////////////////////////////////
    foreach t [array names allDocs] {
      foreach doc $allDocs($t) {
        $doc writeIni
      }
    }
	
  }

  #///////////////////////////////////////////////////////////////////////////
  method cicle {} {
  #
  # PURPOSE: cicles the focus between mdi children
  #
  #///////////////////////////////////////////////////////////////////////////
  
    set lista ""
  
    foreach type [array names allDocs] {
	  foreach doc $allDocs($type) {
	    if { [$doc cget -state] != "withdrawn" } {
	      lappend lista $doc
	    }
      }
    }

    set index 0
    foreach doc $lista {
	  if { $::mdidoc::wFocus eq $doc } { break }
	  incr index
    }
	set doc [lindex $lista [expr $index + 1]]
	#puts "$lista $doc"
	if { $doc ne "" } {
	  $doc raise
	} else {
	  [lindex $lista 0] raise
	}
  }
}




# ------- TEST CODE ----

proc SelectDoc { dw } {
  if {[$dw cget -state] eq "minimized"} {
    $dw restore
  } else  {
    $dw raise
  }
}

proc FillWindowMenu {mdi m} {
  $m delete 0 end
  foreach cw [winfo children $m] {
    destroy $cw
  }
  $m add command -label "Tile across" -underline 0 -command "$mdi arrange tile" 
  $m add command -label "Tile horizontally" -underline 5 \
                 -command "$mdi arrange hor" 
  $m add command -label "Tile vertically" -underline 5 -command "$mdi arrange ver"
  $m add command -label "Cascade" -underline 0 -command "$mdi arrange cascade"
  $m add separator
  $m add command -label "Arrange icons" -underline 0 -command "$mdi arrange icons"
  $m add command -label "Minimize all" -underline 0 -command "$mdi arrange min"
  foreach t [$mdi get_types] {
    $m add cascade -label $t -menu $m.sub$t
    menu $m.sub$t -tearoff 0
    foreach d [lindex [$mdi get_documents $t] 1] {
      $m.sub$t add command -label [$d cget -title] -command "SelectDoc $d "
    }
  }
  foreach d [lindex [$mdi get_documents {}] 1] {
    $m add command -label [$d cget -title] -command "SelectDoc $d"
  }
  if {[$m index end] > 6} {
    $m insert 7 separator
  }
}

set test_path [file normalize [file dir [info script]]]

proc test_mdi { } {
  destroy .tmdi

  set md [menudef create %AUTO%]
  set id [$md add root cascade -label "&File"]
  set cid [$md add $id cascade -label "&New"]
  $md add $cid command -label "&Editor" \
      -command ".tmdi.master slave -type Editor -resizable 0"
  $md add $cid command -label "&Graph" \
      -command ".tmdi.master slave -type Graph"
  $md add $cid command -label "&Table" \
      -command ".tmdi.master slave -type Table"
  $md add $cid command -label "&SQL" \
      -command ".tmdi.master slave -type SQL"
  $md add $id command -label "&Open ..."
  $md add $id command -label "&Close"
  $md add $id separator
  $md add $id command -label "&Exit"
  $md add root cascade -label "&Window" \
      -postcommand [list FillWindowMenu .tmdi.master %W]
  $md add root separator
  set id [$md add root cascade -label "&Help"]
  $md add $id command -label "&Contents ..."
  $md add $id separator
  $md add $id command -label "&About ..."  

  #$md dump
  #return
  toplevel .tmdi
  mdimenu .tmdi.m -definition $md
  update
  set bimg [image create photo \
                -file [file join $::test_path "../../toltk/images/splash.gif"]]
  mdi .tmdi.master -mdimenu .tmdi.m -relief sunken -bd 1 \
      -backimg $bimg
  grid .tmdi.m -row 0 -column 0 -sticky "new"
  grid .tmdi.master -row 1 -column 0 -sticky "snew"
  grid columnconfigure .tmdi 0 -weight 1
  grid rowconfigure .tmdi 1 -weight 1
  $md destroy
}


package provide mdi 0.1
