#/////////////////////////////////////////////////////////////////////////////
# FILE    : tolinspe.tcl
# PURPOSE : This file contains a namespace called TolInspector with its
#           functions. Its for creating a the TOL object inspector and
#           managing its events.
#/////////////////////////////////////////////////////////////////////////////

package require bguifunction
package require snit
package require btable

if { 0 && ![info exists toltk_script_path] } {
  set toltk_script_path [file dirname [info script]]
  source [file join $toltk_script_path init_toltk.tcl]
}

if {"unix" eq $::tcl_platform(platform)} {
  foreach "but units" [list 4 -5 5 5] {
    set comm [string map "%d $units" {
      %W yview scroll %d units
    }]
    bind TreeView <$but> $comm
  }
} else {
  bind TreeView <MouseWheel> {
    %W yview scroll [expr int(-1*%D/36)] units
  }
}
#
##/////////////////////////////////////////////////////////////////////////////
#proc dump { args } {
##
## PURPOSE: Auxiliary procedure to show a string
##
##/////////////////////////////////////////////////////////////////////////////
#puts $args
#}

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
#   tolset:       Name of the current TOL set variable
#   tolindex:
#   at_set:
#   node_prefix:
#   ht_tree:      Tree widget containing the TOL objects (:blt::treeview)
#   blt_tree:     Tree widget containing the TOL objects (:blt::tree)
#   w_tabset:     Tabset widget containing trees of variables and functions
#   ht_vars:      Tree widget containing the tree of Variables
#                 (:blt::treeview)
#   blt_vartree:  Tree widget containing the tree of Variables (:blt::tree)
#   data_menu:    Array with data of menus
#   group_id:
#   ht_funcs:     Tree widget containing the tree of Functions
#                 (:blt::treeview)
#   blt_functree: Tree widget containing the tree of Functions (:blt::tree)
#   item_data:    Array to hold the full content & description of items
#   item_id:      Current index of item to insert in ht_* starting from 1
#   OnSelectItem: Command to invoke on SelectItem in ht_*
#   OnBusyCmd:    Command to invoke when busy
#   SubTypeImgs:  Images associated to types of set
#
#/////////////////////////////////////////////////////////////////////////////
  variable data
  
  # List array with functions defined by user
  variable regFunLst
  
  # name of the current TOL set variable.
  variable tolset ""
  variable tolindex ""
  # estas variables desapareceran cuando ::tol::forallchild admita scripts
  # completos y no solo nombres de proc
  variable at_set
  variable node_prefix

  variable iterSet ""
  variable knownReference ""

  variable gra_parent ""
  # tree widget containing the TOL objects.
  variable ht_tree   ;# is a ::blt::treeview
  variable blt_tree  ;# is a ::blt::tree

  # tabset widget containing trees of variables and functions 
  variable w_tabset

  # list widget containing the variables. It's the window of
  # the tab Variable
  variable ht_vars      ;# is a ::blt::treeview
  variable blt_vartree  ;# is a ::blt::tree
  
  variable data_menu
  variable group_id -1

  # list widget containing the functions. It's the window of
  # the tab Functions
  variable ht_funcs     ;# is a ::blt::treeview
  variable blt_functree ;# is a ::blt::tree

  # array to hold the full content & description of items
  variable item_data

  # current index of item to insert in ht_* starting from 1
  variable item_id
  
  # tell if we are processing a SelectSet event

  #variable selecting_set
  #variable closing

  # command to invoke on SelectItem in ht_*
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
proc ::TolInspector::CreateColumns { ht } {
#
# PURPOSE: Creates the columns (index, name, content, path, description )
#          of the tree of variables or functions
#
# PARAMETERS:
#   ht: path of ::blt::treeview of variables or ::blt::treeview of functions
#
#/////////////////////////////////////////////////////////////////////////////
  # main column ==> Name
  $ht column configure treeView -text [mc "Name"] -justify left \
      -bg white -activetitlebackground gray90 -pad 3
  # index column
  $ht column insert 0 Index \
      -bg white -justify left -activetitlebackground gray90
  # columns Content Path & Description
  #(pgea) se oculta la columna correspondiente a la referencia
  $ht column insert end Content Description Path\
      -bg white -justify left -activetitlebackground gray90 -pad 3
  $ht column insert end Reference -hide yes
  foreach c {Index Content Description Path} {
    $ht column configure $c -text [mc "$c"]
  }
  # associates the command of order to each one of the titles of the
  # columns of the trees
  foreach column [$ht column names] {
    $ht column configure $column \
      -command [list ::TolInspector::SortColumn $ht $column]
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::SortColumn {ht column} {
#
# PURPOSE: Associates the command of order to the title of a column of the
#          tree of variables or the tree of functions
#
# PARAMETERS:
#   ht:     path of ::blt::treeview of variables or ::blt::treeview of
#           functions
#   column: name of the column of this tree
#
#/////////////////////////////////////////////////////////////////////////////  
  set old [$ht sort cget -column] 
  set decreasing 0
  if { "$old" == "$column" } {
    set decreasing [$ht sort cget -decreasing]
    set decreasing [expr !$decreasing]
  }
  $ht sort configure -decreasing $decreasing -column $column -mode integer
  $ht configure -flat no
  $ht sort auto yes

  blt::busy hold $ht
  update
  blt::busy release $ht
}


#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::Create { w args } {
#
# PURPOSE: Creates the object inspector, that contains two panels with three
#          treeviews and three trees.
#           - One for the left side of the inspector (with grammars,
#             included files, console objects and object's spool)
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
  variable ht_tree
  variable blt_tree
  variable ht_vars
  variable blt_vartree
  variable data_menu
  variable ht_funcs
  variable blt_functree
  variable w_tabset
  variable OnSelectItem
  variable OnBusyCmd ""

  # this should be used insted of a lot of scalar variables.
  # but, what can we do now? 

  variable widgets
  variable data

  array set data {
    pool,objects ""
  }

  array set options {
    -onselcmd ""
    -wleft    3
    -wright   7
  }
  array set options $args
  
  # initializes the command OnSelectItem to ""
  set OnSelectItem $options(-onselcmd)

  #variable selecting_set 0
  #variable closing 0

  #upvar \#0 ::TolTk::Images Images

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
  $ht_vars  bind Entry <Enter>  ""
  $ht_vars  bind Entry <Leave>  ""
  $ht_funcs bind Entry <Enter>  ""
  $ht_funcs bind Entry <Leave>  ""
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
  variable blt_tree
  variable ht_tree
  
  # creation of the ScrolledWindow of the left side
  set sw1 [ScrolledWindow $paneLeft.sw -ipad 0 -borderwidth 0 -scrollbar both ]
  # creation of the left tree
  set blt_tree [::blt::tree create tree$sw1.ht]
  set ht_tree [::blt::treeview $sw1.ht -tree $blt_tree -bg white \
           -hideroot no -activeicons "" \
           -selectbackground gray75 -selectforeground black \
           -selectcommand ::TolInspector::SelectObject \
           -highlightthickness 0 -font $toltk_options(fonts,Label)]
  # configuration of the left tree
  $ht_tree configure \
           -closecommand  "::TolInspector::CloseObject %#" \
           -opencommand   "::TolInspector::OpenObject %#"
  $ht_tree style configure text -bg white     
  $ht_tree column configure treeView -bg white
  $ht_tree column insert end FileName -hide true  
  # creation of the root node "Tol Objects"
  set img [::Bitmap::get Objects]
  $ht_tree entry configure root -label [mc "Tol Objects"]\
           -icons [list $img $img]

  # creation of the node father "Grammars"
  set img [::Bitmap::get Grammars]  
  set blt_idx [$blt_tree insert root -label "root-grammars"\
           -tags "root-grammars"]
  $ht_tree entry configure $blt_idx -label [mc "Grammars"]\
           -icons [list $img $img]
  # Inserts in the node of grammars a node by each grammar that exists in Tol
  InsertGrammars

  # creation of the node father "Packages"
  set img [::Bitmap::get "package"]  
  set blt_idx [$blt_tree insert root -label "root-packages"\
           -tags "root-packages"]
  $ht_tree entry configure $blt_idx -label [mc "Packages"]\
           -icons [list $img $img] -button yes

  # creation of the node father "Included Files"
  set img [::Bitmap::get "Files"]
  set blt_idx [$blt_tree insert root -label "root-files" -tags "root-files"]
  $ht_tree entry configure $blt_idx -label [mc "Included Files"] \
      -icons [list $img $img] -button yes
  # creation of the node father "Console Objects"  
  set img [::Bitmap::get "Console"]
  set blt_idx [$blt_tree insert root -label "root-console"\
          -tags "root-console"]
  $ht_tree entry configure $blt_idx -label [mc "Console Objects"] \
      -icons [list $img $img] -button yes
  # creation of the node father "Objects' Spool" 
  set img [::Bitmap::get "pool"]
  set blt_idx [$blt_tree insert root -label "root-pool" -tags "root-pool"]
  $ht_tree entry configure $blt_idx -label [mc "Objects' Spool"] \
      -icons [list $img $img] -button yes
  # associates the tree to scrolled window
  $sw1 setwidget $ht_tree
  # packing 
  pack $sw1 -fill both -expand yes
  # bind the focus on tree
  bind $ht_tree  <Button-1> "::focus $ht_tree"
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
  variable blt_vartree 
  variable ht_vars
  variable blt_functree
  variable ht_funcs
  
  # creates the tabset to display the variables & functions    
  #set w_tabset [::blt::tabset $paneRight.ts -relief flat\
            -bd 0 -outerpad 0 -tier 2 -slant none -textside top \
            -highlightthickness 0 -side left -rotate 90]
  set w_tabset [::blt::tabset $paneRight.ts -relief flat\
            -bd 0 -outerpad 0 -tier 2 -slant right -textside right \
            -highlightthickness 0]
  # creates the scrolled windows for each one of the tabs
  set sw21 [ScrolledWindow $w_tabset.sw1]
  set sw22 [ScrolledWindow $w_tabset.sw2]
  # inserts in the tabset the tabs of functions and variables
  $w_tabset insert end \
      Variables -text [mc Variables] -selectbackground \#d9d9d9 -bg gray75 \
      -image [::Bitmap::get Variables] -pady 0\
      Functions -text [mc Functions] -selectbackground \#d9d9d9 -bg gray75 \
      -image [::Bitmap::get Functions] -pady 0

  # Creation of the right TREE RIGHT with VARIABLES
  set blt_vartree [::blt::tree create tree$sw21.vars]
  set ht_vars [::blt::treeview $sw21.vars -tree $blt_vartree -flat no\
           -bg white -hideroot yes -linewidth 0 -activeicons {}\
           -selectmode multiple\
           -selectbackground gray75 -selectforeground black \
           -highlightthickness 0 -font $toltk_options(fonts,Label)]
  #tv_ordered_selection tvsel -tv $ht_vars
  # configuration of the tree of variables
  $ht_vars configure \
    -opencommand   "::TolInspector::OpenVariable %#" \
    -selectcommand "::TolInspector::SelectItem $ht_vars"
  $ht_vars style configure text -bg white
  bind $ht_vars  <Button-1> "::focus $ht_vars"
    
  # Creation of the right TREE RIGHT with FUNCTIONS
  set blt_functree [::blt::tree create tree$sw22.funcs]
  set ht_funcs [::blt::treeview $sw22.funcs -tree $blt_functree -flat no\
          -bg white -hideroot yes -linewidth 0 -activeicons {} \
          -selectmode multiple\
          -selectbackground gray75 -selectforeground black \
          -highlightthickness 0 -font $toltk_options(fonts,Label)]
  # configuration of the tree of functions
  $ht_funcs configure  -selectcommand "::TolInspector::SelectItem $ht_funcs"
  $ht_funcs style configure text -bg white
  bind $ht_funcs <Button-1> "::focus $ht_funcs"
  
  # associates the trees to scrolled windows
  $sw21 setwidget $ht_vars
  $sw22 setwidget $ht_funcs
  # configure the tabs
  $w_tabset tab configure Variables -window $sw21 -fill both -padx 0 
  $w_tabset tab configure Functions -window $sw22 -fill both -padx 0
  # create the columns of the trees of variables and functions
  CreateColumns $ht_vars
  CreateColumns $ht_funcs
  # packing the tabset
  pack $w_tabset -fill both -expand yes
  $w_tabset tab configure Variables\
         -command "::TolInspector::SelectItem $ht_vars"
  $w_tabset tab configure Functions\
         -command "::TolInspector::SelectItem $ht_funcs"
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::CreateMenus {} {
#
# PURPOSE: Create the menus of the trees
#
#/////////////////////////////////////////////////////////////////////////////
  variable data_menu
  variable ht_vars
  variable ht_funcs
  variable ht_tree

  set data_menu(main)          [menu $ht_vars.m -tearoff 0]
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
  
  # composes the  menus for each one of the trees, depending the node
  # that is selected
  $ht_vars  bind all <Button-3> +[list ::TolInspector::PostVariable %X %Y]
  $ht_funcs bind all <Button-3> +[list ::TolInspector::PostFunction %X %Y]
  $ht_tree  bind all <Button-3> +[list ::TolInspector::PostTree     %X %Y]
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
  variable blt_tree
  variable blt_vartree
  variable blt_functree
  variable item_data

  ::blt::tree destroy $blt_tree
  ::blt::tree destroy $blt_vartree
  ::blt::tree destroy $blt_functree
  unset item_data
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

#/////////////////////////////////////////////////////////////////////////////
  proc ::TolInspector::_InsertChild { args } {
#
# PURPOSE: este proc es para ancapsular desc en una lista
#
#/////////////////////////////////////////////////////////////////////////////
  eval InsertChild [lrange $args 0 3] [list [list [lindex $args 4]]] [lrange $args 5 end]
}

#/////////////////////////////////////////////////////////////////////////////
  proc ::TolInspector::InsertChild { args } {
#
# PURPOSE: Inserts a node in the corresponding treeview of the right panel.
#
# PARAMETERS:
#   args[0] -> grammar
#   args[1] -> name
#   args[2] -> content
#   args[3] -> path
#   args[4] -> desc
#   Si grammar == "Code"
#    args[5] -> ¿es una variable?
#   Si grammar == "Set" || "NameBlock"
#    args[5] -> indexes of the set
#    args[6] -> is file?
#    args[7] -> has subset?
#    args[8] -> subtype
#    args[end] -> "Reference {reference}" end > 4
#
#/////////////////////////////////////////////////////////////////////////////
  variable ht_vars
  variable blt_vartree
  variable ht_funcs
  variable blt_functree
  variable gra_parent

  #puts "InsertChild: args=$args [info level 1]"
  set grammar [lindex $args 0]
  set name [ lindex $args 1 ]
  set content [lindex $args 2]
  set path    [lindex $args 3]
  set desc    [lindex [lindex $args 4] 0]


  if {[llength [lindex $args end]] >= 2} {
    #set ref [list [lindex [lindex $args end] 0] [lrange [lindex $args end] 1 end]]
    set ref [concat [lindex [lindex $args end] 0] [lrange [lindex $args end] 1 end]]
  } else {
    set ref ""      
  }
  #puts "InsertChild: ref=$ref"
  switch $grammar {
    Code {
      set isvar [lindex $args 5]
      if { [string equal $isvar "1"] } {
        Insert_HTItem $ht_vars $blt_vartree $grammar $name $content $path $desc $ref
      } else {
        Insert_HTItem $ht_funcs $blt_functree $grammar $name $content $path $desc $ref
      }
    }
    NameBlock -
    Set {
      # args[5] is the index of the set object
      # args[6] is the flag file
      # args[8] is the subtype
      Insert_HTItem $ht_vars $blt_vartree $grammar $name $content $path $desc \
        [lindex $args 5] [lindex $args 6] [lindex $args 8] $ref
    }
    default {
      Insert_HTItem $ht_vars $blt_vartree $grammar $name $content $path $desc $ref
    }
  }
}

# si grammar == "Set"
#   args[0] -> indexes
#   args[1] -> ¿is a file?
#   args[2] -> subtype

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::Insert_HTItem {ht tree grammar name content path desc args} {
#/////////////////////////////////////////////////////////////////////////////
  variable ht_tree
  variable tolset
  variable tolindex
  variable iterSet
  variable knownReference
  variable node_prefix
  variable item_data
  variable item_id
  variable gra_parent

  #upvar \#0 ::TolTk::Images Images

  if { $knownReference eq "" } {
    set objRef $iterSet
    lappend objRef $item_id
  } else {
    puts "knownReference = $knownReference"
    set objRef $knownReference
  }

  if { $gra_parent eq "NameBlock" } {
    array set info_member [ FilterNameBlockMember $name ]
    if { !$info_member(-show) } {
      incr item_id
      return
    }
    set fcolor $info_member(-fcolor)
  } else {
    set fcolor "black"
  }

  set brief_cont [TrimContent $grammar $content]
  set brief_desc [TrimContent DONTCARE $desc]
  #puts "Insert_HTItem: args=$args"
  if {[llength [lindex $args end]] >= 2} {
    set ref_value [lindex [lindex $args end] 1]
  } elseif {[regexp -- "console" $node_prefix]} {
    set ref_value [list Console]
	eval lappend ref_value $tolindex $item_id
  } else {
    set ref_value ""
  }

  #puts "Insert_HTItem: ref_value=$ref_value tolindex=$tolindex item_id=$item_id node_prefix=$node_prefix"
  #puts "\ttolset=$tolset"
  set data [list Index      $item_id \
                Content     $brief_cont \
                Path        $path \
                Description $brief_desc \
                Reference   $ref_value]

  if {$grammar eq "Set" || $grammar eq "NameBlock"} {
    if {[string length $tolset] || [regexp "console" $node_prefix]} {
      # OJO: si tolset == "" no implica que no estemos en un Set
      # por ejemplo un Set de la consola.
      set item [GetItemName $node_prefix [lindex $args 0]]
      #Tolcon_Trace "InsertHTItem. item(if): $item"
    } else {
      switch [lindex [$ht_tree get -full anchor] end] {
        root-files {
          set item file$item_id
        }
        root-console {
          set item console$item_id
        }
        root-pool {
          set item pool$item_id
        }
        default {
          set item item$item_id
        }
      }
    }
    # aqui hay que pedir el bitmap en funcion del subtype: args[2]
    if {$grammar eq "Set"} {
      set icon_grammar [SubTypeImage [lindex $args 2]]
    } else {
      #(pgea) se reemplaza el valor de la referencia en data
      set data [lreplace $data 9 9 $objRef]  
      #(pgea) se utiliza la informacon de instancia
      set classOf [Tol_ClassOfFromReference $objRef]
      set insCont [Tol_InstanceContentFromReference $objRef]
      if {$insCont ne ""} {
        set data [lreplace $data 3 3 [TrimContent $grammar $insCont]]
      } elseif {$classOf ne ""} {
        set data [lreplace $data 3 3 $classOf]
      }
      set objAddress [ tol::info address $objRef ]
      set icon_grammar [ ::ImageManager::getIconForInstance $objAddress ]
    }
  } elseif { $grammar eq "Anything" } {
    set item "item$item_id"
    set anyType [ tol::info anything $objRef ]
    set icon_grammar [ ::Bitmap::get $anyType ]
  } else {
    set item "item$item_id"
    set icon_grammar [ ::Bitmap::get $grammar ]
  }
  set idnew [$tree insert 0 -label $item]
  set label [OneLine $name]
  if {$label eq ""} {
    if {[set label $brief_desc] eq ""} {
      set label " "
    }
  }
  $ht entry configure $idnew -label $label \
      -icons [list $icon_grammar $icon_grammar] -data $data \
       -foreground $fcolor
  set item_data($item_id) [list $content $desc $grammar]
  incr item_id
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::ClearHiertables { } {
#
# PURPOSE:
#
#/////////////////////////////////////////////////////////////////////////////
  variable w_tabset
  variable ht_vars
  variable ht_funcs
  variable item_data
  variable OnSelectItem

  $ht_vars  entry delete root 0 end
  $ht_funcs entry delete root 0 end
  $ht_vars column  configure Index -hide no
  $ht_funcs column configure Index -hide no
  catch {
    unset item_data
  }
  $w_tabset tab configure Variables -text [mc Variables] -state normal
  $w_tabset tab configure Functions -state normal

  if { [string length $OnSelectItem] } {
    $OnSelectItem
  }
}

# este proc debe pasar a ser un getContainerReference
#
proc ::TolInspector::getPackageReference { treePath } {
  variable ht_tree

  #puts "::TolInspector::getPackageReference $treePath"
  catch {
    set typeContainer "NameBlock"
    set order [ lindex  [ split [ lindex $treePath 2 ] "-" ] end ]
    incr order -1
    #puts "path,2 = [ lindex $treePath 2 ]"
    #puts "order = $order"
    set idx [lindex [ $ht_tree entry children "root-packages"] $order]
    set nameContainer [ $ht_tree entry cget $idx -label ]
    #puts "en root-package:"
    #puts "\torder=$order"
    #puts "\tidx=$idx"
    #puts "\tnameContainer=$nameContainer"
    set container [ list $typeContainer $nameContainer ]
    if { [ llength $treePath ] > 3 } {
      foreach c [ lrange $treePath 3 end-1 ] {
        set orderItem [ lindex  [ split $c "-" ] end ]
        lappend container $orderItem
      }
      set tolindex [ lindex  [ split [ lindex $treePath end ] "-" ] end ]
    } else {
      set tolindex [ list ]
    }
  } msg
  #puts "::TolInspector::getPackageReference: $msg"
  #puts "::TolInspector::getPackageReference: [ list $container $tolindex ]"
  return [ list $container $tolindex ]
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::OpenObject { index } {
#
# PURPOSE: Script called each time a node in ht_tree (left treeview) is
#          opened.
#
# PARAMETERS:
#   index : The id of the node opened
#
#/////////////////////////////////////////////////////////////////////////////
  variable tolset
  variable tolindex
  variable iterSet
  variable at_set
  variable node_prefix
  variable ht_tree
  variable data

  #puts "OpenObject: index=$index"
  set path [$ht_tree get -full $index] 
  #puts "path = $path"
  if { $index } {
    set parent1 [lindex $path 1]
    if { [set ispackage [string equal $parent1 "root-packages"]] ||
         [set isfile [string equal $parent1 "root-files"]] ||
         [set isconsole [string equal $parent1 "root-console"]] ||
         [string equal $parent1 "root-pool"] } {
      set lpath [llength $path]
      #puts "lpath = $lpath"
      if { $lpath == 2 } {
        if { $ispackage } {
          InsertPackages
        } elseif { $isfile } {
          InsertFiles
        } elseif { $isconsole } {
          InsertConsoleObj
        } else {
          InsertPoolObj
        }
      } else {
        #puts "OpenObject: path = $path"
        set leaf [lindex $path end]
        set leaf_info [split $leaf "-"]
        set node_prefix [lindex $leaf_info 0]
        set _tolset $tolset
        if { $parent1 eq "root-packages" } {
          foreach {tolset tolindex} [ getPackageReference $path ] break
        } elseif { [string equal $parent1 "root-pool"] } {
          #Tolcon_Trace "OpenObject. tolset=$tolset"
          set tolset $data(pool,reference,[lindex $leaf_info 0])
          #puts "OpenObject: Spool: tolset=$tolset, leaf_info=$leaf_info"
          if { $lpath == 3 } {
            set tolindex [lrange $tolset 1 end]
            set tolset [lindex $tolset 0]
          } else {
            set tolindex [lrange $leaf_info 1 end]
		    if {[lindex $tolset 0] eq "Console"} {
		      set tolset [lindex $tolset 0]  
		    } else {
		      set tolset [lrange $tolset 0 1]
		    }
            #set tolindex [lrange $tolset 1 end]
            #set tolset [lindex $tolset 0]
            #lappend tolindex [lindex $leaf_info end]
          }
          #Tolcon_Trace "OpenObject. tolset=$tolset"
        } elseif {[regexp -- "console(.+)" $node_prefix --> idx_con]} {
          set tolset "Console"
		  #puts "OpenObject: Console: leaf_info = $leaf_info"
          if {[llength $leaf_info] == 1} {
            set tolindex $idx_con
          } else {
            #set tolindex [list $idx_con [lrange $leaf_info 1 end]]
            set tolindex [lrange $leaf_info 1 end]
          }
        } else {
          set idx [$ht_tree find -name $node_prefix]
          array set aryData [$ht_tree entry cget $idx -data]
          set tolset [list File $aryData(FileName)]
          #set tolset [$ht_tree entry cget [$ht_tree find -name $node_prefix] -label]
          if { $lpath == 3 } {
            set tolindex ""
          } else {
            set tolindex [lrange $leaf_info 1 end]
          }
        }
        Busy
        set at_set $index
        set iterSet [eval list $tolset $tolindex]
        variable gra_parent [ GetParentGrammar $iterSet ]
        ::tol::forallchild $iterSet ::TolInspector::InsertSubset
        set tolset $_tolset
        NotBusy
      }
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::InsertPackages {} {
#
# PURPOSE: Inserts a node in ht_tree (left ::blt::treeview) for each TOL
#          packages
#
#/////////////////////////////////////////////////////////////////////////////
  variable ht_tree
  #upvar \#0 ::TolTk::Images Images
    
  set order 1
  foreach pkg [::tol::info packages] {
    # takes an icon with the same name that the grammar
    set img [ ::Bitmap::get NameBlock ]
    $ht_tree insert -at "root-packages" end package-$order -label $pkg\
          -icons [list $img $img] -button yes
    incr order
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::InsertGrammars {} {
#
# PURPOSE: Inserts a node in ht_tree (left ::blt::treeview) for each TOL
#          grammar
#
#/////////////////////////////////////////////////////////////////////////////
  variable ht_tree
  #upvar \#0 ::TolTk::Images Images
    
  set id 1
  foreach g [::tol::info grammars] {
    # takes an icon with the same name that the grammar
    set img [::Bitmap::get $g]
    $ht_tree insert -at "root-grammars" end grammar-$id -label $g\
          -icons [list $img $img]
    incr id
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::InsertFiles { } {
#
# PURPOSE: Inserts a node in ht_tree (left treeview) for each Included file
#
#/////////////////////////////////////////////////////////////////////////////
  variable ht_tree
  variable blt_tree

  set index 1
  #set img [::Bitmap::get "File"]
  foreach f [lreverse [::tol::info included]] {
    set finfo   [::tol::info included $f]
    set button  [lindex $finfo 3]
    set subtype [lindex $finfo 4]
    # tomar la imagen en funcion del subtype
    set img [SubTypeImage $subtype]
    set tag_file "file$index"
    set blt_idx [$blt_tree insert "root-files" \
             -label $tag_file -tags $tag_file]
    # Tail del fichero. Podría hacerse configurable por el usuario
    set label [file tail $f]
    $ht_tree entry configure $blt_idx -label $label\
    -icons  [list $img $img] -button $button -data [list "FileName" $f]
    incr index
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::InsertConsoleObj { } {
#
# PURPOSE: Inserts a node in ht_tree (left treeview) for each Console object
#
#/////////////////////////////////////////////////////////////////////////////
  variable ht_tree
  variable has_button 0
    
  set index 1
  foreach co [::tol::console stack list] {
    set grammar [lindex $co 0]
    if {$grammar eq "Set" || $grammar eq "NameBlock"} {
      # SUBTYPE: icono en funcion de [lindex $co 7]
      if {$grammar eq "Set"} {
        set icon [SubTypeImage [lindex $co 7]]
      } else  {
        set objRef [ list "Console" $index ]
        set objAddress [ tol::info address $objRef ]
        set icon [ ::ImageManager::getIconForInstance $objAddress ]
      }
      set name [lindex $co 1]
      #tol::forallchild $name HasSubset
      set has_button [lindex $co 6]
      if {$name eq ""} {
        if {[set name [TrimContent DONTCARE [lindex $co 4]]] eq ""} {
          set name " "
        }
      }
      $ht_tree insert -at root-console end "console$index" -label $name \
          -icons  [list $icon $icon] -button $has_button -data [list "FileName" $index]
    }
    incr index
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::InsertPoolObj { } {
#
# PURPOSE: Inserts a node in ht_tree (left treeview) for each object in Spool
#
#/////////////////////////////////////////////////////////////////////////////
  variable data
  variable ht_tree
  array unset data pool,reference,*
  set index 1

  foreach _obj $data(pool,objects) {
    #Tolcon_Trace "InsertPoolObj. _obj=$_obj"
    array set obj $_obj
    if { [string equal $obj(grammar) "Set"] || [string equal $obj(grammar) "NameBlock"] } {
      #Si la referencia del objeto tiene espacios en la ruta, el 
      # llength me daría > 1, por eso añado la 2ª parte de la condicion
      if { ([llength $obj(reference)] > 1) && \
            ![string equal $obj(reference) $obj(objName)]} {
        set infoObj [::tol::info variable $obj(reference)]
      } else {
        if {[catch { set infoObj [concat [list "Set" $obj(reference)] \
                            [::tol::info included $obj(reference)]] }]} {
          set infoObj [concat [list "Set" $obj(reference)] \
                 [::tol::info variable "Set" $obj(reference)]]
        } else {
          # Insertamos el 1 de isFile
          set infoObj [linsert $infoObj 5 1]
        }
      }
      #Tolcon_Trace "infoObj=$infoObj"
      if {[set name [file tail [lindex $infoObj 1]]] eq ""} {
        if {[set name [TrimContent DONTCARE [lindex $infoObj 4]]] eq ""} {
          set name " "
        }
      }
      set has_button [lindex $infoObj 6]
      #set icon [SubTypeImage [lindex $infoObj 7]]
      if {$obj(grammar) eq "Set"} {
        set icon [SubTypeImage [lindex $infoObj 7]]
      } else  {
        puts "en pool = $obj(reference)"
        set objAddress [ tol::info address $objRef ]
        set icon [ ::ImageManager::getIconForInstance $objAddress ]
      }

      $ht_tree insert -at root-pool end "pool$index" -label $name \
          -icon [list $icon $icon] -button $has_button
      #Tolcon_Trace "InsertPoolObj. name=$name"
      if { ([llength $obj(reference)] > 1) && \
            [string equal $obj(reference) $obj(objName)]} {
        set obj(reference) [list $obj(reference)]
      }
      set data(pool,reference,pool$index) $obj(reference)
    }
    incr index
  }
}

proc ::TolInspector::FilterNameBlockMember { name } {
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

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::InsertSubset { args } {
#
# PURPOSE: Inserts a node in ht_tree (left treeview) for each child of a Set
#
# PARAMETERS:
#   args[0] -> grammar
#   args[1] -> name
#   args[2] -> content
#   args[3] -> path
#   args[4] -> desc
#   Si grammar == "Set"
#    args[5] -> indexes of the set
#    args[6] -> is file?
#    args[7] -> has subset?
#    args[8] -> subtype
#
#/////////////////////////////////////////////////////////////////////////////
  variable ht_tree
  variable at_set
  variable tolset
  variable iterSet
  variable node_prefix
  variable has_button 0
  variable blt_tree
  variable gra_parent
  catch {
  #puts "InsertSubset, args=$args"
  set grammar [lindex $args 0]
  if {$grammar eq "Set" || $grammar eq "NameBlock"} {
    #puts "InsertSubset, args=$args"
    if { $gra_parent eq "NameBlock" } {
      array set info_member [ FilterNameBlockMember [ lindex $args 1 ] ]
      if { !$info_member(-show) } {
        return
      }
      set name $info_member(-name)
      set fcolor $info_member(-fcolor)
    } else {
      set name [ lindex $args 1 ]
      set fcolor "black"
    } 
    set content [lindex $args 2]
    # idx es el indice completo desde el Set root
    set idx     [lindex $args 5]
    set isfile  [lindex $args 6]
    set has_button [lindex $args 7]
    set subtype [lindex $args 8]
    #tol::forallchild $tolset $idx HasSubset
    if {$grammar eq "Set"} {
      set icon [SubTypeImage $subtype]
    } else {
      set objRef $iterSet
      lappend objRef [ lindex $idx end ]
      set objAddress [ tol::info address $objRef ]
      set icon [ ::ImageManager::getIconForInstance $objAddress ]
    }
    # Tail por si es un fichero. Podría hacerse configurable por el usuario
    set label [file tail $name]
    #puts [list InsertSubset, idx {lindex $args 5} = $idx]
    set idnew [ $blt_tree insert $at_set \
                   -label [ GetItemName $node_prefix $idx ] ]

    #puts "InsertSubset, node_prefix = $node_prefix"
    #puts "InsertSubset, idnew = $idnew"
    #puts "InsertSubset, -label = [GetItemName $node_prefix $idx]"

    if {$label eq ""} {
      if {[set label [TrimContent DONTCARE [lindex $args 4]]] eq ""} {
        set label " "
      }
    }
    $ht_tree entry configure $idnew -label $label -button $has_button \
      -icons [list $icon $icon] -foreground $fcolor 
  }
} msg
#puts "InsertSubSet: $msg"
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::ClearConsoleObj { } {
#///////////////////////////////////////////////////////////////////////////// 
  variable ht_tree
  variable data
    
  set idx_con [$ht_tree index root-console]
  $ht_tree entry delete $idx_con 0 end
    
  #Quitamos del spool todos los objetos que provengan de la consola
  foreach _obj $data(pool,objects) {
    array set obj $_obj
    if {[string equal $obj(origin) "root-console"]} {
      set idx [lsearch $data(pool,objects) $_obj]
      set data(pool,objects) [lreplace $data(pool,objects) $idx $idx]
    }
  }
  UpdatePoolObj
  #Finalmente limpiamos el stack de la consola de tol
  ::tol::console stack release
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::ClearConsoleObjSel {ht} {
#///////////////////////////////////////////////////////////////////////////// 
  variable vars_selected
  variable data

  foreach var $vars_selected {
    set name [$ht entry cget $var -label]
    #puts "name=$name"
    #Quitamos del spool todos los objetos que provengan de la consola
    foreach _obj $data(pool,objects) {
      array set obj $_obj
      #puts "_obj=$_obj"
      if {[string equal $obj(origin) "root-console"] && [string equal $obj(objName) $name]} {
        set idx [lsearch $data(pool,objects) $_obj]
        set data(pool,objects) [lreplace $data(pool,objects) $idx $idx]
      }
    }
    UpdatePoolObj
    #Finalmente quitamos el objeto de la consola de tol
    ::tol::console stack release $name
  }
  UpdateConsoleObj
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::UpdateFileRoot { } {
#/////////////////////////////////////////////////////////////////////////////  
  variable ht_tree
  set open 0
  set idx_file [$ht_tree index root-files]
  if { [$ht_tree entry isopen $idx_file] } {
    $ht_tree close $idx_file
    set open 1
  }
  if { [$ht_tree index focus] == $idx_file } {
    SelectObject
  }
  if $open {
    $ht_tree open $idx_file
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::UpdateConsoleObj { } {
#/////////////////////////////////////////////////////////////////////////////  
  variable ht_tree

  set idx_con [$ht_tree index root-console]
  if { [$ht_tree entry isopen $idx_con] } {
    $ht_tree close $idx_con
  }
  $ht_tree selection anchor root-console
  $ht_tree selection mark root-console
  #Tolcon_Trace "Abierto el nodo root-console"
  if { [$ht_tree index focus] == $idx_con } {
    SelectObject
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::UpdatePoolObj { } {
#/////////////////////////////////////////////////////////////////////////////  
  variable ht_tree

  set idx_pool [$ht_tree index root-pool]
  if { [$ht_tree entry isopen $idx_pool] } {
    $ht_tree close $idx_pool
  }
  if { [$ht_tree index focus] == $idx_pool } {
    SelectObject
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::SelectObject { } {
#
# PURPOSE: Script called each time a node in ht_tree (left treeview) is
#          selected.
#
#/////////////////////////////////////////////////////////////////////////////
  #Tolcon_Trace "LLAMADA A SELECTOBJECT"
  variable w_tabset
  variable ht_tree
  #variable set_type ""
  variable tolset
  variable tolindex
  #variable closing
  variable node_prefix ""
  variable data

  set index [$ht_tree index anchor]
    
  if { ![string length $index] } {
    ClearHiertables
    return
  }
  ClearHiertables

  set path [ $ht_tree get -full $index ] 
  set tolset ""
  if { $index } {
    #puts "SelectObject index = $index"
    #puts "SelectObject: full = $path"
    set info_node [split [lindex $path end] "-"] ;# ojo si el path tiene -
    set node_type [lindex $info_node 0]
    set node_name [lindex $info_node 1]
    #puts "SelectObject: info_node = $info_node"
  } else {
    set node_type root
    set node_name root
  }

  Busy
  #puts "SelectObject: node_type = $node_type, node_name = $node_name"
  switch -regexp $node_type  {
    root {
      switch $node_name {
        root -
        grammars {
          ClearHiertables
          #$w_tabset tab configure Variables -state disabled
          #$w_tabset tab configure Functions -state disabled
        }
        package {
          puts "implementa el llenado del panel derecho"
        }
        files {
          SelectFileRoot
        }
        console {
          SelectConsoleRoot
        }
        pool {
          SelectPoolRoot
        }
      }
    }
    grammar {
      SelectGrammar
    }
    default {
      set node_prefix $node_type
	  #puts "SelectObject: node_prefix = $node_prefix"
      if { $node_type eq "package" } {
        foreach {tolset tolindex} [ getPackageReference $path ] break
      } elseif {[regexp -- "pool" $node_type]} {
        set tolset $data(pool,reference,[lindex $node_type 0])
        #puts "SelectObject: Spool: tolset=$tolset, info_node=$info_node"
        if { [llength $info_node] == 1 } {
          set tolindex [lrange $tolset 1 end]
          set tolset [lindex $tolset 0]
        } else {
          #set tolindex [lrange $tolset 1 end]
          #set tolset [lindex $tolset 0]
          #lappend tolindex [lindex $info_node end]
          set tolindex [lrange $info_node 1 end]
          if {[lindex $tolset 0] eq "Console"} {
            set tolset [lindex $tolset 0]  
          } else {
            set tolset [lrange $tolset 0 1]
          }
        }
      } elseif {[regexp -- "console(.+)" $node_type --> idx_con]} {
        #puts "SelectObject: Console: tolset=$tolset info_node=$info_node idx_con=$idx_con"
        set tolset "Console"
        #set idx [$ht_tree find -name $node_type]
        #array set aryData [$ht_tree entry cget $idx -data]
        #set tolindex [eval list $aryData(FileName) [lrange $info_node 1 end]]
        if {[llength $info_node] == 1} {
          set tolindex $idx_con
        } else {
          set tolindex [lrange $info_node 1 end]
          #set tolindex [concat [list $idx_con] $tolindex]
        }
      } else {
        set idx [$ht_tree find -name $node_type]
        array set aryData [$ht_tree entry cget $idx -data]
		#puts "SelectObject: FileName=$aryData(FileName) info_node=$info_node"
        set tolset [list File $aryData(FileName)]
        set tolindex [lrange $info_node 1 end]
        #set tolindex [lrange $info_node 2 end]
      }
	  #puts "SelectObject end: tolset=$tolset, tolindex=$tolindex"
      SelectSet
    }
  }
  NotBusy
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::SelectFileRoot { } {
#
# PURPOSE: Script called each time 'Included Files' node in ht_tree is
#          selected. Gets information about each node that must be in
#          included files, and inserts it in the right treeview.
#
#/////////////////////////////////////////////////////////////////////////////
  variable item_id 1
  variable w_tabset
    
  ClearHiertables
  $w_tabset tab configure Variables -text [mc Files]
  #  $w_tabset tab configure Functions -state disabled
  $w_tabset select 0
  foreach f [lreverse [::tol::info included]] {
    set fi [::tol::info included $f]
    # tener en cuenta el subtype cuando lo retorne
    InsertChild Set $f \
        [lindex $fi 0] [lindex $fi 1] [list [lindex $fi 2]] $item_id 1 [lindex $fi 3] [lindex $fi 4]
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::SelectConsoleRoot { } {
#
# PURPOSE: Script called each time 'Console Objects' node in ht_tree is
#          selected. Gets information about each node that must be in console,
#          and inserts it in the right treeview.
#
#/////////////////////////////////////////////////////////////////////////////
  variable w_tabset
  variable ht_vars
  variable ht_funcs
  variable item_id 1
  variable iterSet "Console"
  variable knownReference ""

  ClearHiertables
#(pgea) se quita la linea que oculta la columna del indice y que
#(pgea) el procedimimento anterior ClearHiertables muestra
#  $ht_vars column  configure Index -hide yes
#  $w_tabset tab configure Functions -state disabled
  
  foreach co [::tol::console stack list] {
   #puts "SelectConsoleRoot: co = $co"
   set gra  [lindex $co 0]
   set name [lindex $co 1]
   set cont [lindex $co 2]
   set path [lindex $co 3]
   set desc [lindex $co 4]
   set knownReference [ list Console $item_id ]
    switch [lindex $co 0 ] {
      Code {
        InsertChild $gra $name $cont $path [list $desc] 0
      }
      NameBlock -
      Set {
        #puts "switch: SelectConsoleRoot $gra"
        InsertChild $gra $name $cont $path [list $desc] \
            $item_id [lindex $co 5] [lindex $co 6] [lindex $co 7] \
            [list Reference [list "Console" $item_id]]
      }
      default {
        InsertChild $gra $name $cont $path [list $desc] \
            [list Reference [list "Console" $item_id]]
      }
    }
  }
  set knownReference ""
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::SelectPoolRoot { } {
#
# PURPOSE: Script called each time 'Spool Objects' node in ht_tree is
#          selected. Gets information about each node that must be in pool,
#          and inserts it in the right treeview.
# 
#/////////////////////////////////////////////////////////////////////////////
  variable w_tabset
  variable ht_vars
  variable data
  variable item_id 1
  variable knownReference ""

  ClearHiertables
  $ht_vars column  configure Index -hide yes

  array unset data pool,variables,*
  # Para cada uno de los objetos que debe haber en el spool
  # busco la información del nodo y lo inserto en el treeview de la derecha
  foreach _obj $data(pool,objects) {
    array set obj $_obj
    #Si la referencia del objeto tiene espacios en la ruta, el
    # llength me daría > 1, por eso añado la 2ª parte de la condicion
    #puts "SelectPoolRoot: obj(grammar)=$obj(grammar) obj(objName)=$obj(objName) obj(reference)=$obj(reference)"
    if { ([llength $obj(reference)] > 1) && ![string equal $obj(reference) $obj(objName)]} {
      if {$obj(grammar) eq "Code"} {
        set infoObj [concat [list $obj(grammar) $obj(objName)] \
            [::tol::info function $obj(reference)]]
      } else  {
        set infoObj [::tol::info variable $obj(reference)]
      }
      #Tolcon_Trace "tol info (var|function) reference"
    } else {
      if {[catch { set infoObj [concat [list "Set" $obj(reference)] \
                          [::tol::info included $obj(reference)]] }]} {
        if {$obj(grammar) eq "Code"} {
          set infoObj [concat [list $obj(grammar) $obj(reference)] \
               [::tol::info function $obj(graApply) $obj(reference)]]
        } else  {
          #set infoObj [concat [list $obj(grammar) $obj(reference)] \
          #     [::tol::info variable $obj(grammar) $obj(reference)]]
          set infoObj [::tol::info variable [list $obj(grammar) $obj(reference)]]
        }
        #Tolcon_Trace "tol info (var|function) Grammar nameGlobal"
      } else {
        # Insertamos el 1 de isFile
        set infoObj [linsert $infoObj 5 1]
        #Tolcon_Trace "tol info included"
      }
    }
    #Tolcon_Trace "INFOOBJ: $infoObj"
    set gra  [lindex $infoObj 0]
    set name [lindex $infoObj 1]
    set cont [lindex $infoObj 2]
    set path [lindex $infoObj 3]
    set desc [lindex $infoObj 4]
    set data(pool,variables,$item_id) $obj(reference)

    switch $gra {
      Code {
        InsertChild $gra $name $cont $path [list $desc] 0 "Reference $obj(reference)"
      }
      Set -
      NameBlock {
        set knownReference $obj(reference)
        InsertChild $gra $name $cont $path [list $desc] \
            $item_id [lindex $infoObj 5] [lindex $infoObj 6] [lindex $infoObj 7] "Reference $obj(reference)"
        set knownReference ""
      }
      default {
        InsertChild $gra $name $cont $path [list $desc] "Reference $obj(reference)"
      }
    }
  }  
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::SelectGrammar { } {
#
# PURPOSE: Script called each time a Grammar node in ht_tree is selected. Gets
#          information about each var and function of this grammar, and
#          inserts them in the right treeview.
#
#/////////////////////////////////////////////////////////////////////////////
  variable w_tabset
  variable ht_tree
  variable ht_vars
  variable ht_funcs
  variable item_id 1
  variable OnSelectItem
  variable knownReference

  set knownReference ""
  ClearHiertables
  $ht_vars column  configure Index -hide yes
  $ht_funcs column configure Index -hide yes
  set grammar [$ht_tree entry cget anchor -label]
  if { [string compare $grammar "Anything"] } {
    if { [string length $OnSelectItem] } {
      $OnSelectItem [::tol::info grammars $grammar]
    }
    foreach var  [::tol::info variables $grammar] {
      if {[catch {
        set vinfo [::tol::info variables [list $grammar $var]]
      } msg]} {
        Tol_HciWriter "<E>in ::TolInspector::SelectGrammar : $msg\n</E>"
        continue
      }
      if { [string equal $grammar "Code"] } {
        #Tolcon_Trace "Code : -- $vinfo --"
        InsertChild $grammar $var [lindex $vinfo 2] [lindex $vinfo 3] [list [lindex $vinfo 4]] 1  
      } elseif { $grammar eq "Set" || $grammar eq "NameBlock" } {
        #Tolcon_Trace "Set : -- $vinfo --"
        if { $grammar eq "NameBlock" } {
          set knownReference [ list "NameBlock" $var ]
        }
        InsertChild $grammar $var [lindex $vinfo 2] [lindex $vinfo 3] [list [lindex $vinfo 4]] \
          $item_id [lindex $vinfo 5] [lindex $vinfo 6] [lindex $vinfo 7]
      } else {
        InsertChild $grammar $var [lindex $vinfo 2] [lindex $vinfo 3] [list [lindex $vinfo 4]] $item_id 0
      }  
    }
  } else {
#    $w_tabset tab configure Variables -state disabled
#    $w_tabset select 1
  }
  foreach fun  [::tol::info functions $grammar] {
    set finfo [::tol::info functions $grammar $fun]
    InsertChild Code $fun [lindex $finfo 0] [lindex $finfo 1] [list [lindex $finfo 2]] 0    
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::SelectSet { } {
#
# PURPOSE: Script called each time a Set node in ht_tree is selected. Gets
#          information about each child of this set, and inserts it in the
#          right treeview.
#
#/////////////////////////////////////////////////////////////////////////////
  variable ht_tree
  variable ht_vars
  variable ht_funcs
  
  variable tolset
  variable tolindex
  variable iterSet
  variable var_id 1
  variable fun_id 1
  variable item_id
  #variable selecting_set
  
  #  if { $selecting_set } {
  #puts "eoooo ..."
  #    return
  #  }
  ClearHiertables
  #  set selecting_set 1
  set item_id 1
  
  #puts "SelectSet: tolset=$tolset tolindex=$tolindex"
  set iterSet [eval list $tolset $tolindex]
  variable gra_parent [ GetParentGrammar $iterSet ]
  ::tol::forallchild $iterSet ::TolInspector::_InsertChild
  #  set selecting_set 0
}

proc ::TolInspector::GetParentGrammar { set_ref } {
  set root [ lindex $set_ref 0 ]
  set L [ llength $set_ref ]
  if { ( $root eq "File" && $L <= 2 ) } {
    set root
  } else {
    lindex [ tol::info var $set_ref ] 0
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::CloseObject { index } {
#
# PURPOSE: Script called each time a node in ht_tree (left treeview) is
#          closed.
#
# PARAMETERS:
#   index : The id of the node closed
#
#/////////////////////////////////////////////////////////////////////////////
  variable ht_tree
  #variable closing 1
  set path [$ht_tree get -full $index]
  if { $index } {
    set parent1 [lindex $path 1]
    if { [string equal $parent1 "root-packages"] ||
         [string equal $parent1 "root-files"] ||
         [string equal $parent1 "root-console"] || 
         [string equal $parent1 "root-pool"] } {
      $ht_tree entry delete $index 0 end
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::HasSubset { args } {
#/////////////////////////////////////////////////////////////////////////////  
  if { [string equal [lindex $args 0] "Set"] } {
    variable has_button 1
    return 0
  }
  return 1
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::OpenVariable { index } {
#
# PURPOSE: ????
#
# PARAMETERS:
#   index -> 
#
#/////////////////////////////////////////////////////////////////////////////
  variable tolset     ;# Name of the current TOL set variable
  variable ht_vars    ;# ::blt::treeview of variables
  variable ht_tree    ;# ::blt::treeview left
  variable blt_tree   ;# ::blt::tree left
  variable item_data  ;# Array to hold the full content & description of items
 
  if { ![string length $index] } {
    return
  }
  set node_anchor [lindex [$ht_tree get -full anchor] end]
  
  set tree_index -1
  #puts "OpenVariable = $item_data([lindex [$ht_vars entry cget $index -data] end])"
  if { ![regexp -- "grammar-" $node_anchor] } {
    set grammar [lindex $item_data([lindex [$ht_vars entry cget $index -data] end]) 2]
    if {$grammar eq "Set" || $grammar eq "NameBlock"} {
      $ht_tree open anchor
      #puts "full = [$ht_vars get -full $index]"
      set local_name [lindex [$ht_vars get -full $index] end]
      set idx_anchor [$ht_tree index anchor]
      #puts "a buscar $local_name dentro de $idx_anchor"
      if {[set tree_index [$blt_tree findchild $idx_anchor $local_name]] == -1} {
        foreach idx [$ht_tree entry children anchor] {
          if { [string equal $local_name [lindex [$ht_tree get -full $idx] end]] } {
            set tree_index $idx
            break
          }
        }
      }
    }
  }
  if { $tree_index != -1 } {
    update
    blt::tv::SetSelectionAnchor $ht_tree $tree_index
    set blt::tv::scroll 1
    #$ht_tree selection clearall
    #$ht_tree see $tree_index
    #$ht_tree focus $tree_index
    #$ht_tree selection set $tree_index
    #$ht_tree selection anchor $tree_index
    #set blt::Hiertable::scroll 1
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::SelectItem { ht } {
#
# PURPOSE: Procedure that is executed when the user selects an element of
#          the trees of variables orfunctions.
#          First it take all the arguments necessary of the node of tree to
#          execute the procedure stored in the variable OnSelectItem, and
#          later executes it passing those arguments.
#
# PARAMETERS: path of ht_* (::blt::treeview of variables or of functions)
#
#/////////////////////////////////////////////////////////////////////////////
 # variable w_text       ;# TODO: ????
  variable OnSelectItem ;#Command to invoke on SelectItem in ht_*
  variable item_data    ;# Array to hold the full content & description of items
  
  set entry [$ht index anchor] ;# id of entry selected  
  if { [string length $OnSelectItem] } {
    # there are associated command
    if { [string length $entry] } {
      # almost one entry of ht_* selected
      array set aryData [$ht entry cget $entry -data]
      set name [$ht entry cget $entry -label] ;# name of item
      set itemid $aryData(Index)              ;# id of item
      set path   $aryData(Path)               ;# path of file of item
      #(pgea) se anyade la propiedad Reference
      set objRef $aryData(Reference)          ;# info of item
      set full_data $item_data($itemid) ;# {value} {desc} {grammar}       
      set value [lindex $full_data 0]   ;# value of item PE:
                                         # Anything Field(Set s, Text nombre)

      if { [string equal \"\n\" $value] } {
        # hasn't value        
        set value {"\n"}
      } else {
        set value [string trim $value]
      }
      set desc [string trim [lindex $full_data 1]]
      set grammar [lindex $full_data 2]
      set icon [lindex [$ht entry cget $entry -icons] 0]
      #(pgea) lista de argumentos que recibe ::TolConsole::OnInfo
      set args [list $icon $grammar $name $value $path $desc $objRef]
    } else {
      set args ""
    }
    eval "$OnSelectItem $args"
  }
  return
#;# TODO: ????
#  if { [string length $entry] } {
#    FillText $ht $entry
#  } else {
#    $w_text delete 1.0 end
#  }
}

#/////////////////////////////////////////////////////////////////////////////
#(pgea) se usa?
proc ::TolInspector::FillText { ht entry } {
#/////////////////////////////////////////////////////////////////////////////
  variable w_text
  variable item_data
  #upvar #0 ::TolTk::Images Images

  $w_text delete 1.0 end
  array set aryData [$ht entry cget $entry -data]
  set name [$ht entry cget $entry -label]
  set itemid $aryData(Index)
  set full_data $item_data($itemid)
  
  set value [lindex $full_data 0]
  if { [string equal \"\n\" $value] } {
    set value {"\n"}
  }
  set desc [string trim [lindex $full_data 1]]
  set grammar [lindex $full_data 2]
  set icon [lindex [$ht entry cget $entry -icons] 0]
  foreach {prop val} [list "Grammar: " $grammar "Name: " $name "Value: " [string trim $value]] {
    set idx_start [$w_text index "end -1 chars"]
    $w_text insert end $prop
    set idx_end   [$w_text index "end -1 chars"]
    $w_text tag add tagLabel $idx_start $idx_end
    if {[string equal $prop "Grammar: "]} {
      $w_text image create end -image $icon
      $w_text insert end "  "
    }
    $w_text insert end "$val\n"
    $w_text tag add tagValue $idx_end [$w_text index "end -1 chars"]
  }
  $w_text tag configure tagLabel -font fnBold
  $w_text tag configure tagValue -wrap none
  if { [string length $desc] } {
    set idx_start [$w_text index "end -1 chars"]
    $w_text insert end "Description\n"
    set idx_end [$w_text index "end -1 chars"]
    $w_text tag add tagPropLabel $idx_start $idx_end
    $w_text insert end $desc
    $w_text tag add tagPropValue $idx_end [$w_text index "end -1 chars"]
    $w_text tag configure tagPropLabel -font fnBold -underline 1
    $w_text tag configure tagPropValue -wrap word
  }
}

#proc ::TolInspector::GetIndexes { nodeid } {
#  variable ht_tree
#  # No se utiliza
#  # name is prefix-i1-i2-i3-...-in
#
#  if { $nodeid } {
#    lrange [split [$ht_tree get $nodeid] "-"] 1 end
#  } else list
#}

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
  variable ht_tree
  variable ht_vars

  $ht_tree configure -cursor watch
  $ht_vars configure -cursor watch
  if { [string length $OnBusyCmd] } {
    eval $OnBusyCmd 1
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::NotBusy { } {
#/////////////////////////////////////////////////////////////////////////////  
  variable OnBusyCmd
  variable ht_tree
  variable ht_vars

  $ht_tree configure -cursor ""
  $ht_vars configure -cursor ""

  if { [string length $OnBusyCmd] } {
    eval $OnBusyCmd 0
  }
}

proc Tol_GetViewPrivateMembers { } {
  set viewOpt 0
  catch {
    # no funciona 
    tol::console eval {
      Real __getViewPrivateMembers_tmp__ =
      TolConfigManager::Config::Various::View::PrivateMembers;
    }
    set x [ lindex [ tol::info variable { Real __getViewPrivateMembers_tmp__ } ] 2 ]
    tol::console stack release __getViewPrivateMembers_tmp__
    set viewOpt [ expr { int( $x ) } ]
    
  } msg
  #puts "Tol_GetViewPrivateMembers: $msg"
  return $viewOpt
}

proc Tol_ObjIsClassOf { obj_addr cls_name } {
  tol::console eval [ string map [ list %A $obj_addr %C $cls_name ] {
    Real __gui_check__ = IsInstanceOf( GetObjectFromAddress( "%A" ),"%C" )
  } ]
  set info [ tol::info variable {Real __gui_check__} ]
  set x [ lindex $info 2 ]
  tol::console stack release __gui_check__
  expr { round($x) }
}

proc Tol_ClassOf { obj_addr } {
  tol::console eval [ string map [ list %A $obj_addr ] {
    Text __gui_classof__ = {
      Anything obj = GetObjectFromAddress( "%A" );
      // WriteLn( "Tol_ClassOf " << Grammar( obj ) );
      Text If( Grammar( obj ) == "NameBlock", ClassOf( obj ), "" )
    }
  } ]
  set info [ tol::info variable {Text __gui_classof__} ]
  set x [ string trim [ lindex $info 2 ]  \" ]
  tol::console stack release __gui_classof__
  set x
}

proc Tol_ClassOfFromReference { objReference } {
  set addr [ ::tol::info address $objReference ]
  return [ Tol_ClassOf $addr ]
}

#(pgea) nueva funcion para obtener la informacion de instancia
proc Tol_InstanceInfo { obj_addr } {
  tol::console eval [ string map [ list %A $obj_addr ] {
    Text __gui_instanceinfo__ = {
      Anything __intance__ = GetObjectFromAddress("%A");
      Text If(ObjectExist("Code", "__intance__::_.get.autodoc.info"), 
        __intance__::_.get.autodoc.info(?), "")
    }
  } ]
  set info [ tol::info variable {Text __gui_instanceinfo__} ]
  set x [ string trim [ lindex $info 2 ]  \" ]
  tol::console stack release __gui_instanceinfo__
  set x
}

#(pgea) nueva funcion para obtener la informacion de instancia
proc Tol_InstanceInfoFromReference { objReference } {
  set addr [ ::tol::info address $objReference ]
  return [ Tol_InstanceInfo $addr ]
}

#(pgea) nueva funcion para obtener el contenido de la instancia
proc Tol_InstanceContent { obj_addr } {
  tol::console eval [ string map [ list %A $obj_addr ] {
    Text __gui_instancecontent__ = {
      Anything __intance__ = GetObjectFromAddress("%A");
      Text If(ObjectExist("Code", "__intance__::_.get.autodoc.content"), 
        __intance__::_.get.autodoc.content(?), "")
    }
  } ]
  set info [ tol::info variable {Text __gui_instancecontent__} ]
  set x [ string trim [ lindex $info 2 ]  \" ]
  tol::console stack release __gui_instancecontent__
  set x
}

#(pgea) nueva funcion para obtener el contenido de la instancia
proc Tol_InstanceContentFromReference { objReference } {
  set addr [ ::tol::info address $objReference ]
  return [ Tol_InstanceContent $addr ]
}

proc TolGui_GetMenuEntries { selection idx } {
  #puts "TolGui_GetMenuEntries $selection $idx"
  array set instances {}
  array set menu_class {}
  set i 0
  foreach obj_info $selection {
    set tcl_ref [ lindex $obj_info 0 ]
    set obj_addr [ ::tol::info address $tcl_ref ]
    if { [ Tol_ObjIsClassOf $obj_addr "GuiTools::@MenuDesc" ] } {
      set class [ Tol_ClassOf $obj_addr ]
      lappend instances($class) $obj_addr
      set name_inst [ lindex $obj_info 1 ]
      set menu_items [ TolGui_GetObjMenu $obj_addr ]
      # esto hace que estemos guardando la ultima instancia, asumimos
      # tambien que todas las instancia tienen la misma espcificacion
      # de menu
      if { ![ llength [ array names menu_class $class ] ] || $i == $idx } {
        set menu_class($class) \
            [ list $name_inst $obj_addr $menu_items ]
      }
    }
    incr i
  }
  set menu_info [ list ]
  foreach c [ array names instances ] {
    lappend menu_info [ list $c $menu_class($c) $instances($c) ]
  }
  #puts "$menu_info"
  set menu_info
}

proc TolGui_InsertEntriesFromMenuManager { targetMenu selection } {
  set addrList [ list ]
  foreach obj_info $selection {
    set tcl_ref [ lindex $obj_info 0 ]
    lappend addrList [ ::tol::info address $tcl_ref ]
  }
  ::MenuManager::insertEntriesForSelection $targetMenu $addrList
}

proc TolGui_GetObjMenu { obj_addr } {
  set try [ catch {
    tol::console eval [ string map [ list %A $obj_addr ] {
      GuiTools::@MenuDesc __aux_menu__ = GetObjectFromAddress("%A");
      Set __gui_menu_entries__ = __aux_menu__::getMenuEntries(?)
    } ] } msg ]
  if { $try } {
    puts "ERROR TolGui_GetObjMenu : $msg"
    set result ""
  } else {
    set result [ TolObj2TclObj {Set __gui_menu_entries__} ]
  }
  tol::console stack release __aux_menu__
  tol::console stack release __gui_menu_entries__
  set result
}

proc TolGui_InvokeMethod { method obj_addr } {
  #puts "TolGui_InvokeMethod $method $obj_addr"
  set try [ catch {
    tol::console eval [ string map [ list %A $obj_addr %M $method ] {
      NameBlock __aux_instance__ = GetObjectFromAddress("%A");
      Real __aux_result__ = __aux_instance__::%M(?)
    } ] } msg ]
  if { $try } {
    puts "ERROR TolGui_InvokeMethod : $msg"
  }
  tol::console stack release __aux_instance__
  tol::console stack release __aux_result__
}

proc TclList2SetOfText { lst } {
  set result [ list ]
  foreach i $lst {
    lappend result \"[string map {\" \\\"} $i]\"
  }
  return "SetOfText([join $result ,])"
}

proc TolGui_InvokeGroup { cname function group } {
  set SOA [ TclList2SetOfText $group ]
  set try [ catch {
    tol::console eval \
        [ string map [ list %C $cname %F $function %S $SOA ] {
          Real __aux_result__ = {
            Set aux = EvalSet( %S, NameBlock( Text addr ) {
              NameBlock GetObjectFromAddress( addr )
            } );
            Real %C::%F( aux )
          }
        } ] } msg ]
  if { $try } {
    puts "ERROR TolGui_InvokeGroup : $msg"
  }
  tol::console stack release __aux_result__
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::PostVariable { x y } {
#
# PURPOSE: Composes a popup menu for variables and displays it. Its composed
#          depending on the node/s clicked by user.
#
# PARAMETERS:
#   x -> x-coordinate where popup menu is drawn
#   y -> y-coordinate where popup menu is drawn
#
#/////////////////////////////////////////////////////////////////////////////
  variable data_menu
  variable vars_selected
  variable options_selected
  variable ht_vars
  variable ht_tree
  variable item_data
  variable data

  variable tolset
  variable tolindex

  #User functions to contextual menu
  ::TolInspector::ReadUserFunctions

  array unset options_selected

  foreach it [array names data_menu] {
    $data_menu($it) delete 0 end
  }
  set InRootConsole [string equal [lindex [$ht_tree get -full anchor] end] "root-console"]
  set InRootPool  [string equal [lindex [$ht_tree get -full anchor] end] "root-pool"]
  set InRootFiles [string equal [lindex [$ht_tree get -full anchor] end] "root-files"]
  #puts "PostVariable: InRootConsole,InRootPool,InRootFiles = $InRootConsole,$InRootPool,$InRootFiles"
  set node_act [$ht_vars index current]
  if { [string length $node_act] } {
    set vars_selected [$ht_vars curselection]
    if { [lsearch $vars_selected $node_act] >= 0 } {
      foreach var $vars_selected {
        array set aryData [$ht_vars entry cget $var -data]
	    #puts "PostVariable: aryData = [array get aryData]"
        set itemid $aryData(Index)
        set path   $aryData(Path)
        set grammar [lindex $item_data($itemid) 2]
        set objName [$ht_vars entry cget $var -label]
        #puts "OBJECT=$objName, PATH=$path"
        if { $InRootPool } {
          set object $data(pool,variables,$itemid)
		} else {
          if {[llength $aryData(Reference)]} {
            #set object [lindex $aryData(Reference) 0]
            set object [lrange $aryData(Reference) 0 end]
          } else {
            if { [string length $tolset] } {
              #set object [concat [list File $tolset] $tolindex $itemid]
              set object [concat $tolset $tolindex $itemid]
			  #puts "tolset = $tolset"
            } else {
              if [llength $objName] {
                set object [list $grammar $objName]
              } else {
			    set object $objName
              }
            }
          }
        }
        #puts "PostVariable: OBJECT=$object OBJNAME=$objName TOLSET=$tolset PATH=$path GRAMMAR=$grammar"
        switch -- $grammar {
          Code    -
          Complex -
          Date    -
          Matrix  -
          VMatrix -
          Polyn   -
          Ratio   -
          Real    -
          Serie   -
          Anything -
          Text    -
          NameBlock -
          TimeSet {
            lappend options_selected($grammar) [list $object $objName $path]
          }
          Set {
            # Si el objeto se da como referencia
            if { [llength $object] > 1 }  {
			  #puts "PostVariable: Set: object = $object"
              if {$InRootFiles} {
                set isFile 1
              } else {
                set info [::tol::info var $object]
                set isFile [expr [lindex $info 7] == 4]
              }
              #set desc [lindex $info 4]
            } else {
              if {[catch "::tol::info included $object" info]} {
                set info [::tol::info var [list Set $objName]]
                #set isFile [expr [lindex $info 5] == 4]
                set isFile [expr [lindex $info 7] == 4]
                #set desc [lindex $info 4]
              } else {
                #set info [::tol::info included $objName]
                #set isFile [expr [lindex $info 4] == 4]
                set isFile 1
                #set desc ""
              }
            }
            if $isFile {
              lappend options_selected(File) [list $object $objName $path]
            } else {
              lappend options_selected($grammar) [list $object $objName $path]
            }
          }
        }
      }
      set grammar_selected [array names options_selected]
      if { [llength $grammar_selected] } {
        foreach grammar $grammar_selected {
          switch -- $grammar {
            VMatrix -
            Matrix  {
              $data_menu(main) add cascade -label [mc "Export matrix"]\
              -menu $data_menu(Matrix,Export)
              if {[llength $options_selected($grammar)] == 1} {
                set object [lindex [lindex $options_selected($grammar) 0] 0]
                puts "PostVariable:Matrix: object=$object"
                $data_menu(main) add command -label [mc "Table matrix"] \
                  -command [list ::TolInspector::TableMatrix $object]
                $data_menu(Matrix,Export) add command -label "BDT" \
                  -command [list ::TolInspector::ExportBDTMatrix $object]
                $data_menu(main) add command -label [mc "Draw matrix"] \
                  -command [list ::TolInspector::DrawMatrix $object]
              } else {
                $data_menu(main) add cascade -label [mc "Table matrix"] \
                  -menu $data_menu(Matrix,Table)
                $data_menu(Matrix,Export) add cascade -label "BDT" \
                  -menu $data_menu(Matrix,Export,BDT)
                $data_menu(main) add cascade -label [mc "Draw matrix"] \
                  -menu $data_menu(Matrix,Draw)
                foreach objInfo $options_selected(Matrix) {
                  $data_menu(Matrix,Table) add command -label [lindex $objInfo 1]\
                    -command [list ::TolInspector::TableMatrix [lindex $objInfo 0]]
                  $data_menu(Matrix,Export,BDT) add command -label [lindex $objInfo 1]\
                    -command [list ::TolInspector::ExportBDTMatrix [lindex $objInfo 0]]
                  $data_menu(Matrix,Draw) add command -label [lindex $objInfo 1] \
                    -command [list ::TolInspector::DrawMatrix [lindex $objInfo 0]]
                }
              }
            }
            Serie   {
              if {[llength $options_selected(Serie)] == 1} {
                set lblSerie "serie"
              } else  {
                set lblSerie "series"
              }
              $data_menu(main) add command -label [mc "Draw $lblSerie"]\
              -command ::TolInspector::DrawSerie
              $data_menu(main) add command -label [mc "Table $lblSerie"]\
              -command ::TolInspector::TableSerie             
              if {[llength $options_selected(Serie)] == 1} {
                set object [lindex [lindex $options_selected(Serie) 0] 0]
                $data_menu(main) add command -label [mc "Draw autocorrelation"] \
                  -command [list ::TolInspector::DrawAutocorSerie $object]
                $data_menu(main) add command -label [mc "Table autocorrelation"] \
                  -command [list ::TolInspector::TableAutocorSerie $object]
              } else {
                $data_menu(main) add cascade -label [mc "Draw autocorrelation"] \
                  -menu $data_menu(Serie,DrawAutocor)
                $data_menu(main) add cascade -label [mc "Table autocorrelation"] \
                  -menu $data_menu(Serie,TableAutocor)
                foreach objInfo $options_selected(Serie) {
                  $data_menu(Serie,DrawAutocor) add command -label [lindex $objInfo 1]\
                    -command [list ::TolInspector::DrawAutocorSerie [lindex $objInfo 0]]
                  $data_menu(Serie,TableAutocor) add command -label [lindex $objInfo 1]\
                    -command [list ::TolInspector::TableAutocorSerie [lindex $objInfo 0]]
                }
              }
              $data_menu(main) add command -label [mc "Table statistics"]\
                  -command ::TolInspector::TableStatisticsSerie
            }
            TimeSet {
              if {[llength $options_selected(TimeSet)] == 1} {
                set object [lindex [lindex $options_selected(TimeSet) 0] 0]
                $data_menu(main) add command -label [mc "View timeset"] \
                  -command [list ::TolInspector::ViewTimeSet $object]
              } else {
                $data_menu(main) add cascade -label [mc "View timeset"] \
                  -menu $data_menu(TimeSet,View)
                foreach objInfo $options_selected(TimeSet) {
                  $data_menu(TimeSet,View) add command -label [lindex $objInfo 1] \
                    -command [list ::TolInspector::ViewTimeSet [lindex $objInfo 0]]
                }
              }
            }
            NameBlock {
              # entradas desde MenuManager
              if 0 {
              TolGui_InsertEntriesFromMenuManager \
                  $data_menu(main) \
                  $options_selected(NameBlock)
              }
              # entradas desde @MenuDesc
              # @MenuDesc sera reemplazado, ver https://www.tol-project.org/ticket/1372
              set sel_length [ llength $options_selected(NameBlock) ]
              if { $sel_length } {
                set idx_current [ lsearch $vars_selected $node_act ]
                set all_nb_options \
                    [ TolGui_GetMenuEntries \
                          $options_selected(NameBlock) $idx_current ]
                set num_options [ llength $all_nb_options ]
                #puts "num_options = $num_options"
                foreach mi $all_nb_options {
                  set _cname [ lindex $mi 0 ]
                  set cname [ string map [ list @ "" . _ ] $_cname ]
                  #puts "cname = $cname"
                  foreach { ninst obj_addr entries } [ lindex $mi 1 ] break
                  set instances [ lindex $mi 2 ]
                  if { $num_options > 1 } {
                  # creo el menu de la clase si no existe
                    set mclass $data_menu(main).cls$cname
                    if { ![ winfo exists $mclass ] } {
                      menu $mclass -tearoff 0
                    } else {
                      $mclass delete 0 end
                    }
                    $data_menu(main) add cascade -label $cname -menu $mclass
                    set target_menu $mclass
                  } else {
                    set target_menu $data_menu(main)
                  }
                  foreach inst_entry $entries {
                    foreach { label single group } $inst_entry break
                    set opt_label \
                        "$label [ expr { $sel_length>1 ? $ninst : {} } ]"
                    $target_menu add command -label $opt_label \
                        -command [ list TolGui_InvokeMethod \
                                       $single $obj_addr ]
                    if { $group ne "" && [ llength $instances ] > 1 } {
                      # hay una funcion que se aplica a toda la seleccion
                      $target_menu add command -label "$label All" \
                          -command [ list TolGui_InvokeGroup \
                                         $_cname $group $instances ]
                    }
                  }
                }
              }
            }
            Set {
              if {[llength $options_selected(Set)] == 1} {
                set object  [lindex [lindex $options_selected(Set) 0] 0]
                $data_menu(main) add command -label [mc "Table set"] \
                  -command [list ::TolInspector::TableSet $object]
                $data_menu(main) add command -label [mc "Draw set"] \
                  -command [list ::TolInspector::DrawSet $object]
                # menu special
                $data_menu(main) add cascade -label [mc "Draw set special"] \
                  -menu $data_menu(Set,Draw,Special) 
                $data_menu(Set,Draw,Special) add command \
                   -label [mc SetGraphAllFirst] \
                  -command [list ::TolInspector::DrawSet $object 1]
                $data_menu(Set,Draw,Special) add command \
                  -label [mc SetGraphAllGeneric] \
                  -command [list ::TolInspector::DrawSet $object 2]
                $data_menu(Set,Draw,Special) add command \
                  -label [mc SetGraphEvenUneven] \
                  -command [list ::TolInspector::DrawSet $object 3]                
              } else {
                $data_menu(main) add cascade -label [mc "Table set"] \
                  -menu $data_menu(Set,Table)
                $data_menu(main) add cascade -label [mc "Draw set"] \
                  -menu $data_menu(Set,Draw)
                foreach objInfo $options_selected(Set) {
                  $data_menu(Set,Table) add command -label [lindex $objInfo 1] \
                    -command [list ::TolInspector::TableSet [lindex $objInfo 0]]
                  $data_menu(Set,Draw) add command -label [lindex $objInfo 1] \
                    -command [list ::TolInspector::DrawSet [lindex $objInfo 0]]
                }
                # menu special
                set i 0
                $data_menu(main) add cascade -label [mc "Draw set special"] \
                  -menu $data_menu(Set,Draw,Special,M)                 
                foreach objInfo $options_selected(Set) {
                  # exists menu previus ?
                  if {[winfo exists $data_menu(Set,Draw,Special,M).setdrawspecialm$i]} {
                    destroy $data_menu(Set,Draw,Special,M).setdrawspecialm$i
                  }
                  set mn \
                    [menu $data_menu(Set,Draw,Special,M).setdrawspecialm$i \
                      -tearoff 0]
                  $data_menu(Set,Draw,Special,M) add cascade \
                    -label [lindex $objInfo 1] -menu $mn
                  $mn add command -label [mc SetGraphAllFirst] \
                    -command [list ::TolInspector::DrawSet [lindex $objInfo 0] 1]
                  $mn add command -label [mc SetGraphAllGeneric] \
                    -command [list ::TolInspector::DrawSet [lindex $objInfo 0] 2]
                  $mn add command -label [mc SetGraphEvenUneven] \
                    -command [list ::TolInspector::DrawSet [lindex $objInfo 0] 3]                                  
                  incr i
                }                
              }
            }
            File {
              if {[llength $options_selected(File)] == 1} {
                set objName [lindex [lindex $options_selected(File) 0] 1]
                $data_menu(main) add command -label [mc "View file"] \
                  -command [list ::TolInspector::ViewFile $objName]
                if $InRootFiles {
                   $data_menu(main) add command -label [mc "Decompile file"] \
                     -command [list ::TolInspector::DecompileFile $objName]
                }
              } else {
                $data_menu(main) add cascade -label [mc "View file"] \
                  -menu $data_menu(File,View)
                foreach objInfo $options_selected(File) {
                  $data_menu(File,View) add command -label [lindex $objInfo 1] \
                    -command [list ::TolInspector::ViewFile [lindex $objInfo 1]]
                }
                if $InRootFiles {
                  $data_menu(main) add cascade -label [mc "Decompile file"] \
                    -menu $data_menu(File,Decompile)
                  foreach objInfo $options_selected(File) {
                    $data_menu(File,Decompile) add command -label [lindex $objInfo 1] \
                      -command [list ::TolInspector::DecompileFile [lindex $objInfo 1]]
                  }
                }
              }            
            }
          } ;# end del switch
          # inserto las opciones definidas en MenuManager
          if { $grammar ne "Anything" } {
            TolGui_InsertEntriesFromMenuManager \
                $data_menu(main) \
                $options_selected($grammar)
          }

          $data_menu(main) add separator
          puts "grammar = $grammar"
          $data_menu(main) add cascade -label [mc "Functions"] \
             -menu $data_menu($grammar,Functions)
          ShowMenuUserFunc $grammar
          if {[llength $options_selected($grammar)] == 1} {
            set objName [lindex [lindex $options_selected($grammar) 0] 1]
            set path    [lindex [lindex $options_selected($grammar) 0] 2]
            $data_menu(main) add command -label [mc "View %1\$s's definition" $grammar] \
               -command [list ::TolInspector::ViewDefinition $objName $path $grammar]
          } else {
            $data_menu(main) add cascade -label [mc "View %1\$s's definition" $grammar] \
               -menu $data_menu($grammar,File)
            foreach objInfo $options_selected($grammar) {
              $data_menu($grammar,File) add command -label [lindex $objInfo 1]\
              -command [list ::TolInspector::ViewDefinition\
                              [lindex $objInfo 1] [lindex $objInfo 2] $grammar]
            }
          }
          $data_menu(main) add separator
        } ;#end del foreach
        if { $InRootPool } {
          $data_menu(main) add command -label [mc "Remove from spool"] \
            -command [list ::TolInspector::RemoveFromSpool $ht_vars]
        } else  {
          $data_menu(main) add command -label [mc "Add to spool"] \
            -command [list ::TolInspector::AddToSpool $ht_vars [lindex [$ht_tree get -full anchor] 1]]
        }
        $data_menu(main) add command -label [mc "To Eval window"] \
                -command [list ::TolInspector::AddToEvalWindow $ht_vars]
        $data_menu(main) add separator
        if { $InRootConsole} {
          $data_menu(main) add command -label [mc "Decompile"] \
            -command [list ::TolInspector::ClearConsoleObjSel $ht_vars]
          $data_menu(main) add separator
        }
        $data_menu(main) add command -label [mc "Select all"] \
                -command [list ::TolInspector::SelectAll $ht_vars]
        $data_menu(main) add command -label [mc "Toggle selection"] \
                -command [list ::TolInspector::ToggleSelection $ht_vars]
        tk_popup $data_menu(main) $x $y
      } ;#end del if
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::PostFunction { x y } {
#
# PURPOSE: Composes a popup menu for functions and displays it. Its composed
#          depending on the node/s clicked by user.
#
# PARAMETERS:
#   x -> x-coordinate where popup menu is drawn
#   y -> y-coordinate where popup menu is drawn
#
#/////////////////////////////////////////////////////////////////////////////
  variable data_menu
  variable vars_selected
  variable ht_funcs
  variable ht_tree
  variable item_data
  variable data
  
  variable tolset
  variable tolindex
  
  array unset options_selected
  puts "PostFunction"
  foreach it [array names data_menu] {
    $data_menu($it) delete 0 end
  }
  
  set InRootPool  [string equal [lindex [$ht_tree get -full anchor] end] "root-pool"]
  set node_act [$ht_funcs index current]
  if { [string length $node_act] } {
    set vars_selected [$ht_funcs curselection]
    if { [lsearch $vars_selected $node_act] >= 0 } {
      foreach var $vars_selected {
        array set aryData [$ht_funcs entry cget $var -data]
        set itemid $aryData(Index)
        set path   $aryData(Path)
        set grammar [lindex $item_data($itemid) 2]
        set objName [$ht_funcs entry cget $var -label]
        
        if { $InRootPool } {
          set object $data(pool,variables,$itemid)
        } else {
          if { [string length $tolset] } {
            set object [concat [list $tolset] $tolindex $itemid]
          } else {
            set object $objName
          }
        }
        # La única gramática seleccionable en ht_funcs es Code
        lappend code_selected [list $object $objName $path]
      }
    
      if {[llength $code_selected] == 1} {
        set objName [lindex [lindex $code_selected 0] 1]
        set path    [lindex [lindex $code_selected 0] 2]
        $data_menu(main) add command -label [mc "View %1\$s's definition" $grammar] \
                -command [list ::TolInspector::ViewDefinition $objName $path $grammar]
      } else {
        $data_menu(main) add cascade -label [mc "View %1\$s's definition" $grammar] \
                -menu $data_menu($grammar,File)
        foreach objInfo $code_selected {
          $data_menu($grammar,File) add command -label [lindex $objInfo 1]\
                -command [list ::TolInspector::ViewDefinition\
                [lindex $objInfo 1] [lindex $objInfo 2] $grammar]
        }
      }
  
      if { $InRootPool } {
         $data_menu(main) add command -label [mc "Remove from spool"] \
                  -command [list ::TolInspector::RemoveFromSpool $ht_funcs]
      } else  {
         $data_menu(main) add command -label [mc "Add to spool"] \
                  -command [list ::TolInspector::AddToSpool $ht_funcs [lindex [$ht_tree get -full anchor] 1]]
      }
      $data_menu(main) add command -label [mc "To Eval window"] \
                -command [list ::TolInspector::AddToEvalWindow $ht_funcs]
      $data_menu(main) add separator
      $data_menu(main) add command -label [mc "Select all"] \
              -command [list ::TolInspector::SelectAll $ht_funcs]
      $data_menu(main) add command -label [mc "Toggle selection"] \
              -command [list ::TolInspector::ToggleSelection $ht_funcs]
      tk_popup $data_menu(main) $x $y
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::PostTree { x y } {
#
# PURPOSE: Composes a popup menu for left treeview and displays it. Its
#          composed depending on the node clicked by user.
#
# PARAMETERS:
#   x -> x-coordinate where popup menu is drawn
#   y -> y-coordinate where popup menu is drawn
#
#/////////////////////////////////////////////////////////////////////////////
  variable data_menu
  variable vars_selected
  variable ht_tree
  variable item_data
  variable data
  variable tolset
  variable tolindex
  variable item_id
  #puts "ht_tree: $ht_tree"
  #No terminado todavía
  #return
  array unset options_selected
  
  puts "::TolInspector::PostTree $x $y" 
  foreach it [array names data_menu] {
    $data_menu($it) delete 0 end
  }
  set InRootPool  [string equal [lindex [$ht_tree get -full anchor] end-1] "root-pool"]
  #puts "InRootPool: $InRootPool"
  # InRootFiles has a value 1 if node is a File
  set InRootFiles [string equal [lindex [$ht_tree get -full anchor] end-1] "root-files"]
  #puts "InRootFiles: $InRootFiles"
  # index of the selected node
  set node_act [$ht_tree index current]
  #puts "node_act: $node_act"  
  if { [string length $node_act] } {
    set vars_selected [$ht_tree curselection]
    puts "vars_selected: $vars_selected"      
    if { [lsearch $vars_selected $node_act] >= 0 } { ;# Not root node
      set index [$ht_tree index anchor]
      puts "index: $index"    
      if { $index } {
        set info_node [split [lindex [$ht_tree get -full $index] end] "-"]
        puts "info_node: $info_node"  
        set node_type [lindex $info_node 0]
        puts "node_type: $node_type"          
        set node_name [lindex $info_node 1]
        puts "node_name: $node_name"          
      } else { ;# root node
        set node_type root
        set node_name root
      }
      # if user selects a root or a grammar doesn't nothing
      switch -regexp $node_type  {
        root -
        grammar { return }
      }
      # label of object showed in the tree
      set objName [$ht_tree entry cget $index -label]
      puts "objName: $objName"
      set object $objName

      if {$InRootFiles} {
        array set aryData [$ht_tree entry cget $index -data]
        if [info exist aryData(FileName)]  {
          set objName $aryData(FileName)
        }
      }
      #puts "2objName: $objName"   
      if { $InRootPool } {
        #puts "es InRootPool"        
        set itemid $item_id
        set object $data(pool,variables,$itemid)
      } else {
        #puts "NO InRootPool"
        if { [string length $tolset] } {          
          set object [concat [list $tolset] $tolindex]
        } else {
          set object $objName
        }
      }
      puts "object = $objName '[$ht_tree entry cget $index -data]'"
#Tolcon_Trace "***********************************" {red}
#Tolcon_Trace "index=$index, node_type=$node_type, node_name=$node_name" {red}
#Tolcon_Trace "tolset=$tolset, tolindex=$tolindex, aryData= [$ht_tree entry cget $index -data]" {red}
#Tolcon_Trace "object=$object, label=$objName" {red}
#Tolcon_Trace "***********************************" {red}
      set stackList [::tol::console stack list]
      #puts "stackList: $stackList"
      set stackValue {}
      foreach stackItem $stackList {
        #puts "stackItem: $stackItem"
        #puts "lindex $stackValue 1: [lindex $stackValue 1]"
        #puts "objName: $objName"  
        if {[lindex $stackItem 1] eq $objName} {
          set stackValue $stackItem
        }
        break
      }
      #puts "stackValue: $stackValue"
      set grammar [lindex $stackValue 0]      
      #puts "grammar: $grammar"
      #set full_data $item_data($index) ;# {value} {desc} {grammar}   
      #set grammar [lindex $full_data 2]
      
      #foreach fun  [::tol::info functions $grammar] {
      #  set finfo [::tol::info functions $grammar $fun]
      #  InsertChild Code $fun [lindex $finfo 0] [lindex $finfo 1] [list [lindex $finfo 2]] 0    
      #}
      if {[catch {set info [::tol::info included $objName]}]} {
        if { !($grammar eq "")} {
          if {[catch {set info [::tol::info var [list $grammar $object]}]} {
            #set info [concat [list "$grammar" $objName ] [::tol::info var $grammar $objName]]
			set info [::tol::info var [list $grammar $objName]]
            #Tolcon_Trace "INFO de ::tol::info var Set objName"
          } else  {
            set info [::tol::info var [list $grammar $object]]
            #Tolcon_Trace "INFO de ::tol::info var $grammar object"
          }        
          set isFile [expr [lindex $info 5] == 4]
        } else {
          set isFile 0
        }

      } else {
        set info [::tol::info included $objName]
        #Tolcon_Trace "INFO de ::tol::info included objName"
        set isFile 1
      }
      #puts "paso"

      if $isFile {
        #puts "entro como fichero"        
        $data_menu(main) add command -label [mc "View file"] \
                -command [list ::TolInspector::ViewFile $objName]
        if $InRootFiles {
          $data_menu(main) add command -label [mc "Decompile file"] \
                -command [list ::TolInspector::DecompileFile $objName]
        }
      } else {
        #puts "no entro como fichero"        
        if { $grammar eq "Set" } {
          $data_menu(main) add command -label [mc "Table set"] \
                -command [list ::TolInspector::TableSet $object]
          $data_menu(main) add command -label [mc "Draw set"] \
                -command [list ::TolInspector::DrawSet $object]
        }
      }
      tk_popup $data_menu(main) $x $y
    }
  }
}


#/////////////////////////////////////////////////////////////////////////////
# Procedures asociated with tol inspector popup menu
#/////////////////////////////////////////////////////////////////////////////

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::RemoveFromSpool {ht} {
#
# PURPOSE: Removes from spool all selected objects
#
#/////////////////////////////////////////////////////////////////////////////
  variable vars_selected
  variable item_data
  variable tolset
  variable tolindex
  variable data

  foreach var $vars_selected {
    array set aryData [$ht entry cget $var -data]
    set itemid $aryData(Index)
    set path   $aryData(Path)
    set reference $aryData(Reference)
    set grammar [lindex $item_data($itemid) 2]

    set objName [$ht entry cget $var -label]

    set _objpool [list "grammar" $grammar "objName" $objName "reference" $reference "path" $path]
    if {$grammar eq "Code"} {
      set graApply [lindex $aryData(Content) 0]
      lappend _objpool "graApply" $graApply
    }
    #Tolcon_Trace "OBJPOOL on REMOVE: $_objpool"

    # Buscamos el objeto (identificador: referencia)
    array set objpool $_objpool
    foreach _obj $data(pool,objects) {
      array set obj $_obj
      #Tolcon_Trace "obj(reference)=$obj(reference), objpool(reference)=$objpool(reference)"
      if {[string equal $obj(reference) $objpool(reference)]} {
        if {$obj(grammar) eq "Code"} {
          if {[string equal $obj(graApply) $objpool(graApply)]} {
            set idx [lsearch $data(pool,objects) $_obj]
            #Tolcon_Trace "El obj a quitar tiene idx=$idx"
            set data(pool,objects) [lreplace $data(pool,objects) $idx $idx]
          }
        } else {
          set idx [lsearch $data(pool,objects) $_obj]
          #Tolcon_Trace "El obj a quitar tiene idx=$idx"
          set data(pool,objects) [lreplace $data(pool,objects) $idx $idx]
        }
      }
    }
  }
  #Tolcon_Trace "P.O. after REMOVE: $data(pool,objects)"
  UpdatePoolObj
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::AddToSpool {ht origin} {
#/////////////////////////////////////////////////////////////////////////////
  variable vars_selected
  variable item_data
  variable tolset
  variable tolindex
  variable data

  foreach var $vars_selected {
    array set aryData [$ht entry cget $var -data]
    set itemid $aryData(Index)
    set path   $aryData(Path)
    set grammar [lindex $item_data($itemid) 2]
    set objName [$ht entry cget $var -label]
	#puts "AddToSpool: Reference=$aryData(Reference) tolset=$tolset grammar=$grammar"
    if {[llength $aryData(Reference)]} {
      set reference $aryData(Reference)
    } else {
      if { [string length $tolset] } {
        #set object [concat [list $tolset] $tolindex $itemid]
        #set object [concat [list File $tolset] $tolindex $itemid]
        set object [concat $tolset $tolindex $itemid]
        #puts "AddToSpool: object=$object"
        set reference [::tol::info reference $object]
      } else {
        if {$grammar eq "Set"} {
          #puts "AddToSpool: origin=$origin"
          set reference [::tol::info reference [list Set $objName]]
          #Tolcon_Trace "reference de ::tol::info reference [list $objName]"
        } else {
          if [catch {set reference [::tol::info reference [list $grammar $objName]]}] {
            set reference $objName
            #Tolcon_Trace "reference NO SE QUE PONER"
          }
          #Tolcon_Trace "reference de ::tol::info reference $grammar $objName"
        }
      }
    }
    set _objpool [list "origin" $origin "grammar" $grammar "objName" $objName "reference" $reference "path" $path]
    if {$grammar eq "Code"} {
      set graApply [lindex $aryData(Content) 0]
      lappend _objpool "graApply" $graApply
    } 
    #Tolcon_Trace "***************"
    #Tolcon_Trace "OBJPOOL on ADD:"
    #Tolcon_Trace "***************"
    #foreach {key value} $_objpool {
    #  Tolcon_Trace "  $key=$value"
    #}
    set inserted 0
    # Si la referencia es igual, el objeto es el mismo (salvo para obj de tipo Code)
    array set objpool $_objpool
    foreach _obj $data(pool,objects) {
      array set obj $_obj
      if {[string equal $obj(reference) $objpool(reference)]} {
        #Si es de la gramatica Code miro a ver la gramatica sobre la que se aplica
        if {$objpool(grammar) eq "Code"} {
          if {[string equal $obj(graApply) $objpool(graApply)]} {
            set inserted 1
          }
        } else {
          set inserted 1
        }
      } 
    }
    if {!$inserted} {
      lappend data(pool,objects) $_objpool
      #puts "AddToSpool: P.O. after ADD: $data(pool,objects)"
    } else  {
      #puts "$_objpool ALREADY IN SPOOL"
    }
  }
  
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::AddToEvalWindow {ht} {
#
# PURPOSE: Adds names of selected objects (vars or functions) in treeview ht
#          to eval window
#
#/////////////////////////////////////////////////////////////////////////////
  variable ht_vars
  variable vars_selected 

  set names ""
  foreach var $vars_selected {   
    lappend names [$ht entry cget $var -label]
  }
  ::TolConsole::ToEvalWindow [ListToStr $names]
}


#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::SelectAll {ht} {
#
# PURPOSE: Selects all nodes of a given treeview
#
# PARAMETERS:
#   ht -> treeview's path
#
#/////////////////////////////////////////////////////////////////////////////
  set found [$ht find]
  if {[llength $found] > 1} {
    $ht selection set [lindex $found 1] [lindex $found end]
  } 
}


#/////////////////////////////////////////////////////////////////////////////
  proc ::TolInspector::ToggleSelection {ht} {
#
# PURPOSE: Toggles selection of nodes of a given treeview
#
# PARAMETERS:
#   ht -> treeview's path
#
#/////////////////////////////////////////////////////////////////////////////
  set found [$ht find]
  if {[llength $found] > 1} {
    $ht selection toggle [lindex $found 1] [lindex $found end]
  } 
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::DrawSet {setref {type 0}} {
#
# PURPOSE: Graph a Set. For it it calls to a form for the selection of 
#          columns to graph
#
# PARAMETERS:
#   setref -> Set to graph
#   type   -> 0 for custom association of data columns
#          -> 1 for all data columns to first one
#          -> 2 for all data columns to generic
#          -> 3 for even data columns to odd ones
#
#/////////////////////////////////////////////////////////////////////////////
  variable group_id
  variable w_tabset
  #puts "::TolInspector::DrawSet. setref: $setref"
  incr group_id
  if {[llength $setref] > 1} {
    set name [lindex [::tol::info variable $setref] 1]
  } else {
    set name $setref
  }
  set cmd tablecmd$group_id
  if {![catch {::tol::tableset create $cmd $setref}]} {
    switch $type {
      0 {
        set tl [::project::CreateForm \
          -width 500 -height 500 -type Graphs \
          -iniconfig Graph \
          -title [mc "Set graph: %s" $name] -helpkey "Tb4GraSet"]
        $tl withdraw
        set tlf [$tl getframe] 
        if {[set Instance [::SetGraphDialog::Init $tlf.g $cmd $setref]] != 0} {
          bind $tlf.g <Destroy> +[list ::tol::tableset destroy $cmd]
          pack $tlf.g -fill both -expand yes
          $tl restore
          update
          # Esta opción se debe aplicar cuando el gráfico esté empacado
          ::bayesGraph::GrapOptApplyAxisTicks $Instance ${Instance}::options gr,0
        } else {
          destroy $tl
          return
        }
        $tl bind <Control-F4> [list $tl kill]              
      }
      1 -
      2 -
      3 {
        # windows 
        set tl [::project::CreateForm \
          -title [mc "Set graph: %s" $name] -type Graphs -iniconfig SetGraph]
        $tl withdraw        
        set tlf [$tl getframe]
        set Instance [::SetGraphDialog::DrawSet $tlf.g $cmd $setref $type]
        bind $tlf.g <Destroy> +[list ::tol::tableset destroy $cmd]
        pack $tlf.g -fill both -expand yes
        $tl restore
        update
        # Esta opción se debe aplicar cuando el gráfico esté empacado        
        ::bayesGraph::GrapOptApplyAxisTicks $Instance ${Instance}::options gr,0
      }
    }
  } else {
    tk_messageBox -type ok -icon warning \
        -message [mc "Set is not graficable: %s" $::errorInfo] \
        -parent $w_tabset -title [mc "Warning"]
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::DrawMatrix {matref} {
#
# PURPOSE: draw the given matrix
#
# PARAMETERS:
#   matref -> reference to the tol matrix
#
#/////////////////////////////////////////////////////////////////////////////
  variable group_id
  variable w_tabset

  incr group_id
  if {[llength $matref] > 1} {
    set name [lindex [::tol::info variable $matref] 1]
  } else {
    if { ([info exists obj]) } {
      set name $obj
    }
  }
  if { ([info exists name]) } {
    set cmd tablecmd$group_id
    if {![catch {::tol::tablematrix create $cmd $matref}]} {
      set tl [::project::CreateForm \
                  -width 500 -height 500 -type Graphs \
                  -iniconfig Graph \
		  -title [mc "Matrix graph: %s" $name] -helpkey "Tb4Gra"]
      $tl withdraw
    
      set tlf [$tl getframe]
      if {[set Instance [::SetGraphDialog::Init $tlf.g $cmd $matref]] != 0} {
        bind $tlf.g <Destroy> +[list ::tol::tablematrix destroy $cmd]
        pack $tlf.g -fill both -expand yes
	    $tl restore
        update
        # Esta opción se debe aplicar cuando el gráfico esté empacado
        ::bayesGraph::GrapOptApplyAxisTicks $Instance ${Instance}::options gr,0
      } else {
        destroy $tl
        return
      }
      $tl bind <Control-F4> [list $tl kill]
    } else {
      tk_messageBox -type ok -icon warning \
        -message [mc "Matrix is not graficable: %s" $::errorInfo] \
        -parent $w_tabset -title [mc "Warning"]
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::TableSet { set } {
#/////////////////////////////////////////////////////////////////////////////  
  variable w_tabset
  variable group_id

  incr group_id
  if {[llength $set] > 1} {
    set nameSet [lindex [::tol::info variable $set] 1]
  } else {
    set nameSet $set
  }
  set setgrp group$group_id
  if {![catch {::tol::tableset create $setgrp $set}]} {
    # create
    set tl [::project::CreateForm \
        -title    "[mc "Set table"]: $nameSet" \
        -iniconfig SetTable \
        -type Tables -helpkey "Tb4Edt"
    ]
    set tlf [$tl getframe]
    # config
    btable $tlf.t -parent $tl \
                  -posteditcmd [list ::TolConsole::TableRecentUpdate $tlf.t] \
                  -iniconfig btable
    $tlf.t fillSet $setgrp
    pack $tlf.t -fill both -expand yes
    bind $tlf.t <Destroy> +[list ::tol::tableset destroy $setgrp]
    $tl bind <Control-F4> [list $tl kill]
    update
  } else {
    tk_messageBox -type ok -icon warning -message [mc "Set not tabulable"]\
        -parent $w_tabset -title [mc Warning]
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::TableMatrix { matrix } {
#/////////////////////////////////////////////////////////////////////////////
  set mattb [::tol::matrix $matrix]
  # create
  set tl [::project::CreateForm \
              -title "[mc {Matrix table}]: [lindex $mattb 0]" \
              -iniconfig MatrixTable \
              -type Tables -helpkey "Tb4Edt"
  ]
  set tlf [$tl getframe]
  # create matrix
  btable $tlf.t -parent $tl \
                -posteditcmd [list ::TolConsole::TableRecentUpdate $tlf.t] \
                -iniconfig btable
  $tlf.t fillMatrix $mattb
  $tl bind <Control-F4> [list $tl kill]  
  pack $tlf.t -fill both -expand yes

  #if {$grammar == "VMatrix"} {
    #::tol::console stack release "zxw._.wxz"
  #}
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::ExportBDTMatrix { matrix } {
#/////////////////////////////////////////////////////////////////////////////
  variable group_id
  
  incr group_id
  set tl [toplevel .top$group_id]
  wm title $tl "ExportBDTMatrix"
  
  #pack $tl.t -fill both -expand yes
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::DrawSerie { } {
#/////////////////////////////////////////////////////////////////////////////  
  variable options_selected
  upvar \#0 ::TolInspector::options_selected(Serie) series_selected
  
  #puts "DrawSerie: series_selected=$series_selected"
  set series ""
  foreach it $series_selected {
    lappend series [lindex $it 0]
  }
  DrawSeries $series
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::DrawSeries { series {gcf ""}} {
#/////////////////////////////////////////////////////////////////////////////  
  variable group_id

  if { [llength $series] } {
    incr group_id
    # series     
    set sergrp group$group_id
    set t0 [time {
    eval "::tol::seriegrp create $sergrp $series"
    }]
    # name
    set name [$sergrp serie 0 name]
    if { [$sergrp serie size] > 1} {
      set name ${name}...
    }
    # create
    set t1 [time {
    set tl [::project::CreateForm \
        -title [mc "Series graph: %s" $name] \
        -type Graphs \
        -iniconfig SeriesGraph \
	-width 900 -height 650 -helpkey "Tb4GraSer"
           ]}]
    set tlf [$tl getframe]
    # configuration
    set t2 [time {
    set Instance [::SeriesGraph::Create $tlf.g $sergrp]
    }]
    #puts "t0 = $t0"
    #puts "t1 = $t1"
    #puts "t2 = $t2"
    if { $gcf != "" } {
      ::bayesGraph::LoadGCF $Instance $gcf
    }
    bind $tlf.g <Destroy> +[list ::tol::seriegrp destroy $sergrp]
    pack $tlf.g -fill both -expand yes
    $tl bind <Control-F4> [list $tl kill]
    update
    # Esta opción se debe aplicar cuando el gráfico esté empacado
    #::bayesGraph::GrapOptApplyAxisTicks $Instance ${Instance}::options gr,0
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::DrawAutocorSerie { serie } {
#/////////////////////////////////////////////////////////////////////////////  
  variable group_id
    
  incr group_id
  # title
  
  if {[llength $serie] > 1} {
    set nameSerie [lindex [::tol::info variable $serie] 1]
  } else {
    set nameSerie $serie
  }
  # create
  set tl [::project::CreateForm \
      -title "[mc "Autocorrelation graph"]: $nameSerie" \
      -iniconfig AutocorGraph \
      -type Graphs \
      -width 900 -height 650 -helpkey "Tb4GraAco"
  ]
  set tlf [$tl getframe]
  # configuration
  ::AutocorGraph::Create $tlf.g $serie
  $tl bind <Control-F4> "$tl kill; break"
  pack $tlf.g -fill both -expand yes
  update
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::TableSerie { } {
#/////////////////////////////////////////////////////////////////////////////  
  variable options_selected
  variable group_id
  upvar \#0 ::TolInspector::options_selected(Serie) series_selected

  set series ""
  foreach it $series_selected {
    lappend series [lindex $it 0]
  }

  if { [llength $series] } {
    incr group_id

    set sergrp group$group_id
    eval "::tol::seriegrp create $sergrp $series"
    
    # create
    set tl [::project::CreateForm \
        -title [mc "Time series table"] \
        -iniconfig SeriesTable \
        -type Tables -helpkey "Tb4Edt"
    ]
	set tlf [$tl getframe]
    # configuration
    btable $tlf.t -parent $tl \
                  -posteditcmd [list ::TolConsole::TableRecentUpdate $tlf.t] \
                  -iniconfig btable
    $tlf.t fillSerie $sergrp 
    bind $tlf.t <Destroy> +[list ::tol::seriegrp destroy $sergrp]
    pack $tlf.t -fill both -expand yes
    $tl bind <Control-F4> [list $tl kill]
    update
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::TableAutocorSerie { serie } {
#/////////////////////////////////////////////////////////////////////////////  
  variable group_id
    
  incr group_id
  # title
  if {[llength $serie] > 1} {
    set nameSerie [lindex [::tol::info variable $serie] 1]
  } else {
    set nameSerie $serie
  }
  # create
  set tl [::project::CreateForm \
      -title "[mc "Autocorrelation table"]: $nameSerie" \
      -iniconfig AutocorGraphTable \
      -type Tables -helpkey "Tb4Edt"
  ]
  set tlf [$tl getframe]
  # configuration
  btable $tlf.t -parent $tl \
                -posteditcmd [list ::TolConsole::TableRecentUpdate $tlf.t] \
                -iniconfig btable
  $tlf.t fillAutocor $serie ""
  $tl bind <Control-F4> [list $tl kill]
  pack $tlf.t -fill both -expand yes
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::TableStatisticsSerie { } {
#/////////////////////////////////////////////////////////////////////////////
  variable options_selected
  variable group_id
  upvar \#0 ::TolInspector::options_selected(Serie) series_selected

  set series ""
  foreach it $series_selected {
    lappend series [lindex $it 0]
  }

  if { [llength $series] } {
    incr group_id

    set sergrp group$group_id
    eval "::tol::seriegrp create $sergrp $series"
    # create
    set tl [::project::CreateForm \
        -title [mc "Statistics table"] \
        -iniconfig StatisticsTable \
        -type Tables -helpkey "Tb4Edt"
    ]
    set tlf [$tl getframe]
    # configuration
    btable $tlf.t -parent $tl \
                  -posteditcmd [list ::TolConsole::TableRecentUpdate $tlf.t] \
                  -iniconfig btable
    $tlf.t fillStatistics $sergrp
    bind $tlf.t <Destroy> +[list ::tol::seriegrp destroy $sergrp]
    $tl bind <Control-F4> [list $tl kill]
    pack $tlf.t -fill both -expand yes
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::ViewTimeSet { tms args } {
#/////////////////////////////////////////////////////////////////////////////
  variable group_id
  
  incr group_id
  
  #puts "::TolInspector::ViewTimeSet $tms"
  # args
  array set opts [list \
    -title "[mc "TimeSet calendar"]: $tms" \
    -geometry {}
  ]
  array set opts $args  
  
  # create
  set tl [::project::CreateForm \
      -title $opts(-title) \
      -iniconfig Calendar \
      -type TimeSets -width 470 -height 345 -helpkey "Tb4Cal"
  ]
  # geometry
  $tl setgeometry $opts(-geometry)
  
  set tlf [$tl getframe]
  # configuration
  set tmscmd tms$group_id
  ::tol::timeset create $tmscmd $tms
  ::CalendarTms::Create $tlf.t $tmscmd
  bind $tlf.t <Destroy> +[list ::tol::timeset destroy $tmscmd]
  bind $tl <Control-F4> [list $tl kill]
  pack $tlf.t -fill both -expand yes
  
  return $tl
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::ViewDefinition {name path grammar} {
#/////////////////////////////////////////////////////////////////////////////
  variable w_tabset
  if {$path == ""} {
    tk_messageBox -type ok -icon warning\
      -message [mc "Object %1\$s is not defined in a tol file" $name]\
      -parent $w_tabset -title [mc Warning]
    return 
  }
  set ext [file extension $path]
  switch -- $ext {
    .tol -
    .TOL {
      set Instance [::Editor::Open $path]
      update
      if {[string equal $grammar "Code"] ||
          [string equal $grammar "Anything"]} {
        append txtFind " " $name "( )*\\("
      } else {
        #append txtFind $grammar "( )+" $name
        append txtFind $name
      }
      #Me situo al principio del texto
      set editor [::Editor::GetText $Instance]
      tk::TextSetCursor $editor 1.0
      ::BayesText::FindShow $editor 1 $txtFind 1
      #::Editor::CmdFindNext $Instance $txtFind
    }
    .bst -
    .BST -
    .bdt -
    .BDT {
      # Igual luego se abre como tabla
      ::Editor::Open $path
    }
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::ViewFile {name} {
#
# PURPOSE:
#
# PARAMETERS:
#   name -> cadena con el nombre de un archivo o el de una variable
#           que hace referencia a un archivo
#
#/////////////////////////////////////////////////////////////////////////////
  #  En name puede venir el nombre de un archivo o el de una variable
  # que hace referencia a un archivo.
  #puts "ViewFile. name=$name"
  if [file isfile $name] {
    set path $name
  } else  {
    set path [::tol::info path $name]
  }
  ::Editor::Open $path
}

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
  variable data
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
  #puts "DECOMPILE 1.3"
  #Quitar del spool todos los objetos provenientes de este archivo
  
  foreach _obj $data(pool,objects) {
    array set obj $_obj
    #Tolcon_Trace "REFERENCIA: $obj(reference)"
    if {[string equal [lindex $obj(reference) 0] $path]} {
      set idx [lsearch $data(pool,objects) $_obj]
      set data(pool,objects) [lreplace $data(pool,objects) $idx $idx]
    }
  }
  ::tol::decompile $path
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
  
  puts "ShowMenuUserFunc: gra=$gra"
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
  variable data
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

namespace eval ::LoadStatus {
  variable data
}

#proc ::LoadStatus::Show {file} {
#    
#  set f [toplevel .topStatus]
#  #wm protocol $path WM_DELETE_WINDOW [list ::Editor::OnDelete $this]
#  # bind Destroy más abajo
#  
#  label $f.lfile -text $file
#  # frame y label para barra de estado
#  set data(sb,frame) [frame $f.fop -relief groove -bd 1 -height 7]
#  set data(sb,objs)  [label $f.fop.objs -relief groove -bd 1 -width 16]
#  set data(sb,files) [label $f.fop.files -text [mc "Not Compiled"] \
#          -relief raised -bd 1 -width 16]
#  set data(sb,time)  [label $f.fop.time -bd 1 -width 14]
#  set data(sb,rell)  [label $f.fop.rell -relief groove -bd 1]
#  
#  
#  # empaquetamos los label de estado
#  pack $f.fop.objs $f.fop.files $f.fop.time -side left
#  pack $f.fop.rell -expand true -fill x
#  
#  grid $f.lfile -sticky news
#  grid $f.fop   -sticky ews
#  
#  grid rowconfigure    $f 1 -weight 1
#  grid columnconfigure $f 0 -weight 1
#    
#}
#
#
#proc ::LoadStatus::Hide {w} {
#    
#
