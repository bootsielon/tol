#/////////////////////////////////////////////////////////////////////////////
# Object Inspector Messages
#/////////////////////////////////////////////////////////////////////////////
mcset en "VarWithoutNameNotFunc" \
  "They are not possible to be applied no function on a global object without name"

#/////////////////////////////////////////////////////////////////////////////
# Set Graph Messages
#/////////////////////////////////////////////////////////////////////////////
mcset en "UniqueXAxis"      "If you choose a text, date or generic column as x data of a serie, \
                            you must choose this column as x data of all of the series."

#/////////////////////////////////////////////////////////////////////////////
# Serie Graph Messages
#/////////////////////////////////////////////////////////////////////////////
mcset en "showDesc"         "Show the description like name of the series"
#/////////////////////////////////////////////////////////////////////////////
#Find Messages
#/////////////////////////////////////////////////////////////////////////////
mcset en "SearchUpEndDoc"   "You have searched up to the end of the document"
mcset en "SearchUpBeginDoc" "You have searched up the beginning the document"
mcset en "FindFromBegin"    "Do you want to continue the search from the beginning of the document?" 
mcset en "FindFromEnd"      "Do you want to continue the search from the end of the document?"

#/////////////////////////////////////////////////////////////////////////////
#Format Number
#/////////////////////////////////////////////////////////////////////////////

mcset en "ForNumOpc:-" "Specifies that the converted argument should be left-justified in its field"
mcset en "ForNumOpc:+" "Specifies that a number should always be printed with a sign"
mcset en "ForNumOpcSpace" \
         "Specifies that a space should be added to the beginning of the number if the first character isn't a sign"
mcset en "ForNumOpc:Zero" \
         "Specifies that the number should be padded on the left with\n\
          zeroes instead of spaces"
mcset en "ForNumOpc:f" \
         "Convert floating-point number to signed decimal string of the form xx.yyy,\n\
          where the number of y's is determined by the precision (default: 6).\n\
          If the precision is 0 then no decimal point is output"
mcset en "ForNumOpc:e" \
         "Convert floating-point number to scientific notation in the form x.yyye±zz,\n\
          where the number of y's is determined by the precision (default: 6).\n\
          If the precision is 0 then no decimal point is output.\n\
          If the E form is used then E is printed instead of e"
mcset en "ForNumOpc:g" \
         "If the exponent is less than -4 or greater than or equal to the precision,\n\
          then convert floating-point number as for %e or %E.\n Otherwise convert as for %f.\n\
          Trailing zeroes and a trailing decimal point are omitted"

#/////////////////////////////////////////////////////////////////////////////
#Option Graph
#/////////////////////////////////////////////////////////////////////////////

mcset en "AddMarkSel"  "Append to mark in the selection"

#/////////////////////////////////////////////////////////////////////////////
#ODBC
#/////////////////////////////////////////////////////////////////////////////
mcset en ShowDialogODBC \
  "To show dialog of connection database when starting TOLBase"

#/////////////////////////////////////////////////////////////////////////////
# Selector Colums Graph
#/////////////////////////////////////////////////////////////////////////////
mcset en SetGrapthAllFirst \
  "All the first one"

mcset en SetGrapthAllFirstHint \
  "Generates all possible combinations to graphicate first column to each other"

mcset en SetGrapthAllGeneric \
  "All the generic one"

mcset en SetGrapthAllGenericHint \
  "Generates all possible combinations to graphicate generic column to each other"

mcset en SetGrapthEvenUneven \
  "Even to odd ones"

mcset en SetGrapthEvenUnevenHint \
  "Generates all possible combinations to graphicate even columns to odd ones.\n\
   For example 1 to 2, 3 to 4, etc."
