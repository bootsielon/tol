#/////////////////////////////////////////////////////////////////////////////
# FILE    : ctext.tcl
# PURPOSE : By George Peter Staplin
# NOTES: Modified for the TolBase Editor
# VERSION : 3.1.3  2004 Apr 14 11:12
# KNOWN BUGS:
#
#       * The line:         " gjglkjg lkgjl  // " 1234567
#         Will highlight the ->1234567<- part as a line comment.
#
#       * Quote blinking is not done correctly.
#
#/////////////////////////////////////////////////////////////////////////////

namespace eval ctext {}

#win is used as a unique token to create arrays for each ctext instance
proc ctext::getAr {win suffix name} {
	set arName __ctext[set win][set suffix]
	uplevel [list upvar #0 $arName $name]
	return $arName
}

proc ctext {win args} {
	if {[llength $args] & 1} {
		return -code error "invalid number of arguments given to ctext (uneven number after window) : $args"
	}

	frame $win -class Ctext

	set tmp [text .__ctextTemp]

	ctext::getAr $win config ar

	set ar(-fg) [$tmp cget -foreground]
	set ar(-bg) [$tmp cget -background]
	set ar(-font) [$tmp cget -font]
	set ar(-relief) [$tmp cget -relief]
	destroy $tmp
	set ar(-yscrollcommand) ""
	set ar(-linemap) 1
	set ar(-linemapfg) $ar(-fg)
	set ar(-linemapbg) $ar(-bg)
	set ar(-linemap_mark_command) {}
	set ar(-linemap_markable) 1
	set ar(-linemap_select_fg) #008000
	set ar(-linemap_select_bg) #008000

#	set ar(-linemap_select_fg) black
#	set ar(-linemap_select_bg) yellow
	set ar(-highlight) 1
	set ar(win) $win
	set ar(modified) 0

	set ar(ctextFlags) [list -yscrollcommand -linemap -linemapfg -linemapbg \
-font -linemap_mark_command -highlight -linemap_markable -linemap_select_fg \
-linemap_select_bg]

	array set ar $args
#/////////////////////////////////////////////////////////////////////////////
# Begin modified by Bayes Forecast
    if { [lsearch [font names] ctfnBold] == -1 } {
        set fnSize [lindex $ar(-font) 1]
        if { [string match $fnSize ""]} {
            font create ctfnBold -family $ar(-font) -weight bold
        } else  {
            font create ctfnBold -family [lindex $ar(-font) 0] -size $fnSize -weight bold
        }
    }

    if { [lsearch [font names] ctfnNormal] == -1 } {
        set fnSize [lindex $ar(-font) 1]
        if { [string match $fnSize ""]} {
            font create ctfnNormal -family $ar(-font) -weight normal
        } else  {
            font create ctfnNormal -family [lindex $ar(-font) 0] -size $fnSize -weight normal
        }
    }
# End modified by Bayes Forecast
#/////////////////////////////////////////////////////////////////////////////

	foreach flag {foreground background} short {fg bg} {
		if {[info exists ar(-$flag)] == 1} {
			set ar(-$short) $ar(-$flag)
			unset ar(-$flag)
		}
	}

	#Now remove flags that will confuse text and those that need modification:
	foreach arg $ar(ctextFlags) {
		set loc [lsearch $args $arg]
		if {$loc >= 0} {
			set args [lreplace $args $loc [expr {$loc + 1}]]
		}
	}

	text $win.l -font $ar(-font) -width 1 -height 1 \
		-relief $ar(-relief) -fg $ar(-linemapfg) -bg $ar(-linemapbg) -takefocus 0

        $win.l config -fg $ar(-linemapfg) -bg $ar(-linemapbg)

	set topWin [winfo toplevel $win]
	bindtags $win.l [list $win.l $topWin all]

	if {$ar(-linemap) == 1} {
		pack $win.l -side left -fill y
	}

	set args [concat $args [list -yscrollcommand [list ctext::event:yscroll $win $ar(-yscrollcommand)]]]

	#escape $win, because it could have a space
	pack [eval text \$win.t $args -font \$ar(-font)] -side right -fill both -expand 1

	bind $win.t <Configure> [list ctext::linemapUpdate $win]
	bind $win.l <ButtonPress-1> [list ctext::linemapToggleMark $win %y]
	bind $win.t <KeyRelease-Return> [list ctext::linemapUpdate $win]
	rename $win __ctextJunk$win

	rename $win.t $win._t

    bind $win.t <Destroy> +[list rename $win._t ""]
    bind $win.t <Destroy> +[list rename $win.l ""]

	bind $win <Destroy> [list ctext::event:Destroy $win %W]
	bindtags $win.t [linsert [bindtags $win.t] 0 $win]

	interp alias {} $win {} ctext::instanceCmd $win
	interp alias {} $win.t {} $win

	#If the user wants C comments they should call ctext::enableComments
	ctext::disableComments $win
	ctext::modified $win 0
	ctext::buildArgParseTable $win

#/////////////////////////////////////////////////////////////////////////////
# Begin modified by Bayes Forecast
  ctext::enableComments $win
# End modified by Bayes Forecast
#/////////////////////////////////////////////////////////////////////////////
  ctext::modified $win 0
	return $win
}

proc ctext::event:yscroll {win clientData args} {
	ctext::linemapUpdate $win

	if {$clientData == ""} {
		return
	}
	uplevel #0 $clientData $args
}

proc ctext::event:Destroy {win dWin} {
	if {![string equal $win $dWin]} {
		return
	}
	catch {rename $win {}}
	interp alias {} $win.t {}
	ctext::clearHighlightClasses $win
	array unset [ctext::getAr $win config ar]
}

#This stores the arg table within the config array for each instance.
#It's used by the configure instance command.
proc ctext::buildArgParseTable win {
	set argTable [list]

	lappend argTable any -linemap_mark_command {
		set configAr(-linemap_mark_command) $value
		break
	}

	lappend argTable {1 true yes} -linemap {
		pack $self.l -side left -fill y
		set configAr(-linemap) 1
		break
	}

	lappend argTable {0 false no} -linemap {
		pack forget $self.l
		set configAr(-linemap) 0
		break
	}

	lappend argTable any -yscrollcommand {
		set cmd [list $self._t config -yscrollcommand [list ctext::event:yscroll $self $value]]

		if {[catch $cmd res]} {
			return $res
		}
		set configAr(-yscrollcommand) $value
		break
	}

	lappend argTable any -linemapfg {
		if {[catch {winfo rgb $self $value} res]} {
			return -code error $res
		}
		$self.l config -fg $value
		set configAr(-linemapfg) $value
		break
	}

	lappend argTable any -linemapbg {
		if {[catch {winfo rgb $self $value} res]} {
			return -code error $res
		}
		$self.l config -bg $value
		set configAr(-linemapbg) $value
		break
	}

	lappend argTable any -font {
		if {[catch {$self.l config -font $value} res]} {
			return -code error $res
		}
		$self._t config -font $value
		set configAr(-font) $value
		break
	}

	lappend argTable {0 false no} -highlight {
		set configAr(-highlight) 0
		break
	}

	lappend argTable {1 true yes} -highlight {
		set configAr(-highlight) 1
		break
	}

	lappend argTable {0 false no} -linemap_markable {
		set configAr(-linemap_markable) 0
		break
	}

	lappend argTable {1 true yes} -linemap_markable {
		set configAr(-linemap_markable) 1
		break
	}

	lappend argTable any -linemap_select_fg {
		if {[catch {winfo rgb $self $value} res]} {
			return -code error $res
		}
		set configAr(-linemap_select_fg) $value
		$self.l tag configure lmark -foreground $value
		break
	}

	lappend argTable any -linemap_select_bg {
		if {[catch {winfo rgb $self $value} res]} {
			return -code error $res
		}
		set configAr(-linemap_select_bg) $value
		$self.l tag configure lmark -background $value
		break
	}

	ctext::getAr $win config ar
	set ar(argTable) $argTable
}

proc ctext::instanceCmd {self cmd args} {
	#slightly different than the RE used in ctext::comments
  set commentRE {\"|\\|'|/|\*}
  #"
	switch -glob -- $cmd {
    append {
			if {[catch {$self._t get sel.first sel.last} data] == 0} {
				clipboard append -displayof $self $data
			}
		}
		cget {
			set arg [lindex $args 0]
			ctext::getAr $self config configAr

			foreach flag $configAr(ctextFlags) {
				if {[string match ${arg}* $flag]} {
					return [set configAr($flag)]
				}
			}
			return [$self._t cget $arg]
		}
		conf* {
			ctext::getAr $self config configAr

			if {0 == [llength $args]} {
				set res [$self._t configure]
				set del [lsearch -glob $res -yscrollcommand*]
				set res [lreplace $res $del $del]

				foreach flag $configAr(ctextFlags) {
					lappend res [list $flag [set configAr($flag)]]
				}
				return $res
			}
			array set flags {}
			foreach flag $configAr(ctextFlags) {
				set loc [lsearch $args $flag]
				if {$loc < 0} {
					continue
				}

				if {[llength $args] <= ($loc + 1)} {
					#.t config -flag
					return [set configAr($flag)]
				}

				set flagArg [lindex $args [expr {$loc + 1}]]
				set args [lreplace $args $loc [expr {$loc + 1}]]
				set flags($flag) $flagArg
			}
			foreach {valueList flag cmd} $configAr(argTable) {
				if {[info exists flags($flag)]} {
					foreach valueToCheckFor $valueList {
						set value [set flags($flag)]
						if {[string equal "any" $valueToCheckFor]} $cmd \
						elseif {[string equal $valueToCheckFor [set flags($flag)]]} $cmd
					}
				}
			}
			if {[llength $args]} {
				#we take care of configure without args at the top of this branch
				uplevel 1 [linsert $args 0 $self._t configure]
			}
		}
		copy {
			tk_textCopy $self
		}
		cut {
			if {[catch {$self.t get sel.first sel.last} data] == 0} {
				clipboard clear -displayof $self.t
				clipboard append -displayof $self.t $data
				$self delete [$self.t index sel.first] [$self.t index sel.last]
				ctext::modified $self 1
			}
		}
		delete {
			#delete n.n ?n.n

			#first deal with delete n.n
			set argsLength [llength $args]

			if {$argsLength == 1} {
				set deletePos [lindex $args 0]
				set prevChar [$self._t get $deletePos]

				$self._t delete $deletePos
				set char [$self._t get $deletePos]

				set prevSpace [ctext::findPreviousSpace $self._t $deletePos]
				set nextSpace [ctext::findNextSpace $self._t $deletePos]

				set lineStart [$self._t index "$deletePos linestart"]
				set lineEnd [$self._t index "$deletePos + 1 chars lineend"]

				if {[string equal $prevChar "#"] || [string equal $char "#"]} {
					set removeStart $lineStart
					set removeEnd $lineEnd
				} else {
					set removeStart $prevSpace
					set removeEnd $nextSpace
				}

				foreach tag [$self._t tag names] {
          # BEGIN mod Bayes:
					if { ![string equal $tag "_cComment"] && ![string equal $tag "_cQuote"] } {
						$self._t tag remove $tag $removeStart $removeEnd
					}
          #END Bayes.
				}

				set checkStr "$prevChar[set char]"

				if {[regexp $commentRE $checkStr]} {
					after idle [list ctext::comments $self]
				}
				ctext::highlight $self $lineStart $lineEnd
        ctext::linemapUpdate $self
			} elseif {$argsLength == 2} {
				#now deal with delete n.n ?n.n?
				set deleteStartPos [lindex $args 0]
				set deleteEndPos [lindex $args 1]

				set data [$self._t get $deleteStartPos $deleteEndPos]

				set lineStart [$self._t index "$deleteStartPos linestart"]
				set lineEnd [$self._t index "$deleteEndPos + 1 chars lineend"]
				eval \$self._t delete $args

				foreach tag [$self._t tag names] {
          #BEGIN mod Bayes:
          if { ![string equal $tag "_cComment"] && ![string equal $tag "_cQuote"] } {
						$self._t tag remove $tag $lineStart $lineEnd
					}
          #END Bayes.
        }

				if {[regexp $commentRE $data]} {
					after idle [list ctext::comments $self]
				}

				ctext::highlight $self $lineStart $lineEnd
				if {[string first "\n" $data] >= 0} {
          ctext::linemapUpdate $self
				}
			} else {
				return -code error "invalid argument(s) sent to $self delete: $args"
			}
			ctext::modified $self 1
		}
		fastdelete {
			eval \$self._t delete $args
			ctext::modified $self 1
			ctext::linemapUpdate $self
		}
		fastinsert {
			eval \$self._t insert $args
			ctext::modified $self 1
			ctext::linemapUpdate $self
		}
		highlight {
			ctext::highlight $self [lindex $args 0] [lindex $args 1]
			ctext::comments $self
		}

		insert {
			if {[llength $args] < 2} {
				return -code error "please use at least 2 arguments to $self insert"
			}
			set insertPos [lindex $args 0]
			set prevChar [$self._t get "$insertPos - 1 chars"]
			set nextChar [$self._t get $insertPos]
			set lineStart [$self._t index "$insertPos linestart"]
			set prevSpace [ctext::findPreviousSpace $self._t ${insertPos}-1c]
			set data [lindex $args 1]
			eval \$self._t insert $args

			set nextSpace [ctext::findNextSpace $self._t insert]
			set lineEnd [$self._t index "insert lineend"]

			if {[$self._t compare $prevSpace < $lineStart]} {
				set prevSpace $lineStart
			}

			if {[$self._t compare $nextSpace > $lineEnd]} {
				set nextSpace $lineEnd
			}

			foreach tag [$self._t tag names] {
        #BEGIN mod Bayes:
				if { ![string equal $tag "_cQuote"] && ![string equal $tag "_cComment"] } {
					$self._t tag remove $tag $prevSpace $nextSpace
				}
        #END Bayes.
			}

			set REData $prevChar
			append REData $data
			append REData $nextChar
			if {[regexp $commentRE $REData]} {
				after idle [list ctext::comments $self]
			}

			after idle [list ctext::highlight $self $lineStart $lineEnd]

			switch -- $data {
				"\}" {
					ctext::matchPair $self "\\\{" "\\\}" "\\"
				}
				"\]" {
					ctext::matchPair $self "\\\[" "\\\]" "\\"
				}
				"\)" {
					ctext::matchPair $self "\\(" "\\)" ""
				}
				"\"" {
          #"
					ctext::matchQuote $self
				}
			}
			ctext::modified $self 1
			ctext::linemapUpdate $self
		}

		paste {
			tk_textPaste $self
			ctext::modified $self 1
		}

		edit {
			set subCmd [lindex $args 0]
			set argsLength [llength $args]

			ctext::getAr $self config ar

			if {"modified" == $subCmd} {
				if {$argsLength == 1} {
					return $ar(modified)
				} elseif {$argsLength == 2} {
					set value [lindex $args 1]
					set ar(modified) $value
				} else {
					return -code error "invalid arg(s) to $self edit modified: $args"
				}
			} else {
				#Tk 8.4 has other edit subcommands that I don't want to emulate.
				return [uplevel 1 [linsert $args 0 $self._t $cmd]]
			}
		}

		default {
			return [uplevel 1 [linsert $args 0 $self._t $cmd]]
		}
	}
}

proc ctext::tag:blink {win count} {
	if {$count & 1} {
		$win tag configure __ctext_blink -foreground [$win cget -bg] -background [$win cget -fg]
	} else {
		$win tag configure __ctext_blink -foreground [$win cget -fg] -background [$win cget -bg]
	}
	if {$count == 4} {
		$win tag delete __ctext_blink 1.0 end
		return
	}
	incr count
	after 50 [list ctext::tag:blink $win $count]
}

proc ctext::matchPair {win str1 str2 escape} {
	set prevChar [$win get "insert - 2 chars"]

	if {[string equal $prevChar $escape]} {
		#The char that we thought might be the end is actually escaped.
		return
	}

	set searchRE "[set str1]|[set str2]"
	set count 1

	set pos [$win index "insert - 1 chars"]
	set endPair $pos
	set lastFound ""
	while 1 {
		set found [$win search -backwards -regexp $searchRE $pos]

		if {$found == "" || [$win compare $found > $pos]} {
			return
		}

		if {$lastFound != "" && [$win compare $found == $lastFound]} {
			#The search wrapped and found the previous search
			return
		}

		set lastFound $found
		set char [$win get $found]
		set prevChar [$win get "$found - 1 chars"]
		set pos $found

		if {[string equal $prevChar $escape]} {
			continue
		} elseif {[string equal $char [subst $str2]]} {
			incr count
		} elseif {[string equal $char [subst $str1]]} {
			incr count -1
			if {$count == 0} {
				set startPair $found
				break
			}
		} else {
			#This shouldn't happen.  I may in the future make it return -code error
			puts stderr "ctext seems to have encountered a bug in ctext::matchPair"
			return
		}
	}

	$win tag add __ctext_blink $startPair
	$win tag add __ctext_blink $endPair
	ctext::tag:blink $win 0
}

proc ctext::matchQuote {win} {
	set endQuote [$win index insert]
	set start [$win index "insert - 1 chars"]

	if {[$win get "$start - 1 chars"] == "\\"} {
		#the quote really isn't the end
		return
	}
	set lastFound ""
	while 1 {
		set startQuote [$win search -backwards \" $start]
    # "
		if {$startQuote == "" || [$win compare $startQuote > $start]} {
			#The search found nothing or it wrapped.
			return
		}

		if {$lastFound != "" && [$win compare $lastFound == $startQuote]} {
			#We found the character we found before, so it wrapped.
			return
		}
		set lastFound $startQuote
		set start [$win index "$startQuote - 1 chars"]
		set prevChar [$win get $start]

		if {$prevChar == "\\"} {
			continue
		}
		break
	}

	if {[$win compare $endQuote == $startQuote]} {
		#probably just \"
		return
	}

	$win tag add __ctext_blink $startQuote $endQuote

  ctext::tag:blink $win 0

  #Mark all the text:

}

proc ctext::enableComments {win} {
	$win tag configure _cComment -foreground #008000
	#$win tag configure _cComment -foreground green
	$win tag configure _cQuote -foreground brown
}
proc ctext::disableComments {win} {
	catch {$win tag delete _cComment}
  #BEGIN Bayes:
  catch {$win tag delete _cQuote}
  # END Bayes:
}

proc ctext::comments {win} {
	if {[catch {$win tag cget _cComment -foreground}]} {
		#C comments are disabled
		return
	}

#/////////////////////////////////////////////////////////////////////////////
# Begin modified by Bayes Forecast
# nombre de variable length x ::_length
# Added a new comment sintax: "// ... \n"
# End modified by Bayes Forecast
#/////////////////////////////////////////////////////////////////////////////

	set startIndex 1.0
  set commentRE {\\\\|\"|\\\"|\\'|'|/\*|\*/}

	set commentStart 0
  # BEGIN Bayes
  set quoteStart 0
  # END Bayes
	set isQuote 0
	set isSingleQuote 0
	set isComment 0

  $win tag remove _cComment 1.0 end
  #BEGIN Bayes:
  $win tag remove _cQuote 1.0 end
  #END Bayes:
  while 1 {
    set index [$win search -count ::_length -regexp $commentRE $startIndex end]
		if {$index == ""} {
			break
		}
		set endIndex [$win index "$index + $::_length chars"]
		set str [$win get $index $endIndex]
		set startIndex $endIndex

		if {$str == "\\\\"} {
			continue
		} elseif {$str == "\\\""} {
      # \"
			continue
		} elseif {$str == "\\'"} {
			continue
		} elseif {$str == "\"" && $isComment == 0 && $isSingleQuote == 0} {
      # "
			if {$isQuote} {
				set isQuote 0
        # BEGIN Bayes
        $win tag add _cQuote $quoteStart $endIndex
				$win tag raise _cQuote
        # END Bayes
			} else {
				set isQuote 1
        # BEGIN Bayes
        set quoteStart $index
        # END Bayes
      }
		} elseif {$str == "'" && $isComment == 0 && $isQuote == 0} {
			if {$isSingleQuote} {
				set isSingleQuote 0
			} else {
				set isSingleQuote 1
			}
		} elseif {$str == "/*" && $isQuote == 0 && $isSingleQuote == 0} {
			if {$isComment} {
				#comment in comment
        # BEGIN Bayes (Commented next line)
				#break
        # END Bayes
			} else {
				set isComment 1
				set commentStart $index
			}
		} elseif {$str == "*/" && $isQuote == 0 && $isSingleQuote == 0} {
			if {$isComment} {
        set isComment 0
				$win tag add _cComment $commentStart $endIndex
				$win tag raise _cComment
			} else {
				#comment end without beginning
				break
			}
		}
	}
}

proc ctext::addHighlightClass {win class color keywords} {
	set ref [ctext::getAr $win highlight ar]
	foreach word $keywords {
		set ar($word) [list $class $color]
	}
	$win tag configure $class

	ctext::getAr $win classes classesAr
	set classesAr($class) [list $ref $keywords]
}

#For [ ] { } # etc.
proc ctext::addHighlightClassForSpecialChars {win class color chars} {
	set charList [split $chars ""]

	set ref [ctext::getAr $win highlightSpecialChars ar]
	foreach char $charList {
		set ar($char) [list $class $color]
	}
	$win tag configure $class

	ctext::getAr $win classes classesAr
	set classesAr($class) [list $ref $charList]
}

proc ctext::addHighlightClassForRegexp {win class color re} {
	set ref [ctext::getAr $win highlightRegexp ar]

	set ar($class) [list $re $color]
	$win tag configure $class

	ctext::getAr $win classes classesAr
	set classesAr($class) [list $ref $class]
}

#For things like $blah
proc ctext::addHighlightClassWithOnlyCharStart {win class color char} {
	set ref [ctext::getAr $win highlightCharStart ar]

	set ar($char) [list $class $color]
	$win tag configure $class

	ctext::getAr $win classes classesAr
	set classesAr($class) [list $ref $char]
}

proc ctext::deleteHighlightClass {win classToDelete} {
	ctext::getAr $win classes classesAr

	if {![info exists classesAr($classToDelete)]} {
		return -code error "$classToDelete doesn't exist"
	}

	foreach {ref keyList} [set classesAr($classToDelete)] {
		upvar #0 $ref refAr
		foreach key $keyList {
			if {![info exists refAr($key)]} {
				continue
			}
			unset refAr($key)
		}
	}
	unset classesAr($classToDelete)
}

proc ctext::getHighlightClasses win {
	ctext::getAr $win classes classesAr

	set res [list]

	foreach {class info} [array get classesAr] {
		lappend res $class
	}
	return $res
}

proc ctext::findNextChar {win index char} {
	set i [$win index "$index + 1 chars"]
	set lineend [$win index "$i lineend"]
	while 1 {
		set ch [$win get $i]
		if {[$win compare $i >= $lineend]} {
			return ""
		}
		if {$ch == $char} {
			return $i
		}
		set i [$win index "$i + 1 chars"]
	}
}

proc ctext::findNextSpace {win index} {
	set i [$win index $index]
	set lineStart [$win index "$i linestart"]
	set lineEnd [$win index "$i lineend"]
	#Sometimes the lineend fails (I don't know why), so add 1 and try again.
	if {[$win compare $lineEnd == $lineStart]} {
		set lineEnd [$win index "$i + 1 chars lineend"]
	}

	while {1} {
		set ch [$win get $i]

		if {[$win compare $i >= $lineEnd]} {
			set i $lineEnd
			break
		}

		if {[string is space $ch]} {
			break
		}
		set i [$win index "$i + 1 chars"]
	}
	return $i
}

proc ctext::findPreviousSpace {win index} {
	set i [$win index $index]
	set lineStart [$win index "$i linestart"]
	while {1} {
		set ch [$win get $i]

		if {[$win compare $i <= $lineStart]} {
			set i $lineStart
			break
		}

		if {[string is space $ch]} {
			break
		}

		set i [$win index "$i - 1 chars"]
	}
	return $i
}

proc ctext::clearHighlightClasses {win} {
	#no need to catch, because array unset doesn't complain
	#puts [array exists ::ctext::highlight$win]

	ctext::getAr $win highlight ar
	array unset ar

	ctext::getAr $win highlightSpecialChars ar
	array unset ar

	ctext::getAr $win highlightRegexp ar
	array unset ar

	ctext::getAr $win highlightCharStart ar
	array unset ar

	ctext::getAr $win classes ar
	array unset ar
}

#This is a proc designed to be overwritten by the user.
#It can be used to update a cursor or animation while
#the text is being highlighted.
proc ctext::update {} {

}

proc ctext::highlight {win start end} {
  ctext::getAr $win config configAr
	if {!$configAr(-highlight)} {
		return
	}

	set si $start
	set twin "$win._t"

	#The number of times the loop has run.
	set numTimesLooped 0
	set numUntilUpdate 600

#/////////////////////////////////////////////////////////////////////////////
# Begin modified by Bayes Forecast
# nombre de variable length x ::_length
# End modified by Bayes Forecast
#/////////////////////////////////////////////////////////////////////////////

	ctext::getAr $win highlight highlightAr
	ctext::getAr $win highlightSpecialChars highlightSpecialCharsAr
	ctext::getAr $win highlightRegexp highlightRegexpAr
	ctext::getAr $win highlightCharStart highlightCharStartAr

	while 1 {
		set res [$twin search -count ::_length -regexp -- {([^\s\(\{\[\}\]\)\.\t\n\r;\"'\|,]+)} $si $end]
    #"
		if {$res == ""} {
			break
		}

		set wordEnd [$twin index "$res + $::_length chars"]
		set word [$twin get $res $wordEnd]
		set firstOfWord [string index $word 0]

		if {[info exists highlightAr($word)] == 1} {
			set wordAttributes [set highlightAr($word)]
			foreach {tagClass color} $wordAttributes break

			$twin tag add $tagClass $res $wordEnd
#/////////////////////////////////////////////////////////////////////////////
# Begin modified by Bayes Forecast
      if {$color eq "black"} {
        $twin tag configure $tagClass -foreground $color -font ctfnBold
      } else  {
        $twin tag configure $tagClass -foreground $color -font ctfnNormal
      }
# End modified by Bayes Forecast
#/////////////////////////////////////////////////////////////////////////////

		} elseif {[info exists highlightCharStartAr($firstOfWord)] == 1} {
			set wordAttributes [set highlightCharStartAr($firstOfWord)]
			foreach {tagClass color} $wordAttributes break
			$twin tag add $tagClass $res $wordEnd
#/////////////////////////////////////////////////////////////////////////////
# Begin modified by Bayes Forecast
      $twin tag configure $tagClass -foreground $color -font ctfnNormal
# End modified by Bayes Forecast
#/////////////////////////////////////////////////////////////////////////////

		}
		set si $wordEnd

		incr numTimesLooped
		if {$numTimesLooped >= $numUntilUpdate} {
			ctext::update
			set numTimesLooped 0
		}
	}

	foreach {ichar tagInfo} [array get highlightSpecialCharsAr] {
		set si $start
		foreach {tagClass color} $tagInfo break

		while 1 {
			set res [$twin search -- $ichar $si $end]
			if {"" == $res} {
				break
			}
			set wordEnd [$twin index "$res + 1 chars"]

			$twin tag add $tagClass $res $wordEnd
			$twin tag configure $tagClass -foreground $color

      set si $wordEnd

			incr numTimesLooped
			if {$numTimesLooped >= $numUntilUpdate} {
				ctext::update
				set numTimesLooped 0
			}
		}
	}

  foreach {tagClass tagInfo} [array get highlightRegexpAr] {
    set si $start
		foreach {re color} $tagInfo break

#/////////////////////////////////////////////////////////////////////////////
# Begin modified by Bayes Forecast
# nombre de variable length x ::_length
# End modified by Bayes Forecast
#/////////////////////////////////////////////////////////////////////////////

		while 1 {
			set res [$twin search -count ::_length -regexp -- $re $si $end]
			if {"" == $res} {
				break
			}

			set wordEnd [$twin index "$res + $::_length chars"]
			$twin tag add $tagClass $res $wordEnd
			$twin tag configure $tagClass -foreground $color
      set si $wordEnd

			incr numTimesLooped
			if {$numTimesLooped >= $numUntilUpdate} {
				ctext::update
				set numTimesLooped 0
			}
    }
	}
}

proc ctext::linemapToggleMark {win y} {
	ctext::getAr $win config configAr

	if {!$configAr(-linemap_markable)} {
		return
	}

	set markChar [$win.l index @0,$y]
	set lineSelected [lindex [split $markChar .] 0]
	set line [$win.l get $lineSelected.0 $lineSelected.end]

	if {$line == ""} {
		return
	}

	ctext::getAr $win linemap linemapAr

	if {[info exists linemapAr($line)] == 1} {
		#It's already marked, so unmark it.
		array unset linemapAr $line
		ctext::linemapUpdate $win
		set type unmarked
	} else {
		#This means that the line isn't toggled, so toggle it.
		array set linemapAr [list $line {}]
		$win.l tag add lmark $markChar [$win.l index "$markChar lineend"] 
		$win.l tag configure lmark -foreground $configAr(-linemap_select_fg) \
-background $configAr(-linemap_select_bg)
		set type marked
	}

	if {[string length $configAr(-linemap_mark_command)]} {
		uplevel #0 [linsert $configAr(-linemap_mark_command) end $win $type $line]
	}
}

#args is here because -yscrollcommand may call it
proc ctext::linemapUpdate {win args} {
	if {[winfo exists $win.l] != 1} {
		return
	}

	set pixel 0
	set lastLine {}
	set lineList [list]
	set fontMetrics [font metrics [$win._t cget -font]]
	set incrBy [expr {1 + ([lindex $fontMetrics 5] / 2)}]

	while {$pixel < [winfo height $win.l]} {
		set idx [$win._t index @0,$pixel]

		if {$idx != $lastLine} {
			set line [lindex [split $idx .] 0]
			set lastLine $idx
			$win.l config -width [string length $line]
			lappend lineList $line
		}
		incr pixel $incrBy
	}

	ctext::getAr $win linemap linemapAr

	$win.l delete 1.0 end
	set lastLine {}
	foreach line $lineList {
		if {$line == $lastLine} {
			$win.l insert end "\n"
		} else {
			if {[info exists linemapAr($line)]} {
				$win.l insert end "$line\n" lmark
			} else {
				$win.l insert end "$line\n"
			}
		}
		set lastLine $line
	}
}

proc ctext::modified {win value} {
	ctext::getAr $win config ar
	set ar(modified) $value
	event generate $win <<Modified>>
	return $value
}
