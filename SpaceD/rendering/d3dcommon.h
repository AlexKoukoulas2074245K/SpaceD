/********************************************************************/
/** d3dcommon.h by Alex Koukoulas (C) 2017 All Rights Reserved     **/
/** File Description:                                              **/
/********************************************************************/

#pragma once

// Local Headers

// Remote Headers
#include <d3d11.h>
#include <wrl\client.h>
#include <dxerr.h>
#include <stdio.h>

// Com pointer convenience macro
#define comptr Microsoft::WRL::ComPtr

// Linking to external libs
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxerr.lib")
#pragma comment(lib, "legacy_stdio_definitions.lib")

// D3D HR Debugging Macro
#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)                                               \
        {                                                           \
            HRESULT hr = (x);                                       \
            if(FAILED(hr))                                          \
            {                                                       \
                DXTraceW(__FILE__, (DWORD)__LINE__, hr, L#x, true); \
            }                                                       \
        }
#endif // End of HR macro definition
#else // Debug Not defined
#ifndef HR
#define HR(x)(x)
#endif
#endif // End of HR Debug Macro
