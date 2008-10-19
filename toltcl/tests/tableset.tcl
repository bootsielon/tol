#//////////////////////////////////////////////////////////////////////////////
#
# FILE: tableset.tcl
#
# PURPOSE: test the command tol::tableset
#
#//////////////////////////////////////////////////////////////////////////////



#//////////////////////////////////////////////////////////////////////////////
#
# PROCEDURE: TestWrong
#
# PURPOSE: Test that doesn't produce the expected result
#
#//////////////////////////////////////////////////////////////////////////////
proc TestWrong { } {
  #no puede crear el comando
  tol::tableset create tblset AllLinear
  tol::tableset destroy tblset 
}

proc Write {args} {
  puts $args
}

proc TestWrong1 { } {
  #
  tol::tableset create tblset AxisDatingList
  puts "Da como resultado [tblset info datarows] filas, en realidad son 10"
  puts "Vamos a imprimir todo el contenido del conjunto"
  tblset data apply all Write
  puts "No aparece entero"
  puts [tblset info rowsnames]
  tol::tableset destroy tblset
  
}

#//////////////////////////////////////////////////////////////////////////////
#
# PROCEDURE: TestGenericHeader
#
# PURPOSE: test over a set with generic header
#
#//////////////////////////////////////////////////////////////////////////////

proc TestGenericHeader { } {
  puts "Testing: Generic Header ..."
  tol::console eval {Set setGenHead = [[ [[1,2,3]], [[2,4]], [[6,7,8]] ]]; }
  tol::tableset create tblset setGenHead
  # test the results
  set header_type [tblset info header]
  if { [string equal $header_type "generic"] } {
    puts "Header type '$header_type' ok"
    set idx 1
    foreach lstColInfo [tblset info columns] {
      array set arrColInfo $lstColInfo
      if { [string equal $arrColInfo(name) "Column$idx"] } {
	puts "Column $idx name '$arrColInfo(name)' ok"
      } else {
	puts "Column $idx name check failed: $arrColInfo(name) and should be Column$idx"
      }
      incr idx
    }
  } else {
    puts "Header type check failed: [tblset info header] and should be generic"
  }
  tol::tableset destroy tblset
  tol::console stack release setGenHead 
}

#//////////////////////////////////////////////////////////////////////////////
#
# PROCEDURE: TestStructHeader
#
# PURPOSE: test over a set with Struct header
#
#//////////////////////////////////////////////////////////////////////////////


proc TestStructHeader { } {
  puts "Testing: Struc Header ..."
  tol::console eval {Struct STR { Real RealField; Text TextField };}
  tol::console eval {Set SET = [[ STR(1,"value1"), STR(2,"value2") ]];}
  tol::tableset create tblset SET
  # test the results
  set header_type [tblset info header]
  if { [string equal $header_type "struct"] } {
    puts "Header type '$header_type' ok"
    set idx 1
    foreach name {"RealField" "TextField"} lstColInfo [tblset info columns] {
      array set arrColInfo $lstColInfo
      if { [string equal $arrColInfo(name) $name] } {
	puts "Column $idx name '$name' ok"
      } else {
	puts "Column $idx name check failed: '$arrColInfo(name)' and should be '$name'"
      }
      incr idx
    }
  } else {
    puts "Header type check failed: $header_type and should be struct"
  }
  tol::tableset destroy tblset
  tol::console stack release STR SET
}

#//////////////////////////////////////////////////////////////////////////////
#
# PROCEDURE: TestRowHeader
#
# PURPOSE: test over a set with row header
#
#//////////////////////////////////////////////////////////////////////////////

proc TestRowHeader { } {
  puts "Testing: Row Header ..."
  tol::console eval {Set setGenHead = \
			 [[ [["A","B","C"]], [[1,2,3]], [[2,4]], [[6,7,8]] ]]; }
  tol::tableset create tblset setGenHead
  # test the results
  set header_type [tblset info header]
  if { [string equal [lindex $header_type 0] "row"] } {
    puts "Header type '$header_type' ok"
    set idx 1
    foreach name {"A" "B" "C"} lstColInfo [tblset info columns] {
      array set arrColInfo $lstColInfo
      if { [string equal $arrColInfo(name) $name] } {
	puts "Column $idx name '$name' ok"
      } else {
	puts "Column $idx name check failed: '$arrColInfo(name)' and should be '$name'"
      }
      incr idx
    }
  } else {
    puts "Header type check failed: [tblset info header] and should be row"
  }
  tol::tableset destroy tblset
  tol::console stack release setGenHead 
}

#//////////////////////////////////////////////////////////////////////////////
#
# PROCEDURE: TestRealData
#
# PURPOSE: test over a set with real content
#
#//////////////////////////////////////////////////////////////////////////////

proc TestRealData { } {
  puts "Testing set with real content ..."
  tol::console eval {Set realSet = [[ [[1,?,3,5,6]], [[4,Real(1/0),?]], [[7,Real(-1/0),9,7]] ]];}
  tol::tableset create tblset realSet
  puts [tblset info columns]
  puts [tblset data column 0]
  puts [tblset data column 1]
  puts [tblset data column 2]
  puts [tblset data column 3]
  puts [tblset data column 4]
  tol::tableset destroy tblset
  tol::console stack release realSet
}

#//////////////////////////////////////////////////////////////////////////////
#
# PROCEDURE: TestDateData
#
# PURPOSE: test over a set with date content
#
#//////////////////////////////////////////////////////////////////////////////

proc TestDateData { } {
  puts "Testing set with date content ..."
  tol::console eval {Set dteSet = [[ [["Date1", "Date2"]], [[y1999,y2000]], [[y2001,y2002]] ]];}
  tol::tableset create tblset dteSet
  puts [tblset info columns]
  puts [tblset data column 0]
  puts [tblset data column 1]  
  tol::tableset destroy tblset
  tol::console stack release dteSet
}

#//////////////////////////////////////////////////////////////////////////////
#
# PROCEDURE: TestMixedColumn
#
# PURPOSE: test over a set with column having all checked data type:
#          Real, Date, & Text 
#
#//////////////////////////////////////////////////////////////////////////////

proc TestMixedColumn { } {
  puts "Testing set with mixed content ..."
  tol::console eval {Set mixSet = [[ [["Real", "Date", "Text"]],
				     [[1,      y2000,   "aaa"]],
				     [[2,      y2001,   "bbb"]] ]];}
  tol::tableset create tblset mixSet
  puts [tblset info columns]
  puts [tblset data column 0]
  puts [tblset data column 1]  
  puts [tblset data column 2]  
  tol::tableset destroy tblset
  tol::console stack release mixSet
}

#//////////////////////////////////////////////////////////////////////////////
#
# PROCEDURE: TestTextData
#
# PURPOSE: test over a set with generic (Text) content. In fact we'll have a
#          row that start in Real and Date and will be converted because of 
#          the appearance of a black lamb,  neither Real nor Date SyntaxObject.
#
#//////////////////////////////////////////////////////////////////////////////

proc TestTextData { } {
  puts "Testing set with mixed content ..."
  tol::console eval {Set mixSet = [[ [["RealMix", "DateMix"]],
				     [[1,          y2000,   "aaa", "kkk"]],
				     [[y2002,      "aaa",   "bbb"]] ]];}
  tol::tableset create tblset mixSet
  puts "Header info : [tblset info header]"
  puts [tblset info columns]
  puts [tblset data column 0]
  puts [tblset data column 1]  
  puts [tblset data column 2]  
  puts [tblset data column 3]  
  tol::tableset destroy tblset
  tol::console stack release mixSet
}

proc TestError { } {
  tol::console eval {Set __error = [[ [[ 1, 1.5, 2, 3]],
				      [[ 2, 2.5, 5, 2]],
				      [[ 3, 3.5, 4   ]],
				      [[ 4, 4.5, 0   ]],
				      [[ 5, 5.5, 6, 2]],
				      [[ 6, 6.5, 1, 6]],
				      [[ 7, 7.5, 3   ]],
				      [[ 8, 8.5, 4   ]],
				      [[ 9, 9.5, 9, 1]],
				      [[10,10.5, 2, 4]],
				      [[11,11.5, 4   ]],
				      [[12,12.5, 7, 8]] ]]; }
  tol::tableset create tblset __error
  tol::tableset destroy tblset
}

proc TestNames {  } {
  tol::console eval {Set row1 = [[Real col1 = 11, Real col2 = 12]];}
  tol::console eval {Set s1 = [[row1]];}
  
  tol::tableset create tbl s1
  puts [tbl info columns]
  tol::tableset destroy tbl
  tol::console stack release s1 row1
}
