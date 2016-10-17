#/////////////////////////////////////////////////////////////////////////////
# FILE    : blistorder.tcl
# PURPOSE : Component that adds order funcionality to blistboxplus
#/////////////////////////////////////////////////////////////////////////////

package require byscommon
package require blistboxplus

#/////////////////////////////////////////////////////////////////////////////
::snit::widget blistorder {
#
# PURPOSE: Listboxplus with order funcionality
#
# variables
#  list -> blistboxplus
#  bbox -> buttonbox
#  created -> indicates if the widget has been fully created
#  
#/////////////////////////////////////////////////////////////////////////////
  widgetclass Blistorder
  
  variable _list
  variable bbox
  variable created 0

  #///////////////////////////////////////////////////////////////////////////
  constructor { args } {
  #
  # PURPOSE: blistorder's constructor. Creates an instance of blistorder
  #
  # PARAMETERS:
  #   args -> Arguments for the component
  #
  #///////////////////////////////////////////////////////////////////////////
    # Create the widget
    $self createwidget $args

    # Apply all arguments given:
    $self configurelist $args
  }

  #///////////////////////////////////////////////////////////////////////////
  method createwidget { args } {
  #
  # PURPOSE: Creation of the widget
  #
  # PARAMETERS:
  #   args -> Arguments for the component
  #
  #///////////////////////////////////////////////////////////////////////////
    # Widget that contains blistboxplus
    if {0} {
      install list using blistboxplus $self.lb
    } else {
      set _list [blistboxplus $win.lb]
    }

    # Widget that contains buttonbox
    install bbox using ButtonBox $win.bb -orient vertical -spacing 0 \
                                          -padx 4 -pady 1

    $bbox add -image [Bitmap::get upNext] -height 16 -width 16 \
      -command [list $self _MoveUp] -relief link \
      -helptext [mc "Moves selection up"]
    $bbox add -image [Bitmap::get downNext] -height 16 -width 16 \
      -command [list $self _MoveDown] -relief link \
      -helptext [mc "Moves selection down"]

    grid $win.lb $bbox -sticky news
    grid rowconfigure    $self 0 -weight 1
    grid columnconfigure $self 0 -weight 1 
    grid columnconfigure $self 1 -pad 5
    update
    
    #The widget is created
    set created 1
    
    return $self
  }

  #///////////////////////////////////////////////////////////////////////////
    method _MoveUp {} {
  # 
  # PURPOSE: Moves selected items in listboxplus up
  #
  #///////////////////////////////////////////////////////////////////////////
    set indexes [$self curselection]
    if {$indexes != ""} {
      set first [lindex $indexes 0]
      set last [lindex $indexes end]      
      set start [$self first]
      if { $first != $start } {
        $self move [lindex [$self range [$self first] $first] end-1] \
               after $last
        $self selection clearall
        $self selection set $first $last
      }
    }
  }

  #///////////////////////////////////////////////////////////////////////////
    method _MoveDown {} {
  # 
  # PURPOSE: Moves selected items in listboxplus down
  #
  #///////////////////////////////////////////////////////////////////////////
    set indexes [$self curselection]
    if {$indexes != ""} {
      set first [lindex $indexes 0]
      set last [lindex $indexes end]      
      set end [$self last]
      if { $last != $end } {
        $self move [lindex [$self find $last] 1] before $first
        $self selection clearall
        $self selection set $first $last
      }    
    }
  }
  # methods and options delegation
  delegate method * to _list
  delegate option * to _list
}

package provide blistorder 1.0

  
# TEST CODE

#/////////////////////////////////////////////////////////////////////////////
proc test_blistorder {} {
#
# PURPOSE: Function that test blistorder
#
#/////////////////////////////////////////////////////////////////////////////
  
  # defining the widgets
  variable data
  set data(-selected) 1
  
  destroy .top
  toplevel .top -width 200 -height 100 
    
  TitleFrame .top.f1 -text "Blistorder test"
  set f1 [ .top.f1 getframe ]
  blistorder $f1.blb -cols { col1 col2 col3 col4 } -parent $f1 \
      -height 150 -width 500 -orientation vertical \
      -hiddencols { col1 col2 }
  $f1.blb insert end {1 2 3 4} {one two three four} {uno dos tres cuatro} \
                     {k kk kkk kkkk} {0 0 0 0} {@ # $ %}
  TitleFrame .top.f2 -text "Configure"
  set f2 [ .top.f2 getframe ]
  LabelEntry $f2.lei -width 5 -label Index -text 1
  LabelEntry $f2.lee -width 5 -label End
  LabelEntry $f2.lec -width 50 -label Content -text "{hay que introducir cada} \
     {fila entre corchetes y} {entre estos los items}"
  checkbutton $f2.cb -text "Allow multiple selection" \
    -command { toggleSelectModeCmd }
  $f2.cb select
  button $f2.bi -text "Insert" -command { insertCmd }
  button $f2.bd -text "Delete" -command { deleteCmd }
  button $f2.br -text "Reset" -command { resetCmd }
  $f2.bi configure -width 20
  $f2.bd configure -width 20

  # packing all 
  
  grid .top.f1 -row 0 -column 0 -sticky "new" -padx 10 -pady 10
  grid .top.f2 -row 1 -column 0 -sticky "new" -padx 10 -pady 10
  grid columnconfigure .top 0 -weight 1
  grid rowconfigure .top 0 -weight 0
  grid rowconfigure .top 1 -weight 1
  
  grid $f1.blb -row 0 -column 0 -sticky "new"
  grid columnconfigure $f1 0 -weight 1
  grid rowconfigure $f1 0 -weight 1

  grid $f2.lec -row 0 -column 0 -sticky "new" -columnspan 3
  grid $f2.lei -row 1 -column 0 -sticky "new"
  grid $f2.lee -row 1 -column 1 -sticky "new"
    
  grid $f2.cb  -row 1 -column 2 -sticky "new"
  grid $f2.bi  -row 2 -column 0 -sticky "new"
  grid $f2.bd  -row 2 -column 1 -sticky "new"
  grid $f2.br  -row 2 -column 2 -sticky "new"
  grid columnconfigure $f2 0 -weight 0
  grid columnconfigure $f2 1 -weight 0
  grid columnconfigure $f2 2 -weight 1
  grid rowconfigure $f2 0 -weight 0
  grid rowconfigure $f2 1 -weight 0
  grid rowconfigure $f2 2 -weight 0

  puts "WIDGET blistoder: $f1.blb"
  
  # commands attached to the widgets 
  proc insertCmd {} {
    eval .top.f1.f.blb insert [.top.f2.f.lei cget -text] \
                              [.top.f2.f.lec cget -text]
  }
  
  proc deleteCmd {} {
    .top.f1.f.blb delete [.top.f2.f.lei cget -text] \
                         [.top.f2.f.lee cget -text]
  }
  
  proc resetCmd {} {
    .top.f1.f.blb clear
    .top.f1.f.blb insert end {1 2 3 4} {one two three four} \
	  {uno dos tres cuatro} {k kk kkk kkkk} {0 0 0 0} {@ # $ %}
  }
  
  proc toggleSelectModeCmd {} {
    if { [.top.f1.f.blb cget -selectmode] eq "multiple" } {
	  .top.f1.f.blb configure -selectmode single
    } else {
	  .top.f1.f.blb configure -selectmode multiple
    }
  }
}
