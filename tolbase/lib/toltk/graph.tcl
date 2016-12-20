#/////////////////////////////////////////////////////////////////////////////
# FILE    : graph.tcl
# PURPOSE : This file is part of RBC Toolkit
#/////////////////////////////////////////////////////////////////////////////

# force rbc load

if { [info command "_rbc_InitStack"] eq "" } {
  rbc::graph .__g__
  Rbc_ZoomStack .__g__
  destroy .__g__
  rename rbc::InitStack _rbc_InitStack
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

if { [ catch { rbc::busy patched } ] } {
  rename ::rbc::busy _rbc_busy
  interp alias {} ::rbc::busy {} Busy_Patched
  #@ puts "interp alias {} ::rbc::busy {} Busy_Patched"
}


# Cambia el ViewMode
# modos posibles:
#   zoom-frame
#   zoom-in
#   zoom-out
#   scroll

proc Rbc_ViewMode { graph mode } {
  global zoomInfo

  if { ![rbc::ClearMode $graph $mode] } return

  set zoomInfo($graph,mode) $mode

  if { [string equal $mode "scroll"] } {
    # limpio el stack
    rbc::InitStack $graph $mode
    bind scroll-$graph <ButtonPress-1> { rbc::SetScrollPoint %W %x %y }
    bind scroll-$graph <ButtonRelease-1> { rbc::SetScrollPoint %W %x %y }
    # este evento posiblemente ya no hara falta
    bind scroll-$graph <ButtonPress-3> {
      if { [%W inside %x %y] } {
        rbc::ResetScroll %W
      }
    }
    rbc::AddBindTag $graph scroll-$graph
  } elseif { [regexp "zoom-*" $mode] } {
    #@ puts "mode = $mode"
    bind zoom-$graph <ButtonPress-1> { rbc::SetZoomPoint %W %x %y }
    if { [string equal $mode zoom-frame] } {
      bind zoom-$graph <ButtonRelease-1> { rbc::SetZoomPoint %W %x %y }
    } else {
      bind zoom-$graph <ButtonRelease-1> ""
    }
    bind zoom-$graph <ButtonPress-3> {
      if { [%W inside %x %y] } {
        rbc::ResetZoom %W
      }
    }
    rbc::AddBindTag $graph zoom-$graph
  }
}

proc rbc::RemoveBindTag { widget args } {
  set oldTagList [bindtags $widget]
  foreach tag $args {
    set index [lsearch $oldTagList $tag]
    if { $index >= 0 } {
        bindtags $widget [lreplace $oldTagList $index $index]
    }
  }
}

proc rbc::ClearCrosshairs { graph } {
    rbc::RemoveBindTag $graph crosshairs-$graph
    $graph crosshairs off
}

proc rbc::InitStack { graph {mode zoom-frame} } {
    global zoomInfo

    _rbc_InitStack $graph
    # this is to allow zoom in, out scroll

    set zoomInfo($graph,mode) $mode
}

proc rbc::SetZoomScale { graph scale } {
  global zoomInfo

  set zoomInfo($graph,scale) $scale
}

proc rbc::ClearMode { graph newmode } {
  global zoomInfo

  if { [info exists zoomInfo($graph,mode)] } {
    if { [string equal $newmode $zoomInfo($graph,mode)] } {
      return 0
    }
    set zoomInfo($graph,corner) A
    if { [string equal $zoomInfo($graph,mode) "scroll"] } {
      eval $graph marker delete [$graph marker names "scroll*"]
      rbc::RemoveBindTag $graph "scroll-region-$graph" "scroll-$graph"
    } else {
      eval $graph marker delete [$graph marker names "zoom*"]
      rbc::RemoveBindTag $graph "select-region-$graph" "zoom-$graph"
    }
  } else {
    rbc::InitStack $graph ""
  }
  return 1
}

proc rbc::ZoomScale { graph } {
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
  #@! Redibuja el gráfico
  event generate $graph <Configure>
  # This "update" forces the graph to be redrawn
  set this [::bayesGraph::getInstance $graph]
  upvar \#0 ${this}::data data
  set gr $data(gr,active)
  #Tolcon_Trace "Aplicando Axis Ticks ZoomScale"
  ::bayesGraph::GrapOptApplyAxisTicks    $this ${this}::options gr,$gr
  update

  busy release $graph
}

proc rbc::SetZoomPoint { graph x y } {
  global zoomInfo zoomMod
  if { ![info exists zoomInfo($graph,corner)] } {
    rbc::InitStack $graph
  }
  rbc::GetCoords $graph $x $y $zoomInfo($graph,corner)

  #puts  rbc::SetZoomPoint

  if { [string compare $zoomInfo($graph,mode) "zoom-frame"] } {
    rbc::ZoomScale $graph
    return
  }

  if { [info exists zoomMod] } {
    set modifier $zoomMod
  } else {
    set modifier "Any-"
  }
  bind select-region-$graph <${modifier}Motion> {
    rbc::GetCoords %W %x %y B
    #rbc::MarkPoint $graph B
    rbc::Box %W
  }
  if { $zoomInfo($graph,corner) == "A" } {
    if { ![$graph inside $x $y] } {
      return
    }
    # First corner selected, start watching motion events

    #rbc::MarkPoint $graph A
    rbc::ZoomTitleNext $graph

    rbc::AddBindTag $graph select-region-$graph
    set zoomInfo($graph,corner) B
  } else {
    # Delete the modal binding
    rbc::RemoveBindTag $graph select-region-$graph
    rbc::PushZoom $graph
    set zoomInfo($graph,corner) A
  }
}

proc rbc::SetScrollPoint { graph x y } {
    global zoomInfo

    rbc::GetCoords $graph $x $y A
    bind scroll-region-$graph <Any-Motion> {
        rbc::GetCoords %W %x %y B
        #rbc::MarkPoint $graph B
        rbc::Scroll %W
    }
    if { $zoomInfo($graph,corner) == "A" } {
        if { ![$graph inside $x $y] } {
          return
        }
        # First corner selected, start watching motion events

        #rbc::MarkPoint $graph A
        rbc::ScrollTitle $graph

        rbc::AddBindTag $graph scroll-region-$graph
        set zoomInfo($graph,corner) B
    } else {
        # Delete the modal binding
        ResetScroll $graph
    }
}

proc rbc::Scroll { graph } {
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
  #@! Redibuja el gráfico
  event generate $graph <Configure>
  set zoomInfo($graph,A,x) $zoomInfo($graph,B,x)
  set zoomInfo($graph,A,y) $zoomInfo($graph,B,y)
}

proc rbc::ResetScroll { graph } {
  global zoomInfo

  eval $graph marker delete [$graph marker names "scroll*"]
  rbc::RemoveBindTag $graph scroll-region-$graph
  set zoomInfo($graph,corner) A
}

proc rbc::ScrollTitle { graph } {
    if { [$graph cget -invertxy] } {
        set coords "-Inf -Inf"
    } else {
        set coords "-Inf Inf"
    }
    $graph marker create text -name "scrollTitle" -text "scrolling ..." \
        -coords $coords -bindtags "" -anchor nw
}
