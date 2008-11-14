Attribute VB_Name = "Tol"

Declare Sub TolInit Lib "VBTol.dll" (ByVal addr As Long, ByVal vmode As String, _
                                     ByVal initlib As Integer)

Declare Function TolEval Lib "VBTol.dll" (ByVal expr As String, ByRef nerror As Integer, _
                                          ByRef nwarning As Integer) As Integer

Declare Function TolDecompile Lib "VBTol.dll" (ByVal name As String) As Integer

Declare Function TolDecompileAll Lib "VBTol.dll" () As Integer

Declare Function TolGetText Lib "VBTol.dll" (ByVal lvalue As String, ByRef retvalue As String) As Integer

Declare Function TolCreateText Lib "VBTol.dll" (ByVal name As String, ByVal value As String) As Integer

Const Known As Integer = 0
Const Unknown As Integer = 1
Const PosInf As Integer = 2
Const NegInf As Integer = 3

Type TolReal
  kind As Integer
  value As Double
End Type

Declare Function TolGetReal Lib "VBTol.dll" (ByVal lvalue As String, ByRef retvalue As TolReal) As Integer

Declare Function TolCreateReal Lib "VBTol.dll" (ByVal name As String, ByRef value As TolReal) As Integer

Type MaskCell
  row As Long
  col As Long
  kind As Integer
End Type

Declare Function TolGetMatrix Lib "VBTol.dll" (ByVal lvalue As String, ByRef retvalue() As Double, _
                                               ByRef retmask() As MaskCell) As Integer

Declare Function TolCreateMatrix Lib "VBTol.dll" (ByVal name As String, ByRef value() As Double, _
                                                  ByRef mask() As MaskCell) As Integer

Declare Function TolGetRange Lib "VBTol.dll" (ByVal lvalue As String, _
    ByVal frow As Long, ByVal lrow As Long, ByVal fcol As Long, ByVal fcol As Long, _
    ByRef retvalue() As Double, ByRef retmask() As MaskCell) As Integer

Declare Function TolGetRow Lib "VBTol.dll" (ByVal lvalue As String, ByVal row As Long, _
                                            ByRef retvalue() As Double, ByRef retmask() As MaskCell) As Integer

Declare Function TolGetColumn Lib "VBTol.dll" (ByVal lvalue As String, ByVal col As Long, _
                                               ByRef retvalue() As Double, ByRef retmask() As MaskCell) As Integer

Declare Function TolGetCell Lib "VBTol.dll" (ByVal lvalue As String, ByVal row As Long, ByVal col As Long, _
                                             ByRef retvalue As TolReal) As Integer

Declare Function TolPutRange Lib "VBTol.dll" (ByVal lvalue As String, ByVal row As Long, ByVal col As Long, _
                                              ByRef value() As Double, ByRef mask() As MaskCell) As Integer

Declare Function TolPutRow Lib "VBTol.dll" (ByVal lvalue As String, ByVal row As Long, _
                                              ByRef value() As Double, ByRef mask() As MaskCell) As Integer

Declare Function TolPutColumn Lib "VBTol.dll" (ByVal lvalue As String, ByVal col As Long, _
                                              ByRef value() As Double, ByRef mask() As MaskCell) As Integer

Declare Function TolPutCell Lib "VBTol.dll" (ByVal lvalue As String, ByVal row As Long, ByVal col As Long, _
                                             ByRef value As TolReal) As Integer
