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
@set  _TOL_SDK_ROOT_PATH_=%CD%
@set  TOL_SDK_PATH=_TOL_SDK_ROOT_PATH_
@echo Environment variables needed to compile TOL 2.0.1 in windows:
@set  TOL_SDK_PATH
@echo Exporting environment variables for all users ...
@setx TOL_SDK_PATH "%_TOL_SDK_ROOT_PATH_%"                                     -m
@pause
