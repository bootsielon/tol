# ----------------------------------------------------------------------------
# fmenu.tcl --
#
# This file implement a new widget to create a top level menu from a menu
# definition given as menudef object (see menudef.tcl). The top level menu is
# a frame containing a row of menubuttons an separators. Normaly only one
# separator is used to set the Help menu to the right side of the frame.
# ----------------------------------------------------------------------------

package require Tk 8.4
package require snit
package require struct 

snit::type menudef {
  # definition tree
  variable tdef
  delegate method = to tdef
  delegate method serialize to tdef
  delegate method entries to tdef as children
  delegate method delete to tdef
  
  typemethod validate_type {t isroot} {
    if {$isroot && [lsearch {cascade separator} $t]==-1} {
      error "invalid type \"$t\": only \"cascade\" or \"separator\" type can be used at root level in a menudef"
    }
    if {[lsearch {cascade checkbutton command radiobutton separator} \
             $t] == -1} {
      error "invalid type \"$t\": only \"cascade, checkbutton, command, radiobutton, or separator\" type can be used in a menudef"
      
    }
  }

  method clear { } {
    foreach n [$tdef children root] {
      $tdef delete $n
    }
  }

  method add {parent t args} {
    menudef validate_type $t [expr {$parent eq "root"}]
    if {[expr {[llength $args] % 2}]} {
      set entryid [lindex $args 0]
      set node_entry  [$tdef insert $parent end $entryid]
      set option_list [lrange $args 1 end]
    } else {
      set node_entry  [$tdef insert $parent end]
      set option_list $args
    }
    if { [lsearch {separator} $t]==-1 } {
    	set label [from option_list -label ""]
    	set underline [lsearch [split $label {}] &]
    	set label [string map {& ""} $label]
    	set option_list [lappend option_list -label $label -underline $underline]
    }
    $tdef set $node_entry options $option_list
    $tdef set $node_entry type $t
    return $node_entry
  }

  method insert {node_ref t args} {
    set parent [$tdef parent $node_ref]
    menudef validate_type $t [expr {$parent eq "root"}]
    set index [$tdef index $node_ref]
    if {[expr {[llength $args] % 2}]} {
      set entryid [lindex $args 0]
      set node_entry  [$tdef insert $parent $index $entryid]
      set option_list [lrange $args 1 end]
    } else {
      set node_entry  [$tdef insert $parent $index]
      set option_list $args
    }
    if { [lsearch {separator} $t]==-1 } {
    	set label [from option_list -label ""]
    	set underline [lsearch [split $label {}] &]
    	set label [string map {& ""} $label]
    	set option_list [lappend option_list -label $label -underline $underline]
    }
    $tdef set $node_entry options $option_list
    $tdef set $node_entry type $t
    return $node_entry
  }

  method get_options {entry_id} {
    $tdef get $entry_id options
  }

  method get_type {entry_id} {
    $tdef get $entry_id type
  }

  constructor {} {
    set tdef [::struct::tree]
  }
  destructor {
    $tdef destroy
  }
}

snit::method menudef dump {} {
  $tdef walk root n {
    set info $n
    if {$n ne "root"} {
      append info " =  [$tdef get $n type] [$tdef get $n options]"
    }
    #puts $info
  }
}

snit::widget fmenu {
  delegate method * to hull
  delegate option * to hull
  option -definition ""

  variable allTops {}
  
  onconfigure -definition {newdef} {
    if {$options(-definition) eq ""} {
      set options(-definition) [menudef create %AUTO%]
    }
    if {$newdef ne ""} {
      $options(-definition) = $newdef
    } else {
      $options(-definition) clear
    }
    $self draw
  }

  method draw {} {
    set c 1  
    foreach mb $allTops {
      destroy $mb
    }
    set allTops {}
    set def $options(-definition)
    foreach mb [$def entries root] {
      if {[$def get_type $mb] eq "separator"} {
        grid columnconfigure $win $c -weight 1 
      } else {
        set mbopt(-label) ""
        set lopt [$def get_options $mb]
        set label [from lopt -label ""]
        set underline [from lopt -underline -1]
        set post [from lopt -postcommand ""]
        if {0 && [llength $lopt]} {
          #puts "mbopt = $lopt"
          array set mbopt $lopt
        }
        #puts $lopt
        if {[llength $label] > 1} {
          set label [list $label]
        }
        eval menubutton $win.mb$c -text $label \
            -menu $win.mb$c.menu -underline $underline $lopt
        bind $win.mb$c <Enter> [list $win.mb$c configure -relief raised]
        bind $win.mb$c <Leave> [list $win.mb$c configure -relief flat]
        bind $win.mb$c <ButtonPress-1> [list $win.mb$c configure -relief flat]
        if {$post eq ""} {
          $self drawsub $mb [menu $win.mb$c.menu -tearoff 0]
        } else {
          set post [string map "%W $win.mb$c.menu" $post]
          menu $win.mb$c.menu -postcommand $post -tearoff 0
        }
        grid $win.mb$c -row 0 -column $c -sticky "nw"
		grid columnconfigure $win $c -weight 0
        lappend allTops $win.mb$c
      }
      incr c
    }
  }

  method drawsub {menuid m} {
    set def $options(-definition)
    #puts "a crear los hijos de $menuid [$def get_options $menuid]"
    set i 0    
    foreach entry [$def entries $menuid] {
      set entry_type [$def get_type $entry]
      set entry_opt [$def get_options $entry]
      set post [from entry_opt -postcommand ""]
      if {$entry_type eq "cascade"} {
        menu $m.m$i -tearoff 0
        eval $m add cascade -menu $m.m$i $entry_opt
        if {$post eq ""} {
          $self drawsub $entry $m.m$i
        } else {
          set post [string map "%W $m.m$i" $post]
          $m.m$i configure -postcommand $post
        }
      } else {
        eval $m add $entry_type $entry_opt
      }
      incr i
    }
  }
  
  
  constructor {args} {
    set def [from args -definition]
    $self configure -definition $def
    $self configurelist $args
  }
}


package provide fmenu 0.1

# ------- TEST CODE ----

proc do_post {m} {
  $m delete 0 end
  $m add command -label "Subentry 1"
}

proc test_fmenu { } {
  set md [menudef create %AUTO%]
  set id [$md add root cascade -label "&File"]
  $md add $id command -label "&Open ..."
  $md add $id command -label "&Close"
  $md add $id separator
  $md add $id command -label "&Exit"
  set id [$md add root cascade -label "F&oo"]
  $md add $id command -label "&Bar"
  $md add $id separator
  $md add $id command -label "B&e Be"
  $md add $id cascade -label "&Postcommand" -postcommand "do_post %W"
  $md add root separator
  set id [$md add root cascade -label "&Help"]
  $md add $id command -label "&Contents ..."
  $md add $id separator
  $md add $id command -label "&About ..."  

  #$md dump 
  #return
  if {[winfo exists .tmf]} {
    .tmf.m configure -definition $md
  } else {
    toplevel .tmf
    fmenu .tmf.m -definition $md
    grid .tmf.m -row 0 -column 0 -sticky new
    grid columnconfigure .tmf 0 -weight 1
    grid rowconfigure .tmf 1 -weight 1
  }
  $md destroy
}

#test_fmenu
