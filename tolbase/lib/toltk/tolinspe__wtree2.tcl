
#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::ClearConsoleObj { } {
#///////////////////////////////////////////////////////////////////////////// 
  variable wt_tree

  foreach it [$wt_tree item children "root-console"] {
    $wt_tree item delete $it
  }
  #Limpiamos el stack de la consola de tol
  ::tol::console stack release
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::ClearConsoleObjSel {wt} {
#///////////////////////////////////////////////////////////////////////////// 
  variable vars_selected

  foreach var $vars_selected {
    set name [$wt item text $var Name]
    #puts "name=$name"
    #Quitamos el objeto de la consola de tol
    ::tol::console stack release $name
  }
  UpdateConsoleObj
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::UpdateFileRoot { } {
#/////////////////////////////////////////////////////////////////////////////  
  variable wt_tree
  set open 0
  set idx_file [$wt_tree item id "root-files"]
  if { [$wt_tree item isopen $idx_file] } {
    $wt_tree collapse $idx_file
    set open 1
  }
  #@! if { [$wt_tree index focus] == $idx_file } 
  if { [$wt_tree item state get $idx_file focus] } {
    SelectObject
  }
  if $open {
    $wt_tree expand $idx_file
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::UpdateConsoleObj { } {
#/////////////////////////////////////////////////////////////////////////////  
  variable wt_tree

  set idx_con [$wt_tree item id "root-console"]
  if { [$wt_tree item isopen $idx_con] } {
    $wt_tree collapse $idx_con
  }
  #@! $ht_tree selection anchor root-console
  #@! $ht_tree selection mark root-console
  $wt_tree selection clear
  $wt_tree selection add $idx_con

  #Tolcon_Trace "Abierto el nodo root-console"
  if { [$wt_tree item state get $idx_con focus] } {
    SelectObject
  }
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
  variable wt_vars
  variable wt_tree

  #User functions to contextual menu
  ::TolInspector::ReadUserFunctions

  array unset options_selected
  set all_objects {}

  foreach it [array names data_menu] {
    $data_menu($it) delete 0 end
  }
  
  set parent [$wt_tree selection get 0]
  set parent_tolReference [$wt_tree item text $parent Reference]
  
  set InRootConsole [string equal $parent_tolReference "Console"]
  set InRootFiles [string equal $parent_tolReference "File"]
  set InRootGrammars [string equal $parent_tolReference "Grammar"]  
  
  #puts "PostVariable: InRootConsole,InRootFiles = $InRootConsole,$InRootFiles"
  #@! set node_act [$ht_vars index current]
  set node_act [$wt_vars selection get end] ;# current
  if { [string length $node_act] } {
    #@! set vars_selected [$ht_vars curselection]
    set vars_selected [$wt_vars selection get 0 end] ;# curselection
    if { [lsearch $vars_selected $node_act] >= 0 } {
      foreach var $vars_selected {
        set itemid  [$wt_vars item text $var Index]
        set path    [$wt_vars item text $var Path]
        set grammar [$wt_vars item text $var Grammar]
        set objName [$wt_vars item text $var Name]
        set object  [$wt_vars item text $var Reference] 
        
        #puts "OBJECT=$objName, PATH=$path"
        
        lappend all_objects $object
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
              $data_menu(main) add cascade -label [mc "Export matrix"] \
              -menu $data_menu(Matrix,Export)
              if {[llength $options_selected($grammar)] == 1} {
                set object [lindex [lindex $options_selected($grammar) 0] 0]
                $data_menu(main) add command -label [mc "Table matrix"] \
                  -command [list ::TolInspector::TableMatrix $object]
                $data_menu(Matrix,Export) add command -label "BDT" \
                  -command [list ::TolInspector::ExportBDTMatrix $object]
                $data_menu(main) add command -label [mc "Draw matrix"]... \
                  -command [list ::TolInspector::DrawMatrix $object]
              } else {
                $data_menu(main) add cascade -label [mc "Table matrix"] \
                  -menu $data_menu(Matrix,Table)
                $data_menu(Matrix,Export) add cascade -label "BDT" \
                  -menu $data_menu(Matrix,Export,BDT)
                $data_menu(main) add cascade -label [mc "Draw matrix"]... \
                  -menu $data_menu(Matrix,Draw)
                foreach objInfo $options_selected(Matrix) {
                  $data_menu(Matrix,Table) add command -label [lindex $objInfo 1] \
                    -command [list ::TolInspector::TableMatrix [lindex $objInfo 0]]
                  $data_menu(Matrix,Export,BDT) add command -label [lindex $objInfo 1] \
                    -command [list ::TolInspector::ExportBDTMatrix [lindex $objInfo 0]]
                  $data_menu(Matrix,Draw) add command -label [lindex $objInfo 1] \
                    -command [list ::TolInspector::DrawMatrix [lindex $objInfo 0]]
                }
              }
              if { $grammar eq "Matrix" } {
                set all_matrix {}
                foreach m $options_selected(Matrix) {
                  lappend all_matrix [ lindex $m 0 ]
                }
                TolGui_InsertEntriesMatrixChart $data_menu(main) $all_matrix
              }
            }
            Serie   {
              if {[llength $options_selected(Serie)] == 1} {
                set lblSerie "serie"
              } else  {
                set lblSerie "series"
              }
              $data_menu(main) add command -label [mc "Draw $lblSerie"] \
              -command ::TolInspector::DrawSerie
              $data_menu(main) add command -label [mc "Table $lblSerie"] \
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
                  $data_menu(Serie,DrawAutocor) add command -label [lindex $objInfo 1] \
                    -command [list ::TolInspector::DrawAutocorSerie [lindex $objInfo 0]]
                  $data_menu(Serie,TableAutocor) add command -label [lindex $objInfo 1] \
                    -command [list ::TolInspector::TableAutocorSerie [lindex $objInfo 0]]
                }
              }
              $data_menu(main) add command -label [mc "Table statistics"] \
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
              $data_menu($grammar,File) add command -label [lindex $objInfo 1] \
                  -command [list ::TolInspector::ViewDefinition\
                                [lindex $objInfo 1] [lindex $objInfo 2] $grammar]
            }
          }
          $data_menu(main) add separator
        } ;#end del foreach

        # Store selection in oza
        if { [ llength $all_objects ] > 1 || [ lindex $grammar_selected 0 ] ne "Set" } {
          $data_menu(main) add command -label [mc "Store selection in OZA"]... \
              -command [ list TolGui_StoreSelectionInOZA $all_objects ]
        } else {
          $data_menu(main) add command -label [mc "Store Set in OZA"]... \
              -command [ list TolGui_StoreSelectionInOZA $all_objects ]
          $data_menu(main) add command -label [mc "Store elements in OZA"]... \
              -command [ list TolGui_StoreSetElementsInOZA [ lindex $all_objects 0 ] ]
        }
        $data_menu(main) add separator
        # Store selection in oza

        $data_menu(main) add command -label [mc "Add to Console"] \
                -command [list ::TolInspector::AddToConsole $wt_vars]
        $data_menu(main) add command -label [mc "Write into Eval"] \
                -command [list ::TolInspector::AddToEvalWindow $wt_vars]
        $data_menu(main) add separator
        if { $InRootConsole} {
          $data_menu(main) add command -label [mc "Decompile"] \
            -command [list ::TolInspector::ClearConsoleObjSel $wt_vars]
          $data_menu(main) add separator
        }
        $data_menu(main) add command -label [mc "Select all"] \
                -command [list ::TolInspector::SelectAll $wt_vars]
        $data_menu(main) add command -label [mc "Toggle selection"] \
                -command [list ::TolInspector::ToggleSelection $wt_vars]
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
  variable wt_funcs
  variable wt_tree
  
  array unset options_selected
  foreach it [array names data_menu] {
    $data_menu($it) delete 0 end
  }
  
  set node_act [$wt_funcs selection get 0] ;# current
  if { [string length $node_act] } {
    set vars_selected [$wt_funcs selection get 0 end] ;# curselection
    if { [lsearch $vars_selected $node_act] >= 0 } {
      foreach var $vars_selected {
        set itemid  [$wt_funcs item text $var Index]
        set path    [$wt_funcs item text $var Path]
        set grammar [$wt_funcs item text $var Grammar]
        set objName [$wt_funcs item text $var Name]
        set object  [$wt_funcs item text $var Reference] 
        # La única gramática seleccionable en wt_funcs es Code
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
      $data_menu(main) add command -label [mc "Write into Eval"] \
                -command [list ::TolInspector::AddToEvalWindow $wt_funcs]
      $data_menu(main) add separator
      $data_menu(main) add command -label [mc "Select all"] \
              -command [list ::TolInspector::SelectAll $wt_funcs]
      $data_menu(main) add command -label [mc "Toggle selection"] \
              -command [list ::TolInspector::ToggleSelection $wt_funcs]
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
  variable wt_tree
  variable arguments
   
  #puts "ht_tree: $ht_tree"
  #No terminado todavía
  #return
  array unset options_selected
  
  #puts "::TolInspector::PostTree $x $y" 
  foreach it [array names data_menu] {
    $data_menu($it) delete 0 end
  }
  
  #@!! revisar!! el uso de selection y node_act
  set selection [$wt_tree selection get 0]
  if { ![string length $selection] } { return }
  
  set tolReference [$wt_tree item text $selection Reference]
  if { [llength $tolReference] == 2 } {
    set InRootFiles [string equal [lindex $tolReference 0] "File"]
  } else {
    set InRootFiles 0
  }
  
  # index of the selected node
  set node_act [$wt_tree selection get 0] ;# current  
  #puts "node_act: $node_act"  
  if { [string length $node_act] } {
    set vars_selected [$wt_tree selection get 0 end] ;# curselection    
    #puts "vars_selected: $vars_selected"      
    if { [lsearch $vars_selected $node_act] >= 0 } { ;# Not root node
      #@! set index [$ht_tree index anchor]
      set index [$wt_tree selection get 0]
      
      if { $index == 0 } return
      # if user selects a root or a grammar doesn't nothing      
      set tolReference [$wt_tree item text $index Reference]
      if { [llength $tolReference] == 1 } return
      
      # label of object showed in the tree
      set objName [$wt_tree item text Name]
      set object [$wt_tree item text Reference]
      
      if {$InRootFiles} {
        set objName [lindex $tolReference 1]
      }   
      # TICKET: #1438
      # Por que se pregunta aqui por los objetos de consola?
      set stackList [::tol::console stack list]
      #puts "stackList: $stackList"
      set stackValue {}
      foreach stackItem $stackList {
        if {[lindex $stackItem 1] eq $objName} {
          set stackValue $stackItem
        }
        break
      }
      #puts "stackValue: $stackValue"
      set grammar [lindex $stackValue 0]      
      #puts "grammar: $grammar"
      
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
proc ::TolInspector::AddToConsole {wt} {
#/////////////////////////////////////////////////////////////////////////////
  variable vars_selected

  foreach var $vars_selected {
    set reference [$wt item text $var Reference]
    #Tolcon_Trace "GetObjectFromAddress(\"[::tol::info address $reference]\")"
    ::tol::console eval "GetObjectFromAddress(\"[::tol::info address $reference]\")"
  }  
  UpdateConsoleObj
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::AddToEvalWindow {wt} {
#
# PURPOSE: Adds names of selected objects (vars or functions) in wtree wt to eval window
#
#/////////////////////////////////////////////////////////////////////////////
  variable vars_selected 

  set names ""
  foreach var $vars_selected {   
    lappend names [$wt item text $var Name]
  }
  ::TolConsole::ToEvalWindow [ListToStr $names]
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::SelectAll {wt} {
#
# PURPOSE: Selects all nodes of a given wtree
#
# PARAMETERS:
#   wt -> wtree's object
#
#/////////////////////////////////////////////////////////////////////////////
  $wt selection add 0 end
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::ToggleSelection {wt} {
#
# PURPOSE: Toggles selection of nodes of a given wtree
#
# PARAMETERS:
#   wt -> wtree's object
#
#/////////////////////////////////////////////////////////////////////////////
  set sel [$wt selection get 0 end]
  $wt selection add 0 end
  $wt selection modify {} $sel  
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::SelectionExpand { } {
#/////////////////////////////////////////////////////////////////////////////
  variable wt_vars
  set node [$wt_vars selection get 0]
  if {$node == ""} {
    ::TolInspector::SelectionCollapse 
  } else { 
    $wt_vars expand $node
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::SelectionCollapse { } {
#/////////////////////////////////////////////////////////////////////////////
  variable wt_tree
  set node [$wt_tree selection get 0]
  if {$node eq 0} {} else {
    set parent [$wt_tree item parent $node]
    $wt_tree collapse $node
    $wt_tree selection clear
    $wt_tree selection add $parent
  }
}

#/////////////////////////////////////////////////////////////////////////////
proc ::TolInspector::ToggleItem { x y } {
#/////////////////////////////////////////////////////////////////////////////
  variable wt_tree

  set lstxy [$wt_tree identify $x $y]
  if { [llength $lstxy] } {
    if { [lindex $lstxy 0] == "item" } {
      $wt_tree item toggle [lindex $lstxy 1]
    }
  }
}
