#/////////////////////////////////////////////////////////////////////////////
# FILE    : barima.tcl
# PURPOSE : Editor of definitions of ARIMA models
#/////////////////////////////////////////////////////////////////////////////
package require byscommon
package require Toltcl
package require blistboxplus
package require bcomboboxplus

#/////////////////////////////////////////////////////////////////////////////
::snit::widget barima {
#
# PURPOSE: Editor of ARIMA models.
#
#/////////////////////////////////////////////////////////////////////////////
# SPECIFIC OPTIONS
#  -parent                        # Parent of the widget can be or not an 
#                                   beditmaster
#  -coderequired                  # 1 to show the label of field required
#                                   this next to the entry of the code,
#                                   0 otherwise
#  -codeeditable                  # If has a value 1, it will be possible
#                                   to write on the entry of the code
#  -value                         # Text of the entry of the code ARIMA
#  -timesetrequired               # 1 to show the label of field required
#                                   this next to the combo of the TimeSet,
#                                   0 otherwise
#
# VARIABLES
#  created                        # 1 if the widget has been created
#  widget                         # witgets that compose barima
#  data                           # array with data of the widgets
#  iniconfig                      # section of this widget on ini file
#
#/////////////////////////////////////////////////////////////////////////////
  widgetclass Barima

  option -parent
  option -coderequired    0
  option -codeeditable    0
  option -value           ""
  option -timesetrequired 0
  
  variable widget
  variable data
  variable created 0
  variable iniconfig "BArima"
  
  #///////////////////////////////////////////////////////////////////////////
  typeconstructor {
  #
  # PURPOSE: bsarima typeconstructor
  #
  #///////////////////////////////////////////////////////////////////////////
    global tcl_platform
    global toltk_options
    if { [string equal $tcl_platform(platform) "windows"] } {
      set toltk_options(colors,SystemButtonFace) SystemButtonFace
    } else {
      set toltk_options(colors,SystemButtonFace) #d4d4d4
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  constructor {args} {
  #
  # PURPOSE: Constructor for barima. 
  #          Instanciates all necesary widgets.
  #
  # PARAMETERS:
  #   args ->  List of options for barima.
  #
  #//////////////////////////////////////////////////////////////////////////
    $self configure -parent           [from args -parent ""]
    # Create the widget
    $self createwidget $args
    # Apply all arguments given:
    $self configurelist $args
    $self configure -timesetrequired  [from args -timesetrequired 0]
    # Inicializes the widget
    $self _Init
    # loads the ini config
    $self _ReadIni
    # Widget is now created    
    set created 1
  }
  
  destructor {
    $self _WriteIni
  }
  
  #///////////////////////////////////////////////////////////////////////////
  #
  # PURPOSE: Methods cget & configure
  #
  #///////////////////////////////////////////////////////////////////////////  
  onconfigure -parent { value } {
    set options(-parent) $value
  }
  
  onconfigure -coderequired { value } {
    set options(-coderequired) $value
    if {$value} {
      grid $widget(frcode,r)
    } else {
      grid remove $widget(frcode,r)
    }
  }
  
  onconfigure -timesetrequired { value } {
    set options(-timesetrequired) $value
    if {$value} {
      grid $widget(tfdef,fr,rt)
    } else {
      grid remove $widget(tfdef,fr,rt)
    }
  }
    
  onconfigure -codeeditable { value } {
    set options(-codeeditable) $value
    $widget(frcode,e) configure -editable $value
  }
    
  onconfigure -value { val } {
    set options(-value) $val
    set data(frcode,e) $val
  }
  
  oncget -value {
    return $data(frcode,e)
  }
  
  #///////////////////////////////////////////////////////////////////////////
  # CREATION OF THE WIDGET
  #///////////////////////////////////////////////////////////////////////////

  #///////////////////////////////////////////////////////////////////////////
  method createwidget { args } {
  #
  # PURPOSE: Creation of the objects that compose the widget.
  #
  # PARAMETERS:
  #   args -> List of options for the bsarima.
  #
  #///////////////////////////////////////////////////////////////////////////
    # Creation of the main frames
    set widget(tfdef)   [$self _CreateTfDefinition]
    set widget(lblist)  [$self _CreateListParams]
    set widget(frcode)  [$self _CreateFrameCode]    
    # gridding
    grid $widget(tfdef)  -row 0 -column 0 -sticky news -rowspan 2
    grid $widget(lblist) -row 0 -column 1 -sticky news -columnspan 2
    grid $widget(frcode) -row 1 -column 1 -sticky news      
    grid columnconfigure $self 0 -weight 0
    grid columnconfigure $self 1 -weight 1
    grid rowconfigure    $self 0 -weight 0
    grid rowconfigure    $self 1 -weight 1
  }
  
    
  #///////////////////////////////////////////////////////////////////////////
  method _CreateTfDefinition { } {
  #
  # PURPOSE: Create the title frame wherew the parameters of model ARIMA can
  #          be inserted
  #
  # RETURN: Path of the widget created
  #
  #///////////////////////////////////////////////////////////////////////////
    set widget(tfdef)  [TitleFrame $self.tfdef -text [mc "Parameters"]]
    set widget(tfdef,fr) [$widget(tfdef) getframe]
    # labels
    set widget(tfdef,fr,lt) [label $widget(tfdef,fr).lt \
                                                    -text [mc "TimeSet"]]
    set widget(tfdef,fr,lp) [label $widget(tfdef,fr).lp \
                                                    -text [mc "Period"]]
    set widget(tfdef,fr,ld) [label $widget(tfdef,fr).ld \
                                                    -text [mc "Difference"]]
    set widget(tfdef,fr,la) [label $widget(tfdef,fr).la \
                                                    -text [mc "AR"]]
    set widget(tfdef,fr,lm) [label $widget(tfdef,fr).lm \
                                                    -text [mc "MA"]]
    $self _ClearEntrysDefinition
    # entrys and combos
    set data(ct) ""
    set widget(tfdef,fr,ct) [bcomboboxplus $widget(tfdef,fr).ct \
                             -nrows 5 -cols "timeSet" \
                             -values [::tol::info variable TimeSet] \
                             -variable [varname data(ct)] -showtitles false \
                             -widthlb auto -width 17 \
                             -modifycmd [list $self _ApplyTimeSetToPeriod] \
                             -keyhelptext [mc "Select the TimeSet"] ]
    #$widget(tfdef,fr,ct) activate select
    set widget(tfdef,fr,ep) [bcomboboxplus $widget(tfdef,fr).ep \
                             -nrows 5 -cols "period" -showtitles false \
                             -variable [varname data(tfdef,fr,ep)] \
                             -widthlb auto -width 17 \
                             -keyhelptext [mc "Select or write the Period"] ]
    [$widget(tfdef,fr,ep) getEntryKey] configure \
                           -vcmd {::barima::validationNumber %P 1 99999999999}
    [$widget(tfdef,fr,ep) getEntryKey] configure -validate key
    set widget(tfdef,fr,ed) [bcomboboxplus $widget(tfdef,fr).ed \
                             -variable [varname data(tfdef,fr,ed)] \
                             -nrows 5 -cols "difference" -showtitles false \
                             -widthlb auto -width 17 -values {0 1 2} \
                             -keyhelptext \
                      [mc "Select or write the Differences for this Period"] ]
    [$widget(tfdef,fr,ed) getEntryKey] configure \
                           -vcmd {::barima::validationNumber %P 0 99999999999}
    [$widget(tfdef,fr,ed) getEntryKey] configure -validate key
    set widget(tfdef,fr,ea) [Entry $widget(tfdef,fr).ea \
                             -textvariable [varname data(tfdef,fr,ea)] \
                             -vcmd {::barima::validationARMA %P} \
                             -validate key \
                             -helptext \
                  [mc "Select or write the order of the AR for this Period"] ]
    set widget(tfdef,fr,em) [Entry $widget(tfdef,fr).em \
                             -textvariable [varname data(tfdef,fr,em)] \
                             -vcmd {::barima::validationARMA %P} \
                             -validate key\
                             -helptext \
                  [mc "Select or write the order of the MA for this Period"] ]
    # labels of required field
    set widget(tfdef,fr,rt) [Label $widget(tfdef,fr).rt \
                             -image [::Bitmap::get brequired] \
                             -helptext [mc "TimeSet required"] ]
    set widget(tfdef,fr,rp) [Label $widget(tfdef,fr).rp \
                             -image [::Bitmap::get brequired] \
                             -helptext [mc "Period required"] ]
    set widget(tfdef,fr,rd) [Label $widget(tfdef,fr).rd \
                             -image [::Bitmap::get brequired] \
                             -helptext [mc "Differences required"] ]
    set widget(tfdef,fr,ra) [Label $widget(tfdef,fr).ra \
                             -image [::Bitmap::get brequired] \
                             -helptext [mc "AR part required"] ]
    set widget(tfdef,fr,rm) [Label $widget(tfdef,fr).rm \
                             -image [::Bitmap::get brequired] \
                             -helptext [mc "MA part required"] ]
    # buttons
    set widget(tfdef,fr,bc) [Button $widget(tfdef,fr).bc -relief link \
          -text [mc "Clear"] -helptext [mc "Clear parameters"] \
          -image [::Bitmap::get eraser] -compound left -padx 2 -pady 1 \
          -command [list $self _ClearEntrysDefinition] ]
    set widget(tfdef,fr,bt) [Button $widget(tfdef,fr).bt -relief link \
          -helptext [mc "Move to list"] \
          -image [::Bitmap::get play] -compound left -padx 2 -pady 1 \
          -command [list $self _AddToList] ]
    set widget(tfdef,fr,bf) [Button $widget(tfdef,fr).bf -relief link \
          -helptext [mc "Recove from list"] \
          -image [::Bitmap::get reverse] -compound left -padx 2 -pady 1 \
          -command [list $self _GetFromList] ]
    # gridding
    grid $widget(tfdef,fr,lt) $widget(tfdef,fr,rt) $widget(tfdef,fr,ct) \
                                                                -sticky nws
    grid $widget(tfdef,fr,lp) $widget(tfdef,fr,rp) $widget(tfdef,fr,ep) \
                                                                -sticky nws    
    grid $widget(tfdef,fr,ld) $widget(tfdef,fr,rd) $widget(tfdef,fr,ed) \
                                           $widget(tfdef,fr,bt) -sticky nws
    grid $widget(tfdef,fr,la) $widget(tfdef,fr,ra) $widget(tfdef,fr,ea) \
                                           $widget(tfdef,fr,bf) -sticky nws
    grid $widget(tfdef,fr,lm) $widget(tfdef,fr,rm) $widget(tfdef,fr,em) \
                                                                -sticky nws
    grid x                     x                   $widget(tfdef,fr,bc) \
                                                                 -sticky ns
    grid columnconfigure $widget(tfdef,fr) 1 -weight 0
    return $widget(tfdef)  
  }
  
  
  #///////////////////////////////////////////////////////////////////////////
  method _CreateListParams { } {
  #
  # PURPOSE: Create the blistboxplus with the list of parameters
  #
  # RETURN: Path of the widget created
  #
  #///////////////////////////////////////////////////////////////////////////
    set widget(lblist)  [blistboxplus $self.lblist \
                         -cols {period diff ar ma} \
                         -titles "[mc Period] [mc Difference] AR MA" \
                         -height 120 -nomenu 1]
    $widget(lblist) sort configure -column period
    $widget(lblist) sort auto true
    return $widget(lblist)  
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method _CreateFrameCode { } {
  #
  # PURPOSE: Create the frame with the code created
  #
  # RETURN: Path of the widget created
  #
  #///////////////////////////////////////////////////////////////////////////
    set widget(frcode)  [frame $self.frc]
    
    set widget(frcode,l) [label $widget(frcode).l -text [mc "Code"] ]
    set widget(frcode,e) [Entry $widget(frcode).e \
                          -textvariable [varname data(frcode,e)] \
                          -editable false \
                          -helptext [mc "Code of the ARIMA model"]]
    set widget(frcode,r) [Label $widget(frcode).r \
                         -image [::Bitmap::get brequired] \
                         -helptext [mc "Code of the ARIMA model required"] ]
    grid $widget(frcode,l)  -row 0 -column 0 -sticky nws
    grid $widget(frcode,r)  -row 0 -column 1 -sticky nws
    grid $widget(frcode,e)  -row 0 -column 2 -sticky news
    # if its father is an edit master, shows the button add code
    if { !([$self cget -parent] eq "") && \
          ([winfo class [$self cget -parent]] eq "BEditMaster") } {
      set widget(frcode,bu) [Button $widget(frcode),bu  -relief link \
            -helptext [mc "Add code as key"] -text [mc "Add code"] \
            -image [::Bitmap::get upNext] -compound left \
            -padx 2 -pady 0 -command [list $self _UpdateBEditMaster] ]
      grid $widget(frcode,bu) -row 1 -column 2
    }
    grid columnconfigure $widget(frcode) 0 -weight 0
    grid columnconfigure $widget(frcode) 1 -weight 0
    grid columnconfigure $widget(frcode) 2 -weight 1
    return $widget(frcode)
  
  }  
  
  #///////////////////////////////////////////////////////////////////////////
  method _Init { } {
  #
  # PURPOSE: Initializes the widget once created. It assigns predefined
  #          periods to certain TimeSets and initializes other components of
  #          the widget
  #
  #///////////////////////////////////////////////////////////////////////////
    if { !($created) } {
      # assigns predefined periods to choose to certain TimeSets
      set data(timeSet,Diario)     {1 7 365}
      set data(timeSet,Daily)      $data(timeSet,Diario)
      set data(timeSet,Semanal)    {1 52}
      set data(timeSet,Weekly)     $data(timeSet,Semanal)
      set data(timeSet,Mensual)    {1 12}
      set data(timeSet,Monthly)     $data(timeSet,Mensual)
      set data(timeSet,Trimestral) {1 4}
      set data(timeSet,Quarterly)  $data(timeSet,Trimestral)
      set data(timeSet,Anual)      {1}
      set data(timeSet,Yearly)      $data(timeSet,Anual)
      # assigns predefined periods by default
      set data(timeSet,barimadefault) {}
      for {set i 0} {$i < 365} {incr i 1} {
        lappend data(timeSet,barimadefault) $i
      }
      # inicializes the period 
      $self _ApplyTimeSetToPeriod
    }    
  }
      
  #///////////////////////////////////////////////////////////////////////////
  # BEHAVIOUR OF THE WIDGET
  #///////////////////////////////////////////////////////////////////////////
  
  #///////////////////////////////////////////////////////////////////////////
  method _ClearEntrysDefinition { } {
  #  
  # PURPOSE: Add the items to the list
  #    
  #///////////////////////////////////////////////////////////////////////////
    set data(tfdef,fr,ep) ""
    set data(tfdef,fr,ed) ""
    set data(tfdef,fr,ea) ""
    set data(tfdef,fr,em) ""
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method _AddToList { } {
  #  
  # PURPOSE: Adds the items to the entrys of the definition of model to
  #          blistboxplus
  #    
  #///////////////////////////////////////////////////////////////////////////
    if { [$self _AllFieldsFilled] } { ;# all the entrys are filled
      if { [$self _ValidateFields] } { ;# its values are valid
    # apply the period to the parameters of the part AR and MA (disabled)
        # set data(tfdef,fr,ea) [$self _ApplyPeriod $data(tfdef,fr,ep) \
                                                  $data(tfdef,fr,ea) ]
        # set data(tfdef,fr,em) [$self _ApplyPeriod $data(tfdef,fr,ep) \
                                                  $data(tfdef,fr,em) ]
        # inserts it on the list
        $widget(lblist) insert end "$data(tfdef,fr,ep) $data(tfdef,fr,ed) \
                                   $data(tfdef,fr,ea) $data(tfdef,fr,em)"
        # clean the entrys of definition of model and it updates the code
        $self _ClearEntrysDefinition
        $self _UpdateCode
      }      
    } else { ;# some parameter has not filled up
      ShowMessage E [mc "All the parameters of the model must be filled"] \
                  $self
    }
  }  
  
  #///////////////////////////////////////////////////////////////////////////
  method _AllFieldsFilled { } {
  #  
  # PURPOSE: It verifies that all entrys of definition of the model has been
  #          filled up
  #
  # RETURN: 1 if all the entrys are filled up, 0 otherwise
  #
  #///////////////////////////////////////////////////////////////////////////
    set result 1
    if { ($data(tfdef,fr,ep) eq "") || ($data(tfdef,fr,ed) eq "") || \
         ($data(tfdef,fr,ea) eq "") || ($data(tfdef,fr,em) eq "") } {
      set result 0
    }
    return $result
  }
   
     
  #///////////////////////////////////////////////////////////////////////////
  method _ValidateFields { } {
  #  
  # PURPOSE: It verifies the consistency of the values introduced in entrys
  #          of definition of the model. It handles the error messages
  #
  # RETURN: 1 in case that the values are consistent, 0 otherwise
  #
  #///////////////////////////////////////////////////////////////////////////
    set result 1
    # It builds a list of existing periods
    set lstPeriods {}
    foreach id [$widget(lblist) get] {
      lappend lstPeriods [$widget(lblist) get $id period]
    }
    # verifies if the period already existed
    if { ([lsearch -exact $lstPeriods $data(tfdef,fr,ep)] eq "-1") } { 
      if {[$self _ParseParameters $data(tfdef,fr,ea)]} { ;# parse the AR
        set data(tfdef,fr,ea) \
                    [$self _FixConsecutiveParameters $data(tfdef,fr,ea)]
      } else { ;# estructura de AR incorrecta
        ShowMessage E [mc "The parameters of AR are not valid"] $self
        set result 0      
      }
      if {[$self _ParseParameters $data(tfdef,fr,em)]} { ;# parse the MA
        set data(tfdef,fr,em) \
                    [$self _FixConsecutiveParameters $data(tfdef,fr,em)]
      } else { ;# structure of the MA incorrect
        ShowMessage E [mc "The parameters of MA are not valid"] $self
        set result 0
      } 
    } else { ;# error: the period does not have to exist
      ShowMessage E \
           "[mc "The period"] $data(tfdef,fr,ep) [mc "already exists"]" $self
      set result 0
    }    
    return $result  
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method _ParseParameters {value} {
  #  
  # PURPOSE: It verifies the consistency of the values introduced in entrys
  #          of definition of AR or MA:
  #          example X.Y..Z
  #              - Z must be greater than Y
  #              - Y must be greater than X
  #              - All these values must be separated of . or of ..
  #              - Consecutive values must be separated of . and not of
  #                ..
  # PARAMETERS:
  #   value -> value of the parameter AR o MA introduced
  #
  # RETURN: 1 in case that the values are consistent, 0 otherwise
  #
  #///////////////////////////////////////////////////////////////////////////
    set result 1
    if { ([string last . $value] eq [expr [string length $value] - 1]) ||
         ([string first . $value] eq 0)} { ;# ends or starts of .
      set result 0
    } else { ;# don't ends of .
      # verifies that Z must be greater than Y... 
      # divided by . 
      set lstOri [split $value .]
      # delete the { } that corresponds with ..
      set lstDst {}
      foreach item $lstOri {
        if { !($item eq "") } {
          lappend lstDst $item        
        }
      }
      # verifies the order
      set itemAux "-1"
      foreach item $lstDst {
        if {[expr $itemAux + 1] > $item} {
          set result 0
        }
        set itemAux $item
      }
    }
    return $result
  }
  
  
  #///////////////////////////////////////////////////////////////////////////  
  method _FixConsecutiveParameters { str } {
  #
  # PURPOSE: Deletes the .. in the case of en el caso that it separates
  #          consecutive numbers in the definition of an model AR o MA.
  #          By example 2..3 has not sense, it would be 2.3. This function
  #          returns the string without this possible inconsistency
  #
  # PARAMETERS: The string to analyze.
  #
  # RETURN: The chain with this possible inconsistency corrected.
  #
  #///////////////////////////////////////////////////////////////////////////
    set strResult ""
    # The chain is divided by . and the variables are initialized
    set lstOri [split $str .]
    set nAux 0
    set double 0
    # iterates on the list
    foreach item $lstOri {
       # appears a .
       if { $item eq {} } {
        set double 1  ;# indicates that it has appeared a .
        append strResult .
      } else {
        if {$double} { ;# have been ..
          if {!($item eq [expr $nAux + 1]) } { ;# the numbers ARE NOT
                                               ;# consecutive
            # the ... are given normally
            append strResult $item
            append strResult .            
          } else { ;# the numbers ARE consecutive:  it is corrected
            # deletes a .
            set strResult [string range $strResult 0 \
                                  [expr [string length $strResult] - 2 ] ]
            append strResult $item
            append strResult .
          }
        } else {  ;# have not been ..
          append strResult $item
          append strResult .
        }
        # the variables are prepared to begin
        set nAux $item
        set double 0
      }
    }
    # a point of the end is eliminated and the result is given back
    set strResult [string range $strResult 0 \
                          [expr [string length $strResult] - 2 ] ]
    return $strResult
  }
    
    
  #///////////////////////////////////////////////////////////////////////////  
  method _UpdateCode { } {
  #  
  # PURPOSE: It updates the content of the code based on the elements
  #         introduced in blistboxplus
  #
  #///////////////////////////////////////////////////////////////////////////
    set data(frcode,e) [$self _GetCode]    
  }
    
  #///////////////////////////////////////////////////////////////////////////  
  method _GetFromList { } {
  #  
  # PURPOSE: It takes the selected row from blistboxplus and load it in entrys
  #          of definition of the model
  #
  #///////////////////////////////////////////////////////////////////////////
    # obtains the selection of blistboxplus and eliminates it
    set idSel [$widget(lblist) selection get]
    if { (!([llength [$widget(lblist) get]] eq 0)) && \
         (!($idSel eq "")) } {  ;# there is nothing or nothing
                                                # have selected
      # Cleans entrys of definition of the model
      $self _ClearEntrysDefinition
      set selected [$widget(lblist) get $idSel]

      $widget(lblist) delete $idSel

      # translates and passes its content to entrys
      set data(tfdef,fr,ep) [lindex $selected 0]
      set data(tfdef,fr,ed) [lindex $selected 1]
      set data(tfdef,fr,ea) [lindex $selected 2]
      set data(tfdef,fr,em) [lindex $selected 3]

      #set data(tfdef,fr,ea) [$self _ApplyPeriod [lindex $selected 0] \
                                              [lindex $selected 2] "backward"]
      #set data(tfdef,fr,em) [$self _ApplyPeriod [lindex $selected 0] \
                                              [lindex $selected 3] "backward"]
                                              
      # once brought to entrys, it updates the code
      $self _UpdateCode
      $widget(lblist) first
      if { [llength [$widget(lblist) get]] eq 0 } {
        set data(frcode,e) ""
      }
    }
  }
  
  #///////////////////////////////////////////////////////////////////////////  
  method _GetCode { } {
  # 
  # PURPOSE: Returns the code ARIMA based on the content of blistboxplus with
  #          all its parameters
  #
  # RETURN: A string with the code
  #
  #///////////////////////////////////////////////////////////////////////////
    set strP ""
    set strD ""
    set strA ""    
    set strM ""
    # obtains the list of identifiers without order
    set lstIds    [$widget(lblist) get]
    # the list of identifiers by the period is ordered
    # obtains a list of {id period}... is the same as {value index}
    set lstValues {}
    foreach id $lstIds {
      set period [$widget(lblist) get $id period]
      lappend lstValues "$id $period"
    }
    # the list of values (ids) is ordered by the index (values)    
    set lstValues [lsort -index 1 $lstValues]
    # the list of values (ids) ordered is obtained    
    set lstIds {}
    foreach pair $lstValues {
      lappend lstIds [lindex $pair 0]
    }    
    # iterates on the list ordered of identifiers     
    foreach id $lstIds {
      # constructs the P part
      set itemP [$widget(lblist) get $id period]
      append strP $itemP "_"
      # constructs the DIF part
      set itemD [$widget(lblist) get $id diff]      
      append strD $itemD "_"
      # constructs the AR part 
      set itemA [$widget(lblist) get $id ar]
      append strA $itemA "_"
      # constructs the MA part 
      set itemM [$widget(lblist) get $id ma]
      append strM $itemM "_"
    }
    # clears _ that exceeds in the end
    set strP [string range $strP 0 [expr [string length $strP] - 2 ] ]
    set strD [string range $strD 0 [expr [string length $strD] - 2 ] ]
    set strA [string range $strA 0 [expr [string length $strA] - 2 ] ]
    set strM [string range $strM 0 [expr [string length $strM] - 2 ] ]
    # joins everything to form code ARIMA
    set result ""
    append result "P" $strP "DIF" $strD "AR" $strA "MA" $strM
    # return the result
    return $result
  }
      
  #///////////////////////////////////////////////////////////////////////////        
  method _ApplyPeriod {period str {sense "fordward"}} {
  #
  # PURPOSE: Applies a period to patameters AR or MA of the model.
  #          Either from entrys to blistboxplus (forward) or
  #          from blistboxplus to entrys (backward)
  #
  # PARAMETERS: 
  #  period -> period selected
  #  str -> parameters AR o MA to treat
  #  sense -> sense of the treatment
  #
  # RETURN: Original string with the applied period
  #
  # TODO: Disabled to give greater flexibility
  #///////////////////////////////////////////////////////////////////////////    
    set strResult ""
    # split the string by .
    set lstOri [split $str .]
    set lstDst {}
    set firstItem "-1"
    foreach item $lstOri {
      # ignoramos los .
      if {$item eq {}} {
        lappend lstDst {}
      } else {
        if {$item eq 0} { ;# the 0's are left
          lappend lstDst $item
        } else {
          # applies the period forwards or backwards
          if { $sense eq "fordward"} {          
            lappend lstDst [expr $period + $item - 1]
          } else {
            lappend lstDst [expr $item - $period + 1]
          }
        }
      }
    }
    # recomposes string to give back it
    set strResult 
    foreach item $lstDst {
      append strResult $item .
    }
    # clears end . that exceeds and retuns the result
    set strResult [string range $strResult 0 \
                                     [expr [string length $strResult] - 2 ] ]
    return $strResult
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method _UpdateBEditMaster { } {
  #
  # PURPOSE: Updates the code of the beditmaster when click on update code
  #          button
  #///////////////////////////////////////////////////////////////////////////
    if { !($data(frcode,e) eq "") } {
      [$self cget -parent] showvalue -status ins -values \
                          [list [list code $data(frcode,e) ] ]
    }
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method _ApplyTimeSetToPeriod { } {
  #
  # PURPOSE: Gives the values of the selected TimeSet
  #
  #///////////////////////////////////////////////////////////////////////////
    if {$created} {
      set name "timeSet,$data(ct)"
      if { !([array names data $name] eq "") } {
        $widget(tfdef,fr,ep) configure -values $data($name)
      } else {
        $widget(tfdef,fr,ep) configure -values $data(timeSet,barimadefault)
      }
    }
  }
  
  
  #/////////////////////////////////////////////////////////////////////////////
  method _WriteIni {} {
  #
  # PURPOSE: Write to the ini file the TimeSet selected
  #
  #/////////////////////////////////////////////////////////////////////////////
    set wini ::iniFile::Write
    $wini $iniconfig TimeSet [$self get "TimeSet"]    
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _ReadIni {} {
  #
  # PURPOSE: Read from ini file the TimeSet selected
  #
  #/////////////////////////////////////////////////////////////////////////////
    set rini ::iniFile::Read  
    set data(ct) [$rini $iniconfig TimeSet ""]
    # apply the TimeSet to the widget
    $self _ApplyTimeSetToPeriod
  }
  
  #///////////////////////////////////////////////////////////////////////////
  # PUBLIC METHODS
  #///////////////////////////////////////////////////////////////////////////
  
  #///////////////////////////////////////////////////////////////////////////
  method get { args } {
  #
  # PURPOSE: Returns a string with certain information of widget.
  #
  # PARAMETERS:
  #   args -> If has a value
  #           "TimeSet" returns the TimeSet selected
  #
  # RETURNS: The string with the information required
  #
  #///////////////////////////////////////////////////////////////////////////
    if { $args eq "TimeSet" } {
      return $data(ct)
    }
  }
  
  #///////////////////////////////////////////////////////////////////////////
  method state {stt args} {
  #
  # PURPOSE: We applied an option to the elements of the component.
  #          To entry of the code, the button and the rest of fields entry.
  #
  # PARAMETERS:
  #   stt: option to apply
  #     normal    -> ep(y) ed(y) ea(y) em(y) e(y) bc(y) bt(y) bf(y)
  #     disabled  -> ep(n) ed(n) ea(n) em(n) e(n) bc(n) bt(n) bf(n)
  #
  #///////////////////////////////////////////////////////////////////////////
  # introduces values
    switch $stt {
      "normal" {
        BfoActivateWidget $widget(tfdef,fr,ep)   "normal"
        BfoActivateWidget $widget(tfdef,fr,ed)   "normal"
        BfoActivateWidget $widget(tfdef,fr,ea)   "normal"
        BfoActivateWidget $widget(tfdef,fr,em)   "normal"
        BfoActivateWidget $widget(frcode,e)      "normal"
        BfoActivateWidget $widget(tfdef,fr,bc)   "normal"
        BfoActivateWidget $widget(tfdef,fr,bt)   "normal"
        BfoActivateWidget $widget(tfdef,fr,bf)   "normal"
        if {info exists $widget(frcode,bu) } {
          BfoActivateWidget $widget(frcode,bu)     "normal"
        }
      }
      "disabled" {
        #BfoActivateWidget $widget(tfdef,fr,ep)   "disabled"
        #BfoActivateWidget $widget(tfdef,fr,ed)   "disabled"
        #BfoActivateWidget $widget(tfdef,fr,ea)   "disabled"
        #BfoActivateWidget $widget(tfdef,fr,em)   "disabled"
        #BfoActivateWidget $widget(frcode,e)      "disabled"
        #BfoActivateWidget $widget(tfdef,fr,bc)   "disabled"
        #BfoActivateWidget $widget(tfdef,fr,bt)   "disabled"
        #BfoActivateWidget $widget(tfdef,fr,bf)   "disabled"
        BfoActivateWidget $widget(frcode,bu)     "disabled"
      }
      default {
        puts "BSArima Activate ERROR: invalid state: $stt"
      }
    }
  }    
} ;#snit::widget

package provide barima 1.0

#/////////////////////////////////////////////////////////////////////////////
namespace eval ::barima {
#/////////////////////////////////////////////////////////////////////////////
}

#/////////////////////////////////////////////////////////////////////////////
proc ::barima::validationNumber {value min max} {
#
# PURPOSE: Validate that the numbers introduced in entry are numbers, and
#          they are in a rank of certain values.
#          It returns 1 if the introduced number is greater than the minimum,
#          minor of the maximum or this even empty.
# 
# RETURN: 1 if the content of the entry is valid. 0 otrherwise
#
# TODO: To put within widget
#/////////////////////////////////////////////////////////////////////////////
  set result 1
  if {($value eq "")} { ;# entry empty
    set result 1
  } else {
    if { [string is digit $value] eq 1 } { ;# are digits
      if { (($value >= $min) && ($value <= $max)) } { ;# digits in range
        set result 1
      } else { ;# digits out of range
        set result 0
      }
    } else { ;# are't digits
      set result 0
    }
  }
  return $result
}

  
#/////////////////////////////////////////////////////////////////////////////
proc ::barima::validationARMA {value} {
#
# PURPOSE: Validate that the numbers introduced in the entry of definition
#          the model are valid
# 
# RETURN: 1 if the content of the entry is valid. 0 otrherwise
#
# TODO: To put within widget
#/////////////////////////////////////////////////////////////////////////////
  set result 1
  if {($value eq "")} { ;# entry empty
    set result 1
  } else {
    ;# wants to introduce .. or string  begins by .    
    if { (!([string last ... $value] eq "-1"))} { 
      set result 0
    } else { ;# is not wanted to introduce more than ..      
      set copyValue [string map {. 0} $value] ;# replace the . by 0
      if { [string is digit $copyValue] eq 1 } { ;# are digits
        set result 1
      } else { ;# aren't digits
        set result 0
      }
    }
  }
  return $result
}

proc test_barima { } {  
  destroy .top
  toplevel .top
  set tf1 [TitleFrame .top.f1 -text "BArima test"]
  set f1 [ .top.f1 getframe ]  
  
  # Create barima
  set ba [barima $f1.ba]
  puts "barima widget: $ba"
  
  pack $tf1 -expand yes -fill both
  pack $ba -expand yes -fill both
}