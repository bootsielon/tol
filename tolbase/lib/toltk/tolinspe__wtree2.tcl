
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
  set all_objects {}

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
        lappend all_objects $object
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
                  $data_menu(Matrix,Table) add command -label [lindex $objInfo 1]\
                    -command [list ::TolInspector::TableMatrix [lindex $objInfo 0]]
                  $data_menu(Matrix,Export,BDT) add command -label [lindex $objInfo 1]\
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
  
  #puts "::TolInspector::PostTree $x $y" 
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
    #puts "vars_selected: $vars_selected"      
    if { [lsearch $vars_selected $node_act] >= 0 } { ;# Not root node
      set index [$ht_tree index anchor]
      #puts "index: $index"    
      if { $index } {
        set info_node [split [lindex [$ht_tree get -full $index] end] "-"]
        #puts "info_node: $info_node"  
        set node_type [lindex $info_node 0]
        #puts "node_type: $node_type"          
        set node_name [lindex $info_node 1]
        #puts "node_name: $node_name"          
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
      #puts "objName: $objName"
      set object $objName

      if {$InRootFiles} {
        array set aryData [$ht_tree entry cget $index -data]
        if [info exist aryData(FileName)]  {
          set objName $aryData(FileName)
        }
      }
      #puts "2objName: $objName"   
      if { $InRootPool } {
        #puts "es InRootPool $item_id"        
        set itemid $item_id
        #set object $data(pool,variables,$itemid)
        set object $data(pool,reference,$node_type)
      } else {
        #puts "NO InRootPool"
        if { [string length $tolset] } {          
          set object [concat [list $tolset] $tolindex]
        } else {
          set object $objName
        }
      }
      #puts "object = $objName '[$ht_tree entry cget $index -data]'"
#Tolcon_Trace "***********************************" {red}
#Tolcon_Trace "index=$index, node_type=$node_type, node_name=$node_name" {red}
#Tolcon_Trace "tolset=$tolset, tolindex=$tolindex, aryData= [$ht_tree entry cget $index -data]" {red}
#Tolcon_Trace "object=$object, label=$objName" {red}
#Tolcon_Trace "***********************************" {red}
      
      #
      # TICKET: #1438
      # Por que se pregunta aqui por los objetos de consola?
      #
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
