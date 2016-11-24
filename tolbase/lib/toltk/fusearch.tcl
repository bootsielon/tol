#/////////////////////////////////////////////////////////////////////////////
# FILE    : fusearch.tcl
# PURPOSE : This file contains a namespace called funcSearch with its
#           functions. It's for creating a form that allows to look for a
#           function giving some parameters: name, grammar, parameters,
#           description, etc.
#/////////////////////////////////////////////////////////////////////////////


#/////////////////////////////////////////////////////////////////////////////
# TESTING
#/////////////////////////////////////////////////////////////////////////////
# Put if {1} for testing this file.
if {0} {
  package require Tk
  package require BWidget
  # force BWidget autoload
  Bitmap::use
  set toltk_script_path [file dirname [info script]]
  set toltk_images_path [file join ${toltk_script_path} images]
  if { [lsearch $Bitmap::path $toltk_images_path] == -1 } {
    lappend Bitmap::path [file join $toltk_script_path images]
  }
  package require msgcat
  namespace import msgcat::*
  ::msgcat::mclocale es
  ::msgcat::mcload [file join $toltk_script_path msgs]
  
  source [file join $toltk_script_path "mclistbox.tcl"]
  namespace import ::mclistbox::*
}

package require blistboxplus
package require beditselector


#/////////////////////////////////////////////////////////////////////////////
namespace eval funcSearch {
#
# PURPOSE: The purpose of this namespace is organize code and functions.
#          It also stotres a special namespace is created for each instance 
#          of funcSearch.
# VARIABLES:
#   tmpOpt -> array with information functions search
#     grammars,PRO -> procedure created for grammar editSelector
#     var,grammars -> grammars of searched function
#     var,name     -> name's substring of searched function
#     var,numPars  -> numbers of params of searched function
#     var,desc     -> description's substring of searched function
#     var,par$i,grammar -> grammar of any param of searched function
#     var,par$i,name    -> name's substring of any param of searched function
#     widget,res   -> mclistbox path where results are shown
#   iniconfig -> name of funcSearch in ini file
#   widget -> contains the widget
#     widget(fr) -> Main frame of the document
#   sensitive -> has a value 1 if the user has selected case sensitive search
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpOpt
  variable widget
  variable sensitive
  variable iniconfig
}


#/////////////////////////////////////////////////////////////////////////////
proc ::funcSearch::Show {} {
#
# PURPOSE: Shows the function search dialog
#
#/////////////////////////////////////////////////////////////////////////////
  variable widget   
  if { [info exist widget(document)] && [winfo exists $widget(document)] } {
    if { [$widget(document) cget -state] eq "minimized" } {
      $widget(document) restore
    }
    $widget(document) raise
  } else {
    Create
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::funcSearch::Create {} {
#
# PURPOSE: Creates the function search dialog
#
# PARAMETERS:
#   path: path of dialgo's parent
#
#/////////////////////////////////////////////////////////////////////////////
  global toltk_options
  variable tmpOpt
  variable widget
  variable sensitive
  variable iniconfig
  
  # Name of function search's section in ini file
  set iniconfig "FunctionSearch"
    
  # Create the widget 
  set widget(document) [::project::CreateForm \
    -iniconfig $iniconfig -title [mc "Functions Search"] \
    -helpkey "Tb4Fus" -closecmd [list ::funcSearch::Destroy]]
  set widget(fr) [$widget(document) getframe]
            
  label $widget(fr).limg -image [::Bitmap::get search]
  label $widget(fr).lfind -text [mc "Search functions"]... \
                 -font $toltk_options(fonts,Title1)
    
  # Obtain the complete list of grammars
  set lst [::tol::info grammars]
  set txt [mc "of grammars"]:
  # Beditselector of grammars
  set tmpOpt(grammars,PRO) [beditselector $widget(fr).bes -label $txt \
                -list $lst -width 30 -readonly false -readonly true ]
  set lgra [$tmpOpt(grammars,PRO) get label]
  set egra [$tmpOpt(grammars,PRO) get entry]
  set bgra [$tmpOpt(grammars,PRO) get button]
    
  # Criteria search by name
  label $widget(fr).lname -text [mc "whose name contains"]:
  entry $widget(fr).ename -width 18 -textvariable ::funcSearch::tmpOpt(var,name)
  bind  $widget(fr).ename <Return> [list ::funcSearch::Search]
  bind  $widget(fr).ename <Control-g> [list ::funcSearch::Search]

  # Criteria search by description    
  label $widget(fr).ldesc -text [mc "whose description contains"]:
  entry $widget(fr).edesc -width 18 -textvariable ::funcSearch::tmpOpt(var,desc)
  bind  $widget(fr).edesc <Return> [list ::funcSearch::Search ] 
  bind  $widget(fr).edesc <Control-g> [list ::funcSearch::Search ]

  # Case sensitive search  
  checkbutton $widget(fr).cs -text [mc "Case sensitive"] \
                    -variable ::funcSearch::sensitive

  # Scrolled window for criteria search by params. NOT IMPLEMENTED
if {0} {  
  set sw [ScrolledWindow $widget(fr).sw]
  #set sf [ScrollableFrame $w.sw.fr -constrainedwidth 1 -height 80 -width 560]
  set sf [ScrollableFrame $widget(fr).sw.fr -constrainedwidth 0 -height 80 -width 560]
  $sw setwidget $sf
  set fr [$sf getframe]    
  label $widget(fr).lpars -text [mc "and with at least"]
  frame $widget(fr).fg
  set widget(spinbox) [SpinBox $widget(fr).fg.sbpars -range {0 8} -width 4 \
          -justify right -editable false\
          -textvariable ::funcSearch::tmpOpt(var,numPars) \
          -modifycmd [list ::funcSearch::CheckPar $fr]]
  label $widget(fr).fg.lpars -text [mc "parameters"]
  #grid $w.fg.sbpars $w.fg.lpars -sticky nw -padx 2
}  
  # Button box with
  set widget(bbox) \
          [ButtonBox $widget(fr).bb -orient horizontal -spacing 10 \
           -padx 0 -pady 0 -homogeneous false]
  $widget(bbox) add -text [mc "Find"] -relief link \
          -helptext "[mc "Find"] (Ctrl+G)" -image [::Bitmap::get bfind] \
          -compound left -padx 5 -pady 1 -command [list ::funcSearch::Search]
  $widget(bbox) add  -relief link -helptext [mc "Cancel"] \
          -image [::Bitmap::get cancelx] -text [mc Cancel] \
          -compound left -padx 5 -pady 1 -command [list ::funcSearch::Destroy]

  Separator $widget(fr).sep -orient horizontal -relief ridge
  
  # Grid of results
  set f [frame $widget(fr).fres -bd 2]
    
  scrollbar $f.sy -command "$f.mclbres yview"
  scrollbar $f.sx -orient horizontal -command "$f.mclbres xview"
  set name        [mc "Name"]
  set content     [mc "Content"]
  set path        [mc "Path"]
  set description [mc "Description"]      
    
  set tmpOpt(widget,res) [blistboxplus $f.mclbres \
      -cols "$name $content $path $description"\
      -background white -yscrollcommand "$f.sy set" \
      -xscrollcommand "$f.sx set" -selectmode multiple -background white \
      -highlightthickness 0 -borderwidth 2 -linewidth 2 \
      -exportselection false -border 0 -height 0 -readonly true \
      -colkeypress $name]

  set lb $tmpOpt(widget,res)
  set lbMenu [$lb getMenu]
  
  $lbMenu insert 0 separator
  $lbMenu insert 0 command -label [mc "View definition"] \
            -command  [list ::funcSearch::ViewDefinition] -accelerator CTRL+W
          
  # binds of listboxplus
  #@ anteriormente era una llamada a un método bind
  bind $tmpOpt(widget,res) <Control-KeyRelease> \
    +[list ::funcSearch::OnControlKey $tmpOpt(widget,res) %K %k]
  
  # blisboxplus auto-adjust width of columns without configured width
  #$lb column configure [mc "Name"]        -width [expr 18*7]
  #$lb column configure [mc "Content"]     -width [expr 35*7]
  $lb column configure [mc "Path"]        -width [expr 15*7]
    
  grid $f.mclbres      -sticky news
   
  grid columnconfigure $f 0 -weight 1
  grid rowconfigure    $f 0 -weight 1

  frame $widget(fr).f                                                            -width 25
  grid  $widget(fr).limg $widget(fr).lfind -                 -                 - -sticky nw -pady 5
  grid  ^                $widget(fr).f     $widget(fr).bes   -                 - -sticky nw -pady 0 -padx 1
  grid  ^                ^                 $widget(fr).lname $widget(fr).ename - -sticky nw -pady 1
  grid  ^                ^                 $widget(fr).ldesc $widget(fr).edesc - -sticky nw -pady 1
  grid  ^                ^                 $widget(fr).cs    -                 - -sticky nw -pady 1
  grid  ^                ^                 $widget(bbox)     -                 - -sticky nws
  grid  $widget(fr).sep  -                 -                 -                 - -sticky new -pady 5
  grid  $widget(fr).fres -                 -                 -                 - -sticky news  

  grid columnconfigure $widget(fr) 4 -weight 1
  grid rowconfigure    $widget(fr) 7 -weight 1
  
  # establish initial focus
  focus $widget(fr).ename
  
  ::funcSearch::ReadIni        
}


#/////////////////////////////////////////////////////////////////////////////
proc ::funcSearch::OnControlKey {_lb keysym keycode } {
#
# PURPOSE: Control of the binds of accelerator keys of listbox
#
# PARAMETERS: _lb -> self
#             keysym -> The keysym corresponding to the event, 
#                       substituted as a textual string
#             keycode -> The keycode field from the event
#
#/////////////////////////////////////////////////////////////////////////////
  switch [string toupper $keysym] {
    G {
      ::funcSearch::Search
      }
    W {
      ::funcSearch::ViewDefinition
    }
  }
}  

#/////////////////////////////////////////////////////////////////////////////
proc ::funcSearch::ViewDefinition { } {
#
# PURPOSE: Show the file and function selected on results listbox. It works
#          only when an unique function has been selected.
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpOpt

  set lb $tmpOpt(widget,res)
  set lbMenu [$lb getMenu]
  # rows selected
  set lbrow [$lb selection get]
  # verifies that only a column has been selected
  if {[llength $lbrow] == 1 } { 
    set selected [$lb get $lbrow]    ;# get his content    
    set objName [lindex $selected 0] ;# name of function
    set path [lindex $selected 2]    ;# path of function    
    # verifies if that function has path
    if {$path != ""} {
      # the grammar is the first element of column content
      set grammar [lindex [lindex $selected 1] 0] 
      # call to ViewDefinition
      ::TolInspector::ViewDefinition $objName $path $grammar
    } else {
      tk_messageBox -type ok -icon warning\
        -message [mc "Object %1\$s is not defined in a tol file" $objName]\
        -parent $tmpOpt(widget,res) -title [mc Warning]
    }
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::funcSearch::Search {} {
#
# PURPOSE: Looks for all functions that matches the criteriums introduced in
#          search dialog, shows the results and hides the dialog.
#
# PARAMETERS:
#    dialog: Functions search  path
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpOpt
  variable sensitive
  variable widget
  
  set search 0
  set tmpOpt(var,grammars) [$tmpOpt(grammars,PRO) cget -sel] 

  # Verify that the search can be made
  if { $tmpOpt(var,grammars) == "" } { 
    set search 1 ;# There isn't grammars where search
  }
  if { ($tmpOpt(var,name) == "") && ($tmpOpt(var,desc) == "") } {
    set search 2 ;# There aren't sufficient criteria search
  }
  if { [ catch { regexp "(.*)$tmpOpt(var,name)(.*)" "dummysrting" } ] } {
     set search 3 ;# Criteria search for the name are incorrect
  }
  if { [ catch { regexp "(.*)$tmpOpt(var,desc)(.*)" "dummysrting" } ] } {
     set search 4 ;# Criteria search for the description are incorrect
  }  
  if {$search == 0} {  ;# The search can be made
    . configure -cursor watch
    update
    if {$sensitive} { ;# Case sensitive search
      # Verify grammars and names of functions
      foreach gra $tmpOpt(var,grammars) {      
        set result($gra) [::tol::info functions $gra]
        foreach f $result($gra) {
          if {![regexp "(.*)$tmpOpt(var,name)(.*)" $f]} {                
            set pos [lsearch $result($gra) $f]
            set result($gra) [lreplace $result($gra) $pos $pos]
          }
        }
      }    
      # Verify if the functions are in the included files     
      set resultIncluded [::tol::info var Code] 
      foreach f $resultIncluded {
        set fU [string toupper $f]
        if {![regexp "(.*)$tmpOpt(var,name)(.*)" $f]} {
          set pos [lsearch $resultIncluded $f]
          set resultIncluded [lreplace $resultIncluded $pos $pos]
        }
      }
      # discard duplicated functions
      set resultIncludedNoDup ""
      foreach func $resultIncluded {
        set dup 0 ;# no duplicated
        foreach gra $tmpOpt(var,grammars) {
          if { [lsearch -exact $result($gra) $func] != -1 } {
            set dup 1
          }
        }
        if {$dup == 0} { ;# it is inserted if it isn't duplicate
          lappend resultIncludedNoDup $func
        }
      }
      # Look for in the description and I keep name, content, path 
      # and description
      set funcs ""
      foreach gra [array names result] {
        foreach f $result($gra) {
          set inf [TclInfoFun $gra $f]
          # Verify description
          if {[regexp "(.*)$tmpOpt(var,desc)(.*)" [lindex $inf 3]]} {
            lappend funcs \
              [list $f [lindex $inf 5] [lindex $inf 1] [lindex $inf 3]]
          }
        }
      }
      # For the functions on inluded files, search in the description, 
      # I verify the type of the function, and keep name, content, path  
      # and description
      set funcsIncluded ""
      foreach f $resultIncludedNoDup {       
        set inf [::tol::info var [list Code $f]]
		#puts "funcSearch::Search: inf = $inf"
        if { [ContentCodeGrammar [array names result] [lindex $inf 2] ] } {
          if {[regexp "(.*)$tmpOpt(var,desc)(.*)" [lindex $inf 4]]} {
            lappend funcsIncluded \
              [list $f [lindex $inf 2] [lindex $inf 3] [lindex $inf 4]]
          }
        }
      }
    # No case sensitive search
    } else { 
      set tU  "[string toupper $tmpOpt(var,grammars) ]"
      # Verify grammars and names of functions
      foreach gra $tmpOpt(var,grammars) {     
        set result($gra) [::tol::info functions $gra]
        foreach f $result($gra) {
          set rU [string toupper $result($gra)]
          set fU [string toupper $f]
          set tU [string toupper $tmpOpt(var,name)]
          if {![regexp "(.*)${tU}(.*)" $fU]} {
            set pos [lsearch $result($gra) $f]
            set result($gra) [lreplace $result($gra) $pos $pos]
          }
        }
      }
      
      # Verify if the functions are in the included archives     
      set resultIncluded [::tol::info var Code]   
      foreach f $resultIncluded {
        set fU [string toupper $f]
        if {![regexp "(.*)${tU}(.*)" $fU]} {
          set pos [lsearch $resultIncluded $f]
          set resultIncluded [lreplace $resultIncluded $pos $pos]
        }
      }
      # discard duplicated functions
      set resultIncludedNoDup ""
      foreach func $resultIncluded {
        set dup 0 ;# no duplicated
        foreach gra $tmpOpt(var,grammars) {
          if { [lsearch -exact $result($gra) $func] != -1 } {
            set dup 1
          }
        }
        if {$dup == 0} { ;# it is inserted if it isn't duplicate
          lappend resultIncludedNoDup $func
        }
      }
      # Look for in the description and I keep name, content, path 
      # and description
      set funcs ""
      foreach gra [array names result] {
        foreach f $result($gra) {             
          set inf [TclInfoFun $gra $f]          
          set iU [string toupper $inf]
          set tU [string toupper $tmpOpt(var,desc)]
          if {[regexp "(.*)${tU}(.*)" [lindex $iU 3]]} {
            #Tolcon_Trace "Content: [lindex $inf 0]"
            lappend funcs \
              [list $f [lindex $inf 5] [lindex $inf 1] [lindex $inf 3]]
          }
        }
      }
      # For the functions on inluded files, search in the description, 
      # I verify the type of the function, and keep name, content, path  
      # and description
      set funcsIncluded ""
      foreach f $resultIncludedNoDup {      
        set inf [::tol::info var [list Code $f]]
		#puts "funcSearch::Search: inf = $inf"
        set iU [string toupper $inf]
        set tU [string toupper $tmpOpt(var,desc)]
        if { [ContentCodeGrammar [array names result] [lindex $inf 2] ] } {
          if {[regexp "(.*)${tU}(.*)" [lindex $iU 4]]} {
            lappend funcs \
              [list $f [lindex $inf 2] [lindex $inf 3] [lindex $inf 4]]
          }
        }  
      }
    }
    . configure -cursor ""
    update
    set some [ShowResults $funcs $funcsIncluded]
    if { $some eq 0 } {
      tk_messageBox -type ok -icon info\
        -message [mc "Has not been any coincident result"]\
        -parent $tmpOpt(widget,res) -title [mc Message]
    }
  } else {
    # Show the error messages
    switch -exact -- $search { 
    1 { 
      tk_messageBox -type ok -icon warning\
        -message [mc "There isn't grammars where search"]\
        -parent $tmpOpt(widget,res) -title [mc Warning]
      }
    2 { 
      tk_messageBox -type ok -icon warning\
        -message [mc "There aren't sufficient criteria search"]\
        -parent $tmpOpt(widget,res) -title [mc Warning]
      }
    3 { 
      tk_messageBox -type ok -icon warning\
        -message [mc "Criteria search for the name are incorrect"]\
        -parent $tmpOpt(widget,res) -title [mc Warning]
      }
    4 { 
      tk_messageBox -type ok -icon warning\
        -message [mc "Criteria search for the description are incorrect"]\
        -parent $tmpOpt(widget,res) -title [mc Warning]
      }
    }
  }
}



#/////////////////////////////////////////////////////////////////////////////
proc ::funcSearch::ContentCodeGrammar {gr content} {
#
# PURPOSE: Return if a description of a grammar is of the type indicated 
#          in a list of grammars
#
# PARAMETERS:
#   gr : List of grammars
#   content: Description of function. His first word will be the grammar.
#          
# RETURN: Return 1 if the first word in content is of the list of grammar.
#
#/////////////////////////////////////////////////////////////////////////////
  set hasit 0
  set grammarContent [lindex $content 0]
  foreach g $gr {
    if {$g == $grammarContent} {
      set hasit 1
    }
  }
  return $hasit
}


#/////////////////////////////////////////////////////////////////////////////
proc ::funcSearch::ShowResults {funcs funcsIncluded} {
#
# PURPOSE:
#
# PARAMETERS:
#   funcs: List of functions of Tol grammars that match the search criteriums
#   funcsIncluded: List of functions of included files that match the search 
#                  criteriums
# RETURN: 1 if has been some result, 0 in opposite case
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpOpt        
  set some 0
  set lb $tmpOpt(widget,res)   
  $lb clear
  foreach funInfo $funcs {
    $lb insert end $funInfo
    set some 1
  }    
  foreach funInfo $funcsIncluded {
    $lb insert end $funInfo
    set some 1
  }    
  return $some
}


#/////////////////////////////////////////////////////////////////////////////
proc ::funcSearch::Destroy {} {
#
# PURPOSE: Hides functions search document.
#
#/////////////////////////////////////////////////////////////////////////////
  variable widget
  variable tmpOpt
  ::funcSearch::WriteIni
  array unset tmpOpt
  destroy $widget(document)
  array unset widget
  return 1
}


#/////////////////////////////////////////////////////////////////////////////
proc ::funcSearch::CheckPar {f} {
#
# PURPOSE: Function that checks that a certain fuction has at least a number 
#          of parameters. NOT IMPLEMENTED.
#
# PARAMETERS:
#   f: Function to evaluate.
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpOpt
  variable widget    
    
  set lst [::tol::info grammars]
  #puts "lst: $lst"
  set num $tmpOpt(var,numPars)
  #puts "num: $num"
  # NO funciona correctamente
  #  for {set i 1} {$i <= [expr $num+1]} {incr i} {
  #    destroy $f.f$i $f.lt$i $f.lb$i $f.lc$i $f.e$i
  #  }
  # destruimos los elementos hasta el maximo de elementos que pueda contener
  #puts " rango: [lindex [$widget(spinbox) cget -range] 1]"
  for {set i 1} {$i <= [lindex [$widget(spinbox) cget -range] 1]} {incr i} {
      destroy $f.f$i $f.lt$i $f.lb$i $f.lc$i $f.e$i
  }
  for {set i 1} {$i <= $num} {incr i} {
    frame    $f.f$i -width 15
    label    $f.lt$i -text [mc "in which one is of type"]:
    ComboBox $f.lb$i -values $lst -width 12 -height 5\
             -textvariable ::funcSearch::tmpOpt(var,par$i,grammar)
    label    $f.lc$i -text " [mc {and whose name contains}]:"
    entry    $f.e$i -width 18 -textvariable ::funcSearch::tmpOpt(var,par$i,name)
    grid $f.f$i $f.lt$i $f.lb$i $f.lc$i $f.e$i -sticky nw -pady 2
 }
 grid columnconfigure $f 4 -weight 1
}


#/////////////////////////////////////////////////////////////////////////////
proc ::funcSearch::WriteIni {} {
#
# PURPOSE: Write in ini file the grammars selected
#
#/////////////////////////////////////////////////////////////////////////////
  variable iniconfig
  variable tmpOpt
  set wini ::iniFile::Write
  $wini $iniconfig grammars [$tmpOpt(grammars,PRO) curselection]
}

#/////////////////////////////////////////////////////////////////////////////
proc ::funcSearch::ReadIni {} {
#
# PURPOSE: Read from ini file the grammars selected. If don't read any
#          selected grammar, selects all grammars by default
#
#/////////////////////////////////////////////////////////////////////////////
  variable iniconfig
  variable tmpOpt
  # read saved grammars
  set rini ::iniFile::Read
  set grammars [$rini $iniconfig grammars ""]
  if {[llength $grammars] eq 0} {
    # selects all grammars
    $tmpOpt(grammars,PRO) selectAll
  } else {
    # gets the loaded grammars
    $tmpOpt(grammars,PRO) configure -sel $grammars
  }
}


#/////////////////////////////////////////////////////////////////////////////
proc ::funcSearch::SearchWord { word } {
#
# PURPOSE: Performs a search of a word by name of functions
#
# PARAMETERS:
#  dialog ->  Mdi child of function search
#  word ->    Word to search
#
#/////////////////////////////////////////////////////////////////////////////
  variable tmpOpt
  
  set tmpOpt(var,name) $word
  ::funcSearch::Search 
}