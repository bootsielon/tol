#/////////////////////////////////////////////////////////////////////////////
# FILE    : bentrydatetcl
# PURPOSE : Entry Date. Componente que nos permite seleccionar un 
#           seleccionar una fecha de un calendario. Permite seleccinar fechas
#           nulas.
# TODO: pensar en algo para la edicion de horas:minutos:segundos          
#/////////////////////////////////////////////////////////////////////////////
 
package require byscommon
package require snit

#/////////////////////////////////////////////////////////////////////////////
# date
#/////////////////////////////////////////////////////////////////////////////
package require bcombobox

#/////////////////////////////////////////////////////////////////////////////
namespace eval date {
#
# PURPOSE: Namespace for a widget of a calendar with an only month.
# 
#/////////////////////////////////////////////////////////////////////////////
  variable a
}

#/////////////////////////////////////////////////////////////////////////////
proc ::date::Init {} {
#
# PURPOSE: Initializes a moth calendar
#
#/////////////////////////////////////////////////////////////////////////////
  variable a
  # Position and separation between the elements of calendar
  set a(x0) 20
  set a(dx) 18
  set a(dxmax) 26
  set a(dxmin) 14
  set a(y0) 35
  set a(dy) 14
  set a(dymax) 22
  set a(dymin) 10
  # Months of the year
  set a(mn) { . January February March April May June July August
      September October November December }
  # Days of the week  
  set a(weekdays)  {Sun Mon Tue Wed Thu Fri Sat}
  array set a {
    format,ja {%y\u5e74 %m\u6708}
    format,zh {%y\u5e74 %m\u6708}
  }
  # Options of presentation for the calendar of the month.
  # No es necesario porque se debe cargar en ReadIni
  array set a {
    -titlefont    {Helvetica 10 bold}  -titlefill    black
    -weekdaysfont {Helvetica 9}        -weekdaysfill blue
    -dayfont      {Helvetica 9}        -dayfill      black
    -otherfont    {Helvetica 9}        -otherfill    grey
    -h1font       {Helvetica 9 bold}   -h1color      red
    -h2font       {Helvetica 9 italic} -h2color      orange
    -h1type oval  -h1fill LightYellow  -h1outline    red
    -h2type rect  -h2fill white        -h2outline    blue
    -bg white     -mon 0
  }
    set a(otherday) 1    
}    
    
#/////////////////////////////////////////////////////////////////////////////
proc ::date::OnClickDay { instance } {
#
# PURPOSE: It selects a day, when doing click in that day on the calendar.
# 
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${instance}::data data
    
  if { [string length $data(OnClickDay)] } {
    # Takes all the items that are in the calendar.
    set item [$data(canvas) find withtag current]
    set tags [$data(canvas) itemcget $item -tags]
    # We obtain the selected day.
    set day [$data(canvas) itemcget $item -text]    
    eval $data(OnClickDay) $day $data(month) $data(year) [list $tags]
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::date::OnDoubleDay { instance } {
#
# PURPOSE: It selects a day, when doing double-click in that day on the
#          calendar.
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${instance}::data data
  
  if { [string length $data(OnDoubleDay)] } {
    # Takes all the items that are in the calendar.        
    set item [$data(canvas) find withtag current]
    set tags [$data(canvas) itemcget $item -tags]
    # We obtain the selected day.    
    set day [$data(canvas) itemcget $item -text]
    eval $data(OnDoubleDay) $day $data(month) $data(year) [list $tags]
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::date::Chooser {path month year events} {
#
# PURPOSE: Creates an instance of date
#
# PARAMETERS:
#     path ->   Path to instance. 
#     month ->  Month of calendar.
#     year ->   Year of calendar.
#     events -> It is a list of associated events and handlers to the dates.
#               {OnClickDay Hdlr1 CheckDate Hdlr1 OnDoubleDay Hdlr3}
#               It must be of even length, is used with Array Set.
#
# RETURN: The instance created.
# 
#/////////////////////////////////////////////////////////////////////////////
  variable a
  set instance ::date::${path}
  if { [lsearch [namespace children ::date] $instance] != -1 } {
    error "$instance already exists as a children of ::date"
  }
  namespace eval $instance {
    variable data
  }
  upvar #0 ${instance}::data data
  # creates list of events and handlers
  array set data [list OnClickDay "" OnDoubleDay "" CheckDate "" \
            day 1 year $year month $month]
  array set data $events
  # creates canvas of the calendar
  set data(canvas) [canvas $path -bg $a(-bg) \
     -relief solid -bd 1 -highlightthickness 1]
  # events of the canvas
  $data(canvas) bind day <1>       	+[list ::date::OnClickDay $instance]
  $data(canvas) bind day <Double-1> +[list ::date::OnDoubleDay $instance]
    
  bind $data(canvas) <Destroy> [list ::date::Destroy $instance]
    
  Display $instance
  return $instance
}

#/////////////////////////////////////////////////////////////////////////////
proc ::date::Destroy { instance } {
#
# PURPOSE: Destroys the instance of date.
# 
#/////////////////////////////////////////////////////////////////////////////    
  namespace delete $instance
}

#/////////////////////////////////////////////////////////////////////////////
proc ::date::Display {instance} {
#
# PURPOSE: Display the calendar of month.
# 
#/////////////////////////////////////////////////////////////////////////////        
  upvar #0 ${instance}::data data
  variable a
  
  set c $data(canvas)
  foreach tag {title otherday day} {$c delete $tag}
    
  # Establishes the horizontal and vertical position and separation between
  # the days.
  set x0 $a(x0)
  set y0 $a(y0)
  set x  $x0
  set y  $y0
  set dx $a(dx)
  set dy $a(dy)
    
  set xmax [expr {$x0+$dx*6}]
    
  # Height and width of calendar of the month
  $c configure -width  [expr {$x0+$dx*6+$x0-2}]    
  $c configure -height [expr {$y0+$dy*6+10-2}]
  # Header of calendar
  set title [Format [MonthName $data(month)] $data(year)]
  $c create text [expr ($xmax+$dx)/2] [expr $y-20] -text $title\
            -fill $a(-titlefill) -font $a(-titlefont) -tag title
  # Week days of calendar
  set weekdays $a(weekdays)
  if !$a(-mon) {lcycle weekdays}
  foreach i $weekdays {
    $c create text $x $y -text [mc $i] -fill $a(-weekdaysfill) \
            -font $a(-weekdaysfont) -tag title
    incr x $dx
  }
  # It obtains the number and week day of the first day of the month
  set first [list $data(year) [RemovePre0 $data(month)] 1 0 0 0.0]
  set weekday [::tol::date weekday $first]
  if !$a(-mon) {set weekday [expr {($weekday+6)%7}]}
  # Position of beginning to draw.
  set x [expr {$x0+$weekday*$dx}]
  set x1 $x; set offset 0
  incr y $dy
  
  if {$a(otherday)} {
    while {$weekday} {
      set day [lindex [::tol::date +day $first -[incr offset]] 2]
      $c create text [incr x1 -$dx] $y -text $day \
              -fill $a(-otherfill) -font $a(-otherfont) -tag otherday
      incr weekday -1
    }
  }
  # It obtains the number of day that has the month
  set dmax [::tol::date days $data(year) [RemovePre0 $data(month)]]
  for {set d 1} {$d<=$dmax} {incr d} {
    # Number of present day    
    set txt [::format "%2s" $d]
    # Number of element that will occupy within canvas
    set id [$c create text $x $y -text $txt -tag day -font $a(-dayfont)\
            -fill $a(-dayfill)]
    # To graticulate the days
    if { ($data(CheckDate) != {}) } {
      set result 0
      set result [eval $data(CheckDate) $d $data(month) $data(year)]
      # This day is totally emphasized 
      if { $result == 1 } {
        $c create $a(-h1type) [$c bbox $id] -fill $a(-h1fill)\
                -outline $a(-h1outline) -tags [list day type$result]
        $c itemconfigure $id -font $a(-h1font) -fill $a(-h1color)
      }
      # This day is partially emphasized
      if { $result == 2 } {
        $c create $a(-h2type) [$c bbox $id] -fill $a(-h2fill)\
                -outline $a(-h2outline) -tags [list day type$result]
        $c itemconfigure $id -font $a(-h2font) -fill $a(-h2color)
      }
      # Shows it into the canvas
      $c raise $id
      # The new tag is added to canvas
      $c addtag type$result withtag $id
    }
    # The position is updated to add the following day
    if {[incr x $dx] > $xmax} {
      set x $x0
      incr y $dy
    }
  }
  if {$a(otherday)} {
    if {$x != $x0} {
      for {set d 1} {$x<=$xmax} {incr d; incr x $dx} {
        $c create text $x $y -text $d -fill $a(-otherfill)\
                -font $a(-otherfont) -tag otherday
      }
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::date::Adjust {instance dmonth dyear} {
#    
# PURPOSE: Move to a calendar established in a concrete month and year to
#          another, with a displacement.
#
# PARAMETERS:
#     dmonth -> Amount of months to displace. Example: 1 next or -1 previous
#     dyear -> Amount of years to displace. Example: 1 next or -1 previous
# 
#/////////////////////////////////////////////////////////////////////////////        
  upvar #0 ${instance}::data data
  # Obtains the present month
  set data(month) [RemovePre0 $data(month)]
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
  # It obtains the number of day that has the month
  set maxday [::tol::date days $data(year) [RemovePre0 $data(month)]]
  if {$maxday < $data(day)} {
    set data(day) $maxday
  }
  # Displays the calendar of the updated month and year
  Display $instance
}

#/////////////////////////////////////////////////////////////////////////////
proc ::date::SetMonth {instance month year} {
#/////////////////////////////////////////////////////////////////////////////
  upvar #0 ${instance}::data data
  set data(year) $year
  set data(month) $month
  # It obtains the number of day that has the month
  set maxday [::tol::date days $data(year) [RemovePre0 $data(month)]]
  if {$maxday < $data(day)} {
    set data(day) $maxday
  }
  # Displays the calendar of the updated month and year
  Display $instance
}
  
#/////////////////////////////////////////////////////////////////////////////
proc ::date::Format {month year} {
#
# PURPOSE: Defines the format of the header of the calendar for a month.
#
# PARAMETERS:
#     month -> Present month.
#     year -> Present year.
# 
#/////////////////////////////////////////////////////////////////////////////        
  variable a
    
  if ![info exists a(format,[::msgcat::mclocale])] {
    set format "%m - %y" ;# default format
  } else { 
    set format $a(format,[::msgcat::mclocale]) ;# locale format
  }
  # Apply the format  
  foreach {from to} [list %m $month %y $year] {
    regsub $from $format $to format
  }
  subst $format
}

#/////////////////////////////////////////////////////////////////////////////
proc ::date::RemovePre0 {value} {
#/////////////////////////////////////////////////////////////////////////////        
 if {[string range $value 0 0] eq 0} {
     set newValue [string range $value 1 1]
  } else {
    set newValue $value
  }
  return $newValue
}

#/////////////////////////////////////////////////////////////////////////////
proc ::date::MonthName {pos} {
#
# PURPOSE: Returns the name of the month which position in the year is given 
#          by pos. Example: 1 -> January, 2 -> February, ...
# 
# PARAMETERS:
#    pos -> Position of the month within the year.
# 
#/////////////////////////////////////////////////////////////////////////////
  variable a
    
  set res [mc [lindex $a(mn) [RemovePre0 $pos]]]
}

#/////////////////////////////////////////////////////////////////////////////
proc ::date::lcycle {_list} {
#/////////////////////////////////////////////////////////////////////////////
  upvar $_list list
    set list [concat [lrange $list 1 end] [list [lindex $list 0]]]
}


#/////////////////////////////////////////////////////////////////////////////
::snit::widget bentrydate {
#
# PURPOSE: Defincición de las funcionalidades de un BayeEntryDate
#
#/////////////////////////////////////////////////////////////////////////////
#/////////////////////////////////////////////////////////////////////////////
# options:
#  -bdate  : inizializamos una fecha del selector
#  -btext  : texto a mostrar en el entry de fechas
#  -bstate : state del componente 
#  -btextvariable: variable asociada al entry
#  -bformat : formato a aplicar a la fecha
#  -postcommand: comando a ejecutar tras obtener la fecha

# variable
#  bdate : almacenamos la fecha a mostrar en el caledario y  la seleccionada
#  data : array interno de almacenamiento
#  dateTxt : fecha seleccionada en texto
#
#/////////////////////////////////////////////////////////////////////////////
  widgetclass BEntryDate

  option -bdate
  option -btext
  option -bstate normal
  option -btextvariable {}
  option -bformat %Y%m%d
  option -postcommand {}

  variable bdate
  variable data
  variable dateTxt
  variable wentry

  #///////////////////////////////////////////////////////////////////////////
  constructor {args} {
  #
  # PURPOSE: bentrydate constructor
  #
  # PARAMETERS:
  #   args ->
  #
  #///////////////////////////////////////////////////////////////////////////  
    # Apply all options passed at creation time.
    $self configurelist $args
    # Create widget
    $self CreateWidget
  }

  #///////////////////////////////////////////////////////////////////////////
  #
  # PURPOSE: Metodos cget y configure
  #
  #///////////////////////////////////////////////////////////////////////////

  oncget -bdate {
    if {[info exists bdate]} {
      return $bdate
    } else {
      return ""
    }
  }

  oncget -btext {
    if {[info exists dateTxt]} {
      return $dateTxt
    } else {
      return "" 
    }
  }

  oncget -btextvariable {
    if {[info exists data(entry)]} {
      $data(entry) cget -textvariable
    }
  }

  onconfigure -bdate {value} {
    if {[expr [llength $value] == 1]} {
      set value [lindex $value 0]
      set value [list [string range $value 0 3] \
                      [string range $value 4 5] \
                      [string range $value 6 7]]
    }
    set bdate $value
    
    if [llength $value] {
      set year  [lindex $value 0]
      set month [lindex $value 1]
      set day   [lindex $value 2]
      if {$month ne ""} { set month [format "%02d" [::date::RemovePre0 $month]] }
      if {$day   ne ""} { set day   [format "%02d" [::date::RemovePre0 $day]]}
      set bdate [list $year $month $day]
      set dateTxt ${year}${month}${day}
      if {[string length $options(-bformat)]} {
        catch {
          set dateTxt [clock format [clock scan $dateTxt] -format $options(-bformat)]
        } res
      }
    } else {
      set dateTxt {}
    }
    if {[info exists data(entry)]} {
      $data(entry) configure -text $dateTxt
    }
  }

  onconfigure -btext {value} {
    set bdate [list [string range $value 0 3]\
                    [string range $value 5 6]\
                    [string range $value 8 9]]
    $self configure -bdate $bdate
  }

  onconfigure -bstate {value} {
    if {[info exists data(entry)]} {
      $data(entry)  configure -state $value
    }
    if {[info exists data(button)]} {
      $data(button) configure -state $value
    }
  }

  onconfigure -btextvariable {value} {
    if {[info exists data(entry)]} {
      $data(entry) configure -textvariable $value
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method CreateWidget {args} {
  #
  # PURPOSE: Creacion de los objetos que compones el widget
  #
  # PARAMETERS:
  #   args ->
  #
  #///////////////////////////////////////////////////////////////////////////
    set data(ftop) [frame $self.f]
    # Entry Date 
    set txt {}
    if {[info exists dateTxt]} {
      set txt $dateTxt
    }
    install wentry using Entry $self.f.e -editable false -width 12 -text $txt
    set data(entry) $wentry
    set data(button) [Button $self.f.b -image [::Bitmap::get Date] \
      -command [list eval $self DateChange]]
  
    #grid $self.f.e $self.f.b -padx 0 -sticky nw
    pack $self.f.b -side right
    pack $self.f.e -side left -fill x -expand true
    pack $self.f -expand true -fill both
    grid columnconfigure $self.f 2 -weight 1
 
    # Selector Date
    ::date::Init
   
    set w [toplevel $self.t]
    set data(fbottom) $w
    wm resizable $w 0 0
    wm overrideredirect $w 1
    wm withdraw $w

    # definir una variable donde almacenar la informacion
     if {[info exists bdate]} {
      set year  [lindex $bdate   0]
      set month [lindex $bdate   1]
    } else {
      set ahora [clock format [clock seconds] -format "%Y %m %d"]
      set year  [lindex $ahora  0]
      set month [lindex $ahora  1]
    }
    set this [::date::Chooser $w.c $month $year \
        [list OnDoubleDay "$self DateGetDay"]]

    #$w.c configure -bd 0
    frame $w.f -bd 1 -bg white -relief solid
  
    Button $w.f.bby -text "<<" -height 1 -bg white\
      -command [list ::date::Adjust $this 0 -1] -relief link -width 2\
      -helptext [mc "Moves to previous year"]
    Button $w.f.bbm -text "< " -height 1 -bg white\
      -command [list ::date::Adjust $this -1  0] -relief link -width 2\
      -helptext [mc "Moves to previous month"]
    Button $w.f.clear -text " X" -height 1 -bg white\
      -command [list $self ClearDate] -relief link -width 2\
      -helptext [mc "Clear date"]
    Button $w.f.bnm -text " >" -height 1 -bg white\
      -command [list ::date::Adjust $this 1 0] -relief link -width 2\
      -helptext [mc "Moves to next month"]
    Button $w.f.bny -text ">>" -height 1 -bg white\
     -command [list ::date::Adjust $this  0  1] -relief link -width 2\
     -helptext [mc "Moves to next year"]

    grid $w.f.bby $w.f.bbm $w.f.clear $w.f.bnm $w.f.bny -sticky nwes  -padx 2

    grid columnconfigure $w.f 2 -weight 1

    grid $w.f -sticky nwe  -padx 2
    grid $w.c -sticky nwe  -padx 2
  }

  #///////////////////////////////////////////////////////////////////////////
  method getentry {} {
  #
  # PURPOSE: funcion para obtener el path del entry donde se visualiza la
  #          fecha
  # RETURN: El path completo del entry 
  #
  #///////////////////////////////////////////////////////////////////////////
    variable data
    return $data(entry)
  }

  #///////////////////////////////////////////////////////////////////////////
  method getbutton {} {
  #
  # PURPOSE: Funcion para obtener el path del button que muesta el calendario
  # RETURN: El path completo del button
  #
  #///////////////////////////////////////////////////////////////////////////
    variable data
    return $data(button)
  }

  #///////////////////////////////////////////////////////////////////////////
  method DateChange {} {
  #
  # PURPOSE: Funcion a aplicar al presiona el boton calendario. Muestra un
  #          calendario para seleccionar una fecha 
  #
  #///////////////////////////////////////////////////////////////////////////
    if {[winfo viewable $data(fbottom)] == 1} {
      wm withdraw  $data(fbottom)
      grab release $data(fbottom)
    } else {
      wm deiconify $data(fbottom)
      focus -force $data(fbottom)
      grab -global $data(fbottom)
      # Positioning the dialog
      set posh [winfo rootx $data(entry)]
      set posv \
            [expr [winfo rooty $data(entry)]+[winfo height $data(entry)]]
      set right [expr $posh + [winfo width $data(fbottom)]]
      set bottom [expr $posv + [winfo height $data(entry)] + \
          [winfo height $data(fbottom)]]
      if { $right > [winfo vrootwidth $self] } {
        #It is necessary to change the horizontal location
        set posh [expr [winfo rootx $data(entry)] + \
               [winfo width $data(entry)] - [winfo width $data(fbottom)] ]
      }    
      if { $bottom > [winfo vrootheight $self] } {
        #It is necessary to change the vertical location
        set posv [expr [winfo rooty $data(entry)] - \
                       [winfo height $data(fbottom)] ]
      }    
      set width [winfo width $data(fbottom)]
      set height [winfo height $data(fbottom)]

      # Apply geometry
      wm geometry $data(fbottom) ${width}x${height}+$posh+$posv
      bind $data(fbottom) <ButtonPress> [list $self DateClose %X %Y]
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method DateGetDay {day month year tags} {
  #
  # PURPOSE: Funcion a aplicar al seleccionar una fecha del selector. Al
  #          seleccionar una fecha se muestra en el entry  
  #
  #///////////////////////////////////////////////////////////////////////////
    $self configure -bdate [list $year $month $day]
    wm withdraw  $data(fbottom)
    grab release $data(fbottom)
    # Agregar un postcommand para el control de la fecha.
    if {[string length [$self cget -postcommand]]} {
      update
      eval [$self cget -postcommand]
      update
    }    
  }

  #///////////////////////////////////////////////////////////////////////////
  method DateClose {x y} {
  #
  # PURPOSE: Cerramos el selector de fechas haciendo click fuera del widget
  #
  #///////////////////////////////////////////////////////////////////////////
    if {[expr $x==-1 && $y==-1]}  {
      wm withdraw  $data(fbottom)
      grab release $data(fbottom)
    } else  {
      set obj [winfo containing $x $y]
      if {[string length $obj]} {
        if {[winfo toplevel $obj] != $data(fbottom)} {
          wm withdraw  $data(fbottom)
          grab release $data(fbottom)
        } 
      }
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method ClearDate {} {
  #
  # PURPOSE: Selector de fecha null
  #
  #///////////////////////////////////////////////////////////////////////////
    $self configure -bdate {}
    wm withdraw  $data(fbottom)
    grab release $data(fbottom)
    if {[string length [$self cget -postcommand]]} {
      update
      eval [$self cget -postcommand]
      update
    }    
  }


  #/////////////////////////////////////////////////////////////////////////////
  method Set { year month day} {
  #/////////////////////////////////////////////////////////////////////////////
  puts "$self configure -bdate [list $year $month $day]"
    $self configure -bdate [list $year $month $day]
    set data(month) $month
    set data(year) $year
  }

  # delegation of methods and options to entry
  delegate method * to wentry
  delegate option * to wentry
}

package provide bentrydate 1.0
