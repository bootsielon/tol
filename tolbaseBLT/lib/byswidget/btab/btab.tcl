#/////////////////////////////////////////////////////////////////////////////
# FILE    : btab.tcl
# PURPOSE : Bayes tab manager
#/////////////////////////////////////////////////////////////////////////////

package require BLT
package require snit

#/////////////////////////////////////////////////////////////////////////////
::snit::widget btab {
#
# PURPOSE:  
#
#/////////////////////////////////////////////////////////////////////////////
# SPECIFIC OPTIONS
# -label      -> Label that will be shown in the tabs
# -createcmd  -> Function that creates the widget in each tab.
#                Must have a parameter called path, where the widget will be
#                created.
# BOTH OPTIONS MUST BE SPECIFIED IN CREATION TIME
# 
# VARIABLES
# created     -> Created flag
# widget      -> Contains path of the widgets of the component
# data        -> Contains data  of the component
# counter     -> Counter to create tabs
#                              
#/////////////////////////////////////////////////////////////////////////////
  widgetclass Btab
  
  option -label        {}
  option -createcmd    {}

  variable created
  variable widget
  variable data
  variable counter     0

  #///////////////////////////////////////////////////////////////////////////
  constructor {args} {
  #
  # PURPOSE: btab constructor
  #
  # PARAMETERS:
  #   args ->  list of arguments for btab
  #
  #//////////////////////////////////////////////////////////////////////////
    $self configure -label      [from args -label ""]
    $self configure -createcmd  [from args -createcmd ""]
    # Apply all arguments given:
#    $self configurelist $args    

    # Create the widget
    $self createwidget $args

    # Widget is now created
    set created 1 
  }

  #///////////////////////////////////////////////////////////////////////////
  method createwidget { args } {
  #
  # PURPOSE: Creates widget of the component
  #
  # PARAMETERS:
  #   args -> List with the options for the component.
  #
  #///////////////////////////////////////////////////////////////////////////
    set widget(fr) [frame $self.f]
#    set widget(sb) [$self _CreateScrollBar]
    set widget(ts) [$self _CreateTabSet]
    set widget(bt) [$self _CreateButtons]
    
#    $widget(sb) configure -command [list $widget(ts) view]
#    $widget(ts) configure -scrollcommand [list $widget(sb) set] \
#                           -scrollincrement 20

    $self insertTab

#     pack $widget(sb) $widget(bt)
    pack $widget(bt)

    grid $widget(fr) -column 0 -row 0 -sticky nw
    grid $widget(ts) -column 1 -row 0 -sticky news
    grid columnconfigure $self 0 -weight 0
    grid columnconfigure $self 1 -weight 1
    grid rowconfigure    $self 0 -weight 1
  }

  #///////////////////////////////////////////////////////////////////////////
  method _CreateScrollBar { } {
  #
  # PURPOSE: Crea la barra de desplazamiento
  #
  #///////////////////////////////////////////////////////////////////////////
    set sb [scrollbar $self.f.sb -orient horizontal]
    return $sb
  }

  #///////////////////////////////////////////////////////////////////////////
  method _CreateTabSet { } {
  #
  # PURPOSE: Crea el tabset
  #
  #///////////////////////////////////////////////////////////////////////////
    set ts [::blt::tabset $self.ts -relief flat \
      -highlightthickness 0 -bd 0 -outerpad 0 -slant right \
      -textside right -scrollincrement 20]
    return $ts
  }

  #///////////////////////////////////////////////////////////////////////////
  method _CreateButtons { } {
  #
  # PURPOSE: Crea la barra de botones
  #
  #///////////////////////////////////////////////////////////////////////////
#     set TF [::toolbar::ToolbarFrame $self.f.toolbarFrame]
#     ## 2) Create a toolbar in this toolbar frame...
#     set tb1 [::toolbar::create $TF.tb1]
#     ::toolbar::addbutton $tb1 -image [::Bitmap::get reverse] \
#       -tooltip [mc "TOL Inspector"]\
#       -command [list ::TolConsole::RaiseInspector]
#     ::toolbar::addbutton $tb1 -image [::Bitmap::get play] \
#       -statuswidget $widgets(sb,path) -tooltip [mc "Connection to database"]\
#       -command [list $::project::data(bodbc) showdialog]
#     ::toolbar::addseparator $tb1
#     ::toolbar::addbutton $tb1 -image [::Bitmap::get newtab] \
#       -statuswidget $widgets(sb,path) -tooltip [mc "New TOL file"]\
#       -command [list ::Editor::New]
#     ::toolbar::addbutton $tb1 -image [::Bitmap::get closetab] \
#       -statuswidget $widgets(sb,path) -tooltip [mc "Open TOL file"]\
#       -command [list ::Editor::Open]
#     return $TF
    set bbox \
      [ButtonBox $self.f.bb -orient vertical -spacing 3 -padx 0 -pady 0 \
 	 -homogeneous false]
    $bbox add -relief link -command [list $widget(ts) view -1] \
      -helptext [mc "Scrolls tabset to the left"] -disabledforeground ""\
      -image [::Bitmap::get reverse ] -compound left -padx 1 -pady 3
    $bbox add -relief link -command [list $widget(ts) view 1] \
      -helptext [mc "Scrolls tabset to the right"] -disabledforeground ""\
      -image [::Bitmap::get play ] -compound left -padx 1 -pady 3
    $bbox add -relief link -command [list $self insertTab] \
      -helptext [mc "Inserts new tab"] -disabledforeground ""\
      -image [::Bitmap::get newtab ] -compound left -padx 1 -pady 3
    $bbox add -relief link -command [list $self deleteTab] \
      -helptext [mc "Deletes current tab"] -disabledforeground ""\
      -image [::Bitmap::get closetab] -compound left -padx 1 -pady 3

    return $bbox
  }

  #///////////////////////////////////////////////////////////////////////////
  #  PRIVATE METHODS
  #///////////////////////////////////////////////////////////////////////////
  
  #///////////////////////////////////////////////////////////////////////////
  #  PUBLIC METHODS
  #///////////////////////////////////////////////////////////////////////////

  #///////////////////////////////////////////////////////////////////////////
  method insertTab { } {
  #
  # PURPOSE: Inserts new tab at the end and creates new widget associated 
  #          with the tab.
  #
  #///////////////////////////////////////////////////////////////////////////
    $widget(ts) insert end widget$counter
    set path "$self.ts.widget$counter"
    eval [$self cget -createcmd] $path
    pack $path -fill both -expand true
    $widget(ts) tab configure widget$counter \
      -text "[mc "List"] $counter" \
      -window $path -fill both
    incr counter
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method deleteTab { } {
  #
  # PURPOSE: Deletes active tab
  #
  #///////////////////////////////////////////////////////////////////////////
    if { [$widget(ts) size] > 1 } {
      destroy $widget(ts).[$widget(ts) get select]
      $widget(ts) delete [$widget(ts) index select]
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method getSelected { {entry select}  } {
  #
  # PURPOSE: Returns path of the selected widget in the tab
  #
  #///////////////////////////////////////////////////////////////////////////
    return $widget(ts).[$widget(ts) get $entry]
  }

  #///////////////////////////////////////////////////////////////////////////
  method configureentry { entry args } {
  #
  # PURPOSE: configures widget in the given entry
  # PARAMETERS: entry -> entry index nr#, select, active...
  #             args  -> arguments to configure
  #
  #///////////////////////////////////////////////////////////////////////////
    set path "$widget(ts).[$widget(ts) get $entry]"
    eval $path configure $args
  }

  #///////////////////////////////////////////////////////////////////////////
  method configureall { args } {
  #
  # PURPOSE: configures all widgets in tabs with the given options
  # PARAMETERS:  args  -> arguments to configure
  #
  #///////////////////////////////////////////////////////////////////////////
    for {set i 0} {$i < [$widget(ts) size]} {incr i} {
      eval $self configureentry $i $args
    }
  }
}

package provide btab 1.0