#/////////////////////////////////////////////////////////////////////////////
# FILE    : bysgropt.tcl
# PURPOSE : This file is continuation of bysgraph.tcl. It contains procedures
#           relative to graph options.
#/////////////////////////////////////////////////////////////////////////////

package require blistbox
package require bcomboboxplus
package require blistorder
package require bcolor
package require bfont


#/////////////////////////////////////////////////////////////////////////////
proc ::bayesGraph::ReadIni {this {holdon 0}} {
# PURPOSE: Reads general graph options from .ini file. The ini file is the
#          active graphic configuration file.
#
# PARAMETERS:
#   this  -> Instance of BayesGraph
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::options opt

  variable optLinesAll
  
  #set rini ::iniFile::Read
  if {!$holdon} {
    set rini ::iniFile::ReadGCF
    set key $data(namespace)

    set opt(PhotoLastDir)    [$rini $key PhotoLastDir {}]
    set opt(grid) [$rini $key grid 1]
  }
  for  {set gr 0} {$gr<$data(gr,numGraph)} {incr gr} {
    set g $widgets(gr,$gr)

    if {!$holdon} {
      set opt(gr,$gr,yFormatNumber)   [$rini $key yFormatNumber  ""]
      set opt(gr,$gr,y2FormatNumber)  [$rini $key y2FormatNumber ""]
      set opt(gr,$gr,yFormatPercent)  [$rini $key yFormatPercent  0]
      set opt(gr,$gr,y2FormatPercent) [$rini $key y2FormatPercent 0]
      set opt(gr,$gr,xFormatNumber)   [$rini $key xFormatNumber  ""]
      set opt(gr,$gr,x2FormatNumber)  [$rini $key x2FormatNumber ""]
      set opt(gr,$gr,xFormatPercent)  [$rini $key xFormatPercent  0]
      set opt(gr,$gr,x2FormatPercent) [$rini $key x2FormatPercent 0]
      
      set opt(gr,$gr,yAxisAll)      [$rini $key yAxisAll  1]
      set opt(gr,$gr,xAxisAll)      [$rini $key xAxisAll  1]
      set opt(gr,$gr,x2AxisAll)     [$rini $key x2AxisAll 1]
      set opt(gr,$gr,y2AxisAll)     [$rini $key y2AxisAll 1]
      
      set opt(gr,$gr,xTicksWidth)   [$rini $key xTicksWidth 1]
      set opt(gr,$gr,yTicksWidth)   [$rini $key yTicksWidth 1]
      set opt(gr,$gr,xTicksShow)    [$rini $key xTicksShow  1] 
      set opt(gr,$gr,yTicksShow)    [$rini $key yTicksShow  1]
      set opt(gr,$gr,xTicksType)    [$rini $key xTicksType  auto]
      set opt(gr,$gr,yTicksType)    [$rini $key yTicksType  total]
      set opt(gr,$gr,x2TicksType)   [$rini $key x2TicksType auto]
      set opt(gr,$gr,y2TicksType)   [$rini $key y2TicksType total]
      
      set opt(gr,$gr,xTicksTotal)   [$rini $key xTicksTotal 10]
      set opt(gr,$gr,yTicksTotal)   [$rini $key yTicksTotal 10]
      set opt(gr,$gr,x2TicksTotal)  [$rini $key x2TicksTotal 10]
      set opt(gr,$gr,y2TicksTotal)  [$rini $key y2TicksTotal 10]
      
      set opt(gr,$gr,xTicksStep)    [$rini $key xTicksStep  0]
      set opt(gr,$gr,yTicksStep)    [$rini $key yTicksStep  0]
      set opt(gr,$gr,xTicksFont)    [$rini $key xTicksFont   {{Lucida Console} 8}]
      set opt(gr,$gr,yTicksFont)    [$rini $key yTicksFont   {{Lucida Console} 8}]
      set opt(gr,$gr,xTicksRotate)  [$rini $key xTicksRotate 90]
      set opt(gr,$gr,yTicksRotate)  [$rini $key yTicksRotate [$g axis cget y -rotate]]
      set opt(gr,$gr,xTicksColor)   [$rini $key xTicksColor  [$g axis cget x -color]]
      set opt(gr,$gr,yTicksColor)   [$rini $key yTicksColor  [$g axis cget y -color]]
      set opt(gr,$gr,xTicksShadow)  [$rini $key xTicksShadow [lindex [$g axis cget x -tickshadow] 0]]
      set opt(gr,$gr,yTicksShadow)  [$rini $key yTicksShadow [lindex [$g axis cget y -tickshadow] 0]]
      set opt(gr,$gr,xTicksLength)  [$rini $key xTicksLength 4]
      set opt(gr,$gr,yTicksLength)  [$rini $key yTicksLength 4]
      set opt(gr,$gr,xTicksSubdivisions) [$rini $key xTicksSubdivisions 1]
      set opt(gr,$gr,yTicksSubdivisions) [$rini $key yTicksSubdivisions 1]
      
      set opt(gr,$gr,xAxisShow)     [$rini $key xAxisShow  1]
      set opt(gr,$gr,yAxisShow)     [$rini $key yAxisShow  1]
      set opt(gr,$gr,x2AxisShow)    [$rini $key x2AxisShow 0]
      set opt(gr,$gr,y2AxisShow)    [$rini $key y2AxisShow 0]
      
      set opt(gr,$gr,xAxisMin)      [$rini $key xAxisMin   0]
      set opt(gr,$gr,x2AxisMin)     [$rini $key x2AxisMin  0]
      set opt(gr,$gr,yAxisMin)      [$rini $key yAxisMin   0]
      set opt(gr,$gr,y2AxisMin)     [$rini $key y2AxisMin  0]
      
      set opt(gr,$gr,xAxisMax)      [$rini $key xAxisMax   100]
      set opt(gr,$gr,yAxisMax)      [$rini $key yAxisMax   100]
      set opt(gr,$gr,x2AxisMax)     [$rini $key x2AxisMax  100]
      set opt(gr,$gr,y2AxisMax)     [$rini $key y2AxisMax  100]
      
      set opt(gr,$gr,x2TicksWidth)  [$rini $key x2TicksWidth 1]
      set opt(gr,$gr,y2TicksWidth)  [$rini $key y2TicksWidth 1]
      set opt(gr,$gr,x2TicksShow)   [$rini $key x2TicksShow 1] 
      set opt(gr,$gr,y2TicksShow)   [$rini $key y2TicksShow 1]
      set opt(gr,$gr,x2TicksStep)   [$rini $key x2TicksStep  10]
      set opt(gr,$gr,y2TicksStep)   [$rini $key y2TicksStep  10]
      set opt(gr,$gr,x2TicksFont)   [$rini $key x2TicksFont   {{Lucida Console} 8}]
      set opt(gr,$gr,y2TicksFont)   [$rini $key y2TicksFont   {{Lucida Console} 8}]
      set opt(gr,$gr,x2TicksRotate) [$rini $key x2TicksRotate [$g axis cget x2 -rotate]]
      set opt(gr,$gr,y2TicksRotate) [$rini $key y2TicksRotate [$g axis cget y2 -rotate]]
      set opt(gr,$gr,x2TicksColor)  [$rini $key x2TicksColor  [$g axis cget x2 -color]]
      set opt(gr,$gr,y2TicksColor)  [$rini $key y2TicksColor  [$g axis cget y2 -color]]
      set opt(gr,$gr,x2TicksShadow) [$rini $key x2TicksShadow [lindex [$g axis cget x2 -tickshadow] 0]]
      set opt(gr,$gr,y2TicksShadow) [$rini $key y2TicksShadow [lindex [$g axis cget y2 -tickshadow] 0]]
      set opt(gr,$gr,x2TicksLength) [$rini $key x2TicksLength 4]
      set opt(gr,$gr,y2TicksLength) [$rini $key y2TicksLength 4]
      set opt(gr,$gr,x2TicksSubdivisions) [$rini $key x2TicksSubdivisions 1]
      set opt(gr,$gr,y2TicksSubdivisions) [$rini $key y2TicksSubdivisions 1]
      
      set opt(gr,$gr,xAxisTitle)   [$rini $key xAxisTitle        ""]
      set opt(gr,$gr,xAxisColor)   [$rini $key xAxisTitleColor   [$g axis cget x -titlecolor]]
      set opt(gr,$gr,xAxisFont)    [$rini $key xAxisTitleFont    [$g axis cget x -titlefont]]
      set opt(gr,$gr,xAxisShadow)  [$rini $key xAxisTitleShadow  [lindex [$g axis cget x -titleshadow] 0]]
      set opt(gr,$gr,yAxisTitle)   [$rini $key yAxisTitle        ""]
      set opt(gr,$gr,yAxisColor)   [$rini $key yAxisTitleColor   [$g axis cget y -titlecolor]]
      set opt(gr,$gr,yAxisFont)    [$rini $key yAxisTitleFont    [$g axis cget y -titlefont]]
      set opt(gr,$gr,yAxisShadow)  [$rini $key yAxisTitleShadow  [lindex [$g axis cget y -titleshadow] 0]]
      set opt(gr,$gr,x2AxisTitle)  [$rini $key x2AxisTitle       ""]
      set opt(gr,$gr,x2AxisColor)  [$rini $key x2AxisTitleColor  [$g axis cget x2 -titlecolor]]
      set opt(gr,$gr,x2AxisFont)   [$rini $key x2AxisTitleFont   [$g axis cget x2 -titlefont]]
      set opt(gr,$gr,x2AxisShadow) [$rini $key x2AxisTitleShadow [lindex [$g axis cget x2 -titleshadow] 0]]
      set opt(gr,$gr,y2AxisTitle)  [$rini $key y2AxisTitle       ""]
      set opt(gr,$gr,y2AxisColor)  [$rini $key y2AxisTitleColor  [$g axis cget y2 -titlecolor]]
      set opt(gr,$gr,y2AxisFont)   [$rini $key y2AxisTitleFont   [$g axis cget y2 -titlefont]]
      set opt(gr,$gr,y2AxisShadow) [$rini $key y2AxisTitleShadow [lindex [$g axis cget y2 -titleshadow] 0]]
      
      set opt(gr,$gr,font)         [$rini $key font [$g cget -font]]
      set opt(gr,$gr,foreground)   [$rini $key foreground [$g cget -foreground]]
      set opt(gr,$gr,shadow)       [$rini $key shadow [lindex [$g cget -shadow] 0]]
      
      # options line
      set opt(gr,$gr,barmode)       [$rini $key barmode aligned]    
      
      set opt(gr,$gr,linehides)     [$rini $key linehides false]
      set opt(gr,$gr,linetypes)     [$rini $key linetypes LineElement]    
      set opt(gr,$gr,barwidth)      [$rini $key barwidth 0.6]
      set opt(gr,$gr,linewidth)     [$rini $key linewidth 2]
      set opt(gr,$gr,dashes)        [$rini $key dashes {}]
      set opt(gr,$gr,smooth)        [$rini $key smooth linear]
      set opt(gr,$gr,barrelief)     [$rini $key barrelief raised]    
      set opt(gr,$gr,borderwidth)   [$rini $key borderwidth 0]
      set opt(gr,$gr,colors)        [$rini $key colors {#00c600 blue red yellow orange #d14949 grey purple pink navyblue #356a6a #804000 #05eefa #009148 #ffffbb #29fb04}]
      
      set opt(gr,$gr,xyMappedAxis)     [$rini $key xyMappedAxis {{x y}}]
      set opt(gr,$gr,areaTypes)        [$rini $key areaTypes none]
      set opt(gr,$gr,areaTypesBar)     [$rini $key areaTypesBar none]    
      set opt(gr,$gr,areaTiles)        [$rini $key areaTiles {}]
      set opt(gr,$gr,areaForegrounds)  [$rini $key areaForegrounds defcolor]
      set opt(gr,$gr,areaBackgrounds)  [$rini $key areaBackgrounds white]
      set opt(gr,$gr,stippleimages)    [$rini $key stippleimages {}]
      set opt(gr,$gr,stippledegradeds) [$rini $key stippledegradeds 50]
      
      set opt(gr,$gr,showvalues)      [$rini $key showvalues   none]
      set opt(gr,$gr,valuefont)       [$rini $key valuefont   {Arial 8}]
      set opt(gr,$gr,valuerotate)     [$rini $key valuerotate 0]
      set opt(gr,$gr,valuecolor)      [$rini $key valuecolor  black]
      set opt(gr,$gr,valueformatdec)  [$rini $key valueformatdec 2]
      set opt(gr,$gr,valueshadow)     [$rini $key valueshadow    {}]
      set opt(gr,$gr,valueanchor)     [$rini $key valueanchor  center]
      set opt(gr,$gr,showsymbols)     [$rini $key showsymbols  0]
      set opt(gr,$gr,symbol)          [$rini $key symbol  triangle]
      set opt(gr,$gr,pixels)          [$rini $key pixels 4]
      set opt(gr,$gr,scalesymbols)    [$rini $key scalesymbols false]
      set opt(gr,$gr,fill)            [$rini $key fill  defcolor]
      set opt(gr,$gr,outline)         [$rini $key outline   defcolor]
      set opt(gr,$gr,outlinewidth)    [$rini $key outlinewidth 1]
      
      set opt(gr,$gr,selffit)         [$rini $key selffit 0]
      
      set opt(gr,$gr,xMarkers)        [$rini $key xMarkers {}]
      set opt(gr,$gr,yMarkers)        [$rini $key yMarkers {}]
      set opt(gr,$gr,xMarkersIn)      [$rini $key xMarkersIn {}]
      set opt(gr,$gr,yMarkersIn)      [$rini $key yMarkersIn {}]
      
      set opt(gr,$gr,x2Markers)       [$rini $key x2Markers {}]
      set opt(gr,$gr,y2Markers)       [$rini $key y2Markers {}]
      set opt(gr,$gr,x2MarkersIn)     [$rini $key x2MarkersIn {}]
      set opt(gr,$gr,y2MarkersIn)     [$rini $key y2MarkersIn {}]
      
      set opt(gr,$gr,xMarkersColorsInEach)   [$rini $key xMarkersColorsInEach  \
                                                  {#356a6a #804000 #05eefa #009148 #ffffbb}]
      set opt(gr,$gr,x2MarkersColorsInEach)  [$rini $key x2MarkersColorsInEach \
                                                  {#356a6a #804000 #05eefa #009148 #ffffbb}]
      set opt(gr,$gr,yMarkersColorsInEach)   [$rini $key yMarkersColorsInEach  \
                                                  {#356a6a #804000 #05eefa #009148 #ffffbb}]
      set opt(gr,$gr,y2MarkersColorsInEach)  [$rini $key y2MarkersColorsInEach \
                                                  {#356a6a #804000 #05eefa #009148 #ffffbb}]

      set opt(gr,$gr,xMarkersDashesInEach)  [$rini $key xMarkersDashesInEach  \
                                                 {{1 1} {2 2} {3 3} {4 4} {5 5}}]
      set opt(gr,$gr,x2MarkersDashesInEach) [$rini $key x2MarkersDashesInEach \
                                                 {{1 1} {2 2} {3 3} {4 4} {5 5}}]
      set opt(gr,$gr,yMarkersDashesInEach)  [$rini $key yMarkersDashesInEach  \
                                                 {{1 1} {2 2} {3 3} {4 4} {5 5}}]
      set opt(gr,$gr,y2MarkersDashesInEach) [$rini $key y2MarkersDashesInEach \
                                                 {{1 1} {2 2} {3 3} {4 4} {5 5}}]
      
      set opt(gr,$gr,xMarkersWidthsInEach)   [$rini $key xMarkersWidthsInEach  \
                                                  {1 1 1 1 1}]
      set opt(gr,$gr,x2MarkersWidthsInEach)  [$rini $key x2MarkersWidthsInEach \
                                                  {1 1 1 1 1}]
      set opt(gr,$gr,yMarkersWidthsInEach)   [$rini $key yMarkersWidthsInEach  \
                                                  {1 1 1 1 1}]
      set opt(gr,$gr,y2MarkersWidthsInEach)  [$rini $key y2MarkersWidthsInEach \
                                                  {1 1 1 1 1}]
      
      set opt(gr,$gr,xMarkersUndersInEach)   [$rini $key xMarkersUndersInEach  \
                                                  {0 0 0 0 0}]
      set opt(gr,$gr,x2MarkersUndersInEach)  [$rini $key x2MarkersUndersInEach \
                                                  {0 0 0 0 0}]
      set opt(gr,$gr,yMarkersUndersInEach)   [$rini $key yMarkersUndersInEach  \
                                                  {0 0 0 0 0}]
      set opt(gr,$gr,y2MarkersUndersInEach)  [$rini $key y2MarkersUndersInEach \
                                                  {0 0 0 0 0}]
      
      set opt(gr,$gr,legend,Hide)     [$rini $key legendHide      [$g legend cget -hide]]
      set opt(gr,$gr,legend,Bg)       [$rini $key legendBg        ""]
      set opt(gr,$gr,legend,Bd)       [$rini $key legendBd        [$g legend cget -bd]]
      set opt(gr,$gr,legend,Fg)       [$rini $key legendFg        [$g legend cget -fg]]
      set opt(gr,$gr,legend,Font)     [$rini $key legendFont      [$g legend cget -font]]
      set opt(gr,$gr,legend,Shadow)   [$rini $key legendShadow    [$g legend cget -shadow]]
      set opt(gr,$gr,legend,Position) [$rini $key legendPosition  "bottom"]
      set opt(gr,$gr,legend,Anchor)   [$rini $key legendAnchor    [$g legend cget -anchor]]
      set opt(gr,$gr,legend,Relief)   [$rini $key legendRelief    [$g legend cget -relief]]
      set opt(gr,$gr,legend,Raised)   [$rini $key legendRaised    [$g legend cget -raised]]
      set opt(gr,$gr,legend,IPadXl)   [$rini $key legendIPadXl    [lindex [$g legend cget -ipadx] 0]]
      set opt(gr,$gr,legend,IPadXr)   [$rini $key legendIPadXr    [lindex [$g legend cget -ipadx] 1]]
      set opt(gr,$gr,legend,IPadYt)   [$rini $key legendIPadYt    [lindex [$g legend cget -ipady] 0]]
      set opt(gr,$gr,legend,IPadYb)   [$rini $key legendIPadYb    [lindex [$g legend cget -ipady] 1]]
      set opt(gr,$gr,legend,PadXl)    [$rini $key legendPadXl     [lindex [$g legend cget -padx] 0]]
      set opt(gr,$gr,legend,PadXr)    [$rini $key legendPadXr     [lindex [$g legend cget -padx] 1]]
      set opt(gr,$gr,legend,PadYt)    [$rini $key legendPadYt     [lindex [$g legend cget -pady] 0]]
      set opt(gr,$gr,legend,PadYb)    [$rini $key legendPadYb     [lindex [$g legend cget -pady] 1]]
      set opt(gr,$gr,legend,Rows)     [$rini $key legendRows      [$g legend cget -rows]]
      set opt(gr,$gr,legend,Columns)  [$rini $key legendColumns   [$g legend cget -columns]]
      
      set opt(gr,$gr,grid,Color)     [$rini $key gridColor     "#e8e8e8"]
      set opt(gr,$gr,grid,Dashes)    [$rini $key gridDashes    {3 1}]
      set opt(gr,$gr,grid,LineWidth) [$rini $key gridLineWidth [$g grid cget -linewidth]]
      set opt(gr,$gr,grid,Minor)     [$rini $key gridMinor     0]
      set opt(gr,$gr,grid,MapX)      [$rini $key gridMapX      x]
      set opt(gr,$gr,grid,MapY)      [$rini $key gridMapY      y]
      
      set opt(gr,$gr,background)      [$rini $key background  #ffffff]
      set opt(gr,$gr,relief)          [$rini $key relief      flat]
      set opt(gr,$gr,areaborderwidth) [$rini $key areaborderwidth 0]
      
      set opt(gr,$gr,lm)             [$rini $key lm          [$g cget -lm]]
      set opt(gr,$gr,rm)             [$rini $key rm          [$g cget -rm]]
      set opt(gr,$gr,tm)             [$rini $key tm          [$g cget -tm]]
      set opt(gr,$gr,bm)             [$rini $key bm          [$g cget -bm]]
      set opt(gr,$gr,plotbackground)  [$rini $key plotbackground  white]
      set opt(gr,$gr,plotrelief)      [$rini $key plotrelief      flat]
      set opt(gr,$gr,plotborderwidth) [$rini $key plotborderwidth 0]
      set opt(gr,$gr,plotpadxl)  [$rini $key plotpadxl 0]
      set opt(gr,$gr,plotpadxr)  [$rini $key plotpadxr 0]
      set opt(gr,$gr,plotpadyt)  [$rini $key plotpadyt 0]
      set opt(gr,$gr,plotpadyb)  [$rini $key plotpadyb 0]
      set opt(gr,$gr,invertxy)   [$rini $key invertxy 0]
    }
    # Para el orden de las series, opt puede cambiar, pero no cambiamos data
    # lstnames {id name caption label}
    set opt(gr,$gr,lstnames) {}
    set i 1
    foreach it $data(gr,$gr,elements) {
      lappend opt(gr,$gr,lstnames) [list $i \
        $it \
        [$g element cget [lindex $it 0] -label] \
        [$g element cget [lindex $it 0] -label]]
      incr i
    }    

    #Completamos valores de arrays
    GrapOptGetMarkers $this
    
    # Completamos los arrays de opciones de lineas
    foreach it $optLinesAll {
      set len [llength $opt(gr,$gr,$it)]
      for {set i $len} {[expr $i < [llength [$g element names]]]} {incr i} {
        lappend opt(gr,$gr,$it) [lindex $opt(gr,$gr,$it) 0]
      }
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::WriteIni {this { gcf 0 } } {
#
# PURPOSE: Reads general graph options from .ini file. The ini file is the
#          active graphic configuration file.
#
# PARAMETERS:
#   this  -> Instance of BayesGraph
#   gcf   -> 1. Indicate save in GCF file. O. Save in ini file
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::options opt
  variable optLinesAll
  
  set gr $data(gr,active)
  
  # exists gcf      
  if {$gcf} {
    set wini ::iniFile::WriteGCF    
  } else {
    set wini ::iniFile::Write
  }  

  set key $data(namespace)
  
  $wini $key PhotoLastDir $opt(PhotoLastDir)
  #Coger el estado del grid
  $wini $key grid $opt(grid)
  
  # opciones de Max y Min de ejes
  foreach pre {x y x2 y2} {
    foreach post {Show Min Max All} {
      $wini $key ${pre}Axis${post} $opt(gr,$gr,${pre}Axis${post})
    }
  }
  # opciones de formato de ejes
  foreach pre {y y2 x x2} {
    foreach post {Number Percent} {
      $wini $key ${pre}Format${post} $opt(gr,$gr,${pre}Format${post})
    }
  }
  #Opciones de ticks (tipo, cuantos, cada cuantos y estilo) de ejes
  foreach pre {x y x2 y2} {
    foreach post {Type Total Step Font Rotate Color Shadow Length Subdivisions Width Show} {
      $wini $key ${pre}Ticks${post} $opt(gr,$gr,${pre}Ticks${post})
    }
  }
  #Opciones del título del gráfico
  foreach it {font foreground shadow} {
    $wini $key $it $opt(gr,$gr,$it)
  }
  # Titulos y opciones de títulos de título de ejes
  foreach pre {x y x2 y2} {
    $wini $key ${pre}AxisTitle $opt(gr,$gr,${pre}AxisTitle)
    foreach post {Color Font Shadow} {
      $wini $key ${pre}AxisTitle${post} $opt(gr,$gr,${pre}Axis${post})
    }
  }

  #Opciones de auto-ajuste
  foreach it {selffit} {
    $wini $key $it $opt(gr,$gr,$it)
  }
  #Opciones de Marcadores
  foreach it {x x2 y y2} {
    foreach at {"" In} {
      $wini $key ${it}Markers${at} $opt(gr,$gr,${it}Markers${at})
    }
  }
  foreach it {x x2 y y2} {
    foreach ty {Colors Dashes Widths Unders} {
      $wini $key ${it}Markers${ty}InEach $opt(gr,$gr,${it}Markers${ty}InEach)
    }
  }
  
  #Opciones de estilo de los elementos
  foreach it $optLinesAll {
    $wini $key $it $opt(gr,$gr,$it)
  }

  #Opciones del grid
  foreach it {Color Dashes LineWidth Minor MapX MapY} {
    $wini $key grid$it $opt(gr,$gr,grid,$it)
  }
  #Opciones de la leyenda
  foreach it {Hide Rows Columns Font Fg Shadow Bg Bd Relief Position Anchor Raised} {
    $wini $key legend$it $opt(gr,$gr,legend,$it)
  }
  foreach it {IPadXl IPadXr IPadYt IPadYb PadXl PadXr PadYt PadYb} {
    $wini $key legend$it $opt(gr,$gr,legend,$it)
  }
  #Opciones del gráfico
  foreach it {background relief areaborderwidth lm rm tm bm invertxy} {
    $wini $key $it $opt(gr,$gr,$it)
  }
  foreach it {plotbackground plotrelief plotborderwidth plotpadxl plotpadxr plotpadyt plotpadyb} {
    $wini $key $it $opt(gr,$gr,$it)
  }
  
  # OnWriteInit  
  if [info exists ${this}::data(proc,OnWriteIni)] {
    $data(proc,OnWriteIni) $this
  }
  
  if {$gcf} {
    ::bayesGraph::SaveGCF $this
    ::iniFile::FlushGCF $key
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::bayesGraph::CalculateXAxisNumTicks {this axis} {
#  
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 ${this}::data data
  set gr $data(gr,active)
  set g [getActiveGraph $this]

  set font [$g axis cget $axis -tickfont]
  if {[$g axis cget $axis -rotate] > 15} {
    array set fontMetrics [font metrics $font]
    set widthTick [expr $fontMetrics(-ascent) + $fontMetrics(-descent)]
  } else  {
    set lst [$data(gr,$gr,OnGetXTicks) $this]
    set widthTick [expr [font measure $font [lindex $lst end]] + 4]
  }
  set limits    [$g axis limits $axis]
  set pixelIni  [$g axis transform $axis [lindex $limits 0]]
  set pixelFin  [$g axis transform $axis [lindex $limits 1]]
  
  set widthAxis [expr $pixelFin - $pixelIni]
  set nTicks    [expr $widthTick ? ($widthAxis / $widthTick) : 0]
  return $nTicks
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::CalculateYAxisNumTicks {this axis} {
#  
#/////////////////////////////////////////////////////////////////////////////      
  set g [getActiveGraph $this]
  set font [$g axis cget $axis -tickfont]
  array set fontMetrics [font metrics $font]
  #si se ha rotado (hacer si se permite rotar)
  set heightTick [expr $fontMetrics(-ascent) + $fontMetrics(-descent)]
  set limits    [$g axis limits $axis]
  set pixelIni  [$g axis transform $axis [lindex $limits 0]]
  set pixelFin  [$g axis transform $axis [lindex $limits 1]]

  set heightAxis [expr $pixelIni - $pixelFin]
  set nTicks    [expr $heightTick ? ($heightAxis / $heightTick) : 0]
  return $nTicks
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::FixXAxisStep {this step axis} {
#  
#/////////////////////////////////////////////////////////////////////////////      
  upvar \#0 ${this}::data data
  set g [getActiveGraph $this]
  set min [$g axis cget $axis -min]
  set max [$g axis cget $axis -max]
  if {[info exist data(proc,OnFixStepX)]} {
    set step [$data(proc,OnFixStepX) $this $min $max $step]
  }
  return $step
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::FixYAxisStep {this step} {
#  
#/////////////////////////////////////////////////////////////////////////////      
  if {$step <= 0} {
    return 0
  }
  if {$step < 1} {
    set nDig [expr $step ? (int(floor(log10(1/$step)))) : 0]
    set num [expr $nDig + 2]
    set step [format "%.${num}f" $step]
  } else {
    set nDig [expr $step ? (int(floor(log10($step)))) : 0]
    if {$nDig >= 1} {
      set num [expr $nDig - 1]
      if {![catch {expr int(pow(10,$num)) ? floor($step/int(pow(10,$num)))*pow(10,$num) : 0} res]} {
        set step $res
      } else {
        set step 0
      }
      #set step [expr int(pow(10,$num)) ? floor($step/int(pow(10,$num)))*pow(10,$num) : 0]
    } else  {
      set num [expr $nDig]
      set step [format "%.1f" $step]
    }
  }
  return $step
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::ComputeXAxisStep {this nTicks axis} {
#
# PURPOSE:
#
# PARAMETERS:
#   this   -> Instance of BayesGraph
#   nTicks ->
#   axis   -> 
# RETURN 
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  variable tmpOpt
  if {$nTicks == 0 } { return 0 }

  set g [getActiveGraph $this]
  set min [$g axis cget $axis -min]
  set max [$g axis cget $axis -max]
  if {![string length $min] && ![string length $max]} {
    #Le damos el valor minimo y maximo del eje opuesto
    switch [string length $axis] {
      2       { set axiOpo [string index $axis 0] }
      default { set axiOpo ${axis}2 }
    }
    set values2 [::bayesGraph::GetLimitsValueAxis $this $axiOpo]
    set min [lindex $values2 0]
    set max [lindex $values2 1]
    set tmpOpt(var,${axis}AxisMin) $min
    set tmpOpt(var,${axis}AxisMax) $max

    ::bayesGraph::GrapOptApplyAxisLimits $this $axis $min $max
  } elseif {![string length $min]} {
    set min [expr $max - 100]
  } elseif {![string length $max]} {
    set max [expr $min + 100]
  }
  if {[info exist ${this}::data(proc,OnComputeStepX)]} {
    set step [$data(proc,OnComputeStepX) $this $min $max $nTicks]
  } else {
    set dif [expr $max - $min]
    set step [expr $nTicks ? ($dif/$nTicks) : 0]
  }
  return $step
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::ComputeYAxisStep {this nTicks axis} {
#  
#/////////////////////////////////////////////////////////////////////////////      
  variable tmpOpt
  if {$nTicks == 0 } { return 0 }
  set g [getActiveGraph $this]
  #Obtenemos valor minimo y maximo de las lineas
  set min [$g axis cget $axis -min]
  set max [$g axis cget $axis -max]
  if {![string length $min] && ![string length $max]} {
  #Le damos el valor minimo y maximo del eje opuesto
    switch [string length $axis] {
      2       { set axiOpo [string index $axis 0] }
      default { set axiOpo ${axis}2 }
    }
    set values2 [::bayesGraph::GetLimitsValueAxis $this $axiOpo]
    if {[llength $values2]} {
      set min [lindex $values2 0]
      set max [lindex $values2 1]
      set tmpOpt(var,${axis}AxisMin) $min
      set tmpOpt(var,${axis}AxisMax) $max
      ::bayesGraph::GrapOptApplyAxisLimits $this $axis $min $max
    }
  }
  # Se comprueba valor de los ejes
  if {![string length $min] && ![string length $max]} {
    set min 0
    set max 100
  } elseif {![string length $min]} {
    set min [expr $max - 100]
  } elseif {![string length $max]} {
    set max [expr $min + 100]
  }
  set dif [expr $max - $min]
  set step [expr $nTicks ? ($dif/$nTicks) : 0]
  return [FixYAxisStep $this $step]
}


# Opciones del grafico
#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::LoadGCF {this {fileGCF ""}} {
#  
#/////////////////////////////////////////////////////////////////////////////      
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::options opt
  set key $data(namespace)

  if {[string length $fileGCF]} {   
    set opt(GCFlastDir) [file dirname $fileGCF]
    set opt(GCFFile)    $fileGCF
    ::iniFile::SetGCF   $key $opt(GCFFile)
  } else  {  
    set opt(GCFFile)    [::iniFile::Read $key GCFFile ""]
    set opt(GCFlastDir) [::iniFile::Read $key GCFlastDir ""]

    if { ![file exists $opt(GCFFile)] } {
      if {![string equal $opt(GCFFile) ""]} {
        tk_messageBox -type ok -icon info \
          -message [mc "File $opt(GCFFile) doesn't exist"] \
          -title [mc "Warning"]
        set opt(GCFFile) ""
      }    
      ::iniFile::SetGCF $key ""
    } else  {    
      ::iniFile::SetGCF $key $opt(GCFFile)
    }
  }
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::SaveGCF {this} {
#  
#/////////////////////////////////////////////////////////////////////////////      
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::options opt
  set key $data(namespace)

  ::iniFile::Write $key GCFFile $opt(GCFFile)
  ::iniFile::Write $key GCFlastDir $opt(GCFlastDir)
  ::iniFile::SaveGCF $key $opt(GCFFile)
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::OpenGraphOpt {this} {
#  
#/////////////////////////////////////////////////////////////////////////////      
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::options opt
  set key $data(namespace)
  ::bayesGraph::SaveGCF $this
  #Escribe en la variable opt(GCFFile)
  set typelist {{"Graphic Configuration Files" {".gcf"}}}
  set file [tk_getOpenFile -title [mc "Open"]    -parent $widgets(this) \
                           -initialdir $opt(GCFlastDir)\
                           -defaultextension .gcf -filetypes $typelist]
  #Falta comprobar que el GCF es de mi clave
  ::iniFile::SetGCF $key $file
  set opt(GCFFile) $file
  if ![string equal $file ""] {
    set opt(GCFlastDir) [file dirname $file]
  }
  ::${data(namespace)}::ReadIni   $this
  ::${data(namespace)}::Configure $this
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::SaveGraphOpt {this} {
#  
#/////////////////////////////////////////////////////////////////////////////      
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::options opt
  set key $data(namespace)
  #Escribe en la variable opt(GCFFile)
  set typelist {{"Graphic Configuration Files" {".gcf"}}}
  set file [tk_getSaveFile -title [mc "Save as"] -parent $widgets(this) \
                           -initialdir $opt(GCFlastDir)\
                           -defaultextension .gcf -filetypes $typelist]
  set opt(GCFFile) $file
  set opt(GCFlastDir) [file dirname $file]
  ::bayesGraph::SaveGCF $this
  WriteIni $this 1
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOpt {this args} {
#
# PURPUSE
# PARAMETERS: this -> Instace
#             args -> options
#               -tabshow: tab selected
#               -nlne: line show
#/////////////////////////////////////////////////////////////////////////////      
  global toltk_options
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data data
  # options
  array set opts [list \
    -tabshow {} \
    -nline   1 \
    -iniaxis x
  ]  
  array set opts $args  
  
  set path $widgets(this)
  set dialog [Dialog $path.t -title [mc "Graph Options"]\
              -modal local -parent $path]

  wm protocol $dialog WM_DELETE_WINDOW [list ::bayesGraph::GrapOptDestroy $this $dialog]
  set w [$dialog getframe]
  wm resizable $dialog 0 0
  set w_tabset [::blt::tabset $w.ts -relief flat -highlightthickness 0\
		        -bd 0 -outerpad 0 -tier 2 -slant both -textside right\
                -side left -rotate 90 ]
  
  #insertar tabset
  $w_tabset insert end General
  set f0 [frame $w_tabset.f0]
  $w_tabset tab configure "General" -text [mc "General"]\
           -window $f0 -fill both -padx 0 -selectbackground \#d9d9d9 -bg gray75
  #Crear opciones generales
  eval GrapOptCreate $this $f0 [array get opts]
  #Crear opciones especificas
  if [info exists ${this}::data(proc,OnOptionCreate)] {
    $data(proc,OnOptionCreate) $this $w_tabset
  }
    
  set w_bbox \
          [ButtonBox $w.bb -orient horizontal -spacing 10 -padx 0 -pady 0 \
          -homogeneous false]
  $w_bbox add -text [mc "Accept"] -image [::Bitmap::get accept] \
      -helptext [mc "OK"] -relief link -compound left -padx 5 -pady 1\
      -command [list ::bayesGraph::GrapOptOK $this $dialog]
  $w_bbox add -text [mc "Cancel"] -image [::Bitmap::get cancelx] \
      -helptext [mc "Cancel"] -relief link -compound left -padx 5 -pady 1\
      -command [list ::bayesGraph::GrapOptDestroy $this $dialog]
  $w_bbox add -text [mc "Apply"] -image [::Bitmap::get apply] \
      -helptext [mc "Apply"] -relief link -compound left -padx 5 -pady 1\
      -command [list ::bayesGraph::GrapOptApply $this]

  if {$data(gr,numGraph) > 1} {
    $w_bbox add -text [mc "Apply to all"] -image [::Bitmap::get applyAll] \
      -helptext [mc "Apply to all"] -relief link -compound left -padx 5 -pady 1\
      -command [list ::bayesGraph::GrapOptApplyAll $this] 
  }
  
  pack $w_bbox -fill x -expand false -side bottom
  pack $w_tabset -fill both -expand true
  bind $dialog <F1> "ShowHelp Tb4GraOpc"
  
  $dialog draw
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::GrapOptDestroy {this dialog} {
#
# PURPOSE: Destroy graph options dialog. It also call to procedure that 
#          destroy specific options of a bayesGraph's child.
#
# PARAMETERS:
#   this   -> Instance of bayesGraph
#   dialog -> Graph options dialog
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  variable tmpOpt
  
  #Destruir opciones especificas
  unset data(lines,selectant)
  if [info exists ${this}::data(proc,OnOptionDestroy)] {
    $data(proc,OnOptionDestroy) $this
  }
  array unset tmpOpt
  destroy $dialog
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptCreate {this frame args} {
#  
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::widgets widgets
  # options
  array set opts [list \
    -tabshow {} \
    -nline   1 \
    -iniaxis x
  ]
  array set opts $args

  set widgets(general,w_tabset) [::blt::tabset $frame.ts -relief flat \
    -highlightthickness 0 -bd 0 -outerpad 0 -tier 2 -slant right \
    -textside right]
  #puts "GrapOptCreate. $widgets(general,w_tabset)"  
  $widgets(general,w_tabset) insert end Axis Lines Grid Legend Style

  set f1  [frame $widgets(general,w_tabset).f1]
  set f2  [frame $widgets(general,w_tabset).f2]
  set f3  [frame $widgets(general,w_tabset).f3]
  set f4  [frame $widgets(general,w_tabset).f4]
  set f5  [frame $widgets(general,w_tabset).f5]
  
  $widgets(general,w_tabset) tab configure "Axis" -text "[mc "Axis"]" \
    -window $f1 -fill both -padx 0 -selectbackground \#d9d9d9 -bg gray75
  $widgets(general,w_tabset) tab configure "Lines" -text [mc "Lines"] \
    -window $f2 -fill both -padx 0 -selectbackground \#d9d9d9 -bg gray75
  $widgets(general,w_tabset) tab configure "Grid" -text [mc "Grid"] \
    -window $f3 -fill both -padx 0 -selectbackground \#d9d9d9 -bg gray75
  $widgets(general,w_tabset) tab configure "Legend" -text [mc "Legend"] \
    -window $f4 -fill both -padx 0 -selectbackground \#d9d9d9 -bg gray75
  $widgets(general,w_tabset) tab configure "Style" -text [mc "Style"] \
    -window $f5 -fill both -padx 0 -selectbackground \#d9d9d9 -bg gray75
  
  GrapOptGet            $this
  GrapOptCreateAxisXY   $this $f1 $opts(-iniaxis)
  GrapOptCreateLines    $this $f2 $opts(-nline)

  GrapOptCreateGrid     $this $f3
  GrapOptCreateLegend   $this $f4
  GrapOptCreateStyle    $this $f5

  GrapOptInit  $this
  GrapOptCheck $this
  #GrapOptSet   $this
  
  # show tabset indicates
  if {[string length $opts(-tabshow)]} {
    set ts $widgets(general,w_tabset)
    $ts select [$ts index -name $opts(-tabshow)]
  }  

  grid $widgets(general,w_tabset) -sticky news  
  grid rowconfigure    $frame 0 -weight 1
  grid columnconfigure $frame 0 -weight 1
}


#/////////////////////////////////////////////////////////////////////////////
proc ::bayesGraph::ShowAxisSelectXY {this} {
#
# PURPOSE: Muestra la informacion relativa al axis seleccionado
#
# PARAMETERS: this -> instancia del grafico
#
#/////////////////////////////////////////////////////////////////////////////
  # completamos las accines pendientes de visualizacion
  update idletasks

  global toltk_options
  upvar \#0 ${this}::data data
  set gr $data(gr,active)
  variable tmpOpt
  # eje acutal
  set axis $tmpOpt(var,xyAxisSelect)

  # asignacion de valores
  foreach {a b} [list $tmpOpt(var,xyOldAxis) xy xy $axis] {
    if {[info exists tmpOpt(var,${b}AxisShow)]} {
      set tmpOpt(var,${a}AxisShow)     $tmpOpt(var,${b}AxisShow)
      set tmpOpt(var,${a}AxisMin)      $tmpOpt(var,${b}AxisMin)
      set tmpOpt(var,${a}AxisMax)      $tmpOpt(var,${b}AxisMax)
      set tmpOpt(var,${a}AxisAll)      $tmpOpt(var,${b}AxisAll)
      set tmpOpt(var,${a}TicksType)    $tmpOpt(var,${b}TicksType)
      set tmpOpt(var,${a}TicksTotal)   $tmpOpt(var,${b}TicksTotal)
      set tmpOpt(var,${a}TicksStep)    $tmpOpt(var,${b}TicksStep)
      set tmpOpt(var,${a}TicksFont)    $tmpOpt(var,${b}TicksFont)
      set tmpOpt(var,${a}TicksRotate)  $tmpOpt(var,${b}TicksRotate)
      set tmpOpt(var,${a}TicksColor)   $tmpOpt(var,${b}TicksColor)
      set tmpOpt(var,${a}TicksLength)  $tmpOpt(var,${b}TicksLength)
      set tmpOpt(var,${a}TicksShadow)  $tmpOpt(var,${b}TicksShadow)
      set tmpOpt(var,${a}TicksSubdivisions)  $tmpOpt(var,${b}TicksSubdivisions)
      set tmpOpt(var,${a}TicksShow)    $tmpOpt(var,${b}TicksShow)
      set tmpOpt(var,${a}TicksWidth)   $tmpOpt(var,${b}TicksWidth)
      
      set tmpOpt(var,${a}AxisTitle)   $tmpOpt(var,${b}AxisTitle)
      set tmpOpt(var,${a}AxisColor)   $tmpOpt(var,${b}AxisColor)
      set tmpOpt(var,${a}AxisFont)    $tmpOpt(var,${b}AxisFont)
      set tmpOpt(var,${a}AxisShadow)  $tmpOpt(var,${b}AxisShadow)

      set tmpOpt(var,${a}FormatNumber)  $tmpOpt(var,${b}FormatNumber)
      set tmpOpt(var,${a}FormatPercent) $tmpOpt(var,${b}FormatPercent)

      set tmpOpt(var,${a}Markers)       $tmpOpt(var,${b}Markers)
      set tmpOpt(var,${a}MarkersIn)     $tmpOpt(var,${b}MarkersIn)

      foreach j {0 1 2 3 4} {
        set tmpOpt(var,${a}MarkerColorInEach$j) $tmpOpt(var,${b}MarkerColorInEach$j)
        set tmpOpt(var,${a}MarkerWidthInEach$j) $tmpOpt(var,${b}MarkerWidthInEach$j)
        set tmpOpt(var,${a}MarkerDasheInEach$j) $tmpOpt(var,${b}MarkerDasheInEach$j)
        set tmpOpt(var,${a}MarkerUnderInEach$j) $tmpOpt(var,${b}MarkerUnderInEach$j)
      }
    }
  }
  # Etiquetas para los ejes
  set AXI [string toupper [string index $axis 0]]
  if {[info exists data(gr,$gr,OnGet${AXI}Ticks)]} {
    if {[string is double $tmpOpt(var,${axis}AxisMin)] && \
        [string length $tmpOpt(var,${axis}AxisMin)] } {
      set value [lindex [$data(gr,$gr,OnGet${AXI}Ticks) $this] \
        [expr int(floor($tmpOpt(var,${axis}AxisMin)))]]
      if {[string length $value]} {
        set tmpOpt(var,xyAxisMin) $value
      }
    }
    if {[string is double $tmpOpt(var,${axis}AxisMax)] && \
        [string length $tmpOpt(var,${axis}AxisMax)]} {
      set value [lindex [$data(gr,$gr,OnGet${AXI}Ticks) $this] \
        [expr int(floor($tmpOpt(var,${axis}AxisMax)))]]
      if {[string length $value]} {
        set tmpOpt(var,xyAxisMax) $value
      }
    }
  }
  # activamos opciones dependiendo de la pulsacion
#  GrapOptCheckXYTicksType $this

  GrapOptCheckXYAxisAll   $this
  # guardamos valor actual
  set tmpOpt(var,xyOldAxis) $axis
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::GrapOptCreateAxisXY {this w {iniaxis x}} {
#
# PURPOSE: Muestra la informacion relativa al axis seleccionado
#
# PARAMETERS: this -> instancia del grafico
#
#/////////////////////////////////////////////////////////////////////////////
  global toltk_options
  upvar \#0 ${this}::data data
  variable tmpOpt
  set gr $data(gr,active)
  # X-Selector
  TitleFrame $w.tfxs -text [mc "Axis"]  
  set fs [$w.tfxs getframe]
  # inizializamos visulizacion
  set tmpOpt(var,xyOldAxis)    $iniaxis
  set tmpOpt(var,xyAxisSelect) $iniaxis
  ShowAxisSelectXY $this

  set tmpOpt(widget,rbAxisX) [radiobutton $fs.rbAxisX -text "X: [mc Bottom]"\
     -value x -highlightthickness 0 -pady 0 \
     -variable ::bayesGraph::tmpOpt(var,xyAxisSelect) \
     -command [list ::bayesGraph::ShowAxisSelectXY $this]]
  set tmpOpt(widget,rbAxisX2) [radiobutton $fs.rbAxisX2 -text "X2: [mc Top]"\
     -value x2 -highlightthickness 0 -pady 0\
     -variable ::bayesGraph::tmpOpt(var,xyAxisSelect) \
     -command [list ::bayesGraph::ShowAxisSelectXY $this]]
  set tmpOpt(widget,rbAxisY) [radiobutton $fs.rbAxisY -text "Y: [mc Left]" \
     -value y -highlightthickness 0 -pady 0 \
     -variable ::bayesGraph::tmpOpt(var,xyAxisSelect) \
     -command [list ::bayesGraph::ShowAxisSelectXY $this]]
  set tmpOpt(widget,rbAxisY2) [radiobutton $fs.rbAxisY2 -text "Y2: [mc Right]"\
     -value y2 -highlightthickness 0 -pady 0\
     -variable ::bayesGraph::tmpOpt(var,xyAxisSelect) \
     -command [list ::bayesGraph::ShowAxisSelectXY $this]]

  grid $fs.rbAxisX   -sticky nws -pady 2
  grid $fs.rbAxisX2  -sticky nws -pady 2
  grid $fs.rbAxisY   -sticky nws -pady 2
  grid $fs.rbAxisY2  -sticky nws -pady 2

  grid rowconfigure    $fs 9 -weight 1
  grid columnconfigure $fs 0 -weight 1

  # Tab Set
  set w_tabset [::blt::tabset $w.ts -relief flat -highlightthickness 0\
		        -bd 0 -outerpad 0 -tier 2 -slant right -textside right]
  $w_tabset insert end Scales Title Format Marker

  set f1 [frame $w_tabset.f1]
  set f2 [frame $w_tabset.f2]
  set f3 [frame $w_tabset.f3]
  set f4 [frame $w_tabset.f4]
  
  $w_tabset tab configure "Scales" -text [mc Scales]  -window $f1 \
    -fill both -padx 0 -selectbackground \#d9d9d9 -bg gray75
  $w_tabset tab configure "Format" -text [mc Format]  -window $f2 \
    -fill both -padx 0 -selectbackground \#d9d9d9 -bg gray75
  $w_tabset tab configure "Marker" -text [mc Marker]  -window $f3 \
    -fill both -padx 0 -selectbackground \#d9d9d9 -bg gray75
  $w_tabset tab configure "Title"  -text [mc Title]   -window $f4 \
    -fill both -padx 0 -selectbackground \#d9d9d9 -bg gray75

  # X-Axis
  TitleFrame $f1.tf1a -text [mc "Scales"]
  set f1a [$f1.tf1a getframe]

  checkbutton $f1a.chShow -text [mc "Visible"]\
     -highlightthickness 0 -pady 0 -padx 0\
     -variable ::bayesGraph::tmpOpt(var,xyAxisShow)

  set AXI [string toupper [string index $tmpOpt(var,xyAxisSelect) 0]]
  #Comprobamos que existe la funcion
  if {[info exists data(gr,$gr,OnGet${AXI}Ticks)]} {
    set lst [$data(gr,$gr,OnGet${AXI}Ticks) $this]
  } else {
    set lst {}
  }

  label $f1a.lFirst -text [mc First]:
  set tmpOpt(widget,xyAxisMin) \
    [SpinBox $f1a.sbFirst -values $lst -justify right -width 20 \
      -textvariable ::bayesGraph::tmpOpt(var,xyAxisMin)]
      
  label $f1a.lLast -text [mc Last]: 
  set tmpOpt(widget,xyAxisMax) \
    [SpinBox $f1a.sbLast -values $lst -justify right -width 20 \
      -textvariable ::bayesGraph::tmpOpt(var,xyAxisMax)]
  
  set tmpOpt(widget,chXYAll) \
    [checkbutton $f1a.chAll -text [mc "All values"]\
      -highlightthickness 0 -pady 0 -padx 0\
      -variable ::bayesGraph::tmpOpt(var,xyAxisAll)\
      -command [list ::bayesGraph::GrapOptCheckXYAxisAll $this]]

  grid $f1a.chShow -            -sticky w -pady 2
  grid $f1a.lFirst $f1a.sbFirst -sticky w -pady 2
  grid $f1a.lLast  $f1a.sbLast  -sticky w -pady 2
  grid $f1a.chAll  -column 1    -sticky nw 
  grid rowconfigure    $f1a 3 -weight 1
  grid columnconfigure $f1a 1 -weight 1

  # X-Axis ticks
  TitleFrame $f1.tf1b -text [mc "Ticks"]
  set f1b [$f1.tf1b getframe]

  set tmpOpt(widget,rbXYTotal) [radiobutton $f1b.rbTotal -text [mc Total]:\
     -value total -highlightthickness 0 -pady 0\
     -variable ::bayesGraph::tmpOpt(var,xyTicksType)\
     -command [list ::bayesGraph::GrapOptCheckXYTicksType $this]]
  set tmpOpt(widget,rbXYEach) [radiobutton $f1b.rbEach -text [mc Each]:\
     -value each -highlightthickness 0 -pady 0\
     -variable ::bayesGraph::tmpOpt(var,xyTicksType)\
     -command [list ::bayesGraph::GrapOptCheckXYTicksType $this]]
  set tmpOpt(widget,rbXYAuto) [radiobutton $f1b.rbAuto -text [mc Automatic]\
     -value auto -highlightthickness 0 -pady 0\
     -variable ::bayesGraph::tmpOpt(var,xyTicksType)\
     -command [list ::bayesGraph::GrapOptCheckXYTicksType $this]]

  set tmpOpt(widget,xyTicksTotal) [Entry $f1b.eTicksTotal -validate key\
     -vcmd {expr {[string is int %P] && [string length %P]<3}}\
     -width 10 -justify right -textvariable ::bayesGraph::tmpOpt(var,xyTicksTotal)]
  set tmpOpt(widget,xyTicksStep) [Entry $f1b.eTicksStep -validate key\
     -vcmd {expr {[string is double %P] && [string length %P]<9}}\
     -width 10 -justify right -textvariable ::bayesGraph::tmpOpt(var,xyTicksStep)]

  frame $f1b.f
  label $f1b.f.lFont -text [mc "Font"]:
  bfont $f1b.f.bFont -variable ::bayesGraph::tmpOpt(var,xyTicksFont)
  label $f1b.f.lRotate -text [mc "Rotate"]:
  SpinBox $f1b.f.sbRotate -range {0 360} -width 4 -justify right -editable true\
      -textvariable ::bayesGraph::tmpOpt(var,xyTicksRotate)
  label $f1b.f.lColor -text [mc "Color"]:
  bcolor $f1b.f.fColor -variable ::bayesGraph::tmpOpt(var,xyTicksColor)
  label $f1b.f.lLength -text [mc "Length"]:
  SpinBox $f1b.f.sbLength -range {0 20} -width 4 -justify right -editable true\
      -textvariable ::bayesGraph::tmpOpt(var,xyTicksLength)
  label $f1b.f.lShadow -text [mc "Shadow"]:
  bcolor $f1b.f.fShadow -variable ::bayesGraph::tmpOpt(var,xyTicksShadow) \
    -label "[mc "Change Shadow"]:" -optmenu 1  
  label $f1b.f.lSubdiv -text [mc "Subdivisions"]:
  SpinBox $f1b.f.sbSubdiv -range {1 10} -width 4 -justify right -editable true\
      -textvariable ::bayesGraph::tmpOpt(var,xyTicksSubdivisions)
  checkbutton $f1b.f.chShow -text [mc "Show ticks"]\
     -highlightthickness 0 -pady 0 -padx 0\
     -variable ::bayesGraph::tmpOpt(var,xyTicksShow)
  label $f1b.f.lWidth -text [mc "Line width"]:
  SpinBox $f1b.f.sbWidth -range {0 10} -width 4 -justify right -editable true\
      -textvariable ::bayesGraph::tmpOpt(var,xyTicksWidth)

  frame $f1b.f.f -width 15
  grid $f1b.f.f $f1b.f.chShow  -              $f1b.f.lWidth  $f1b.f.sbWidth  -sticky nw -padx 2
  grid ^        $f1b.f.lFont   $f1b.f.bFont   $f1b.f.lRotate $f1b.f.sbRotate -sticky nw -padx 2
  grid ^        $f1b.f.lColor  $f1b.f.fColor  $f1b.f.lLength $f1b.f.sbLength -sticky nw -padx 2
  grid ^        $f1b.f.lShadow $f1b.f.fShadow $f1b.f.lSubdiv $f1b.f.sbSubdiv -sticky nw -padx 2
  grid columnconfigure $f1b.f 2 -pad 12

  grid $f1b.rbTotal $f1b.eTicksTotal -sticky w 
  grid $f1b.rbEach  $f1b.eTicksStep  -sticky w 
  grid $f1b.rbAuto  -                -sticky nw 
  grid $f1b.f       -                -sticky ns -pady 4
  grid rowconfigure    $f1b 5 -weight 1
  grid columnconfigure $f1b 2 -weight 1

  grid $f1.tf1a -sticky news -padx 2
  grid $f1.tf1b -sticky news -padx 2
  grid rowconfigure    $f1 0 -weight 1
  grid rowconfigure    $f1 1 -weight 3
  grid columnconfigure $f1 0 -weight 1
  
  # Tab Title
  TitleFrame $f4.tf -text [mc "Title"]
  set f4a   [$f4.tf getframe]
  ::bayesGraph::GrapOptCreateTitles \
    $this $f4a [list xyAxisTitle xyAxisFont xyAxisColor xyAxisShadow]
  
  grid $f4.tf    -sticky news -padx 2
  grid rowconfigure    $f4 0 -weight 1
  grid columnconfigure $f4 0 -weight 1
  
  # Tab Set Format
  TitleFrame $f2.tf -text [mc "Number"]
  set f2a   [$f2.tf getframe]
  # Number
  set tmpOpt(var,eNumText)  12345.67890
  set tmpOpt(var,eSamText)  ""

  label $f2a.lFormat -text [mc "Format"]:
  Entry $f2a.eFormat -width 60 -justify left -text $tmpOpt(var,xyFormatNumber) \
    -textvariable ::bayesGraph::tmpOpt(var,xyFormatNumber)

  checkbutton $f2a.chPercent -text [mc "Show percent"] \
     -highlightthickness 0 -pady 0 -padx 0 \
     -variable ::bayesGraph::tmpOpt(var,xyFormatPercent) \
     -command [list ::bayesGraph::CheckPercent $this]

  label $f2a.lNumber -text [mc "Number"]:
  Entry $f2a.eNumber -width 30 -justify right \
     -validate key -vcmd {string is double %P} \
     -text $tmpOpt(var,eNumText) -textvariable ::bayesGraph::tmpOpt(var,eNumText)

  Button $f2a.bApply -text [mc Prove]\
    -helptext [mc "It proves the specified format"]\
    -command  [list ::bayesGraph::ProveNumber $this]

  label $f2a.lSample -text [mc "Sample"]:
  Entry $f2a.eSample -width 60 -justify left -editable 0 \
    -text $tmpOpt(var,eSamText) -textvariable ::bayesGraph::tmpOpt(var,eSamText)

  button $f2a.bHelpNumFor -text [mc "Format number"] \
     -image [::Bitmap::get infor] -compound left \
     -command [list ::bayesGraph::ShowNumberSpecifiers $this $w]

  grid $f2a.lFormat      $f2a.eFormat  -            -sticky nw -padx 5 -pady 5
  grid $f2a.chPercent    -             -            -sticky nw -padx 5 -pady 5
  grid $f2a.lNumber      $f2a.eNumber  $f2a.bApply  -sticky nw -padx 5 -pady 5
  grid $f2a.lSample      $f2a.eSample  -            -sticky nw -padx 5 -pady 5
  grid $f2a.bHelpNumFor  -             -            -sticky nw -padx 5 -pady 5

  grid rowconfigure    $f2a 6 -weight 1
  grid columnconfigure $f2a 1 -weight 1

  # Text
  grid $f2.tf    -sticky news -padx 2
  grid rowconfigure    $f2 0 -weight 1
  grid columnconfigure $f2 0 -weight 1

  # Marker
  GrapOptCreateMarkers $this $f3

  # Auto-ajuste
  TitleFrame $w.tfsf -text [mc "Self fit of values"]
  set f3 [$w.tfsf getframe]

  checkbutton $f3.cbSelfFit \
     -text [mc "Self fit of axes to the data of the graph to open"] \
     -highlightthickness 0 -pady 0 -padx 0 -offvalue 0 -onvalue 1 \
     -variable ::bayesGraph::tmpOpt(var,selffit)

  grid $f3.cbSelfFit -sticky nw -padx 2 -pady 4
  grid rowconfigure    $f3 1 -weight 1
  grid columnconfigure $f3 1 -weight 1


  # General
  grid $w.tfxs  $w_tabset -sticky news -padx 2
  grid ^        $w.tfsf   -sticky news -padx 2
  grid rowconfigure    $w 0 -weight 1
  grid rowconfigure    $w 1 -weight 0
  grid columnconfigure $w 1 -weight 1
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::ShowNumberSpecifiers {this w} {
#
# PURPOSE: Muestra una ventana con informacion sobre como dar formato a un
#   numero. Tambien se puede especificar el formato deseado y hacer pruebas
#
# PARAMETERS:
#   this  -> Instance of BayesGraph
#   w     -> Tabset axis
#   axis  -> axis [y | y2]
#
#/////////////////////////////////////////////////////////////////////////////
  if [winfo exist $w.t] {
    raise $w.numberFormat
    return
  }
  global toltk_options
  upvar \#0 ${this}::data data
  variable tmpOpt
  set dialog [Dialog $w.numberFormat -title [mc "Number format"]\
              -default 0 -parent $w -modal local -separator true]
  wm resizable $dialog 0 0
  set f1 [$dialog getframe]
  set color1 red
  # help
  label $f1.lmenos  -text "%-" -foreground $color1
  label $f1.lmenosd -justify left -text [mc ForNumOpc:-]

  label $f1.lmas   -text "%+" -foreground $color1
  label $f1.lmasd  -justify left -text [mc ForNumOpc:+]
  
  label $f1.lspace  -text "%space" -foreground $color1
  label $f1.lspaced -justify left -text [mc "ForNumOpc:Space"]

  label $f1.zero  -text "%0" -foreground $color1
  label $f1.zerod -justify left -text [mc "ForNumOpc:Zero"]

  label $f1.lf  -text "%f" -foreground $color1
  label $f1.lfd -justify left -text [mc "ForNumOpc:f"]

  label $f1.leE  -text "%e %E" -foreground $color1
  label $f1.leEd -justify left -text [mc "ForNumOpc:e"]

  label $f1.lgG  -text "%g %G" -foreground $color1
  label $f1.lgGd -justify left -text [mc "ForNumOpc:g"]

  label $f1.lpercent  -text "%%" -foreground $color1
  label $f1.lpercentd -text [mc "No conversion: just insert %"]
  
  Button $f1.b -image [::Bitmap::get "accept"]\
      -helptext [mc "OK"] -relief link -compound left -text [mc "Accept"]\
      -command "destroy $dialog"

  grid $f1.lmenos   $f1.lmenosd    -sticky nw -padx 5
  grid $f1.lmas     $f1.lmasd      -sticky nw -padx 5
  grid $f1.lspace   $f1.lspaced    -sticky nw -padx 5
  grid $f1.zero     $f1.zerod      -sticky nw -padx 5
  grid $f1.lf       $f1.lfd        -sticky nw -padx 5
  grid $f1.leE      $f1.leEd       -sticky nw -padx 5
  grid $f1.lgG      $f1.lgGd       -sticky nw -padx 5
  grid $f1.lpercent $f1.lpercentd  -sticky nw -padx 5
  grid $f1.b                       -sticky ns -columnspan 2 -pady 3

  $dialog draw
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::ProveNumber {this} {
#
# PURPOSE: guarda los parametros intoducidos
#
# PARAMETERS:
#   this  -> Instance of BayesGraph
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpOpt

  if {[string length $tmpOpt(var,xyFormatNumber)] && \
      [string length $tmpOpt(var,eNumText)]} {
    if {$tmpOpt(var,xyFormatPercent)} {
      set n [expr $tmpOpt(var,eNumText) * 100]
    } else  {
      set n $tmpOpt(var,eNumText)
    }
    if {![catch {set tmpOpt(var,eSamText) [format $tmpOpt(var,xyFormatNumber) $n]} res]} {
      return $tmpOpt(var,eSamText)
    } else {
      set tmpOpt(var,eSamText) ""
    }
  } else {
    set tmpOpt(var,eSamText) ""
  }
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::CheckPercent {this} {
#
# PURPOSE: Añade o elimina el tanto por ciento "%%" del formato
#
# PARAMETERS:
#   this  -> Instance of BayesGraph
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpOpt
  if {$tmpOpt(var,xyFormatPercent)} {
    set tmpOpt(var,xyFormatNumber) $tmpOpt(var,xyFormatNumber)%%
  } else  {
    set p [string first "%%" $tmpOpt(var,xyFormatNumber)]
    if {[expr $p >= 0]} {
      set tmpOpt(var,xyFormatNumber) \
        [string replace $tmpOpt(var,xyFormatNumber) $p \
               [expr $p + [string length "%%"] - 1] ""]
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::GrapOptCreateTitles {this f lstSuf} {
#
# PURPOSE: Crea las opcines de visualizacion de titulos de los ejes
#
# PARAMETERS:
#   this  -> Instance of BayesGraph
#   f     -> Frame title
#   lstSuf -> Lista de sufijo para los nombres de las variables. de 0 a 3 
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpOpt
  
  label $f.lTitle -text [mc "Title"]:
  Entry $f.eTitle -width 45 \
    -textvariable ::bayesGraph::tmpOpt(var,[lindex $lstSuf 0])
  label $f.lFont -text [mc "Font"]:
  bfont $f.bFont -variable ::bayesGraph::tmpOpt(var,[lindex $lstSuf 1])
  label $f.lColor -text [mc "Color"]:
  bcolor $f.fColor -variable ::bayesGraph::tmpOpt(var,[lindex $lstSuf 2])
  label $f.lShadow -text [mc "Shadow"]:
  bcolor $f.fShadow -variable ::bayesGraph::tmpOpt(var,[lindex $lstSuf 3]) \
    -label "[mc "Change shadow"]:" -optmenu 1  
  
  frame $f.f -width 25
  grid $f.f $f.lTitle $f.eTitle - - - - - -sticky nw -padx 2 -pady 3
  grid ^     ^ $f.lFont $f.bFont $f.lColor $f.fColor $f.lShadow $f.fShadow\
      -sticky nw -padx 2 -pady 0

  grid rowconfigure    $f 7 -weight 1
  grid columnconfigure $f 7 -weight 1
  grid columnconfigure $f 3 -pad   20
  grid columnconfigure $f 5 -pad   20
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptCreateLines {this w {linedefault 1}} {
#  
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::widgets widgets
  variable tmpOpt

  # Lines list
  TitleFrame $w.tf0 -text [mc "Lines list"]
  set f0 [$w.tf0 getframe]

  set widgets(lines,lst) [blistorder $f0.linesList \
    -cols [list id name caption label] -parent $w \
    -hiddencols {id name caption} -selectbackground grey \
    -colkeypress {label} -orientation horizontal -showtitles false \
    -height 200 -width 100 \
    -nomenu true -selectmode single -values $tmpOpt(var,lstnames) \
    -selectcommand [list ::bayesGraph::ShowSelectLines $this]]

  # LIST LINES
  # visualizamos opciones sobre la linea default
  # calculation position line in listbox
  set i 1
  set lineDef $linedefault
  foreach it $tmpOpt(var,lstnames) {
    if {[string equal $linedefault [lindex $it 0]]} {
      set lineDef $i
    }
    incr i
  }

  $widgets(lines,lst) selection set $lineDef
  ::bayesGraph::ShowOptLines $this $linedefault
  
  grid $widgets(lines,lst) -sticky news
  grid rowconfigure    $f0 0 -weight 1
  grid columnconfigure $f0 0 -weight 1  

  # OPTIONS LINES AND DATA
  set w_tabset [::blt::tabset $w.ts -relief flat -highlightthickness 0\
    -bd 0 -outerpad 0 -tier 2 -slant right -textside right]
  $w_tabset insert end Line Data

  set ts1 [frame $w_tabset.f1]
  set ts2 [frame $w_tabset.f2]

  $w_tabset tab configure "Line" -text [mc "Line"] \
    -window $ts1 -fill both -padx 0 -selectbackground \#d9d9d9 -bg gray75
  $w_tabset tab configure "Data" -text [mc "Data"]\
    -window $ts2 -fill both -padx 0 -selectbackground \#d9d9d9 -bg gray75

  GrapOptCreateLinesLine $this $ts1
  GrapOptCreateLinesData $this $ts2
  
  #GRID
  grid  $w.tf0 $w_tabset -sticky news -padx 1   
  
  grid rowconfigure    $w 0 -weight 1
 
  grid columnconfigure $w 0 -weight 1
  grid columnconfigure $w 1 -weight 0
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptCreateLinesLine {this w} {
#  
#/////////////////////////////////////////////////////////////////////////////  
  variable tmpOpt
  upvar \#0 ${this}::widgets widgets
  
  # Optiones lines
  TitleFrame $w.tf2 -text [mc "Line Options"]
  set f2 [$w.tf2 getframe]

  radiobutton $f2.rbIsLine -text [mc "Lines"] \
     -value LineElement -highlightthickness 0 -pady 0 -padx 0 \
     -variable ::bayesGraph::tmpOpt(var,linetypes_)
  label   $f2.llwidth -text [mc "Lines Width"]:  
  SpinBox $f2.sblwidth -justify right -width 4 -editable 0 -range {0 10}\
      -textvariable ::bayesGraph::tmpOpt(var,linewidth_)
  label $f2.ldashes -text [mc "Lines Type"]:
  ::Objects::ComboDashesCreate ::bayesGraph::tmpOpt(var,dashes_) ${f2}.sbdashes

  label $f2.lSmooth -text [mc "Smooth"]:
  set lst {linear step natural quadratic}
  foreach it $lst { lappend values [mc $it] }

  ComboBox $f2.cbSmooth -width 15 -height 5 -editable false\
     -values $values -justify right\
     -modifycmd [list ::Objects::ComboUnTranslate $f2.cbSmooth $lst\
       ::bayesGraph::tmpOpt(var,smooth_)]
  ::Objects::ComboSetIni $f2.cbSmooth $lst ::bayesGraph::tmpOpt(var,smooth_)

if {0} {
#puts "\nTitle: values: $values lst: $lst ::bayesGraph::tmpOpt(var,smooth_) : $::bayesGraph::tmpOpt(var,smooth_) \n"
#puts "\nTitle: [lindex $values [lsearch -exact $lst $::bayesGraph::tmpOpt(var,smooth_)]] \n"
  bcomboboxplus $f2.cbSmooth -cols {title id} -varcol {id} -parent $f2 \
    -variable ::bayesGraph::tmpOpt(var,smooth_) \
    -showtitles false -hiddencols {id} -hiddencolslb {id} \
    -text [lindex $values \
      [lsearch -exact $lst $::bayesGraph::tmpOpt(var,smooth_)]] \
    -values [ljoin $values $lst " "]
  $f2.cbSmooth activate select
}
  set values {}
  set lst {none image solid striped {stipple image} {stipple degraded}}
  foreach it $lst { lappend values [mc $it]  }
  label $f2.llTypeFill -text [mc "Fill type"]:
  ComboBox $f2.cblTypeFill -width 15 -height 5 \
    -editable false -justify right -values $values \
    -modifycmd [list ::Objects::ComboUnTranslate $f2.cblTypeFill $lst\
      ::bayesGraph::tmpOpt(var,areaTypes_)]
  ::Objects::ComboSetIni $f2.cblTypeFill $lst \
    ::bayesGraph::tmpOpt(var,areaTypes_)

  variable typelist {{"Gif images" {".gif"}}}
  variable defExt .gif
  label $f2.lImage -text [mc Image]:
  set tmpOpt(widget,areaTile) [::Objects::ChooseFileCreate \
    ::bayesGraph::tmpOpt(var,areaTiles_) $f2.bFillImage \
    -filetypes $typelist -defaultextension $defExt]
  
  grid $f2.rbIsLine   -               -sticky nw -pady 0
  grid $f2.llwidth    $f2.sblwidth    -sticky nw -pady 0
  grid $f2.ldashes    $f2.sbdashes    -sticky nw -pady 0
  grid $f2.lSmooth    $f2.cbSmooth    -sticky nw -pady 0
  grid $f2.llTypeFill $f2.cblTypeFill -sticky nw -pady 0
  grid $f2.lImage     $f2.bFillImage  -sticky nw -pady 0
  grid rowconfigure    $f2 10 -weight 1
  grid columnconfigure $f2 10 -weight 1

  # Optiones bar
  TitleFrame $w.tf3 -text [mc "Bar Options"]
  set f3 [$w.tf3 getframe]

  radiobutton $f3.rbIsLine -text [mc "Bar"] \
     -value BarElement -highlightthickness 0 -pady 0 -padx 0 \
     -variable ::bayesGraph::tmpOpt(var,linetypes_)
  label   $f3.lbwidth -text [mc "Bars Width"]:
  SpinBox $f3.sbbwidth -justify right -width 4 -editable 0 \
      -values {0 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1} \
      -textvariable ::bayesGraph::tmpOpt(var,barwidth_)
  
  set list {flat groove raised ridge solid sunken}
  set values ""
  foreach it $list { lappend values [mc $it] }
  label $f3.lRelief -text [mc "Bar relief"]:
  ComboBox $f3.cbRelief -width 15 -height 5 -editable false\
      -values $values -justify right\
      -modifycmd   [list ::Objects::ComboUnTranslate $f3.cbRelief $list \
      ::bayesGraph::tmpOpt(var,barrelief_)]]
  ::Objects::ComboSetIni $f3.cbRelief $list \
    ::bayesGraph::tmpOpt(var,barrelief_)
  
  label $f3.lbdw -text [mc "Border width"]:
  set tmpOpt(widget,bdwidth) [SpinBox $f3.sbbdw -range {1 100} \
    -justify right -width 4 -editable 0 \
    -textvariable ::bayesGraph::tmpOpt(var,borderwidth_)]
  
  label $f3.lbTypeFill -text [mc "Fill type"]:
  set values ""
  set list {none striped {stipple image} {stipple degraded}}
  foreach it $list { lappend values [mc $it]  }
  ComboBox $f3.cbbTypeFill -width 15 -height 5 \
    -editable false -justify right -values $values \
    -modifycmd   [list ::Objects::ComboUnTranslate $f3.cbbTypeFill $list\
    ::bayesGraph::tmpOpt(var,areaTypesBar_)]
  ::Objects::ComboSetIni $f3.cbbTypeFill $list \
    ::bayesGraph::tmpOpt(var,areaTypesBar_)

  grid $f3.rbIsLine   -               -sticky nw -pady 1
  grid $f3.lbwidth    $f3.sbbwidth    -sticky nw -pady 1
  grid $f3.lRelief    $f3.cbRelief    -sticky nw -pady 1
  grid $f3.lbdw       $f3.sbbdw       -sticky nw -pady 1
  grid $f3.lbTypeFill $f3.cbbTypeFill -sticky nw -pady 1
  grid rowconfigure    $f3 10 -weight 1
  grid columnconfigure $f3 10 -weight 1
   
  # General options
  TitleFrame $w.tf4 -text [mc "General Options"]
  set f4 [$w.tf4 getframe]
  
  label $f4.lname -text [mc Name]:
  Entry $f4.ename -width 50 -justify left \
    -textvariable ::bayesGraph::tmpOpt(var,linelabels_)
    
  checkbutton $f4.chHide -text [mc "Hide line"] \
    -highlightthickness 0 -pady 2 -padx 0 -onvalue true -offvalue false \
    -variable ::bayesGraph::tmpOpt(var,linehides_)  
  
  label $f4.lColor -text "[mc Color]:"
  bcolor $f4.fColor -variable ::bayesGraph::tmpOpt(var,colors_)

  label $f4.lMapped -text "[mc Mapped]:"
  foreach axis {x x2 y y2} {
    radiobutton $f4.rbMap${axis} -text [string toupper $axis] \
      -value $axis -highlightthickness 0 -pady 0 \
      -variable ::bayesGraph::tmpOpt(var,[string index $axis 0]MappedAxis_)
  }
  
  label $f4.lForeground -text [mc Foreground]:
  bcolor $f4.fForeground -optmenu 2 \
    -variable ::bayesGraph::tmpOpt(var,areaForegrounds_)    
  
  label $f4.lBackground -text [mc Background]:
  bcolor $f4.fBackground -optmenu 2 \
    -variable ::bayesGraph::tmpOpt(var,areaBackgrounds_)
  
  variable typelist {{"XBM images" {".xbm"}}}
  variable defExt .xbm
  label $f4.lImageBack -text [mc {Image background}]:
  ::Objects::ChooseFileCreate \
    ::bayesGraph::tmpOpt(var,stippleimages_) $f4.bFillImage \
    -filetypes $typelist -defaultextension $defExt

  label $f4.lsbDegraded -text [mc {Degraded percentage}]:
  SpinBox $f4.sbbstipple -justify right -width 4 -editable 0 \
    -values {12 25 50 75} \
    -textvariable ::bayesGraph::tmpOpt(var,stippledegradeds_)  

  grid $f4.chHide      -               -              -              \
    -sticky nw -pady 1
  grid $f4.lColor      $f4.fColor      -               -              \
    -sticky nw -pady 1  
  grid $f4.lForeground $f4.fForeground $f4.lImageBack  $f4.bFillImage \
    -sticky nw -pady 1
  grid $f4.lBackground $f4.fBackground $f4.lsbDegraded $f4.sbbstipple \
    -sticky nw -pady 1
  grid $f4.lMapped     $f4.rbMapx      $f4.rbMapy      -              \
    -sticky nw -pady 1
  grid ^               $f4.rbMapx2     $f4.rbMapy2     -              \
    -sticky nw -pady 1
  grid $f4.lname       $f4.ename       -               -              \
    -sticky nw -pady 1
  
  grid rowconfigure    $f4 10 -weight 1
  grid columnconfigure $f4 10 -weight 1
  
  # Optiones bar
  TitleFrame $w.tf5 -text [mc "For all element options"]
  set f5 [$w.tf5 getframe]  
  set list {infront stacked overlap aligned}
  set values ""
  foreach it $list { lappend values [mc $it] }
  label $f5.lBarMode -text [mc "Bar mode"]:
  ComboBox $f5.cbBarMode -width 15 -height 5 -editable false\
     -values $values -justify right\
     -modifycmd   [list ::Objects::ComboUnTranslate $f5.cbBarMode $list\
      ::bayesGraph::tmpOpt(var,barmode)]
  ::Objects::ComboSetIni  $f5.cbBarMode $list \
    ::bayesGraph::tmpOpt(var,barmode)  
  
  grid $f5.lBarMode    $f5.cbBarMode   -sticky nw -pady 1  
  grid rowconfigure    $f5 10 -weight 1
  grid columnconfigure $f5 10 -weight 1
 
  # Grid title frame
  grid  $w.tf2  $w.tf3 -sticky news  
  grid  $w.tf4  -      -sticky news
  grid  $w.tf5  -      -sticky news
  
  grid rowconfigure    $w 2  -weight 1
  grid columnconfigure $w 10 -weight 1  
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptCreateLinesData {this w} {
#  
#/////////////////////////////////////////////////////////////////////////////
  variable tmpOpt
  upvar \#0 ${this}::widgets widgets
  
  # DATA VALUES
  TitleFrame $w.tf1 -text [mc "Data values"]
  set f1 [$w.tf1 getframe]
  label $f1.lvshowvalues -text [mc "Show"]:

  set lst {both none x y}
  foreach it $lst { lappend values [mc $it]  }
  ComboBox $f1.cbshowvalues -width 8 -height 5 -editable false \
    -justify right -values $values \
    -modifycmd [list ::Objects::ComboUnTranslate $f1.cbshowvalues $lst \
      ::bayesGraph::tmpOpt(var,showvalues_)]
  ::Objects::ComboSetIni  $f1.cbshowvalues $lst \
    ::bayesGraph::tmpOpt(var,showvalues_)
  
  label $f1.lvfont -text [mc "Font"]:
  bfont $f1.bvfont -variable ::bayesGraph::tmpOpt(var,valuefont_)
  label $f1.lvrotate -text [mc Rotate]: -justify right
  SpinBox $f1.sbvrotate -range {0 360} -width 15 -justify right -width 4 \
    -editable 0 -textvariable ::bayesGraph::tmpOpt(var,valuerotate_)
  
  label $f1.lvcolor -text [mc Color]:
  bcolor $f1.fvcolor -variable ::bayesGraph::tmpOpt(var,valuecolor_)
  
  label $f1.lvformat -text [mc "Decimals"]:
  SpinBox $f1.sbvformat -range {0 4} -width 4 -justify right -editable false \
    -textvariable ::bayesGraph::tmpOpt(var,valueformatdec_)
  
  label $f1.lvshadow -text [mc "Shadow"]:
  bcolor $f1.fvshadow -variable ::bayesGraph::tmpOpt(var,valueshadow_) \
    -label "[mc "Change Shadow"]:" -optmenu 1  
  
  label $f1.lvanchor -text [mc Anchor]:
  ::Objects::ComboPosCreate ::bayesGraph::tmpOpt(var,valueanchor_) $f1.fvanchor
  
  grid $f1.lvshowvalues $f1.cbshowvalues  -             -             \
    -sticky nw -padx 2 -pady 3
  grid $f1.lvfont       $f1.bvfont        $f1.lvrotate  $f1.sbvrotate \
    -sticky w  -padx 2 -pady 1
  grid $f1.lvcolor      $f1.fvcolor       $f1.lvformat  $f1.sbvformat \
    -sticky w  -padx 2 -pady 1
  grid $f1.lvshadow     $f1.fvshadow      $f1.lvanchor  $f1.fvanchor  \
    -sticky w  -padx 2 -pady 1
  
  grid rowconfigure    $f1 10 -weight 1
  grid columnconfigure $f1 10 -weight 1
  
  # SYMBOLS DATA
  TitleFrame $w.tf2 -text [mc "Data symbols"]
  set f2 [$w.tf2 getframe]
  
  checkbutton $f2.chShowSymbols -text [mc "Show symbols"] \
    -highlightthickness 0 -pady 0 -padx 0 \
    -variable ::bayesGraph::tmpOpt(var,showsymbols_)
#Comando para desabilitar las opciones de simbolos  
#  -command [list ::bayesGraph::GrapOptCheckShowSymbols $this $serie]
    
  label $f2.lSymbol -text [mc "Symbol"]:
  set lst {circle square diamond plus cross splus scross triangle arrow}
  set values {}
  foreach it $lst { lappend values [mc $it] }
  ComboBox $f2.cbSymbol -width 10 -height 5 -values $values -justify right \
    -modifycmd   [list ::Objects::ComboUnTranslate $f2.cbSymbol $lst \
      ::bayesGraph::tmpOpt(var,symbol_)]
  ::Objects::ComboSetIni  $f2.cbSymbol $lst \
    ::bayesGraph::tmpOpt(var,symbol_)
  
  label $f2.lPixels -text [mc "Pixels"]:
  SpinBox $f2.sbPixels -range {2 36 2} -width 4 -justify right \
    -textvariable ::bayesGraph::tmpOpt(var,pixels_)
    
  checkbutton $f2.chScaleSymbols -highlightthickness 0 -pady 0 -padx 0 \
    -text [mc "Scale symbols"] \
    -variable ::bayesGraph::tmpOpt(var,scalesymbols_)

  label $f2.lFill -text [mc "Fill"]:
  bcolor $f2.fFill -variable ::bayesGraph::tmpOpt(var,fill_) -optmenu 3

  label $f2.lOutline -text [mc "Outline"]:
  bcolor $f2.fOutline -variable ::bayesGraph::tmpOpt(var,outline_) -optmenu 2  

  label $f2.lOWid -text [mc "Outline width"]:
  SpinBox $f2.sbOWid -range {0 8} -width 4 -justify right \
    -textvariable ::bayesGraph::tmpOpt(var,outlinewidth_)

  grid $f2.chShowSymbols -            -                  -             \
    -sticky nw -padx 2 -pady 4
  grid $f2.lSymbol       $f2.cbSymbol $f2.lPixels        $f2.sbPixels  \
    -sticky nw -padx 2
  grid $f2.lOutline      $f2.fOutline $f2.lOWid          $f2.sbOWid    \
    -sticky nw -padx 2
  grid $f2.lFill         $f2.fFill    $f2.chScaleSymbols -             \
    -sticky nw -padx 2  

  grid rowconfigure    $f2 10 -weight 1
  grid columnconfigure $f2 10 -weight 1
  
  grid $w.tf1 -sticky news -padx 2   
  grid $w.tf2 -sticky news -padx 2

  # GRID
  grid rowconfigure    $w 0 -weight 0
  grid rowconfigure    $w 1 -weight 1

#  grid rowconfigure    $w 1 -weight 0    
  grid columnconfigure $w 0 -weight 1
}


#/////////////////////////////////////////////////////////////////////////////
proc ::bayesGraph::GrapOptCreateMarkers {this w} {
#
# PURPOSE:
#
# PARAMETERS:
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpOpt
  # tab_set
  set tsmarker [::blt::tabset $w.ts -relief flat -highlightthickness 0\
		        -bd 0 -outerpad 0 -tier 2 -slant right -textside right]
  $tsmarker insert end Marker Style

  set f1 [frame $tsmarker.f1]
  set f2 [frame $tsmarker.f2]

  $tsmarker tab configure "Marker"   -text [mc Marker]   -window $f1 \
    -fill both -padx 0 -selectbackground \#d9d9d9 -bg gray75
  $tsmarker tab configure "Style" -text [mc Style] -window $f2 \
    -fill both -padx 0 -selectbackground \#d9d9d9 -bg gray75

  # Marker
  # Each
  TitleFrame $f1.eachXY -text [mc "Each"]
  ::bayesGraph::CreateMarkers $f1.eachXY xyMarkers
  # In X
  TitleFrame $f1.inXY -text [mc "In"]
  ::bayesGraph::CreateMarkers $f1.inXY xyMarkersIn
  # marker
  grid $f1.eachXY  $f1.inXY  -sticky news -padx 2
  grid rowconfigure    $f1 0 -weight 1
  grid columnconfigure $f1 0 -weight 1
  grid columnconfigure $f1 1 -weight 1

  #Style
  TitleFrame $f2.tf -text [mc "Markers Style"]
  set f [$f2.tf getframe]
  
  label $f.lStyle -text [mc "Choose lines color, type, position and width for 5 Markers"]:

  label $f.lColor  -text [mc Color]:
  label $f.lDashes -text [mc "Style"]:
  label $f.lWidth  -text [mc "Width"]:
  label $f.lUnder  -text [mc "Under"]:

  for {set i 0} {$i < 5} {incr i} {
    label $f.lDesc$i -text "[mc Marker] [expr $i+1]:"
    bcolor $f.fColor$i \
      -variable ::bayesGraph::tmpOpt(var,xyMarkerColorInEach$i)     
    ::Objects::ComboDashesCreate \
      ::bayesGraph::tmpOpt(var,xyMarkerDasheInEach$i) $f.sbDashes$i
    SpinBox $f.sbWidth$i -justify right -width 4 -editable 0 -range {0 10}\
            -textvariable ::bayesGraph::tmpOpt(var,xyMarkerWidthInEach$i)
    checkbutton $f.chUnder$i -highlightthickness 0 \
       -variable ::bayesGraph::tmpOpt(var,xyMarkerUnderInEach$i)
  }

  grid $f.lStyle - - - - -sticky nw -padx 5 -pady 5
  frame $f.f -width 15
  grid $f.f  $f.lColor  $f.lDashes  $f.lWidth $f.lUnder -sticky nw -padx 15 -pady 2
  for {set i 0} {$i < 5} {incr i} {
    grid $f.f $f.fColor$i  $f.sbDashes$i \
              $f.sbWidth$i $f.chUnder$i -sticky new -padx 15 -pady 2
  }  

  grid rowconfigure    $f 100 -weight 1
  grid columnconfigure $f 100 -weight 1

  # Style
  grid $f2.tf -sticky news -padx 2
  grid rowconfigure    $f2 0 -weight 1
  grid columnconfigure $f2 0 -weight 1

  # General
  grid $tsmarker -sticky news -padx 2
  grid rowconfigure    $w 0 -weight 1
  grid columnconfigure $w 0 -weight 1
}


#/////////////////////////////////////////////////////////////////////////////
proc ::bayesGraph::CreateMarkers {tf axis} {
#
# PURPOSE:
#
# PARAMETERS:
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpOpt

  set f [$tf getframe]

  entry $f.e -width 18 -validate key\
     -vcmd {expr {[string is double %P] && [string length %P]<38}}
  Button $f.add -image [Bitmap::get downNext] -height 16 -width 16\
      -relief link -helptext [mc "AddMarkSel"]\
      -command [list ::bayesGraph::AddElto $f.e $f.sw.lb]
  ScrolledWindow $f.sw
  blistbox $f.sw.lb -selectmode extended -listvar ::bayesGraph::tmpOpt(var,$axis)\
    -width 20 -height 2 -breadonly true -bkeypress true -bparent $tf
  $f.sw setwidget $f.sw.lb

  bind $f.e     <Key-Return> "[list ::bayesGraph::AddElto $f.e $f.sw.lb]; break"
  bind $f.sw.lb <Button-1>   +[list focus -force %W]
  bind $f.sw.lb <Key-Delete> +[list ::bayesGraph::DelSelection %W]

  grid $f.e    $f.add  -sticky nw -padx 2
  grid $f.sw   -       -sticky news 
  grid rowconfigure    $f 1 -weight 1
  grid columnconfigure $f 0 -weight 1
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::AddElto {e lb} {
#  
#/////////////////////////////////////////////////////////////////////////////      
  
  regsub -all -- "( )+" [$e get] "" marker
  if [string equal $marker ""] {
    return
  }
  if  { ([lsearch -exact [$lb get 0 end] $marker] != -1) } {
    tk_messageBox -type ok -icon info -message [mc "Marker already inserted"] -title [mc "Warning"]
  } else {
    $lb insert end $marker
    $e delete 0 end
  }
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::DelSelection {w} {
#  
#/////////////////////////////////////////////////////////////////////////////      
  foreach i [lsort -integer -decreasing [$w curselection]] {
    $w delete $i
  }
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptCreateGrid {this w} {
#
#/////////////////////////////////////////////////////////////////////////////    
  variable tmpOpt
  TitleFrame $w.tf1 -text [mc "Grid options"]
  set f1 [$w.tf1 getframe]

  label $f1.lColor -text [mc "Color"]:
  bcolor $f1.fColor -variable ::bayesGraph::tmpOpt(var,grid,Color)
  label $f1.lDashes -text [mc "Style"]:
  ::Objects::ComboDashesCreate ::bayesGraph::tmpOpt(var,grid,Dashes) $f1.sbDashes
  label $f1.lLineWidth -text [mc "Line width"]:
  SpinBox $f1.sbLineWidth -range {1 4} -width 4 -justify right\
      -editable false -textvariable ::bayesGraph::tmpOpt(var,grid,LineWidth)
  checkbutton $f1.chMinor -text [mc "Draw for minor ticks"]\
     -highlightthickness 0 -pady 0 -padx 0 -variable ::bayesGraph::tmpOpt(var,grid,Minor)
  
  frame $f1.f
  label $f1.f.lMapX -text [mc "Map X to"]: 
  radiobutton $f1.f.x  -variable ::bayesGraph::tmpOpt(var,grid,MapX) -text [mc "X axis"]  -value x\
         -highlightthickness 0 -pady 0
  radiobutton $f1.f.x2 -variable ::bayesGraph::tmpOpt(var,grid,MapX) -text [mc "X2 axis"] -value x2\
         -highlightthickness 0 -pady 0
  radiobutton $f1.f.xN -variable ::bayesGraph::tmpOpt(var,grid,MapX) -text [mc "none"]    -value none\
         -highlightthickness 0 -pady 0
  label $f1.f.lMapY -text [mc "Map Y to"]: 
  radiobutton $f1.f.y  -variable ::bayesGraph::tmpOpt(var,grid,MapY) -text [mc "Y axis"]  -value y\
         -highlightthickness 0 -pady 0
  radiobutton $f1.f.y2 -variable ::bayesGraph::tmpOpt(var,grid,MapY) -text [mc "Y2 axis"] -value y2\
         -highlightthickness 0 -pady 0
  radiobutton $f1.f.yN -variable ::bayesGraph::tmpOpt(var,grid,MapY) -text [mc "none"]    -value none\
         -highlightthickness 0 -pady 0

  grid $f1.f.lMapX $f1.f.x $f1.f.x2 $f1.f.xN -sticky nw -padx 2
  grid $f1.f.lMapY $f1.f.y $f1.f.y2 $f1.f.yN -sticky nw -padx 2

  frame $f1.f1 -width 25 
  grid $f1.f1 $f1.lColor     $f1.fColor      $f1.lDashes $f1.sbDashes -sticky nw -padx 2
  grid ^      $f1.lLineWidth $f1.sbLineWidth $f1.chMinor -            -sticky nw -padx 2
  grid ^      $f1.f          -               -           -            -sticky nw -padx 2

  grid rowconfigure    $f1 2 -weight 1
  grid columnconfigure $f1 4 -weight 1
  grid columnconfigure $f1 2 -pad 20

  grid $w.tf1 -sticky news -padx 2 
  grid rowconfigure       $w 0 -weight 1
  grid columnconfigure    $w 0 -weight 1

}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptCreateLegend {this w} {
#
#/////////////////////////////////////////////////////////////////////////////    
  variable tmpOpt
  TitleFrame $w.tf1 -text [mc "Legend options"]
  set f1 [$w.tf1 getframe]

  checkbutton $f1.chHide -text [mc "Hide legend"] -padx 0 -pady 0\
    -highlightthickness 0 -variable ::bayesGraph::tmpOpt(var,legend,Hide)
  label $f1.lRows -text [mc "Rows"]:
  SpinBox $f1.sbRows -range {0 16} -width 4 -justify right\
      -editable false -textvariable ::bayesGraph::tmpOpt(var,legend,Rows)
  label $f1.lColumns -text [mc "Columns"]:
  SpinBox $f1.sbColumns -range {0 16} -width 4 -justify right\
      -editable false -textvariable ::bayesGraph::tmpOpt(var,legend,Columns)

  label $f1.lFont -text [mc "Font"]:
  bfont $f1.bFont -variable ::bayesGraph::tmpOpt(var,legend,Font)
  label $f1.lFg -text [mc "Color"]:
  bcolor $f1.fFg -variable ::bayesGraph::tmpOpt(var,legend,Fg)
  label $f1.lShadow -text [mc "Shadow"]:
  bcolor $f1.fShadow -variable ::bayesGraph::tmpOpt(var,legend,Shadow) \
    -label "[mc "Change Shadow"]:" -optmenu 1  

  label $f1.lBg -text [mc "Background"]:
  bcolor $f1.fBg -variable ::bayesGraph::tmpOpt(var,legend,Bg) -optmenu 1  
  set list {flat groove raised ridge solid sunken}
  foreach it $list { lappend values [mc $it] }
  label $f1.lRelief -text [mc Relief]:
  ComboBox $f1.cbRelief -width 10 -height 5 -editable false\
     -values $values -justify right\
     -modifycmd   [list ::Objects::ComboUnTranslate $f1.cbRelief $list\
      ::bayesGraph::tmpOpt(var,legend,Relief)]
  ::Objects::ComboSetIni  $f1.cbRelief $list ::bayesGraph::tmpOpt(var,legend,Relief)
  label $f1.lBd -text [mc "Border width"]:
  SpinBox $f1.sbBd -range {0 6} -width 4 -justify right\
      -editable false -textvariable ::bayesGraph::tmpOpt(var,legend,Bd)

  label $f1.lPosi -text [mc "Position"]:
  #set list {leftmargin rightmargin topmargin bottommargin plotarea}
  set list {left right top bottom plotarea}
  set values {}
  foreach it $list { lappend values [mc $it] }
  ComboBox $f1.cbPosi -width 8 -height 5 -editable false\
     -values $values -justify right\
     -modifycmd   [list ::Objects::ComboUnTranslate $f1.cbPosi $list\
      ::bayesGraph::tmpOpt(var,legend,Position)]
  ::Objects::ComboSetIni  $f1.cbPosi $list ::bayesGraph::tmpOpt(var,legend,Position)  
  label $f1.lAnchor -text [mc Anchor]:
  ::Objects::ComboPosCreate ::bayesGraph::tmpOpt(var,legend,Anchor) $f1.fAnchor
  checkbutton $f1.chRaised -text [mc "Raised"]\
     -highlightthickness 0 -pady 0 -padx 0 -variable ::bayesGraph::tmpOpt(var,legend,Raised)

  label $f1.lXPad -text [mc "X pad (left | right)"]
  label $f1.lYPad -text [mc "Y pad (top | bottom)"]

  label $f1.lIX  -text [mc "Internal"]:
  label $f1.lEX  -text [mc "External"]:
  label $f1.lIY  -text [mc "Internal"]:
  label $f1.lEY  -text [mc "External"]:
  SpinBox $f1.sbIPadXl -range {0 50} -width 3 -justify right\
      -editable false -textvariable ::bayesGraph::tmpOpt(var,legend,IPadXl)
  SpinBox $f1.sbIPadXr -range {0 50} -width 3 -justify right\
      -editable false -textvariable ::bayesGraph::tmpOpt(var,legend,IPadXr)
  SpinBox $f1.sbIPadYt -range {0 50} -width 3 -justify right\
      -editable false -textvariable ::bayesGraph::tmpOpt(var,legend,IPadYt)
  SpinBox $f1.sbIPadYb -range {0 50} -width 3 -justify right\
      -editable false -textvariable ::bayesGraph::tmpOpt(var,legend,IPadYb)
  SpinBox $f1.sbPadXl  -range {0 50} -width 3 -justify right\
      -editable false -textvariable ::bayesGraph::tmpOpt(var,legend,PadXl)
  SpinBox $f1.sbPadXr  -range {0 50} -width 3 -justify right\
      -editable false -textvariable ::bayesGraph::tmpOpt(var,legend,PadXr)
  SpinBox $f1.sbPadYt  -range {0 50} -width 3 -justify right\
      -editable false -textvariable ::bayesGraph::tmpOpt(var,legend,PadYt)
  SpinBox $f1.sbPadYb  -range {0 50} -width 3 -justify right\
      -editable false -textvariable ::bayesGraph::tmpOpt(var,legend,PadYb)
 
  frame $f1.f -width 15
  grid $f1.f $f1.chHide -          $f1.lRows   $f1.sbRows   - $f1.lColumns $f1.sbColumns - -sticky nw -padx 2
  grid ^     $f1.lFont  $f1.bFont  $f1.lFg     $f1.fFg      - $f1.lShadow  $f1.fShadow   - -sticky nw -padx 2
  grid ^     $f1.lBg    $f1.fBg    $f1.lRelief $f1.cbRelief - $f1.lBd      $f1.sbBd      - -sticky nw -padx 2
  grid ^     $f1.lPosi  $f1.cbPosi $f1.lAnchor $f1.fAnchor  - $f1.chRaised -             - -sticky nw -padx 2
  grid ^     $f1.lXPad  -  $f1.lIX $f1.sbIPadXl $f1.sbIPadXr $f1.lEX $f1.sbPadXl $f1.sbPadXr  -sticky nw -padx 2
  grid ^     $f1.lYPad  -  $f1.lIY $f1.sbIPadYt $f1.sbIPadYb $f1.lEY $f1.sbPadYt $f1.sbPadYb  -sticky nw -padx 2

  grid rowconfigure    $f1 5 -weight 1
  grid columnconfigure $f1 8 -weight 1
  grid columnconfigure $f1 2 -pad 20
  grid columnconfigure $f1 5 -pad 20
  grid columnconfigure $f1 8 -pad 20

  grid $w.tf1 -sticky news -padx 2 
  grid rowconfigure       $w 0 -weight 1
  grid columnconfigure    $w 0 -weight 1
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptCreateStyle {this w} {
#
#/////////////////////////////////////////////////////////////////////////////    
  variable tmpOpt
  
  # Main title
  TitleFrame $w.tf0 -text [mc "Main title"]
  set f0 [$w.tf0 getframe]
  ::bayesGraph::GrapOptCreateTitles \
    $this $f0 [list title font foreground shadow]
  
  grid $w.tf0    -sticky news -padx 2
  grid rowconfigure    $f0 0 -weight 1
  grid columnconfigure $f0 0 -weight 1
  
  # Exterior area
  TitleFrame $w.tf1 -text [mc "Exterior Area"]
  set f1 [$w.tf1 getframe]
  label $f1.lBg -text [mc "Background"]:
  bcolor $f1.fBg -variable ::bayesGraph::tmpOpt(var,background)
  set list {flat groove raised ridge solid sunken}
  foreach it $list { lappend values [mc $it] }
  label $f1.lRelief -text [mc Relief]:
  ComboBox $f1.cbRelief -width 10 -height 5 -editable false\
     -values $values -justify right\
     -modifycmd   [list ::Objects::ComboUnTranslate $f1.cbRelief $list\
      ::bayesGraph::tmpOpt(var,relief)]
  ::Objects::ComboSetIni  $f1.cbRelief $list ::bayesGraph::tmpOpt(var,relief)
  label $f1.lBd -text [mc "Border width"]:
  SpinBox $f1.sbBd -range {0 6} -width 4 -justify right\
      -editable false -textvariable ::bayesGraph::tmpOpt(var,areaborderwidth)
  label $f1.lMargins -text "[mc MARGINS]"
  label $f1.lDesc -text "(0=[mc auto])"
  label $f1.lLM -text [mc Left]:
  Entry $f1.eLM -validate key -vcmd {expr {[string is integer %P]}}\
     -width 10 -justify right -textvariable ::bayesGraph::tmpOpt(var,lm)
  label $f1.lRM -text [mc Right]:
  Entry $f1.eRM -validate key -vcmd {expr {[string is integer %P]}}\
     -width 10 -justify right -textvariable ::bayesGraph::tmpOpt(var,rm)
  label $f1.lTM -text [mc Top]:
  Entry $f1.eTM -validate key -vcmd {expr {[string is integer %P]}}\
     -width 10 -justify right -textvariable ::bayesGraph::tmpOpt(var,tm)
  label $f1.lBM -text [mc Bottom]:
  Entry $f1.eBM -validate key -vcmd {expr {[string is integer %P]}}\
     -width 10 -justify right -textvariable ::bayesGraph::tmpOpt(var,bm)

  frame $f1.f -width 25
  grid $f1.f $f1.lBg      $f1.fBg $f1.lRelief $f1.cbRelief $f1.lBd  $f1.sbBd  -sticky nw -padx 2
  grid ^     $f1.lMargins -       $f1.lLM     $f1.eLM      $f1.lRM  $f1.eRM   -sticky nw -padx 2
  grid ^     $f1.lDesc    -       $f1.lTM     $f1.eTM      $f1.lBM  $f1.eBM   -sticky nw -padx 2

  grid rowconfigure    $f1 2 -weight 1
  grid columnconfigure $f1 6 -weight 1
  grid columnconfigure $f1 2 -pad 20
  grid columnconfigure $f1 4 -pad 20
  grid columnconfigure $f1 6 -pad 20

  TitleFrame $w.tf2 -text [mc "Plot Area"]
  set f2 [$w.tf2 getframe]

  label $f2.lBg -text [mc "Background"]:
  bcolor $f2.fBg -variable ::bayesGraph::tmpOpt(var,plotbackground)
  set list {flat groove raised ridge solid sunken}
  set values {}
  foreach it $list { lappend values [mc $it] }
  label $f2.lRelief -text [mc Relief]:
  ComboBox $f2.cbRelief -width 10 -height 5 -editable false\
     -values $values -justify right\
     -modifycmd   [list ::Objects::ComboUnTranslate $f2.cbRelief $list\
      ::bayesGraph::tmpOpt(var,plotrelief)]
  ::Objects::ComboSetIni  $f2.cbRelief $list ::bayesGraph::tmpOpt(var,plotrelief)
  label $f2.lBd -text [mc "Border width"]:
  SpinBox $f2.sbBd -range {0 6} -width 4 -justify right\
      -editable false -textvariable ::bayesGraph::tmpOpt(var,plotborderwidth)
  label $f2.lPad -text [mc "PAD"]
  label $f2.lPadXl -text [mc Left]:
  SpinBox $f2.sbPadXl -range {0 20} -width 3 -justify right\
      -editable false -textvariable ::bayesGraph::tmpOpt(var,plotpadxl)
  label $f2.lPadXr -text [mc Right]:
  SpinBox $f2.sbPadXr -range {0 20} -width 3 -justify right\
      -editable false -textvariable ::bayesGraph::tmpOpt(var,plotpadxr)
  label $f2.lPadYt -text [mc Top]:
  SpinBox $f2.sbPadYt -range {0 20} -width 3 -justify right\
      -editable false -textvariable ::bayesGraph::tmpOpt(var,plotpadyt)
  label $f2.lPadYb -text [mc Bottom]:
  SpinBox $f2.sbPadYb -range {0 20} -width 3 -justify right\
      -editable false -textvariable ::bayesGraph::tmpOpt(var,plotpadyb)

  checkbutton $f2.chInvertxy -text [mc "Invert X-Y axis"]\
     -highlightthickness 0 -pady 0 -padx 0 -variable ::bayesGraph::tmpOpt(var,invertxy)

  frame $f2.f -width 25
  grid $f2.f $f2.lBg   $f2.fBg $f2.lRelief $f2.cbRelief $f2.lBd    $f2.sbBd    -sticky nw -padx 2
  grid ^     $f2.lPad  -       $f2.lPadXl  $f2.sbPadXl  $f2.lPadXr $f2.sbPadXr -sticky nw -padx 2
  grid ^     ^         ^       $f2.lPadYt  $f2.sbPadYt  $f2.lPadYb $f2.sbPadYb -sticky nw -padx 2
  grid ^     $f2.chInvertxy - - - - - -sticky nw -padx 2

  grid rowconfigure    $f2 3 -weight 1
  grid columnconfigure $f2 6 -weight 1
  grid columnconfigure $f2 2 -pad 20
  grid columnconfigure $f2 4 -pad 20
  grid columnconfigure $f2 6 -pad 20

  grid $w.tf0 -sticky news -padx 2
  grid $w.tf1 -sticky news -padx 2 
  grid $w.tf2 -sticky news -padx 2
  grid rowconfigure       $w 0 -weight 1
  grid rowconfigure       $w 1 -weight 2
  grid rowconfigure       $w 2 -weight 3
  grid columnconfigure    $w 0 -weight 1
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::GrapOptGet {this} {
#
# PURPOSE: Initializes all options variables and stores them into the
#          temporary 'tmpOpt' array. 
#
# PARAMETERS:
#   this   -> Instance of BayesGraph
#
#/////////////////////////////////////////////////////////////////////////////
  GrapOptGetAxis    $this
  GrapOptGetTitles  $this
  GrapOptGetLines   $this
#  GrapOptGetData    $this
  GrapOptGetMarkers $this
  GrapOptGetGrid    $this
  GrapOptGetLegend  $this
  GrapOptGetStyle   $this
  #GrapOptSet $this
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::GrapOptGetAxis {this} {
#
# PURPOSE: Initializes axis options variables and stores them into the
#          temporary 'tmpOpt' array. 
#
# PARAMETERS:
#   this   -> Instance of BayesGraph
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::options opt
  variable tmpOpt
  set gr $data(gr,active)
  set g [getActiveGraph $this]
  
  # opciones recogidas del grafico
  foreach axis {x y x2 y2} {
    set tmpOpt(var,${axis}AxisMin)     [$g axis cget $axis -min]
    set tmpOpt(var,${axis}AxisMax)     [$g axis cget $axis -max]
    set tmpOpt(var,${axis}TicksWidth)  [$g axis cget $axis -linewidth]
    set tmpOpt(var,${axis}TicksShow)   [$g axis cget $axis -showticks]
    set tmpOpt(var,${axis}TicksStep)   [$g axis cget $axis -stepsize]
    set tmpOpt(var,${axis}TicksFont)   [$g axis cget $axis -tickfont]
    set tmpOpt(var,${axis}TicksRotate) [$g axis cget $axis -rotate]
    set tmpOpt(var,${axis}TicksColor)  [$g axis cget $axis -color]
    set tmpOpt(var,${axis}TicksShadow) [lindex [$g axis cget $axis -tickshadow] 0]
    set tmpOpt(var,${axis}TicksLength) [$g axis cget $axis -ticklength]
    set tmpOpt(var,${axis}TicksSubdivisions) [$g axis cget $axis -subdivisions]
  }

  #Estas opciones deben venir del ini
  foreach axis {x y x2 y2} {
    set tmpOpt(var,${axis}AxisAll)        $opt(gr,$gr,${axis}AxisAll)
    set tmpOpt(var,${axis}AxisShow)       $opt(gr,$gr,${axis}AxisShow)
    set tmpOpt(var,${axis}TicksType)      $opt(gr,$gr,${axis}TicksType)
    set tmpOpt(var,${axis}TicksTotal)     $opt(gr,$gr,${axis}TicksTotal)
    set tmpOpt(var,${axis}TicksStep)      $opt(gr,$gr,${axis}TicksStep)
    set tmpOpt(var,${axis}FormatNumber)   $opt(gr,$gr,${axis}FormatNumber)
    set tmpOpt(var,${axis}FormatPercent)  $opt(gr,$gr,${axis}FormatPercent)
  }
  
  # comprobamos valor maximo y minimo con opcion "Todos los valores"
  foreach axis {x y x2 y2} {
    if {$tmpOpt(var,${axis}AxisAll)} {
      set values [::bayesGraph::GetLimitsValueAxis $this $axis]
      if {![string eq $tmpOpt(var,${axis}AxisMin) [lindex $values 0]] || \
            ![string eq $tmpOpt(var,${axis}AxisMin) [lindex $values 0]]} {
        set tmpOpt(var,${axis}AxisAll) 0
      }
    }
  }
  # Ajust axis chart of values
  set tmpOpt(var,selffit)  $opt(gr,$gr,selffit)
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptGetTitles {this} {
#
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::options opt
  variable tmpOpt
  set gr $data(gr,active)
  set g [getActiveGraph $this]

  set tmpOpt(var,title)          [$g cget -title]
  set tmpOpt(var,font)           [$g cget -font]
  set tmpOpt(var,foreground)     [$g cget -foreground]
  set tmpOpt(var,shadow)         [lindex [$g cget -shadow] 0]

  foreach pre {x y x2 y2} {
    set tmpOpt(var,${pre}AxisTitle) $opt(gr,$gr,${pre}AxisTitle)
    foreach post {Color Font Shadow} {
      set tmpOpt(var,${pre}Axis${post}) $opt(gr,$gr,${pre}Axis${post})
    }
  }
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptGetLines {this} {
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::options opt
  variable tmpOpt

  variable optLinesAll
  
  set gr $data(gr,active)
  set g [getActiveGraph $this]

  # options general
  set tmpOpt(var,barmode) $opt(gr,$gr,barmode)
  # optiones lines & bar
  foreach key $optLinesAll {
    set tmpOpt(var,$key) $opt(gr,$gr,$key)  
  }
  # names lines
  set tmpOpt(var,lstnames) $opt(gr,$gr,lstnames)
}


#/////////////////////////////////////////////////////////////////////////////
proc ::bayesGraph::ShowOptLines {this nline} {
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::widgets widgets
  variable tmpOpt
  variable optLines
  
  # options general
  set idx [expr $nline - 1]
  foreach key $optLines {
    set tmpOpt(var,${key}_) [lindex $tmpOpt(var,$key) $idx]
  }
  # axis
  set xy [lindex $tmpOpt(var,xyMappedAxis) $idx]
  set tmpOpt(var,xMappedAxis_) [lindex $xy 0]
  set tmpOpt(var,yMappedAxis_) [lindex $xy 1]
  # Name line. Note: label -> 3
  foreach id [$widgets(lines,lst) get] {
    if {[string equal [$widgets(lines,lst) get $id id] $nline]} {
      set idLb $id
    }
  }  
  set tmpOpt(var,linelabels_) [lindex [$widgets(lines,lst) get $idLb label] 0]
}


#/////////////////////////////////////////////////////////////////////////////
proc ::bayesGraph::ShowSelectLines {this} {
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::widgets widgets
  variable tmpOpt
  
  # get info now
  set linSel {}
  set idSel  [$widgets(lines,lst) selection get]
  foreach id $idSel {
    lappend linSel [$widgets(lines,lst) get $id id]
  }
  
  # check exists selection prior ?
  if {[info exists data(lines,selectant)] && \
      [llength $data(lines,selectant)]} {
    ::bayesGraph::GrapOptSetLines $this $data(lines,selectant)
  }

  # select prior
  set data(lines,selectant)   $linSel

  if {[llength data(lines,selectant)]} {
    ::bayesGraph::ShowOptLines $this $linSel
  }  
}


#/////////////////////////////////////////////////////////////////////////////
proc ::bayesGraph::GrapOptSetLines {this lstlines} {
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::widgets widgets
  variable tmpOpt
  variable optLines

  foreach idx $lstlines {
    set i [expr $idx - 1]
    # options general
    foreach key $optLines {
      set tmpOpt(var,$key) \
        [lreplace $tmpOpt(var,$key) $i $i $tmpOpt(var,${key}_)]
    }
    # axis
    set tmpOpt(var,xyMappedAxis) [lreplace $tmpOpt(var,xyMappedAxis) $i $i \
      "$tmpOpt(var,xMappedAxis_) $tmpOpt(var,yMappedAxis_)"]

    # Line name
    foreach id [$widgets(lines,lst) get] {
      if {[string equal [$widgets(lines,lst) get $id id] $idx]} {
        set idLb $id
      }
    }
    if {[string length $tmpOpt(var,linelabels_)]} {
      set name $tmpOpt(var,linelabels_)
    } else {
      set name [$widgets(lines,lst) get $idLb caption]
    }
    $widgets(lines,lst) set $idLb [list $name] label
  }
  
  # new List Names with order
  set tmpOpt(var,lstnames) {}
  foreach id [$widgets(lines,lst) get] {
    lappend tmpOpt(var,lstnames) [$widgets(lines,lst) get $id]
  }
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptGetMarkers {this} {
#
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::options opt
  variable tmpOpt
  set gr $data(gr,active)

  #Estas opciones deben venir del ini
  set tmpOpt(var,xMarkers)     $opt(gr,$gr,xMarkers)
  set tmpOpt(var,yMarkers)     $opt(gr,$gr,yMarkers)
  set tmpOpt(var,xMarkersIn)   $opt(gr,$gr,xMarkersIn)
  set tmpOpt(var,yMarkersIn)   $opt(gr,$gr,yMarkersIn)
  set tmpOpt(var,x2Markers)    $opt(gr,$gr,x2Markers)
  set tmpOpt(var,y2Markers)    $opt(gr,$gr,y2Markers)
  set tmpOpt(var,x2MarkersIn)  $opt(gr,$gr,x2MarkersIn)
  set tmpOpt(var,y2MarkersIn)  $opt(gr,$gr,y2MarkersIn)

  foreach axis {x x2 y y2} {
    set i 0
    foreach color $opt(gr,$gr,${axis}MarkersColorsInEach) {
      set tmpOpt(var,${axis}MarkerColorInEach$i) $color
      incr i
    }
    set i 0
    foreach width $opt(gr,$gr,${axis}MarkersWidthsInEach) {
      set tmpOpt(var,${axis}MarkerWidthInEach$i) $width
      incr i
    }
    set i 0
    foreach dashes $opt(gr,$gr,${axis}MarkersDashesInEach) {
      set tmpOpt(var,${axis}MarkerDasheInEach$i) $dashes
      incr i
    }
    set i 0
    foreach under $opt(gr,$gr,${axis}MarkersUndersInEach) {
      set tmpOpt(var,${axis}MarkerUnderInEach$i) $under
      incr i
    }

  }
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptGetGrid {this} {
#
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::options opt
  set gr $data(gr,active)
  set g [getActiveGraph $this]
  variable tmpOpt

  #  Al coger las opciones de opt, perdemos la opcion de que las opciones
  # muestren los cambios hechos desde otro lugar (TkConsole, p.ejemplo)
  # pero bueno...
  foreach it {Color Dashes LineWidth Minor MapX MapY} {
    set tmpOpt(var,grid,$it) $opt(gr,$gr,grid,$it)
  }
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptGetLegend {this} {
#
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::options opt
  set gr $data(gr,active)
  set g [getActiveGraph $this]
  variable tmpOpt

  set tmpOpt(var,legend,Hide)     [$g legend cget -hide]
  set tmpOpt(var,legend,Bg)       [$g legend cget -bg]
  set tmpOpt(var,legend,Bd)       [$g legend cget -bd]
  set tmpOpt(var,legend,Fg)       [$g legend cget -fg]
  set tmpOpt(var,legend,Font)     [$g legend cget -font]
  set tmpOpt(var,legend,Shadow)   [lindex [$g legend cget -shadow] 0]
  set tmpOpt(var,legend,Position) $opt(gr,$gr,legend,Position)
  set tmpOpt(var,legend,Anchor)   [$g legend cget -anchor]
  set tmpOpt(var,legend,Relief)   [$g legend cget -relief]
  set tmpOpt(var,legend,Raised)   [$g legend cget -raised]
  set tmpOpt(var,legend,IPadXl)   [lindex [$g legend cget -ipadx] 0]
  set tmpOpt(var,legend,IPadXr)   [lindex [$g legend cget -ipadx] 1]
  set tmpOpt(var,legend,IPadYt)   [lindex [$g legend cget -ipady] 0]
  set tmpOpt(var,legend,IPadYb)   [lindex [$g legend cget -ipady] 1]
  set tmpOpt(var,legend,PadXl)    [lindex [$g legend cget -padx] 0]
  set tmpOpt(var,legend,PadXr)    [lindex [$g legend cget -padx] 1]
  set tmpOpt(var,legend,PadYt)    [lindex [$g legend cget -pady] 0]
  set tmpOpt(var,legend,PadYb)    [lindex [$g legend cget -pady] 1]
  set tmpOpt(var,legend,Rows)     [$g legend cget -rows]
  set tmpOpt(var,legend,Columns)  [$g legend cget -columns]
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptGetStyle {this} {
#
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::options opt
  variable tmpOpt
  set gr $data(gr,active)
  set g [getActiveGraph $this]

  foreach it {background relief lm rm tm bm} {
    set tmpOpt(var,$it)     [$g cget -$it]
  }
  foreach it {plotbackground plotrelief plotborderwidth} {
    set tmpOpt(var,$it)     [$g cget -$it]
  }
  
  set tmpOpt(var,areaborderwidth)  [$g cget -borderwidth]

  set tmpOpt(var,invertxy)  $opt(gr,$gr,invertxy)
  set tmpOpt(var,plotpadxl) [lindex [$g cget -plotpadx] 0]
  set tmpOpt(var,plotpadxr) [lindex [$g cget -plotpadx] 1]
  set tmpOpt(var,plotpadyt) [lindex [$g cget -plotpady] 0]
  set tmpOpt(var,plotpadyb) [lindex [$g cget -plotpady] 1]
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::GrapOptSet {this} {
#
# PURPOSE: Commits all options values from the temporary 'tmpOpt' array to
#          other structure.
#
# PARAMETERS:
#   this   -> Instance of BayesGraph
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::options options

  variable tmpOpt

  set gr $data(gr,active)
 
  # options axis
  foreach axis {x x2 y y2} {
    set tmpOpt(var,${axis}MarkersColorsInEach) {}
    set tmpOpt(var,${axis}MarkersWidthsInEach) {}
    set tmpOpt(var,${axis}MarkersDashesInEach) {}
    set tmpOpt(var,${axis}MarkersUndersInEach) {}
  }
  # options axis list
  foreach axis {x x2 y y2} {
    for {set i 0} {$i<5} {incr i} {
      lappend tmpOpt(var,${axis}MarkersColorsInEach) \
        $tmpOpt(var,${axis}MarkerColorInEach$i)
      lappend tmpOpt(var,${axis}MarkersWidthsInEach) \
        $tmpOpt(var,${axis}MarkerWidthInEach$i)
      lappend tmpOpt(var,${axis}MarkersDashesInEach) \
        $tmpOpt(var,${axis}MarkerDasheInEach$i)
      lappend tmpOpt(var,${axis}MarkersUndersInEach) \
        $tmpOpt(var,${axis}MarkerUnderInEach$i)
    }
  }

  regsub -all var, [array names ::bayesGraph::tmpOpt var,*] "" optNames

  foreach it $optNames {
    set options(gr,$gr,$it) $tmpOpt(var,$it)
  }
  #Ya no se usa
  set options(gr,${gr}_INIT)  YES
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::GrapOptInit {this} {
#
# PURPOSE:
#
# PARAMETERS:
#   this   -> Instance of BayesGraph
#
#/////////////////////////////////////////////////////////////////////////////
upvar \#0 ${this}::data data
  variable tmpOpt
  set gr $data(gr,active)
  set values [::bayesGraph::GetLimitsValueAxis $this x]
  if {[string is double $tmpOpt(var,xyAxisMin)] && \
      $tmpOpt(var,xyAxisMin) <= [lindex $values 0]} {
    $tmpOpt(widget,xyAxisMin) setvalue  first
    set tmpOpt(var,xyAxisMin) [$tmpOpt(widget,xyAxisMin) getvalue]
  }
  if {[string is double $tmpOpt(var,xyAxisMax)] && \
      $tmpOpt(var,xyAxisMax) >= [lindex $values 1]} {
    $tmpOpt(widget,xyAxisMax) setvalue  last
    set tmpOpt(var,xyAxisMax) [$tmpOpt(widget,xyAxisMax) getvalue]
  }
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptApplyAll {this} {
#
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 ${this}::data data
  set old_active $data(gr,active)
  for {set gr 0} {$gr<$data(gr,numGraph)} {incr gr} {
    set data(gr,active) $gr
    GrapOptApply $this
  }
  set data(gr,active) $old_active
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptApply {this} {
#
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::options opt

  set gr $data(gr,active)
  set g [getActiveGraph $this]
  variable tmpOpt
  upvar \#0 ${this}::widgets widgets

  # leemos los valores actuales de los ejes activos en el formulario
  ShowAxisSelectXY $this
  # leemos los valores actuales de la linea marcada
  set linSel {}
  foreach id [$widgets(lines,lst) selection get] {
    lappend linSel [$widgets(lines,lst) get $id id]
  }  
  GrapOptSetLines         $this $linSel
  # options
  GrapOptApplyMappedLines $this ::bayesGraph::tmpOpt var
  #Debe ir antes de calcular el step pq cambia rotate
  GrapOptApplyTicksStyle $this ::bayesGraph::tmpOpt var
  GrapOptApplyAxisPos    $this ::bayesGraph::tmpOpt var
  GrapOptApplyAxisTicks  $this ::bayesGraph::tmpOpt var
  GrapOptApplyTitles     $this ::bayesGraph::tmpOpt var

  GrapOptApplyLnesAll    $this ::bayesGraph::tmpOpt var
  
  GrapOptApplyMarkers    $this ::bayesGraph::tmpOpt var
  GrapOptApplyMarkersIn  $this ::bayesGraph::tmpOpt var

  GrapOptApplyGrid       $this ::bayesGraph::tmpOpt var,grid
  GrapOptApplyLegend     $this ::bayesGraph::tmpOpt var,legend
  GrapOptApplyStyle      $this ::bayesGraph::tmpOpt var

  GrapOptApplyOrder       $this ::bayesGraph::tmpOpt var
  
  # Show Line
  if {![info exists opt(gr,$gr,linehides)] || \
      ![info exists tmpOpt(::bayesGraph::tmpOpt,linehides)] || \
      ![string equal $opt(gr,$gr,linehides) $tmpOpt(::bayesGraph::tmpOpt,linehides)]} {
    GrapOptApplyLineDefault $this ::bayesGraph::tmpOpt var hide linehides
  }  
  

  if [info exists ${this}::data(proc,OnOptionApply)] {
    $data(proc,OnOptionApply) $this
  }

  #Save options after apply
  GrapOptSet $this
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::GrapOptApplyAxisPos {this var pre} {
#
# PURPOSE:
#
# PARAMETERS:
#   this   -> Instance of BayesGraph
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 $var ary

  upvar \#0 ${this}::data data
#  variable tmpOpt
  set gr $data(gr,active)
  set g [getActiveGraph $this]
  foreach  axis {x y x2 y2} {
    # si esta visible
    if {![$g axis cget $axis -hide]} {
      set AXIS  [string toupper $axis]
      set AXI   [string index   $AXIS 0]
      if {$ary(${pre},${axis}AxisAll)} {
        set values [::bayesGraph::GetLimitsValueAxis $this $axis]
        set vmin   [lindex $values 0]
        set vmax   [lindex $values 1]
      } else  {
        set vmin $ary(${pre},${axis}AxisMin)
        set vmax $ary(${pre},${axis}AxisMax)
      }
      if {[info exists data(gr,$gr,OnGet${AXI}Ticks)]} {
        set lst [$data(gr,$gr,OnGet${AXI}Ticks) $this]
        if {![string is double $vmin]} {
          set vmin [lsearch $lst $vmin]
        }
        if {![string is double $vmax]} {
          set vmax [lsearch $lst $vmax]
        }
      }
      if {$vmin<$vmax} {
        ::bayesGraph::GrapOptApplyAxisLimits $this $axis $vmin $vmax
      }
    }
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::GrapOptApplyAxisTicks {this var pre} {
#
# PURPOSE:
#
# PARAMETERS:
#   this -> Instance of BayesGraph
#   var  ->
#   pre  ->
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 $var ary
  set g [getActiveGraph $this]
  # Axis
  foreach axis {x x2 y y2} {
    set AXI  [string toupper [string index $axis 0]]  
    if {![$g axis cget $axis -hide]} {
      switch $ary(${pre},${axis}TicksType) {
        total {
          set step \
            [Compute${AXI}AxisStep $this \
                                   $ary(${pre},${axis}TicksTotal) \
                                   $axis]
        }
        each {
          if {[info exists data(proc,OnFixStep${AXI})]} {
            set min  [$g axis cget $axis -min]
            set max  [$g axis cget $axis -max]
            set step [$data(proc,OnFixStep{AXI}) \
                        $this $min $max $ary(${pre},${axis}TicksStep)]         
          } else  {
            set step $ary(${pre},${axis}TicksStep)
          }
        }
        auto {
          set nTicks [Calculate${AXI}AxisNumTicks $this         $axis]
          set step   [Compute${AXI}AxisStep       $this $nTicks $axis]
        }
        default {set step 1}
      }
      # Verifico si step es demasiado pequeño para los límites del eje
      set max [$g axis cget $axis -max]
      set min [$g axis cget $axis -min]
      set max [expr { $max eq "" ? 1 : $max }]
      set min [expr { $min eq "" ? 0 : $min }]
      set step [expr { $step eq "" ? 1 : $step }]
      if {($step==0) || [expr (($max-$min)/$step) < 1000 ]} {
        $g axis configure $axis -stepsize $step
      }
    }
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::bayesGraph::GrapOptApplyAxisAuto {this} {
#
# PURPOSE:
#
# PARAMETERS:
#   this -> Instance of BayesGraph
#
#/////////////////////////////////////////////////////////////////////////////
  update
  set g [getActiveGraph $this]
  foreach axis {x x2y y2} {
    set AXI [string toupper [string index $axis 0]]
    set nTicks [Calculate${AXI}AxisNumTicks $this         $axis]
    set step   [Compute${AXI}AxisStep       $this $nTicks $axis]
    $g axis configure $axis -stepsize $step    
  }
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptApplyTicksStyle {this var pre} {
#
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 $var ary
  upvar \#0 ${this}::data data

  set g [getActiveGraph $this]
  foreach pref {x x2 y y2} {
    # command axis  
    set cmd {}
    if {[info exists ${this}::data(proc,${pref}AxisCommand)]} {
      set cmd "$data(proc,${pref}AxisCommand)"
    } else {
      set cmd [list ::bayesGraph::FormatNumberAxis \
               $ary(${pre},${pref}FormatNumber) \
               $ary(${pre},${pref}FormatPercent)]
    }
    # configuration
    $g axis configure $pref \
        -tickfont     $ary(${pre},${pref}TicksFont)\
        -rotate       $ary(${pre},${pref}TicksRotate)\
        -color        $ary(${pre},${pref}TicksColor)\
        -ticklength   $ary(${pre},${pref}TicksLength)\
        -tickshadow   $ary(${pre},${pref}TicksShadow)\
        -subdivisions $ary(${pre},${pref}TicksSubdivisions)\
        -linewidth    $ary(${pre},${pref}TicksWidth)\
        -showticks    $ary(${pre},${pref}TicksShow)\
        -hide         [expr !$ary(${pre},${pref}AxisShow)]\
        -command      $cmd
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::FormatNumberAxis {forNum forPer bltg tick} {
#
# PURPOSE: Aplica un formato a los valores del eje Y.
#
# PARAMETERS:
#   forNum  -> format number
#   forPer  -> format percent
#   gltg    -> the pathname of the widget
#   tick    -> current the numeric value of the tick
#
#/////////////////////////////////////////////////////////////////////////////

  if {[string length $forNum]} {
      if {$forPer} {
      set valAux [expr $tick * 100]
    } else {
      set valAux $tick
    }
    if  {![catch {set nf [format $forNum $valAux]} res]} {
      return $nf
    } else {
      return $tick
    }
  } else  {
    return $tick
  }
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptApplyTitles {this var pre} {
#
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 $var ary

  set g [getActiveGraph $this]
  if [info exist ${var}(${pre},title)] {
    $g configure -title           "$ary(${pre},title)"
  }
  $g configure -font         $ary(${pre},font)\
               -foreground   $ary(${pre},foreground)\
               -shadow       $ary(${pre},shadow)

  foreach pref {x y x2 y2} {
    $g axis configure $pref -title       $ary(${pre},${pref}AxisTitle)\
                            -titlefont   $ary(${pre},${pref}AxisFont)\
                            -titlecolor  $ary(${pre},${pref}AxisColor)\
                            -titleshadow $ary(${pre},${pref}AxisShadow)
  }
}

#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptApplyLnesAll {this var pre} {
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::options opt
  upvar \#0 ${this}::data data
  variable tmpOpt
  variable optLinesData
  variable optLinesSymbol
  set gr $data(gr,active)
  set okAll 0
  
  # Line Type
  if {![info exists opt(gr,$gr,linetypes)] || \
      ![info exists tmpOpt(var,linetypes)] || \
      ![string equal $opt(gr,$gr,linetypes) $tmpOpt(var,linetypes)]} {
    GrapOptApplyLineTypes $this $var $pre
    set okAll 1
  }  
  # Colors
  if {($okAll) || \
      ![info exists opt(gr,$gr,colors)] || \
      ![info exists tmpOpt(var,colors)] || \
      ![string equal $opt(gr,$gr,colors) $tmpOpt(var,colors)]} {
    GrapOptApplyColors $this $var $pre
  }
  # Widths  
  if {($okAll) || \
      ![info exists opt(gr,$gr,linewidth)] || \
      ![info exists tmpOpt(var,linewidth)] || \
      ![string equal $opt(gr,$gr,linewidth) $tmpOpt(var,linewidth)]} {
    GrapOptApplyLineWidth $this $var $pre
  }
  # Dashes 
  if {($okAll) || \
      ![info exists opt(gr,$gr,dashes)] || \
      ![info exists tmpOpt(var,dashes)] || \
      ![string equal $opt(gr,$gr,dashes) $tmpOpt(var,dashes)]} {
    GrapOptApplyDashes $this $var $pre
  }
  # Smooths
  if {($okAll) || \
      ![info exists opt(gr,$gr,smooth)] || \
      ![info exists tmpOpt(var,smooth)] || \
      ![string equal $opt(gr,$gr,smooth) $tmpOpt(var,smooth)]} {
    GrapOptApplySmooth $this $var $pre
  }
  # Bar mode
  if {($okAll) || \
      ![info exists opt(gr,$gr,barmode)] || \
      ![info exists tmpOpt(var,barmode)] || \
      ![string equal $opt(gr,$gr,barmode) $tmpOpt(var,barmode)]} {
    GrapOptApplyBarMode $this $var $pre
  }
  # Bar Width
  if {($okAll) || \
      ![info exists opt(gr,$gr,barwidth)] || \
      ![info exists tmpOpt(var,barwidth)] || \
      ![string equal $opt(gr,$gr,barwidth) $tmpOpt(var,barwidth)]} {
    GrapOptApplyBarWidth $this $var $pre
  }
  # Fill. Depent type line and type fill
  GrapOptApplyFill $this $var $pre
  # Datas
  set ok 0
  set optData $optLinesData
  foreach key $optData {
    if {![info exists opt(gr,$gr,$key)] || \
        ![info exists tmpOpt(var,$key)] || \
        ![string equal $opt(gr,$gr,$key) $tmpOpt(var,$key)]} {
      set ok 1
    }
  }  
  if {$ok} { GrapOptApplyShowValues $this $var $pre }
  # Symbol
  set ok 0
  set optSymbol $optLinesSymbol
  foreach key $optSymbol {
    if {![info exists opt(gr,$gr,$key)] || \
        ![info exists tmpOpt(var,$key)] || \
        ![string equal $opt(gr,$gr,$key) $tmpOpt(var,$key)]} {
      set ok 1
    }
  }
  if {$ok} { GrapOptApplyShowSymbols $this $var $pre }
  # Line Name
  if {![info exists opt(gr,$gr,lstnames)] || \
      ![info exists tmpOpt(var,lstnames)] || \
      ![string equal $opt(gr,$gr,lstnames) $tmpOpt(var,lstnames)]} {
    GrapOptApplyLineName $this $var $pre
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::bayesGraph::GrapOptApplyLineName {this var pre} {
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 $var ary
  set g [getActiveGraph $this]

  if {[info exists ary($pre,lstnames)]} {
    foreach it $ary($pre,lstnames) {
      $g element configure [lindex [lindex $it 1] 0] -label [lindex $it 3]
    }
  }  
}


#/////////////////////////////////////////////////////////////////////////////
proc ::bayesGraph::GrapOptApplyLineDefault {this var pre opt key} {
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 $var ary
  set g [getActiveGraph $this]

  if {[info exists ary($pre,$key)]} {
    set j 0
    set lstId [GrapOptGetIdLines $this $var $pre]
    foreach it [GrapOptGetNamesLines $this $var $pre] {
      set i [expr [lindex $lstId $j] - 1]
      foreach e $it {
        $g element configure $e -$opt [lindex $ary($pre,$key) $i]
      }
      incr j
    }
  }  
}

 
#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptApplyLineWidth {this var pre} {
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 $var ary
  set g [getActiveGraph $this]

  set j 0
  set lstId [GrapOptGetIdLines $this $var $pre]
  foreach it [GrapOptGetNamesLines $this $var $pre] {
    set i [expr [lindex $lstId $j] - 1]
    foreach e $it {
      if {[string equal [$g element type $e] "LineElement"]} {
        $g element configure $e -linewidth [lindex $ary($pre,linewidth) $i]
      } else  {
        $g element configure $e -borderwidth [lindex $ary($pre,borderwidth) $i]
      }
    }
    incr j
  }
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptApplyBarMode {this var pre} {
#
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 $var ary
  set g [getActiveGraph $this]
  $g configure -barmode $ary(${pre},barmode)
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptApplyBarRelief {this var pre} {
#
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 $var ary
  set g [getActiveGraph $this]
    
  set j 0
  set lstId [GrapOptGetIdLines $this $var $pre]
  foreach it [GrapOptGetNamesLines $this $var $pre] {
    set i [expr [lindex $lstId $j] - 1]
    foreach e $it {
      if {[string equal [$g element type $e] "BarElement"]} {
        $g element configure $it -relief [lindex $ary($pre,barrelief) $i]
      }
    }
    incr j
  }
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptApplyDashes {this var pre} {
#
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 $var ary
  set g [getActiveGraph $this]

  set j 0
  set lstId [GrapOptGetIdLines $this $var $pre]
  foreach it [GrapOptGetNamesLines $this $var $pre] {
    set i [expr [lindex $lstId $j] - 1]
    foreach e $it {
      if {[string equal [$g element type $e] "LineElement"]} {
        $g element configure $e -dashes [lindex $ary($pre,dashes) $i]
      }
    }
    incr j
  }  
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptApplySmooth {this var pre} {
#
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 $var ary
  set g [getActiveGraph $this]

  set j 0
  set lstId [GrapOptGetIdLines $this $var $pre]
  foreach it [GrapOptGetNamesLines $this $var $pre] {
    set i [expr [lindex $lstId $j] - 1]
    foreach e $it {
      if {[string equal [$g element type $e] "LineElement"]} {
        $g element configure $e -smooth [lindex $ary($pre,smooth) $i]
      }
    }
    incr j
  }   
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptApplyBarWidth {this var pre} {
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 $var ary
  set g [getActiveGraph $this]
  
  set j 0
  set lstId [GrapOptGetIdLines $this $var $pre]
  foreach it [GrapOptGetNamesLines $this $var $pre] {
    set i [expr [lindex $lstId $j] - 1]
    foreach e $it {
      if {[string equal [$g element type $e] "BarElement"]} {
        $g element configure $e -barwidth [lindex $ary($pre,barwidth) $i]
      }
    }  
    incr j
  }
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptApplyOrder {this var pre} {
#
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 $var ary
  set g [getActiveGraph $this]
  $g element show \
    [lreverse [join [lindex [ltraspose $ary(${pre},lstnames)] 1]]]
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptApplyLineTypes {this var pre} {
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 $var ary
  set g [getActiveGraph $this]

  set j 0
  set lstId [GrapOptGetIdLines $this $var $pre]
  foreach it [GrapOptGetNamesLines $this $var $pre] {
    set i [expr [lindex $lstId $j] - 1]
    foreach e $it {
      if {![string equal [$g element type $e] [lindex $ary(${pre},linetypes) $i]]} {
        ::bayesGraph::GrapOptChangeType $this $e $e
      }
    }  
    incr j
  }
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptApplyColors {this var pre} {
#
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 $var ary
  set g [getActiveGraph $this]

  set j 0
  set lstId [GrapOptGetIdLines $this $var $pre]
  foreach it [GrapOptGetNamesLines $this $var $pre] {
    set i [expr [lindex $lstId $j] - 1]
    foreach e $it {
      if {[string equal [$g element type $e] "LineElement"]} {
        $g element configure $e -color [lindex $ary($pre,colors) $i]
      } else  {
        $g element configure $e -foreground [lindex $ary($pre,colors) $i]
      }
    }  
    incr j
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::GrapOptApplyFill {this var pre} {
#
#/////////////////////////////////////////////////////////////////////////////
  global toltk_images_path
  upvar \#0 $var ary  
  set g [getActiveGraph $this]
  
  set j 0
  set lstId [GrapOptGetIdLines $this $var $pre]
  foreach it [GrapOptGetNamesLines $this $var $pre] {
    set i [expr [lindex $lstId $j] - 1]
    foreach e $it {
      set areatile    ""
      set areapattern ""
      # Foreground
      if {[string equal [lindex $ary($pre,areaForegrounds) $i] defcolor]} {
        if {[string equal [$g element type $e] LineElement]} {
          set color [$g line cget $e -color]        
        } else {
          set color [$g line cget $e -foreground]
        }
      } else {
        set color [lindex $ary($pre,areaForegrounds) $i]
      }
      # Background
      if {[string equal [lindex $ary($pre,areaBackgrounds) $i] defcolor]} {
        if [string equal [$g element type $e] LineElement] {
          set backcolor [$g line cget $e -color]        
        } else {
          set backcolor [$g line cget $e -foreground]        
        }
      } else {
        set backcolor [lindex $ary($pre,areaBackgrounds) $i]
      }
      # Area Type
      if [string equal [$g element type $e] LineElement] {
        set areaTypeAux [lindex $ary($pre,areaTypes) $i]
      } else {
        set areaTypeAux [lindex $ary($pre,areaTypesBar) $i]
      }
      switch $areaTypeAux {      
        image {
          if ![string equal [lindex $ary($pre,areaTiles) $i] ""] {
            set areatile [::Bitmap::get [lindex $ary($pre,areaTiles) $i]]
          }
        }
        solid   { set areapattern solid  }
        striped { set areapattern @[file join $toltk_images_path fill.xbm] }
        "stipple image" { set areapattern @[lindex $ary($pre,stippleimages) $i] }
        "stipple degraded" {
          set areapattern "gray[lindex $ary($pre,stippledegradeds) $i]"
        }
      }
      # Apply
      if [string equal [$g element type $e] LineElement] {
        $g element configure $e -areatile $areatile -areapattern $areapattern \
          -areaforeground $color -areabackground $backcolor
      } else {
        if [string equal $areapattern solid] { set areapattern "" }
        $g element configure $e \
          -stipple $areapattern -foreground $color -background $backcolor
      }
    }  
    incr j
  }
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptApplyShowValues {this var pre} {
#
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 $var ary
  set g [getActiveGraph $this]
 
  set j 0
  set lstId [GrapOptGetIdLines $this $var $pre]
  foreach it [GrapOptGetNamesLines $this $var $pre] {
    set i [expr [lindex $lstId $j] - 1]
    foreach e $it {
      $g element configure $e \
        -showvalues  [lindex $ary(${pre},showvalues)         $i] \
        -valuefont   [lindex $ary(${pre},valuefont)         $i] \
        -valuecolor  [lindex $ary(${pre},valuecolor)        $i] \
        -valueformat %0.[lindex $ary(${pre},valueformatdec) $i]f \
        -valueshadow [lindex $ary(${pre},valueshadow)       $i] \
        -valuerotate [lindex $ary(${pre},valuerotate)       $i] \
        -valueanchor [lindex $ary(${pre},valueanchor)       $i]
    }      
    incr j
  }
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptApplyShowSymbols {this var pre} {
#
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 $var ary
  set g [getActiveGraph $this]
  
  set j 0
  set lstId [GrapOptGetIdLines $this $var $pre]
  foreach it [GrapOptGetNamesLines $this $var $pre] {
    set i [expr [lindex $lstId $j] - 1]
    foreach e $it {
      if {[lindex $ary(${pre},showsymbols) $i]} {
        if [string equal [$g element type $e] "LineElement"] {      
          $g element configure $e \
            -symbol       [lindex $ary(${pre},symbol)     $i] \
            -pixels       [lindex $ary(${pre},pixels)    $i] \
            -scalesymbols [lindex $ary(${pre},scalesymbols)    $i] \
            -fill         [lindex $ary(${pre},fill)     $i] \
            -outline      [lindex $ary(${pre},outline)      $i] \
            -outlinewidth [lindex $ary(${pre},outlinewidth) $i]
        }
      } else {
        if [string equal [$g element type $e] "LineElement"] {
          $g element configure $e -symbol none
        }
      }
    }
    incr j
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::GrapOptApplyMarkers {this var pre} {
#
# PURPOSE:
#
# PARAMETERS:
#   this:
#   var:
#   pre:
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  upvar \#0 $var ary
  variable tmpOpt
  
  set gr $data(gr,active)
  set g [getActiveGraph $this]
  
  # delete marker
  foreach marker [$g marker names] {
    $g marker delete $marker
  }
  # marker each
  foreach {axis map} {x mapx y mapy x2 mapx y2 mapy} {
    # comprobamos si esta visible el eje
    if {![$g axis cget $axis -hide]} {
    # se puede preguntar por la variable
    # if $tmpOpt(var,${axis}AxisShow)
      set AXI [string toupper [string index $axis 0]]
      if {[info exists ary(${pre},${axis}Markers)]} {
        set lstMarkers $ary(${pre},${axis}Markers)
        if [llength $lstMarkers] {
          if {[info exists data(gr,$gr,OnGet${AXI}Ticks)]} {
            set numTicks [llength [$data(gr,$gr,OnGet${AXI}Ticks) $this]]
            set iniMarker -1
          } else  {
            # nos quedamos con el valor minimo y maximo del eje
            #TODO: igual es interesante quedarnos con el max y min de las lineas
            #      asiciadas al eje
            set max [$g axis cget $axis -max]
            set min [$g axis cget $axis -min]
            if {[string length $max] && [string length $min]} {
              set numTicks  $max
              set iniMarker $min
            } else {
              set iniMarker -1
              set numTicks   0
            }
          }
          set i 0
          set j 0
          foreach it $lstMarkers {
            if {$j==5} { set j 0 }
            set color $tmpOpt(var,${axis}MarkerColorInEach$j)
            set width $tmpOpt(var,${axis}MarkerWidthInEach$j)
            set dashe $tmpOpt(var,${axis}MarkerDasheInEach$j)
            set under $tmpOpt(var,${axis}MarkerUnderInEach$j)
            incr j
            #set pos -1
            set pos $iniMarker
            $g marker bind $it <Enter> \
                [list ::bayesGraph::WriteInfo $this "[mc Marker]${axis}: each $it"]
            $g marker bind $it <Leave> \
                [list ::bayesGraph::WriteInfo $this ""]
            while {$pos < $numTicks} {
              set pos [expr $pos + $it]
              # limites
              if {[string equal x [string index $axis 0]]} {
                set coord [list $pos -Inf $pos Inf]
              } else {
                set coord [list -Inf $pos Inf $pos]
              }
              catch { $g marker create line -name ${it}_$pos -coords $coord \
                    -outline $color -dashes $dashe -$map $axis \
                    -bindtags $it -linewidth $width -under $under }
            }
            incr i
          }
        }
      }
    }
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::bayesGraph::GrapOptApplyMarkersIn {this var pre} {
#
# PURPOSE:
#
# PARAMETERS:
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  upvar \#0 $var ary
  variable tmpOpt

  set gr $data(gr,active)
  set g [getActiveGraph $this]

  # marker in
  foreach {axis map} {x mapx y mapy x2 mapx y2 mapy} {
# se puede preguntar tambien por -hide del axis
    if {![$g axis cget $axis -hide]} {
#    if $tmpOpt(var,${axis}AxisSelect)
      if {[info exists ary(${pre},${axis}MarkersIn)]} {
        set lstMarkersIn $ary(${pre},${axis}MarkersIn)
        if [llength $lstMarkersIn] {   
          set i 0    
          set lon [llength [$g marker names]]
          set j [expr $lon<5 ? $lon : $lon % 5] 

          foreach it $lstMarkersIn {
            if {$j==5} { set j 0 } 
            set color $tmpOpt(var,${axis}MarkerColorInEach$j)
            set width $tmpOpt(var,${axis}MarkerWidthInEach$j)
            set dashe $tmpOpt(var,${axis}MarkerDasheInEach$j)
            set under $tmpOpt(var,${axis}MarkerUnderInEach$j)
            incr j  
            # limites
            if {[string equal x [string index $axis 0]]} {
              set coord [list $it -Inf $it Inf]
            } else {
              set coord [list -Inf $it Inf $it]
            }
            # axis
            if {[string length $it]==2} {
              set inx 2
            } else {
              set inx {}
            } 
            # marker
            $g marker bind $it <Enter> \
              [list ::bayesGraph::WriteInfo $this "[mc Marker]$axis: in $it"]
            $g marker bind $it <Leave> \
              [list ::bayesGraph::WriteInfo $this ""]
            catch { $g marker create line -name $it -coords $coord \
              -outline $color -dashes $dashe -$map $axis \
              -bindtags $it -linewidth $width -under $under}
            incr i
          }
        }
      }
    }
  }
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptApplyGrid {this var pre} {
#
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 $var ary

  set g [getActiveGraph $this]
  $g grid configure -color     $ary(${pre},Color)\
                    -dashes    $ary(${pre},Dashes)\
                    -linewidth $ary(${pre},LineWidth)\
                    -minor     $ary(${pre},Minor)
  if [string equal $ary(${pre},MapX) "none"] {
    $g grid configure -mapx {}
  } else {
    $g grid configure -mapx $ary(${pre},MapX)
  }
  if [string equal $ary(${pre},MapY) "none"] {
    $g grid configure -mapy {}
  } else {
    $g grid configure -mapy $ary(${pre},MapY)
  }
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptApplyLegend {this var pre} {
#
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 $var ary

  set g [getActiveGraph $this]

  $g legend configure -hide       $ary(${pre},Hide)\
                      -bg         $ary(${pre},Bg)\
                      -bd         $ary(${pre},Bd)\
                      -fg         $ary(${pre},Fg)\
                      -font       $ary(${pre},Font)\
                      -shadow     $ary(${pre},Shadow)\
                      -position   $ary(${pre},Position)\
                      -anchor     $ary(${pre},Anchor)\
                      -relief     $ary(${pre},Relief)\
                      -raised     $ary(${pre},Raised)\
                      -ipadx      [list $ary(${pre},IPadXl) $ary(${pre},IPadXr)]\
                      -ipady      [list $ary(${pre},IPadYt) $ary(${pre},IPadYb)]\
                      -padx       [list $ary(${pre},PadXl)  $ary(${pre},PadXr)]\
                      -pady       [list $ary(${pre},PadYt)  $ary(${pre},PadYb)]\
                      -rows       $ary(${pre},Rows)\
                      -columns    $ary(${pre},Columns)
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptApplyStyle {this var pre} {
#
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 $var ary

  set g [getActiveGraph $this]
  $g configure -plotbackground  $ary(${pre},plotbackground)\
               -plotrelief      $ary(${pre},plotrelief)\
               -plotborderwidth $ary(${pre},plotborderwidth)\
               -plotpadx        [list $ary(${pre},plotpadxl) $ary(${pre},plotpadxr)]\
               -plotpady        [list $ary(${pre},plotpadyt) $ary(${pre},plotpadyb)]\
               -invertxy        $ary(${pre},invertxy)\
               -background      $ary(${pre},background)\
               -relief          $ary(${pre},relief)\
               -borderwidth     $ary(${pre},areaborderwidth)\
               -lm              $ary(${pre},lm)\
               -rm              $ary(${pre},rm)\
               -tm              $ary(${pre},tm)\
               -bm              $ary(${pre},bm)
  #El color de los ejes lo vamos a cambiar segun el color de fondo exterior
  $g axis configure x  -background $ary(${pre},background)
  $g axis configure x2 -background $ary(${pre},background)
  $g axis configure y  -background $ary(${pre},background)
  $g axis configure y2 -background $ary(${pre},background)
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptOK {this dialog} {
#
#/////////////////////////////////////////////////////////////////////////////    
  GrapOptApply $this
  GrapOptDestroy $this $dialog
}


#///////////////////////////////////////////////////////////////////////////// 
  proc ::bayesGraph::GrapOptCheck {this} {
#
# PURPOSE: Checks for all widgets from graph dialog options that need to be 
#          checked for enabling/disabling other associated controls. 
#
# PARAMETERS:
#   this   -> Instance of bayesGraph
#
#/////////////////////////////////////////////////////////////////////////////
  GrapOptCheckXYAxisAll    $this
  GrapOptCheckXYTicksType  $this
#  GrapOptCheckShowSymbols $this
}

#///////////////////////////////////////////////////////////////////////////// 
proc ::bayesGraph::GrapOptCheckXYAxisAll {this} {
#
# PURPOSE:
#
# PARAMETERS:
#   this   -> Instance of bayesGraph
#
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::data data
  variable tmpOpt
  set gr $data(gr,active)
  if {[info exists tmpOpt(widget,xyAxisMin)] && \
      [info exists tmpOpt(widget,xyAxisMax)] && \
      [info exists tmpOpt(var,xyAxisAll)]} {
    # eje acutal
    set axis $tmpOpt(var,xyAxisSelect)
    set AXI  [string toupper [string index $axis 0]]
    if {[info exists tmpOpt(var,xyAxisSelect)] &&
        [info exists data(gr,$gr,OnGet${AXI}Ticks)]} {
      $tmpOpt(widget,xyAxisMin) configure \
        -values [$data(gr,$gr,OnGet${AXI}Ticks) $this] -range {}
      $tmpOpt(widget,xyAxisMax) configure \
         -values [$data(gr,$gr,OnGet${AXI}Ticks) $this] -range {}
    } else  {
      $tmpOpt(widget,xyAxisMin) configure -values {} -range {-INF INF 1}
      $tmpOpt(widget,xyAxisMax) configure -values {} -range {-INF INF 1}
    }
    if $tmpOpt(var,xyAxisAll) {
      focus $tmpOpt(widget,chXYAll)
      $tmpOpt(widget,xyAxisMin) configure -state disabled
      $tmpOpt(widget,xyAxisMax) configure -state disabled
      if {[llength [$tmpOpt(widget,xyAxisMin) cget -values]]} {
        $tmpOpt(widget,xyAxisMin) setvalue  first
      } 
      if {[llength [$tmpOpt(widget,xyAxisMax) cget -values]]} {
        $tmpOpt(widget,xyAxisMax) setvalue  last
      }
    } else  {
      $tmpOpt(widget,xyAxisMin) configure -state normal 
      $tmpOpt(widget,xyAxisMax) configure -state normal
    }
  }
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptCheckXYTicksType {this} {
#
#/////////////////////////////////////////////////////////////////////////////    
  variable tmpOpt
  if {[info exists tmpOpt(widget,rbXYTotal)] && \
      [info exists tmpOpt(widget,xyTicksStep)] && \
      [info exists tmpOpt(widget,xyTicksTotal)]} {
    switch $tmpOpt(var,xyTicksType) {
      total {
        if {[info exists tmpOpt(widget,rbXYTotal)]} {
          focus $tmpOpt(widget,rbXYTotal)
        }
        $tmpOpt(widget,xyTicksStep)  configure -state disabled
        $tmpOpt(widget,xyTicksTotal) configure -state normal
      }
      each {
        if {[info exists tmpOpt(widget,rbXYEach)]} {
          focus $tmpOpt(widget,rbXYEach)
        }
        $tmpOpt(widget,xyTicksTotal) configure -state disabled
        $tmpOpt(widget,xyTicksStep)  configure -state normal
      }
      auto {
        set tmpOpt(var,xyTicksTotal) \
          [CalculateXAxisNumTicks $this $tmpOpt(var,xyAxisSelect)]
        set tmpOpt(var,xyTicksStep) \
          [ComputeXAxisStep $this $tmpOpt(var,xyTicksTotal) \
                                  $tmpOpt(var,xyAxisSelect)]
        if {[info exists tmpOpt(widget,rbXYAuto)]} {
          focus $tmpOpt(widget,rbXYAuto)
        }
        $tmpOpt(widget,xyTicksTotal) configure -state disabled
        $tmpOpt(widget,xyTicksStep)  configure -state disabled
      }
    }
  }
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::ChangeFormat  { var name1 name2 op} {
#
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 ${name1}($name2) varsource
  upvar \#0 $var vardestine
  set vardestine %0.${varsource}f
}


#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::WriteInfo {this text} {
#
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 ${this}::widgets widgets
  $widgets(sb,lInfo) configure -text $text
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::GrapOptApplyMappedLines {this var pre} {
#
# PURPOSE: Asigna las lineas a los ejes especificados en opciones
#
# PARAMETERS:
#   this -> Instance of bayesGraph
#   var  -> nombre de la variable
#   pre  -> prefijo de array
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 $var ary
  set g [getActiveGraph $this]

  # all lines
  set j 0
  set lstId [GrapOptGetIdLines $this $var $pre]
  foreach it [GrapOptGetNamesLines $this $var $pre] {
    set i [expr [lindex $lstId $j] - 1]
    foreach e $it {
      set xy [lindex $ary($pre,xyMappedAxis) $i]
      if {!([string equal [$g element cget $e -mapx] [lindex $xy 0]] && \
            [string equal [$g element cget $e -mapy] [lindex $xy 1]])} {
        ::bayesGraph::GrapOptApplyMappedLinesChange \
          $this $var $e [lindex $xy 0] [lindex $xy 1]
      }
    }
    incr j
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::GrapOptApplyMappedLinesChange {this var le X Y} {
#
# PURPOSE: Cambiamos de eje/s un linea. Si el eje no esta visible, lo
#          mostramos y configuramos maximo y minimo. 
#
# PARAMETERS:
#   this -> Instance of bayesGraph
#   var  -> nombre de la variable
#   le   -> Lista de elementos
#   X    -> Axis X destiny
#   Y    -> Axis Y destiny 
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 $var ary
  set g [getActiveGraph $this]
  variable tmpOpt
  # comrpbamos que no este en el mismo eje
  foreach e $le {
    set xOld [$g element cget $e -mapx]
    set yOld [$g element cget $e -mapy]
    if {($X!=$xOld) || ($Y!=$yOld)} {
      foreach axis "$X $Y" {
        set axi  [string index $axis 0]
        # comprobamos si eje X visible
        if {[$g axis cget $axis -hide]} {
          $g axis configure $axis -hide 0
          if {[info exists tmpOpt(var,${axis}AxisShow)]} {
            set tmpOpt(var,${axis}AxisShow) 1
          }
          # valores
          set val  [$g line cget $e -${axi}data]
          set lmax [lmaximo $val]
          set lmin [lminimo $val]
          set tmpOpt(var,${axis}AxisMin) $lmin
          set tmpOpt(var,${axis}AxisMax) $lmax
          ::bayesGraph::GrapOptApplyAxisLimits $this $axis $lmin $lmax
        }
      }
    }
    $g element configure $e -mapx $X -mapy $Y
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::GetLimitsValueAxis {this axis} {
#
# PURPOSE: Obtenemos el valor maximo y minimo de todas las lineas ascoicadas a
#          un eje
#  this -> Instance of bayesGraph
#  axis -> Eje actual
#
# PARAMETERS: el valor minimo y maximo asociado a un eje
#
#/////////////////////////////////////////////////////////////////////////////
  # inicializamos variables 
  set g [getActiveGraph $this]
  set axi [string index $axis 0]
  set lst [$g element show]
  set lmax {}
  set lmin {}
  # buscamos valor maximo y minimo de todas las lineas
  foreach e $lst {
    if {( [$g element cget $e -map${axi}]==$axis) && \
        (![$g element cget $e -hide])} {
      # valores
      set val [$g line cget $e -${axi}data]
      # buscamos valorores maximos y minimos Y
      if {[set vmax [lmaximo $val]] ne ""} { 
        lappend lmax $vmax
      }
      if {[set vmin [lminimo $val]] ne ""} {
        lappend lmin $vmin
      }
    }
  }
  # buscamos el maximo y minimo
  if {[llength $lmax] && [llength $lmin]} {
    set max [lmaximo $lmax]
    set min [lminimo $lmin]
    set lst [list $min $max]
  } else  {
    set lst {}
  }
  return $lst
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::GrapOptApplyAxisLimits {this axis min max} {
#
# PURPOSE: Aplicamos la configuracion de minimo y maximo de un eje. Para ello
#          comprobamos que el stepsize no quede muy pequeño
#
#  this -> Instance of bayesGraph
#  axis -> Eje actual
#  min  -> Min value
#  max  -> Max value
#
# PARAMETERS: el valor minimo y maximo asociado a un eje
#
#/////////////////////////////////////////////////////////////////////////////
  # inicializamos variables
  variable tmpOpt
  set g [getActiveGraph $this]
  set stepsize [$g axis cget $axis -stepsize]
  set maxTicks 200
  # comprobamos el valor de stepsize, si es mayor que 1000, aplicamos 1000
  # con esta comprobación evitamos que de error al cambiar el min y max
  if {[string length $max] && [string length $min]} {
    set n [expr $stepsize ? ($max - $min) / $stepsize : $stepsize]
    if {$n>$maxTicks} {
      set tmpOpt(var,${axis}TicksStep) [expr ($max - $min) / $maxTicks]
      $g axis configure $axis -stepsize $tmpOpt(var,${axis}TicksStep)
    }
  }
  # convertimos a double
  if {[string is int $min]} { set min "$min.0" }
  if {[string is int $max]} { set max "$max.0" }
  if {$min<$max} {
    if {0 && [string index $axis 0] eq "x"} {
      set min [expr $min - 0.5]
      set max [expr $max + 0.5]
    }
    $g axis configure $axis $axis -min $min -max $max
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::bayesGraph::SetMainTitle {this title} {
#
# PURPOSE: Set main title o chart
#
#  this  -> Instance of bayesGraph
#  title -> list of title
#
# PARAMETERS: el valor minimo y maximo asociado a un eje
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::widgets widgets  

  for  {set gr 0} {$gr<$data(gr,numGraph)} {incr gr 1} {
    set inx [expr $gr>=[llength $title] ? [llength $title] - 1 : $gr]
    $widgets(gr,$gr) configure -title [lindex $title $inx]
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::bayesGraph::GrapOptChangeType {this elem segs} {
#
#/////////////////////////////////////////////////////////////////////////////    
  set g [getActiveGraph $this]
  set typeElem   [$g element type [lindex $segs 0]]
  
  foreach el $segs {
    set xdata       [$g element cget $el -xdata]
    set ydata       [$g element cget $el -ydata]
    set label       [$g element cget $el -label]
    set mapx        [$g element cget $el -mapx]
    set mapy        [$g element cget $el -mapy]
    set label       [$g element cget $el -label]
    set hide        [$g element cget $el -hide]
    set showvalues  [$g element cget $el -showvalues]
    set valueanchor [$g element cget $el -valueanchor]
    set valuecolor  [$g element cget $el -valuecolor]
    set valuefont   [$g element cget $el -valuefont]
    set valueformat [$g element cget $el -valueformat]
    set valuerotate [$g element cget $el -valuerotate]
    set valueshadow [$g element cget $el -valueshadow]
    if [string equal $typeElem "BarElement"] {
      set bgcolor [$g element cget $el -background]
      set color   [$g element cget $el -foreground]
      set width   [$g element cget $el -borderwidth]
      set xbm     [$g element cget $el -stipple]
      
      $g bar delete $el
      $g element create $el -xdata $xdata -ydata $ydata\
          -color $color -areabackground $bgcolor -areaforeground $color \
          -linewidth $width -areapattern $xbm -symbol "none"
    } else  {
      set color   [$g element cget $el -color]
      set bgcolor [$g element cget $el -areabackground]
      if {$bgcolor eq "defcolor"} {
        set bgcolor $color
      }
      set width [$g element cget $el -linewidth]
      set xbm   [$g element cget $el -areapattern]
      if {$xbm eq "solid"} {
        set xbm ""
      }
      $g element delete $el
      $g bar create $el -xdata $xdata -ydata $ydata\
          -foreground $color   -barwidth 0.5\
          -background $bgcolor -borderwidth $width -stipple $xbm
    }
    $g element conf $el\
           -valueanchor $valueanchor -valuecolor  $valuecolor\
           -valuefont   $valuefont   -valueformat $valueformat\
           -valuerotate $valuerotate -valueshadow $valueshadow\
           -showvalues  $showvalues  -hide        $hide\
           -mapx $mapx  -mapy $mapy  -label $label
  }
}

#/////////////////////////////////////////////////////////////////////////////    
#NOTE. No esta en uso. Para usarla habría que modificar la función y guardar
#los widget en variables
proc ::bayesGraph::GrapOptCheckShowSymbols {this serie} {
#
#/////////////////////////////////////////////////////////////////////////////    
  variable tmpOptElem

  if {!$tmpOptElem(var,curve,$serie,showsymbols)} {
    SetAttribute $tmpOptElem(widget,fSymbol) state disabled
    $tmpOptElem(widget,chShow)    configure -state normal
  } else  {
    SetAttribute $tmpOptElem(widget,fSymbol) state normal
  }
}

#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptGetNamesLines {this var pre} {
#
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 $var ary
  return [lindex [ltraspose $ary(${pre},lstnames)] 1]
}

#/////////////////////////////////////////////////////////////////////////////    
proc ::bayesGraph::GrapOptGetIdLines {this var pre} {
#
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 $var ary
  return [lindex [ltraspose $ary(${pre},lstnames)] 0]
}