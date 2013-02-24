#/////////////////////////////////////////////////////////////////////////////
# FILE    : graph.tcl
# PURPOSE : This file is part of BLT Toolkit
#/////////////////////////////////////////////////////////////////////////////

# force blt load

if { [info command "_blt_InitStack"] eq "" } {
  blt::graph .__g__
  Blt_ZoomStack .__g__
  destroy .__g__
  rename blt::InitStack _blt_InitStack
}

proc Busy_Patched { cmd args } {
    puts "-- Busy $cmd $args --"
    switch $cmd {
      patched {
        return 1
      }
      default {
        
      }
    }
}

if { [ catch { blt::busy patched } ] } {
  rename ::blt::busy _blt_busy
  interp alias {} ::blt::busy {} Busy_Patched
  puts "interp alias {} ::blt::busy {} Busy_Patched"
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

proc blt::RemoveBindTag { widget args } {
  set oldTagList [bindtags $widget]
  foreach tag $args {
    set index [lsearch $oldTagList $tag]
    if { $index >= 0 } {
	bindtags $widget [lreplace $oldTagList $index $index]
    }
  }
}

proc blt::InitStack { graph {mode zoom-frame} } {
    global zoomInfo
  
    _blt_InitStack $graph
    # this is to allow zoom in, out scroll
  
    set zoomInfo($graph,mode) $mode
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

proc blt::ResetScroll { graph } {
  global zoomInfo

  eval $graph marker delete [$graph marker names "scroll*"]
  blt::RemoveBindTag $graph scroll-region-$graph
  set zoomInfo($graph,corner) A  
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
