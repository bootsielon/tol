#/////////////////////////////////////////////////////////////////////////////
# FILE    : bsersel.tcl
# PURPOSE : This file 
#/////////////////////////////////////////////////////////////////////////////

# Requiere funciones de acceso a archivo .ini ::iniFile::*
# Ver test al final del archivo
package require snit
package require Tk
package require BLT
package require byscommon


#/////////////////////////////////////////////////////////////////////////////
::snit::type bsersel {
#
# PURPOSE: Defincición de las funcionalidades de un 
#
#/////////////////////////////////////////////////////////////////////////////

#
#/////////////////////////////////////////////////////////////////////////////
  delegate option * to hull

  option -project   {} ;# Nombre del proyecto
  option -inientry  {} ;# Entrada en el .ini
  option -def       {} ;# Nombre del cjto TOL con la definicion del algebra
  
  variable tmp     ;# Valores temporales de los elementos seleccionados
    # para selUni.numBaj  - tmp($mag,$dim,$sDi)      <-- $val
    # para selMul.numBaj  - tmp($mag,$dim,$sDi,$val) <-- 0|1
    # para selUni.numAlt  - método get de bselectfields tmp($mag,$dim,$sDi)
    # para selMul.numAlt  - método get de beditselector tmp($mag,$dim,$sDi)

  variable alg     ;# Meta información del álgebra
    # alg(TCL,<key>)                    - Valor de clave TCL $-key
    # alg(Mag,-cod,$mag)                - Codigo de magnitud
    # alg(Mag,-nam,$mag)                - Nombre de magnitud
    # alg(Mag,-des,$mag)                - Descripcion de magnitud
    # alg(Mag,TCL,<key>,$mag)           - Valor de clave TCL $-key
    # alg(Dim,$mag,-cod,$dim)           - Codigo de dimension 
    # alg(Dim,$mag,-nam,$dim)           - Nombre de dimension 
    # alg(Dim,$mag,-des,$dim)           - Descripcion de dimension 
    # alg(Dim,$mag,-req,$dim)           - Si la dimension es requirida o no
    # alg(Dim,$mag,TCL,<key>,$dim)      - Valor de clave TCL $-key
    # alg(sDi,$mag,$dim,-cod,$sDi)      - Codigo de subdimension 
    # alg(sDi,$mag,$dim,-nam,$sDi)      - Nombre de subdimension 
    # alg(sDi,$mag,$dim,-des,$sDi)      - Descripcion de subdimension 
    # alg(sDi,$mag,$dim,-req,$sDi)      - Si la subdimension es requerida o no
    # alg(sDi,$mag,$dim,-typ,$sDi)      - Tipo de la subdimension
    # alg(sDi,$mag,$dim,-dex,$sDi)      - Lista dependencias existenciales
    # alg(sDi,$mag,$dim,-hie,$sDi)      - Lista de valores jerárquicos
    # alg(sDi,$mag,$dim,-dva,$sDi)      - Lista dependencias de valor
    # alg(sDi,$mag,$dim,TCL,<key>,$sDi) - Valor de clave TCL $-key 

  variable data    ;# Diversas informaciones
    # data(iniEntry)      - entrada en el archivo .ini para options
    # data(project)       - Nombre del proyecto

  variable options ;# Opciones que se guardan en archivo .ini
    # options(wform)      - ancho porcentual del formulario de selección
    # options(wlist)      - ancho porcentual de la lista de series

  variable algwid  ;# Rutas de widgets del algebra creados
    # algwid($mag,$dim,$sDi) - frame interno de cada subdimension 

  variable widget  ;# Rutas de widgets generales creados
    # widget(tl)          - toplevel donde se crea el selector de series
    # widget(tabset)      - tabset que separa el selector por magnitudes
    # widget(panel,form)  - panel del formulario de seleccion de series
    # widget(panel,list)  - panel del la lista de series
    # widget(output)      - listbox con la selección de series

  #///////////////////////////////////////////////////////////////////////////
  constructor { args } {
  #
  # PURPOSE: bsersel constructor
  #
  # PARAMETERS:
  #   args -> options
  #
  #///////////////////////////////////////////////////////////////////////////
    # Apply all options passed at creation time.
    $self configurelist $args
    set data(project)  [$self cget -project]
    set data(iniEntry) [$self cget -inientry]
    set data(alreadytriggering) 0
    $self _readini
  }

  #///////////////////////////////////////////////////////////////////////////
  #
  # PURPOSE: Metodos cget y configure
  #
  #///////////////////////////////////////////////////////////////////////////


  # delegacion de metodos y opciones
  delegate method * to hull
  delegate option * to hull

  #///////////////////////////////////////////////////////////////////////////
  method _init {} {
  #
  # PURPOSE: Inicializa el array alg con la informacion del algebra
  #
  #///////////////////////////////////////////////////////////////////////////
    set def [$self cget -def]

    #Información general
    set nRes [GetUniqueName Text _GuiAlgDim]
    set cmd "Text $nRes = GuiAlgGetDef($def);"
    ::tol::console eval $cmd
    array set info [TclInfoVar Text $nRes]
    eval "set lstDefTcl $info(CONTENT)"
    ::tol::console stack release $nRes
    foreach {key value} $lstDefTcl {
      set alg(TCL,$key) $value
    }

    # Obtener magnitudes
    set cmd "Text $nRes = GuiAlgGetMag($def);"
    ::tol::console eval $cmd
    array set info [TclInfoVar Text $nRes]
    eval "set lstMag $info(CONTENT)"
    ::tol::console stack release $nRes
    set idxMag 0
    foreach mag $lstMag {
      # Guardar magnitud
      set codMag [lindex $mag 0]
      set alg(Mag,-idx,$codMag) $idxMag
      set alg(Mag,-cod,$codMag) $codMag
      set alg(Mag,-nam,$codMag) [lindex $mag 1]
      set alg(Mag,-des,$codMag) [lindex $mag 2]
      #Info TCL
      set cmd "Text $nRes = GuiAlgGetMagTclSet(\"$codMag\",$def);"
      ::tol::console eval $cmd
      array set info [TclInfoVar Text $nRes]
      eval "set lstMagTcl $info(CONTENT)"
      ::tol::console stack release $nRes
      foreach {key value} $lstMagTcl {
        set alg(Mag,TCL,$key,$codMag) $value
      }
      #Obtener dimensiones
      set nRes [GetUniqueName Text _GuiAlgDim]
      set cmd "Text $nRes =  GuiAlgGetDim(\"$codMag\",$def);"
      ::tol::console eval $cmd
      array set info [TclInfoVar Text $nRes]
      eval "set lstDim $info(CONTENT)"
      ::tol::console stack release $nRes
      set idxDim 0
      foreach dim $lstDim {
        # Guardar dimension
        set codDim [lindex $dim 0]
        set alg(Dim,$codMag,-idx,$codDim) $idxDim
        set alg(Dim,$codMag,-cod,$codDim) $codDim
        set alg(Dim,$codMag,-nam,$codDim) [lindex $dim 1]
        set alg(Dim,$codMag,-des,$codDim) [lindex $dim 2]
        set alg(Dim,$codMag,-req,$codDim) [lindex $dim 3]
        #Info TCL
        set cmd "Text $nRes = \
          GuiAlgGetNodeTclSet(\"$codMag\",\"$codDim\",\"\",$def);"
        ::tol::console eval $cmd
        array set info [TclInfoVar Text $nRes]
        eval "set lstDimTcl $info(CONTENT)"
        ::tol::console stack release $nRes
        foreach {key value} $lstDimTcl {
          set alg(Dim,$codMag,TCL,$key,$codDim) $value
        }
        # Obtener subdimensiones
        set nRes [GetUniqueName Text _GuiAlgDim]
        set cmd "Text $nRes = \
          GuiAlgGetSubDim(\"$codMag\",\"$codDim\",$def);"
        ::tol::console eval $cmd
        array set info [TclInfoVar Text $nRes]
        eval "set lstSubDim $info(CONTENT)"
        ::tol::console stack release $nRes
        set idxSDi 0
        foreach sDi $lstSubDim {
          # Guardar subdimension
          set codSDi [lindex $sDi 0]
          set alg(sDi,$codMag,$codDim,-idx,$codSDi) $idxSDi
          set alg(sDi,$codMag,$codDim,-cod,$codSDi) $codSDi
          set alg(sDi,$codMag,$codDim,-nam,$codSDi) [lindex $sDi 1]
          set alg(sDi,$codMag,$codDim,-des,$codSDi) [lindex $sDi 2]
          set alg(sDi,$codMag,$codDim,-req,$codSDi) [lindex $sDi 3]
          set alg(sDi,$codMag,$codDim,-typ,$codSDi) [lindex $sDi 4]
          # Dimensiones que dependen existencialmente de la subdimension 
          set cmd "Text $nRes = \
            GuiAlgGetExiDepSubDim(\"$codMag\",\"$codDim\",\"$codSDi\",$def);"
          ::tol::console eval $cmd
          array set infExi [TclInfoVar Text $nRes]
          eval "set lstSubDimExi $infExi(CONTENT)"
          ::tol::console stack release $nRes
#Tolcon_Trace "$codDim $codSDi $lstSubDimExi"
          set alg(sDi,$codMag,$codDim,-dex,$codSDi) $lstSubDimExi
          # Subdimensiones que dependen por valor de la subdimension
          set cmd "Text $nRes = \
            GuiAlgGetValDepSubDim(\"$codMag\",\"$codDim\",\"$codSDi\",$def);"
          ::tol::console eval $cmd
          array set infVal [TclInfoVar Text $nRes]
          eval "set lstSubDimVal $infVal(CONTENT)"
          ::tol::console stack release $nRes
#Tolcon_Trace "$codDim $codSDi $lstSubDimVal"
          set alg(sDi,$codMag,$codDim,-dva,$codSDi) $lstSubDimVal
          #Info TCL
          set cmd "Text $nRes = \
            GuiAlgGetNodeTclSet(\"$codMag\",\"$codDim\",\"$codSDi\",$def);"
          ::tol::console eval $cmd
          array set info [TclInfoVar Text $nRes]
          eval "set lstSubDimTcl $info(CONTENT)"
          ::tol::console stack release $nRes
          foreach {key value} $lstSubDimTcl {
            set alg(sDi,$codMag,$codDim,TCL,$key,$codSDi) $value
          }
          incr idxSDi
        }
        incr idxDim
      }
      incr idxMag
    }
    parray alg    
  }

  #///////////////////////////////////////////////////////////////////////////
  method _getMag {} {
  #
  # PURPOSE: Gets list of magnitud
  #
  # PARAMETERS: 
  #
  #///////////////////////////////////////////////////////////////////////////
    regsub -all Mag,-cod,\
      [array names ${selfns}::alg Mag,-cod,*] "" lstMag
    set lst ""
    foreach mag $lstMag {
      lappend lst [list $mag $alg(Mag,-idx,$mag)]
    }
    set lstOrd [lsort -integer -index 1 $lst]
    set lstFin ""
    foreach reg $lstOrd {
      lappend lstFin [lindex $reg 0]
    }
    return $lstFin
  }

  #///////////////////////////////////////////////////////////////////////////
  method _getDim {mag} {
  #
  # PURPOSE: Gets list of dimensions of a given magnitud in order of creation
  #
  # PARAMETERS: 
  #   mag -> Magnitud
  #
  #///////////////////////////////////////////////////////////////////////////
    regsub -all Dim,$mag,-cod,\
      [array names ${selfns}::alg Dim,$mag,-cod,*] "" lstDim
    set lst ""
    foreach dim $lstDim {
      lappend lst [list $dim $alg(Dim,$mag,-idx,$dim)]
    }
    set lstOrd [lsort -integer -index 1 $lst]
    set lstFin ""
    foreach reg $lstOrd {
      lappend lstFin [lindex $reg 0]
    }
    return $lstFin
  }

  #///////////////////////////////////////////////////////////////////////////
  method _getSubDim {mag dim} {
  #
  # PURPOSE: Gets list of subdimension of a given dimension in order of
  #          creation
  #
  # PARAMETERS: 
  #   mag -> Magnitud
  #   dim -> Dimension
  #
  #///////////////////////////////////////////////////////////////////////////
    regsub -all sDi,$mag,$dim,-cod, \
      [array names ${selfns}::alg sDi,$mag,$dim,-cod,*] "" lstSDi
    set lst ""
    foreach sDi $lstSDi {
      lappend lst [list $sDi $alg(sDi,$mag,$dim,-idx,$sDi)]
    }
    set lstOrd [lsort -integer -index 1 $lst]
    set lstFin ""
    foreach reg $lstOrd {
      lappend lstFin [lindex $reg 0]
    }
    return $lstFin
  }

  #///////////////////////////////////////////////////////////////////////////
  method create {tl } {
  #
  # PURPOSE: Creates a Windows with series selector and series list.
  #          Actually creates a tab for each magnitud
  #
  # PARAMETERS: 
  #
  #///////////////////////////////////////////////////////////////////////////
    global toltk_options
    # Inicilaizamos alg
    $self _init

    set widget(tl) $tl 
    set w [lindex [split $tl .] end]
    option add *TitleFrame.ipad             2 user
    option add *$w*Radiobutton.padY         0 user
    option add *$w*Radiobutton.borderWidth  0 user
    option add *$w*Checkbutton.padY         0 user
    option add *$w*Checkbutton.borderWidth  0 user
    option add *$w*Checkbutton.font {Arial 8} user
    option add *$w*Radiobutton.font {Arial 8} user
  
    set pw [PanedWindow $tl.pw -side bottom -pad 0]
    set widget(panel,form) [set pane1 [$pw add -weight $options(wform)]]
    set widget(panel,list) [set pane2 [$pw add -weight $options(wlist)]]

    # Lista de series
    set flis [$self _createlist $pane2]
    set bbox [$self _createbbox $pane2]
    grid $bbox -column 0 -row 0 -sticky news
    grid $flis -column 1 -row 0 -sticky news
    grid columnconfigure $pane2 1 -weight 1
    grid rowconfigure    $pane2 0 -weight 1
  
    # Selectores de series
    set widget(tabset) [set w_tabset [::blt::tabset $pane1.ts -relief flat \
              -highlightthickness 0 -bd 0 -outerpad 0 -tier 2 -slant right \
              -textside right -selectcommand [list $self _changeMag]]]
  
    # lista de magnitudes
    foreach mag [$self _getMag] {
      $w_tabset insert end $mag
      set w     [frame $w_tabset.f${mag}]
      $w_tabset tab configure $mag \
         -text $alg(Mag,-nam,$mag) -window $w -fill both  \
         -font $toltk_options(fonts,Title1) -ipady 3
      DynamicHelp::add $w -text $alg(Mag,-des,$mag)
      $self _createMag $w $mag
    }
    grid $w_tabset   -sticky news
    grid columnconfigure $pane1 0 -weight 1
    grid rowconfigure    $pane1 0 -weight 1
    pack $pw -expand true -fill both
    $self _changeMag
    # Establecemos la magnitud por defecto (primera)
    # TODO: Podia ser la ultima en que se dejo el selector de series
    #$self _changeMag
  }
  #///////////////////////////////////////////////////////////////////////////
  method _triggerMag {mag} {
  #
  # PURPOSE: Trigger all subdimensions in magnitud which other subdimensions 
  #          depend on
  # PARAMETERS:
  #   mag -> magnitud
#set cmd "Text WriteLn(\"Lanzando trigger -> mag:$mag,dim:$dim,subdim:$sDi\");"
#set cmd "Real 1;"
#::tol::console eval $cmd
  #///////////////////////////////////////////////////////////////////////////
    set max 0
    foreach it [$self _getDim $mag] {
        set max $max+1
    }
    set localtrig 0
    if { !$data(alreadytriggering) } {
        set data(alreadytriggering) 1
        set localtrig 1
        Tolcon_Trace "cargando --> $mag" 
        . configure -cursor watch
        ::progressbar::Show [mc "loading"] 60 2
        ::progressbar::SetText [mc "Getting Values"]
    }
    set iterador 0

    variable alreadyTriggered
    foreach dim [$self _getDim $mag] {
      #::TolInspector::Busy
      set lstSubDim [$self _getSubDim $mag $dim]
      foreach sDi $lstSubDim {
        if {![info exist alreadyTriggered($dim,$sDi)]} {
           set type [$self _getSubDimType $mag $dim $sDi]
           if { $type eq "uni.dir" || $type eq "uni.ind" || $type eq "uni.dat" } {
             $self _trigger $mag $dim $sDi
           }
           foreach reg $alg(sDi,$mag,$dim,-dva,$sDi) {
               set dimDep [lindex $reg 0]
               set sDiDep [lindex $reg 1]
               set alreadyTriggered($dimDep,$sDiDep) 1
           }
           set alreadyTriggered($dim,$sDi) 1
        }
      }
      set iterador [expr $iterador+1]
      ::progressbar::Set [expr $iterador * 100 / ($max)]
    }
    array unset alreadyTriggered
    if { $localtrig } {
        Tolcon_Trace "terminando --> $mag"
      set data(alreadytriggering) 0
      ::progressbar::Destroy
      . configure -cursor ""
    }
  

  }

  #///////////////////////////////////////////////////////////////////////////
  method _changeMag {} {
  #///////////////////////////////////////////////////////////////////////////
    set magSel [$widget(tabset) get focus] 
    $self _triggerMag $magSel
  }

  #///////////////////////////////////////////////////////////////////////////
  proc _configure {f cols idx} {
  #
  # PURPOSE: Configures row and column weight for a given frame suposed it
  #          has been grid as a matrix in $cols cols and last element index
  #          is $idx.
  # PARAMETERS:
  #   f    -> widget to configure
  #   idx  -> index of last element of matrix
  #   cols -> number of columns of matrix
  #///////////////////////////////////////////////////////////////////////////
    set rows [_getRows $cols $idx]
    for {set i 0} {$i < $rows} {incr i} {
      grid rowconfigure    $f $i -weight 1
    }
    for {set j 0} {$j < $cols} {incr j} {
      grid columnconfigure $f $j -weight 1
    }
  }

  #///////////////////////////////////////////////////////////////////////////
  proc _getRows {cols idx} {
  #///////////////////////////////////////////////////////////////////////////
    if {[expr $idx%$cols] eq 0} {
      set rows [expr ($idx/$cols)]
    } else {
      set rows [expr ($idx/$cols)+1]
    }
    return $rows
  }

  #///////////////////////////////////////////////////////////////////////////
  method _createMag {w mag} {
  #
  # PURPOSE: Fills the tab of a given magnitud
  # PARAMETERS:
  #   w   -> path
  #   mag -> magnitud
  #///////////////////////////////////////////////////////////////////////////
  #Tolcon_Trace "_createMag: w=$w, mag=$mag"

    if [info exist ${selfns}::alg(Mag,TCL,-cols,$mag)] {
      set cols $alg(Mag,TCL,-cols,$mag)
    } else {
      set cols 2
    }
    set idx 0
    foreach dim [$self _getDim $mag] {
      set tf [$self _createDim $w $mag $dim]
      grid $tf -row [expr $idx/$cols] -column [expr $idx%$cols] -sticky news
      incr idx
    }
    
    #Configuro columnas y filas de magnitud
    _configure $w $cols $idx
  }


  #///////////////////////////////////////////////////////////////////////////
  method _createDim {w mag dim} {
  #
  # PURPOSE: Creates a title frame for a given dimension and calls to create
  #          subdimensions
  # PARAMETERS:
  #   w   -> path
  #   mag -> magnitud
  #   dim -> dimension
  #///////////////////////////////////////////////////////////////////////////
  #Tolcon_Trace "_createDim: mag=$mag dim=$dim"

    set dimPat [string tolower $dim]
    set tf [TitleFrame $w.$dimPat -text $alg(Dim,$mag,-nam,$dim) -bd 1 \
              -side center -fg blue -font {Verdana 9 bold}]
    set f [$tf getframe]
    
    DynamicHelp::add $tf -text $alg(Dim,$mag,-des,$dim)

    set lstSubDim [$self _getSubDim $mag $dim]
    if [info exist ${selfns}::alg(Dim,$mag,TCL,-cols,$dim)] {
      set cols $alg(Dim,$mag,TCL,-cols,$dim)
    } else {
      set cols [llength $lstSubDim]
    }

    set idx 0
    foreach sDi $lstSubDim {
      set tfs [$self _createSubDim $f $mag $dim $sDi]
      grid $tfs -row [expr $idx/$cols] -column [expr $idx%$cols] -sticky news
      incr idx 

    }
    #Configuro columnas y filas de dimension
    _configure $f $cols $idx
    return $tf
  }

  #///////////////////////////////////////////////////////////////////////////
  method _createSubDim {w mag dim sDi} {
  #
  # PURPOSE: Creates a title frame for a given subdimension and fills it with
  #          enabled values
  # PARAMETERS:
  #   w   -> path
  #   mag -> magnitud
  #   dim -> dimension 
  #   sDi -> subdimension 
  #///////////////////////////////////////////////////////////////////////////
  #Tolcon_Trace "_createSubDim: mag=$mag dim=$dim sDi=$sDi"

    set sDiPat [string tolower $sDi]
    set tf [TitleFrame $w.$sDiPat -text $alg(sDi,$mag,$dim,-nam,$sDi) -bd 1]
    set f [$tf getframe]
    set algwid($mag,$dim,$sDi) $f
    DynamicHelp::add $tf -text $alg(sDi,$mag,$dim,-des,$sDi)
    $self _fillSubDim $mag $dim $sDi

    return $tf
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _fillSubDim {mag dim sDi} {
  #
  # PURPOSE: Fills subdimension frame for a subdimension
  # PARAMETERS:
  #   mag -> magnitud identifier
  #   dim -> dimension identifier
  #   sDi -> subdimension identifier
  #/////////////////////////////////////////////////////////////////////////////
  #Tolcon_Trace "_fillSubDim mag=$mag, dim=$dim, sDi=$sDi"
    set type [$self _getSubDimType $mag $dim $sDi]
    switch -- $type {
       "uni.dir" { $self _fillSubDimUniDir $mag $dim $sDi }
       "uni.ind" { $self _fillSubDimUniInd $mag $dim $sDi }
       "mul.dir" { $self _fillSubDimMulDir $mag $dim $sDi }
       "mul.lib" { $self _fillSubDimMulInd $mag $dim $sDi 0}
       "mul.ind" { $self _fillSubDimMulInd $mag $dim $sDi }
       "uni.dat" { $self _fillSubDimDat $mag $dim $sDi 1}
       "mul.dat" { $self _fillSubDimDat $mag $dim $sDi 0}
       "uni.hie" { $self _fillSubDimHie $mag $dim $sDi 1}
       "mul.hie" { $self _fillSubDimHie $mag $dim $sDi 0}
    }
    #Tolcon_Trace "subDim: $mag $dim $sDi es de tipo $type"
  }

 
#/////////////////////////////////////////////////////////////////////////////
method _findcode {idcoltab cod} {
#
# PURPOSE: Find a value in the column col of tview and returns its index
# PARAMETERS:
#   idcoltab -> list of pairs id, cod
#   val      -> value to find
#/////////////////////////////////////////////////////////////////////////////
  foreach pair $idcoltab {
    if {[lindex $pair 1] eq $cod} {
      return [lindex $pair 0]
    }
  }
  return -1
}
#/////////////////////////////////////////////////////////////////////////////
method _fillSubDimHie {mag dim sDi trigger} {
#
# PURPOSE: Fills subdimension frame for a subdimension marked as hierarchical
# PARAMETERS:
#   mag -> magnitud identifier
#   dim -> dimension identifier
#   sDi -> subdimension identifier
#   trigger -> 0/1 trigger event for dependants?
#   multisel -> 0/1 allows multiselection 
#/////////////////////////////////////////////////////////////////////////////

  set w $algwid($mag,$dim,$sDi)
  set lstReg [$self _getSubDimValues $mag $dim $sDi]
  set defval [$self _getSubDimDefValue $mag $dim $sDi]

  set valCont 0
  #if $multisel { set icosel 0 } else { set icosel 1 }
# -bonselcmd [list $self _trigger $mag $dim $sDi] \
# Si no existe crearlo
  if {![info exist ${selfns}::tmp($mag,$dim,$sDi)]} {
    ScrolledWindow $w.sw${sDi}
    if {$trigger} {
      set tmp($mag,$dim,$sDi) [ btreeview $w.sw${sDi}.tv \
        -bonselcmd [list $self _trigger $mag $dim $sDi] \
		-btogglehier false \
        -height 150 -linewidth 2 -borderwidth 1 \
        -bmultiselect false -bicosel false ] 
    } else {
      set tmp($mag,$dim,$sDi) [ btreeview $w.sw${sDi}.tv \
        -btogglehier false \
        -height 150 -linewidth 2 -borderwidth 1 \
        -bmultiselect true -bicosel true ]
    }
    
    $tmp($mag,$dim,$sDi) bColumnInsert end cod -hide true
    
    
    $w.sw${sDi} setwidget $w.sw${sDi}.tv
    grid $w.sw${sDi} -sticky news
    #grid $w.sw${sDi}.tv  -row 0 -column 0 -sticky new
 
    grid columnconfigure $w 0 -weight 1
    grid rowconfigure    $w 0 -weight 1
  }
  # Insertar los valores
  # Crear elementos nuevos
    #set idx 0
    $tmp($mag,$dim,$sDi) delete root
    set alg(sDi,$mag,$dim,-hie,$sDi) ""
    set plotted 0
    set iteration -1
    set regidx 0
    set plottedreg ""
    set numels [expr [llength $lstReg]-1 ]
    
    while { ($plotted < $numels) && ($iteration < $numels) } {
       set iteration [expr $iteration + 1]

       foreach reg $lstReg {
          set ide [lindex $reg 0]
          set abr [lindex $reg 1]
          set nom [lindex $reg 2]
          set des [lindex $reg 3]
          set par [lindex $reg 4]
          set lev [lindex $reg 5]
          if {$iteration eq 0} { lappend plottedreg 0 }
          if { $abr eq $defval } {
            set mark ON
          } else {
            set mark OFF
          }
          set isplotted [lindex $plottedreg $regidx]

#          set cmd "Text WriteLn(\"Processing iteration $iteration regidx $regidx plotted $plotted plottedreg -> $plottedreg \");"
#          ::tol::console eval $cmd

          if { $isplotted eq 0 } {
              set infCols [ list cod $abr ]
              if {$par ne ""} {
                 set paridx [$self _findcode $alg(sDi,$mag,$dim,-hie,$sDi) $par]
                 if { $paridx >= 0 } {
                   set plottedreg [ lreplace $plottedreg $regidx $regidx 1 ]
                   set plotted [expr $plotted+1]
                   set idx [ $tmp($mag,$dim,$sDi) insertNode $paridx 0 $mark $abr $nom $infCols ]
                 }
              } else {
                 set plottedreg [ lreplace $plottedreg $regidx $regidx 1 ]
                 set plotted [expr $plotted+1]
                 set idx [ $tmp($mag,$dim,$sDi) insertNode root 0 $mark $abr $nom $infCols ]
              }
              lappend alg(sDi,$mag,$dim,-hie,$sDi) [list $idx $abr]
          }
          set regidx [expr $regidx+1]
       }
       set regidx 0
    }
}
  
#/////////////////////////////////////////////////////////////////////////////
method _fillSubDimDat {mag dim sDi trigger} {
#
# PURPOSE: Fills subdimension frame for a subdimension marked as returning a 
#          date data type
# PARAMETERS:
#   mag -> magnitud identifier
#   dim -> dimension identifier
#   sDi -> subdimension identifier
#/////////////////////////////////////////////////////////////////////////////
#Tolcon_Trace "_fillSubDimUniInd mag=$mag dim=$dim, sDi=$sDi"
  set w $algwid($mag,$dim,$sDi)
  set lstReg [$self _getSubDimValues $mag $dim $sDi]
  set defval [$self _getSubDimDefValue $mag $dim $sDi]
  set defFound 0
  set valCont 0

  if {[llength $lstReg]} {
    foreach reg $lstReg {
      lappend lstRegMod [linsert [lrange $reg 1 end] end [lindex $reg 0]]
      set valCont [expr $valCont+1]
      if {$defval eq [lindex $reg 1]} {
           set defFound $valCont
      }
    }
  } else {
    set lstRegMod ""
  }
  # Si no existe crearlo
  if {![info exist ${selfns}::tmp($mag,$dim,$sDi)]} {
    label $w.l${sDi} -text [mc $sDi]
      if {$trigger} {
          set tmp($mag,$dim,$sDi) [ bentrydate $w.bsf${sDi} \
             -postcommand [list $self _trigger $mag $dim $sDi] ]
      } else {
          set tmp($mag,$dim,$sDi) [bentrydate $w.bsf${sDi} ]
      }

    
    grid $w.l${sDi}    -row 0 -column 0 -sticky ne
    grid $w.bsf${sDi}  -row 0 -column 1 -sticky new
 
    grid columnconfigure $w 1 -weight 1
    grid rowconfigure    $w 0 -weight 1
  }
  puts $defval
  # Insertar los valores
  $tmp($mag,$dim,$sDi) configure -bdate $defval

#  if {$defval ne ""} {
#     if {$defFound eq 1} {
#        $tmp($mag,$dim,$sDi) selectIndex $defFound
#     }
#  }

}

  #/////////////////////////////////////////////////////////////////////////////
  method _fillSubDimUniDir {mag dim sDi} {
  #
  # PURPOSE: Fills subdimension frame for a subdimension with 'unique'
  #          selection type and 'direct' show method (radiobuttons)
  # PARAMETERS:
  #   mag -> magnitud identifier
  #   dim -> dimension identifier
  #   sDi -> subdimension identifier
  #/////////////////////////////////////////////////////////////////////////////
  #Tolcon_Trace "_fillSubDimUniDir mag=$mag dim=$dim, sDi=$sDi"
    set w $algwid($mag,$dim,$sDi)
    set lstReg [$self _getSubDimValues $mag $dim $sDi]

    if [info exist ${selfns}::alg(sDi,$mag,$dim,TCL,-cols,$sDi)] {
      set cols $alg(sDi,$mag,$dim,TCL,-cols,$sDi)
    } else {
      set cols 1
    }

    # Eliminar elementos anteriores y eliminar configure padre
    set idx 0
    foreach f [winfo child $w] {
      destroy $f
      incr idx
    }
    grid columnconfigure $w $cols                 -weight 0
    grid rowconfigure    $w [_getRows $cols $idx] -weight 0

    # Crear elementos nuevos
    set idx 0
    set defval [$self _getSubDimDefValue $mag $dim $sDi]

    foreach reg $lstReg {
      set ide [lindex $reg 0]
      set abr [lindex $reg 1]
      set nom [lindex $reg 2]
      set des [lindex $reg 3]
      set wid rb$abr
      radiobutton $w.$wid -text $nom -value $abr \
         -variable ${selfns}::tmp($mag,$dim,$sDi)
      if {$abr eq $defval} {
        $w.$wid select
      } 
      DynamicHelp::add $w.$wid -text [mc $des]
      grid $w.$wid -row [expr $idx/$cols] -column [expr $idx%$cols] -sticky nws
      incr idx
    }
    if {$idx ne 0} {
      trace variable ${selfns}::tmp($mag,$dim,$sDi) \
        w [list $self _trigger $mag $dim $sDi]
    }
    grid columnconfigure $w $cols                  -weight 1
    grid rowconfigure    $w [_getRows $cols $idx]  -weight 1
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _fillSubDimUniInd {mag dim sDi} {
  #
  # PURPOSE: Fills subdimension frame for a subdimension with 'unique'
  #          selection type and 'indirect' show method (bselectfields)
  # PARAMETERS:
  #   mag -> magnitud identifier
  #   dim -> dimension identifier
  #   sDi -> subdimension identifier
  #/////////////////////////////////////////////////////////////////////////////
  #Tolcon_Trace "_fillSubDimUniInd mag=$mag dim=$dim, sDi=$sDi"
    set w $algwid($mag,$dim,$sDi)
    set lstReg [$self _getSubDimValues $mag $dim $sDi]
    set defval [$self _getSubDimDefValue $mag $dim $sDi]
    set defFound 0
    set valCont 0

    if {[llength $lstReg]} {
      foreach reg $lstReg {
        lappend lstRegMod [linsert [lrange $reg 1 end] end [lindex $reg 0]]
        set valCont [expr $valCont+1]
        if {$defval eq [lindex $reg 1]} {
             set defFound $valCont
        }
      }
    } else {
      set lstRegMod ""
    }
    # Si no existe crearlo
    if {![info exist ${selfns}::tmp($mag,$dim,$sDi)]} {
      label $w.l${sDi} -text [mc $sDi]
      set tmp($mag,$dim,$sDi) [bcomboboxplus $w.bsf${sDi} \
        -modifycmd [list $self _trigger $mag $dim $sDi] \
        -cols    [list [mc Code] [mc Name] [mc Description] [mc Id]] \
        -hiddencols    [list [mc Description] [mc Id]] \
        -nrows 6 \
        -widthlb 600 \
        -font {Arial 8}]
      
      grid $w.l${sDi}    -row 0 -column 0 -sticky ne
      grid $w.bsf${sDi}  -row 0 -column 1 -sticky new
   
      grid columnconfigure $w 1 -weight 1
      grid rowconfigure    $w 0 -weight 1
    }
    # Insertar los valores
    $tmp($mag,$dim,$sDi) clear
    foreach item $lstRegMod {
      $tmp($mag,$dim,$sDi) insert end $item
    }

    if {$defval ne ""} {
       if {$defFound eq 1} {
          $tmp($mag,$dim,$sDi) selectIndex $defFound
       }
    }

  }

  #/////////////////////////////////////////////////////////////////////////////
  method _fillSubDimMulDir {mag dim sDi} {
  #
  # PURPOSE: Fills subdimension frame for a subdimension with 'multiple'
  #          selection type and 'direct' show method (checkbuttons)
  # PARAMETERS:
  #   mag -> magnitud identifier
  #   dim -> dimension identifier
  #   sDi -> subdimension identifier
  #/////////////////////////////////////////////////////////////////////////////
  #Tolcon_Trace "_fillSubDimMulDir mag=$mag dim=$dim, sDi=$sDi"

    set w $algwid($mag,$dim,$sDi)
    set lstReg [$self _getSubDimValues $mag $dim $sDi]
    if [info exist ${selfns}::alg(sDi,$mag,$dim,TCL,-cols,$sDi)] {
      set cols $alg(sDi,$mag,$dim,TCL,-cols,$sDi)
    } else {
      set cols 3
    }
#set cmd "Text WriteLn(\"Llamada a testar (before) -> mag:$mag,dim:$dim,subdim:$sDi \");"
#::tol::console eval $cmd

    # Eliminar elementos anteriores y eleminar configure padre
    set idx 0
    foreach f [winfo child $w] {
      set abr [string range [lindex [split $f .] end] 2 end]
      set tmp($mag,$dim,$sDi,$abr) 0
      destroy $f
      incr idx
    }
    grid columnconfigure $w $cols                 -weight 0
    grid rowconfigure    $w [_getRows $cols $idx] -weight 0
    # Crear elementos nuevos
    set idx 0
    set defval [$self _getSubDimDefValue $mag $dim $sDi]
    foreach reg $lstReg {
      set ide [lindex $reg 0]
      set abr [lindex $reg 1]
      set nom [lindex $reg 2]
      set des [lindex $reg 3]
      set wid ch$abr
      checkbutton $w.$wid -text $nom -onvalue $abr \
         -variable ${selfns}::tmp($mag,$dim,$sDi,$abr)

      if {$abr eq $defval} {
        $w.$wid select
      }
      DynamicHelp::add $w.$wid -text [mc $des]
      grid $w.$wid -row [expr $idx/$cols] -column [expr $idx%$cols] -sticky nw
      incr idx
    }
    grid columnconfigure $w $cols                 -weight 1
    grid rowconfigure    $w [_getRows $cols $idx] -weight 1
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _fillSubDimMulInd {mag dim sDi {readonly 1}} {
  #
  # PURPOSE: Fills subdimension frame for a subdimension with 'multiple'
  #          selection type and 'indirect' show method (beditselector)
  # PARAMETERS:
  #   mag -> magnitud identifier
  #   dim -> dimension identifier
  #   sDi -> subdimension identifier
  #   readonly  -> indica si es editable o no
  #/////////////////////////////////////////////////////////////////////////////
  #Tolcon_Trace "_fillSubDimMulInd mag=$mag dim=$dim, sDi=$sDi"
    set w $algwid($mag,$dim,$sDi)
    set lstReg [$self _getSubDimValues $mag $dim $sDi]
    set defval [$self _getSubDimDefValue $mag $dim $sDi]
    set defFound 0
    set longitud [llength $lstReg]
    if {[llength $lstReg]} {
      foreach reg $lstReg {
        set lstValues [linsert [lrange $reg 1 end] end [lindex $reg 0]]
        lappend lstRegMod $lstValues
        set val [lindex $reg 1]
        if {$defval eq $val} {
             set defFound $lstValues
        }
      }
    } else {
      set lstRegMod ""
    }
    # si no existe crearlo
    if {![info exist ${selfns}::tmp($mag,$dim,$sDi)]} {
      set tmp($mag,$dim,$sDi) [beditselector $w.bes${sDi} -width 25\
        -label [mc $sDi] -readonly $readonly  -font {Arial 8}\
        -entcols [list [mc Code] [mc Name]] -msgeometry 1000x500+40+10\
        -hlpcols [list [mc Code] [mc Name] [mc Description]]\
        -cols    [list [mc Code] [mc Name] [mc Description] [mc Id]] ]
      grid $w.bes${sDi}  -row 0 -column 0 -sticky new

      grid columnconfigure $w 0 -weight 1
      grid rowconfigure    $w 0 -weight 1
    }
    # Insertar los valores
    $tmp($mag,$dim,$sDi) configure -list $lstRegMod -sel ""

    if {$defval ne ""} {
       if {$defFound ne 0} {
          $tmp($mag,$dim,$sDi) configure -sel  [list $defFound]
       }
    }
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _trigger {mag dim sDi args} {
  #
  # PURPOSE: Method called when a 'unique' selection type subdimension changes
  #          its selection, then other subdimensions values may change and 
  #          other dimensions existence may change.
  # PARAMETERS:
  #   mag   -> magntitud
  #   dim   -> dimension 
  #   sDi   -> subdimension 
  #   args  -> rest of arguments (not used)
  #/////////////////////////////////////////////////////////////////////////////
    set localtrig 0
    if { !$data(alreadytriggering) } {
         set data(alreadytriggering) 1
      set localtrig 1
      . configure -cursor watch
    }
    #Tolcon_Trace "********** Launching _trigger $mag $dim $sDi"
      
    set def [$self cget -def]

    foreach reg $alg(sDi,$mag,$dim,-dva,$sDi) {
      set dimDep [lindex $reg 0]
      set sDiDep [lindex $reg 1]
      $self _fillSubDim $mag $dimDep $sDiDep
    }
    if { $localtrig } {
        set data(alreadytriggering) 0
        . configure -cursor ""
      }
      
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _getSubDimValues {mag dim sDi} {
  #
  # PURPOSE: Get allowed values of a subdimension
  # PARAMETERS:
  #   mag -> magnitud identifier
  #   dim -> dimension identifier
  #   sDi -> subdimension identifier
  #/////////////////////////////////////////////////////////////////////////////
  #Tolcon_Trace "_getSumDimValues mag=$mag, dim=$dim, sDi=$sDi"
    set res [$self _getSelection]

    set cmd [TclLst2TolSet $res -level 2]
    set sSet [StringReplace $cmd "SetOfText" "AlgNodeSt"]

    set def    [$self cget -def]
    set nRes [GetUniqueName Text _GuiAlgDim]
    set cmd "Text $nRes = \
      GuiAlgGetSubDimVal(\"$mag\",\"$dim\",\"$sDi\",$sSet,$def);"
    ::tol::console eval ${cmd}
    array set info [TclInfoVar Text $nRes]
    eval "set lstSubDimVal $info(CONTENT)"
    ::tol::console stack release $nRes
    #Tolcon_Trace "\ncmd=$cmd \n lstSubDimVal = $lstSubDimVal"
    return $lstSubDimVal
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _getSubDimDefValue {mag dim sDi} {
  #
  # PURPOSE: Get default value of a subdimension
  # PARAMETERS:
  #   mag -> magnitud identifier
  #   dim -> dimension identifier
  #   sDi -> subdimension identifier
  #/////////////////////////////////////////////////////////////////////////////
    #Tolcon_Trace "_getSumDimValues mag=$mag, dim=$dim, sDi=$sDi"
    set res [$self _getSelection]
        
    set cmd [TclLst2TolSet $res -level 2]
    set sSet [StringReplace $cmd "SetOfText" "AlgNodeSt"]
        
    set def    [$self cget -def]
    set nRes [GetUniqueName Text _GuiAlgDim]
    set cmd "Text $nRes = \
            GuiAlgGetSubDimDefVal(\"$mag\",\"$dim\",\"$sDi\",$sSet,$def);"
    ::tol::console eval ${cmd}
    array set info [TclInfoVar Text $nRes]
    eval "set lstSubDimVal $info(CONTENT)"
    ::tol::console stack release $nRes
    #Tolcon_Trace "\ncmd=$cmd \n lstSubDimVal = $lstSubDimVal"
    return $lstSubDimVal
  }
    
  #/////////////////////////////////////////////////////////////////////////////
  method _getSubDimType {mag dim sDi} {
  #
  # PURPOSE: Returns type of a subdimension
  # PARAMETERS:
  #   mag -> magnitud
  #   dim -> dimension 
  #   sDi -> subdimension 
  #/////////////////////////////////////////////////////////////////////////////
    set nDeEx [llength $alg(sDi,$mag,$dim,-dex,$sDi)]
    set nDeVa [llength $alg(sDi,$mag,$dim,-dva,$sDi)]
    
    if {[expr $nDeEx + $nDeVa] eq 0} {
      set car "mul"
    } else {
      set car "uni"
    }
    if [info exist ${selfns}::alg(sDi,$mag,$dim,TCL,-type,$sDi)] {
      set met [string tolower $alg(sDi,$mag,$dim,TCL,-type,$sDi)]
      if { ($met ne "hie") && ($met ne "dir") && ($met ne "dat") && ($met ne "lib")} {
        set met "ind"
      }
    } else {
      set met "ind"
    }
    set typ ${car}.${met}

    return $typ
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _getSelection {} {
  #
  # PURPOSE: Returns a names list corresponding to selection of actual tab
  #
  #/////////////////////////////////////////////////////////////////////////////
    set res ""
    set magSel [$widget(tabset) get focus] 

    # lista de magnitudes
    foreach mag [$self _getMag] {
      if {$magSel eq $mag} {
        set res [$self _getSelMag $mag]
      }
    }
    return $res
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _checkMag {mag} {
  #
  # PURPOSE: Checks if all requiered dimensions and subdimensions are choosen
  #
  # PARAMETERS:
  #   mag -> magnitud identifier
  # 
  #/////////////////////////////////////////////////////////////////////////////
    set res 1
    foreach dim [$self _getDim $mag] {
      foreach sDi [$self _getSubDim $mag $dim] {
        set sDiLst  [$self _getSelSubDim $mag $dim $sDi]
        set type [$self _getSubDimType $mag $dim $sDi]
        if {$res && ($alg(sDi,$mag,$dim,-req,$sDi) eq 1) && \
            ( $type ne "uni.dat" && $type ne "mul.dat" ) && ([llength $sDiLst] eq 0)} {
          set msg [mc "Must fill subdimension %1\$s of dimension %2\$s" \
             $alg(sDi,$mag,$dim,-nam,$sDi) $alg(Dim,$mag,-nam,$dim)]
          tk_messageBox -message $msg -title [mc "Required fields"]
          set res 0
        }
      }
    }
    return $res
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _getSelMag {mag} {
  #
  # PURPOSE: Returns a names list corresponding to selection of a given magnitud
  #
  # PARAMETERS:
  #   mag -> magnitud identifier
  # 
  #/////////////////////////////////////////////////////////////////////////////
    set res ""
    foreach dim [$self _getDim $mag] {
      foreach sDi [$self _getSubDim $mag $dim] {
        set sDiLst  [$self _getSelSubDim $mag $dim $sDi]
        foreach sDiRes $sDiLst {
          lappend res $sDiRes
        }
      }
    }
    #Tolcon_Trace "MAG: $mag     --> $res"
    return $res
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _getSelMagAsName {mag} {
  #
  # PURPOSE: Returns a names list corresponding to selection of a given magnitud
  #          as a valid name
  #
  # PARAMETERS:
  #   mag -> magnitud identifier
  # 
  #/////////////////////////////////////////////////////////////////////////////
    set lstDimf ""
    if {[$self _checkMag $mag]} {
      set lstDim [$self _getDim $mag]
      set cmd [$self _getSelMag_ lstDimf $mag [lindex $lstDim 0] $lstDim]
      #Tolcon_Trace "\n$cmd\n"
      eval $cmd
    }
    return $lstDimf
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _getSelMag_ {var mag dim lstDim} {
  #
  # PURPOSE: Returns the TCL script that when its evaluated returns the algebra
  #          names relative to a given magnitud Its returned in var variable.
  # PARAMETERS:
  #   var -> name of TCL variable that will contain the list of magnitud names
  #   mag -> magnitud identifier
  #   dim -> dimension identifier
  #   lstDim -> list of dimensions of given magnitud To iterate
  # 
  #/////////////////////////////////////////////////////////////////////////////
  #Tolcon_Trace "_getSelMag var=$var mag=$mag dim=$dim lstDim=$lstDim"
    set txt ""
    set idx [lsearch $lstDim $dim]
    set spa [string repeat " " [expr 2*$idx]]
    if {$dim eq [lindex $lstDim end]} {
      set txt \
"${spa}foreach $dim [list [$self _getSelDim $mag $dim]] {
${spa}  lappend $var ${mag}_[ListToStr $lstDim $alg(TCL,-dimSep) {[lindex $}    { 0]}]
${spa}}"
    } else {
      set txt \
"${spa}foreach $dim [list [$self _getSelDim $mag $dim]] {
[$self _getSelMag_ $var $mag [lindex $lstDim [expr $idx+1]] $lstDim]
${spa}}"
    }
    Tolcon_Trace "\n$txt\n"
    return $txt
  }


  #/////////////////////////////////////////////////////////////////////////////
  method _getSelDim {mag dim} {
  #/////////////////////////////////////////////////////////////////////////////
    set lstSD [$self _getSubDim $mag $dim]
    set cmd [$self _getSelDim_ lstSDf $mag $dim [lindex $lstSD 0] $lstSD]
    set lstSDf ""
    eval $cmd
    return $lstSDf
  }


  #/////////////////////////////////////////////////////////////////////////////
  method _getSelDim_ {var mag dim sDi lstSDi} {
  #
  # PURPOSE: Returns the TCL script that when its evaluated returns the algebra
  #          names relative to a given dimension. Its returned in var variable.
  # PARAMETERS:
  #   var -> name of TCL variable that will contain the list of dimension names
  #   mag -> magnitud identifier
  #   dim -> dimension identifier
  #   sDi -> subdimension identifier
  #   lstSDi -> list of subdimensions of given dimension. To iterate
  # 
  #/////////////////////////////////////////////////////////////////////////////
  #Tolcon_Trace "_getSelDim_ var=$var mag=$mag dim=$dim sDi=$sDi, lstSDi=$lstSDi"
    set txt ""
    set idx [lsearch $lstSDi $sDi]
    set spa [string repeat " " [expr 2*$idx]]

    set dimTxt [string range $dim 0 [expr $alg(TCL,-numCarDim)-1]]
    if {$sDi eq [lindex $lstSDi end]} {
      set txt \
"${spa}foreach $sDi [list [$self _getSelSubDim $mag $dim $sDi]] {
${spa}  lappend $var ${dimTxt}[ListToStr $lstSDi $alg(TCL,-sDiSep) {[lindex $}    { 2]}]
${spa}}"
    } else {
      set txt \
"${spa}foreach $sDi [list [$self _getSelSubDim $mag $dim $sDi]] {
[$self _getSelDim_ $var $mag $dim [lindex $lstSDi [expr $idx+1]] $lstSDi]
${spa}}"
    }
    Tolcon_Trace "\n$txt\n"
    return $txt
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _getSelSubDim {mag dim sDi} {
  #
  # PURPOSE: Returns a names list corresponding to a given subdimension
  # PARAMETERS:
  #   mag -> magnitud identifier
  #   dim -> dimension identifier
  #   sDi -> subdimension identifier
  # 
  #/////////////////////////////////////////////////////////////////////////////
#Tolcon_Trace "Obteniendo subdimension: $mag $dim $sDi"
    set lst ""
    set type [$self _getSubDimType $mag $dim $sDi]
    #TODO: Validate here
    switch -- $type {
       "uni.hie" {
         if {[info exist ${selfns}::tmp($mag,$dim,$sDi)] } {
           if {$tmp($mag,$dim,$sDi) ne ""} {
               if {[ $tmp($mag,$dim,$sDi) getSelected ] ne ""} {
                    set lst [ join [  $tmp($mag,$dim,$sDi) getSelected cod ] ]
               } else {
                   set lst ""
               }
           }
         }
       }
       "mul.hie" {
         if {[info exist ${selfns}::tmp($mag,$dim,$sDi)] } {
           if {$tmp($mag,$dim,$sDi) ne ""} {
               if {[ $tmp($mag,$dim,$sDi) getSelected ] ne ""} {
                    set lst [ join [  $tmp($mag,$dim,$sDi) getSelected cod ] ]
               } else {
                   set lst ""
               }
           }
         }
       }
       "uni.dat" {
         if {[info exist ${selfns}::tmp($mag,$dim,$sDi)] } {
           if {$tmp($mag,$dim,$sDi) ne ""} {
               if {[ $tmp($mag,$dim,$sDi) cget -btext ] ne ""} {
                    set lst [ join [  $tmp($mag,$dim,$sDi) cget -btext ] ]
               } else {
                   set lst "00000000"
               }
           }
         }
       }
      "mul.dat" {
          if {[info exist ${selfns}::tmp($mag,$dim,$sDi)] } {
            if {$tmp($mag,$dim,$sDi) ne ""} {
                if {[ $tmp($mag,$dim,$sDi) cget -btext ] ne ""} {
                     set lst [ join [  $tmp($mag,$dim,$sDi) cget -btext ] ]
                } else {
                    set lst "00000000"
                }
            }
          }
      }
       "uni.dir" {
         if {[info exist ${selfns}::tmp($mag,$dim,$sDi)] } {
           if {$tmp($mag,$dim,$sDi) ne ""} {
             set lst [list $tmp($mag,$dim,$sDi)]
           }
         }
       }
       "uni.ind" {
         if {[info exist ${selfns}::tmp($mag,$dim,$sDi)]} {
           set val [lindex [$tmp($mag,$dim,$sDi) key get] 0]
           if {$val ne ""} {
             set lst [list $val]
           } else {
             set lst ""
           }
         }
       }
       "mul.dir" {  
         regsub -all $mag,$dim,$sDi, \
           [array names ${selfns}::tmp $mag,$dim,$sDi,*] "" ideNames
         foreach ide $ideNames {
           if {$tmp($mag,$dim,$sDi,$ide) != 0} {
             lappend lst $ide 
           }
         }
       }
       "mul.lib" -
       "mul.ind" {
         if {[info exist ${selfns}::tmp($mag,$dim,$sDi)]} {
           set lst [$tmp($mag,$dim,$sDi) selection get [mc Code]]
         }
       }
    }    

    set res ""
    foreach val $lst {
      lappend res [list $dim $sDi $val]
    }
    #Tolcon_Trace "sDi: $dim $sDi --> $res"
    return $res
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _createlist {w} {
  # PURPOSE: Creates series list listbox frame
  # PARAMETERS:
  #   w  - path
  #/////////////////////////////////////////////////////////////////////////////
    
    set tf [TitleFrame $w.tflist -text [mc {Series selected}] -bd 1 -side center]
    set f [$tf getframe]
      
    ScrolledWindow $f.sw
    set widget(output) [blistbox $f.sw.lb -state normal -width 30 \
        -selectmode extended]
    $f.sw setwidget $f.sw.lb
  
    grid $f.sw -sticky news
  
    grid columnconfigure $f 0 -weight 1
    grid rowconfigure    $f 0 -weight 1
     
    return $tf
  }
  
  #/////////////////////////////////////////////////////////////////////////////
  method _createbbox {f} {
  #
  # PURPOSE: Creates a button box with options over form selection and names
  #          list.
  # PARAMETERS:
  #  f  - path
  #
  #/////////////////////////////////////////////////////////////////////////////
 
    # Botones
    set bbox [ButtonBox $f.bb -orient vertical -spacing 2]
    set spNor  2 ;#Espacio normal entre botones del mismo grupo
    set spSep 20 ;#Espacio entre grupos de botones
    # add add button
    $bbox add -image [Bitmap::get play] \
      -relief link -bd 1 -padx 1 -pady 1 \
      -command [list $self _addselection]\
      -helptext [mc "Add series to list"]
    $bbox configure -spacing $spNor
    # add clear sel button
    $bbox add -image [Bitmap::get reverse] \
      -relief link -bd 1 -padx 1 -pady 1 \
      -command [list $self _clearselection ]\
      -helptext [mc "Clear selection"]
    # add clear all button
    $bbox add -image [Bitmap::get Clear] \
      -relief link -bd 1 -padx 1 -pady 1 \
      -command [list $self _clearall]\
      -helptext [mc "Clear all"]
    $bbox configure -spacing $spSep
    # add include button
    $bbox add -image [::Bitmap::get "eval"] \
       -relief link -bd 1 -padx 1 -pady 1 \
       -helptext "[mc "Include"] (F9)" -command [list $self _includeseries]
    bind $widget(output) <F9> [list $self _includeseries]
    $bbox configure -spacing $spNor
    # add clear button
    $bbox add -image [::Bitmap::get "decom"] -highlightthickness 0 \
       -activebackground \#ececec \
       -relief link -bd 1 -padx 1 -pady 1 \
       -helptext "[mc Release] (F8)" -command ::TolConsole::ClearConsole
    bind $widget(output) <F8> ::TolConsole::ClearConsole
    $bbox configure -spacing $spSep
    # add load list button
    $bbox add -image [Bitmap::get open] \
      -relief link -bd 1 -padx 1 -pady 1 \
      -command [list lbLoadFromFile $widget(output)] \
      -helptext [mc "Load selection from file"]
    $bbox configure -spacing $spNor
    # add save list button
    $bbox add -image [Bitmap::get save] \
      -relief link -bd 1 -padx 1 -pady 1 \
      -command [list lbSaveToFile $widget(output) 1 1] \
      -helptext [mc "Save selection in file"]
    $bbox configure -spacing $spSep
    # exit button
    #$bbox add -image [Bitmap::get exit] -relief link -bd 1 -padx 1 -pady 1\
    #   -command [list $self _destroy]\
    #   -helptext [mc "Close the form"]
  
    return $bbox
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _addselection {} {
  #
  # PURPOSE: Adds selection from algebra to series list listbox
  #
  #/////////////////////////////////////////////////////////////////////////////
   set mag [$widget(tabset) get focus] 
    set lst [$self _getSelMagAsName $mag]
    foreach name $lst {
      $widget(output) insert end $name
    }
  }

  
  #/////////////////////////////////////////////////////////////////////////////
  method _clearselection {} {
  #
  # PURPOSE: Clears selected series of series list listbox
  #
  #/////////////////////////////////////////////////////////////////////////////
    lbDeleteSelection $widget(output)
  }
  
  
  #/////////////////////////////////////////////////////////////////////////////
  method _clearall {} {
  #
  # PURPOSE: Clears all series of series list listbox
  #
  #/////////////////////////////////////////////////////////////////////////////
    $widget(output) deletespecial
  }


  #/////////////////////////////////////////////////////////////////////////////
  method _includeseries {} {
  #
  # PURPOSE: Includes series of series list listbox
  #
    #/////////////////////////////////////////////////////////////////////////////
    set max 0
    foreach it [$widget(output) get 0 end] {
        set max $max+1
    }
    ::progressbar::Show [mc "loading"] 60 2
    set iterador 0
    foreach it [$widget(output) get 0 end] {
      ::progressbar::SetText "[mc "Compiling serie"]: $it"
      set code "Anything $alg(TCL,-funAlgGetData)(\"$it\",[$self cget -def])"
      ::tol::console eval $code
      set iterador [expr $iterador+1]
      ::progressbar::Set [expr $iterador * 100 / ($max)]
    }
    ::TolInspector::UpdateConsoleObj
    ::progressbar::Destroy
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _destroy {} {
  #/////////////////////////////////////////////////////////////////////////////
    $self _writeini
    array unset tmp
    array unset alg
  }


  #/////////////////////////////////////////////////////////////////////////////
  method _writeini {} {
  #/////////////////////////////////////////////////////////////////////////////
    set wini ::iniFile::Write
  
    set options(wform)   [winfo width $widget(panel,form)]
    set options(wlist)   [winfo width $widget(panel,list)]
  
    $wini $data(iniEntry) wform     $options(wform)
    $wini $data(iniEntry) wlist     $options(wlist)
  }

  #/////////////////////////////////////////////////////////////////////////////
  method _readini {} {
  #/////////////////////////////////////////////////////////////////////////////
    set rini ::iniFile::Read
    set options(wform) [$rini $data(iniEntry) wform 7]
    set options(wlist) [$rini $data(iniEntry) wlist 3]
  }
}




package provide bsersel 1.0

# -------- TEST CODE ------

proc test_bsersel { } {
  # Desde un ambito global
  global env
  source "./toltk/project.tcl"
  source "./toltk/iniparseBys.tcl"
  source "./toltk/iniFile.tcl"

  load iniFile
  ::iniFile::Create [file join $env(HOME) "prueba.ini"]

  bsersel .a -project CocaCola
  .a show
}

#test_bsersel
