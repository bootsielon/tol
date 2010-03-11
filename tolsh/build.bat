set tclkitsh=tclkitsh858
copy windows\%tclkitsh%.exe windows\%tclkitsh%-rt
Windows\%tclkitsh% sdx.kit wrap tolsh.exe -runtime Windows\%tclkitsh%-rt
del windows\%tclkitsh%-rt

