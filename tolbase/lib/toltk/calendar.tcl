#/////////////////////////////////////////////////////////////////////////////
# FILE    : calendar.tcl
# PURPOSE : This file contains the namespace CalendarTms. 
#           CalendarTms is for creating a several months calendar to
#           highlight dates in a TOL TimeSet.
#/////////////////////////////////////////////////////////////////////////////


#/////////////////////////////////////////////////////////////////////////////
# Namespace CalendarTms
#/////////////////////////////////////////////////////////////////////////////

#/////////////////////////////////////////////////////////////////////////////
  namespace eval CalendarTms {
#
# PURPOSE: The purpose of this namespace is organize code and functions.
#          A special namespace is created for each instance of CalendarTms.
#
# VARIABLES:
#   tmpOpt  -> temporary array used to store the widgets and variables of the
#              options dialog.
#   calendarExpert ->
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpOpt
  variable calendarExpert ""
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::CalendarTms::Create {path {tms {}} } {
#
# PURPOSE: Creates a CalendarTms instance. Creates and packs the calendar
#          window (frame with toolbar, month calendars and status bar)
#
# PARAMETERS:
#   path -> 
#   tms  -> Timeset that is used. 
#
# RETURN: CalendarTms instance
#
#/////////////////////////////////////////////////////////////////////////////
  set instance ::CalendarTms::$path

  #  A namespace is created for each bayes graph widget.

  namespace eval $instance {
  
    # data -> array with information about the instance:
    #   this  -> path to frame of the widget
    #   all   -> if it's a general CalendarTms
    #   tms   -> if it isn't a general CalendarTms, the TimeSet to highlight
    #   rows  -> number of rows 
    #   cols  -> number of cols
    #   n     -> number of months that are in the calendar
    #   day   -> day selected
    #   month -> month selected
    #   year  -> year selected

    variable data

  }
  upvar \#0 ${instance}::data data
    
  # Use or not of timeset specify
  if {($tms == "")} {
    set data(all)   1
  } else  {
    set data(tms)   $tms
  }
  # Read the origin configuration
  ReadIni $instance  

  set data(n) [expr $data(rows)*$data(cols)]  
  set data(day)   1

  # Create the widgets
  set data(this)  [frame $path -class CalendarTms]

  bind $data(this) <Destroy> [list ::CalendarTms::Destroy $instance]
  set sw [ScrolledWindow $path.sw]
  # buttons
  set bbox [ButtonBox $path.bb -orient horizontal -spacing 5 -padx 0 -pady 0]
  $bbox add -image [Bitmap::get "begin"] -height 16 -width 16\
    -command [list ::CalendarTms::Adjust $instance  0 -1]\
    -helptext [mc "Moves to previous year"] -relief link
  $bbox add -image [Bitmap::get "reverse"] -height 16 -width 16\
    -command [list ::CalendarTms::Adjust $instance -1 0]\
    -helptext [mc "Moves to previous month"] -relief link
  set data(back) [$bbox add -image [Bitmap::get "backward"] -height 1 -width 3\
        -command [list ::CalendarTms::Back $instance]\
        -helptext [mc "Moves to previous %1\$d months" $data(n)] -relief link]
  set data(next) [$bbox add -image [Bitmap::get "forward"] -height 1 -width 3\
        -command [list ::CalendarTms::Next $instance]\
        -helptext [mc "Moves to next %1\$d months" $data(n)] -relief link]
  $bbox add -image [Bitmap::get "play"] -height 16 -width 16\
    -command [list ::CalendarTms::Adjust $instance  1  0]\
    -helptext [mc "Moves to next month"] -relief link
  $bbox add -image [Bitmap::get "end"] -height 16 -width 16\
    -command [list ::CalendarTms::Adjust $instance  0  1]\
    -helptext [mc "Moves to next year"] -relief link
  set data(zoomin) [$bbox add -image [Bitmap::get "zoomin"] -height 16\
         -width 16\
         -command [list ::CalendarTms::ZoomIn $instance]\
          -helptext [mc "Zoom In"] -relief link]
  set data(zoomout) [$bbox add -image [Bitmap::get "zoomout"] -height 16\
          -width 16\
          -command [list ::CalendarTms::ZoomOut $instance]\
          -helptext [mc "Zoom Out"] -relief link]
  $bbox add -image [Bitmap::get "tools"] -height 16 -width 16\
     -command [list ::CalendarTms::OptionsCreate $instance]\
     -helptext [mc Options] -relief link
  
  if {[info exists data(all)]} { ;# General calendar
    set data(allTms) [::tol::info variable TimeSet]
    #puts "::CalendarTms::Create data(allTms) = $data(allTms)"
    foreach item $data(allTms) {
      ::tol::timeset create $item [list TimeSet $item]
    }
    label $path.lTms -text [mc TimeSet]:
    # Combobox with the timesets
    bcombobox $path.cbTms -width 18 -height 5 -editable false \
        -values $data(allTms) \
        -textvariable ::CalendarTms::${path}::data(tms) \
        -modifycmd [list ::CalendarTms::Display $instance]
  }
  
  set sf [ScrollableFrame $path.sw.fr -width 450]
  $sw setwidget $sf
  set f [$sf getframe]
  set data(frame) $f
  # Creation of the calendar
  CreateCalendars $instance
  # Set the statusbars
  set data(statusbar) [frame $path.status]
  set status_sep [Separator $data(statusbar).ss -orient horizontal]
  set sf [frame $data(statusbar).sf]
  set data(statusbar,cursor) [label $sf.cursor -text "" -relief sunken\
                              -width 13]
  grid $data(statusbar,cursor) -sticky w
  grid columnconfigure $sf 1 -weight 1
  grid $status_sep -sticky ew -pady 2
  grid $sf         -sticky ew

  grid columnconfigure $data(statusbar) 0 -weight 1

  if {[info exists data(all)]} {
    # If it's a general calendar
    grid $path.bb $path.lTms $path.cbTms -sticky nw -padx 2
    grid $path.sw     -columnspan 3      -sticky news
    grid $path.status -columnspan 3      -sticky  ew
  } else  {
    # If the calendar is of specific timeset
    grid $path.bb     -sticky nw -padx 2
    grid $path.sw     -sticky news
    grid $path.status -sticky  ew
  }
  # Packing the canvas
  grid rowconfigure    $path 1 -weight 1
  grid columnconfigure $path 0 -weight 1

}

#/////////////////////////////////////////////////////////////////////////////
proc ::CalendarTms::CreateCalendars {instance} {
#
# PURPOSE:  Associates the events and packs every month of calendar.
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${instance}::data data
  
  set month $data(month)
  set year  $data(year)
  
  for {set i 0} {$i < $data(n)} {incr i} {
    # associates the events of every month of the calendar.
    date::Chooser $data(frame).$i $month $year \
	[list OnClickDay "::CalendarTms::GetDay $instance" \
	     CheckDate   "::CalendarTms::CheckDate $instance" \
	     OnDoubleDay "::CalendarTms::ShowHours $instance"] 
    incr month
    if {$month>12} {set month 1; incr year}
  }
  # Pack every month of calendar
  for {set i 0} {$i < $data(rows)} {incr i} {
    for {set j 0} {$j < $data(cols)} {incr j} {
        grid $data(frame).[expr $data(cols)*$i + $j] -row $i -column $j \
                                                     -sticky news

    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc  ::CalendarTms::Destroy { instance } {
#
# PURPOSE: Destroy the instance of CalendarTms
#
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 ${instance}::data data
  # Eliminates the different used timesets
  if {[info exists data(all)]} {
    foreach item $data(allTms) {
      ::tol::timeset destroy $item
    }
  }
  # Save the state  
  WriteIni $instance
  # Delete the namespace of the instace
  namespace delete $instance
}

#/////////////////////////////////////////////////////////////////////////////
proc ::CalendarTms::ReadIni {instance} {
#
# PURPOSE: Load the configuration of calendar.
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${instance}::data data
  ::date::Init
  upvar \#0 ::date::a a
  set rini ::iniFile::Read

  # Definition of options of the calendar
  # Year of the first month of calendar  
  set data(year)     [$rini Calendar year  [lindex [::tol::date now] 0]]
  # First month of calendar
  set data(month)    [$rini Calendar month [lindex [::tol::date now] 1]]
  # Definition of position and separation between the elements of calendar
  set data(rows)     [$rini Calendar rows 2]
  set data(cols)     [$rini Calendar cols 3]
  set a(dx)          [$rini Calendar dx  18]
  set a(dy)          [$rini Calendar dy  14]
  set a(-mon)        [$rini Calendar mon  0]
  set a(otherday)    [$rini Calendar otherday 1]
  set a(-bg)         [$rini Calendar bg white]
  # Definition of values of presentation of calendar.
  array set def {
    titlefont     {Helvetica 10 bold}
    weekdaysfont  {Helvetica 9}
    dayfont       {Helvetica 9}
    otherfont     {Helvetica 9}
    titlefill     black
    weekdaysfill  blue
    dayfill       black
    otherfill     grey
    h1type        oval
    h1font        {Helvetica 9 bold}
    h1color       red
    #h1fill        LightYellow
    #h1outline     red
    h1fill        ""
    h1outline     ""
    h2type        rect
    h2font        {Helvetica 9 italic}
    h2color       orange
    #h2fill        white
    #h2outline     blue
    h2fill        ""
    h2outline     ""
  }
  # Load of values of configuration
  # Example: -$pre$post-> -dayfill  
  foreach pre {title weekdays other day} {
    foreach post {font fill} {
        set a(-$pre$post) [$rini Calendar $pre$post $def($pre$post)]
    }
  }
  foreach pre {h1 h2} {
    foreach post {font color type fill outline} {
       set a(-$pre$post) [$rini Calendar $pre$post $def($pre$post)]
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::CalendarTms::WriteIni {instance} {
#
# PURPOSE: Save the configuration of calendar.
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${instance}::data data
  upvar \#0 ::date::a a
  set wini ::iniFile::Write
  # Write the values of position and separation between the elements
  $wini Calendar year     $data(year)
  $wini Calendar month    $data(month)
  $wini Calendar rows     $data(rows)
  $wini Calendar cols     $data(cols)
  $wini Calendar dx       $a(dx)
  $wini Calendar dy       $a(dy)
  $wini Calendar mon      $a(-mon)
  $wini Calendar otherday $a(otherday)
  $wini Calendar bg       $a(-bg)

  # Write of values of configuration
  foreach pre {title weekdays other day} {
    foreach post {font fill} {
      $wini Calendar $pre$post $a(-$pre$post)
    }
  }
  foreach pre {h1 h2} {
    foreach post {font color type fill outline} {
      $wini Calendar $pre$post $a(-$pre$post)
    }
  }
  ::iniFile::Flush
}

#/////////////////////////////////////////////////////////////////////////////
proc ::CalendarTms::GetDay {instance day month year tags} {
#
# PURPOSE: Shows the date in format tol in the bottom bar
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${instance}::data data    
#puts "##################"
#puts [ListHours $instance $day $month $year]
  set data(date) $day/$month/$year
  $data(statusbar,cursor) configure\
       -text "y${year}m[format "%02d" $month]d[format "%02d" $day]"    
}

#/////////////////////////////////////////////////////////////////////////////
proc ::CalendarTms::ShowHours {instance day month year tags} {
#/////////////////////////////////////////////////////////////////////////////    
  if { [lsearch $tags type2] != -1 } {
#puts "day: $day"
#puts "month: $month"
#puts "year: $year"
#puts "tags: $tags"
#puts [concat "textSalida: " [ListHours $instance $day $month $year] ]
            
    Tolcon_Trace [ListHours $instance $day $month $year]
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::CalendarTms::DateTol {day month year} {
#/////////////////////////////////////////////////////////////////////////////
#    set day    
#    set month  
#    set year   
}

#/////////////////////////////////////////////////////////////////////////////
proc ::CalendarTms::ZoomIn {instance} {
#
# PURPOSE: Increases the distance between the elements of calendar.
#
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 ${instance}::data data
  upvar \#0 ::date::a a    
  # Increases the distance between the elements
  set a(dx) [expr $a(dx)+1]
  set a(dy) [expr $a(dy)+1]
  # It isn't possible to be extended more
  if {$a(dx) >= $a(dxmax)} { ;# Horizontally
    set a(dx) $a(dxmax)
    $data(zoomin) configure -state disabled
  }
  if {$a(dy) >= $a(dymax)} { ;# Vertically
    set a(dy) $a(dymax)
    $data(zoomin) configure -state disabled
  }
  # Now it's possible to zoom out  
  $data(zoomout) configure -state normal
  Display $instance
}

#/////////////////////////////////////////////////////////////////////////////
proc ::CalendarTms::ZoomOut {instance} {
#
# PURPOSE: Decreases the distance between the elements of calendar.
#
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 ${instance}::data data    
  upvar \#0 ::date::a a
  # Decreases the distance between the elements
  set a(dx) [expr $a(dx)-1]
  set a(dy) [expr $a(dy)-1]
  # It isn't possible to be reduced more
  if {$a(dx) <= $a(dxmin)} { ;# Horizontally
    set a(dx) $a(dxmin)
    $data(zoomout) configure -state disabled
  }
  if {$a(dy) <= $a(dymin)} { ;# Vertically
    set a(dy) $a(dymin)
    $data(zoomout) configure -state disabled
  }
   # Now it's possible to zoom in
  $data(zoomin) configure -state normal
  Display $instance
}

#/////////////////////////////////////////////////////////////////////////////
proc ::CalendarTms::Display {instance} {
#
# PURPOSE: Displays each one of the months of calendar
#
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 ${instance}::data data  
  set n [expr $data(rows)*$data(cols)]
  # Display each month
  for {set i 0} {$i < $n} {incr i} {    
    date::Display ::date::$data(frame).$i
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::CalendarTms::ReDisplay {instance} {
#
# PURPOSE: Refresh the calendar.
#
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 ${instance}::data data
  # Destroy the previous calendar
  for {set i 0} {$i < $data(n)} {incr i} {
    destroy $data(frame).$i
  }
  set data(n) [expr $data(rows)*$data(cols)]
  # Configure back and next buttons   
  $data(back) configure -helptext \
        [mc "Moves to previous %1\$d months" $data(n)]
  $data(next) configure -helptext \
        [mc "Moves to next %1\$d months" $data(n)]
  # Create and display updated calendar 
  CreateCalendars $instance
  Display $instance
}

#/////////////////////////////////////////////////////////////////////////////
proc ::CalendarTms::Adjust {instance dmonth dyear} {
#
# PURPOSE: Move to a calendar witch starts in a concrete month and year,
#          with a displacement respect to the present values.
#
# PARAMETERS:
#     dmonth -> Amount of months to displace. Example: 1 next or -1 previous
#     dyear -> Amount of years to displace. Example: 1 next or -1 previous
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${instance}::data data
  # The increases or decrements are applied
  incr data(year)  $dyear
  incr data(month) $dmonth
  # Increases the year of the calendar
  if {$data(month)>12} {
    incr data(year)  [expr $data(month)/12] 
    set data(month)  [expr $data(month)%12]
  }
  # Decreases the year of the calendar
  if {$data(month)<1}  {
    incr data(year) -[expr 1+abs($data(month))/12]
    set data(month) [expr 12 - (abs($data(month))%12)]        
  }
  # Adjust the calendars of all the months
  set n [expr $data(rows)*$data(cols)]
  for {set i 0} {$i < $n} {incr i} {
    date::Adjust ::date::$data(frame).$i $dmonth $dyear
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::CalendarTms::Back {instance} {
#
# PURPOSE: Moves the calendar to the previous [n number of months].
#
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 ${instance}::data data
  Adjust $instance -$data(n) 0
}

#/////////////////////////////////////////////////////////////////////////////
proc ::CalendarTms::Next {instance} {
#
# PURPOSE: Moves the calendar to the following [n number of months].
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${instance}::data data
  Adjust $instance $data(n) 0
}

# Return -

#/////////////////////////////////////////////////////////////////////////////
proc ::CalendarTms::CheckDate {instance day month year} {
#
# PURPOSE: Check if a date is belongs to a Timeset.
#
# PARAMETERS:
#     day, month, year -> Date to check.
#       
# RETURN:
#    0 - date does not belong to tms
#    1 - date does belong to tms but only the hour 0 of that day
#    2 - date does belong and some other hour of the same day
#
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 ${instance}::data data
  set result 0
  set date "$year $month $day"
  # The calendar has a Timeset
  if { [string length $data(tms)] } {
    # The date belongs to the Timeset 
    #puts "::CalendarTms::CheckDate data(tms) = $data(tms)"    
    set result [$data(tms) contain $date]
   #foreach {y0 m0 d0 h0 mi0 s0} [$data(tms) start [list $year $month $day]] break
   # if { $y0 == $year && $m0 == $month && $d0 == $day } {
   #   if { [expr $h0 + $mi0 + $s0 > 0] } {
   #     set result 2
   #   } else {
   #     foreach {y0 m0 d0} [$data(tms) next] break
   #     set result [expr (($y0 == $year) && ($m0 == $month) && ($d0 == $day))+1]
   #   }
   # }
  }
  return $result
}

#/////////////////////////////////////////////////////////////////////////////
proc ::CalendarTms::ListHours {instance day month year} {
#
# PURPOSE:
#
# PARAMETERS:
#     day, month, year -> Date 
#
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 ${instance}::data data
  set lstHours {}
  # The calendar has a Timeset
  if { [string length $data(tms)] } {
    set tick [$data(tms) start [list $year $month $day]]
    foreach {y0 m0 d0} $tick break
    while { ($y0 == $year) && ($m0 == $month) && ($d0 == $day) } {
      lappend lstHours [lrange $tick 3 end]
      set tick [$data(tms) next]
      foreach {y0 m0 d0} $tick {
        break
      }
    }
  }
  set lstHours
}

#/////////////////////////////////////////////////////////////////////////////
proc ::CalendarTms::ViewTimeSet {} {
#
# PURPOSE:
#
#/////////////////////////////////////////////////////////////////////////////    
  # create
  if { [winfo exists $::CalendarTms::calendarExpert] } {
    if { [$::CalendarTms::calendarExpert cget -state] eq "minimized" } {
      $::CalendarTms::calendarExpert restore
    }
    $::CalendarTms::calendarExpert raise
	return
  } else {
    set tl [::project::CreateForm \
              -width 470 -height 345 \
              -iniconfig Calendar -title [mc "TimeSets calendar"] \
	      -helpkey  "Tb4Cal"]
	set tlf [$tl getframe]
    set ::CalendarTms::calendarExpert $tl
    # config
	::CalendarTms::Create $tlf.t
    $tl bind <Control-F4> [list $tl kill]
    pack $tlf.t -fill both -expand yes
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::CalendarTms::OptionsCreate {instance} {
#
# PURPOSE: It creates the a dialog of options of the calendar
#
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 ${instance}::data data
  variable tmpOpt
  # Read the options that the calendar had previously
  OptionsGet $instance
  # Create the dialog  
  set path $data(this)
  set dialog [Dialog $data(this).t -title [mc "Calendar Options"]\
            -default 0 \
            -parent $data(this) -modal local]
  wm protocol $dialog WM_DELETE_WINDOW \
        [list ::CalendarTms::OptionsDestroy $instance $dialog]
  # Create the frame within the dialog
  set w [$dialog getframe]
  wm resizable $dialog 0 0
    
  # Create the set of tabs
  set w_tabset [NoteBook $w.nb -tabbevelsize 8 -tabpady {2 6} -font {Arial 8}] 
  set num_i 0
  foreach lab_i [list General Style Highlight] {
    $w_tabset insert $num_i $lab_i -text "  [mc $lab_i]  " -background \#d9d9d9 
    set tab_i [$w_tabset getframe $lab_i]
    $tab_i configure -borderwidth 2 -background \#d9d9d9
    incr num_i    
    set f$num_i [frame $tab_i.f]
    pack $tab_i.f -fill both -expand yes  
  }
  $w_tabset raise [$w_tabset pages 0]
          
  # Fill up each one of the tabs
  OptionsCreateGeneral $instance $f1
  OptionsCreateStyle   $instance $f2
  OptionsCreateHighlight $instance $f3
    
  set w_bbox \
          [ButtonBox $w.bb -orient horizontal -spacing 10 -padx 0 -pady 0 \
          -homogeneous false]
  $w_bbox add -text [mc "Accept"] -relief link \
          -helptext [mc "Accept"] \
          -image [::Bitmap::get accept] -compound left -padx 5 -pady 1 \
          -command [list ::CalendarTms::OptionsOK $instance $dialog]
  $w_bbox add -text [mc "Cancel"] -relief link \
          -helptext [mc "Cancel"]\
          -image [::Bitmap::get cancelx] -compound left -padx 5 -pady 1 \
          -command [list ::CalendarTms::OptionsDestroy $instance $dialog]
  $w_bbox add -text [mc "Apply"] -relief link \
          -helptext [mc "Apply"]\
          -image [::Bitmap::get apply] -compound left -padx 5 -pady 1 \
          -command [list ::CalendarTms::OptionsApply $instance]     
    
  # Pack all created components
  pack $w_bbox -fill x -expand false -side bottom
  $w_tabset compute_size
  pack $w_tabset -fill both -expand true
    
  $dialog draw
}

#/////////////////////////////////////////////////////////////////////////////
proc ::CalendarTms::OptionsCreateGeneral {instance w} {
#
# PURPOSE: Creation of the tab "General Options"
#
# PARAMETERS:
#     w -> frame container
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpOpt
  TitleFrame $w.tf -text [mc "General Options"]
  set f [$w.tf getframe]

  #month and year
  label $f.lm -text [mc Month]:
  SpinBox $f.sbm -range {1 12 1}\
    -textvariable ::CalendarTms::tmpOpt(var,month) -width 6 -editable 0
  label $f.ly -text [mc Year]:
  SpinBox $f.sby -range {0 9999 1}\
     -textvariable ::CalendarTms::tmpOpt(var,year) -width 6
  #rows and columns
  label $f.lr -text [mc Rows]:
  SpinBox $f.sbr -range {1 8 1}\
    -textvariable ::CalendarTms::tmpOpt(var,rows) -width 6 -editable 0
  label $f.lc -text [mc Columns]:
  SpinBox $f.sbc -range {1 8 1}\
    -textvariable ::CalendarTms::tmpOpt(var,cols)  -width 6 -editable 0
  #other options
  checkbutton $f.mon -text [mc "Sunday starts week"]\
     -variable ::CalendarTms::tmpOpt(var,-mon)    
  checkbutton $f.oth  -text [mc "Other days"]\
     -variable ::CalendarTms::tmpOpt(var,otherday)

  frame $f.f -width 25
  grid $f.f $f.lm   $f.sbm  $f.ly   $f.sby -sticky nw -pady 1 -padx 2 
  grid ^    $f.lr   $f.sbr  $f.lc   $f.sbc -sticky nw -pady 1 -padx 2 
  grid ^    $f.mon  -       -       $f.oth -sticky nw -pady 2 -padx 2
  grid columnconfigure $f 4 -weight 1
  grid rowconfigure    $f 2 -weight 1
  grid columnconfigure $f 1 -pad 20

  grid $w.tf -sticky news -padx 2
  grid rowconfigure    $w 0 -weight 1
  grid columnconfigure $w 0 -weight 1
}

#/////////////////////////////////////////////////////////////////////////////
proc ::CalendarTms::OptionsCreateStyle {instance w} {
#
# PURPOSE: Creation of the tab "Appareance Options"
#
# PARAMETERS:
#     w -> frame container
#
#/////////////////////////////////////////////////////////////////////////////    
  global toltk_options
  variable tmpOpt
  TitleFrame $w.tf -text [mc "Appareance Options"]
  set f [$w.tf getframe]
  #Colors and fonts
  label $f.lbg -text [mc "Background"]:
  ::Objects::ColorCreate ::CalendarTms::tmpOpt(var,-bg) $f.fbg

  label $f.lFont  -text [mc "Font"]  -font $toltk_options(fonts,Title1)
  label $f.lColor -text [mc "Color"] -font $toltk_options(fonts,Title1)

  label $f.lt -text [mc "Month names"]:
  ::Objects::FontCreate ::CalendarTms::tmpOpt(var,-titlefont) $f.btf
  ::Objects::ColorCreate ::CalendarTms::tmpOpt(var,-titlefill) $f.ftc
  label $f.lw -text [mc "Weekdays"]:
  ::Objects::FontCreate ::CalendarTms::tmpOpt(var,-weekdaysfont) $f.bwf
  ::Objects::ColorCreate ::CalendarTms::tmpOpt(var,-weekdaysfill) $f.fwc
  label $f.ld -text [mc "Month days"]:
  ::Objects::FontCreate ::CalendarTms::tmpOpt(var,-dayfont) $f.bdf
  ::Objects::ColorCreate ::CalendarTms::tmpOpt(var,-dayfill) $f.fdc
  label $f.lo -text [mc "Other days"]:
  ::Objects::FontCreate ::CalendarTms::tmpOpt(var,-otherfont) $f.bof
  ::Objects::ColorCreate ::CalendarTms::tmpOpt(var,-otherfill) $f.foc

  frame $f.f -width 25
  grid $f.f $f.lbg $f.fbg    -          -sticky nw -pady 0 -padx 2
  grid ^    ^      $f.lFont  $f.lColor  -sticky n -pady 2 -padx 2 
  grid ^    $f.lt  $f.btf    $f.ftc     -sticky n -pady 0 -padx 2
  grid ^    $f.lw  $f.bwf    $f.fwc     -sticky n -pady 0 -padx 2
  grid ^    $f.ld  $f.bdf    $f.fdc     -sticky n -pady 0 -padx 2
  grid ^    $f.lo  $f.bof    $f.foc     -sticky n -pady 0 -padx 2
  
  grid configure $f.lbg $f.lt $f.lw $f.ld $f.lo -sticky nw

  grid columnconfigure $f 4 -weight 1
  grid rowconfigure    $f 6 -weight 1

  grid $w.tf -sticky news -padx 2
  grid rowconfigure    $w 0 -weight 1
  grid columnconfigure $w 0 -weight 1
}

#/////////////////////////////////////////////////////////////////////////////
proc ::CalendarTms::OptionsCreateHighlight {instance w} {
#
# PURPOSE: Creation of the tab "Highligth Appareance Options"
#
# PARAMETERS:
#     w -> frame container
#
#/////////////////////////////////////////////////////////////////////////////    
  global toltk_options
  variable tmpOpt
  TitleFrame $w.tf -text [mc "Highligth Appareance Options"]
  set f [$w.tf getframe]


  frame $f.f
  label $f.lFont    -text [mc "Font"]    -font $toltk_options(fonts,Title1)
  label $f.lColor   -text [mc "Color"]   -font $toltk_options(fonts,Title1)
  label $f.lType    -text [mc "Type"]    -font $toltk_options(fonts,Title1)
  label $f.lFill    -text [mc "Fill"]    -font $toltk_options(fonts,Title1)
  label $f.lOutline -text [mc "Outline"] -font $toltk_options(fonts,Title1)

  label $f.lh1 -text [mc "Full highligth"]:
  ::Objects::FontCreate ::CalendarTms::tmpOpt(var,-h1font) $f.bh1fo
  ::Objects::ColorCreate ::CalendarTms::tmpOpt(var,-h1color) $f.fh1c
  radiobutton $f.rbh1O -text [mc oval] -value oval -highlightthickness 0 \
    -pady 0 -variable ::CalendarTms::tmpOpt(var,-h1type)
  radiobutton $f.rbh1R -text [mc "rectangle"] -value rect -highlightthickness 0\
    -pady 0 -variable ::CalendarTms::tmpOpt(var,-h1type)
  ::Objects::ColorCreate ::CalendarTms::tmpOpt(var,-h1fill)    $f.fh1f "" 1
  ::Objects::ColorCreate ::CalendarTms::tmpOpt(var,-h1outline) $f.fh1o "" 1
  label $f.lh2 -text [mc "Partial highligth"]: -anchor w
  ::Objects::FontCreate ::CalendarTms::tmpOpt(var,-h2font) $f.bh2fo
  ::Objects::ColorCreate ::CalendarTms::tmpOpt(var,-h2color) $f.fh2c
  radiobutton $f.rbh2O -text [mc "oval"] -value oval -highlightthickness 0\
    -pady 0 -variable ::CalendarTms::tmpOpt(var,-h2type)
  radiobutton $f.rbh2R -text [mc "rectangle"] -value rect -highlightthickness 0\
    -pady 0 -variable ::CalendarTms::tmpOpt(var,-h2type)
  ::Objects::ColorCreate ::CalendarTms::tmpOpt(var,-h2fill)    $f.fh2f "" 1
  ::Objects::ColorCreate ::CalendarTms::tmpOpt(var,-h2outline) $f.fh2o "" 1

  grid $f.f   $f.lFont $f.lColor $f.lType  -         $f.lFill $f.lOutline \
    -sticky n -pady 2 -padx 2
  grid $f.lh1 $f.bh1fo $f.fh1c   $f.rbh1O  $f.rbh1R  $f.fh1f  $f.fh1o     \
    -sticky n
  grid $f.lh2 $f.bh2fo $f.fh2c   $f.rbh2O  $f.rbh2R  $f.fh2f  $f.fh2o     \
    -sticky n

  grid configure $f.lh1 $f.lh2 -sticky nw

  grid columnconfigure $f 7 -weight 1
  grid rowconfigure    $f 3 -weight 1

  grid $w.tf -sticky news -padx 2
  grid rowconfigure    $w 0 -weight 1
  grid columnconfigure $w 0 -weight 1
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::CalendarTms::OptionsDestroy {this dialog} {
#
# PURPOSE: Destroy options dialog
#
# PARAMETERS:
#   this   -> Instance of CalendarTms
#   dialog -> Options dialog
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpOpt
  array unset tmpOpt
  destroy $dialog
}

#/////////////////////////////////////////////////////////////////////////////
proc ::CalendarTms::OptionsGet {instance} {
#
# PURPOSE: Get the options from the variable temporary tmpOpt to the Calendar
#    
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 ${instance}::data data
  upvar \#0 ::date::a a
  variable tmpOpt
  # Get the the own options of the calendar
  set tmpOpt(var,year)       $data(year)
  set tmpOpt(var,month)      $data(month)
  set tmpOpt(var,rows)       $data(rows)
  set tmpOpt(var,cols)       $data(cols)
  set tmpOpt(var,-mon)       $a(-mon)
  set tmpOpt(var,otherday)   $a(otherday)
  set tmpOpt(var,-bg)        $a(-bg)
  # Get the options of presentation of the calendar  
  foreach pre {-title -weekdays -other -day} {
    foreach post {font fill} {
      set tmpOpt(var,$pre$post) $a($pre$post)
    }
  }
  foreach pre {-h1 -h2} {
    foreach post {font color type fill outline} {
      set tmpOpt(var,$pre$post) $a($pre$post)
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::CalendarTms::OptionsSet {instance} {
#
# PURPOSE: Set the options from the Calendar to the variable temporary tmpOpt
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${instance}::data data
  upvar \#0 ::date::a a
  variable tmpOpt
  # Set the the own options of the calendar
  set data(year)    $tmpOpt(var,year)
  set data(month)   $tmpOpt(var,month)
  set data(rows)    $tmpOpt(var,rows)
  set data(cols)    $tmpOpt(var,cols)
  set a(-mon)       $tmpOpt(var,-mon)
  set a(otherday)   $tmpOpt(var,otherday)
  set a(-bg)        $tmpOpt(var,-bg)
  # Set the options of presentation of the calendar
  foreach pre {-title -weekdays -other -day} {
    foreach post {font fill} {
      set a($pre$post) $tmpOpt(var,$pre$post)
    }
  }
  foreach pre {-h1 -h2} {
    foreach post {font color type fill outline} {
      set a($pre$post) $tmpOpt(var,$pre$post)
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::CalendarTms::OptionsApply {instance} {
#
# PURPOSE: The options are applied, and the dialogue is refreshed without
#          destroying it
#
#/////////////////////////////////////////////////////////////////////////////    
  OptionsSet $instance
  ReDisplay  $instance
}

#/////////////////////////////////////////////////////////////////////////////
proc ::CalendarTms::OptionsOK {instance dialog} {
#
# PURPOSE: The options are applied, and the dialogue is destroyed
#
#/////////////////////////////////////////////////////////////////////////////    
  OptionsApply $instance
  OptionsDestroy $instance $dialog
}
