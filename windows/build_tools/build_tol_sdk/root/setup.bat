@echo ==============================================================================
@echo =                                                                            = 
@echo =                 TOL - GNU/GPL Time Oriented Language                       =
@echo =                                                                            = 
@echo = TOL-SDK is used to link other programs or libraries to TOL in windows      =
@echo = without compilation of TOL project                                         = 
@echo =                                                                            = 
@echo =                     http://www.tol-project.org                             =
@echo =                                                                            = 
@echo ==============================================================================
@set  _TOL_SDK_ROOT_PATH_=C:\Users\vdebuen\prj\tolp\trunk\windows\tol-sdk
@set  TOL_SDK_INC="%_TOL_SDK_ROOT_PATH_%"
@set  TOL_SDK_LIB="_TOL_SDK_ROOT_PATH_%\lib"       
@echo Environment variables needed to compile TOL 2.0.1 in windows:
@set  _TOL_SDK_ROOT_PATH_
@set  TOL_SDK_INC
@set  TOL_SDK_LIB
@echo Exporting environment variables for all users ...
@setx TOL_SDK_INC "%_TOL_SDK_ROOT_PATH_%"                                     -m
@setx TOL_SDK_LIB "%_TOL_SDK_ROOT_PATH_%\lib"                                 -m
@pause