# This is a template file, it will not work if don't modify it to
# something useful.

tol::include   file.tol
tol::decompile file.tol
tol::include   file.tol
set quality 1.0
set partial "[tol::info variable Grammar Name]"