#/////////////////////////////////////////////////////////////////////////////
# FILE    : graph.tcl
# PURPOSE : This file is part of BLT Toolkit
#/////////////////////////////////////////////////////////////////////////////

proc Blt_ActiveLegend { graph } {
    $graph legend bind all <Enter> [list blt::ActivateLegend $graph ]
    $graph legend bind all <Leave> [list blt::DeactivateLegend $graph]
    $graph legend bind all <ButtonPress-1> [list blt::HighlightLegend $graph]
}

proc Blt_Crosshairs { graph } {
    blt::Crosshairs $graph 
}

proc Blt_ZoomStack { graph } {
    blt::ZoomStack $graph
}

# Cambia el ViewMode
# modos posibles:
#   zoom-frame
#   zoom-in
#   zoom-out
#   scroll

proc Blt_ViewMode { graph mode } {
  global zoomInfo

  if { ![blt::ClearMode $graph $mode] } return

  set zoomInfo($graph,mode) $mode

  if { [string equal $mode "scroll"] } {
    # limpio el stack
    blt::InitStack $graph $mode
    bind scroll-$graph <ButtonPress-1> { blt::SetScrollPoint %W %x %y }
    bind scroll-$graph <ButtonRelease-1> { blt::SetScrollPoint %W %x %y }
    # este evento posiblemente ya no hara falta
    bind scroll-$graph <ButtonPress-3> { 
      if { [%W inside %x %y] } { 
        blt::ResetScroll %W 
      }
    }
    blt::AddBindTag $graph scroll-$graph
  } elseif { [regexp "zoom-*" $mode] } {
    puts "mode = $mode"
    bind zoom-$graph <ButtonPress-1> { blt::SetZoomPoint %W %x %y }
    if { [string equal $mode zoom-frame] } { 
      bind zoom-$graph <ButtonRelease-1> { blt::SetZoomPoint %W %x %y }
    } else {
      bind zoom-$graph <ButtonRelease-1> ""
    }
    bind zoom-$graph <ButtonPress-3> { 
      if { [%W inside %x %y] } { 
        blt::ResetZoom %W 
      }
    }
    blt::AddBindTag $graph zoom-$graph
  }
}

proc blt::SetZoomScale { graph scale } {
  global zoomInfo

  set zoomInfo($graph,scale) $scale
}

proc blt::ClearMode { graph newmode } {
  global zoomInfo

  if { [info exists zoomInfo($graph,mode)] } {
    if { [string equal $newmode $zoomInfo($graph,mode)] } {
      return 0
    }
    set zoomInfo($graph,corner) A
    if { [string equal $zoomInfo($graph,mode) "scroll"] } {
      eval $graph marker delete [$graph marker names "scroll*"]
      blt::RemoveBindTag $graph "scroll-region-$graph" "scroll-$graph"
    } else {
      eval $graph marker delete [$graph marker names "zoom*"]
      blt::RemoveBindTag $graph "select-region-$graph" "zoom-$graph"
    }
  } else {
    blt::InitStack $graph ""
  }
  return 1
}

proc Blt_PrintKey { graph } {
    blt::PrintKey $graph
}

proc Blt_ClosestPoint { graph } {
    blt::ClosestPoint $graph
}

#
# The following procedures that reside in the "blt" namespace are
# supposed to be private.
#

proc blt::ActivateLegend { graph } {
    set elem [$graph legend get current]
    $graph legend activate $elem
}
proc blt::DeactivateLegend { graph } {
    set elem [$graph legend get current]
    $graph legend deactivate $elem
}

proc blt::HighlightLegend { graph } {
    set elem [$graph legend get current]
    set relief [$graph element cget $elem -labelrelief]
    if { $relief == "flat" } {
	$graph element configure $elem -labelrelief raised
	$graph element activate $elem
    } else {
	$graph element configure $elem -labelrelief flat
	$graph element deactivate $elem
    }
}

proc blt::Crosshairs { graph { event "Any-Motion" } } {
    $graph crosshairs on
    bind crosshairs-$graph <$event>   {
	%W crosshairs configure -position @%x,%y 
    }
    bind crosshairs-$graph <Leave>   {
	%W crosshairs off
    }
    bind crosshairs-$graph <Enter>   {
	%W crosshairs on
    }
    $graph crosshairs configure -color red
    blt::AddBindTag $graph crosshairs-$graph
}

proc blt::ClearCrosshairs { graph } {
  blt::RemoveBindTag $graph crosshairs-$graph
  $graph crosshairs off
}

proc blt::InitStack { graph {mode zoom-frame} } {
    global zoomInfo
    set zoomInfo($graph,interval) 100
    set zoomInfo($graph,afterId) 0
  set zoomInfo($graph,afterId1) 0
    set zoomInfo($graph,A,x) {}
    set zoomInfo($graph,A,y) {}
    set zoomInfo($graph,B,x) {}
    set zoomInfo($graph,B,y) {}
    set zoomInfo($graph,stack) {}
    set zoomInfo($graph,corner) A
  
  # this is to allow zoom in, out scroll
  
  set zoomInfo($graph,mode) $mode
}

proc blt::DeleteStack { graph } {
  global zoomInfo

  foreach it [array names zoomInfo $graph,*] {
    unset zoomInfo($it)
  }
}

proc blt::ZoomStack { graph {start "ButtonPress-1"} {reset "ButtonPress-3"} } {
  global zoomInfo zoomMod
    
  blt::InitStack $graph
    
  if { [info exists zoomMod] } {
    set modifier $zoomMod
  } else {
    set modifier ""
  }
  bind zoom-$graph <${modifier}${start}> { blt::SetZoomPoint %W %x %y }
  bind zoom-$graph <${modifier}${reset}> { 
    if { [%W inside %x %y] } { 
      blt::ResetZoom %W 
    }
  }
  blt::AddBindTag $graph zoom-$graph
}

proc blt::PrintKey { graph {event "Shift-ButtonRelease-3"} } {
    bind print-$graph <$event>  { Blt_PostScriptDialog %W }
    blt::AddBindTag $graph print-$graph
}

proc blt::ClosestPoint { graph {event "Control-ButtonPress-2"} } {
    bind closest-point-$graph <$event>  {
	blt::FindElement %W %x %y
    }
    blt::AddBindTag $graph closest-point-$graph
}

proc blt::AddBindTag { widget tag } {
    set oldTagList [bindtags $widget]
    if { [lsearch $oldTagList $tag] < 0 } {
	bindtags $widget [linsert $oldTagList 0  $tag]
    }
}

proc blt::RemoveBindTag { widget args } {
    set oldTagList [bindtags $widget]
  foreach tag $args {
    set index [lsearch $oldTagList $tag]
    if { $index >= 0 } {
	bindtags $widget [lreplace $oldTagList $index $index]
    }
  }
}

proc blt::FindElement { graph x y } {
    if ![$graph element closest $x $y info -interpolate yes] {
	beep
	return
    }
    # --------------------------------------------------------------
    # find(name)		- element Id
    # find(index)		- index of closest point
    # find(x) find(y)		- coordinates of closest point
    #				  or closest point on line segment.
    # find(dist)		- distance from sample coordinate
    # --------------------------------------------------------------
    set markerName "bltClosest_$info(name)"
    catch { $graph marker delete $markerName }
    $graph marker create text -coords { $info(x) $info(y) } \
	-name $markerName \
	-text "$info(name): $info(dist)\nindex $info(index)" \
	-font *lucida*-r-*-10-* \
	-anchor center -justify left \
	-yoffset 0 -bg {} 

    set coords [$graph invtransform $x $y]
    set nx [lindex $coords 0]
    set ny [lindex $coords 1]

    $graph marker create line -coords "$nx $ny $info(x) $info(y)" \
	-name line.$markerName 

    blt::FlashPoint $graph $info(name) $info(index) 10
    blt::FlashPoint $graph $info(name) [expr $info(index) + 1] 10
}

proc blt::FlashPoint { graph name index count } {
    if { $count & 1 } {
        $graph element deactivate $name 
    } else {
        $graph element activate $name $index
    }
    incr count -1
    if { $count > 0 } {
	after 200 blt::FlashPoint $graph $name $index $count
	update
    } else {
	eval $graph marker delete [$graph marker names "bltClosest_*"]
    }
}

proc blt::GetCoords { graph x y index } {
    global zoomInfo
    if { [$graph cget -invertxy] } {
	set zoomInfo($graph,$index,x) $y
	set zoomInfo($graph,$index,y) $x
    } else {
	set zoomInfo($graph,$index,x) $x
	set zoomInfo($graph,$index,y) $y
    }
}

proc blt::MarkPoint { graph index } {
    global zoomInfo
    set x [$graph xaxis invtransform $zoomInfo($graph,$index,x)]
    set y [$graph yaxis invtransform $zoomInfo($graph,$index,y)]
    set marker "zoomText_$index"
    set text [format "x=%.4g\ny=%.4g" $x $y] 

    if [$graph marker exists $marker] {
     	$graph marker configure $marker -coords { $x $y } -text $text 
    } else {
    	$graph marker create text -coords { $x $y } -name $marker \
   	    -font *lucida*-r-*-10-* \
	    -text $text -anchor center -bg {} -justify left
    }
}

proc blt::DestroyZoomTitle { graph } {
    global zoomInfo

    if { $zoomInfo($graph,corner) == "A" } {
	catch { $graph marker delete "zoomTitle" }
    }
}

proc blt::PopZoom { graph } {
  global zoomInfo

  set zoomStack $zoomInfo($graph,stack)
  if { [llength $zoomStack] > 0 } {
    set cmd [lindex $zoomStack 0]
    set zoomInfo($graph,stack) [lrange $zoomStack 1 end]
    eval $cmd
    blt::ZoomTitleLast $graph
    busy hold $graph
  set this [::bayesGraph::getInstance $graph]
  upvar \#0 ${this}::data data
  set gr $data(gr,active)
  #Tolcon_Trace "Aplicando Axis Ticks PopZoom"
  ::bayesGraph::GrapOptApplyAxisTicks    $this ${this}::options gr,$gr
    update
    set id [after 2000 "blt::DestroyZoomTitle $graph"]
    set zoomInfo($graph,afterId1) $id
    busy release $graph
  } else {
    catch { $graph marker delete "zoomTitle" }
  }
}

# Push the old axis limits on the stack and set the new ones

proc blt::PushZoom { graph } {
  global zoomInfo
  eval $graph marker delete [$graph marker names "zoom*"]
  if { [info exists zoomInfo($graph,afterId)] } {
    after cancel $zoomInfo($graph,afterId)
  }
  set x1 $zoomInfo($graph,A,x)
  set y1 $zoomInfo($graph,A,y)
  set x2 $zoomInfo($graph,B,x)
  set y2 $zoomInfo($graph,B,y)

  if { ($x1 == $x2) || ($y1 == $y2) } { 
    # No delta, revert to start
    return
  }
  set cmd {}
  foreach margin { xaxis yaxis x2axis y2axis } {
    foreach axis [$graph $margin use] {
      set min [$graph axis cget $axis -min] 
      set max [$graph axis cget $axis -max]
      set c [list $graph axis configure $axis -min $min -max $max]
      append cmd "$c\n"
    }
  }
  set zoomInfo($graph,stack) [linsert $zoomInfo($graph,stack) 0 $cmd]

  busy hold $graph 
  # This update lets the busy cursor take effect.
  update

  foreach margin { xaxis x2axis } {
    foreach axis [$graph $margin use] {
      set min [$graph axis invtransform $axis $x1]
      set max [$graph axis invtransform $axis $x2]
      if { $min > $max } {
        set tmp $min
        set min $max
        set max $tmp
      }
      # this code is to allow zoom-out operations
      if { [string equal $zoomInfo($graph,mode) "zoom-out"] } {
        set limits [$graph axis limits $axis]
        set lower [lindex $limits 0]
        set upper [lindex $limits 1]
        set delta [expr ($upper - $lower)/2.0]
        set min [expr $min - $delta]
        set max [expr $max + $delta]
      }
      # end zoom-out path
	  
#      if { $min > $max } { 
# 		$graph axis configure $axis -min $max -max $min
# 	    } else {
# 		$graph axis configure $axis -min $min -max $max
# 	    }
      $graph axis configure $axis -min $min -max $max
    }
  }
  foreach margin { yaxis y2axis } {
    foreach axis [$graph $margin use] {
      set min [$graph axis invtransform $axis $y1]
      set max [$graph axis invtransform $axis $y2]

      if { $min > $max } {
        set tmp $min
        set min $max
        set max $tmp
      }

      # this code is to allow zoom-out operations
      if { [string equal $zoomInfo($graph,mode) "zoom-out"] } {
        set limits [$graph axis limits $axis]
        set lower [lindex $limits 0]
        set upper [lindex $limits 1]
        set delta [expr ($upper - $lower)/2.0]
        set min [expr $min - $delta]
        set max [expr $max + $delta]
      }
      #end zoom-out patch

# 	    if { $min > $max } { 
# 		$graph axis configure $axis -min $max -max $min
# 	    } else {
# 		$graph axis configure $axis -min $min -max $max
# 	    }
      $graph axis configure $axis -min $min -max $max
	}
  }
  # This "update" forces the graph to be redrawn
  set this [::bayesGraph::getInstance $graph]
  upvar \#0 ${this}::data data
  set gr $data(gr,active)
  #Tolcon_Trace "Aplicando Axis Ticks PushZoom"
  ::bayesGraph::GrapOptApplyAxisTicks    $this ${this}::options gr,$gr
  update
    
  busy release $graph
}

#
# This routine terminates either an existing zoom, or pops back to
# the previous zoom level (if no zoom is in progress).
#

proc blt::ResetZoom { graph } {
  global zoomInfo 

  if { ![info exists zoomInfo($graph,corner)] } {
    blt::InitStack $graph 
  }
  eval $graph marker delete [$graph marker names "zoom*"]

  if { $zoomInfo($graph,corner) == "A" } {
    # Reset the whole axis
    blt::PopZoom $graph
  } else {
    global zoomMod
    if { [info exists zoomMod] } {
      set modifier $zoomMod
    } else {
      set modifier "Any-"
    }
    set zoomInfo($graph,corner) A
    blt::RemoveBindTag $graph select-region-$graph
  }
}

proc blt::ResetScroll { graph } {
  global zoomInfo

  eval $graph marker delete [$graph marker names "scroll*"]
  blt::RemoveBindTag $graph scroll-region-$graph
  set zoomInfo($graph,corner) A  
}

option add *zoomTitle.font	  -*-helvetica-medium-R-*-*-18-*-*-*-*-*-*-* 
option add *zoomTitle.shadow	  yellow4
option add *zoomTitle.foreground  yellow1
option add *zoomTitle.coords	  "-Inf Inf"

option add *scrollTitle.font	  -*-helvetica-medium-R-*-*-18-*-*-*-*-*-*-* 
option add *scrollTitle.shadow	  yellow4
option add *scrollTitle.foreground  yellow1
option add *scrollTitle.coords	  "-Inf Inf"

proc blt::ZoomTitleNext { graph } {
    global zoomInfo
    set level [expr [llength $zoomInfo($graph,stack)] + 1]
    if { [$graph cget -invertxy] } {
	set coords "-Inf -Inf"
    } else {
	set coords "-Inf Inf"
    }
    $graph marker create text -name "zoomTitle" -text "Zoom #$level" \
	-coords $coords -bindtags "" -anchor nw
}

proc blt::ScrollTitle { graph } {
    if { [$graph cget -invertxy] } {
	set coords "-Inf -Inf"
    } else {
	set coords "-Inf Inf"
    }
    $graph marker create text -name "scrollTitle" -text "scrolling ..." \
	-coords $coords -bindtags "" -anchor nw
}

proc blt::ZoomTitleLast { graph } {
    global zoomInfo

    set level [llength $zoomInfo($graph,stack)]
    if { $level > 0 } {
     	$graph marker create text -name "zoomTitle" -anchor nw \
	    -text "Zoom #$level" 
    }
}

proc blt::ZoomScale { graph } {
  global zoomInfo

  set cmd {}
  foreach axis {x y} {
    set limits [$graph axis limits $axis]
    set min [lindex $limits 0]
    set max [lindex $limits 1]
    set delta$axis [expr 0.5 * ($max - $min)]
    set c [list $graph axis configure $axis -min $min -max $max]
    append cmd "$c\n"
  }
  set zoomInfo($graph,stack) [linsert $zoomInfo($graph,stack) 0 $cmd]
  busy hold $graph 
  # This update lets the busy cursor take effect.
  update

    Tolcon_Trace "En ZoomScale zoomInfo($graph,scale) vale  $zoomInfo($graph,scale)"

  if { [string equal $zoomInfo($graph,mode) "zoom-in"] } {
    set deltax [expr $deltax * $zoomInfo($graph,scale)]
    set deltay [expr $deltay * $zoomInfo($graph,scale)]
  } else {
    set deltax [expr $deltax / $zoomInfo($graph,scale)]
    set deltay [expr $deltay / $zoomInfo($graph,scale)]
  }

  foreach {axis delta} [list x $deltax y $deltay] {
    set v [$graph axis invtransform $axis $zoomInfo($graph,A,$axis)]
    set min [expr $v - $delta]
    set max [expr $v + $delta]
    $graph axis configure $axis -min $min -max $max
  }
  # This "update" forces the graph to be redrawn
  set this [::bayesGraph::getInstance $graph]
  upvar \#0 ${this}::data data
  set gr $data(gr,active)
  #Tolcon_Trace "Aplicando Axis Ticks ZoomScale"
  ::bayesGraph::GrapOptApplyAxisTicks    $this ${this}::options gr,$gr
  update
  
  busy release $graph
}

proc blt::SetZoomPoint { graph x y } {
  global zoomInfo zoomMod
  if { ![info exists zoomInfo($graph,corner)] } {
    blt::InitStack $graph
  }
  blt::GetCoords $graph $x $y $zoomInfo($graph,corner)

  #puts  blt::SetZoomPoint

  if { [string compare $zoomInfo($graph,mode) "zoom-frame"] } {
    blt::ZoomScale $graph
    return
  }

  if { [info exists zoomMod] } {
    set modifier $zoomMod
  } else {
    set modifier "Any-"
  }
  bind select-region-$graph <${modifier}Motion> { 
    blt::GetCoords %W %x %y B
    #blt::MarkPoint $graph B
    blt::Box %W
  }
  if { $zoomInfo($graph,corner) == "A" } {
	if { ![$graph inside $x $y] } {
	    return
	}
	# First corner selected, start watching motion events

	#blt::MarkPoint $graph A
	blt::ZoomTitleNext $graph 

	blt::AddBindTag $graph select-region-$graph
	set zoomInfo($graph,corner) B
  } else {
	# Delete the modal binding
	blt::RemoveBindTag $graph select-region-$graph
	blt::PushZoom $graph 
	set zoomInfo($graph,corner) A
  }
}

proc blt::SetScrollPoint { graph x y } {
    global zoomInfo

    blt::GetCoords $graph $x $y A
    bind scroll-region-$graph <Any-Motion> { 
	blt::GetCoords %W %x %y B
	#blt::MarkPoint $graph B
	blt::Scroll %W
    }
    if { $zoomInfo($graph,corner) == "A" } {
	if { ![$graph inside $x $y] } {
	    return
	}
	# First corner selected, start watching motion events

	#blt::MarkPoint $graph A
	blt::ScrollTitle $graph 

	blt::AddBindTag $graph scroll-region-$graph
	set zoomInfo($graph,corner) B
    } else {
	# Delete the modal binding
      ResetScroll $graph
    }
}

option add *zoomOutline.dashes		4	
option add *zoomTitle.anchor		nw
option add *zoomOutline.lineWidth	2
option add *zoomOutline.xor		yes

proc blt::MarchingAnts { graph offset } {
    global zoomInfo

    incr offset
    if { [$graph marker exists zoomOutline] } {
	$graph marker configure zoomOutline -dashoffset $offset 
	set interval $zoomInfo($graph,interval)
	set id [after $interval [list blt::MarchingAnts $graph $offset]]
	set zoomInfo($graph,afterId) $id
    }
}

proc blt::Box { graph } {
    global zoomInfo

    if { $zoomInfo($graph,A,x) > $zoomInfo($graph,B,x) } { 
	set x1 [$graph xaxis invtransform $zoomInfo($graph,B,x)]
	set y1 [$graph yaxis invtransform $zoomInfo($graph,B,y)]
	set x2 [$graph xaxis invtransform $zoomInfo($graph,A,x)]
	set y2 [$graph yaxis invtransform $zoomInfo($graph,A,y)]
    } else {
	set x1 [$graph xaxis invtransform $zoomInfo($graph,A,x)]
	set y1 [$graph yaxis invtransform $zoomInfo($graph,A,y)]
	set x2 [$graph xaxis invtransform $zoomInfo($graph,B,x)]
	set y2 [$graph yaxis invtransform $zoomInfo($graph,B,y)]
    }
    set coords { $x1 $y1 $x2 $y1 $x2 $y2 $x1 $y2 $x1 $y1 }
    if { [$graph marker exists "zoomOutline"] } {
	$graph marker configure "zoomOutline" -coords $coords 
    } else {
	set X [lindex [$graph xaxis use] 0]
	set Y [lindex [$graph yaxis use] 0]
	$graph marker create line -coords $coords -name "zoomOutline" \
	    -mapx $X -mapy $Y
	set interval $zoomInfo($graph,interval)
	set id [after $interval [list blt::MarchingAnts $graph 0]]
	set zoomInfo($graph,afterId) $id
    }
}

proc blt::Scroll { graph } {
  global zoomInfo

  set x1 [$graph xaxis invtransform $zoomInfo($graph,A,x)]
  set x2 [$graph xaxis invtransform $zoomInfo($graph,B,x)]
  set y1 [$graph yaxis invtransform $zoomInfo($graph,A,y)]
  set y2 [$graph yaxis invtransform $zoomInfo($graph,B,y)]
  #set deltax [expr $x2 - $x1]
  #set deltay [expr $y2 - $y1]
  # Cambiado para efecto 'agarrar el gráfico'
  set deltax [expr $x1 - $x2]
  set deltay [expr $y1 - $y2]
  foreach {delta axis} [list $deltax x $deltay y] {
    set limits [$graph axis limits $axis]
    set lower [expr [lindex $limits 0] + $delta]
    set upper [expr [lindex $limits 1] + $delta]
    $graph axis configure $axis -min $lower -max $upper
  }
  set zoomInfo($graph,A,x) $zoomInfo($graph,B,x)
  set zoomInfo($graph,A,y) $zoomInfo($graph,B,y)
}

proc Blt_PostScriptDialog { graph } {
    set top $graph.top
    toplevel $top

    foreach var { center landscape maxpect preview decorations padx 
	pady paperwidth paperheight width height colormode } {
	global $graph.$var
	set $graph.$var [$graph postscript cget -$var]
    }
    set row 1
    set col 0
    label $top.title -text "PostScript Options"
    table $top $top.title -cspan 7
    foreach bool { center landscape maxpect preview decorations } {
	set w $top.$bool-label
	label $w -text "-$bool" -font *courier*-r-*12* 
	table $top $row,$col $w -anchor e -pady { 2 0 } -padx { 0 4 }
	set w $top.$bool-yes
	global $graph.$bool
	radiobutton $w -text "yes" -variable $graph.$bool -value 1
	table $top $row,$col+1 $w -anchor w
	set w $top.$bool-no
	radiobutton $w -text "no" -variable $graph.$bool -value 0
	table $top $row,$col+2 $w -anchor w
	incr row
    }
    label $top.modes -text "-colormode" -font *courier*-r-*12* 
    table $top $row,0 $top.modes -anchor e  -pady { 2 0 } -padx { 0 4 }
    set col 1
    foreach m { color greyscale } {
	set w $top.$m
	radiobutton $w -text $m -variable $graph.colormode -value $m
	table $top $row,$col $w -anchor w
	incr col
    }
    set row 1
    frame $top.sep -width 2 -bd 1 -relief sunken
    table $top $row,3 $top.sep -fill y -rspan 6
    set col 4
    foreach value { padx pady paperwidth paperheight width height } {
	set w $top.$value-label
	label $w -text "-$value" -font *courier*-r-*12* 
	table $top $row,$col $w -anchor e  -pady { 2 0 } -padx { 0 4 }
	set w $top.$value-entry
	global $graph.$value
	entry $w -textvariable $graph.$value -width 8
	table $top $row,$col+1 $w -cspan 2 -anchor w -padx 8
	incr row
    }
    table configure $top c3 -width .125i
    button $top.cancel -text "Cancel" -command "destroy $top"
    table $top $row,0 $top.cancel  -width 1i -pady 2 -cspan 3
    button $top.reset -text "Reset" -command "destroy $top"
    #table $top $row,1 $top.reset  -width 1i
    button $top.print -text "Print" -command "blt::ResetPostScript $graph"
    table $top $row,4 $top.print  -width 1i -pady 2 -cspan 2
}

proc blt::ResetPostScript { graph } {
    foreach var { center landscape maxpect preview decorations padx 
	pady paperwidth paperheight width height colormode } {
	global $graph.$var
	set old [$graph postscript cget -$var]
	if { [catch {$graph postscript configure -$var [set $graph.$var]}] != 0 } {
	    $graph postscript configure -$var $old
	    set $graph.$var $old
	}
    }
    $graph postscript output "out.ps"
    puts stdout "wrote file \"out.ps\"."
    flush stdout
}
