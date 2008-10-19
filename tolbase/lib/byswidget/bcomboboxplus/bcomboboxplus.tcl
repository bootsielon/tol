#/////////////////////////////////////////////////////////////////////////////
# FILE    : bcomboboxplus.tcl
# PURPOSE : Bayes Combobox Plus. Component that allows to select an element,
#           showing a list with all the information that necesary to select
#           it.
#           Once selected, the obtained result is divided for its
#           presentation in 1+n parts: one for the key field and n for
#           fields that we wish to show. n can be zero.
#/////////////////////////////////////////////////////////////////////////////

package require byscommon
package require BLT

package require blistboxplus
package require bentrydate


#/////////////////////////////////////////////////////////////////////////////
::snit::widget bcomboboxplus {
#
# PURPOSE: Bayes Combobox Plus. Allows to choose an element of a list.
#
#/////////////////////////////////////////////////////////////////////////////
# SPECIFIC OPTIONS
# colkey ->       Key column of comboboxplus. If it is not defined, 
#                 the first column is taken like key.
# cols ->         List of columns of the comboboxplus. Including hidden 
#                 columns. Required.
# hiddencols ->   Hidden columns of the comboboxplus. If it is not defined,
#                 all columns are visible.
# titles ->       Title of each one of labels in the comboboxplus.
# colslb ->       Columns of the listboxplus, including the hidden columns. 
#                 If it is not defined, is taken the value of -cols.
# hiddencolslb -> Hidden columns of the listboxplus. If it is not defined,
#                 all columns are visible.
# parent ->       Indicates the father where comboboxplus is located. If it 
#                 is not defined, the parent is .
# showlabelkey -> If it has value 1 it shows label of the key on 
#                 bcomboboxplus. Default 0.
# showlabels ->   If it has value 1 it shows the labels of not hidden fields
#                 that hasn't the key. Default 0.
# width ->        Width of entrys. Can be a list or a value (all entrys 
#                 will have the same width). Default 10.
# keyhelptext ->  List of helptext that is showed when the mouse is over the 
#                 entry of the key.
# helptexts ->    List of helptext that is showed when the mouse is over the 
#                 entrys of the fields.
# modifycmd ->    Command to execute after selecting an item.
# postcommand ->  Command to execute before showing the list.
# color ->        Colour of the labels (red, blue, etc.). Default "black".
# nrows ->        Number of visible rows of blistboxplus.
# variable ->     Variable with text of entry key
# text ->         Text of the entry of the key field.
# widthlb ->      Width of the pop-up blistboxplus. Default 350. If has a
#                 value "auto" it autoadjust to width of entry of the key.
# keydate ->      If has a value 1, the widget of the key will be a
#                 bentrydate instead of an Entry. This option only can be set
#                 on creation time.
# fieldsdate ->   Takes a list that for each field of bcomboboxplus, if has a
#                 value 1, the widget of this field will be abentrydate
#                 instead of an Entry. This option only can be set on
#                 creation time.
# font ->         Font used in the widget. Is a list with the name and the
#                 size of the font.
# varcol ->       Name of the field whose entry will be associated the
#                 variable defined in the option -variable. If it's empty,
#                  this variable will be associated to the entry of the key.
# keypress ->     Activates the search by pressed keys
# colkeypress ->  Column by which the search when pressing is made a key
#
# VARIABLES
# lblKey ->           Label next to key field.
# entryKey ->         Entry that content the key field.
# arrowBtn ->         Arrow button.
# created ->          Indicates if witget has been created.
# fr ->               Pop-up frame with blistboxplus.
# in ->               It takes a value 1 if the cursor is located in the 
#                     pop-up frame of listbox.
# ncols ->            Number of columns.
# colGrid($widget) -> Array with column where was grided the widget $widget.
# e ->                Array of entrys (or bentrydates) of the fields
# blstentry    -> lista de objetos entry visualizados
#
#/////////////////////////////////////////////////////////////////////////////
  widgetclass Bcomboboxplus

  option -titles       {}
  option -cols         {}
  option -hiddencols   {}
  option -colslb       {}
  option -hiddencolslb {}
  option -parent       .
  option -showlabelkey 0 
  option -showlabels   0
  option -colkey       ""
  option -width        10
  option -keyhelptext  ""
  option -helptexts    ""
  option -modifycmd    ""
  option -postcommand  ""
  option -color        {Black} 
  option -nrows        5
  option -widthlb      350
  option -variable     "" 
  option -text         ""
  option -keydate      0
  option -fieldsdate   {}
  option -font         {Arial 8}
  option -varcol       ""
  option -keypress     true
  option -colkeypress  "" 
      
  variable lblKey
  variable entryKey
  variable arrowBtn
  variable menu  
  variable lstBox
  variable created 0  
  variable fr
  variable in
  variable e
  variable ncols 0  
  variable helptexts
  variable colGrid
  variable varKey ""
  variable blstentry {}
  variable vez 0
  variable lstPreVal {}
  variable idxFind {}
  variable valBusqueda ""
 

  #///////////////////////////////////////////////////////////////////////////
  constructor {args} {
  #
  # PURPOSE: Constructor for bcomboboxplus.
  #
  # PARAMETERS:
  #   args ->  List of options for comboboxplus
  #
  #///////////////////////////////////////////////////////////////////////////
    $self configure  -font       [from args -font ""]
    install lblKey    using Label        $self.lk -font [$self cget -font]     
    install arrowbtn  using ArrowButton  $self.ab -dir bottom
    
    $self configure -keypress    [from args -keypress  true]
    $self configure -colkeypress [from args -colkeypress ""]    
    $self configure -keydate     [from args -keydate ""]
    $self configure -fieldsdate  [from args -fieldsdate ""]
    $self configure -cols        [from args -cols ""]
    $self configure -colkey      [from args -colkey ""]
    $self configure -widthlb     [from args -widthlb ""]
    $self configure -width       [from args -width ""]

    $self configure -keyhelptext [from args -keyhelptext ""]
    $self configure -helptexts   [from args -helptexts ""]
    $self configure -color       [from args -color ""]
    $self configure -modifycmd   [from args -modifycmd ""]
    $self configure -postcommand [from args -postcommand ""]    
    $self configure -nrows       [from args -nrows ""]
    $self configure -variable    [from args -variable ""]
    $self configure -varcol      [from args -varcol ""]

    # Create the widget
    $self createwidget $args
    $self configure -colslb       [from args -colslb ""]
    $self configure -text         [from args -text ""]
    $self configure -showlabels   [from args -showlabels ""]
    $self configure -showlabelkey [from args -showlabelkey ""]
    $self configure -titles       [from args -titles ""]

    # Apply all arguments given:
    $self configurelist $args
    
    # Widget is now created
    set created 1
    
    # initializes the widget
    $self _Init
  }

  #///////////////////////////////////////////////////////////////////////////
  #
  # PURPOSE: Methods cget & configure
  #
  #///////////////////////////////////////////////////////////////////////////    
  onconfigure -width { lst } {    
    if {[llength $lst]} { ;# 1 item
    set aux $lst
      for {set c 1} {$c<$ncols} {incr c} {
        set lst [concat $lst $aux]
      }
    }
    set options(-width) $lst
    # Widget created
    if {$created} {
      $self.ek configure -width [lindex [$self cget -width ] [$self _PosKey]]        
      #width of fields labels
      for {set c 0} {$c<$ncols} {incr c} {
        set name [lindex [$self cget -cols ] $c]
        $e($name) configure -width [lindex [$self cget -width ] $c]        
      }
    }
  }
  

  onconfigure -colkey { value } {
    if {$value eq ""} {
      set value [lindex [$self cget -cols] 0]
    }
    set options(-colkey) $value
    # Widget created
    if {$created == 1} {
      set lbrow [$lstBox selection get]
      # the selection now doesn't exists in blistboxplus
      if {$lbrow != ""} {        
        set selected [$lstBox get $lbrow]
      }
      # Updates the label and entry for the key field
      $self.lk configure -text [lindex [$self cget -titles ] [$self _PosKey]]      
      if { [$self.ek cget -text] != "" } {
        if {$lbrow != ""} {
          $self.ek configure -text  [lindex $selected [$self _PosKey]]
        } else  {
          # Imposible to update from blistboxplus
          $self.ek configure -text  ""
        }
      }
      # Updates other fields
      for {set c 0} {$c<$ncols} {incr c} {
        set name [lindex [$self cget -cols ] $c]
        if {$lbrow != ""} {
          $e($name) configure -text  [lindex $selected $c]
        } else  {
          # Imposible to update from blistboxplus
          $e($name) configure -text  ""
        }
        # Grid this field
        
        grid $self.l$name
        if { $name eq [lindex [$self cget -cols ] [$self _PosKey]]} {
          #grid $self.e$name -sticky {}
        } else {
          grid $e($name) -sticky ew
          grid columnconfigure $self $colGrid($e($name)) -weight 1
        }
      }
      # Hides the field of the key in the other fields  
      set name [lindex [$self cget -cols ] [$self _PosKey]]
      grid remove $self.l$name
      grid remove $e($name)
      grid columnconfigure $self $colGrid($e($name)) -weight 0
      $self configure -hiddencols [$self cget -hiddencols]
    }
  }
    
  onconfigure -titles { lst } {
    set options(-titles) $lst
    if {!($lst eq "")} {
      # Configures the title of key field
      $self.lk configure -text [lindex [$self cget -titles ] 0]
      # Configures the rest of titles
      for {set c 0} {$c<$ncols} {incr c} {
        set name [lindex [$self cget -cols ] $c]
        set shown [winfo exist $self.l$name]
        if { $shown == 1 } {
          $self.l$name configure -text [lindex [$self cget -titles ] $c]
        }
      }
      $lstBox configure -titles $lst
    }
  }

  onconfigure -cols { lst } {
    set options(-cols) $lst
    # Establishes the number of columns
    set ncols [llength [$self cget -cols]]
  }

  onconfigure -hiddencols { lst } {
    set options(-hiddencols) $lst
    for {set c 0} {$c<$ncols} {incr c} {
      if { $c != [$self _PosKey] } {
        set aux [expr $c + 1]
        set name [lindex [$self cget -cols ] $c]
        set fieldCreated [lsearch -glob [grid slaves $self] $e($name)]
        set hidden [lsearch -glob [$self cget -hiddencols ] $name]
        if { $fieldCreated == "-1" } {
          # Shows this field
          grid $e($name)
          grid columnconfigure $self $colGrid($e($name)) -weight 1
          grid $self.l$name
        }
        if { $hidden != "-1" } {
          # Hiddes this field
          grid remove $e($name)
          grid columnconfigure $self $colGrid($e($name)) -weight 0
          grid remove $self.l$name
        }
      }
    }
    $self configure -showlabels [$self cget -showlabels]
  }

  onconfigure -showlabels { value } {
    set options(-showlabels) $value
    # Show labels
    if { $value == 0 } {
      # Show labels off
      for {set c 0} {$c<$ncols} {incr c} {
        if {$c != [$self _PosKey]} {
          set name [lindex [$self cget -cols ] $c]
          set exists [winfo exist $self.l$name]
          if { $exists == 1 } {                
            grid remove $self.l$name
          }
        }
      } 
    } else {
      # Show labels on
      for {set c 0} {$c<$ncols} {incr c} {
        if {$c != [$self _PosKey]} {
          set name [lindex [$self cget -cols ] $c]
          set exists [winfo exist $self.l$name]
          if { $exists == 1 } {
            set hidden [lsearch -glob [$self cget -hiddencols ] $name]
            if { $hidden == -1 } {
              grid $self.l$name
            } else  {
              grid remove $self.l$name
            }
          }
        }
      }
    }
  }

  onconfigure -showlabelkey { value } {
    set options(-showlabelkey) $value
    if { $value == 0 } {
      # Show label key off
      grid remove $self.lk
    } else {
      # Show label key on        
      grid $self.lk
    }
  }
    
  onconfigure -colslb { lst } {
    if {$lst eq ""} {
      set lst [$self cget -cols]
    }
    set options(-colslb) $lst
    if { $created == 1 } {
      $self _ShowList 
      $lstBox configure -cols $lst
      $self _CloseList
    } else  {
      $lstBox configure -cols $lst
    }
  }

  onconfigure -hiddencolslb { lst } {
    set options(-hiddencolslb) $lst
    if { $created == 1 } {
      #$self _ShowList
      eval $lstBox configure -hiddencols \
        [lappend [$self cget -hiddencolslb] $lst]
      #$self _CloseList
    } else  {
      eval $lstBox configure -hiddencols \
        [lappend [$self cget -hiddencolslb] $lst]
    }
  }    

  onconfigure -keyhelptext { value } {
    set options(-keyhelptext) $value
  }    

  onconfigure -helptexts { lst } {
    set options(-helptexts) $lst
  }

  onconfigure -color { value } {
    set options(-color) $value
  }

  onconfigure -modifycmd { lst } {
    set options(-modifycmd) $lst
  }

  onconfigure -postcommand { lst } {
    set options(-postcommand) $lst
  }
    
  onconfigure -nrows { value } {
    set options(-nrows) $value
  }
    
  onconfigure -widthlb { value } {
    set options(-widthlb) $value
  }

  onconfigure -variable { value } {
    if { $created == 1 } {
      puts "Bcomboboxplus onconfigure -variable error: this option only\
            can be configured on creation time."
    } else {
      if {[llength value] eq 0} {
        # default value
        set options(-variable) [varname varKey]    
      } else {
        set options(-variable) $value
      }
    }
  }

  onconfigure -text { value } {
    set options(-text) $value
    if { $created == 1 } {
      $self.ek configure -text $value
    }
  }
  
  onconfigure -keydate { value } {
    if { $created == 1 } {
      puts "Bcomboboxplus onconfigure -keydate error: this option only can\
            be configured on creation time."
    } else {
      set options(-keydate) $value      
    }
  }
  
  onconfigure -fieldsdate { lst } {
    if { $created == 1 } {
      puts "Bcomboboxplus onconfigure -fieldsdate error: this option only\
            can be configured on creation time."
    } else {
      set options(-fieldsdate) $lst      
    }
  }
  
  onconfigure -varcol { value } {
    if { $created == 1 } {
      puts "Bcomboboxplus onconfigure -varcol error: this option only\
            can be configured on creation time."
    } else {
      if {$value eq ""} {
        # by default gives the -colkey option value
        set options(-varcol) "key"
      } else {
        if { [lsearch -exact [$self cget -cols ] $value ] eq -1 } {
          puts "Bcomboboxplus onconfigure -varcol error: the argument given \
                '$value' must be a valid column."
        } else {
          set options(-varcol) $value
        }
      }
    }    
  }

  #///////////////////////////////////////////////////////////////////////////
  method createwidget { args } {
  #
  # PURPOSE: Creation of the objects that compose the widget.
  #
  # PARAMETERS:
  #   args -> List of options for the bcomboboxplus.
  #
  #///////////////////////////////////////////////////////////////////////////
    global tcl_platform
    update
    set blstentry {}
    set colAct 0
    # Creates the label, entry and arrowbutton for the key field
    $self.lk configure -text [lindex [$self cget -titles ] [$self _PosKey]] \
                       -helptext [$self cget -keyhelptext ] \
                       -foreground [$self cget -color ]
    # entryKey gives the type of widget set in option -keydate
    if {[$self cget -keydate]} {
      set entryKey  [bentrydate $self.ek ]
    } else {
      set entryKey  [Entry $self.ek -takefocus 1 -font [$self cget -font]]
    }
    lappend blstentry $entryKey
    grid $self.lk -row 0 -column $colAct
    incr colAct
    grid $self.ek -row 0 -column $colAct -sticky ew
    set colGrid($self.ek) $colAct

    $self.ek configure -width [lindex [$self cget -width ] [$self _PosKey]] 
    $self.ek configure -helptext [$self cget -keyhelptext ]
    grid columnconfigure $self $colAct -weight 1
    incr colAct
    $self.ab configure -helptext [mc "List of values"] \
            -command [list $self _ShowList] -height 20 -highlightthickness 0 \
            -borderwidth 1 -takefocus 0 -type button -ipadx 1
    grid $self.ab -row 0 -column $colAct    
    incr colAct    
    # Creates labels and entrys for all fields
    incr colAct
    for {set c 0} {$c<$ncols} {incr c} {
      set name [lindex [$self cget -cols ] $c]        
      # Creates a label  
      install l$name using Label $self.l$name -font  [$self cget -font] \
                                        -foreground [$self cget -color ]
      $self.l$name configure -text [lindex [$self cget -titles ] $c]
      $self.l$name configure -helptext [lindex [$self cget -helptexts ] $c] 
      grid $self.l$name -row 0 -column $colAct    
      incr colAct
        
      # Creates an entry
      if { [info exist [lindex [$self cget -fieldsdate] $c] ] eq 0 } {
        if { [ lindex [$self cget -fieldsdate] $c ] eq 1 } {          
          set e($name) [bentrydate $self.ed$name]
        } else {
          set e($name) [Entry $self.en$name -font [$self cget -font] ]          
        }
      } else {
        set e($name) [Entry $self.en$name -font  [$self cget -font] ]
      }
      lappend blstentry $e($name)
      
      set aux [$self cget -width ]
      $e($name) configure -width [lindex [$self cget -width ] $c] 
      $e($name) configure -helptext [lindex [$self cget -helptexts ] $c] 
      
      grid $e($name) -row 0 -column $colAct -sticky ew
      grid columnconfigure $self $colAct -weight 1
      set colGrid($e($name)) $colAct
      incr colAct        
    }
    #Set the variable for the field of varname
    if { !([$self cget -variable] eq "") } {
      set varn [$self cget -variable ]
      if { [$self cget -varcol ] eq "key" } {
        # variable is set to the entry of the key
        $self.ek configure -textvariable $varn
      } else {
        # variable is set to another entry
        $e([$self cget -varcol ]) configure -textvariable $varn                                     
      }
    }
    #Remove the label and entry on the fields
    set name [lindex [$self cget -cols ] [$self _PosKey]]
    grid remove $self.l$name
    grid remove $e($name)
    grid columnconfigure $self $colGrid($e($name)) -weight 0
    #Top-level pull-down
    set fr [toplevel $self.fr -borderwidth 0 -relief solid \
            -highlightthickness 0 -takefocus 0 -bd 1 ]
    wm withdraw $fr
    if { [string equal $tcl_platform(platform) "windows"] } {
      update idletasks
    } 
    wm overrideredirect $fr 1
    wm transient $fr [winfo toplevel $self]
    wm withdraw  $fr
    wm resizable $fr 0 0
 
    # width of the listboxplus
    set wlb 0
    for {set c 0} {$c < [llength $aux]} {incr c 1} {
      set name [lindex [$self cget -cols ] $c]
      set hidden [lsearch -glob [$self cget -hiddencols ] $name]
      if {$hidden == -1} {
        set wlb [expr $wlb + [lindex $aux $c ]  ]
      }
    }
    set lstBox [blistboxplus $fr.blb -cols [ $self cget -colslb ] \
            -hiddencols [ $self cget -hiddencolslb ] -parent $self \
            -orientation vertical -selectmode single\
            -width $wlb -font [$self cget -font] \
            -exportselection false -border 0 -height 0 -nomenu 1]
  
    pack $lstBox -side top -fill both -expand yes
    #grid $lstBox -sticky news
    set in 1;
    bind $fr <Enter> +[list $self _Enter]
    bind $fr <Leave> +[list $self _Leave]
    bind $fr <ButtonPress-1>    +[list $self _Hide ]
    bind $fr <KeyPress-Escape>  +[list $self _Hide ]
    set lb [$lstBox getlb]
    bind $lb <Double-Button-1>  +[list $self _CloseList]
    bind $lb <KeyPress-Return>  +[list $self _CloseList]
    # Bindings
    bind $self.ek <Key-Up>                +[list $self _SelectPrevious]
    bind $self.ek <Key-Down>              +[list $self _SelectNext]
    bind $self.ek <Key-Home>              +[list $self _SelectFirst]
    bind $self.ek <Key-End>               +[list $self _SelectLast]
    bind $self.ek <Double-ButtonPress-1>   [list $self _ShowList]
    # menu
    $self _CreateMenu
  }


  #///////////////////////////////////////////////////////////////////////////
  method _CreateMenu {args} {
  #
  # PURPOSE: Append menu to listbox.
  #
  #///////////////////////////////////////////////////////////////////////////
    install menu using menu $self.m -tearoff 0

    $menu add command -label [mc "Select from a List"] \
      -command "$self _ShowList"
    $menu add command -label [mc "Unselect all"] \
      -command "$self SetValueView {}"
    $menu add command -label [mc "Copy"] \
      -command "$self _CopySelected 0"
    $menu add command -label [mc "Copy all"] \
      -command "$self _CopySelected 1"
    
    $menu add command -label [mc "Search in list"] \
      -command "$self _FindCreate"

    
    
    bind $self.ek <Button-3> +[list tk_popup $menu %X %Y]
  }
  

  #///////////////////////////////////////////////////////////////////////////
  method _CopySelected { {all 0} } {
  #
  # PURPOSE: Copy the selected items to the clipboard
  #
  # PARAMETERS: all -> 1 : Copy all entrys
  #                    0 : Copy only code
  #
  #///////////////////////////////////////////////////////////////////////////
    clipboard clear
    if {$all} {
      set txt [$self values get]
    } else {
      set txt [$self values get [$self cget -colkey]]
    }
    clipboard append $txt
  }   
  
  
  #///////////////////////////////////////////////////////////////////////////
  method _Init {} {
  #
  # PURPOSE: Method that initializes the widget.
  #
  #///////////////////////////////////////////////////////////////////////////
    global tcl_platform
    
    if { ($tcl_platform(platform) eq "windows") } {
      $self _ShowList 1    
      $self _CloseList 1
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method _Enter { } {
  #
  # PURPOSE: Give value to the variable "in" when enter on frame of
  #          listboxplus.
  #
  #///////////////////////////////////////////////////////////////////////////
    set in 1
  }
        
  #///////////////////////////////////////////////////////////////////////////
  method _Leave { } {
  #
  # PURPOSE: Give value to the variable "in" when left on frame of
  #          listboxplus.
  #
  #///////////////////////////////////////////////////////////////////////////
    set in 0
  }

  #///////////////////////////////////////////////////////////////////////////
  method _ShowList { {initial 0} } {
  #
  # PURPOSE: Shows the selection list in a new window
  #
  # PARAMETERS:
  #   args -> List of options for the comboboxplus Witget
  #
  # TODO:
  #   Leave creation of the widget to the CreateWitget method. This method
  #   only shows, not create it.
  # 
  #///////////////////////////////////////////////////////////////////////////
    # ejecutamos comando previo a la visulizacion
    if {[string length [$self cget -postcommand]  ] && $created} {
      . configure -cursor watch
      update
      if {!$initial} {        
        eval [$self cget -postcommand]
      }
      . configure -cursor ""
      update
    }
    ;# if blistboxplus hasn't items, will not do show it
    if { $created } {          
      if {[winfo viewable $fr] == 1} {
        wm withdraw $fr
        grab release $fr
      } else {
        set vwidth [winfo reqwidth $fr]
        set vheight [winfo reqheight $fr]
        if {!([llength [$lstBox get]] eq 0)} {
          wm deiconify $fr
          #geometry
          focus -force $fr
          grab -global $fr
        }
      }
      # Positioning the dialog
      set posh [winfo rootx $self.ek]  
      set posv [expr [winfo rooty $self.ek]+[winfo height $self.ek]]
      # position of the right edge of listboxplus
      set right [expr $posh + [winfo width $fr]]
      # position of the inferior edge of listboxplus
      set bottom [expr $posv + [winfo height $self.ek] \
                             + [winfo height $fr]]
      if { $right > [winfo vrootwidth $self] } {
        #It is necessary to change the horizontal location
          set posh [expr [winfo rootx $self.ek] + [winfo width $self.ek] \
                                                     - [winfo width $fr] ]
      }    
      if { $bottom > [winfo vrootheight $self] } {
        #It is necessary to change the vertical location
        set posv [expr [winfo rooty $self.ek] - [winfo height $fr] ]
      }
      if {[$self cget -widthlb] eq "auto"} {
        set width [winfo width $self.ek] 
      } else {
        set width [$self cget -widthlb]
      }
      $lstBox configure -width $width
      # Calculus of the height of blistboxplus 
      # Adjustment do not leave rows
      set nVisibleRows [$self cget -nrows]    
      set nRows [llength [$lstBox get]]
      if {$nRows < $nVisibleRows} {
        set nVisibleRows $nRows
      }
      # Adjustment of the height of listbox with/without title
      if {[$self cget -showtitles]} {
        set height \
          [expr ($nVisibleRows * 19) + (($nVisibleRows - 1) * 2 ) + 22]
      } else  {
        set height \
          [expr ($nVisibleRows * 19) + (($nVisibleRows - 1) * 2 ) + 2 ]
      }
      # Apply geometry
      wm geometry $fr ${width}x$height+$posh+$posv
      # check value previus ?
      if {!$initial} {         
        focus [$lstBox getlb]      
        set lstPreVal [$self values get]
        if {[string length [lindex $lstPreVal 0]]} {
          lbOnKeyPress $lstBox \
                       [string range [lindex $lstPreVal 0] end end] \
                       [string range [lindex $lstPreVal 0] 0   end-1] \
                       [clock clicks -milliseconds] 500
        } else {
          set id [lindex [$lstBox get] 0]
          # we positioned ourselves in the first element
          if {[string length $id]} {
            $lstBox selection set $id
            $lstBox see   $id
             $lstBox focus $id
          }
        }
      }
    }
    update
  }

  #///////////////////////////////////////////////////////////////////////////
  method _CloseList { {initial 0} } {
  #
  # PURPOSE: Closes the selection list
  #
  #
  #///////////////////////////////////////////////////////////////////////////
    wm withdraw $fr
    grab release $fr
    if {$in == 1 } {
      $self _ShowSelected
    }
    if {[string length [$self cget -modifycmd] ] && $created } {
      . configure -cursor watch
      update
      if {!$initial} {     
        eval [$self cget -modifycmd]
      }
      . configure -cursor ""
      update
    }
    update
  }

  #///////////////////////////////////////////////////////////////////////////
  method _Hide { } {
  #
  # PURPOSE: 
  #
  #
  #///////////////////////////////////////////////////////////////////////////
    if {$in == 0 } {
      wm withdraw $fr
      grab release $fr
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method _ShowSelected { } {
  #
  # PURPOSE: Shows in bcomboboxplus the content of the row selected in
  #          blistboxplus
  #
  #///////////////////////////////////////////////////////////////////////////
    # Recover the row selected of blistboxplus
    set lbrow [$lstBox selection get]
    if {$lbrow != ""} {
      # Recover the selected values
      set selected [$lstBox get $lbrow]     
      # Show the key value
      $self.ek configure -text [lindex $selected [$self _PosKey]]
      $self configure -text [lindex $selected [$self _PosKey]]
      # Show the not hidden values
      for {set c 0} {$c<$ncols} {incr c} {
        if {$c != [$self _PosKey]} {
            set name [lindex [$self cget -cols ] $c]
          $e($name) configure -text  [lindex $selected $c]
        }
      }
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method selectIndex { index } {
  #
  # PURPOSE: Selects index in blistboxplus and shows the content in
  #          bcomboboxplus
  #
  #///////////////////////////////////////////////////////////////////////////
    #Index is got
    set lbinx [$lstBox get]
    #if the index is in the indexes list
    if { [lsearch $lbinx $index ] != -1 } {
      #first index
      set first [lindex $lbinx 0]
      #last index
      set last [lindex $lbinx end]
      #The previous selection is cleared
      $lstBox selection clear $first $last
      #And the new one is set
      $lstBox selection set $index $index
      $self _ShowSelected
    }
  }
    
  #///////////////////////////////////////////////////////////////////////////
  method selectText { text { col "" } } {
  #
  # PURPOSE: Selects index with the given text in the given col in blistboxplus 
  #          and shows the content in bcomboboxplus
  # PARAMETERS: text -> Text to search
  #             col ->  column to search in 
  #
  #///////////////////////////////////////////////////////////////////////////
    if { $col == "" } {
      set col [$self cget -colkey]
    }
    foreach inx [$self get] {
      if { [$self get $inx $col] == $text } {
	$self selectIndex $inx
	break
      }
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method _SelectFirst { } {
  #
  # PURPOSE: Selects the first row in blistboxplus and it automatically
  #          updates it in bcomboboxplus
  #
  #///////////////////////////////////////////////////////////////////////////
    if {[$self.ek cget -text] != ""} {
      $lstBox first
      $self _ShowSelected
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method _SelectLast { } {
  #
  # PURPOSE: Selects the last row in blistboxplus and it automatically
  #          updates it in bcomboboxplus
  #
  #///////////////////////////////////////////////////////////////////////////
    if {[$self.ek cget -text] != ""} {
      $lstBox last
      $self _ShowSelected
    }
  } 
    
  #///////////////////////////////////////////////////////////////////////////
  method _SelectPrevious { } {
  #
  # PURPOSE: Selects the previous row in blistboxplus and it automatically
  #          updates it in bcomboboxplus
  #
  #///////////////////////////////////////////////////////////////////////////
    if {[$self.ek cget -text] != ""} {
      $lstBox previous
      $self _ShowSelected
    }
  }
       
  #///////////////////////////////////////////////////////////////////////////
  method _SelectNext { } {
  #
  # PURPOSE: Selects the following row in blistboxplus and it automatically
  #          updates it in bcomboboxplus
  #
  #///////////////////////////////////////////////////////////////////////////
    if {[$self.ek cget -text] != ""} {
      $lstBox next
      $self _ShowSelected
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method key { id {value ""} } {
  # 
  # PURPOSE: Set the value of entry of the key, get the value of entry of
  #          the key or clean this entry
  # 
  # PARAMETERS:
  #   id -> "set"
  #         "get"
  #         "clean"
  #   value -> Value of the key for set operation
  # 
  # RETURN: If id is "get" returns the value of the key entry
  # 
  #///////////////////////////////////////////////////////////////////////////
    if { $id eq "set" } {
      $self.ek configure -text $value
    } else {
      if { $id eq "get" } {
        return [$self.ek cget -text]
      } else {
        if { $id eq "clean" } {
          $self.ek configure -text ""
        }
      }
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  method values { id {lst ""} } {
  # 
  # PURPOSE: Set the values of the entrys (excluding the key), get or clean
  # 
  # PARAMETERS:
  #   id -> "set"
  #         "get"
  #         "clean"
  #   lst -> List of values of the entrys for set operation
  # 
  # RETURN: If id is "get" returns the list with values of the entrys
  # 
  #///////////////////////////////////////////////////////////////////////////
    if { $id eq "set" } {
      $self values clean
      for {set c 0} {$c<$ncols} {incr c} {
        set name [lindex [$self cget -cols ] $c]
        $e($name) configure -text [lindex $lst $c]
      } 
    } else {
      if { $id eq "get" } {
        if {[llength $lst] eq 1} { ;# returns a value
          set lstAux ""
          for {set c 0} {$c<$ncols} {incr c} {
            set name [lindex [$self cget -cols ] $c]
            if {[string compare $name $lst] eq 0} {
                set lstAux [$e($name) cget -text]
            }            
          }
          if { $lstAux eq "" } {
             set lstAux [$self.ek cget -text]
          }
          set lst $lstAux
        } else { ;# returns all values
          set lst ""
          for {set c 0} {$c<$ncols} {incr c} {
            set name [lindex [$self cget -cols ] $c]
            if { [$e($name) cget -text] eq "" } {
              lappend lst [$self.ek cget -text]
           } else {
              lappend lst [$e($name) cget -text]
            }
          }
        }
        return $lst
      } else {
        if { $id eq "clean" } {
          for {set c 0} {$c<$ncols} {incr c} {
            set name [lindex [$self cget -cols ] $c]
            $e($name) configure -text ""
          }           
        }
      }
    }
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method setEntry { name value } {
  #///////////////////////////////////////////////////////////////////////////
    if {$name eq [$self cget -colkey]} {
      $self.ek configure -text $value
    } 
    $e($name) configure -text $value
  }
    
  #///////////////////////////////////////////////////////////////////////////
  method configureAll {opc val args} {
  #
  # PURPOSE: An option is applied all entry, including a the key
  #
  # PARAMETERS:
  #   opc: Option to apply
  #   val: Value associated to the option
  #
  #///////////////////////////////////////////////////////////////////////////
    # Values to entry of the key are introduced
    $self.ek configure -$opc $val    
    # Values to the rest of entrys are introduced
    for {set c 0} {$c<$ncols} {incr c} {
       set name [lindex [$self cget -cols ] $c]
       $e($name) configure -$opc $val
    }
  }
   
    
  #///////////////////////////////////////////////////////////////////////////
  method configureDesc {opc val args} {
  #
  # PURPOSE: We applied an option all entry, less to the key 
  #
  # PARAMETERS:
  #   opc: Option to apply
  #   val: Value associated to the option
  #
  #///////////////////////////////////////////////////////////////////////////
    # Values are introduced
    for {set c 0} {$c<$ncols} {incr c} {
        set name [lindex [$self cget -cols ] $c]
        $e($name) configure -$opc $val
    }
  }
    
  #///////////////////////////////////////////////////////////////////////////
  method getEntryKey { } {
  #
  # PURPOSE: Returns the path to the entry with the key 
  #
  #///////////////////////////////////////////////////////////////////////////
    return $self.ek
  }

  #///////////////////////////////////////////////////////////////////////////
  method _PosKey { } {
  #
  # PURPOSE: Devuelve la posicion en la que se encuentra la clave dentro de
  #          la lista de columnas del bcomboboxplus
  #
  # RETURNS:
  #   Devuelve la posicion en la que se encuentra la clave
  #
  #///////////////////////////////////////////////////////////////////////////
     return [lsearch -glob [$self cget -cols] [$self cget -colkey]] 
  }
       
  #///////////////////////////////////////////////////////////////////////////
  method activate {stt args} {
  #
  # PURPOSE: Aplicamos una opción a los elemetos del componente. Por un lado
  #          al entry del código, al botón y al resto de campos entry.
  #
  # PARAMETERS:
  #   stt: opción a aplicar
  #     normal    -> Code (yes) Button (no)  Desc (no)
  #     active    -> Code (yes) Button (no)  Desc (yes)
  #     select    -> Code (no)  Button (yes) Desc (no)
  #     disabled  -> Code (no)  Button (no)  Desc (yes)
  #     notactive -> Code (no)  Button (no)  Desc (no)
  #     allactive -> Code (yes) Button (yes) Desc (yes)
  #     descnoedit-> Code (yes) Button (yes) Desc (not editable)
  #
  #///////////////////////////////////////////////////////////////////////////
  # introducimos valores
    switch $stt {
      "normal" {
        BfoActivateWidget    $self.ek    $stt
        $self.ek configure   -editable   true
        BfoActivateWidget    $self.ab    "disabled"
        $self configureDesc  state       disabled
        $self configureDesc  editable    false
        bind $self.ek <Double-ButtonPress-1>   [list $self _ShowList]
      }
      "active" {
        BfoActivateWidget    $self.ek    "normal"
        $self.ek configure   -editable   true
        BfoActivateWidget    $self.ab    "disabled"
        $self configureDesc  state       normal
        $self configureDesc  editable    true
        bind $self.ek <Double-ButtonPress-1>   [list $self _ShowList]
      }
      "select" {
        BfoActivateWidget    $self.ek    "disabled"
        $self.ek configure   -editable   false
        BfoActivateWidget    $self.ab    "active"
        $self configureDesc  state       normal
        $self configureDesc  editable    false
        bind $self.ek <Double-ButtonPress-1>   {}
      }
      "disabled" {
        BfoActivateWidget    $self.ek    "disabled"
        $self.ek configure   -editable   false
        BfoActivateWidget    $self.ab    "disabled"
        $self configureDesc  state       normal
        $self configureDesc  editable    true
        bind $self.ek <Double-ButtonPress-1>   {}
      }
      "notactive" {
        BfoActivateWidget    $self.ek    "disabled"
        $self.ek configure   -editable   false
        BfoActivateWidget    $self.ab    "disabled"
        $self configureDesc  state       normal
        $self configureDesc  editable    false
        bind $self.ek <Double-ButtonPress-1>   {}
      }
      "allactive" {
        BfoActivateWidget    $self.ek    "normal"
        $self.ek configure   -editable   true
        $self configureDesc  state       normal
        eval BfoActivateWidget     $self.ab    active
        $self configureDesc  editable    true
        bind $self.ek <Double-ButtonPress-1>   [list $self _ShowList]
      }
      "descnoedit" {
        BfoActivateWidget    $self.ek    "normal"
        $self.ek configure   -editable   true
        $self configureDesc  state       normal
        eval BfoActivateWidget     $self.ab    active
        $self configureDesc  editable    false
        bind $self.ek <Double-ButtonPress-1>   [list $self _ShowList]
      }
      default {
        puts "BComboboxPlus Activate ERROR: invalid state: $stt"
      }
    }
  }
    #///////////////////////////////////////////////////////////////////////////
  method getwidget {} {
  #
  # PURPOSE: Obtenemos el nombre de todos los objetos creados
  #
  # PARAMETERS: lista con el path de los entrys creados
  #   
  #///////////////////////////////////////////////////////////////////////////
    return $blstentry
  }
  
    #///////////////////////////////////////////////////////////////////////////
  method SetValueView {lst args} {
  #
  # PURPOSE: Insertamos los valores en los entrys
  #
  # PARAMETERS: args -> lista con los valores a incluir, el primero es el del
  #             codigo
  #   
  #///////////////////////////////////////////////////////////////////////////
    # limpiamos valores visualizados
    $self values clean
    # introducuimos valores
    set i 0
    foreach obj $blstentry {
      set current [$obj cget -state]
      $obj configure -state normal
      $obj configure -text [lindex $lst $i] 
      $obj configure -state $current
      incr i
    }
  }
    
    
    
#/////////////////////////////////////////////////////////////////////////////
   method _FindCreate { } {
#
# PURPOSE: Creates a Find Dialog given a Instance
#
#/////////////////////////////////////////////////////////////////////////////
  #upvar \#0 ${Instance}::data data

  set dialog [Dialog $self.dg -title [mc "Find"]\
          -default 0 -modal none -parent $self]
  wm protocol $dialog WM_DELETE_WINDOW \
    [list $self _FindDestroy $dialog]  
  set w [$dialog getframe]
  wm resizable $self.dg 0 0
  
  set data(frm,cad) [LabelEntry  $w.eTxt -label "[mc "Find"]: " \
          -textvariable data($self,find,text)]
  checkbutton $w.case  -text [mc "Case sensitive"] \
          -variable data($self,find,case) -pady 0
  TitleFrame $w.dir -bd 2 -relief groove -text [mc "Direction"]
  set f [$w.dir getframe]
  radiobutton $f.forward -text [mc "Forward"] -value forward \
          -variable [varname data(find,dirbusqueda)] \
          -highlightthickness 0 -pady 0
  radiobutton $f.backward -text [mc "Backward"] -value backward \
          -variable [varname data(find,dirbusqueda)] \
          -highlightthickness 0 -pady 0
  Button $w.findNext -text [mc "Find next"] -width 16 \
          -command [list $self _FindNext] \
          -helptext [mc "Find next"]
  Button $w.cancelar -text [mc "Cancel"] -width 16 -helptext "" \
    -command [list $self _FindDestroy $dialog]
  
  bind $dialog <Return> [list $self _FindNext]
  bind $dialog <F3>     [list $self _FindNext]
  bind $dialog <Escape> [list $self _FindDestroy $dialog]

  # Hemos añadido un tag para la selección, hay que quitarle como el tag sel
  #bind $data(editor) <ButtonPress> +[list $data(editor) tag delete found]
  
  grid $f.forward  $f.backward -sticky nw
  
  grid $w.eTxt  -       $w.findNext -sticky news -padx 5 -pady 2
  grid $w.case  $w.dir  $w.cancelar -sticky new  -padx 5
  
  grid columnconfigure $w  0 -weight 1
  grid columnconfigure $w  1 -weight 0
  grid rowconfigure    $w  0 -weight 0
  grid rowconfigure    $w  1 -weight 0
  grid rowconfigure    $w  2 -weight 0
  grid rowconfigure    $w 10 -weight 1
  
  wm geometry $dialog 360x90
  $dialog draw
  update
  focus $data(frm,cad)
  $data(frm,cad).e selection range 0 end
}

#/////////////////////////////////////////////////////////////////////////////
method _FindNext { } {
#
# PURPOSE: Make the next find in editor
#
#/////////////////////////////////////////////////////////////////////////////  
  #  Coger la posición actual del cursor en el text y buscar desde ahí,
  variable data
  
  #dg.frame.eTxt.e
  set entryBus $self.dg.frame.eTxt.e

  #set lstPreVal {}
  #set idxFind {}
  set data($self,indices) [$self entry children root]
  set auxVal "*[$entryBus cget -text]*"
  
  Tolcon_Trace "valBusqueda --> $valBusqueda"
  Tolcon_Trace "valor aux   --> $auxVal"
  
  if {$valBusqueda == ""} {
    Tolcon_Trace "LIMPIO VARIABLES 1"
    set lstPreVal {}
    set idxFind {}
    set vez 0
    set data($self,valBusqueda) *[$entryBus cget -text]*
    set valBusqueda $data($self,valBusqueda)
  } else {
    if {$valBusqueda != $auxVal} {
      Tolcon_Trace "LIMPIO VARIABLES 2"
      set lstPreVal {}
      set idxFind {}
      set vez 0
      set data($self,valBusqueda) $auxVal
    }
  }
  
  if {[llength $data($self,indices)]} {
    foreach idx $data($self,indices) {
       lappend lstPreVal [$self entry cget $idx -data]
    }
  }
  
  Tolcon_Trace "Direccion de búsqueda -> $data(find,dirbusqueda)"
  #Tolcon_Trace "LISTA --> $lstPreVal"
  Tolcon_Trace "combobox -> $self"
  #Tolcon_Trace "lstBox   -> $lstBox"
  #Tolcon_Trace "indices  -> $data($self,indices)"
  #Tolcon_Trace "Valor Buscado -> $data($self,valBusqueda)"
  Tolcon_Trace "VEZ -> $vez"
  
  if {[llength $lstPreVal]} {
    set pos 1
    if {![llength $idxFind]} {
        Tolcon_Trace "BUSCO"
        foreach elem $lstPreVal {
          set aux [lsearch $elem $data($self,valBusqueda)]
          if {$aux != -1} {
            Tolcon_Trace "Encontrado en pos -> $pos"
            lappend idxFind $pos
          }
          incr pos 1
        }
    }
    Tolcon_Trace "LISTA DE POSICIONES ENCONTRADAS --> $idxFind"
    if {[llength $idxFind]} {
      set ok 1 
      Tolcon_Trace "Valor -> $idxFind"
      Tolcon_Trace "Tamaño lista -> [llength $idxFind]"
      $self selection clear 0 end
      if {[llength $idxFind] == 1} {
        Tolcon_Trace "IF"
        Tolcon_Trace "INTENTO POSICIONAR EL CURSOR -> [lindex $idxFind 0]"
        $self selectIndex [lindex $idxFind 0]
      } else {
        Tolcon_Trace "ELSE"
        if {$vez < [llength $idxFind]} {
           Tolcon_Trace "INTENTO POSICIONAR EL CURSOR -> [lindex $idxFind $vez]"
           $self selectIndex [lindex $idxFind $vez]
        } else {
           set rq \
            [tk_messageBox -type yesno -icon question \
              -title [mc "Confirmation. Restart search"] \
              -parent $entryBus \
              -message [mc "You have arrived to the end of list. Do you want to restart search?"]]
          
           if {[string equal $rq yes]} {
                set vez 0
                $self selection clear 0 end
                set lstPreVal {}
                set idxFind {}
                $self _FindNext
                return
           }
        }
      }      
    } else {
      set msg [mc {No coincidence was found in list}]
      ShowMessage W $msg $self
      set ok 0
    }
  } else {
    set msg [mc {No coincidence was found in list}]
    ShowMessage W $msg $self
    set ok 0
  }
    
  incr vez 1
}

#/////////////////////////////////////////////////////////////////////////////
method _FindDestroy {dialog} {
#
# PURPOSE: Replace text in editor
#
#/////////////////////////////////////////////////////////////////////////////
  variable options

  #upvar \#0 ${Instance}::data data
  # inizialization values
  #set options(find,dir)  $data(find,dir)
  #set options(find,case) $data(find,case)

  # destroy
  set vez 0
  set entryBus $self.dg.frame.eTxt.e
  $entryBus configure -text ""
  
  destroy $dialog
}    

    
    
    
    
  # delegation of methods and options
  delegate method * to lstBox
  delegate option * to lstBox
}



 
  

package provide bcomboboxplus 1.0


# ------- TEST CODE ----

#/////////////////////////////////////////////////////////////////////////////
proc test_bcomboboxplus {} {
#
# PURPOSE: Test function bcomboboxplus
#
#/////////////////////////////////////////////////////////////////////////////

  # defining the widgets
  namespace eval test_bcomboboxplus {
    variable var
  }
  set ::var ""
  destroy .top
  toplevel .top
  # key column
  set cc "c2"
  # all columns  
  set ca "c1 c2 c3 c4 c5 c6"
  # hidden columns  
  set ch "c4"
  # labels of bcomboboxplus  
  set clsf { {Label1} {Label 2} {Label 3} {Label 4} {Label 5} {Label 6} }
  # showlabels in bcomboboxplus
  set sl 0  
  set slk 0  
  # width of fields
  set wdh   "10 15 5 5 10 15"
  # blistboxplus content
  set lbc "{1 2 3 4 5 6} {one two three four five six}  \
            {uno dos tres cuatro cinco seis} \
            {k kk kkk kkkk kkkkk kkkkkk} {0 0 0 0 0 0} {@ # $ % | ?}"
  # visible columns of blistboxplus
  set cvLb "c1 c2 c3 c4"
  # hidden columns of blistboxplus  
  set chLb "c5 c6"
  # number of rows of blistboxplus
  set lbr 5 
  # helptextkey
  set htk "helptext del campo clave"
  # helptexts
  set hts { {help c1} {help c2} {help c3} {help c4} {help c5} {help c6} }
  TitleFrame .top.f1 -text "BComboboxPlus test"
  set f1 [ .top.f1 getframe ]
  append tl $cvLb " " $chLb
  
  # Create bcomboboxplus
  set bcbp [bcomboboxplus $f1.bsf -cols $ca -keyhelptext $htk -helptexts $hts \
                        -color "Blue" -width $wdh -parent $f1 \
                        -titles $clsf -showlabels $sl -showlabelkey $slk \
                        -hiddencolslb $chLb -hiddencols $ch -colslb $tl \
                        -colkey $cc -variable "::test_bcomboboxplus::var" -varcol "c1"\
                        -modifycmd   [list puts "executing modifycmd"] \
                        -postcommand [list puts "executing postcommand"] ]
  # insert values in blistboxplus on bcomboboxplus            
  eval $f1.bsf insert end $lbc
  # Create frames 
  TitleFrame .top.f2 -text [mc "Configure"]
  set f2 [ .top.f2 getframe ]
  TitleFrame $f2.f21 -text [mc "Combobox Plus"]
  set f21 [ $f2.f21 getframe ]
  LabelFrame $f21.f211
  set f211 [ $f21.f211 getframe ]
  TitleFrame $f2.f22 -text [mc "List Box"]
  set f22 [ $f2.f22 getframe ]
  # Test comboboxplus
  Label $f21.lf   -text [mc "Labels"]
  Label $f21.lc   -text [mc "Key column"]
  Label $f21.lv   -text [mc "Columns"]
  Label $f21.lh   -text [mc "Hidden columns"]
  Label $f21.lwdh -text [mc "Width of fields"]
  Entry $f21.ef   -width 30 -text $clsf
  Entry $f21.ec   -width 5  -text $cc
  Entry $f21.ev   -width 20 -text $ca 
  Entry $f21.eh   -width 20 -text $ch  
  Entry $f21.ewdh -width 5  -text $wdh
  # variables associated to each checkbutton and buttons
  set ::varCbkl 0
  set ::varCb   0
  checkbutton $f211.cblk -text [mc "Show label key"] \
        -command { toggleShowLabelKey } -variable ::varCblk
  checkbutton $f211.cb -text [mc "Show labels"] \
            -command { toggleShowLabels } -variable ::varCb
  button $f211.bi -text [mc "Apply"] -command { applyCmd } -width 20
  # Test listbox
  Label $f22.lcn -text [mc "Content"]
  Label $f22.lf  -text [mc "Titles"]
  Label $f22.lh  -text [mc "Hidden columns"]
  Label $f22.lr  -text [mc "Number of rows"]  
  Entry $f22.ecn -text $lbc
  Entry $f22.eh  -width 20 -text $chLb
  Entry $f22.er  -text $lbr
  set ::varTlb 1  
  checkbutton $f22.cblb -text [mc "Show titles"] \
            -command { toggleShowTitles } -variable ::varTlb
  button $f22.bd  -text [mc "Apply"] -command { applyLbCmd } -width 20  
  # packing all
  grid .top.f1      -row 0  -column 0 -sticky news -padx 10 -pady 10
  grid .top.f2      -row 1  -column 0 -sticky news -padx 10 -pady 10
  grid columnconfigure .top 0 -weight 1
  grid rowconfigure    .top 0 -weight 0
  grid rowconfigure    .top 1 -weight 1    
  grid $f1.bsf -row 0 -column 0 -sticky "news"
  grid columnconfigure $f1 0 -weight 1
  grid rowconfigure $f1 0  -weight 1
  grid $f2.f21     -row 0  -column 0 -sticky news -padx 10 -pady 10
  grid $f2.f22     -row 0  -column 1 -sticky news -padx 10 -pady 10 
  grid $f21.f211   -row 10 -column 0 -sticky news -columnspan 2
  grid $f21.lc     -row 0  -column 0 -sticky "news" -padx 10 -pady 2
  grid $f21.lv     -row 1  -column 0 -sticky "news" -padx 10 -pady 2
  grid $f21.lh     -row 2  -column 0 -sticky "news" -padx 10 -pady 2
  grid $f21.lf     -row 3  -column 0 -sticky "news" -padx 10 -pady 2
  grid $f21.lwdh   -row 4  -column 0 -sticky "news" -padx 10 -pady 2
  grid $f21.ec     -row 0  -column 1 -sticky "news" -padx 10 -pady 2
  grid $f21.ev     -row 1  -column 1 -sticky "news" -padx 10 -pady 2
  grid $f21.eh     -row 2  -column 1 -sticky "news" -padx 10 -pady 2
  grid $f21.ef     -row 3  -column 1 -sticky "news" -padx 10 -pady 2
  grid $f21.ewdh   -row 4  -column 1 -sticky "news" -padx 10 -pady 2  
  grid $f211.cblk  -row 1  -column 0 -sticky "news" -padx 10 -pady 2
  grid $f211.cb    -row 1  -column 1 -sticky "news" -padx 10 -pady 2
  grid $f211.bi    -row 1  -column 2 -sticky "news" -padx 10 -pady 2
  grid $f22.lcn    -row 1  -column 0 -sticky "news" -padx 10 -pady 2
  grid $f22.lh     -row 3  -column 0 -sticky "news" -padx 10 -pady 2
  grid $f22.lr     -row 4  -column 0 -sticky "news" -padx 10 -pady 2
  grid $f22.ecn    -row 1  -column 1 -sticky "news" -padx 10 -pady 2
  grid $f22.eh     -row 3  -column 1 -sticky "news" -padx 10 -pady 2
  grid $f22.er     -row 4  -column 1 -sticky "news" -padx 10 -pady 2
  grid $f22.bd     -row 5  -column 1 -sticky "news" -padx 10 -pady 2
  grid $f22.cblb   -row 5  -column 0 -sticky "news" -padx 10 -pady 2

  proc applyCmd {} {
    .top.f1.f.bsf configure -titles     [.top.f2.f.f21.f.ef cget -text]
    .top.f1.f.bsf configure -hiddencols [.top.f2.f.f21.f.eh cget -text]
    .top.f1.f.bsf configure -colkey     [.top.f2.f.f21.f.ec cget -text]
    .top.f1.f.bsf configure -width      [.top.f2.f.f21.f.ewdh cget -text]
  }

  proc applyLbCmd {} {
    .top.f1.f.bsf clear
    eval .top.f1.f.bsf insert end [.top.f2.f.f22.f.ecn cget -text]    
    .top.f1.f.bsf configure -colslb [.top.f2.f.f21.f.ev cget -text]
    .top.f1.f.bsf configure -hiddencolslb  [.top.f2.f.f22.f.eh cget -text]
    .top.f1.f.bsf configure -nrows  [.top.f2.f.f22.f.er cget -text]
  }

  proc toggleShowLabels {} {
    if { [.top.f1.f.bsf cget -showlabels] } {
      .top.f1.f.bsf configure -showlabels 0
    } else {
      .top.f1.f.bsf configure -showlabels 1
    }
  }

  proc toggleShowLabelKey {} {
    if { [.top.f1.f.bsf cget -showlabelkey] } {
      .top.f1.f.bsf configure -showlabelkey 0
    } else {
      .top.f1.f.bsf configure -showlabelkey 1
    }
  }
    
  proc toggleShowTitles {} {
    if { [.top.f1.f.bsf cget -showtitles] } {
      .top.f1.f.bsf configure -showtitles false
    } else {
      .top.f1.f.bsf configure -showtitles true
    }
  }
}
