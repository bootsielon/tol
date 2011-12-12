#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Release
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/toljavajni.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m32 -D"__USE_HASH_MAP__=__HASH_MAP_GOOGLE__" -D"__USE_POOL__=__POOL_BFSMEM__" -DHAVE_CONFIG_H -DUSE_DELAY_INIT -D_USE_MM_IO_ -D__HASH_MAP_GCC__=3 -D__HASH_MAP_GOOGLE__=2 -D__HASH_MAP_ICC__=4 -D__HASH_MAP_MSVC__=1 -D__POOL_BFSMEM__=1 -D__POOL_NONE__=0 -D__USE_DYNSCOPE__ -D__USE_ZIP_ARCHIVE__
CXXFLAGS=-m32 -D"__USE_HASH_MAP__=__HASH_MAP_GOOGLE__" -D"__USE_POOL__=__POOL_BFSMEM__" -DHAVE_CONFIG_H -DUSE_DELAY_INIT -D_USE_MM_IO_ -D__HASH_MAP_GCC__=3 -D__HASH_MAP_GOOGLE__=2 -D__HASH_MAP_ICC__=4 -D__HASH_MAP_MSVC__=1 -D__POOL_BFSMEM__=1 -D__POOL_NONE__=0 -D__USE_DYNSCOPE__ -D__USE_ZIP_ARCHIVE__

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=/usr/local/tol201-gcc-release/lib/libtol.so

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Release.mk dist/Release/GNU-Linux-x86/libTOLJavaJNI-V2.so

dist/Release/GNU-Linux-x86/libTOLJavaJNI-V2.so: /usr/local/tol201-gcc-release/lib/libtol.so

dist/Release/GNU-Linux-x86/libTOLJavaJNI-V2.so: ${OBJECTFILES}
	${MKDIR} -p dist/Release/GNU-Linux-x86
	${LINK.cc} -shared -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libTOLJavaJNI-V2.so -fPIC ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/toljavajni.o: nbproject/Makefile-${CND_CONF}.mk toljavajni.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -I/usr/local/tol201-gcc-release/include -I/usr/lib/jvm/java-6-sun/include -I/usr/lib/jvm/java-6-sun/include/linux -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/toljavajni.o toljavajni.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Release
	${RM} dist/Release/GNU-Linux-x86/libTOLJavaJNI-V2.so

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
