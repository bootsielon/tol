// toldll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#if defined(_MSC_VER)
#  include <win_tolinc.h>
#endif

#include "tol/tol_bcommon.h"

BTraceInit("toldll.cpp");

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

