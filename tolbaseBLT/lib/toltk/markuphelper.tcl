namespace eval ::MarkupHelper {
  variable markup {}
  variable backup {}
  variable indent 0

  proc Init { } {
    variable markup {}
    variable restore {}
    variable indent 0
  }

  proc SetIndent { n } {
    variable indent

    if { $n >= 0 && n <= 9 } {
      set indent n
    }
    return $indent
  }

  proc IncrIndent { d } {
    variable indent

    set n [ incr indent $d ]
    if { $indent < 0 } {
      set indent 0
    } elseif { $indent > 9 } {
      set indent 9
    }
    return $indent
  }

  proc Indent { } {
    variable indent
    variable markup

    append markup [ string repeat ":" $indent ]
  }

  proc Backup { } {
    variable markup
    variable backup $markup

    return $backup
  }

  proc Restore { } {
    variable markup
    variable backup

    set old $markup
    set markup $backup
    return $old
  }

  proc Verbatim { text } {
    return [ string map {\n \n<br>} $text ]
  }

  proc Preformatted { text } {
    variable markup

    append markup "#pre\n"
    append markup "${text}\n"
    append markup "#unpre\n"
  }

  proc Escape { text } {
    set specialChars {
      [  &lb;
      ]  &rb;
      <  &lt;
      >  &gt;
      '  &ap;
      &  &amp;
    }
    return [ string map $specialChars $text ]
  }

  proc Preprocess { text } {
    set idx [ string first ";" $text ]
    set ismarkup 0
    if { $idx != -1 } {
      set header [ string range $text 0 [ expr { $idx -1 } ] ]
      if { [ string range [ string trimleft $header ] 0 3 ] eq "#!NB" } {
        set ismarkup 1
        set body [ string range $text [ expr { $idx +1 } ] end ]
      } else {
        set body $text
      }
    } else {
      set body $text
    }
    if { $ismarkup } {
      return $body
    } else {
      return [ Verbatim [ Escape $body ] ]
    }
  }

  proc GetText { } {
    variable markup

    return $markup
  }

  proc Text { text args } {
    variable markup

    if { $markup eq "" } {
      Indent
    }
    array set opts { -tags {} -br 0 }
    array set opts $args
    foreach t $opts(-tags) {
      append markup "<$t>"
    }
    append markup $text
    foreach t [ lreverse $opts(-tags) ] {
      append markup "</$t>"
    }
    BR $opts(-br)
  }

  proc Image { image {br 0} } {
    variable markup

    append markup "\[!image $image !\]"
    BR $br
  }

  proc LabelValue { name value {br 0}} {
    variable markup

    append markup "<b>$name:</b> "
    append markup "$value"
    BR $br
  }
  
  proc InlineHeader { text } {
    Text $text -tags h
  }

  proc Header1 { text } {
    variable markup
    
    NeedNewLine
    append markup "= $text =\n"
  }

  proc Header2 { text } {
    variable markup
    
    append markup "\n\n== $text ==\n"
  }

  proc Header3 { text } {
    variable markup
    
    append markup "\n\n=== $text ===\n"
  }

  proc CR { } {
    variable markup

    append markup "\n"
  }

  proc BR { { br 1 } } {
    variable markup
    variable indent

    if { $br } {
      if { $indent } {
        append markup "\n"
        Indent
      } else {
        append markup " <br>\n"
      }
    }
  }

  proc NeedNewLine { } {
    variable markup

    if { [ string index $markup end ] ne "\n" } {
      append markup "\n"
    }
  }
   
  proc Line { } {
    variable markup

    if { [ string index $markup end ] ne "\n" } {
      append markup "\n"
    }
    append markup "#---\n"
  }

  proc AppendHT { htBuffer } {
    variable markup

    append markup $htBuffer
  }

  # esto deberia recibir un ObjectAddress
  proc BuildTolbaseInfo {icon grammar name content path desc args } {
    array set opts $args

    set extraHTInfo {}
    foreach {classOf insInfo insCont} {{} {} {}} break
    if { $grammar eq "NameBlock" } {
      if { [ info exists opts(-objref) ] } {
        set objRef $opts(-objref)
        set classOf [Tol_ClassOfFromReference $objRef]
        set insInfo [Tol_InstanceInfoFromReference $objRef]
        set insCont [Tol_InstanceContentFromReference $objRef]
      } elseif { [ info exists opts(-objaddr) ] } {
        set objAddr $opts(-objaddr)
        set classOf [Tol_ClassOf $objAddr]
        set insInfo [Tol_InstanceInfo $objAddr]
        set insCont [Tol_InstanceContent $objAddr]
      }
      if { $insInfo ne "" } {
        Backup
        Init
        Header2 [ mc "Instance's Info" ]
        foreach line [ split $insInfo \n ] {
          set s [ string index $line 0 ]
          set e [ string index $line end ]
          if { $s eq "\[" && $e eq "\]" } {
            Text "[ string range $line 1 end-1 ]:" \
                -tags {b} -br 1
          } elseif { $s eq "{" && $e eq "}" } {
            Text "[ string range $line 1 end-1 ]: " -tags {b}
          } else {
            Text \
                [ Preprocess [ string trimright $line ] ] -br 1
          }
        }
        set extraHTInfo [ Restore ]
      }
    } elseif { $grammar eq "Set" } {
      if { [ info exists opts(-objref) ] } {
        set objRef $opts(-objref)
        set classOf [Tol_StructOfFromReference $objRef]
      } elseif { [ info exists opts(-objaddr) ] } {
        set objAddr $opts(-objaddr)
        set classOf [Tol_StructOf $objAddr]
      }
      if { $classOf ne "" } {
        Backup
        Init
        BR
        Text [ mc "Definition:" ] -tags b
        CR
        IncrIndent 1
        BR
        Text "Struct" -tags b
        Text " $classOf {"
        IncrIndent 1
        BR
        set fields [ tol::info struct $classOf ]
        set ll0 [ llength $fields ]
        set ll1 [ expr { $ll0 -1 } ]
        for { set fi 0 } { $fi < $ll0 } { incr fi } {
          foreach {n t} [ lindex $fields $fi ] break
          Text "$t" -tags b
          Text " ${n};"
          if { $fi == $ll1 } {
            IncrIndent -1
          }
          BR
        }
        Text "}"
        IncrIndent -1
        BR
        set extraHTInfo [ Restore ]
      }
    }
    Text "Grammar: " -tags b
    Image $icon
    if {$classOf eq ""} {
      Text " $grammar"
    } else {
      Text " $classOf"
    }
    BR
    LabelValue "Name" $name
    if {$insCont ne ""} {
      set _content $insCont
    } elseif {$classOf ne ""} {
      set _content $classOf
    } else {
      set _content $content
    }
    BR
    if { $grammar eq "Text" } {
      Text "Content:" -tags b
      CR
      Preformatted $_content
    } else {
      LabelValue "Content" [ Verbatim [ Escape $_content ] ]
      BR
    }
    if {$path ne ""} {
      LabelValue "Path" $path
      BR
    }
    if {$desc ne ""} {
      Text "Description:" -tags b -br 1
      Text \
          [ Preprocess [ string trimright $desc ] ]
    }
    AppendHT $extraHTInfo
    return [ GetText ]
  }
}
