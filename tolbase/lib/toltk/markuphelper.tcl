namespace eval ::MarkupHelper {
  variable markup

  proc Init { } {
    variable markup {}
  }

  proc Verbatim { text } {
    return [ string map {\n \n<br>} $text ]
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

  proc BR { { br 1 } } {
    variable markup

    if { $br } {
      append markup " <br>"
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

  # esto deberia recibir un ObjectAddress
  proc BuildTolbaseInfo {icon grammar name content path desc args } {
    array set opts $args

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
    LabelValue "Content" [ Escape $_content ]
    if {$path ne ""} {
      BR
      LabelValue "Path" $path
    }
    if {$desc ne ""} {
      BR
      Text "Description:" -tags b -br 1
      Text \
          [ Preprocess [ string trimright $desc ] ]
    }
    if { $insInfo ne "" } {
      Header2 "Instance's Info"
      foreach line [ split $insInfo \n ] {
        set s [ string index $line 0 ]
        set e [ string index $line end ]
        if { $s eq "\[" && $e eq "\]" } {
          Text "[ string range $line 1 end-1 ]:" \
              -tags {b i} -br 1
        } elseif { $s eq "{" && $e eq "}" } {
          Text "[ string range $line 1 end-1 ]: " -tags {b i}
        } else {
          Text \
              [ Preprocess [ string trimright $line ] ] -br 1
        }
      }
    }
    return "<m>[ GetText ]</m>"
  }
}
