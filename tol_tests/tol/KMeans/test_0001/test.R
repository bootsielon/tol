library(tolLink)
setwd('E:/prj/tolp/tolp/trunk/tol_tests/tol/KMeans/test_0001')
cases = tolExecute('MatReadFile','cases.bbm', grammar='Matrix') 
classes = kmeans(cases,5)
classes

