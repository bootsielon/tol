#! /bin/bash
#Generates windows library cblas.dll and its cblas.lib from atlas cygwin 
#environment to be used by Visual C++ 2005
#This file must be copied to the lib prefix path used in atlas configuration
#where liblapack.a and libcblas.lib were stored
#You must run first scritp libcblas_win_dll.sh to generate libcblas.lib

GCC=gcc
defname=libclapack.def
dllname=libclapack.dll
libbat=generate_libclapack_implib.bat
CLIBPATH=/lib/gcc/i686-pc-cygwin/3.4.4
MINGWPATH=/lib/mingw

#--------------------------------------------
# .DEF file generation
#--------------------------------------------
echo 'Generating the .def file...'

Cexports=(\
# CLAPACK interface subset
	clapack_sgesv clapack_sgetrf clapack_sgetrs clapack_sgetri \
	clapack_sposv clapack_spotrf clapack_spotrs clapack_spotri \
	clapack_slauum clapack_strtri \
	clapack_dgesv clapack_dgetrf clapack_dgetrs clapack_dgetri \
	clapack_dposv clapack_dpotrf clapack_dpotrs clapack_dpotri \
	clapack_dlauum clapack_dtrtri \
	clapack_cgesv clapack_cgetrf clapack_cgetrs clapack_cgetri \
	clapack_cposv clapack_cpotrf clapack_cpotrs clapack_cpotri \
	clapack_clauum clapack_ctrtri \
	clapack_zgesv clapack_zgetrf clapack_zgetrs clapack_zgetri \
	clapack_zposv clapack_zpotrf clapack_zpotrs clapack_zpotri \
	clapack_zlauum clapack_ztrtri)


#--------------------------------------------
# Generate a .def file
#--------------------------------------------
echo "EXPORTS" > ${defname}
for (( i = 0 ; i < ${#Cexports[*]} ; i++ )) ; do
	export=${Cexports[$i]}
	echo -e "${export}=${export}" >> ${defname}
done

#--------------------------------------------
# Making a .DLL from the .a files
#--------------------------------------------
echo 'Generating windows .dll file...'


${GCC} -mno-cygwin -shared -o ${dllname} ${defname} liblapack.a libcblas.lib -Wl,--enable-auto-import -Wl,--no-whole-archive libatlas.a


#--------------------------------------------
# Making a .bat for the import-library
#
# TODO: figure out a better way to find vcvars32.bat
#--------------------------------------------
echo 'Generating the import library script...'
echo " " > $libbat
echo -e 'call "%ProgramFiles%/Microsoft Visual Studio 8/VC/bin/vcvars32.bat"' >> $libbat
echo -e 'lib.exe /DEF:libclapack.def /MACHINE:Ix86 /OUT:libclapack.lib' >> $libbat

echo 'Running the import library script...'
chmod +x ./$libbat
./$libbat


