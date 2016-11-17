#/////////////////////////////////////////////////////////////////////////////
# FILE    : btable.tcl
# PURPOSE : Bayes Table. 
#/////////////////////////////////////////////////////////////////////////////

package require byscommon
package require Tktable
package require snit
package require BWidget
package require bfunctions
package require bmultiselect
package require bfont
package require bcolor
package require tclodbc

if { [string equal $tcl_platform(platform) "windows"] } {
  package require cwind
}


#/////////////////////////////////////////////////////////////////////////////
::snit::widget btable {
#
# PURPOSE: Bayes Table. Widget that manages tables
# Specific options:
# -parent-> btable's parent
# -toolbar-> show/hide toolbar
# -statusbar-> show/hide statusbar
# -function-> function applied to selected cells shown in statusbar
# -path-> path of the open file
# -typelist-> list of the filtered types in open/save dialog
# -defext-> default extension in which files will be saved
# -defpath-> default path in which open dialog starts
# -defsavepath-> default path in which save dialog starts
# -checkcolumn-> 
# -internalvalues-> 
# -closecmd-> command to execute when closing the table
# -poseditcmd-> command to execute when a new file is opened or saved
# -iniconfig     -> Header in .ini file
# -bgalt-> flag that sets if alternated row background is used
# -bgalt1-> Color that will be used in odd rows
# -bgalt2-> Color that will be used in even rows
# -postordercmd -> command to execute when order
#
# variables:
# tb-> toolbar
# tab-> table
# sb-> statusbar
# sw-> scrollwindow
# widget-> widgets path
# data-> widgets data
# created-> created flag
#/////////////////////////////////////////////////////////////////////////////
  widgetclass Btable
  
  option -parent       {}
  option -toolbar      1
  option -statusbar    1
  option -function     {None}
  option -path         {}
  option -typelist     {{"Bayes Struct Table"  {".bst"}}
                        {"Bayes Data Table"    {".bdt"}}
                        {"Bayes Matrix Table"  {".bmt"}}
                        {"All Files"           {"*"}   }}
  option -defext       {}
  option -defsavepath      {/}
  option -defopenpath      {/}
  option -internalvalues   0
  option -checkcolumn      0
  option -closecmd         {}
  option -posteditcmd         {}
  option -iniconfig     {}
  option -state         {normal}
  option -cols          4
  option -rows          5
  option -bgalt         0
  option -bgalt1        white
  option -bgalt2        white
  option -toolenable    {}
  option -tooldisable   {}
  option -postordercmd         {}

  variable widget
  variable data
  variable created

  #///////////////////////////////////////////////////////////////////////////
  constructor {args} {
  #
  # PURPOSE: btable constructor. An instance of btable is created.
  #
  # PARAMETERS:
  #   args -> options
  #
  #///////////////////////////////////////////////////////////////////////////
    set created 0
    #Toolbar
    set widget(tb) [frame $self.tb -bd 2 -relief raised]
    #Frame
    set widget(fr) [frame $self.fr -bd 2 -relief raised -width 400 -height 500]
    #Statusbar
    set widget(sb) [frame $self.sb -relief groove -bd 1 -height 7]
    #Scrollwindow
    set widget(sw) [ScrolledWindow $widget(fr).sw]
    #Table with default options
    install tab using table $widget(sw).t \
                  -variable matrix \
                  -multiline 0 -selectmode extended -resizeborders col \
                  -relief groove \
                  -browsecommand [list $self _Active %W %S %s] \
                  -validate 1 -vcmd [list $self _Validate %S] \
                  -selecttitles 0 \
                  -titlecols 0 -titlerows 1 -colseparator \t -rowseparator \n \
                  -usecommand 1 -command [list $self _Command %i %C %s] \
                  -rowtagcommand [list $self _RowTag]
    
    set widget(tab) $tab
    
    puts btable:$self
    
    #variables containing types and formats and edited files are initialized
    set data(types) {}
    set data(formats) {}

    $self createwidget
    
    #options are configured from command line
    $self configure -path            [from args -path ""]
    $self configure -cols            [from args -cols ""]
    $self configure -rows            [from args -rows ""]
    $self configure -parent          [from args -parent ""]
    $self configure -toolbar         [from args -toolbar ""]
    $self configure -statusbar       [from args -statusbar ""]
    $self configure -function        [from args -function ""]
    $self configure -typelist        [from args -typelist ""]
    $self configure -defsavepath     [from args -defsavepath ""]
    $self configure -defopenpath     [from args -defopenpath ""]
    $self configure -internalvalues  [from args -internalvalues ""]
    $self configure -checkcolumn     [from args -checkcolumn ""]
    $self configure -closecmd        [from args -closecmd ""]
    $self configure -posteditcmd     [from args -posteditcmd ""]
    $self configure -iniconfig       [from args -iniconfig ""]
    $self configure -bgalt           [from args -bgalt ""]
    $self configure -bgalt1          [from args -bgalt1 ""]
    $self configure -bgalt2          [from args -bgalt2 ""]
    $self configure -toolenable      [from args -toolenable ""]
    $self configure -tooldisable     [from args -tooldisable ""]
    $self configure -postordercmd    [from args -postordercmd ""]
    

    $self configurelist $args

    #iniFile is readed
    $self _ReadIni

    if {[winfo class [$self cget -parent]] == "Toplevel"} {      
      wm protocol [$self cget -parent] WM_DELETE_WINDOW [list $self _CmdClose]
    } elseif  {[winfo class [$self cget -parent]] == "Mdidoc"} {
      [$self cget -parent] configure -closecmd [list $self _CmdClose]
    }
    # The widget is created
    set created 1
  }

  #///////////////////////////////////////////////////////////////////////////
  destructor { 
  #
  # PURPOSE: Destroy the trace asociated with -posteditcmd
  #
  #///////////////////////////////////////////////////////////////////////////
    trace vdelete options(-path) w [$self cget -posteditcmd]
  }

  #///////////////////////////////////////////////////////////////////////////
  #
  # PURPOSE: Methods cget & configure
  #
  #///////////////////////////////////////////////////////////////////////////    
  onconfigure -cols { arg } {
    set cols [$self cget -cols]
    if { $arg == 0 } {
      set arg 1
    }
    if { $cols > $arg } {
      set data(types)   [lreplace $data(types) $arg $cols]
      set data(formats) [lreplace $data(formats) $arg $cols]
    } elseif { $cols < $arg } {
      for { set i $cols } { $i < $arg } { incr i } {
	set data(types) [linsert $data(types) end text]
	set data(formats) [linsert $data(formats) end {}]
      }
    }
    $widget(tab) configure -cols $arg
  }

  onconfigure -rows { arg } {
    if { $arg == 0 } {
      set arg 1
    }
    $widget(tab) configure -rows $arg
    $self _UpdateInfoRows
  }

  oncget -cols {
    return [$widget(tab) cget -cols]
  }

  oncget -rows {
    return [$widget(tab) cget -rows]
  }


  onconfigure -function { lst } {
    set totitle [string totitle $lst]
    set options(-function) $totitle
    if {$created} {
      $self _UpdateFunc 
    }
  }

  onconfigure -path { lst } {
    set options(-path) $lst
    #When the path is changed, New or Open methods are triggered automaticly
    if {$lst == ""} {
      $self _New
    } else {
      $self _Open $lst
    }
  }
  
  onconfigure -toolbar { lst } {
    set options(-toolbar) $lst
    #The toolbar is hidden/shown
    if {$lst == 0} {
      grid remove $widget(tb)
    } else {
      grid $widget(tb)
    }
  }

  onconfigure -statusbar { lst } {
    set options(-statusbar) $lst
    #The statusbar is hidden/shown
    if {$lst == 0} {
      grid remove $widget(sb)
    } else {
      grid $widget(sb)
    }
  }

  onconfigure -typelist { lst } {
    set options(-typelist) $lst
  }

  onconfigure -defext { lst } {
    set options(-defext) $lst
  }

  onconfigure -defopenpath { lst } {
    set options(-defopenpath) $lst
  }

  onconfigure -defsavepath { lst } {
    set options(-defsavepath) $lst
  }

  onconfigure -internalvalues { lst } {
    set options(-checkcolumn) $lst
  }

  onconfigure -checkcolumn { lst } {
    set options(-checkcolumn) $lst
  }

  onconfigure -closecmd { lst } {
    set options(-closecmd) $lst
  }

  onconfigure -posteditcmd { lst } {
    if { $options(-posteditcmd) != "" } {
      #The previous trace is deleted
      trace vdelete options(-path) w $options(-posteditcmd)
    }
    set options(-posteditcmd) $lst
    #A trace to the new variable is added
    trace variable options(-path) w $options(-posteditcmd)
  }

  onconfigure -state { value } {
    #the -state option of the tkTable is changed
    $widget(tab) configure -state $value
    set options(-state) $value
    #edition button of the toolbar are shown/hidden
    set lst { replace ordAsc ordDes ordAdv }
    if {$value == "normal" } {
      foreach but $lst {
        $widget(tb).$but configure -state normal
      }
      $widget(sw).m entryconfigure 6 -state normal
      $widget(sw).m entryconfigure 8 -state normal
      $widget(sw).m entryconfigure 9 -state normal
    } else {
      foreach but $lst {
        $widget(tb).$but configure -state disabled
      }
      $widget(sw).m entryconfigure 6 -state disabled
      $widget(sw).m entryconfigure 8 -state disabled
      $widget(sw).m entryconfigure 9 -state disabled
    }
  }

  onconfigure -bgalt { arg } {
    set options(-bgalt) $arg
    if { $options(-bgalt) } {
      if { [expr ![$self tag exist bgalt1] && ![$self tag exist bgalt1]] } {
	$self tag configure bgalt1 -bg [$self cget -bgalt1]
	$self tag configure bgalt2 -bg [$self cget -bgalt2]	
      }
    } else {
      if { [expr [$self tag exist bgalt1] && [$self tag exist bgalt1]] } {
	$self tag delete bgalt1
	$self tag delete bgalt2
      }
    }  
    $self configure -padx [$self cget -padx]
  }

  onconfigure -bgalt1 { arg } {
    set options(-bgalt1) $arg
    $self tag configure bgalt1 -bg $arg
  }

  onconfigure -bgalt2 { arg } {
    set options(-bgalt2) $arg
    $self tag configure bgalt2 -bg $arg
  }

  onconfigure -toolenable { lst } {
    # Enable buttons on demand, by the given list.
    if { [llength $lst] != 0 } {
      $self _ToolEnable $lst
    }
  }

  onconfigure -tooldisable { lst } {
    # Enable buttons on demand, by the given list.
    if { [llength $lst] != 0 } {
      $self _ToolDisable $lst
    }
  }
  
  onconfigure -postordercmd { lst } {
    set options(-postordercmd) $lst
  }

  #///////////////////////////////////////////////////////////////////////////
  method createwidget { } {
  #
  # PURPOSE: The widget is created and packed
  #
  #///////////////////////////////////////////////////////////////////////////      
    #Table tags are configured
    $self tag configure title  -bd 1 -fg black -bg "light grey" -anchor c -state normal
    $self tag configure found  -bd 1 -fg black -bg yellow

    $self _CreateToolBar
    $self _CreateStatusBar
    $self _CreateMenu
    $self _CreateBinds

    #Links scrollwindow with the table 
    $widget(sw) setwidget $widget(tab)
    
    pack $widget(sw) -expand true -fill both    
    
    grid $widget(tb) -column 0 -row 0 -sticky ew
    grid $widget(fr) -column 0 -row 1 -sticky news
    grid $widget(sb) -column 0 -row 2 -sticky ew
    grid columnconfigure $self 0 -weight 1
    grid rowconfigure $self 1 -weight 1
  }
    
  #///////////////////////////////////////////////////////////////////////////
  method _CreateToolBar { } {
  #
  # PURPOSE: Create the toolbar with buttons icons and commands associated
  #          
  #///////////////////////////////////////////////////////////////////////////  
    set f $widget(tb)
    set n_b [list \
        "bnew"         "_CmdNew"     "[mc [list New white table]] (CTRL+N)"\
        "bopen"        "cmdOpen"    "[mc Open] (CTRL+O)"\
        "bsave"        "_CmdSave"    "[mc Save] (CTRL+S)"\
        "bsaveas"      "_CmdSaveAs"  "[mc "Save As"] (CTRL+G)"\
        "sep0"        "--"          ""\
        "home"        "_AdjustTable" "[mc "Adjust table"]"\
        "sep1"        "--"          ""\
        "leftEnd"     "_CmdFirst"    "[mc First]" \
        "reverse"    "_CmdBack"     "[mc Back] " \
        "play"   "_CmdNext"     "[mc Next] " \
        "rightEnd"    "_CmdLast"     "[mc Last] " \
        "sep2"        "--"          ""\
        "bcopy"        "_CmdCopy"     "[mc Copy] (CTRL+C)" \
        "bfind"        "_CmdFind"     "[mc Find] (CTRL+F)" \
        "replace"     "_CmdReplace"  "[mc Replace] (CTRL+R)" \
        "bfilter"      "_CmdFilter"   "[mc Filter] (ALT+F)" \
        "sep3"        "--"          ""\
        "ordAsc"      "_CmdOrdAsc"   "[mc "Order increasingly"]"\
        "ordDes"      "_CmdOrdDes"   "[mc "Order decreasingly"]"\
        "ordAdv"      "_CmdOrdAdv"   "[mc "Advanced order"]"\
        "sep4"        "--"          ""\
        "tools"       "_CmdOptions" "[mc Options]"\
        "sep5"        "--"          ""\
        "xl-logo"     "_CmdExcel"   "[mc Excel]"\
    ]
    foreach {n_img n_proc n_hint} $n_b {
      if {[string equal $n_proc "--"]} {
        Separator $f.$n_img -orient vertical
      } else {
        set widget(tb,$n_img) \
        [Button $f.$n_img -image [::Bitmap::get ${n_img}] -bd 2 \
                          -relief link -helptext $n_hint \
                          -command [list $self $n_proc ]]
      }
      
      pack $f.$n_img -side left -pady 2 -fill y -padx 1
    }
    $widget(tb,bfilter) configure -state disabled
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method _ToolEnable { lst } {
  #
  # PURPOSE: Enable the list group given by parameter
  #
  # PARAMETERS: list of groups of buttons to enable. Posible values are:
  #      file       -> first group of buttons (new, open, save)
  #      adjust     -> home button (used to adjust the table)
  #      navigation -> Navigation buttons (first,previous, next,last)
  #      edition    -> edition buttons (copy, find, replace, filter)
  #      order      -> ordering buttons (ascending, descending, advanced)
  #      options    -> table options button
  #      export     -> export group (export to excel)
  #      all        -> enable all groups of buttons
  #      none       -> disable all groups of buttons
  #
  #///////////////////////////////////////////////////////////////////////////
    # Buttons related with each option:
    set lstFile       { bnew bopen bsave}
    set lstAdjust     { home }
    set lstNavigation { leftEnd reverse play rightEnd }
    set lstEdition    { bcopy bfind replace bfilter }
    set lstOrder      { ordAsc ordDes ordAdv }
    set lstOptions    { tools }
    set lstExport     { xl-logo }
    
    set lstEnable {}
    
    foreach group $lst {
      # Fill the list of buttons to be enabled, without duplicates
      if { [string equal $group "file"] } {
        if { ([lsearch -exact -dictionary -ascii $lstEnable bnew] == -1) } {
          set lstAux [concat $lstEnable $lstFile]
          set lstEnable $lstAux
        }
      }
      if { [string equal $group "adjust"] } {
        if { ([lsearch -exact -dictionary -ascii $lstEnable home] == -1) } {
          set lstAux [concat $lstEnable $lstAdjust]
          set lstEnable $lstAux
        }
      }
      if { [string equal $group "navigation"] } {
        if { ([lsearch -exact -dictionary -ascii $lstEnable leftEnd] == -1) } {
          set lstAux [concat $lstEnable $lstNavigation]
          set lstEnable $lstAux
        }
      }
      if { [string equal $group "edition"] } {
        if { ([lsearch -exact -dictionary -ascii $lstEnable bcopy] == -1) } {
          set lstAux [concat $lstEnable $lstEdition]
          set lstEnable $lstAux
        }
      }
      if { [string equal $group "order"] } {
        if { ([lsearch -exact -dictionary -ascii $lstEnable ordAsc] == -1) } {
          set lstAux [concat $lstEnable $lstOrder]
          set lstEnable $lstAux
        }
      }
      if { [string equal $group "options"] } {
        if { ([lsearch -exact -dictionary -ascii $lstEnable tools] == -1) } {
          set lstAux [concat $lstEnable $lstOptions]
          set lstEnable $lstAux
        }
      }
      if { [string equal $group "export"] } {
        if { ([lsearch -exact -dictionary -ascii $lstEnable xl-logo] == -1) } {
          set lstAux [concat $lstEnable $lstExport]
          set lstEnable $lstAux
        }
      }      
      if { [string equal $group "all"] } {
        set lstEnable [concat $lstFile $lstAdjust $lstNavigation $lstEdition \
                      $lstOrder $lstOptions $lstExport]
      } 
      if { [string equal $group "none"] } {
        $self _ToolDisable all
      } 
    }
    
    foreach butt $lstEnable {
      $widget(tb,$butt) configure -state normal
    }
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method _ToolDisable { lst } {
  #
  # PURPOSE: Disable the list group given by parameter
  #
  # PARAMETERS: list of groups of buttons to disable. Posible values are:
  #      file       -> first group of buttons (new, open, save)
  #      adjust     -> home button (used to adjust the table)
  #      navigation -> Navigation buttons (first,previous, next,last)
  #      edition    -> edition buttons (copy, find, replace, filter)
  #      order      -> ordering buttons (ascending, descending, advanced)
  #      options    -> table options button
  #      export     -> export group (export to excel)
  #      all        -> disable all groups of buttons
  #      none       -> enable all groups of buttons
  #
  #///////////////////////////////////////////////////////////////////////////
    # Buttons related with each option:
    set lstFile       { bnew bopen bsave}
    set lstAdjust     { home }
    set lstNavigation { leftEnd reverse play rightEnd }
    set lstEdition    { bcopy bfind replace bfilter }
    set lstOrder      { ordAsc ordDes ordAdv }
    set lstOptions    { tools }
    set lstExport     { xl-logo }
    
    set lstDisable {}
    
    foreach group $lst {
      # Fill the list of buttons to be disabled, without duplicates
      if { [string equal $group "file"] } {
        if { ([lsearch -exact -dictionary -ascii $lstDisable bnew] == -1) } {
          set lstAux [concat $lstDisable $lstFile]
          set lstDisable $lstAux
        }
      }
      if { [string equal $group "adjust"] } {
        if { ([lsearch -exact -dictionary -ascii $lstDisable home] == -1) } {
          set lstAux [concat $lstDisable $lstAdjust]
          set lstDisable $lstAux
        }
      }
      if { [string equal $group "navigation"] } {
        if { ([lsearch -exact -dictionary -ascii $lstDisable leftEnd] == -1) } {
          set lstAux [concat $lstDisable $lstNavigation]
          set lstDisable $lstAux
        }
      }
      if { [string equal $group "edition"] } {
        if { ([lsearch -exact -dictionary -ascii $lstDisable bcopy] == -1) } {
          set lstAux [concat $lstDisable $lstEdition]
          set lstDisable $lstAux
        }
      }
      if { [string equal $group "order"] } {
        if { ([lsearch -exact -dictionary -ascii $lstDisable ordAsc] == -1) } {
          set lstAux [concat $lstDisable $lstOrder]
          set lstDisable $lstAux
        }
      }
      if { [string equal $group "options"] } {
        if { ([lsearch -exact -dictionary -ascii $lstDisable tools] == -1) } {
          set lstAux [concat $lstDisable $lstOptions]
          set lstDisable $lstAux
        }
      }
      if { [string equal $group "export"] } {
        if { ([lsearch -exact -dictionary -ascii $lstDisable xl-logo] == -1) } {
          set lstAux [concat $lstDisable $lstExport]
          set lstDisable $lstAux
        }
      }      
      if { [string equal $group "all"] } {
        set lstDisable [concat $lstFile $lstAdjust $lstNavigation $lstEdition \
                      $lstOrder $lstOptions $lstExport]
      } 
      if { [string equal $group "none"] } {
        $self _ToolEnable all
      } 
    }
    
    foreach butt $lstDisable {
      $widget(tb,$butt) configure -state disabled
    }
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method _CreateStatusBar { } {
  #
  # PURPOSE: Create the statusbar
  #
  #///////////////////////////////////////////////////////////////////////////
    set f $widget(sb)

    #Rows info
    set widget(sb,lRows) [label $f.rows -relief groove -bd 1 -width 16]
    #Position info
    set widget(sb,lPosi) [label $f.posi -relief groove -bd 1 -width 16]
    #Saved/NotSaved flag
    set widget(sb,lInfo) [label $f.info -relief groove -bd 1 -width 3]
    #Function applied to selected cells
    set widget(sb,lFunc) [label $f.func -relief groove -bd 1 -width 30 \
                                        -anchor w -padx 4]
    #Fill extra space
    set widget(sb,rell)  [label $f.rell -relief groove -bd 1]
  
    pack $f.rows $f.posi $f.info $f.func -side left
    pack $f.rell -expand true -fill x
    
    #Function menu (appears when mouse right button is clicked over function
    #label
    $self _CreateMenuFunc

    set f $widget(sb,lFunc)
    #bind that shows the menu
    bind $f <ButtonPress-3> \
                     [list $self _CmdOnB3Func $widget(menufunc) %W %x %y %X %Y]
  }

  #///////////////////////////////////////////////////////////////////////////
  method _CreateMenuFunc { } {
  #
  # PURPOSE: Create menu with all the possible functions for the selected
  #          cells
  #
  #///////////////////////////////////////////////////////////////////////////    
    #Creación de un menú contextual
    set menu [menu $widget(sb,lFunc).m -tearoff 0]
    set widget(menufunc) $menu
    #Sum
    $menu add command -label [mc "Sum"] \
          -command [list $self configure -function "sum"]
    #Average
    $menu add command -label [mc "Avg"] \
          -command [list $self configure -function "avg"]
    #Count
    $menu add command -label [mc "Count"] \
          -command [list $self configure -function "count"]
    #Count numbers
    $menu add command -label [mc "Count num"] \
          -command [list $self configure -function "countn"]
    #Max
    $menu add command -label [mc "Max"] \
          -command [list $self configure -function "max"]
    #Min
    $menu add command -label [mc "Min"] \
          -command [list $self configure -function "min"]
    #None
    $menu add command -label [mc "None"] \
          -command [list $self configure -function "none"]
  }

  
  #///////////////////////////////////////////////////////////////////////////
  method _CreateMenu { } {
  #
  # PURPOSE: Create the popup menu for the table
  #
  #///////////////////////////////////////////////////////////////////////////    
    #Creación de un menú contextual
    set menu [menu $widget(sw).m -tearoff 0]
    set widget(menu) $menu
    set mi [menu $widget(sw).m.ins -tearoff 0]
    set me [menu $widget(sw).m.eli -tearoff 0]
    set mt [menu $widget(sw).m.type -tearoff 0]
    set mc [menu $widget(sw).m.copy -tearoff 0]
    set mf [menu $widget(sw).m.func -tearoff 0]

    $menu add command -label ""
    $menu add cascade -label [mc "Set column type"] -menu $mt
    $menu add command -label [mc "Set column format"] \
          -command [list $self _CmdSetFormat]  
    $menu add separator
    $menu add cascade -label [mc "Special copy"] -menu $mc
    $menu add command -label [mc "Select all"] -accelerator CTRL+E\
          -command [list $self _CmdSelectAll]
    $menu add command -label [mc "Delete"] -accelerator SUPR \
          -command [list $self _CmdDelete]
    $menu add separator
    $menu add cascade -label [mc "Insert"] -menu $mi
    $menu add cascade -label [mc "Eliminate"] -menu $me
    $mi add command -label [mc "Rows"] \
          -command [list $self _CmdInsertRows]  
    $mi add command -label [mc "Columns"] \
          -command [list $self _CmdInsertCols]  
    $me add command -label [mc "Rows"] \
          -command [list $self _CmdDeleteRows]  
    $me add command -label [mc "Columns"] \
          -command [list $self _CmdDeleteCols]  
    $mt add command -label [mc "Date"] \
          -command [list $self _CmdSetType date]  
    $mt add command -label [mc "Text"] \
          -command [list $self _CmdSetType text]  
    $mt add command -label [mc "Real"] \
          -command [list $self _CmdSetType real]
    $mc add command -label [mc "Copy headers"] -accelerator CTRL+H \
          -command [list $self _CmdSpecialCopy headers]
    $mc add command -label [mc "Copy table"] -accelerator CTRL+A \
          -command [list $self _CmdSpecialCopy table]
    if { [TclInfoExists Set GuiFunctionTablePrj] } {
      $menu add separator
      $menu add cascade -label [mc "Functions"] -menu $mf    
    }
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method _CreateBinds { } {
  #
  # PURPOSE: Create binds
  #
  #///////////////////////////////////////////////////////////////////////////    
    set t $widget(tab)
    bind $t <ButtonPress-3> [list $self _CmdOnB3 $widget(menu) %W %x %y %X %Y]
    bind $t <Double-ButtonPress-1> [list $self _OnDblClickHeader]
    bind $t <F3>        [list $self _CmdFind]  
    bind $t <F4>        [list $self _CmdReplace]  
    bind $t <Delete>    [list $self _CmdDelete]
    bind $t <Return>    [list $self _CmdNext]
    bind $t <KP_Enter>    [list $self _CmdNext]
    bind $t <Control-KeyRelease> [list $self _OnControlKey %K %k]
  }

  #///////////////////////////////////////////////////////////////////////////
  method _OnControlKey {keysym keycode} {
  #
  # PURPOSE: Control the "Control + Key Press"
  #
  # PARAMETERS: keysym -> The keysym corresponding to the event, 
  #                       substituted as a textual string
  #             keycode -> The keycode field from the event
  #
  #///////////////////////////////////////////////////////////////////////////
    switch [string toupper $keysym] {
      E { $self _CmdSelectAll }
      F { $self _CmdFind }
      R { $self _CmdReplace }
      N { $self _CmdNew }
      O { $self cmdOpen }
      S { $self _CmdSave }
      X { $self _CmdCut }
      C { $self _CmdCopy }
      H { $self _CmdSpecialCopy headers }
      A { $self _CmdSpecialCopy table }
      V { $self _CmdPaste }
      J { puts $data(types) }
      K { puts $data(formats) }
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method _IsSaved { val } {
  #
  # PURPOSE: Prints information in statusbar about file save status.
  #
  # PARAMETERS: val -> Saved/NotSaved flag
  #
  #///////////////////////////////////////////////////////////////////////////
    if { [$self cget -state] != "disabled" } {
      set data(isSaved) $val
      if {$val == 0} {
        $widget(sb,lInfo) configure -text "*"
      } else {
        $widget(sb,lInfo) configure -text ""
      }
    }
  }
  
  #/////////////////////////////////////////////////////////////////////////////
  method _InitState {from} {
  #
  # PURPOSE: Initializes some table properties and variables according to the
  #          state of table editor.
  #
  # PARAMETERS:
  #   from  -> State that defines values for the variables
  #
  #/////////////////////////////////////////////////////////////////////////////
    switch $from {
      new {
        $self configure -titlecols 0
        $self configure -rows 5 -cols 4
        $self _IsSaved 1
        $self _UpdateInfoRows
        $self _UpdateTypes
      }
      open {
        $self configure -titlecols 0
        $self _IsSaved 1
        $self _UpdateInfoRows
        $self _UpdateTypes
        $self _AdjustTable
      }
      save {
        $self _IsSaved 1
      }
      matrix - autocor {
        $self _IsSaved 1
#        $self configure -state disabled
        $self configure -titlecols 1
        $self _UpdateInfoRows
        $self _UpdateTypes $from
        $self _AdjustTable
      }
      sql {
        $self _IsSaved 1
#        $self configure -state disabled
        $self configure -titlecols 0
        $self _UpdateInfoRows
        $self _AdjustTable
      }
      default {
        $self _IsSaved 1
#        $self configure -state disabled
        $self configure -titlecols 0
        $self _UpdateInfoRows
        $self _UpdateTypes $from
        $self _AdjustTable
      }
    }    
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method getMenu { } {
  #
  # PURPOSE: Return path to the menu
  #
  # RETURN: Path that points to menu
  #
  #///////////////////////////////////////////////////////////////////////////  
    return $widget(menu)
  }

  #///////////////////////////////////////////////////////////////////////////
  method getToolBar { } {
  #
  # PURPOSE: Return path to the toolbar
  #
  # RETURN: Path that points to toolbar
  #
  #///////////////////////////////////////////////////////////////////////////  
    return $widget(tb)
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method getTable { } {
  #
  # PURPOSE: Return path to the table
  #
  # RETURN: Path that points to table
  #
  #///////////////////////////////////////////////////////////////////////////  
    return $widget(tab)
  }

  #///////////////////////////////////////////////////////////////////////////
  method getStatusBar { } {
  #
  # PURPOSE: Return path to statusbar
  #
  # RETURN: Path that points to statusbar
  #
  #///////////////////////////////////////////////////////////////////////////  
    return $widget(sb)
  }

  #///////////////////////////////////////////////////////////////////////////
  method getLastCell { } {
  #
  # PURPOSE: Return last cell where right mouse button was clicked
  #
  # RETURN: Last cell where right mouse button was clicked
  #
  #///////////////////////////////////////////////////////////////////////////  
    return $data(lastrow),$data(lastcol)
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method getSelectedRows { } {
  #
  # PURPOSE: Return last cell where right mouse button was clicked
  #
  # RETURN: Last cell where right mouse button was clicked
  #
  #///////////////////////////////////////////////////////////////////////////  
    set selrows [$self curselection]
  }

  #///////////////////////////////////////////////////////////////////////////
  method _ReadIni { } {
  #
  # PURPOSE: Reads Ini file in case it exists and configures all the options
  #          according with the values read.
  #
  #///////////////////////////////////////////////////////////////////////////  
    global tcl_platform

    if { [info exist ::iniFile::idIni] && \
         [string length [$self cget -iniconfig]] } {
      set rini ::iniFile::Read
      set key [$self cget -iniconfig]
      $self configure -bg              [$rini $key bg "white"]
      $self configure -bgalt           [$rini $key bgalt 0]
      $self configure -bgalt1          [$rini $key bgalt1 "white"]
      $self configure -bgalt2          [$rini $key bgalt2 "white"]
      $self configure -font            [$rini $key font "helvetica 9"]
      if { [$rini $key colseparator "TAB"] == "TAB" } {
        $self configure -colseparator "\t"
      } elseif { [$rini $key colseparator "TAB"] == "RET" } {
        $self configure -colseparator "\n"
      } else {
        $self configure -colseparator    [$rini $key colseparator "\t"]
      }
      #col and row separators are stored in special way if they are \n or \t
      if { [$rini $key rowseparator "RET"] == "TAB" } {
        $self configure -rowseparator "\t"
      } elseif { [$rini $key rowseparator "RET"] == "RET" } {
        $self configure -rowseparator "\n"
      } else {
        $self configure -rowseparator    [$rini $key rowseparator "\n"]
      }
      $self configure -function        [$rini $key function "None"]
      #depending on the operating system the path is set
      if { [string equal $tcl_platform(platform) "windows"] } {
        $self configure  -defsavepath     [$rini $key defsavepath "c:\\"]
        $self configure  -defopenpath     [$rini $key defopenpath "c:\\"]
      } else {
        $self configure  -defsavepath     [$rini $key defsavepath "/"]
        $self configure  -defopenpath     [$rini $key defopenpath "/"]
      }
      
      $self configure  -internalvalues  [$rini $key internalvalues 0]
      $self configure  -checkcolumn     [$rini $key checkcolumn 0]
      # options find and replace
      set data(find,dir)     [$rini $key FindDir     forward]
      set data(find,case)    [$rini $key FindCase    0]
      set data(find,full)    [$rini $key FindFull    0]
      set data(replace,dir)  [$rini $key ReplaceDir  forward]
      set data(replace,case) [$rini $key ReplaceCase 0]
      set data(replace,full) [$rini $key ReplaceFull 0]            
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method _WriteIni { } {
  #
  # PURPOSE: Writes Ini file in case it exists with some of the current
  #          options of the table
  #
  #///////////////////////////////////////////////////////////////////////////  
    if { [info exist ::iniFile::idIni] && \
         [string length [$self cget -iniconfig]] } {
      set wini ::iniFile::Write
      set key [$self cget -iniconfig]
      foreach op {bg font function internalvalues \
                  checkcolumn defsavepath defopenpath bgalt bgalt1 bgalt2} {
        $wini $key $op [$self cget -$op]
      }
      #col and row separators are stored in special way if they are \n or \t      
      foreach op { rowseparator colseparator } {
        if { [$self cget -$op] == "\n" } {
          $wini $key $op RET
        } elseif { [$self cget -colseparator] == "\t" } {
          $wini $key $op TAB
        } else {
          $wini $key $op [$self cget -$op]
        }      
      }
      # options find and replace
      $wini $key FindDir     $data(find,dir)
      $wini $key FindCase    $data(find,case)    
      $wini $key FindFull    $data(find,full)
      $wini $key ReplaceDir  $data(replace,dir)
      $wini $key ReplaceCase $data(replace,case)
      $wini $key ReplaceFull $data(replace,full)
      ::iniFile::Flush
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method _New { } {
  #
  # PURPOSE: Open new blank document in the table editor
  #
  #///////////////////////////////////////////////////////////////////////////  
    $self configure -state normal
    $self _DeleteRows 0 [$self cget -rows]
    $self _DeleteCols 0 [$self cget -cols]
    for {set i 0} {$i < [$self cget -rows]} {incr i} {
      $self set 0,$i ""
    }
    $self _InitState new
  }

  #///////////////////////////////////////////////////////////////////////////
  method _Open { path } {
  #
  # PURPOSE: Open the document given in variable path
  #
  # PARAMETERS: path -> Document to be opened
  #
  #///////////////////////////////////////////////////////////////////////////  
    $self configure -state normal
    $self _DeleteRows 0 [$self cget -rows]
    $self _DeleteCols 0 [$self cget -cols]

    if { ![catch {set fd [open $path r] } msgerr] } {
      set txt [read -nonewline $fd]
      #Saved previous row and column separator
      set rsep [$self cget -rowseparator]
      set csep [$self cget -colseparator]
      #Set them to \n and ;
      $self configure -rowseparator "\n" -colseparator ";"
      # Call to table's paste handler with the opened file
      $self _PasteHandler 0,0 $txt
      $self configure -rowseparator $rsep -colseparator $csep
      #TOL files have last column separator after the last col,
      #that's why last column is deleted.
      set typecol [expr [$self cget -cols]-1]
      set data(types) [lreplace $data(types) $typecol $typecol]
      set data(formats) [lreplace $data(formats) $typecol $typecol]
      $self configure -cols [expr [$self cget -cols] -1]
      
      close $fd
      $self _InitState open
      return 1
    }
    error $msgerr
  }

  #///////////////////////////////////////////////////////////////////////////
  method _Sort { start end col args } {
  #
  # PURPOSE: Sort table. Modification of tk::table::Sort process.
  # PARAMETERS: start -> selection's top-left cell 
  #             end -> selection's bottom-right cell 
  #             col -> selection's index (simple selection)
  #             args -> extra sort arguments
  #
  #///////////////////////////////////////////////////////////////////////////  
    #Start row and column
    scan $start %d,%d sr sc
    #End row and column
    scan $end   %d,%d er ec
    
    if {($col < $sc) || ($col > $ec)} {
      return -code error "$col is not within sort range $sc to $ec"
    }
    
    set col [expr {$col - $sc}]

    set row {}
    upvar \#0 [varname matrix] matrix
    set selected {}
    for {set i $sr} {$i <= $er} {incr i} {
      for {set j $sc} {$j <= $ec} {incr j} {
        if { [info exists matrix($i,$j)] } {
          lappend row $matrix($i,$j)
        } else {
          lappend row {}
        }
      }
      lappend selected $row
      set row {}
    }
    
    if { $col == 0 && $args == "advorder" } {
      set i $sr
      foreach row [eval [list lsort -command [list $self _Compare] $selected]] {
        $self set row $i,$sc $row
        incr i
      }
    } else {
      set i $sr
      foreach row [eval [list lsort -index $col] $args [list $selected]] {
        $self set row $i,$sc $row
        incr i
      }
    }
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method _Compare { a b {order ""}} {
  #
  # PURPOSE: Compare two rows and decide how to order them.
  #
  # PARAMETERS: a -> First row to be compared
  #             b -> Second row to be compared
  #             order -> List that shows how to compare rows.
  #                      Example: { {1 inc} {3 dec} {2 inc} }
  #                      will order first by the first column increasingly,
  #                      then by the thrird decreasingly
  #                      and finally by the second increasingly
  #          
  # RETURN: -1 if a should go first
  #          1 if b should go first
  #
  #///////////////////////////////////////////////////////////////////////////  
    if {$order == ""} {
      set order $data(advorder) 
    } else {
      set order $order
    }
    #the first order method is got
    set firstorder [lindex $order 0]
    
    #Col and way are got
    set col [lindex $firstorder 0]

    set func [lindex $firstorder 2]

    set a0 [lindex $a $col]
    set b0 [lindex $b $col]

    #The elements are compared
    if       { ($a0 < $b0) && ($func == "inc") } {
      return -1
    } elseif { ($a0 > $b0) && ($func == "inc") } {
      return 1
    } elseif { ($a0 < $b0) && ($func == "dec") } {
      return 1
    } elseif { ($a0 > $b0) && ($func == "dec") } {
      return -1
    #If they are the same we should try with the next order method recursively
    } else {
      set neworder [lrange $order 1 end]
      if {[llength $neworder] > 0} {
        return [$self _Compare $a $b $neworder]
      #If there are the same.. the order doesnt matter
      } else {
        return -1
      }
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method _Active { W S s } {
  #
  # PURPOSE: The method is triggered when the active cell changes.
  #          Changes the active cell and recalculates function applied to
  #          selected cells.
  # PARAMETERS: W -> Pathname to the window for which the command was
  #                  generated.
  #             S -> The index of the new active cell.
  #             s -> Is the index of the last active cell.
  #
  #///////////////////////////////////////////////////////////////////////////  
    #Updates information of the current row,col
    set row [lindex [split $S ,] 0]
    set col [lindex [split $S ,] 1]
    $widget(sb,lPosi) configure -text "[mc Row]: $row, [mc Col]: $col"

    #if only one file is selected the selection is updated, otherwise function
    #would be calculated with the previous selected cell.
    if {[llength [$self curselection]] == 1 } {
      $self selection clear all
      $self selection set active
    }
    
    #call to function calculation
    if { [$self cget -function] == "None" } {
      $widget(sb,lFunc) configure -text "[mc None]"
    } else {
      $self _UpdateFunc
    }
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method _InsertCols { side index count typ } {
  #
  # PURPOSE: Inserts columns and manages types when inserting
  # PARAMETERS: side -> side where the columns will be inserted.
  #             index -> Index from which the columns will be inserted.
  #             count -> Number of columns to be inserted.
  #             typ -> Type of the columns to be inserted.
  #
  #///////////////////////////////////////////////////////////////////////////
    #Types and formats are stored in temporaly variables.
    set temptype $data(types)
    set tempformat $data(formats)
    
    #Columns with the type date and real are changed temporaly to text.
    set dates [lsearch -all $data(types) date]
    set reals [lsearch -all $data(types) real]
    foreach i $dates {
      $self setType $i text
    }
    foreach i $reals {
      $self setType $i text
    }
    #Types for the new columns are inserted
    for { set i 0 } { $i < $count } { incr i } {
      set data(types) [linsert $data(types) end text]
      set data(formats) [linsert $data(formats) end {}]
    }
    #Then the columns are inserted
    if { $side == "left" } {
      $self insert cols $index -$count
      for { set i 0 } { $i < $count } { incr i } {
        set temptype [linsert $temptype $index $typ]
        set tempformat [linsert $tempformat $index {}]
      }
    } else {
      $self insert cols $index $count
      for { set i 0 } { $i < $count } { incr i } {
        set temptype [linsert $temptype [expr $index+1] $typ]
        set tempformat [linsert $tempformat [expr $index+1] {}]
      }
    }
    #Previous types are restored + the new types.
    set data(types) $temptype
    set data(formats) $tempformat
    $self _IsSaved 0
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method _InsertRows { side index count } {
  #
  # PURPOSE: Inserts rows
  # PARAMETERS: side -> side where the rows will be inserted.
  #             index -> Index from which the rows will be inserted.
  #             count -> Number of rows to be inserted.
  #
  #///////////////////////////////////////////////////////////////////////////
    if { $side == "up" } {
      $self insert rows $index -$count
    } else {
      $self insert rows $index $count
    }
    $self _UpdateInfoRows
    $self _IsSaved 0
  }

  #///////////////////////////////////////////////////////////////////////////
  method _DeleteCols { colIni cols } {
  #
  # PURPOSE: Deletes columns and manages types when deleting
  # PARAMETERS: colIni -> Initial column to be deleted
  #             cols -> Number of columns to be deleted
  #
  #///////////////////////////////////////////////////////////////////////////
    #Types and formats are stored in temporaly variables.
    set temptype $data(types)
    set tempformat $data(formats)

    #Columns with the type date and real are changed temporaly to text.    
    set dates [lsearch -all $data(types) date]
    set reals [lsearch -all $data(types) real]
    foreach i $dates {
      $self setType $i text
    }
    foreach i $reals {
      $self setType $i text
    }
    #The columns are deleted
    $self delete cols $colIni $cols
    #The types too
    set temptype [lreplace $temptype $colIni [expr $colIni+$cols-1]]            
    set tempformat [lreplace $tempformat $colIni [expr $colIni+$cols-1]]
    #whole table cant be deleted, there is always at least one cell left
    #so if the user selects to delete all colums the text type and empty
    #format are added
    if { $temptype == "" } {
      set temptype [linsert $temptype end text]
      set tempformat [linsert $tempformat end {}]
    }    
    
    #types are restored
    set data(types) $temptype
    set data(formats) $tempformat

    $self _IsSaved 0
  }

  #///////////////////////////////////////////////////////////////////////////
  method _DeleteRows { rowIni rows } {
  #
  # PURPOSE: Deletes rows.
  # PARAMETERS: rowIni -> Initial row to be deleted
  #             rows -> Number of rows to be deleted
  #
  #///////////////////////////////////////////////////////////////////////////   
    set lista [$self _GetSelectedRow]
   
    if {[llength $lista]} {
      set aux 0
      foreach idx $lista {
        if {$aux == 0} {
          $self delete rows $idx 1
        } else {
          $self delete rows [expr $idx - $aux] 1
        }
        incr aux
      }
  
      $self _UpdateInfoRows
      $self _IsSaved 0
    }
  }
  
  #/////////////////////////////////////////////////////////////////////////////
  method _GetSelectedRow { args } {
  #
  # PURPOSE: Crear la leyenda de colores
  # 
  #/////////////////////////////////////////////////////////////////////////////
    variable widget
    variable data
    
    set selRowAux [$self getSelectedRows]
    set lst {}
    
    if {[llength $selRowAux]} {
      foreach it $selRowAux {
        if {[llength $lst]} {
          if {([lindex [split [lindex $it 0] ,] 0]) != ([lindex $lst [expr [llength $lst] - 1]])} {
            lappend lst [lindex [split [lindex $it 0] ,] 0]
          }
        } else {
          lappend lst [lindex [split [lindex $it 0] ,] 0]
        }
      }
    }
  
    return $lst
  }

  #///////////////////////////////////////////////////////////////////////////
  method _Command { set cell val } {
  #
  # PURPOSE: Returns the value that will be shown for the cell in case of get
  #          or the value that will be saved in the matrix in case of set.
  # PARAMETERS: set -> Flag that shows the function applied to the cell (get/set)
  #             cell -> The cell
  #             val -> The value (in case of set)
  #
  #///////////////////////////////////////////////////////////////////////////
    upvar \#0 [varname matrix] matrix
    #The column and type are gotten
    set col [lindex [split $cell ,] 1]
    set type [lindex $data(types) $col]
    #SET
    if {$set} {
      switch $type {
        text {
          set matrix($cell) $val
        }
        date {
          $self _InsertDate $col $cell $val
        }
        real {
          $self _InsertReal $col $cell $val
        }
      }
    #GET
    } else {
      if {[info exists matrix($cell)]} {
        switch $type {
          text {
            return $matrix($cell)
          }
          date {
            return [$self _ShowDate $col $cell]
          }
          real {
            return [$self _ShowReal $col $cell]
          }
        }
      } else {
        return
      } 
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method _InsertReal { col cell val } {
  #
  # PURPOSE: Inserts a real
  #
  # PARAMETERS: col -> Column in which the real will be inserted
  #             cell -> The cell
  #             val -> The value
  #
  #///////////////////////////////////////////////////////////////////////////
    upvar \#0 [varname matrix] matrix
    #When value is empty or ? nothing is inserted
    if { ($val == "") || ($val == "?") } {
      set matrix($cell) $val
    #In case of title cells
    } elseif { [$self tag includes title $cell] } {
      set matrix($cell) $val
    #In case of double
    } elseif { [string is double $val] } {
      set matrix($cell) $val
    #In other case the type must be changed to text or the value wont be
    #inserted
    } else {
      set rq [tk_messageBox -type "yesno" -icon warning \
       -title [mc "Warning"] -parent $self\
       -message "[mc CellNotReal].\n\
       [mc LikeChangeTypeText]"]
      switch -exact $rq {
        yes {
          $self setType $col text
          set matrix($cell) $val
        }
        default {
          return
        }
      }
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method _InsertDate { col cell val } {
  #
  # PURPOSE: Inserts a date
  #
  # PARAMETERS: col -> Column in which the date will be inserted
  #             cell -> The cell
  #             val -> The value
  #
  #///////////////////////////////////////////////////////////////////////////
    upvar \#0 [varname matrix] matrix
    #When value is empty nothing is inserted
    if { ($val == "") } {
      set matrix($cell) $val
    #In case of title cell   
    } elseif { [$self tag includes title $cell] } {
      set matrix($cell) $val
    #if the val validates regular expresion is a date
    } elseif { [regexp {(^([0-9]{4})(/(0[1-9]|1[0-2])(/(0[1-9]|[12][0-9]|3[01])(:([0-1][0-9]|2[0-3])(:([0-5][0-9])(:([0-5][0-9])(\.[0-9]{0,3})?)?)?)?)?)?$)|(^(y[0-9]{4})(m(0[1-9]|1[0-2])(d(0[1-9]|[12][0-9]|3[01])(h([0-1][0-9]|2[0-3])(i([0-5][0-9])(s([0-5][0-9])(\.[0-9]{0,3})?)?)?)?)?)?$)|(^([0-9]{4})(-(0[1-9]|1[0-2])(-(0[1-9]|[12][0-9]|3[01])( ([0-1][0-9]|2[0-3])(:([0-5][0-9])(:([0-5][0-9])(\.[0-9]{0,3})?)?)?)?)?)?$)} \
                $val] } {
      set matrix($cell) $val
    #In other case the type must be changed to text or the value wont be
    #inserted
    } else {
      set rq [tk_messageBox -type "yesno" -icon warning \
       -title [mc "Warning"] -parent $self\
       -message "[mc CellNotDate].\n\
       [mc LikeChangeTypeText]"]
      switch -exact $rq {
        yes {
          $self setType $col text
          set matrix($cell) $val
        }
        default {
          return
        }
      }
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method _ShowReal { col cell } {
  #
  # PURPOSE: Shows a real in the column format
  #
  # PARAMETERS: col -> Column in which the date will be shown
  #             cell -> The cell
  #
  #///////////////////////////////////////////////////////////////////////////
    upvar \#0 [varname matrix] matrix
    #When the cell is empty or contains ? nothing is returned
    if { ($matrix($cell) == "") || ($matrix($cell) == "?") } {
      return
    #In case the cell is a title
    } elseif { [$self tag includes title $cell] } {
      return $matrix($cell)
    #In case the cell is double    
    } elseif { [string is double $matrix($cell)] } {

      set format [lindex $data(formats) $col]
      #The format is applied
      if {$format == ""} {
        return $matrix($cell)
      } else {
        return [format $format $matrix($cell)]
      }      
    #In other case the type is changed to text 
    } else {
      $self setType $col text
      if { 0 } {
        set rq [tk_messageBox -type "ok" -icon warning \
                    -title [mc "Warning"] -parent $widget(tab)\
                    -message "[mc SomeCellNotReal].\n\
       [mc TypeSetText]"]
      }
      return $matrix($cell)
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method _ShowDate { col cell } {
  #
  # PURPOSE: Shows a date in the column format
  #
  # PARAMETERS: col -> Column in which the date will be shown
  #             cell -> The cell
  #
  #///////////////////////////////////////////////////////////////////////////
    upvar \#0 [varname matrix] matrix
    #When the cell is empty or contains ? nothing is returned    
    if { ($matrix($cell) == "") } {
      return
    #In case the cell is a title
    } elseif { [$self tag includes title $cell] } {
      return $matrix($cell)
    #In case the cell is a date
    } elseif { [regexp {(^([0-9]{4})(/(0[1-9]|1[0-2])(/(0[1-9]|[12][0-9]|3[01])(:([0-1][0-9]|2[0-3])(:([0-5][0-9])(:([0-5][0-9])(\.[0-9]{0,3})?)?)?)?)?)?$)|(^(y[0-9]{4})(m(0[1-9]|1[0-2])(d(0[1-9]|[12][0-9]|3[01])(h([0-1][0-9]|2[0-3])(i([0-5][0-9])(s([0-5][0-9])(\.[0-9]{0,3})?)?)?)?)?)?$)|(^([0-9]{4})(-(0[1-9]|1[0-2])(-(0[1-9]|[12][0-9]|3[01])( ([0-1][0-9]|2[0-3])(:([0-5][0-9])(:([0-5][0-9])(\.[0-9]{0,3})?)?)?)?)?)?$)} \
                $matrix($cell)] } {
      #Is converted to iso8601
      regsub  {[:h ]} $matrix($cell) "T" dateT
      regsub  {\.(.){0,3}$} $dateT "" withoutMS
      regsub -all {[/\.ymdis\-]} $withoutMS "" iso8601
      
      set mask 00000101T00:00:00
      set addmask [string range $mask [string length $iso8601] end]
      
      set res [join "$iso8601 $addmask" ""]
      #The format is applied
      set format [lindex $data(formats) $col]
      
      if {$format == ""} {
        return $matrix($cell)
      } else {
        return [clock format [clock scan $res] -format $format]
      }      
    #In other case the type is changed to text 
    } else {
      $self setType $col text
      if { 0 } {
      set rq [tk_messageBox -type "ok" -icon warning \
       -title [mc "Warning"] -parent $self\
       -message "[mc SomeCellNotDate].\n\
       [mc TypeSetText]"]
      }
      return $matrix($cell)
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method _UpdateFunc { } {
  #
  # PURPOSE: Updates the result of the function applied to the selected cells
  #
  #///////////////////////////////////////////////////////////////////////////
    if {[llength [$self curselection]] > 0 } {
      #Row and col separator are temporaly changed
      set rsep [$self cget -rowseparator]
      set csep [$self cget -colseparator]
      $self configure -rowseparator " " -colseparator " "
      
      if {[catch {set sel [::tk::table::GetSelection $self]} err]} {
        set total 0
        $widget(sb,lFunc) configure -text "[mc [$self cget -function]]: $total"
        $self configure -rowseparator $rsep -colseparator $csep
        return
      }
  
      switch [$self cget -function] {
        Sum { $self _Sum $sel }
        Avg { $self _Avg $sel }
        Count { $self _Count $sel }
        Countn { $self _Countn $sel }
        Max { $self _Max $sel }
        Min { $self _Min $sel }
      }
      #Row and col separator are restored      
      $self configure -rowseparator $rsep -colseparator $csep
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method _Sum { sel } {
  #
  # PURPOSE: Applies sumatory function to the selection
  #
  # PARAMETERS: sel -> The selection
  #
  #///////////////////////////////////////////////////////////////////////////
    set total 0
  
    foreach ele $sel {
      if { [string is double $ele] && [string length $ele] > 0 } {
        set total [expr ($total + $ele)]
      }
    }
    $widget(sb,lFunc) configure -text "[mc Sum]=$total"
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method _Avg { sel } {
  #
  # PURPOSE: Applies average function to the selection
  #
  # PARAMETERS: sel -> The selection
  #
  #///////////////////////////////////////////////////////////////////////////
    set total 0
    set i 0

    foreach ele $sel {
      if { [string is double $ele] && [string length $ele] > 0 } {
        set i [expr ($i + 1)]
        set total [expr ($total + $ele)]
      }
    }
    if { $i != 0 } {
      set total [expr ($total / $i)]
    }
    $widget(sb,lFunc) configure -text "[mc Avg]=$total"
  }

  #///////////////////////////////////////////////////////////////////////////
  method _Count { sel } {
  #
  # PURPOSE: Applies count function to the selection
  #
  # PARAMETERS: sel -> The selection
  #
  #///////////////////////////////////////////////////////////////////////////
    set total 0
    
    foreach ele $sel {
      if {[string length $ele] > 0 } {
        set total [expr ($total + 1)]          
      }
    }
    $widget(sb,lFunc) configure -text "[mc Count]=$total"        
  }

  #///////////////////////////////////////////////////////////////////////////
  method _Countn { sel } {
  #
  # PURPOSE: Applies count numbers function to the selection
  #
  # PARAMETERS: sel -> The selection
  #
  #///////////////////////////////////////////////////////////////////////////
    set total 0
    
    foreach ele $sel {
      if { [string is double $ele] && [string length $ele] > 0 } {
        set total [expr ($total + 1)]          
      }
    }
    $widget(sb,lFunc) configure -text "[mc Countn]=$total"        
  }

  #///////////////////////////////////////////////////////////////////////////
  method _Max { sel } {
  #
  # PURPOSE: Applies maximum function to the selection
  #
  # PARAMETERS: sel -> The selection
  #
  #///////////////////////////////////////////////////////////////////////////
    set total 0
    set i 0
    
    foreach ele $sel {
      if { [string length $ele] > 0 } {
        if { $i == 0 } {
          set total $ele
          set i 1
        } else {
          if { $ele > $total } {
            set total $ele
          }
        }
      }
    }
    $widget(sb,lFunc) configure -text "[mc Max]=$total"        
  }

  #///////////////////////////////////////////////////////////////////////////
  method _Min { sel } {
  #
  # PURPOSE: Applies minimum function to the selection
  #
  # PARAMETERS: sel -> The selection
  #
  #///////////////////////////////////////////////////////////////////////////
    set total 0
    set i 0
    
    foreach ele $sel {
      if { [string length $ele] > 0 } {
        if { $i == 0 } {
          set total $ele
          set i 1
        } else {
          if { $ele < $total } {
            set total $ele
          }
        }
      }
    }
    $widget(sb,lFunc) configure -text "[mc Min]=$total"        
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method _UpdateInfoRows { } {
  #
  # PURPOSE: Rows info is updated.
  #
  #///////////////////////////////////////////////////////////////////////////
    set dataRows [$self cget -rows]
    set titleRows [$self cget -titlerows]   
    set text "$dataRows [mc rows] ($titleRows [mc title])"
    $widget(sb,lRows) configure -text $text
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method _UpdateTypes { {from ""} } {
  #
  # PURPOSE: Sets default types depending on the type of the varible
  #          Pre-defined types are:
  #          serie: First column date the other real
  #          matrix: all columns real
  #          default: all columns text
  #
  # PARAMETERS: from -> type of the table that is being configured
  #
  #///////////////////////////////////////////////////////////////////////////
    set data(types) ""
    set data(formats) ""

    switch $from {
      serie {
        #First column date
        set data(types) [linsert $data(types) 0 date]
        set data(formats) [linsert $data(formats) 0 {}]
        #The others real
        for {set i 1} {$i < [$self cget -cols]} {incr i} {
          set data(types) [linsert $data(types) $i real]
          set data(formats) [linsert $data(formats) $i {}]
        }
      }
      matrix {
        #All columns real
        for {set i 0} {$i < [$self cget -cols]} {incr i} {
          set data(types) [linsert $data(types) $i real]
          set data(formats) [linsert $data(formats) $i {}]
        }
      }
      default {
        #All columns text
        for {set i 0} {$i < [$self cget -cols]} {incr i} {
          set data(types) [linsert $data(types) $i text]
          set data(formats) [linsert $data(formats) $i {}]
        }
      }
    }    
  }

  #///////////////////////////////////////////////////////////////////////////
  method _OnDblClickHeader { } {
  #
  # PURPOSE: Adjusts column width when header is double-clicked
  #
  #///////////////////////////////////////////////////////////////////////////
    #Selected cell
    if { ![catch {set cell [$self index active]} msgerr] } {
      set row [lindex [split $cell ,] 0]
      set col [lindex [split $cell ,] 1]
      #Check if the header was clicked
      if {$row == 0} {
        $self _AdjustColumn $col
      }
    }
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method _AdjustTable { {spans ""} } {
  #
  # PURPOSE: Ajusts all columns width with or without spans, depending on the
  #          parameter.
  # PARAMETERS: spans -> "spans" in case spans is required
  #                      "" if no spans and is required
  #
  #///////////////////////////////////////////////////////////////////////////
    if { $spans == "spans"} {
      for {set i [$self cget -colorigin]} {$i < [$self cget -cols]} {incr i} {
        $self _AdjustColumnSpans $i
      }
    } else {
      for {set i [$self cget -colorigin]} {$i < [$self cget -cols]} {incr i} {
        $self _AdjustColumn $i
      }
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method _AdjustColumn { column {first 50} {last 200} } {
  #
  # PURPOSE: Adjusts column width for the previous and next rows given in
  #          parameters
  #
  # PARAMETERS: column -> The column to be adjusted
  #             first -> Number of previous rows to adjust
  #             last ->  Number of next rows to adjust
  #///////////////////////////////////////////////////////////////////////////
    set maxLenFin 1
    
    set topleft [$self index topleft]
    set topleftrow [lindex [split $topleft ,] 0]
    
    set firstrow [expr $topleftrow - $first]
    set lastrow  [expr $topleftrow + $last]
    
    if {$firstrow < 0} {
      set firstrow [$self cget -roworigin]
    }
    if {$lastrow > [$self cget -rows]} {
      set lastrow [$self cget -rows]
    }
    for {set i $firstrow} {$i <= $lastrow} {incr i} {
      set maxLen [string length [$self get $i,$column]]
      # check if the cell is horizontally expanded
      if {$maxLen > $maxLenFin} {
        set maxLenFin $maxLen
      } 
    }
    # Spans control
    $self width $column [expr $maxLenFin + 1]
  }

  #///////////////////////////////////////////////////////////////////////////
  method _AdjustColumnSpans { column } {
  #
  # PURPOSE: Adjusts column width for whole table
  #
  # PARAMETERS: column -> The column to be adjusted
  #
  #///////////////////////////////////////////////////////////////////////////
    # Spans control
    set find 0
    set lstSpan [$self spans]
    if { [llength $lstSpan] } {
      set find 1  
      set lstInd ""
      set ok 1
      # list with the index
      for {set x 0} {$x < [expr [llength $lstSpan] -1]} {incr x 2} {
            lappend lstInd [lindex $lstSpan $x]
      }
    }
    # Search of the column width
    set maxLenFin 1
    for {set i [$self cget -roworigin]} {$i <= [$self cget -rows]} {incr i} {
      set maxLen [string length [$self get $i,$column]]
      # check if the cell is horizontally expanded
      if { $find } {
        set posi [lsearch $lstInd $i,$column]
        if [expr $posi > -1] {
          set fc [lindex $lstSpan [expr $posi * 2 + 1]]
          set nc [string range $fc [expr [string first , $fc] + 1] end]
          set maxLen [expr $maxLen / ($nc+1) + 3] 
        }
      }
      if {$maxLen > $maxLenFin} {
        set maxLenFin $maxLen
      } 
    }
    # Spans control
    $self width $column [expr $maxLenFin + 1]
  }

  #///////////////////////////////////////////////////////////////////////////
  method _Validate { val } {
  #
  # PURPOSE: Validates each keystroke when typing in a cell
  #
  # PARAMETERS: val -> The value to be validated
  #
  #///////////////////////////////////////////////////////////////////////////
    if [regexp {^[^(\n|\r)]*(\n|\r)$} $val] {
      return 0
    } else {
      $self _IsSaved 0
      return 1
    }
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method _CmdOnB3 { m w x y X Y } {
  #
  # PURPOSE: Shows contextual menu when right mouse button is clicked
  #
  # PARAMETERS: m -> The menu
  #             w -> The parent
  #             x -> The absolute position
  #             y -> The absolute postion
  #             X -> The relative position
  #             Y -> The relative position
  #
  #///////////////////////////////////////////////////////////////////////////
    if ![llength [$self border mark $x $y]] {
      set cell [$self index @$x,$y]
      #Last row and col where right button was pressed is saved
      set data(lastrow) [lindex [split $cell ,] 0]
      set data(lastcol) [lindex [split $cell ,] 1]
      #The type is retrieved
      set type [lindex $data(types) $data(lastcol)]
      $widget(menu) entryconfigure 0 \
        -label [mc "[string toupper $type]"]
      if { $type == "text" } {
        $widget(menu) entryconfigure 2 -state disabled
      } else {
        $widget(menu) entryconfigure 2 -state normal
      }

      set sel [$self curselection]
      #Column of the first cell in selection
      set firstcol [lindex [split [lindex $sel 0] ,] 1]
      #Column of the last cell in selection
      set lastcol  [lindex [split [lindex $sel end] ,] 1]
      #if more than one column is selected or selection is empty functions
      #are disabled
      if { [TclInfoExists Set GuiFunctionTablePrj] } {
	if { ($sel == "") } {
	  $widget(menu) entryconfigure 11 -state disabled
	} else {
	  $widget(menu) entryconfigure 11 -state normal
	  $self _UpdateMenuFunc
	}
      }
      tk_popup $m $X $Y
    }
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method _UpdateMenuFunc { } {
  #
  # PURPOSE: Updates menu function
  #
  #///////////////////////////////////////////////////////////////////////////
    #Primero destruye los menus previos en caso de que existan
    if { [winfo exist $widget(menu).func] } {
      foreach child [winfo children $widget(menu).func] {
	destroy $child
      }
      destroy $widget(menu).func
    }
    #Crea el menu de las funciones
    set mc [menu $widget(menu).func -tearoff 0]

    #obtiene el tipo de la columna
    set type [lindex $data(types) $data(lastcol)]

    #objeto tol con las funciones del usuario
    set obj GuiFunctionTablePrj

    set lstDef [GetGlobalUniqueName __lstDef]
    set tolCmd "Text $lstDef = TolSet2TclLst($obj);"
    TclTolEval $tolCmd
    eval "set lstUserDef [TclGetVar Text $lstDef]"
    ::tol::console stack release $lstDef
    
    foreach func $lstUserDef {
      set nameFun  [lindex $func 0]
      set nameGui  [lindex $func 1]
      set typeIn   [lindex $func 2]
      set typeOut  [lindex $func 3]
      
      if {[string tolower $typeIn] == [string tolower $type]} {
	set m [menu $widget(menu).func.[string tolower $nameFun] -tearoff 0]
	$widget(menu).func add cascade -label $nameGui -menu $m
	$m add command -label [mc "Graph"] \
           -command [list $self _CompileMenuFunc $typeOut $nameFun $typeIn $nameGui chart]
	$m add command -label [mc "Table"] \
           -command [list $self _CompileMenuFunc $typeOut $nameFun $typeIn $nameGui table]
	$m add command -label [mc "Compile"] \
           -command [list $self _CompileMenuFunc $typeOut $nameFun $typeIn $nameGui]
      }
      if {[string tolower $typeIn] == "set"} {
	$widget(menu).func add command -label $nameGui \
	  -command [list $self _CompileMenuFunc $typeOut $nameFun $typeIn $nameGui]
      }
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method _CompileMenuFunc { typeOut nameFun typeIn nameGui {func ""} } {
  #
  # PURPOSE: Creates TOL object with the given parameters.
  #          It can also tabulate and show charts
  # PARAMETERS: typeOut -> Type that returns TOL object
  #             nameFun -> Name that will have the object
  #             typeIn  -> Type of the parameter
  #             nameGui -> Name used in the GUI
  #             func    -> "chart" creates a graph
  #                        "table" creates a table
  #                        "" just compiles
  #
  #///////////////////////////////////////////////////////////////////////////
    #if the typeOut is SetOf* is changed to Set
    if { [regexp {^SetOf} $typeOut] } {
      set typeOutRes "Set"
    } else {
      set typeOutRes $typeOut
    }
    
    if { $typeIn == "Set" } {
      set paramHeaders "[string trim [$self get 0,0 0,[expr [$self cget -cols] -1]]]"
      set paramHeadersAux {}
      foreach it $paramHeaders {
        lappend paramHeadersAux "\"$it\""
      }
     
      set sSetHeader {}
      if {[llength $paramHeadersAux]} {
        set sSetHeader "SetOfText([join $paramHeadersAux ,])"
        set par1 [$self _CreateSetForFunction]
        set lstAux {}
        lappend lstAux $sSetHeader
        lappend lstAux $par1
        set param "SetOfSet([join $lstAux ,])"
      } else {
        set param [$self _CreateSet]
      }
            
      set function [GetGlobalUniqueName $nameFun]
      set tolCmd "$typeOutRes $function = ${nameFun}($param)"
      
      TclTolEval $tolCmd
      
      ::TolInspector::UpdateConsoleObj
      
      ::TolConsole::RaiseInspector      
    } else {
      #Selection, type of the column and format are got
      set sel [$self curselection]
      set type [lindex $data(types) $data(lastcol)]
      set saveformat [lindex $data(formats) $data(lastcol)]
      
      #If is a date the format is changed temporaly to TOL format
      if { $type == "date" } {
	$self setType $data(lastcol) date y%Ym%md%dh%Hi%Ms%S.000
      }
      
      #The value of each cell is got
      foreach cell $sel {
	lappend lst [$self get $cell] 
      }
      
      #Format is restored
      if { $type == "date" } {
	$self setType $data(lastcol) date $saveformat
      }
      
    #The parameters of the object are built with typeIn
      set param [TclLst2TolSet $lst -level 1 \
		   -grammar [string totitle [string tolower $typeIn]]]
      
      set function [GetGlobalUniqueName $nameFun]
      set tolCmd "$typeOutRes $function = ${nameFun}($param)"
      
      TclTolEval $tolCmd
      
      ::TolInspector::UpdateConsoleObj
      
      if { $func != "" } {
	if { $typeOut == "Serie" } {
	  set tolCmd "Text SaddVisuConceptView(SetOfSerie($function),\"$nameGui\",\
                                            GCFFile,DefFirst,DefLast,\
                                            \"$func\",\"$typeOut\");"
	} else {
	  set tolCmd "Text SaddVisuConceptView($function,\"$nameGui\",\
                                            GCFFile,DefFirst,DefLast,\
                                            \"$func\",\"$typeOut\");"
	}
	TclTolEval $tolCmd
	
      } else {
	::TolConsole::RaiseInspector
      }
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method _CreateSet { } {
  #
  # PURPOSE: Returns the syntax to create TOL object.
  # PARAMETERS:
  #
  #///////////////////////////////////////////////////////////////////////////
    #Gets current selection
    set cells [$self curselection]
    #Inicialize lists
    set rows {} 
    set sSet {}

    #The content of the selected rows are added to rowsres
    foreach cell $cells {
      set row [lindex [split $cell ,] 0]
      if { [lsearch $rows $row] == -1 } {
	lappend rows $row
	set sAny {} 
	for { set i 0 } { $i <= [$self cget -cols] } { incr i } {
	  set type [lindex $data(types) $i]
	  if { $type == "text" } {
	    lappend sAny "\"[string trim [$self get $row,$i] \"]\""
	  } elseif { $type == "date" } {
	    set saveformat [lindex $data(formats) $i]
	    $self setType $i date y%Ym%md%dh%Hi%Ms%S.000
	    lappend sAny [$self get $row,$i]
	    $self setType $i date $saveformat
	  } elseif { $type == "real" } {
	    lappend sAny [$self get $row,$i]
	  }
	}
        lappend sSet "SetOfAnything([join $sAny ,])"
      }
    }
    set code "SetOfSet([join $sSet ,])"

    return $code
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method _CreateSetForFunction { } {
  #
  # PURPOSE: Returns the syntax to create TOL object.
  # PARAMETERS:
  #
  #///////////////////////////////////////////////////////////////////////////
    #Gets current selection
    set cells [$self curselection]
    #Inicialize lists
    set rows {} 
    set sSet {}

    #The content of the selected rows are added to rowsres
    foreach cell $cells {
      set row [lindex [split $cell ,] 0]
      if { [lsearch $rows $row] == -1 } {
        lappend rows $row
        set sAny {} 
        for { set i 0 } { $i <= [$self cget -cols] } { incr i } {
          set type [lindex $data(types) $i]
          if { $type == "text" } {
            lappend sAny "\"[string trim [$self get $row,$i] \"]\""
          } elseif { $type == "date" } {
            set saveformat [lindex $data(formats) $i]
            $self setType $i date y%Ym%md%dh%Hi%Ms%S.000
            lappend sAny [$self get $row,$i]
            $self setType $i date $saveformat
          } elseif { $type == "real" } {
            lappend sAny [$self get $row,$i]
          }
        }
        lappend sSet "SetOfAnything([join $sAny ,])"
      }
    }
    
    set sSetReturn "[join $sSet ,]"
    
    #return $sSet
    return $sSetReturn
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method _CmdOnB3Func { m w x y X Y} {
  #
  # PURPOSE: Shows contextual function menu when right mouse button is clicked
  #
  # PARAMETERS: m -> The menu
  #             w -> The parent
  #             x -> The absolute position
  #             y -> The absolute postion
  #             X -> The relative position
  #             Y -> The relative position
  #
  #///////////////////////////////////////////////////////////////////////////
     if ![llength [$self border mark $x $y]] {
       tk_popup $m $X $Y
     }
  }

  #///////////////////////////////////////////////////////////////////////////
  method _CmdFirst { } {
  #
  # PURPOSE: Shows the beginning of the table
  #
  #///////////////////////////////////////////////////////////////////////////
    $self see origin
  }

  #///////////////////////////////////////////////////////////////////////////
  method _CmdBack { } {
  #
  # PURPOSE: Moves active cell one cell up
  #
  #///////////////////////////////////////////////////////////////////////////
    ::tk::table::MoveCell $self -1  0
  }

  #///////////////////////////////////////////////////////////////////////////
  method _CmdNext { } {
  #
  # PURPOSE: Moves active cell one cell down
  #
  #///////////////////////////////////////////////////////////////////////////
    if { ![catch {$self index active}] } {
      if { [$self index active row] < [expr {[$self cget -rows] -1}] } {
        ::tk::table::MoveCell $self 1  0
      } else {
        $self insert rows end
        ::tk::table::MoveCell $self 1  0
        $self _UpdateInfoRows
        $self _IsSaved 0
      }
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method _CmdLast { } {
  #
  # PURPOSE: Shows the end of the table
  #
  #///////////////////////////////////////////////////////////////////////////
    $self see end
  }

  #///////////////////////////////////////////////////////////////////////////
  method _CmdSelectAll { } {
  #
  # PURPOSE: Select all the table
  #
  #///////////////////////////////////////////////////////////////////////////
    ::tk::table::SelectAll $self
  }

  #///////////////////////////////////////////////////////////////////////////
  method _CmdDelete { } {
  #
  # PURPOSE: Deletes current selection
  #
  #///////////////////////////////////////////////////////////////////////////
    if {[llength [$self curselection]] > 1 } {
      $self curselection {}
      $self selection clear all
    }
    $self _IsSaved 0
  }
  
  
  #///////////////////////////////////////////////////////////////////////////
  method _CmdCopy { } {
  #
  # PURPOSE: Copy to clipboard current selection
  #
  #///////////////////////////////////////////////////////////////////////////
    # before delete clipboard 
    clipboard clear
    tk_tableCopy $widget(tab)
  }

  #///////////////////////////////////////////////////////////////////////////
  method _CmdSpecialCopy { arg } {
  #
  # PURPOSE: Copies headers or whole table depending on parameter.
  # PARAMETERS: arg -> headers -> copy headers
  #                    table -> copy table
  #
  #///////////////////////////////////////////////////////////////////////////
    #The current selection is saved
    set sel [$self curselection]
    set selfirst [lindex $sel 0]
    set sellast [lindex $sel end]
    #Current state of selecttitles is saved
    set temptit [$self cget -selecttitles]
    #Selecttitles is temporally set to 1
    $self configure -selecttitles 1
    #The headers are selected
    $self selection clear all
    if { $arg == "headers" } {  
      $self selection set 0,0 0,[expr [$self cget -cols] -1]
    } else {
      $self selection set 0,0 end
    }
    # before delete clipboard
    clipboard clear
    #copy
    tk_tableCopy $widget(tab)
    #the selection and selecttitles state are restored
    $self selection clear all
    $self configure -selecttitles $temptit
    $self selection set $selfirst $sellast
  }

  #///////////////////////////////////////////////////////////////////////////
  method _CmdCut { } {
  #
  # PURPOSE: Cuts to clipboard current selection
  #
  #///////////////////////////////////////////////////////////////////////////
    # before delete clipboard 
    clipboard clear
    tk_tableCut $widget(tab)
    $self _IsSaved 0
  }
  
  #/////////////////////////////////////////////////////////////////////////////
  method _CmdPaste { } {
  #
  # PURPOSE: Paste information saved in clipboard
  #
  #/////////////////////////////////////////////////////////////////////////////
    set txt [clipboard get]
    set p   [$self index active]
    if {![string length $p]} {
      set p 0,0
    }
    
    #set rowIni [lindex [split $p ","] 0]
    #set colIni [lindex [split $p ","] 1]
    #set rsep [$widget(tab) cget -rowseparator]
    #set csep [$widget(tab) cget -colseparator]
    #
    #if {[string comp {} $rsep]} { set drows [split $txt $rsep] }
    #set numRows [llength $drows]
    #if [string equal [lindex $drows end] {}] {
    #  set numRows [incr numRows -1]
    #}
    #set numCols 0  
    #foreach line $drows {
    #  if [expr [llength [split $line $csep]] > $numCols] {
    #    set numCols [llength [split $line $csep]]
    #  }
    #}
    #set colsold [$widget(tab) cget -cols]
    #for {set i $colsold} {$i < [expr $colIni+$numCols]} {incr i} {
    #  set data(types) [linsert $data(types) $i text] 
    #}
    
    $self _PasteHandler $p $txt
  }
    
  #/////////////////////////////////////////////////////////////////////////////
  method _CmdNew { } {
  #
  # PURPOSE: Cleans table editor to edit a new white table
  #
  #/////////////////////////////////////////////////////////////////////////////
    if [string equal [$self _CheckSaved] "cancel"] return
    $self configure -path ""
  }

  
  #/////////////////////////////////////////////////////////////////////////////
  method cmdOpen { } {
  #
  # PURPOSE: Opens selected file
  #
  #/////////////////////////////////////////////////////////////////////////////
    if [string equal [$self _CheckSaved] "cancel"] return
    
    set path [tk_getOpenFile -title [mc "Open"] \
                       -parent $self \
                       -initialdir [$self cget -defopenpath] \
                       -defaultextension [$self cget -defext] \
                       -filetypes [$self cget -typelist]]
    if {$path != ""} {
      set pathfolder [file dirname $path]
      $self configure -defopenpath $pathfolder
      $self configure -path $path
    }
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _CmdSave { } {
  #
  # PURPOSE: Saves the current table to a file.
  # RETURN: Returns 1 if file has been saved correctly or 0 if not.
  #
  #/////////////////////////////////////////////////////////////////////////////
    set nameFile [$self cget -path]

    if {$nameFile == ""} {
      set ok [$self _CmdSaveAs]
    } else  {
      set ok [$self _FileSave $nameFile]
    }
    return $ok
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _CmdClose { } {
  #
  # PURPOSE: Closes the table
  # RETURN: Returns 1 if the table has been closed 0 if not.
  #
  #/////////////////////////////////////////////////////////////////////////////
    if { [string equal [$self _CheckSaved] "cancel"] } {  
      return 0
    }
    #Ini file is written
    $self _WriteIni
    #if closecmd is defined, is executed
    set cmd [$self cget -closecmd]
    if {[string length $cmd]} {
       eval $cmd
    }

    destroy [$self cget -parent]
    return 1
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _CmdSaveAs {{nf ""}} {
  #
  # PURPOSE: Saves table data actually opened in table editor asking for the
  #          user to select a file to save it.
  #
  # PARAMETERS:
  #   nf   -> name of file proposed
  #
  # RETURN: Returns 1 if file has been saved correctly or 0 if not.
  #
  #/////////////////////////////////////////////////////////////////////////////
    if {$nf == ""} {
      set dir [$self cget -defsavepath]
      set fil ""     
    } else  {
      set dir "[file dirname $nf]"
      set fil "[file tail $nf]"    
    }
    set path [tk_getSaveFile -title [mc "Save as"] -initialdir $dir \
         -initialfile $fil -parent $self \
         -defaultextension [$self cget -defext] \
         -filetypes [$self cget -typelist]]
    if {![string equal $path ""] } {
      set pathfolder [file dirname $path]
      $self configure -defsavepath $pathfolder
      set ok [$self _FileSave $path]
      set options(-path) $path
    } else {
      set ok 0
    }
    return $ok
  }  
  
  
  #//////////////////////////////////////////////////////////////////////////  
  method _PostOrderCommand { } {
  #
  # PURPOSE: Execute postordercommand
  #
  #///////////////////////////////////////////////////////////////////////////
    set cmd [$self cget -postordercmd]
    if {[string length $cmd]} {
      eval $cmd
    }  
  }
  
  #/////////////////////////////////////////////////////////////////////////////
  method _CmdOrdAsc { } {
  #
  # PURPOSE: Orders the selected cell increasingly using the left most column as
  #          index
  #
  #/////////////////////////////////////////////////////////////////////////////
    if { [llength [$self curselection]] } {
      set start [lindex [$self curselection] 0]    
      set end [lindex [$self curselection] end]
      set col [lindex [split $start ,] 1]
      set args "-increasing"
      $self _Sort $start $end $col $args
      $self _IsSaved 0
      
      $self _PostOrderCommand
    }
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _CmdOrdDes { } {
  #
  # PURPOSE: Orders the selected cell decreasingly using the left most column as
  #          index
  #
  #/////////////////////////////////////////////////////////////////////////////
    if { [llength [$self curselection]] } {
      set start [lindex [$self curselection] 0]    
      set end [lindex [$self curselection] end]
      set col [lindex [split $start ,] 1]
      set args "-decreasing"
      $self _Sort $start $end $col $args
      $self _IsSaved 0
      
      $self _PostOrderCommand
    }
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _CmdCloseOrdAdv { w { onlyexit 0 } } {
  #
  # PURPOSE: Closes advanced order dialog
  #
  # PARAMETERS:
  #   w -> the dialog
  #   onlyexit -> flag that specifies if the table should be ordered before
  #               closing or not.
  #
  #/////////////////////////////////////////////////////////////////////////////
    if { $onlyexit } { 
      $w enddialog ""
    } else {
      set f [$w getframe]
      set data(advorder) [$f.bms get selected]
      if { $data(advorder) != "" } {
        set trows [expr [$self cget -rows]-1]
        set tcols [expr [$self cget -cols]-1]
        $self _Sort 1,0 $trows,$tcols 0 advorder
        $self _IsSaved 0
      }
      $self _PostOrderCommand
      
      $w enddialog $data(advorder)
    }
  }
  
  #/////////////////////////////////////////////////////////////////////////////
  method _CmdOrdAdv { } {
  #
  # PURPOSE: Creates advanced order dialog
  #
  #/////////////////////////////////////////////////////////////////////////////
    if {![winfo exists $self.d]} {
      set dialog [Dialog $self.d -title [mc "Advanced selector of order"] \
                  -modal local -parent $self]
      set dialogframe [$dialog getframe]
      wm protocol $dialog WM_DELETE_WINDOW [list $self _CmdCloseOrdAdv $dialog]
      
      #The name of the column is set with the header, if there is no header, a
      #default column name will be set.
      #For each column to lines are added, one to order incrementally and the
      #other decrementally.
      set select ""
      for {set i 0} {$i < [$self cget -cols]} {incr i} {
        if {[$self get 0,$i] == ""} {
          lappend select "$i {[mc column] $i} inc"
          lappend select "$i {[mc column] $i} dec"
        } else {
          lappend select "$i {[mc [$self get 0,$i]]} inc"
          lappend select "$i {[mc [$self get 0,$i]]} dec"
        }
      }
      
      bmultiselect $dialogframe.bms \
            -cols "[mc "index"] [mc "name"] [mc "order"]" \
            -hiddencols [mc "index"] -unsel $select \
            -labelunsel [mc "Not selected"] -labelsel [mc "Selected"]
    
      set dialogframe_bbox \
          [ButtonBox $dialogframe.bb -orient horizontal -spacing 10 -padx 0 -pady 0 \
          -homogeneous false]
      $dialogframe_bbox add -text [mc "Accept"] -relief link \
          -helptext [mc "Accept selection"] \
          -image [::Bitmap::get accept] -compound left -padx 5 -pady 1 \
          -command  [list $self _CmdCloseOrdAdv $dialog] 
      $dialogframe_bbox add -text [mc "Cancel"] -relief link \
          -helptext [mc "Cancel selection"] \
          -image [::Bitmap::get cancelx] -compound left -padx 5 -pady 1 \
          -command [list $self _CmdCloseOrdAdv $dialog 1]
 
      pack $dialogframe_bbox   -side bottom -expand no  -fill x 
      pack $dialogframe.bms    -fill both -expand true
    }
    #wm resizable $dialog 0 0
    $self.d draw
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _CmdSetType { typ } {
  #
  # PURPOSE: Sets the type of the last right clicked column
  #
  # PARAMETERS:
  #   typ -> the type
  #
  #/////////////////////////////////////////////////////////////////////////////
    $self setType $data(lastcol) $typ
  }

  #/////////////////////////////////////////////////////////////////////////////
  method setType { col typ {format {}}} {
  #
  # PURPOSE: Sets the type of the column.Empty format is also set.
  #
  # PARAMETERS:
  #   col -> the column
  #   typ -> the type
  #
  #/////////////////////////////////////////////////////////////////////////////
    set data(types) [lreplace $data(types) $col $col $typ]
    set data(formats) [lreplace $data(formats) $col $col $format]
    $self configure -padx [$self cget -padx]
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _CmdSetFormat {  } {
  #
  # PURPOSE: Sets the format of the last right clicked column
  #
  #/////////////////////////////////////////////////////////////////////////////
    set type [lindex $data(types) $data(lastcol)]
    if { $type == "date" } {
      $self _CmdSetFormatDate
    } else {
      $self _CmdSetFormatReal
    }
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _CmdSetFormatDate {  } {
  #
  # PURPOSE: Creates format dialog for date type
  #
  #/////////////////////////////////////////////////////////////////////////////
    set dialog [Dialog $self.fd -title [mc "Date format"]\
            -default 0 -modal none -parent $self]
    set w [$dialog getframe]
    wm resizable $dialog 0 0
    #Some default formats are included
    set data(dateformat,formats) [list \
     [mc "TOL Long Format"]   "y%Ym%md%dh%Hi%Ms%S.000" \
     [mc "TOL Short Format"]  "y%Ym%md%d" \
     "Completo YYYY/MM/dd"    "%Y/%m/%d" \
     "Completo dd/MM/YYYY"    "%d/%m/%Y" \
     "Fecha larga"            "%d de %B de %Y" \
     "Completo dd/Month/YYYY" "%d/%h/%Y" \
     [mc "User Defined"]]
    foreach {cad for} $data(dateformat,formats) {
      lappend lstCad $cad
    }
    
    label $w.lFormat -text [mc Format]:
    set widget(dateformat,date) [ComboBox $w.cbFormat \
                    -width 30 -height 6 -editable false -values $lstCad \
                    -textvariable [varname data(dateformat,date)] \
                     -modifycmd [list $self _CheckFormatDate]]
    label $w.lString -text [mc String]:
    set widget(dateformat,datestring) [Entry $w.eString \
             -width 45 -state disabled\
             -textvariable [varname data(dateformat,datestring)]]
    
    bind $w.eString <KeyRelease> [list $self _CheckDateString]
    label $w.lView   -text [mc Sample]:
    set widget(dateformat,dateview) [Entry $w.eView   -width 45 -state disabled\
             -textvariable [varname data(dateformat,dateview)]]
    Button $w.bHelp  -text [mc "Format Specifiers"] \
             -command [list $self _ShowFormatSpecifiers $w "date"] \
             -helptext [mc "Format Specifiers"]
    Button $w.ok -text [mc "Ok"] -width 16 \
            -command [list $self _DateFormatApply $dialog] \
            -helptext [mc "Ok"]
    Button $w.cancel -text [mc "Cancel"] -width 16 \
            -command [list destroy $dialog] \
            -helptext [mc "Cancel"]

    grid  $w.lFormat $w.cbFormat -sticky nw -padx 2 -pady 2
    grid  $w.lString $w.eString  -sticky nw -padx 2 -pady 2
    grid  $w.lView   $w.eView    -sticky nw -padx 2 -pady 2
    grid  $w.bHelp   -           -sticky nw -pady 2 -pady 2
    grid  $w.ok      $w.cancel   -sticky nw -pady 2 -pady 2

    wm geometry $dialog 360x90
    $dialog draw
    update

  }

  #/////////////////////////////////////////////////////////////////////////////
  method _CheckFormatDate {  } {
  #
  # PURPOSE: Enables entry to configure the format when User Defined is
  #          selected in other case shows system date with selected format
  #
  #/////////////////////////////////////////////////////////////////////////////
    if [string equal $data(dateformat,date) [mc "User Defined"]] {
      $widget(dateformat,datestring) configure -state normal
    } else  {
      $widget(dateformat,datestring) configure -state disabled
      set idx [lsearch -exact $data(dateformat,formats) $data(dateformat,date)]
      set data(dateformat,datestring) [lindex $data(dateformat,formats) [expr $idx+1]]
      #Shows system date with selected format
      set data(dateformat,dateview) \
            [clock format [clock scan now] -format $data(dateformat,datestring)]
    }
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _CheckDateString {  } {
  #
  # PURPOSE: Shows system date with typed format
  #
  #/////////////////////////////////////////////////////////////////////////////
    if {[set idx [lsearch -exact $data(dateformat,formats) \
                                 $data(dateformat,datestring)]] != -1} {
      set data(dateformat,date) [lindex $data(dateformat,formats) [expr $idx-1]]
    } else  {
      set data(dateformat,date) [lindex $data(dateformat,formats) end]
      $widget(dateformat,datestring) configure -state normal
    }
    set data(dateformat,dateview) \
          [clock format [clock scan now] -format $data(dateformat,datestring)]
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _DateFormatApply { dialog } {
  #
  # PURPOSE: Applies format to the column
  #
  #/////////////////////////////////////////////////////////////////////////////
    set data(formats) [lreplace $data(formats) $data(lastcol) $data(lastcol) \
                      $data(dateformat,datestring)]
    destroy $dialog
    $self configure -padx [$self cget -padx]
  }
  
  #/////////////////////////////////////////////////////////////////////////////
  method _CmdSetFormatReal {  } {
  #
  # PURPOSE: Creates format dialog for real type
  #
  #/////////////////////////////////////////////////////////////////////////////
    set dialog [Dialog $self.fre -title [mc "Real format"]\
            -default 0 -modal none -parent $self]
    set w [$dialog getframe]
    wm resizable $dialog 0 0

    set data(realformat,real) 12345.67890

    
    label $w.lReal -text [mc "Number"]:
    set widget(realformat,real) [Entry $w.eReal  -width 20 \
             -textvariable [varname data(realformat,real)]]

    label $w.lFormat -text [mc "Format"]:
    set widget(realformat,realstring) [Entry $w.eFormat -width 20 \
             -textvariable [varname data(realformat,realformat)]]
    
    bind $w.eReal <KeyRelease> [list $self _CheckReal]
    bind $w.eFormat <KeyRelease> [list $self _CheckReal]

    label $w.lView   -text [mc Sample]:
    set widget(realformat,realview) [Entry $w.eView   -width 20 \
             -state disabled \
             -textvariable [varname data(realformat,realview)]]
    
    Button $w.bHelp  -text [mc "Format Specifiers"] \
             -command [list $self _ShowFormatSpecifiers $w "real"] \
             -helptext [mc "Format Specifiers"]
    Button $w.ok -text [mc "Ok"] -width 16 \
            -command [list $self _RealFormatApply $dialog] \
            -helptext [mc "Ok"]
    Button $w.cancel -text [mc "Cancel"] -width 16 \
            -command [list destroy $dialog] \
            -helptext [mc "Cancel"]

    grid  $w.lReal   $w.eReal    -sticky nw -padx 2 -pady 2
    grid  $w.lFormat $w.eFormat  -sticky nw -padx 2 -pady 2
    grid  $w.lView   $w.eView    -sticky nw -padx 2 -pady 2
    grid  $w.bHelp   -           -sticky nw -pady 2 -pady 2
    grid  $w.ok      $w.cancel   -sticky nw -pady 2 -pady 2

    wm geometry $dialog 360x90
    $dialog draw
    update
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _CheckReal {  } {
  #
  # PURPOSE: Shows the result of appliying the format to the number
  #
  #/////////////////////////////////////////////////////////////////////////////
    if { ![catch {set data(realformat,realview) \
            [format $data(realformat,realformat) $data(realformat,real)]} msgerr] } {
    } else {
      set data(realformat,realview) [mc "Format not valid"]
    }
  }
  
  #/////////////////////////////////////////////////////////////////////////////
  method _RealFormatApply { dialog } {
  #
  # PURPOSE: Applies format to the column
  #
  #/////////////////////////////////////////////////////////////////////////////
    set data(formats) [lreplace $data(formats) $data(lastcol) $data(lastcol) \
                      $data(realformat,realformat)]
    destroy $dialog
    $self configure -padx [$self cget -padx]
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _ShowFormatSpecifiers { w typ } {
  #
  # PURPOSE: Shows help for the format specifiers
  #
  #/////////////////////////////////////////////////////////////////////////////
    if [winfo exist $w.t] {
      raise $w.t
      return
    }
    set dialog [Dialog $w.t -title [mc "Format Specifiers"]\
                -default 0 -parent $w -modal none -separator true]
    
    set w [$dialog getframe]
    
    set txt [text $w.txt -state normal -yscrollcommand "$w.scroll set"]
    set scroll [scrollbar $w.scroll -command "$txt yview"]
         
    $txt tag configure label -font {Arial 9} -foreground red
    $txt tag configure desc -font {Arial 9} -foreground black -lmargin1 "20" \
                                                            -lmargin2 "20"

    if { $typ == "real" } {
      set chars [list \
        % c d e f g i o s u x]
    } else {
      set chars [list \
        % a A b B c C d D e h H I j K l m M n p r R s S t T u U V w W x X y Y Z]
    }
    
    foreach char $chars {
      $txt insert end %$char {label}
      $txt insert end \n
      $txt insert end "[mc desc$typ%$char]" {desc}
      $txt insert end \n
    }
    
    $txt configure -state disabled

    pack $scroll -side right -fill y 
    pack $txt -side bottom -expand yes -fill both

    $dialog draw
  }
  
  
  #/////////////////////////////////////////////////////////////////////////////
  method _CmdInsertCols { } {
  #
  # PURPOSE: Creates dialog to insert columns
  #
  #/////////////////////////////////////////////////////////////////////////////
    #default data
    set data(insertcols,side) left
    set data(insertcols,type) text
    
    
    set dialog [Dialog $self.ic -title [mc "Insert Columns"]\
            -default 0 -modal none -parent $self]
    set w [$dialog getframe]
    wm resizable $dialog 0 0
    
    wm protocol $dialog WM_DELETE_WINDOW [list $self _InsertColsClose $dialog]
    
    label $w.lNum -text [mc "Columns"]
    SpinBox $w.sNum -range {1 99} -width 2 -justify right -editable true \
          -textvariable [varname data(insertcols,num)]
    TitleFrame $w.side -bd 2 -relief groove -text [mc "Side"]
    set f [$w.side getframe]
    radiobutton $f.left -text [mc "Left"] -value left \
            -variable [varname data(insertcols,side)] \
            -highlightthickness 0 -pady 0
    radiobutton $f.right -text [mc "Right"] -value right \
            -variable [varname data(insertcols,side)] \
            -highlightthickness 0 -pady 0
    TitleFrame $w.type -bd 2 -relief groove -text [mc "Type"]
    set f1 [$w.type getframe]
    radiobutton $f1.text -text [mc "Text"] -value text \
            -variable [varname data(insertcols,type)] \
            -highlightthickness 0 -pady 0
    radiobutton $f1.real -text [mc "Real"] -value real \
            -variable [varname data(insertcols,type)] \
            -highlightthickness 0 -pady 0
    radiobutton $f1.date -text [mc "Date"] -value date \
            -variable [varname data(insertcols,type)] \
            -highlightthickness 0 -pady 0
    Button $w.ok -text [mc "Ok"] -width 8 \
            -command [list $self _InsertColsOk $dialog] \
            -helptext [mc "Insert Columns"]
    Button $w.cancelar -text [mc "Cancel"] -width 8 \
            -helptext "" -command [list $self _InsertColsClose $dialog]
    
    bind $dialog <Escape> [list $self _InsertColsClose $dialog]
  
    grid $f.left
    grid $f.right    
    
    grid $f1.text
    grid $f1.real    
    grid $f1.date

    grid $w.lNum  $w.sNum     -sticky nw -padx 5
    grid $w.type  $w.side     -sticky nw -padx 5
    grid $w.ok  $w.cancelar   -sticky nw -padx 5

    wm geometry $dialog 360x90
    $dialog draw
    update
  }
  
  #/////////////////////////////////////////////////////////////////////////////
  method _InsertColsOk { dialog } {
  #
  # PURPOSE: Inserts column with the options set in the dialog and after
  #          destroys the dialog
  # PARAMETERS:
  #   dialog -> the dialog
  #
  #/////////////////////////////////////////////////////////////////////////////
    $self _InsertCols $data(insertcols,side) \
                      $data(lastcol) \
                      $data(insertcols,num) \
                      $data(insertcols,type)
    destroy $dialog
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _InsertColsClose { dialog } {
  #
  # PURPOSE: Destroys the dialog
  # PARAMETERS:
  #   dialog -> the dialog
  #
  #/////////////////////////////////////////////////////////////////////////////
    destroy $dialog
  }
  

  #/////////////////////////////////////////////////////////////////////////////
  method _CmdInsertRows { } {
  #
  # PURPOSE: Creates dialog to insert rows
  #
  #/////////////////////////////////////////////////////////////////////////////
    #default data
    set data(insertrows,side) up
    
    set dialog [Dialog $self.ir -title [mc "Insert Rows"]\
            -default 0 -modal none -parent $self]
    set w [$dialog getframe]
    wm resizable $dialog 0 0
    
    wm protocol $dialog WM_DELETE_WINDOW [list $self _InsertRowsClose $dialog]
    
    label $w.lNum -text [mc "Rows"]
    SpinBox $w.sNum -range {1 99} -width 2 -justify right -editable true \
          -textvariable [varname data(insertrows,num)]
    TitleFrame $w.side -bd 2 -relief groove -text [mc "Side"]
    set f [$w.side getframe]
    radiobutton $f.up -text [mc "Up"] -value up \
            -variable [varname data(insertrows,side)] \
            -highlightthickness 0 -pady 0
    radiobutton $f.down -text [mc "Down"] -value down \
            -variable [varname data(insertrows,side)] \
            -highlightthickness 0 -pady 0

    Button $w.ok -text [mc "Ok"] -width 8 \
            -command [list $self _InsertRowsOk $dialog] \
            -helptext [mc "Insert Rows"]
    Button $w.cancelar -text [mc "Cancel"] -width 8 \
            -helptext "" -command [list $self _InsertRowsClose $dialog]
    
    bind $dialog <Escape> [list $self _InsertRowsClose $dialog]
  
    grid $f.up $f.down
    
    grid $w.lNum  $w.sNum     -sticky nw -padx 5
    grid $w.side  -           -sticky nw -padx 5
    grid $w.ok  $w.cancelar   -sticky nw -padx 5

    wm geometry $dialog 360x90
    $dialog draw
    update
  }
  
  #/////////////////////////////////////////////////////////////////////////////
  method _InsertRowsOk { dialog } {
  #
  # PURPOSE: Inserts column with the options set in the dialog and after
  #          destroys the dialog
  # PARAMETERS:
  #   dialog -> the dialog
  #
  #/////////////////////////////////////////////////////////////////////////////
    $self _InsertRows $data(insertrows,side) \
                      $data(lastrow) \
                      $data(insertrows,num)
    destroy $dialog
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _InsertRowsClose { dialog } {
  #
  # PURPOSE: Destroys the dialog
  # PARAMETERS:
  #   dialog -> the dialog
  #
  #/////////////////////////////////////////////////////////////////////////////
    destroy $dialog
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _CmdDeleteCols { } {
  #
  # PURPOSE: Deletes selected columns
  #
  #/////////////////////////////////////////////////////////////////////////////
    set cells [$self curselection]
    if ![llength $cells] {
      set cells {0,0}
    }
    set colIni [lindex [split [lindex $cells 0] ","] 1]
    set colEnd [lindex [split [lindex $cells end] ","] 1]
    set cols [expr $colEnd-$colIni+1]

    $self _DeleteCols $colIni $cols
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _CmdDeleteRows { } {
  #
  # PURPOSE: Deletes selected rows
  #
  #/////////////////////////////////////////////////////////////////////////////
    set cells [$self curselection]
    if ![llength $cells] {
      set cells {0,0}
    }
    
    set rowIni [lindex [split [lindex $cells 0] ","] 0]
    set rowEnd [lindex [split [lindex $cells end] ","] 0]
    set rows [expr $rowEnd-$rowIni+1]
    
    $self _DeleteRows $rowIni $rows
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _CmdFind { } {
  #
  # PURPOSE: Set default parameters for the Find dialog
  #
  #/////////////////////////////////////////////////////////////////////////////
    set data(find,text) ""
    set data(find,next) 0
    $self _FindCreate
  }
  
  #/////////////////////////////////////////////////////////////////////////////
  method _FindCreate { } {
  #
  # PURPOSE: Creates a Find Dialog
  #
  #/////////////////////////////////////////////////////////////////////////////
    set dialog [Dialog $self.df -title [mc "Find"]\
            -default 0 -modal none -parent $self]
    set w [$dialog getframe]
    wm resizable $dialog 0 0
    
    wm protocol $dialog WM_DELETE_WINDOW [list $self _FindClose $dialog]
    
    set data(find,cad) [LabelEntry  $w.eTxt -label "[mc "Find"]: " \
            -textvariable [varname data(find,text)] -labelwidth 12]
    checkbutton $w.full -text [mc "Full word"] \
            -variable [varname data(find,full)] -pady 0
    checkbutton $w.case  -text [mc "Case sensitive"] \
            -variable [varname data(find,case)] -pady 0
    TitleFrame $w.dir -bd 2 -relief groove -text [mc "Direction"]
    set f [$w.dir getframe]
    radiobutton $f.forward -text [mc "Forward"] -value forward \
            -variable [varname data(find,dir)] \
            -highlightthickness 0 -pady 0
    radiobutton $f.backward -text [mc "Backward"] -value backward \
            -variable [varname data(find,dir)] \
            -highlightthickness 0 -pady 0
    Button $w.findNext -text [mc "Find next"] -width 16 \
            -command [list $self _FindNext 1] \
            -helptext [mc "Find next"]
    Button $w.mark -text [mc "Mark all"] -width 16 \
            -command [list $self _MarkAll] \
            -helptext [mc "Mark "]
    Button $w.cancelar -text [mc "Cancel"] -width 16 \
            -helptext "" -command [list $self _FindClose $dialog]
    
    bind $dialog <Return> [list $self _FindNext 1]
    bind $dialog <F3>     [list $self _FindNext 1]
    bind $dialog <Escape> [list $self _FindClose $dialog]
  
    grid $f.forward  $f.backward -sticky nw
    
    grid $w.eTxt  -           -sticky news -padx 5 -pady 5
    grid $w.full  $w.findNext -sticky nw   -padx 5
    grid $w.case  $w.mark     -sticky nw   -padx 5
    grid $w.dir $w.cancelar   -sticky nw   -padx 5
    wm geometry $dialog 360x90
    $dialog draw
    update
    focus $data(find,cad)
  }
  
  #/////////////////////////////////////////////////////////////////////////////
  method _FindClose { dialog } {
  #
  # PURPOSE: Closes find dialog and deletes possibly marked cells
  # PARAMETERS:
  #   dialog -> Find dialog
  #
  #/////////////////////////////////////////////////////////////////////////////
    set foundcells [$self tag celltag found]
    foreach cell $foundcells {
      $self tag celltag "" $cell
    }
    destroy $dialog
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _FindNext { ask {key find} } {
  #
  # PURPOSE: Finds next cell that matches the searched text.
  #
  # PARAMETERS:
  #   ask -> asks to continue from the beginning when the end of the table is
  #          reached
  #   key -> type of search. Default value find. Domain [find | replace]
  #
  #/////////////////////////////////////////////////////////////////////////////  
    set pos     [$self _FindText $key]

    if { $pos != "" } {
      set data($key,next) 1
      set p "[lindex $pos 0],[lindex $pos 1]"
      $self selection clear all
      $self see $p
      if { [$self cget -state] == "normal" } {
        $self activate $p
      } else {
        $self activate $p
        $self selection set $p
      }
    } elseif { $ask == 0} {
      set data($key,next) 0
    } else {
      set data($key,next) 0
      if [string equal $data($key,dir) "forward"] {
        set msg "[mc SearchUpEndDoc].\n[mc FindFromBegin]"
      } else {
        set msg "[mc SearchUpBeginDoc].\n[mc FindFromEnd]"
      }
      set rq [tk_messageBox -type okcancel -icon info -parent $self\
              -title [mc "Table"] -message $msg]
      if {$rq == "ok"} {
        if [string equal $data($key,dir) "forward"] {
          set ro [$self cget -roworigin]
          set co [$self cget -colorigin]
          $self activate "$ro,$co"
        } else {
          $self activate end
        }
        $self _FindNext $ask $key
      }
    }
    return $pos
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _MarkAll { } {
  #
  # PURPOSE: Marks all cells that match with the searched text
  #
  #/////////////////////////////////////////////////////////////////////////////
    # save value
    set tpmfinddir $data(find,dir)
    set data(find,dir) forward
    set ro [$self cget -roworigin]
    set co [$self cget -colorigin]
    $self activate "$ro,$co"
    
    set pos     [$self _FindText]
    
    while { $pos != "" } {
      set data(find,next) 1
      set p "[lindex $pos 0],[lindex $pos 1]"
      $self selection clear all
      $self see $p    
      $self activate $p

      $self tag celltag found $p

      set pos     [$self _FindText]
    }
    $self activate "$ro,$co"
    # retore value
    set data(find,dir) $tpmfinddir
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _FindText { {key find} } {
  #
  # PURPOSE: Find the text in the table
  # PARAMETERS:
  #   key -> type of search. Default value find. Domain [find | replace]  
  # RETURN:
  #
  #/////////////////////////////////////////////////////////////////////////////
    #values are initialized
    set ok 0
    set rc ""
    #number of columns and rows
    set r   [$self cget -rows]
    set c   [$self cget -cols]
    set ro  [$self cget -roworigin]
    set co  [$self cget -colorigin]
    #active cell
    if { ![catch {set pos [$self index active]} msgerr] } {
      if {$pos eq "" } { 
        set rp $ro
        set cp $co
      } else  {
        set pc [string first , $pos]
        set rp [string range $pos 0         [expr $pc - 1]]
        set cp [string range $pos [incr pc] end]
      } 
    } else  {
      set rp $ro
      set cp $co
    }
    #search direction is set
    if [string equal $data($key,dir) "forward"] {
      set dir 1
    } else {
      set dir -1
    }  
    #case sensitive
    if $data($key,case) {
      set nc ""    
    } else  {
      set nc -nocase    
    }
    set sw $data($key,next)

    upvar \#0 [varname matrix] matrix

    #search
    if {![string equal $data($key,text) ""]} {
      while {[expr ($ok == 0) && ($cp >= $co) && ($cp < $c)]} {
        while {[expr ($ok == 0) && ($rp >= $ro) && ($rp < $r)]} {
          if { [info exists matrix($rp,$cp)] } {
            set txt $matrix($rp,$cp)
            if { $data($key,full) == 0 } {
              set comp [eval string match $nc *[list $data($key,text)]* [list $txt]]
            } else {
              set comp [eval string match $nc [list $data($key,text)] [list $txt]]
            }
            if { $comp } {
              if {($sw eq "0") || ($pos ne "$rp,$cp")} {
                set ok 1  
                set rc "$rp $cp"
              }
              set sw 0
            }
          }
          incr rp $dir
        }
        if {[expr ($dir == 1)]} {
          set rp $ro
        } else  {
          set rp [expr $r - 1]
        }
        incr cp $dir
      }
    }
    return $rc
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _CmdReplace { } {
  #
  # PURPOSE: Set default parameters for the Replace dialog
  #
  #/////////////////////////////////////////////////////////////////////////////
    set data(replace,text)    ""
    set data(replace,newtext) ""
    set data(replace,next)    0
    $self _ReplaceCreate
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _ReplaceCreate { } {
  #
  # PURPOSE: Creates a Replace Dialog
  #
  #/////////////////////////////////////////////////////////////////////////////
    set dialog [Dialog $self.dr -title [mc "Replace"]\
            -default 0 -modal none -parent $self]
    set w [$dialog getframe]
    wm resizable $dialog 0 0
    
    set data(replace,find) [LabelEntry  $w.eTxtF \
                            -label "[mc "Find"]: " \
                            -labelwidth 12  \
                            -textvariable [varname data(replace,text)]]
    set data(replace,rep) [LabelEntry  $w.eTxtR  \
                           -label "[mc "Replace"]: " \
                           -labelwidth 12 \
                           -textvariable [varname data(replace,newtext)]]
    checkbutton $w.full -text [mc "Full word"] \
            -variable [varname data(replace,full)] -pady 0
    checkbutton $w.case  -text [mc "Case sensitive"] \
            -variable [varname data(replace,case)] -pady 0
    TitleFrame $w.dir -bd 2 -relief groove -text [mc "Direction"]
    set f [$w.dir getframe]
    radiobutton $f.forward -text [mc "Forward"] -value forward \
            -variable [varname data(replace,dir)] \
            -highlightthickness 0 -pady 0
    radiobutton $f.backward -text [mc "Backward"] -value backward \
            -variable [varname data(replace,dir)] \
            -highlightthickness 0 -pady 0
    Button $w.findNext -text [mc "Find next"] -width 16 \
            -command [list $self _FindNext 1 replace] \
            -helptext [mc "Find next"]
    Button $w.rep -text [mc "Replace"] -width 16 \
            -command [list $self _Replace] \
            -helptext [mc "Replace"]
    Button $w.repAll -text [mc "Replace all"] -width 16 \
            -command [list $self _ReplaceAll] \
            -helptext [mc "Mark "]
    Button $w.cancel -text [mc "Cancel"] -width 16 \
            -helptext "" -command "destroy $dialog"
    frame $w.fr
    
    bind $dialog <Return> [list $self _Replace]
    bind $dialog <F3>     [list $self _Replace]
    bind $dialog <Escape> [list destroy $dialog]
  
    # Hemos añadido un tag para la selección, hay que quitarle como el tag sel
    #bind $ <ButtonPress> +[list $data(tabla) tag delete found]
    grid $f.forward  $f.backward -sticky nw
    
    grid $w.eTxtF  -            -sticky news -padx 5
    grid $w.eTxtR  -            -sticky news -padx 5 -pady 5
    grid $w.full   $w.findNext  -sticky nw   -padx 5
    grid $w.case   $w.rep       -sticky nw   -padx 5
    grid $w.dir    $w.repAll    -sticky nw   -padx 5
    grid ^         $w.cancel    -sticky nw   -padx 5
    wm geometry $dialog 360x90
    $dialog draw
    update
    focus $data(replace,find)
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _Replace { { ask 1 } } {
  #
  # PURPOSE: Replaces text in the table
  #
  # PARAMETERS:
  #   ask -> asks to continue from the beginning when the end of the table is
  #          reached
  #
  #/////////////////////////////////////////////////////////////////////////////  
    upvar \#0 [varname matrix] matrix

    if { ![catch {set cell [$self index active]} msgerr] } {
      if $data(replace,case) {
        set nc ""    
      } else  {
        set nc -nocase    
      }
      if { [info exists matrix($cell)] && $matrix($cell) != ""} {
	if { $data(replace,newtext) != "" } {
	  eval regsub $nc $data(replace,text) $matrix($cell) \
	                  $data(replace,newtext) matrix($cell)
	} else {
          set newvalue "[regsub $nc $data(replace,text) $matrix($cell) {}]"
 	  set matrix($cell) $newvalue
        }
        $self _IsSaved 0
      }
    }
    return [$self _FindNext $ask replace]
  }
  
  #/////////////////////////////////////////////////////////////////////////////
  method _ReplaceAll { } {
  #
  # PURPOSE: Replaces all cells with the text in the table
  #
  #/////////////////////////////////////////////////////////////////////////////
    # save value
    set tmpreplacedir $data(replace,dir)
    set data(replace,dir) forward
    set ro [$self cget -roworigin]
    set co [$self cget -colorigin]
    $self activate "$ro,$co"
    
    set pos     [$self _Replace 0]
    
    while { $pos != "" } {
      set pos     [$self _Replace 0]
    }
    $self activate "$ro,$co"
    # restore value
    set data(replace,dir) $tmpreplacedir
  }

#   #/////////////////////////////////////////////////////////////////////////////
#   method _CmdFilter { } {
#   #
#   # PURPOSE: Filters data in the table
#   #
#   #/////////////////////////////////////////////////////////////////////////////  
#     set dialog [Dialog $self.f -title [mc "Filter"] -modal local \
#                 -parent $self -default 0 -cancel 1]

#     set f [$dialog getframe]
    
#     set values {}
#     for {set i 0} {$i < [$self cget -cols]} {incr i} {
#       set col [$self get 0,$i]
#       set type [string totitle [lindex $data(types) $i]]
#       if { $col != "" } {
# 	lappend values [list $i $type {} {} $col]
#       } else {
# 	lappend values [list $i $type {} {} [mc column]$i]
#       }
#     }
#     puts $values
#     set filter [bfilter $f.bf -values $values -language "TOL" \
# 		  -showbuttons 1  \
# 		  -okcommand [list puts ok] \
# 		  -cancelcommand [list puts cancel]]

#     puts $filter
#     pack $filter -expand true -fill both  
#     $dialog draw
#   }

  #/////////////////////////////////////////////////////////////////////////////
  method _OptionsGet { } {
  #
  # PURPOSE: Gets the current options for the options dialog
  #
  #/////////////////////////////////////////////////////////////////////////////  
    set colsep [$self cget -colseparator]
    set rowsep [$self cget -rowseparator]
    
    if { $colsep == "\t" } {
      set data(options,colseptype) TAB
    } elseif { $colsep == "\n" } {
      set data(options,colseptype) RET
    } else {
      set data(options,colseptype) OTHER
      set data(options,colseparator) $colsep
    }

    if { $rowsep == "\t" } {
      set data(options,rowseptype) TAB
    } elseif { $rowsep == "\n" } {
      set data(options,rowseptype) RET
    } else {
      set data(options,rowseptype) OTHER
      set data(options,rowseparator) $rowsep
    }
    
    set data(options,font) [$self cget -font]
    set data(options,bg) [$self cget -bg]
    set data(options,cbalt) [$self cget -bgalt]
    set data(options,bgalt1) [$self cget -bgalt1]
    set data(options,bgalt2) [$self cget -bgalt2]
    set data(options,whole) [$self cget -checkcolumn]
    set data(options,inter) [$self cget -internalvalues]
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _OptionsSet { } {
  #
  # PURPOSE: Sets the options selected in the options dialog
  #
  #/////////////////////////////////////////////////////////////////////////////  
    if { $data(options,rowseptype) == "TAB" } {
      $self configure -rowseparator \t
    } elseif { $data(options,rowseptype) == "RET" } {
      $self configure -rowseparator \n
    } else {
      $self configure -rowseparator $data(options,rowseparator)
    }

    if { $data(options,colseptype) == "TAB" } {
      $self configure -colseparator \t
    } elseif { $data(options,colseptype) == "RET" } {
      $self configure -colseparator \n
    } else {
      $self configure -colseparator $data(options,colseparator)
    }

    $self configure -font $data(options,font)
    $self configure -bg $data(options,bg)
    $self configure -bgalt $data(options,cbalt)
    $self configure -bgalt1 $data(options,bgalt1)
    $self configure -bgalt2 $data(options,bgalt2)
    $self configure -checkcolumn $data(options,whole)
    $self configure -internalvalues $data(options,inter)
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _CmdOptions { } {
  #
  # PURPOSE: Creates an options Dialog
  #
  #/////////////////////////////////////////////////////////////////////////////     
    set dialog [Dialog $self.t -title [mc "Table Options"] -modal local \
                -parent $self -default 0 -cancel 1]
  
    wm protocol $dialog WM_DELETE_WINDOW [list $self _OptionsDestroy $dialog]
    
    set w [$dialog getframe]
    wm resizable $dialog 0 0
    
    set w_tabset [NoteBook $w.nb -tabbevelsize 8 -tabpady {2 6} -font {Arial 8}]
    
    #insertar tabset  
    set num_i 0
    foreach lab_i [list Export Style Behaviour] {
      $w_tabset insert $num_i $lab_i -text "  [mc $lab_i]  " -background \#d9d9d9 
      set tab_i [$w_tabset getframe $lab_i]
      $tab_i configure -borderwidth 2 -background \#d9d9d9
      incr num_i      
      set f$num_i [frame $tab_i.f]
      pack $tab_i.f -fill both -expand yes  
    }
    $w_tabset raise [$w_tabset pages 0]

    $self _OptionsGet
    $self _OptionsCreateExport    $f1
    $self _OptionsCreateStyle     $f2
    $self _OptionsCreateBehaviour $f3   
    
    set w_bbox \
          [ButtonBox $w.bb -orient horizontal -spacing 10 -padx 0 -pady 0 \
          -homogeneous false]
    $w_bbox add -text [mc "Accept"] -relief link \
          -helptext [mc "Make the changes and leave the dialog"] \
          -image [::Bitmap::get accept] -compound left -padx 5 -pady 1 \
          -command [list $self _OptionsOk $dialog]
    $w_bbox add -text [mc "Cancel"] -relief link \
          -helptext [mc "Leave the dialog without apply any change"] \
          -image [::Bitmap::get cancelx] -compound left -padx 5 -pady 1 \
          -command [list $self _OptionsDestroy $dialog]
    $w_bbox add -text [mc "Apply"] -relief link \
          -helptext [mc "Make the changes without leave the dialog"] \
          -image [::Bitmap::get apply] -compound left -padx 5 -pady 1 \
          -command [list $self _OptionsSet]
  
    $w_tabset compute_size
    grid $w_tabset -sticky news
    grid $w_bbox   -sticky news
        
    grid rowconfigure    $w 0 -weight 1
    grid rowconfigure    $w 1 -weight 0
    grid columnconfigure $w 0 -weight 1
    
    $dialog draw
  }
  
  #/////////////////////////////////////////////////////////////////////////////
  method _OptionsCreateExport { w } {
  #
  # PURPOSE: Creates export options tab
  #
  # PARAMETERS:
  #   w -> the frame in which the options widgets will be created
  #
  #/////////////////////////////////////////////////////////////////////////////  
    TitleFrame $w.tf1 -text [mc "Columns separator"]
    set f1 [$w.tf1 getframe]
    set widget(options,rbColSep1) [radiobutton $f1.rbColSep1 -text [mc Tab] \
       -value TAB -highlightthickness 0 -pady 0 \
       -variable [varname data(options,colseptype)] \
       -command [list $self _OptionsCheckColSepType]]
    set widget(options,rbColSep2) [radiobutton $f1.rbColSep2 -text [mc Return] \
       -value RET -highlightthickness 0 -pady 0 \
       -variable [varname data(options,colseptype)] \
       -command [list $self _OptionsCheckColSepType]]
    set widget(options,rbColSepN) [radiobutton $f1.rbColSepN -text [mc Other]: \
       -value OTHER -highlightthickness 0 -pady 0 \
       -variable [varname data(options,colseptype)] \
       -command [list $self _OptionsCheckColSepType]]
    set widget(options,eColSep) [Entry $f1.eColSep \
       -width 5 -justify right -textvariable [varname data(options,colseparator)]]
    frame $f1.f -width 25

    grid $f1.f  $widget(options,rbColSep1)  $widget(options,rbColSep2) $widget(options,rbColSepN) $widget(options,eColSep) -sticky w  -padx 2 -pady 0
    grid rowconfigure    $f1 1 -weight 1
    grid columnconfigure $f1 4 -weight 1


    TitleFrame $w.tf2 -text [mc "Rows separator"]
    set f2 [$w.tf2 getframe]
    set widget(options,rbRowSep1) [radiobutton $f2.rbRowSep1 -text [mc Return] \
       -value RET -highlightthickness 0 -pady 0 \
       -variable [varname data(options,rowseptype)] \
       -command [list $self _OptionsCheckRowSepType]]
    set widget(options,rbRowSep2) [radiobutton $f2.rbRowSep2 -text [mc Tab] \
       -value TAB -highlightthickness 0 -pady 0 \
       -variable [varname data(options,rowseptype)] \
       -command [list $self _OptionsCheckRowSepType]]
    set widget(options,rbRowSepN) [radiobutton $f2.rbRowSepN -text [mc Other]: \
       -value OTHER -highlightthickness 0 -pady 0 \
       -variable [varname data(options,rowseptype)] \
       -command [list $self _OptionsCheckRowSepType]]
    set widget(options,eRowSep) [Entry $f2.eRowSep \
       -width 5 -justify right -textvariable [varname data(options,rowseparator)]]

    frame $f2.f -width 25

    grid $f2.f  $widget(options,rbRowSep1)  $widget(options,rbRowSep2) $widget(options,rbRowSepN) $widget(options,eRowSep) -sticky w  -padx 2 -pady 0
    grid rowconfigure    $f2 1 -weight 1
    grid columnconfigure $f2 4 -weight 1
  
    grid $w.tf1 -sticky news -padx 2
    grid $w.tf2 -sticky news -padx 2
    grid rowconfigure    $w 0 -weight 1
    grid rowconfigure    $w 1 -weight 3
    grid columnconfigure $w 0 -weight 1
  }
  
  #/////////////////////////////////////////////////////////////////////////////
  method _OptionsCreateStyle { w } {
  #
  # PURPOSE: Creates style options tab
  #
  # PARAMETERS:
  #   w -> the frame in which the options widgets will be created
  #
  #/////////////////////////////////////////////////////////////////////////////  
    TitleFrame $w.tf1 -text [mc "Style"]
    set f1 [$w.tf1 getframe]
    set widget(options,font) [bfont $f1.bFont \
      -variable [varname data(options,font)] -showlabel 1 -label [mc "Font"]:]

    set widget(options,bg) [bcolor $f1.bColor \
       -variable [varname data(options,bg)] -showlabel 1 -label [mc "Color"]:]
 
    set widget(options,cbalt) [checkbutton $f1.cbalt \
            -text [mc "Alternate row colors"] \
            -variable [varname data(options,cbalt)] -pady 0]

    set widget(options,bgalt1) [bcolor $f1.bColorAlt1 \
       -variable [varname data(options,bgalt1)] -showlabel 1 -label [mc "Color "]1:]

    set widget(options,bgalt2) [bcolor $f1.bColorAlt2 \
       -variable [varname data(options,bgalt2)] -showlabel 1 -label [mc "Color "]2:]

    grid $widget(options,font) $widget(options,bg) -padx 10
    grid  $widget(options,cbalt) $widget(options,bgalt1) $widget(options,bgalt2) -padx 10
  
    grid $w.tf1 -sticky news -padx 2
    grid rowconfigure    $w 0 -weight 1
    grid columnconfigure $w 0 -weight 1
  }
  
  #/////////////////////////////////////////////////////////////////////////////
  method _OptionsCreateBehaviour { w } {
  #
  # PURPOSE: Creates behaviour options tab
  #
  # PARAMETERS:
  #   w -> the frame in which the options widgets will be created
  #
  #/////////////////////////////////////////////////////////////////////////////  
    TitleFrame $w.tf1 -text [mc "Behaviour"]
    set f1 [$w.tf1 getframe]
  
    checkbutton $f1.whole -text [mc "Check whole table when changing types"] \
            -variable [varname data(options,whole)] -pady 0 -state disabled
    checkbutton $f1.inter -text [mc "Works with internal data"] \
            -variable [varname data(options,inter)] -pady 0 -state disabled

    grid $f1.whole -sticky w -padx 2 -pady 1
    grid $f1.inter -sticky w -padx 2 -pady 1
    
    grid $w.tf1 -sticky news -padx 2
    grid rowconfigure    $w 0 -weight 1
    grid columnconfigure $w 0 -weight 1
  }
  
  
  #/////////////////////////////////////////////////////////////////////////////
  method _OptionsCheckColSepType { } {
  #
  # PURPOSE: Manages behaviour of column separator widgets
  #
  #/////////////////////////////////////////////////////////////////////////////  
    switch $data(options,colseptype) {
      TAB   {
        focus $widget(options,rbColSep1)
        $widget(options,eColSep) configure -state disabled 
      }
      RET   {
        focus $widget(options,rbColSep2)
        $widget(options,eColSep) configure -state disabled 
      }
      OTHER { $widget(options,eColSep) configure -state normal }
    }
  }
  
  #/////////////////////////////////////////////////////////////////////////////
  method _OptionsCheckRowSepType { } {
  #
  # PURPOSE: Manages behaviour of row separator widgets
  #
  #/////////////////////////////////////////////////////////////////////////////  
    switch $data(options,rowseptype) {
      RET   {
        focus $widget(options,rbRowSep1)
        $widget(options,eRowSep) configure -state disabled 
      }
      TAB   {
        focus $widget(options,rbRowSep2)
        $widget(options,eRowSep) configure -state disabled 
      }
      OTHER { $widget(options,eRowSep) configure -state normal }
    }
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _OptionsOk { dialog } {
  #
  # PURPOSE: Sets selected options and destroys the options window
  #
  # PARAMETERS:
  #   dialog -> options window
  #
  #/////////////////////////////////////////////////////////////////////////////  
    $self _OptionsSet
    $self _OptionsDestroy $dialog
  }
  
  #/////////////////////////////////////////////////////////////////////////////
  method _OptionsDestroy { dialog } {
  #
  # PURPOSE: Destroys the options window
  #
  # PARAMETERS:
  #   dialog -> options window
  #
  #/////////////////////////////////////////////////////////////////////////////  
    destroy $dialog
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _RowTag { number } {
  #
  # PURPOSE: Returns tag that will be used in the given row number
  #
  # PARAMETERS:
  #   number -> row's number
  #
  #/////////////////////////////////////////////////////////////////////////////  
    if { ![$self cget -bgalt] } {
      return ""
    } else {
      if { [expr $number % 2] } {
	return bgalt1
      } else {
	return bgalt2
      }
    }
  }
  
  #/////////////////////////////////////////////////////////////////////////////
  method _CheckSaved { } {
  #
  # PURPOSE: Checks if a file has changed and asks for saving.
  #
  # RETURN: Returns ok if file has been saved correctly or cancel if not.
  #
  #/////////////////////////////////////////////////////////////////////////////
    
    set nameFile [$self cget -path]
    set isSaved  $data(isSaved)
    # checks if the file is saved
    if {!$isSaved} {
      if {$nameFile != ""} {
        set name $nameFile
      } else  {
        set name "untitled.bst"
      }

      set rq [tk_messageBox -type "yesnocancel" -icon warning \
              -title [mc "Warning"] -parent $self \
              -message "[mc "Table %1\$s has changed!" $name].\n\
              [mc "Do you want to save changes?"]"] 
      switch -exact $rq {
        yes {
          if {$nameFile != "" } {
            $self _FileSave $nameFile
          } else {
            $self _CmdSaveAs $nameFile
          }
        }
        cancel {return cancel}
        default {return ok}
      }
    }
    return ok
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _FileSave { path {replace 0}} {
  #
  # PURPOSE: Saves table data actually opened in table editor in a text file.
  #
  # PARAMETERS:
  #   path -> name of file
  #   replace -> replaces ? with ""
  #
  # RETURN: Returns 1 if file has been saved correctly or 0 if not.
  #
  #/////////////////////////////////////////////////////////////////////////////
    if { ![ catch { set fd [open $path w] } msgerr] } {
      set numCols [$self cget -cols]
      set list    [$self get @0,0 end]
      if $replace {
        set list [ListReplace $list {} "?"]
      }
      set struct "{$numCols {\n} {} { ; }} {1 { ; } {} {}}"
      set txt [ListToStrStruct $list $struct]  
      puts $fd $txt
      close $fd
      $self _IsSaved 1
      return 1
    }
    error $msgerr
  }



  #/////////////////////////////////////////////////////////////////////////////
  method _CmdExcel { } {
  #
  # PURPOSE: Opens Microsoft Excel with data of the table
  #
  #/////////////////////////////////////////////////////////////////////////////
    global tcl_platform
    global env

    if { [string equal $tcl_platform(platform) "windows"] } {      
      if { [$self cget -cols] < 256 && [$self cget -rows] < 65536 } {
        set tempcolsep [$self cget -colseparator]
        set temprowsep [$self cget -rowseparator]

        $self configure -colseparator "\t"
        $self configure -rowseparator "\n"

        exec $env(COMSPEC) /c start excel.exe &

        $self _CmdSpecialCopy table
        ::cwind::waitwind {Microsoft Excel*} 10
        ::cwind::send |CTRL+| v |CTRL-|
        $self configure -colseparator $tempcolsep
        $self configure -rowseparator $temprowsep
      } else {
        set rq [tk_messageBox -type "ok" -icon warning \
        -title [mc "Warning"] -parent $self\
        -message [mc "excellimit"]]
      }
    } else {
      set rq [tk_messageBox -type "ok" -icon warning \
      -title [mc "Warning"] -parent $self\
      -message [mc "onlywin"]]
    }
  }

  #/////////////////////////////////////////////////////////////////////////////
  method fillSerie { sergrp {format {} } } {
  #
  # PURPOSE: Procedure to fill data elements of the table. Gets data from a 
  #          group of series and sets them to the table.
  #
  # PARAMETERS:
  #   sergrp -> tol series group (created by toltcl ::tol::create command)
  #
  #/////////////////////////////////////////////////////////////////////////////
    upvar \#0 [varname matrix] matrix
    
    $self configure -state normal
    $self _DeleteRows 0 [$self cget -rows]
    $self _DeleteCols 0 [$self cget -cols]
    
    # insert the series into the table
    set numCols [$sergrp serie size]
    set numRows [$sergrp dating size]
    set matrix(0,0) [$sergrp dating name]
    
    for {set i 0} {$i < $numRows} {incr i} {
      set matrix([expr $i+1],0) [$self _GetTickLabel $sergrp $i]
    }
    for {set j 0} {$j < $numCols} {incr j} {
      set matrix(0,[expr $j+1]) [$sergrp serie $j name]
      foreach vec_data [$sergrp serie $j data] {
        set x [lindex $vec_data 0]
        set y [lindex $vec_data 1]
        for {set i 0} {$i < [llength $x]} {incr i} {
          set matrix([expr round([lindex $x $i])+1],[expr $j+1]) [lindex $y $i]
        }
      }
    }
    $self configure -rows [expr $numRows+1] -cols [expr $numCols+1]
    $self _InitState serie
    $self setType 0 date $format
  }
  
  #/////////////////////////////////////////////////////////////////////////////
  method _GetTickLabel { group tick } {
  #
  # PURPOSE: Gets ticks label
  #
  # PARAMETERS:
  #   group -> group
  #   tick  -> tick
  #
  #/////////////////////////////////////////////////////////////////////////////
    set str ""
    catch {
      set str [$group dating $tick]
    } 
    return $str
  }

  #/////////////////////////////////////////////////////////////////////////////
  method fillSet { tableset } {
  #
  # PURPOSE: Procedure to fill data elements of the table. Gets data from a 
  #          tableset and sets them to the table.
  #
  # PARAMETERS:
  #   tableset -> tol tableset
  #
  #/////////////////////////////////////////////////////////////////////////////
    upvar \#0 [varname matrix] matrix

    $self configure -state normal    
    $self _DeleteRows 0 [$self cget -rows]
    $self _DeleteCols 0 [$self cget -cols]

    set numRows [$tableset info datarows]
    set numCols [llength [$tableset info columns]]

    set i_ini 0
    set infHea [$tableset info header]
    set infCol [$tableset info columns]
    #insert the set into the table
    if {[string equal [lindex $infHea 0] "row"]} {
      set i_ini 1
      set j 1
      set colsF [expr [llength $infCol] - [lindex $infHea 1] -1]
      foreach lstInfo [lrange $infCol 0 $colsF] {
        set matrix($i_ini,$j) [lindex $lstInfo 1]
        #$self setType $j [string tolower [lindex $lstInfo 3]]
        incr j
      }
    } elseif {[string equal $infHea "struct"] || \
              [string equal $infHea "generic"]} {
      set j 1
      foreach lstInfo $infCol {
        set matrix($i_ini,$j) [lindex $lstInfo 1]
        #$self setType $j [string tolower [lindex $lstInfo 3]]
        incr j
      }
    } 
    set i 1
    foreach name [$tableset info rowsnames] {
      set matrix($i,0) $name
      incr i
    }
    $tableset data apply all [list $self _FillCell [expr $i_ini+1]]
  
    $self configure -rows [expr $numRows+$i_ini+1] -cols [expr $numCols+1]

    $self _InitState set
    set j 1
    set colsF [expr [llength $infCol] - [lindex $infHea 1] -1]
    foreach lstInfo [lrange $infCol 0 $colsF] {
      $self setType $j [string tolower [lindex $lstInfo 3]]
      incr j
    }
  }
  
  #/////////////////////////////////////////////////////////////////////////////
  method _FillCell { i_ini i j value } {
  #
  # PURPOSE: Fill each cell of the table.Only used by fillSet method
  #
  # PARAMETERS:
  #   i_ini -> initial row
  #   i -> row
  #   j -> column
  #   value -> the value
  #
  #/////////////////////////////////////////////////////////////////////////////
    upvar \#0 [varname matrix] matrix
    set matrix([expr $i_ini+$i],[expr $j+1]) $value
  }

  #/////////////////////////////////////////////////////////////////////////////
  method fillMatrix { refMatrix } {
  #
  # PURPOSE: Procedure to fill data elements of the table. Gets data from a 
  #          matrix and sets them to the table.
  #
  # PARAMETERS:
  #   refMatrix -> tol matrix
  #
  #/////////////////////////////////////////////////////////////////////////////
    upvar \#0 [varname matrix] matrix
    
    $self configure -state normal
    $self _DeleteRows 0 [$self cget -rows]
    $self _DeleteCols 0 [$self cget -cols]

    # insert the matrix into the table
    set numCols [lindex $refMatrix 2]
    set numRows [lindex $refMatrix 1]
  
    set matrix(0,0) [lindex $refMatrix 0]
    for {set j 0} {$j < $numCols} {incr j} {
      set matrix(0,[expr $j+1]) [expr $j+1]
    }
    set lst [lindex $refMatrix 3]
    for {set i 0} {$i < $numRows} {incr i} {
      set matrix([expr $i+1],0) [expr $i+1]
      for {set j 0} {$j < $numCols} {incr j} {
        set matrix([expr $i+1],[expr $j+1]) [lindex $lst [expr $numCols*$i+$j]]
      }
    }

    $self configure -rows [expr $numRows+1] -cols [expr $numCols+1]
    
    $self _InitState matrix
  }

  #///////////////////////////////////////////////////////////////////////////
  method fillSQL { stmt } {
  #
  # PURPOSE: Procedure to fill data elements of the table. Gets data from a 
  #          tolodbc statement and sets them to the table.
  #
  # PARAMETERS:
  #   stmt -> tolodbc statement
  #
  #///////////////////////////////////////////////////////////////////////////
    upvar \#0 [varname matrix] matrix

    $self configure -state normal
    $self _DeleteRows 0 [$self cget -rows]
    $self _DeleteCols 0 [$self cget -cols]

    $stmt execute



    set rowCount [$stmt rowcount]
    set colsname [$stmt columns]
    set types    [$stmt columns type]
    set colCount [llength $types]


    if {$rowCount == -1} {

      $widget(sb,lFunc) configure -text [mc "Query executed successfully"]

    } else  {

      $widget(sb,lFunc) configure -text [mc "Process completed. $rowCount rows affected"]

    }


    set j 0
    foreach colname $colsname {
      set matrix(0,$j) $colname
      incr j
    }

    set i 1
    while {[set row [$stmt fetch]] != {}} {
      set j 0
      foreach cell $row {
        set matrix($i,$j) $cell
        incr j
      }
      incr i
    }

    $self configure -rows $i -cols $colCount

    for {set i 0} {$i < [$self cget -cols]} {incr i} {
      set typecol [string tolower [NumSql2Tol [lindex $types $i]]]
      set data(types) [linsert $data(types) $i $typecol]
      set data(formats) [linsert $data(formats) $i {}]
    }

    $self _InitState sql
  }

  #/////////////////////////////////////////////////////////////////////////////
  method fillStatistics { sergrp } {
  #
  # PURPOSE: Procedure to fill data elements of the statistics table.
  #          Gets data from a group of series and sets them to the table.
  #
  # PARAMETERS:
  #   sergrp -> tol series group
  #
  #/////////////////////////////////////////////////////////////////////////////
    upvar \#0 [varname matrix] matrix
    
    $self configure -state normal
    $self _DeleteRows 0 [$self cget -rows]
    $self _DeleteCols 0 [$self cget -cols]

    Tolcon_Trace "SERGRP de fillStatistics method --> $sergrp"

    # insert the series into the table
    set stats0  [ $sergrp serie 0 stats ]
    set numRows [ llength $stats0 ]
    set numCols [ $sergrp serie size ]

    set row 1
    foreach {name value} $stats0 {
      set matrix($row,0) [ mc $name ]
      incr row
    }
    
    for { set j 0 } { $j < $numCols } { incr j } {
      set col [ expr { $j + 1 } ]
      set matrix(0,$col) [ $sergrp serie $j name ]
      set row 1
      foreach { s v } [ $sergrp serie $j stats ] {
        set matrix($row,$col) $v
        incr row
      }
    }
    
    $self configure -rows [expr $numRows+1] -cols [expr $numCols+1]
    
    $self _InitState statistics
  }

  #/////////////////////////////////////////////////////////////////////////////
  method fillAutocor { serie nLag } {
  #
  # PURPOSE: Procedure to fill data elements of the autocorrelation table.
  #          Gets data from a group of series and sets them to the table.
  #
  # PARAMETERS:
  #   sergrp -> tol series group
  #
  #/////////////////////////////////////////////////////////////////////////////
    upvar \#0 [varname matrix] matrix
    
    $self configure -state normal
    $self _DeleteRows 0 [$self cget -rows]
    $self _DeleteCols 0 [$self cget -cols]

    set typecor {ACF PACF IACF IPACF}
  
    set numCols [llength $typecor]
    if {$nLag == ""} {
      set numRows [lindex [::tol::autocor [lindex $typecor 0] $serie] 1]
    } else {
      set numRows $nLag
    }
      
    for {set i 0} {$i < $numRows} {incr i} {
      set matrix([expr $i+1],0) [expr $i+1]
    }
    for {set j 0} {$j < $numCols} {incr j} {
      set matrix(0,[expr $j+1]) [lindex $typecor $j]
      set values [eval ::tol::autocor [lindex $typecor $j] {$serie} $nLag]
      for {set i 0} {$i < $numRows} {incr i} {
        set matrix([expr $i+1],[expr $j+1]) \
               [lindex [lindex $values 3] $i]
      }
    }

    $self configure -rows [expr $numRows+1] -cols [expr $numCols+1]
    
    $self _InitState autocor
  }

  #///////////////////////////////////////////////////////////////////////////
  method _PasteHandler {cell txt} {
  #
  # PURPOSE: Pastes txt from the cell given (ini cell)
  # PARAMETERS: cell -> initial cell
  #             txt -> data to be pasted
  #
  #///////////////////////////////////////////////////////////////////////////
    # The needed rows and columns are calculated
    set rowIni [lindex [split $cell ","] 0]
    set colIni [lindex [split $cell ","] 1]
    set rsep [$self cget -rowseparator]
    set csep [$self cget -colseparator]
  
    if {[string comp {} $rsep]} { set drows [split $txt $rsep] }
    set numRows [llength $drows]
    if [string equal [lindex $drows end] {}] {
      set numRows [incr numRows -1]
    }
    set numCols 0  
    foreach line $drows {
      if [expr [llength [split $line $csep]] > $numCols] {
        set numCols [llength [split $line $csep]]
      }
    }
  
    #If more rows are necessary they are created
    if [expr $rowIni+$numRows > [$self cget -rows]] {
      $self configure -rows [expr $rowIni+$numRows]
      $self _UpdateInfoRows
    }
    #If more columns are necessary they are created
    set colsold [$self cget -cols]

    if [expr $colIni+$numCols > [$self cget -cols]] {
      $self configure -cols [expr $colIni+$numCols]
    }

    if { $created == 1 } {
      for {set i $colsold} {$i < [expr $colIni+$numCols]} {incr i} {
	set data(types) [linsert $data(types) $i text] 
	set data(formats) [linsert $data(formats) $i {}] 
      }
    } else {
      $self _UpdateTypes
    }
    
    # In version 2.8 of tktable is not possible to paste in title cells
    # so a cheat is necessary
    set titlerows [$self cget -titlerows]
    set titlecols [$self cget -titlecols]
    $self configure -titlerows 0 -titlecols 0
    
    $self _TablePasteHandler $self $cell $txt
    $self configure -titlerows $titlerows -titlecols $titlecols    
  }

  #///////////////////////////////////////////////////////////////////////////
  method _TablePasteHandler {w cell data2} {
  #
  # PURPOSE: Modification of tk::table::tk_tablePasteHandler. Trim when
  #          setting the data in the cell is added
  # PARAMETERS: w -> self
  #             cell -> the cell
  #             data2 -> the data to paste
  #
  #///////////////////////////////////////////////////////////////////////////
    upvar \#0 [varname matrix] matrix
    #
    # Don't allow pasting into the title cells
    #
    if {[$w tag includes title $cell]} {
        return
    }

    set rows	[expr {[$w cget -rows]-[$w cget -roworigin]}]
    set cols	[expr {[$w cget -cols]-[$w cget -colorigin]}]
    set r	[$w index $cell row]
    set c	[$w index $cell col]
    set rsep	[$w cget -rowseparator]
    set csep	[$w cget -colseparator]
    ## Assume separate rows are split by row separator if specified
    ## If you were to want multi-character row separators, you would need:
    # regsub -all $rsep $data <newline> data
    # set data [join $data <newline>]
    if {[string compare {} $rsep]} { set data2 [split $data2 $rsep] }
    set row	$r
    foreach line $data2 {
	if {$row > $rows} break
	set col	$c
	## Assume separate cols are split by col separator if specified
	## Unless a -separator was specified
	if {[string compare {} $csep]} { set line [split $line $csep] }
	## If you were to want multi-character col separators, you would need:
	# regsub -all $csep $line <newline> line
	# set line [join $line <newline>]
	foreach item $line {
	    if {$col > $cols} break
            #Trim is added to tk::table::tk_tablePasteHandler code
      	    set matrix($row,$col) [string trim $item " "]
	    incr col
	}
	incr row
    }
  }

  # delegation of methods and options
  delegate method * to tab
  delegate option * to tab
}

package provide btable 1.0

  # ------- TEST CODE ----

#/////////////////////////////////////////////////////////////////////////////
proc test_btable {} {
#
# PURPOSE: Test function btable
#
#/////////////////////////////////////////////////////////////////////////////

  destroy .top
  toplevel .top
  set tab [btable .top.tbl  -parent .top -iniconfig btable]

  pack $tab -fill both -expand true
}
