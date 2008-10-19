Attribute VB_Name = "Tol"

Declare Sub TolInit Lib "VBTol.dll" (ByVal addr As Long, ByVal vmode As String)

Declare Function TolEval Lib "VBTol.dll" (ByVal expr As String, ByRef nerror As Integer, ByRef nwarning As Integer) As Integer

Declare Function TolDecompile Lib "VBTol.dll" (ByVal name As String) As Integer

Declare Function TolDecompileAll Lib "VBTol.dll" () As Integer
