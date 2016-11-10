#/////////////////////////////////////////////////////////////////////////////
# FILE    : corgraph.tcl
# PURPOSE : This file 
#           It's a child of bayesGraph so inherites all its functionalities.
#/////////////////////////////////////////////////////////////////////////////


#/////////////////////////////////////////////////////////////////////////////
namespace eval AutocorGraph {
#  
#/////////////////////////////////////////////////////////////////////////////  
  variable tmpOpt
}


#/////////////////////////////////////////////////////////////////////////////  
proc ::AutocorGraph::Create { path serie } {
#  
#/////////////////////////////////////////////////////////////////////////////    
  global toltk_images_path
  global zoomInfo
#puts "Create ..... path: $path \n serie: $serie"
  set structure {v {h g g} {h g g}}
  set Instance [::bayesGraph::Create $path $structure]
  upvar \#0 ${Instance}::widgets widgets
  upvar \#0 ${Instance}::data data

  set data(namespace) AutocorGraph
  set data(proc,OnOptionCreate)   ::AutocorGraph::OptionsCreate
  set data(proc,OnOptionDestroy)  ::AutocorGraph::OptionsDestroy
  set data(proc,OnOptionApply)    ::AutocorGraph::OptionsApply
  set data(proc,OnComputeStepX)   ::AutocorGraph::ComputeStepX
  set data(proc,OnFixStepX)       ::AutocorGraph::FixStepX
  set data(proc,OnEvalWindow)     ::AutocorGraph::EvalWindow
  set data(proc,OnToTable)        ::AutocorGraph::ToTable
  set data(proc,OnWriteIni)       ::AutocorGraph::WriteIni

  # se configuran los valores de inicio
  set data(typecor) {{ACF "Autocorrelation"} \
                     {PACF "Partial Autocorrelation"} \
                     {IACF "Inverse Autocorrelation"} \
                     {IPACF "Inverse partial Autocorrelation"}}
  set data(serie) $serie

  for  {set gr 0} {$gr<$data(gr,numGraph)} {incr gr 1} {
    set grpath $widgets(gr,$gr)
    set data(gr,active) $gr
    set data(gr,$gr,title) [lindex [lindex $data(typecor) $gr] 1]
    set data(gr,$gr,type)  [lindex [lindex $data(typecor) $gr] 0]
    $grpath bar create serie -relief solid -label $data(gr,$gr,title)
    set data(gr,$gr,elements) serie
    bind [winfo toplevel $widgets(this)] <F[expr $gr+2]> +[list ::AutocorGraph::Maximize $Instance [expr $gr+1]]

    set data(gr,$gr,OnGetXTicks)    ::AutocorGraph::GetXValues
    set data(gr,$gr,OnGetXValues)   ::AutocorGraph::GetXValues  
  }

  ::bayesGraph::BindLegend $Instance
  bind [winfo toplevel $widgets(this)] <Escape> [list ::AutocorGraph::Restore $Instance]
  set data(gr,active) 0
  #Establecer el fichero GCF del que se lee
  ::bayesGraph::LoadGCF $Instance
  ReadIni $Instance
  Configure $Instance 

  return $Instance
}


#/////////////////////////////////////////////////////////////////////////////  
proc ::AutocorGraph::Maximize {this num} {
#  
#/////////////////////////////////////////////////////////////////////////////    
  #Tolcon_Trace "Maximize this=$this, num=$num"
  #Hacerlo general, no solo para 4 gráficos
  upvar \#0 ${this}::widgets widgets
  switch -- $num {
    1 {
       set where1 max
       set w2 [lindex $widgets(pws) 1]
       set where2 max}
    2 {
       set where1 max
       set w2 [lindex $widgets(pws) 1]
       set where2 min}
    3 {
       set where1 min
       set w2 [lindex $widgets(pws) 2]
       set where2 max}
    4 {
       set where1 min
       set w2 [lindex $widgets(pws) 2]
       set where2 min
      }
  }
  ::PanedWindow::MoveSash [lindex $widgets(pws) 0] 1 x $where1
  ::PanedWindow::MoveSash $w2 1 y $where2
}


#/////////////////////////////////////////////////////////////////////////////  
proc ::AutocorGraph::Restore {this} {
#  
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 ${this}::widgets widgets
  ::PanedWindow::MoveSash [lindex $widgets(pws) 0] 1 x middle
  ::PanedWindow::MoveSash [lindex $widgets(pws) 1] 1 y middle
  ::PanedWindow::MoveSash [lindex $widgets(pws) 2] 1 y middle
}


#/////////////////////////////////////////////////////////////////////////////  
proc ::AutocorGraph::Configure {this} {
#  
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::options opt  

  set old_active $data(gr,active)
  for {set gr 0} {$gr<$data(gr,numGraph)} {incr gr} {
    set grpath $widgets(gr,$gr)
    set data(gr,active) $gr
    OptionsApplyNLags $this $opt(nLag)
  }
  set data(gr,active) $old_active
  ::bayesGraph::Configure $this
  for {set gr 0} {$gr<$data(gr,numGraph)} {incr gr} {
    set grpath $widgets(gr,$gr)
    set data(gr,active) $gr
    OptionsApplyNSigmas $this $opt(nSigmas)
  }
  set data(gr,active) $old_active
}


#/////////////////////////////////////////////////////////////////////////////  
proc ::AutocorGraph::DestroyMarker { this } {
#  
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 ${this}::options opt
  set grpath [::bayesGraph::getActiveGraph $this]
  if { [info exist opt(markers)] } {
    if {[llength $opt(markers)] > 0} {
      foreach item $opt(markers) {
        $grpath marker delete $item 
      }
    }
  }
}


#/////////////////////////////////////////////////////////////////////////////  
proc ::AutocorGraph::GetXValues {this} {
#  
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data data
  set gr $data(gr,active)
  set grpath $widgets(gr,$gr)
  set values [$grpath element cget serie -xdata]
  #Tolcon_Trace "AutocorGraph::GetXValues: values: $values"
  return $values
}


#/////////////////////////////////////////////////////////////////////////////  
proc ::AutocorGraph::ComputeStepX {this min max total} {
#  
#/////////////////////////////////////////////////////////////////////////////    
  #Tolcon_Trace "min: $min; max: $max; total: $total"
  set dif [expr $max - $min]
  if {$total > $dif} {
    set step 1
  } else {
    set step [expr round($dif/$total)]
  }
  return $step
}


#/////////////////////////////////////////////////////////////////////////////  
proc ::AutocorGraph::FixStepX {this min max step} {
#  
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 ${this}::data data
  if {[string length $max] && [string length $min]} {
    set dif [expr $max - $min]
    if {$step > $dif } {
      set step [expr round($dif)]
    } else {
      set step [expr round($step)]
    }
  } else {
    set step [expr round($step)]
  } 
  return $step
}


#/////////////////////////////////////////////////////////////////////////////  
proc ::AutocorGraph::EvalWindow {this} {
#
#/////////////////////////////////////////////////////////////////////////////    
  upvar \#0 ${this}::data data
  set serie $data(serie)
  if {[llength $serie] > 1} {
    set nameSerie [lindex [::tol::info variable $serie] 1]
  } else {
    set nameSerie $serie
  }
  return $nameSerie
}


#/////////////////////////////////////////////////////////////////////////////  
proc ::AutocorGraph::ToTable {this} {
#
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::options opt
  upvar \#0 ${this}::widgets widgets
  set grpath [::bayesGraph::getActiveGraph $this]
  set tl .tbl_from_$data(serie)
#  set tl $widgets(this)_table
  if [winfo exists $tl] {
    raise $tl
  } else  {
    # title
    if {[llength $data(serie)] > 1} {
      set nameSerie [lindex [::tol::info variable $data(serie)] 1]
    } else {
      set nameSerie $data(serie)
    }
    # create
    set tl [::project::CreateForm \
      -title "[mc "Autocorrelation table"]: $nameSerie" \
      -iniconfig AutocorGraphTable \
      -type Tables
    ]
	set tlf [$tl getframe]
    # config
    btable $tlf.t -parent $tl \
                  -posteditcmd [list ::TolConsole::TableRecentUpdate $tlf.t] \
                  -iniconfig btable
    $tlf.t fillAutocor $data(serie) ""
    $tl bind <Control-F4> [list $tl kill]
    pack $tlf.t -fill both -expand yes
  }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::AutocorGraph::OptionsCreate {this ts} {
#
# PURPOSE: Creates especific options of a autocorrelation graph. It's called
#          by BayesGraph when general graph options are created.
#
# PARAMETERS:
#   this -> Instance of BayesTable
#   ts   -> tabset (=>NoteBook) in which options must be created
#
#/////////////////////////////////////////////////////////////////////////////
  ReadIniOpc $this
  #insertar tabset
  set num_i 1
  set lab_i Autocorrelation
  $ts insert $num_i $lab_i -text "  [mc $lab_i]  " -background \#d9d9d9 
  set tab_i [$ts getframe $lab_i]
  $tab_i configure -borderwidth 2 -background \#d9d9d9  
  set f [frame $tab_i.f]
  pack $tab_i.f -fill both -expand yes
    
  set w_tabset [NoteBook $f.nb -tabbevelsize 8 -tabpady {2 6} -font {Arial 8}] 
  set num_i 0
  set lab_i Parameters
  $w_tabset insert $num_i $lab_i -text "  [mc $lab_i]  " -background \#d9d9d9 
  set tab_i [$w_tabset getframe $lab_i]
  $tab_i configure -borderwidth 2 -background \#d9d9d9
  set f1 [frame $tab_i.f]
  pack $tab_i.f -fill both -expand yes  
  $w_tabset raise [$w_tabset pages 0]
  
  OptionsGet $this
  OptionsCreateParameters $this $f1
  
  #OptionsInit  $this
  #OptionsSet   $this
  
  $w_tabset compute_size 
  grid $w_tabset -sticky news  
  grid rowconfigure    $f 0 -weight 1
  grid columnconfigure $f 0 -weight 1
}


#/////////////////////////////////////////////////////////////////////////////
proc ::AutocorGraph::OptionsCreateParameters {this w} {
#
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::options opt
  variable tmpOpt
  TitleFrame $w.tf1 -text [mc "Parameters"]
  set f1 [$w.tf1 getframe]
  label $f1.lnLags -text [mc "Numbers of lags"]:
  SpinBox $f1.sbnLags -width 8 -justify right -editable 1 -range "0 500" \
    -textvariable ::AutocorGraph::tmpOpt(var,nLag)
  label $f1.lnSigma -text [mc "Sigma factor"]:
  set tmpOpt(widget,comboSigma) [::Objects::ComboAutoFillCreate $f1.cbnSigma \
    -width 7 -height 5 -editable 1 -justify right \
    -values $opt(listSigmas) \
    -textvariable ::AutocorGraph::tmpOpt(var,nSigmas)]
  label $f1.lSigma -text [mc "Sigma value"]:
  label $f1.lvSigma -text $tmpOpt(var,valueSigma)
  frame $f1.f -width 20
  grid $f1.f $f1.lnLags  $f1.sbnLags  -sticky w -pady 2
  grid ^     $f1.lnSigma $f1.cbnSigma -sticky w -pady 2
  grid ^     $f1.lSigma  $f1.lvSigma  -sticky w -pady 2
  grid rowconfigure    $f1 3 -weight 1
  grid columnconfigure $f1 2 -weight 1
  grid $w.tf1 -sticky news -padx 2 
  grid rowconfigure    $w 0 -weight 1
  grid columnconfigure $w 0 -weight 1
}  

#/////////////////////////////////////////////////////////////////////////////
  proc ::AutocorGraph::OptionsDestroy {this} {
#
# PURPOSE: Called before options dialog is destroyed. This procedure is called
#          by parent BayesGraph. Actually writes to ini file options dialog's
#          options.
#
# PARAMETERS:
#   this -> Instance of BayesGraph
#
#/////////////////////////////////////////////////////////////////////////////
  ::AutocorGraph::WriteIniOpc $this
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::AutocorGraph::OptionsGet {this} {
#
# PURPOSE: Initializes all options variables and stores them into the
#          temporary 'tmpOpt' array. 
#
# PARAMETERS:
#   this   -> Instance of BayesGraph
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::options opt
  variable tmpOpt

  set tmpOpt(var,nLag)       $opt(nLag)
  set tmpOpt(var,nSigmas)    $opt(nSigmas)
  set tmpOpt(var,listSigmas) $opt(listSigmas)
  set tmpOpt(var,valueSigma) $data(valueSigma)
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::AutocorGraph::OptionsSet {this} {
#
# PURPOSE: Commits all options values from the temporary 'tmpOpt' array to
#          other structure.
#
# PARAMETERS:
#   this   -> Instance of BayesGraph
#
#/////////////////////////////////////////////////////////////////////////////
  upvar  \#0 ${this}::options opt

  variable tmpOpt
  set opt(nLag)       $tmpOpt(var,nLag)
  set opt(nSigmas)    $tmpOpt(var,nSigmas)
  set opt(listSigmas) $tmpOpt(var,listSigmas)
}


#/////////////////////////////////////////////////////////////////////////////
proc ::AutocorGraph::OptionsApply {this} {
#
#/////////////////////////////////////////////////////////////////////////////  
  variable tmpOpt  
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::options opt  

  OptionsApplyNLags   $this $tmpOpt(var,nLag)
  OptionsApplyNSigmas $this $tmpOpt(var,nSigmas)

  ::bayesGraph::GrapOptCheckXYAxisAll $this
  ::bayesGraph::GrapOptApplyAxisPos $this ::bayesGraph::tmpOpt var
  OptionsSet $this
}


#/////////////////////////////////////////////////////////////////////////////
proc ::AutocorGraph::OptionsApplyNLags { this nLag} {
#
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::options opt
  set gr $data(gr,active)
  set grpath $widgets(gr,$gr)
  #read values from TOL
  set values [::tol::autocor $data(gr,$gr,type) $data(serie) $nLag] 
  set data(valueSigma) [lindex $values 2]
  for {set i 1} {$i <= $nLag} {incr i 1} {
    lappend  lvalues $i.0 [lindex [lindex $values 3] [expr $i-1]]
    }
  $grpath bar configure serie -data $lvalues
  # Limits & Axis...
  set data(gr,$gr,limitsY) [$grpath axis limits y]
  set a_miny  [expr abs([lindex $data(gr,$gr,limitsY) 0])]
  set a_maxy  [expr abs([lindex $data(gr,$gr,limitsY) 1])]
  if { [expr $a_miny-$a_maxy] > 0 } {
    set bound $a_miny
  } else {
    set bound $a_maxy
  }
  if {[expr $bound-(3*$data(valueSigma))] < 0 } {
    set bound [expr 3*$data(valueSigma)]
  }
  ::bayesGraph::GrapOptApplyAxisLimits $this x 1              $nLag
  ::bayesGraph::GrapOptApplyAxisLimits $this y [expr -$bound] $bound
}


#/////////////////////////////////////////////////////////////////////////////
proc ::AutocorGraph::OptionsApplyNSigmas { this nSigmas} {
#
#/////////////////////////////////////////////////////////////////////////////  
  global toltk_images_path
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::options opt
  set gr $data(gr,active)
  set grpath $widgets(gr,$gr)
  #DestroyMarker $this
  
  set posSig [expr $nSigmas * $data(valueSigma)]
  $grpath marker create polygon -name sigmaPos -under yes\
    -coords {-Inf  $posSig Inf $posSig Inf Inf -Inf Inf}\
    -stipple @[file join $toltk_images_path fill.xbm]
  $grpath marker create polygon -name sigmaNeg -under yes\
    -coords {-Inf  -$posSig Inf -$posSig Inf -Inf -Inf -Inf}\
    -stipple @[file join $toltk_images_path fill.xbm]
  $grpath marker create line -name sigmaPosL -outline red -linewidth 2\
    -coords {-Inf  $posSig Inf $posSig}
  $grpath marker create line -name sigmaNegL -outline red -linewidth 2\
    -coords {-Inf  -$posSig Inf -$posSig}
    $grpath marker create text -name mtextd -coords {Inf $posSig}  -fill "" -outline red\
    -anchor ne -text "${nSigmas}*Sigma"
  $grpath marker create text -name mtexti -coords {Inf -$posSig} -fill "" -outline red\
    -anchor se -text "-${nSigmas}*Sigma"
  $grpath marker create line -name ejeX -coords {-Inf 0 Inf 0} -dashes {2 1 1 1}
  $grpath marker create line -name ejeY -coords {0 -Inf 0 Inf} -dashes {2 1 1 1}
  set opt(markers) { sigmaPos sigmaNeg sigmaPosL sigmaNegL mtextd mtexti }
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::AutocorGraph::ReadIni {this} {
#
# PURPOSE: Reads autocor graph specific options from .ini file. First of all,
#          reads options from BayesGraph. Ini file is the active graphic 
#          configuration file.
#
# PARAMETERS:
#   this  -> Instance of BayesGraph
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::options opt
  #set rini ::iniFile::Read
  set rini ::iniFile::ReadGCF

  set opt(nSigmas)     [$rini AutocorGraph nSigmas 2]
  set opt(nLag)        [$rini AutocorGraph nLag 30]
  ::bayesGraph::ReadIni $this
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::AutocorGraph::WriteIni {this} {
#
# PURPOSE: Writes autocor graph specific options to .ini file. This procedure
#          is called by parent BayesGraph. Ini file is the active graphic
#          configuration file.
#
# PARAMETERS:
#   this  -> Instance of BayesGraph
#
#/////////////////////////////////////////////////////////////////////////////
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::widgets widgets
  upvar \#0 ${this}::options opt
  #set wini ::iniFile::Write
  set wini ::iniFile::WriteGCF

  $wini AutocorGraph nSigmas  $opt(nSigmas)
  $wini AutocorGraph nLag     $opt(nLag)
}


#/////////////////////////////////////////////////////////////////////////////
proc ::AutocorGraph::ReadIniOpc {this} {
#
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::options opt
  set rini ::iniFile::Read
  set opt(listSigmas) [$rini AutocorGraphOpc listSigmas {1 2 3}]
}


#/////////////////////////////////////////////////////////////////////////////
proc ::AutocorGraph::WriteIniOpc {this} {
#
#/////////////////////////////////////////////////////////////////////////////  
  upvar \#0 ${this}::data data
  upvar \#0 ${this}::options opt
  variable tmpOpt
  set wini ::iniFile::Write
  $wini AutocorGraphOpc listSigmas [${tmpOpt(widget,comboSigma)} cget -values]
}
