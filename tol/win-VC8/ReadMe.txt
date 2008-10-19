/////////////////////////////////////////////////////////////////////////////
// Visual C++ 2005 project to build Time Oriented Language (TOL)
/////////////////////////////////////////////////////////////////////////////

You need Visual C++ 2005 professional edition and Microsoft Platform SDK.

You'll need SDK 2003 that is available from

  http://www.microsoft.com/msdownload/platformsdk/sdkupdate/

See instructions at

  http://msdn.microsoft.com/vstudio/express/visualc/usingpsdk/
or
  http://msdn2.microsoft.com/es-es/express/aa700755(en-us).aspx


You should follow at least first four steps. In Step 3: Update the Visual 
C++ directories in the Projects and Solutions section in the Options dialog 
box. You must use second explained method, it's to say, YOU MUST update the 
Visual C++ Directories by modifying the VCProjectEngine.dll.express.config 
file located in the \vc\vcpackages subdirectory of the Visual C++ Express 
install location. Please make sure that you also delete, if it exists before 
restarting Visual C++ Express Edition, the file "vccomponents.dat" located 
in the directory

  "%USERPROFILE%\Local Settings\Application Data\Microsoft\VCExpress\8.0"
  
Depending on your system configuration this directory could be different.
Probably you can find it by typing this in a console

  echo "%APPDATA%\Microsoft\VCExpress\8.0" 

In english Windows 2000 versions it will be something as this

  C:\Documents and Settings\<USER>\Application Data\Microsoft\VCExpress\8.0
    
In spanish Windows 2000 versions it will be something as this

  C:\Documents and Settings\<USER>\Datos de programa\Microsoft\VCExpress\8.0
  
Also you can use the explorer searcher to find a directory called vcpackages
