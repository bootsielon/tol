#/////////////////////////////////////////////////////////////////////////////
# FILE    : tolinspe.tcl
# PURPOSE : This file contains a namespace called TolInspector with its
#           functions. Its for creating a the TOL object inspector and
#           managing its events.
#/////////////////////////////////////////////////////////////////////////////

package require bguifunction
package require snit
package require btable
package require wtree 

if { 0 && ![info exists toltk_script_path] } {
  set toltk_script_path [file dirname [info script]]
  source [file join $toltk_script_path init_toltk.tcl]
}

if {"unix" eq $::tcl_platform(platform)} {
  foreach "but units" [list 4 -5 5 5] {
    set comm [string map "%d $units" {
      %W yview scroll %d units
    }]
    #@! REVISAR
    bind TreeView <$but> $comm
  }
} else {
  #@! REVISAR
  bind TreeView <MouseWheel> {
    %W yview scroll [expr int(-1*%D/36)] units
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc startupResources { } {
#
# PURPOSE: Establishes the preferences of the inspector
#
#/////////////////////////////////////////////////////////////////////////////    
  global tcl_platform
  global toltk_options
  DynamicHelp::configure -background #FFFFC0
  if {[string equal $tcl_platform(platform) "windows"]} {
    set toltk_options(colors,SystemButtonFace) SystemButtonFace
    set toltk_options(fonts,MenuNor)           {Tahoma 8}
    set toltk_options(fonts,Fixed)             {{Courier New} 9}
    set toltk_options(fonts,FixSmall)          {{Courier New} 8}
    set toltk_options(fonts,Title1)            {Arial 9 bold}
    set toltk_options(fonts,Label)             {Arial 9}
    set toltk_options(fonts,TxtSmall)          {Arial 8}
    set toltk_options(relief,ridge)            ridge
  } else {
    set toltk_options(colors,SystemButtonFace) #d4d4d4
    set toltk_options(fonts,MenuNor)           {-*-helvetica-medium-r}
    set toltk_options(fonts,Fixed)             {Courier 9}
    set toltk_options(fonts,FixSmall)          {-*-courier-small-r}
    set toltk_options(fonts,Title1)            {-*-helvetica-bold-r}
    set toltk_options(fonts,Label)             {-*-helvetica-medium-r}
    set toltk_options(fonts,TxtSmall)          {-*-helvetica-small-r}
    set toltk_options(relief,ridge)            raised
  }
  option add *background         $toltk_options(colors,SystemButtonFace) user
  option add *Label.font         $toltk_options(fonts,Label) user
  option add *Radiobutton.font   $toltk_options(fonts,Label) user
  option add *Checkbutton.font   $toltk_options(fonts,Label) user
  option add *Button.font        $toltk_options(fonts,Label) user
  option add *Menu.font          $toltk_options(fonts,MenuNor) user
  option add *TitleFrame.l.font  $toltk_options(fonts,Title1) user
  option add *Tabset.selectForeground black user

  option readfile [file join $::toltk_script_path toltk.rc] user
}

#/////////////////////////////////////////////////////////////////////////////
namespace eval ::TolInspector {
#
# PURPOSE: The purpose of this namespace is creating a the TOL object
#          inspector and managing its events.
# VARIABLES:
#   data: 
#   regFunLst:    List array with functions defined by user
#   wt_tree:      Tree widget containing the TOL objects (::wtree)
#   w_tabset:     Tabset widget containing trees of variables and functions
#   wt_vars:      Tree widget containing the tree of Variables (::wtree)
#   data_menu:    Array with data of menus
#   group_id:
#   wt_funcs:     Tree widget containing the tree of Functions (::wtree)
#   OnSelectItem: Command to invoke on SelectItem in wt_*
#   OnBusyCmd:    Command to invoke when busy
#   SubTypeImgs:  Images associated to types of set
#
#/////////////////////////////////////////////////////////////////////////////
  variable data
  variable arguments
  
  # List array with functions defined by user
  variable regFunLst
  
  # tree widget containing the TOL objects.
  variable wt_tree   ;# is a ::wtree
  
  # tabset widget containing trees of variables and functions 
  variable w_tabset

  # list widget containing the variables. It's the window of the tab Variable
  variable wt_vars      ;# is a ::wtree
  
  variable data_menu
  variable group_id -1

  # list widget containing the functions. It's the window of the tab Functions
  variable wt_funcs     ;# is a ::wtree

  # command to invoke on SelectItem in wt_*
  variable OnSelectItem

  # command to invoke when busy
  variable OnBusyCmd ""

  # images associated to types of set.
  variable SubTypeImgs {
    GenericSet MODFile PRJFile DIRFile TOLFile
    BDCFile BDTFile BSTFile BMTFile BSIFile TSIFile BMIFile
    StructuredSet TableSet}

}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::SubTypeImage { idx } {
#/////////////////////////////////////////////////////////////////////////////
  variable SubTypeImgs

  #Tolcon_Trace "SubTypeImage $idx = [lindex $SubTypeImgs $idx]"
  ::Bitmap::get [lindex $SubTypeImgs $idx]
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::Create { w args } {
#
# PURPOSE: Creates the object inspector, that contains two panels with three
#          treeviews and three trees.
#           - One for the left side of the inspector (with grammars,
#             included files and console objects)
#           - And two for the right one (one for variables tab and one for
#             functions tab)
#
# PARAMETERS:
#   w:    path where tol inspector must be created
#   args: list of pairs {-option value} with options for the inspector.
#      -onselcmd  Comand to execute when a node is selected
#      -wleft     relative weight of left pane
#      -wrigth    relative weight of rigth pane
#
#/////////////////////////////////////////////////////////////////////////////
  global toltk_options
  variable wt_tree  
  variable wt_vars  
  variable data_menu
  variable wt_funcs  
  variable w_tabset
  variable OnSelectItem
  variable OnBusyCmd ""

  # this should be used insted of a lot of scalar variables.
  # but, what can we do now? 

  variable widgets
  variable data

  array set options {
    -onselcmd ""
    -wleft    3
    -wright   7
  }
  array set options $args
  
  # initializes the command OnSelectItem to ""
  set OnSelectItem $options(-onselcmd)

  #upvar \#0 ::TolTk::Images Images
  
  bind TreeCtrl <Button1-Motion> {
    TreeCtrl::Motion1 %W %x %y
  }

  # creation of PANED WINDOW MAIN
  set pwMain [PanedWindow $w -side bottom -pad 0 -width 4]
  # creation of pane left and pane right
  set paneLeft  [$pwMain add -weight $options(-wleft)]
  set paneRight [$pwMain add -weight $options(-wright)]
  set widgets(panes,left)  $paneLeft
  set widgets(panes,right) $paneRight
  
  # when a panel is destroyed, the item 'geometry,side' of the array variable
  # data gives the value of the width of panel 'side' (left or right)
  bind $paneLeft <Destroy> {
    namespace eval  ::TolInspector {
      variable data
      variable widgets
      set data(geometry,left)  [winfo width $widgets(panes,left)]
    }
  }  
  bind $paneRight <Destroy> {
    namespace eval  ::TolInspector {
      variable data
      variable widgets
      set data(geometry,right)  [winfo width $widgets(panes,right)]
    }
  }
  # bind for destruction of paned window main
  bind $pwMain <Destroy> ::TolInspector::Destroy

  # creation of pane left with tree with the TOL OBJECTS
  CreatePaneLeft  $paneLeft
  # creation of the right panel with TABSET of VARIABLES and FUNCTIONS
  CreatePaneRight $paneRight 
  # creation of the menus
  CreateMenus
  # packing the panedwindowmain
  pack $pwMain -fill both -expand yes

  # esto es para evitar que aparezca el icono open cuando
  # relleno estando el mouse sobre ht_*
  #@! Por revisar
  #@! $ht_vars  bind Entry <Enter>  ""
  #@! $ht_vars  bind Entry <Leave>  ""
  #@! $ht_funcs bind Entry <Enter>  ""
  #@! $ht_funcs bind Entry <Leave>  ""
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::CreatePaneLeft { paneLeft } {
#
# PURPOSE: Creation of TREE LEFT with the TOL OBJECTS
#
# PARAMETERS:
#   paneLeft:  path of the pane of the left in TolInspector
#
#/////////////////////////////////////////////////////////////////////////////
  global toltk_options
  variable wt_tree
  
  # creation of the ScrolledWindow of the left side
  set sw1 [ScrolledWindow $paneLeft.sw -ipad 0 -borderwidth 0 -scrollbar none]

  # creation of the left tree
  set wt_tree [::wtree $sw1.wt -table 0 -filter no \
    -background white -showroot 1 -selectmode single -showheader 1 \
    -highlightbackground gray75 -highlightcolor black \
    -highlightthickness 0 -font $toltk_options(fonts,Label) \
    -columns [::TolInspector::WNodeDef 1] ]
  #@> poner showheader a 0
    
  # configuration of the left tree
  #$wt_tree column configure "range 1 end" -visible no
  #$wt_tree column configure tail -visible no
  $wt_tree column configure first -expand yes -weight 1
  $wt_tree column configure all -itembackground ""       
  $wt_tree notify bind $wt_tree <Selection> "::TolInspector::SelectObject"    
  $wt_tree notify bind $wt_tree <Expand-before> "::TolInspector::OpenObject %I"
  $wt_tree notify bind $wt_tree <Collapse-before> "::TolInspector::CloseObject %I"  
           
  # creation of the root node "Tol Objects"
  set img [::Bitmap::get Objects]
  #@ Se pone el style a mano, no sé la manera natural de hacer esto con wtree
  $wt_tree item style set root [WiName 1] [$wt_tree column cget [WiName 1] -itemstyle]
  $wt_tree item text root [WiName 1] [mc "Tol Objects"]
  $wt_tree item image root [WiName 1] $img

  # creation of the node father "Grammars"
  set img [::Bitmap::get Grammars]  
  set wt_row [::TolInspector::WNode 1 "" Root $img [mc "Grammars"] "" "" "" "root-grammars" "Grammar"]
  $wt_tree insert $wt_row -at child -relative root -tags "root-grammars"
  # Inserts in the node of grammars a node by each grammar that exists in Tol
  InsertGrammars

  # creation of the node father "Packages"
  set img [::Bitmap::get "package"]  
  set wt_row [::TolInspector::WNode 1 "" Root $img [mc "Packages"] "" "" "" "root-packages" "Package"]    
  $wt_tree insert $wt_row -at child -relative root -tags "root-packages" -button yes

  # creation of the node father "Included Files"
  set img [::Bitmap::get "Files"]
  set wt_row [::TolInspector::WNode 1 "" Root $img [mc "Included Files"] "" "" "" "root-files" "File"]    
  $wt_tree insert $wt_row -at child -relative root -tags "root-files" -button yes
      
  # creation of the node father "Console Objects"  
  set img [::Bitmap::get "Console"]
  set wt_row [::TolInspector::WNode 1 "" Root $img [mc "Console Objects"] "" "" "" "root-console" "Console"]  
  $wt_tree insert $wt_row -at child -relative root -tags "root-console" -button yes
      
  # associates the tree to scrolled window 
  $sw1 setwidget $wt_tree

  # packing 
  pack $sw1 -fill both -expand yes
  
  # bind the focus on tree
  bind ${wt_tree}.frameTree.t <Button-1> "::focus $wt_tree"
  
  # expand/collapse (and fill/unfill) a node with a double clic
  bind ${wt_tree}.frameTree.t <Double-1> "$wt_tree item toggle \[ $wt_tree selection get 0 \]"
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::CreatePaneRight { paneRight } {
#
# PURPOSE: Creation of the right pane with TABSET with VARIABLES and FUNCTIONS
#
# PARAMETERS:
#   paneLeft:  path of the pane of the right in TolInspector
#
#/////////////////////////////////////////////////////////////////////////////
  global toltk_options
  variable w_tabset
  variable wt_vars
  variable wt_funcs
  
  # creates the tabset to display the variables & functions    
  #set w_tabset [::blt::tabset $paneRight.ts -relief flat\
            -bd 0 -outerpad 0 -tier 2 -slant none -textside top \
            -highlightthickness 0 -side left -rotate 90]
  set w_tabset [::blt::tabset $paneRight.ts -relief flat\
            -bd 0 -outerpad 0 -tier 2 -slant right -textside right \
            -highlightthickness 0]
  # creates the scrolled windows for each one of the tabs
  set sw21 [ScrolledWindow $w_tabset.sw12 -scrollbar none]  
  set sw22 [ScrolledWindow $w_tabset.sw22 -scrollbar none]
  
  # inserts in the tabset the tabs of functions and variables
  $w_tabset insert end \
      Variables -text [mc Variables] -selectbackground \#d9d9d9 -bg gray75 \
      -image [::Bitmap::get Variables] -pady 0\
      Functions -text [mc Functions] -selectbackground \#d9d9d9 -bg gray75 \
      -image [::Bitmap::get Functions] -pady 0

  # Creation of the right TREE RIGHT with VARIABLES
  set wt_vars [::wtree $sw21.vars -table 1 -filter no \
    -background white -showroot 0 -selectmode extended -showheader 1 \
    -highlightbackground gray75 -highlightcolor black \
    -highlightthickness 0 -font $toltk_options(fonts,Label) \
    -columns [::TolInspector::WNodeDef 0] ]
  # configuration of the tree of variables
  $wt_vars column configure all -itembackground ""     
  $wt_vars notify bind $wt_vars <Expand-before> "::TolInspector::OpenVariable %I"
  $wt_vars notify bind $wt_vars <Selection> "::TolInspector::SelectItem $wt_vars"
  #@! el bind debería poder hacerse directamente sobre el wtree
  bind ${wt_vars}.frameTree.t  <Button-1> "::focus $wt_vars"    
  bind ${wt_vars}.frameTree.t <Double-1> "::TolInspector::SelectionExpand"
  
  # Creation of the right TREE RIGHT with FUNCTIONS
  set wt_funcs [::wtree $sw22.vars -table 1 -filter no \
    -background white -showroot 0 -selectmode extended -showheader 1 \
    -highlightbackground gray75 -highlightcolor black \
    -highlightthickness 0 -font $toltk_options(fonts,Label) \
    -columns [::TolInspector::WNodeDef 0] ]
  # configuration of the tree of functions
  $wt_funcs column configure all -itembackground ""     
  $wt_funcs notify bind $wt_funcs <Selection> "::TolInspector::SelectItem $wt_funcs"      
  bind ${wt_funcs}.frameTree.t <Button-1> "::focus $wt_funcs"      
  
  # associates the trees to scrolled windows
  $sw21 setwidget $wt_vars
  $sw22 setwidget $wt_funcs  
  # configure the tabs
  $w_tabset tab configure Variables -window $sw21 -fill both -padx 0 
  $w_tabset tab configure Functions -window $sw22 -fill both -padx 0

  # packing the tabset
  pack $w_tabset -fill both -expand yes
  $w_tabset tab configure Variables\
         -command "::TolInspector::SelectItem $wt_vars"
  $w_tabset tab configure Functions\
         -command "::TolInspector::SelectItem $wt_funcs"
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::CreateMenus {} {
#
# PURPOSE: Create the menus of the trees
#
#/////////////////////////////////////////////////////////////////////////////
  variable data_menu
  variable wt_vars
  variable wt_funcs
  variable wt_tree  

  set data_menu(main)          [menu $wt_tree.m -tearoff 0]
  set data_menu(Code,File)     [menu $data_menu(main).mcof -tearoff 0]
  set data_menu(Complex,File)  [menu $data_menu(main).mcxf -tearoff 0]
  set data_menu(Date,File)     [menu $data_menu(main).mdaf -tearoff 0]
  set data_menu(Matrix,File)   [menu $data_menu(main).mmaf -tearoff 0]
  set data_menu(VMatrix,File)  [menu $data_menu(main).mvmf -tearoff 0]
  set data_menu(Polyn,File)    [menu $data_menu(main).mpof -tearoff 0]
  set data_menu(Ratio,File)    [menu $data_menu(main).mraf -tearoff 0]
  set data_menu(Real,File)     [menu $data_menu(main).mref -tearoff 0]
  set data_menu(Serie,File)    [menu $data_menu(main).msef -tearoff 0]
  set data_menu(Set,File)      [menu $data_menu(main).mstf -tearoff 0]
  set data_menu(File,File)     [menu $data_menu(main).mfif -tearoff 0]
  set data_menu(Anything,File) [menu $data_menu(main).mspf -tearoff 0]
  set data_menu(Text,File)     [menu $data_menu(main).mtef -tearoff 0]
  set data_menu(TimeSet,File)  [menu $data_menu(main).mtif -tearoff 0]
  set data_menu(NameBlock,File)  [menu $data_menu(main).mnbf -tearoff 0]

  set data_menu(Code,Functions)     [menu $data_menu(main).mcofu -tearoff 0]
  set data_menu(Complex,Functions)  [menu $data_menu(main).mcxfu -tearoff 0]
  set data_menu(Date,Functions)     [menu $data_menu(main).mdafu -tearoff 0]
  set data_menu(Matrix,Functions)   [menu $data_menu(main).mmafu -tearoff 0]
  set data_menu(VMatrix,Functions)  [menu $data_menu(main).mvmfu -tearoff 0]
  set data_menu(Polyn,Functions)    [menu $data_menu(main).mpofu -tearoff 0]
  set data_menu(Ratio,Functions)    [menu $data_menu(main).mrafu -tearoff 0]
  set data_menu(Real,Functions)     [menu $data_menu(main).mrefu -tearoff 0]
  set data_menu(Serie,Functions)    [menu $data_menu(main).msefu -tearoff 0]
  set data_menu(Set,Functions)      [menu $data_menu(main).mstfu -tearoff 0]
  set data_menu(File,Functions)     [menu $data_menu(main).mfifu -tearoff 0]
  set data_menu(Anything,Functions) [menu $data_menu(main).mspfu -tearoff 0]
  set data_menu(Text,Functions)     [menu $data_menu(main).mtefu -tearoff 0]
  set data_menu(TimeSet,Functions)  [menu $data_menu(main).mtifu -tearoff 0]
  set data_menu(NameBlock,Functions)  [menu $data_menu(main).mnbfu -tearoff 0]

  set data_menu(Matrix,Table)       [menu $data_menu(main).mmat   -tearoff 0]
  set data_menu(Matrix,Export)      [menu $data_menu(main).mmae   -tearoff 0]
  set data_menu(Matrix,Draw)        [menu $data_menu(main).mmac   -tearoff 0]
  set data_menu(Matrix,Export,BDT)  [menu $data_menu(main).mmaebdt -tearoff 0]
  set data_menu(Serie,DrawAutocor)  [menu $data_menu(main).mseda  -tearoff 0]
  set data_menu(Serie,TableAutocor) [menu $data_menu(main).mseta  -tearoff 0]
  set data_menu(Set,Table)          [menu $data_menu(main).mstt   -tearoff 0]
  set data_menu(Set,Table2)         [menu $data_menu(main).mstt2  -tearoff 0]
  set data_menu(Set,Draw)           [menu $data_menu(main).mstd   -tearoff 0]
  set data_menu(Set,Draw,Special)   [menu $data_menu(main).mstds  -tearoff 0]
  set data_menu(Set,Draw,Special,M) [menu $data_menu(main).mstdsm -tearoff 0]  
  set data_menu(File,View)          [menu $data_menu(main).mfiv   -tearoff 0]
  set data_menu(File,Decompile)     [menu $data_menu(main).mfid   -tearoff 0]
  set data_menu(TimeSet,View)       [menu $data_menu(main).mtiv   -tearoff 0]
  #$ht_vars  bind Entry <Button-3> +[$ht_vars bind Entry <Button-1>]
  
  # composes the  menus for each one of the trees, depending the node that is selected
  bind ${wt_tree}.frameTree.t <Button-3> "::TolInspector::PostTree %X %Y" 
  bind ${wt_vars}.frameTree.t <Button-3> "::TolInspector::PostVariable %X %Y" 
  bind ${wt_funcs}.frameTree.t <Button-3> "::TolInspector::PostFunction %X %Y" 
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::GetWeight { pane } {
#
# PURPOSE: Set data(geometry,$pane) to the value of the current geometry
#          of the pane
#
# PARAMETERS:
#   pane -> string with a value "left" or "right" for each one of panels
#
#///////////////////////////////////////////////////////////////////////////// 
  variable widgets
  variable data

  if {[winfo exists $widgets(panes,$pane)]} {
    set data(geometry,$pane) [winfo width $widgets(panes,$pane)]
  }
  set data(geometry,$pane) 
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::Destroy { } {
#
# PURPOSE: Destroys the three trees of TolInspector and array with full
#          content & description of items
#
#/////////////////////////////////////////////////////////////////////////////  
  #@! wtree destroy ??
  return
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::OneLine { str } {
#
# PURPOSE: Gives a string with carriage returns \n and return this string
#          until the first carriage return that appears after a word,
#          replacing the rest by ... if some word existed after that carriage
#          return
#          Examples: the str "a\n b c"     returns "a..."
#                            "\n \n a\n b" returns "a..."
#                            "\n \n a\n"   returns "a"
# PARAMETERS:
#   str -> string with \n
#
#/////////////////////////////////////////////////////////////////////////////
  set str [string trim $str]
  set lnidx [string first "\n" $str]
  if { $lnidx != -1 } {
    if { [incr lnidx -1] > 0 } {
      return "[string range $str 0 $lnidx]..."
    } else {
      return "..."
    }
  }
  set str
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::TrimContent { grammar content } {
#/////////////////////////////////////////////////////////////////////////////  
  if { [string equal "Real" $grammar] } {
     return [::TolConsole::Format $content]
  } else {
    if { [string equal \"\n\" $content] } {
      set content {"\n"}
    }
    return [OneLine $content]
    set content [string trim $content]
    set lnidx [string first "\n" $content]
    if { $lnidx != -1 } {
      if { [incr lnidx -1] > 0 } {
        return "[string range $content 0 $lnidx]..."
      } else {
        return "..."
      }
    }
    set content
  }
}

#>> [tolinspe_wtree1.tcl]
#>> proc ::TolInspector::InsertChild
#>> proc ::TolInspector::InsertItem
#>> proc ::TolInspector::ClearHiertables
#>> proc ::TolInspector::OpenObject
#>> proc ::TolInspector::InsertPackages
#>> proc ::TolInspector::InsertGrammars
#>> proc ::TolInspector::InsertFiles
#>> proc ::TolInspector::InsertConsoleObj
#>> proc ::TolInspector::InsertSubset

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::FilterNameBlockMember { name } {
#/////////////////////////////////////////////////////////////////////////////
  set st_reg [ regexp {(.+::)?(_)?(\.)?(.*)} $name => p1 p2 p3 p4]
  if { !$st_reg } {
    list -show 1 -fcolor "black" -name $name
  } elseif { $p2 eq "_" } {
    if { $p3 eq "." } {
      if { [string match {autodoc.member.*} $p4] } {
        # es un elemento de autodocumentacion y no se muestra
        list -show 0
      } {
        # es un miembro de solo lectura
        list -show 1 -fcolor "gray50" -name "_.$p4"
      }
    } else {
      # es un miembro oculto, 
      set showPrivate [ Tol_GetViewPrivateMembers ]
      list -show $showPrivate -fcolor "gray75" -name "_$p4"
    }
  } else {
    list -show 1 -fcolor "black" -name $p4
  }
}

#>> [tolinspe__wtree2.tcl]
#>> proc ::TolInspector::ClearConsoleObj
#>> proc ::TolInspector::ClearConsoleObjSel
#>> proc ::TolInspector::UpdateFileRoot
#>> proc ::TolInspector::UpdateConsoleObj

#>> [tolinspe_wtree1.tcl]
#>> proc ::TolInspector::SelectObject
#>> proc ::TolInspector::SelectFileRoot
#>> proc ::TolInspector::SelectConsoleRoot
#>> proc ::TolInspector::SelectGrammar
#>> proc ::TolInspector::SelectSet

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::GetParentGrammar { set_ref } {
#/////////////////////////////////////////////////////////////////////////////
  set root [ lindex $set_ref 0 ]
  set L [ llength $set_ref ]
  if { ( $root eq "File" && $L <= 2 ) } {
    set root
  } else {
    lindex [ tol::info var $set_ref ] 0
  }
}

#>> [tolinspe_wtree1.tcl]
#>> proc ::TolInspector::CloseObject
#>> proc ::TolInspector::OpenVariable
#>> proc ::TolInspector::SelectItem

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::GetItemName { prefix idx_list } {
#/////////////////////////////////////////////////////////////////////////////  
  set node_id $prefix
  foreach id $idx_list {
    append node_id "-$id"
  }
  set node_id
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::OnBusy { cmd } {
#/////////////////////////////////////////////////////////////////////////////  
  variable OnBusyCmd $cmd
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::Busy { } {
#/////////////////////////////////////////////////////////////////////////////  
  variable OnBusyCmd
  variable wt_tree  
  variable wt_vars

  $wt_tree configure -cursor watch
  $wt_vars configure -cursor watch
  if { [string length $OnBusyCmd] } {
    eval $OnBusyCmd 1
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::NotBusy { } {
#/////////////////////////////////////////////////////////////////////////////  
  variable OnBusyCmd
  variable wt_tree   
  variable wt_vars

  $wt_tree configure -cursor ""
  $wt_vars configure -cursor ""

  if { [string length $OnBusyCmd] } {
    eval $OnBusyCmd 0
  }
}

#>> [tolinspe__tolgui.tcl]
#>> proc TclList2SetOfText
#>> proc Tol_*
#>> proc TolGui_*


#>> [tolinspe__wtree2.tcl]
#>> proc ::TolInspector::PostVariable
#>> proc ::TolInspector::PostFunction
#>> proc ::TolInspector::PostTree
#>> proc ::TolInspector::AddToEvalWindow
#>> proc ::TolInspector::SelectAll
#>> proc ::TolInspector::ToggleSelection

#>> [__draw.tcl]
#>> proc ::TolInspector::ExportBDTMatrix
#>> proc ::TolInspector::Draw*
#>> proc ::TolInspector::Table*
#>> proc ::TolInspector::View*

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::DecompileFile {name} {
#
# PURPOSE:
#
# PARAMETERS:
#   name -> cadena con el nombre de un archivo o el de una variable
#           que hace referencia a un archivo
#
# TODO: Actualmente en name solo puede llegar el nombre de un archivo
#
#/////////////////////////////////////////////////////////////////////////////
  #puts "DECOMPILE 1.1, name=$name"
  if {[file isfile $name]} {
    set path $name
  } else  {
    set path [::tol::info path $name]
  }
  #puts "DECOMPILE 1.2"
  #Comprobar si el archivo ya está abierto y si es así poner que no está compilado
  if [namespace exists ::Editor] {
    foreach ins [namespace children ::Editor] {
      upvar \#0 ${ins}::data dataEd
      if [string equal $dataEd(nameFile) $path] {
        set dataEd(isCompiled) 0
      }
    }
  }
  # https://www.tol-project.org/ticket/1483
  catch {
    ::tol::decompile $path
  }
  ::TolInspector::UpdateFileRoot
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::ReadUserFunctions {} {
#
# PURPOSE: Recorre el set global que tiene la definicion de funciones y va
#          guardando esa informacion en arrays para almacenarla finalmente en
#          regFunLst
#
#/////////////////////////////////////////////////////////////////////////////
  variable regFunLst
  
  #Set global que tiene la definicion de funciones
  set obj GuiFunction

  if { [llength [set infAry [TclInfoVar Set $obj]] ] } {
    #Tolcon_Trace "ReadUserFunctions: infAry = $infAry"
    array set info $infAry
    set nFun [lindex $info(CONTENT) 0]
    #Tolcon_Trace "Numero de funciones =$nFun"
    for  {set idf 1} {$idf <= $nFun} {incr idf} {
      array set ifNameFun [TclInfoRef [list Set $obj $idf 1]]
      array set ifNameGui [TclInfoRef [list Set $obj $idf 2]]
      array set ifTypeGui [TclInfoRef [list Set $obj $idf 3]]
      array set ifTypeOut [TclInfoRef [list Set $obj $idf 4]]
      array set ifParams  [TclInfoRef [list Set $obj $idf 5]]
      #Hago lindex $a(CONTENT) 0 para quitar las comillas
      set regFun($idf,nameFun) [lindex $ifNameFun(CONTENT) 0]
      set regFun($idf,nameGui) [lindex $ifNameGui(CONTENT) 0]
      set regFun($idf,typeGui) [lindex $ifTypeGui(CONTENT) 0]
      set regFun($idf,typeOut) [lindex $ifTypeOut(CONTENT) 0]
      set nPar [lindex $ifParams(CONTENT) 0]
      #Tolcon_Trace "Numero de parametros =$nPar"
      for  {set idp 1} {$idp <= $nPar} {incr idp} {
        array set ipNameGui [TclInfoRef [list Set $obj $idf 5 $idp 1]]
        array set ipTypePar [TclInfoRef [list Set $obj $idf 5 $idp 2]]
        array set ipTypeSel [TclInfoRef [list Set $obj $idf 5 $idp 3]]
        array set ipListVal [TclInfoRef [list Set $obj $idf 5 $idp 4]]
        set regPar($idp,nameGui) [lindex $ipNameGui(CONTENT) 0]
        set regPar($idp,typePar) [lindex $ipTypePar(CONTENT) 0]
        set regPar($idp,typeSel)         $ipTypeSel(CONTENT)
        set nVal [lindex $ipListVal(CONTENT) 0]
        #Tolcon_Trace "nVal=$nVal"
        set lstVal ""
        for  {set idv 1} {$idv <= $nVal} {incr idv} {
           array set iVal [TclInfoRef [list Set $obj $idf 5 $idp 4 $idv]]
           lappend lstVal [lindex $iVal(CONTENT) 0]
        }
        set regPar($idp,lstVal) $lstVal
      }
      set regFun($idf,parameters) [array get regPar]
      array unset regPar
      #parray regFun
    }
    set regFunLst [array get regFun]
  } else {
    set regFunLst ""
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::ShowMenuUserFunc {gra} {
#
# PURPOSE: Muestra un menu contextual que...
#
#/////////////////////////////////////////////////////////////////////////////
  variable data_menu
  variable regFunLst
  
  #puts "ShowMenuUserFunc: gra=$gra"
  if {[expr [info exist ::TolInspector::regFunLst] && [llength $regFunLst] ]} {
    array set regFun $regFunLst
    regsub -all ,nameGui \
       [array names regFun *,nameGui] "" lstFun
    foreach idf [lsort -integer $lstFun] {
      set typeGui [split $regFun($idf,typeGui) .]
      switch  [lindex $typeGui 0] {
        Ite - Set {
          if {[lindex $typeGui 1] eq $gra} {
            $data_menu($gra,Functions) add command -label [mc $regFun($idf,nameGui)]\
              -command [list ::TolInspector::CallUserFunction $gra $idf]
          }
        }
        default {
        }
      }
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::CallUserFunction {grammar idx} {
#
# PURPOSE: Para los tipos de funciones Ite y Set, muestra un dialogo que 
#          pide los 
#
#/////////////////////////////////////////////////////////////////////////////
  variable regFunLst
  variable group_id
  variable options_selected

  upvar \#0 ::TolInspector::options_selected($grammar) items_selected
  #puts "CallUserFunction. grammar=$grammar, idx=$idx, items_selected=$items_selected"
  
  set all_objs [ list ]
  foreach it $items_selected {
    set tcl_ref [ lindex $it 0 ]
    set obj_addr [ ::tol::info address $tcl_ref ]
    lappend all_objs "GetObjectFromAddress(\"$obj_addr\")"
  }

  if { [ llength $all_objs ] } {
    incr group_id
    array set regFun $regFunLst
    set gui [bguifunction .t$group_id \
                 -title $regFun($idx,nameGui) \
                 -regpars $regFun($idx,parameters)]
    set txtPars [join [$gui get values] ,]
    if {$txtPars ne ""} {
      set txtPars ",$txtPars"
    }
    destroy $gui
    set typeGui [split $regFun($idx,typeGui) .]
    switch [lindex $typeGui 0] {
      Ite {
        foreach obj $all_objs {
          set cmd "$regFun($idx,typeOut) $regFun($idx,nameFun) ($obj $txtPars)"
          #Tolcon_Trace "Ite.cmd=$cmd"
          ::tol::console eval $cmd
          ::TolConsole::SaveEval $cmd
        }
      }
      Set {
        set obj_args [join $all_objs ,]
        set setCmd "SetOf[lindex $typeGui 1] ($obj_args)"
        set cmd "$regFun($idx,typeOut) $regFun($idx,nameFun) ($setCmd $txtPars)"
        #Tolcon_Trace "Set.cmd=$cmd"
        ::tol::console eval $cmd
        ::TolConsole::SaveEval $cmd
      }
      default {
      }
    }
    ::TolInspector::UpdateConsoleObj
  }
}
