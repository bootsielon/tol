package require BWidget
package require wtree
package require msgcat

namespace eval ::TolConfigGUI {

  namespace import ::msgcat::*

  variable win
  variable tree
}

proc ::TolConfigGUI::GetFullName { member parent } {
  return [ expr { $parent eq "" ? "$member" : "${parent}::$member" } ]
}

proc ::TolConfigGUI::CreateSectionNode { section secParent dictConfig } {
  variable tree
  variable treeInfo

  set fullSection [ GetFullName $section $secParent ]
  set nid [ $tree insert [ list [ list "" [ mc $section ] ] [ list "" ] ] \
               -at end -relative $treeInfo(node,$secParent) -button auto ]
  set treeInfo(node,$fullSection) $nid

  array set config $dictConfig
  foreach _n [ array names config ] {
    if { [ regexp _.autodoc.member.(.+) $_n ==> n ] } {
      FillTree $n $fullSection $config($n)
    }
  }
  return $fullSection
}

proc ::TolConfigGUI::CreateMemberNode { member secParent value } {
 variable tree
  variable treeInfo

  set full [ GetFullName $member $secParent ]
  set nid [ $tree insert [ list [ list "" [ mc $member ] ] [ list $value ] ] \
               -at end -relative $treeInfo(node,$secParent) -button auto ]
  set treeInfo(node,$full) $nid
  set treeInfo(value,$full) $value

  return $full
}

proc ::TolConfigGUI::FillTree { member secParent value } {
  variable win
  variable treeInfo

 
  set full [ GetFullName $member $secParent ]
  set type [ ::TolConfig::GetType $full ]
  if { $type eq "section" } {
    CreateSectionNode $member $secParent $value
  } else {
    CreateMemberNode $member $secParent $value
  }
}

proc ::TolConfigGUI::FillTreeConfig { } {
  variable win
  variable tree
  variable treeInfo

  array unset treeInfo
  set treeInfo(node,) root
  $tree item delete all
  FillTree TolConfigManager::Config "" [ ::TolConfig::GetConfig ]

}

proc ::TolConfigGUI::AcceptConfig { } {
  puts AcceptConfig
}

proc ::TolConfigGUI::Show { } {
  variable win
  variable tree

  set win .tolconfig
  if { ![ winfo exists $win ] } {
    Dialog $win -title [ mc "Tol configuration"] \
        -modal local
    $win add -text Ok
    $win add -text Cancel
    
    set f [ $win getframe ]
    grid rowconfigure $f 0 -weight 1
    grid columnconfigure $f 0 -weight 1
    set pw [ PanedWindow $f.pw -side bottom -pad 0 -width 4 ]
    set paneLeft  [ $pwMain add ]
    set paneRight [ $pwMain add ]

    set tree $f.tree
    wtree  $tree
    $tree configure -table no -filter yes -columns {
      { {image text} -tags NAME -label "Name" }
      { {text} -tags STATUS -label "Value" }
    }
    grid $tree -row 0 -column 0 -sticky "snew"
    grid rowconfigure $win 1 -weight 1
    grid columnconfigure $win 0 -weight 1
  }
  FillTreeConfig
  set ans [ $win draw ]
  if { $ans == 0 } {
    AcceptConfig
  }
}
