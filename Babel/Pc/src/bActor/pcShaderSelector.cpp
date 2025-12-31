// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcxbShaderSelector.cpp
//   Purpose : platform specific shader selector functions
// Component : Xbox Actor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Locals

const DWORD _vsDeclBlendedPrimVertex[] =
{
    0x20000000,
    0x40020000,
    0x40020001,
    0x40050002,
    0x40020003,
    0x40040004,
    0x40010005,
    0xFFFFFFFF
};

const DWORD _vsDeclPrimVertex[] =
{
    0x20000000,
    0x40020000,
    0x40020003,
    0x40040004,
    0x40010005,
    0xFFFFFFFF
};

// ********************************************************************************
// Local Functions

void bAssembleShader(TBShaderActorDesc* actorDesc)
{
    char c;
    uint32 scanCount;
    char* p;
    LPD3DXBUFFER compiledBuf;
    LPD3DXBUFFER errorBuf;
    HRESULT hr;

    scanCount = 0xFFFFFFFF;
    p = codeBuffer;
    do
    {
        if (scanCount == 0)
            break;
        scanCount = scanCount - 1;
        c = *p;
        p = p + 1;
    } while (c != '\0');

    hr = D3DXAssembleShader(codeBuffer, (UINT)((~scanCount) - 1), 0, NULL, &compiledBuf, &errorBuf);
    if (hr < 0)
    {
        bkPrintf("baGenerateShader: Assembly failed: '%s':\n", DXGetErrorString8A(hr));
        bkPrintf(codeBuffer);

        // No NULL-check here in retail.
        bkPrintf((char*)errorBuf->lpVtbl->GetBufferPointer(errorBuf));
        errorBuf->lpVtbl->Release(errorBuf);
        return;
    }

    const DWORD* pFunction = (const DWORD*)compiledBuf->lpVtbl->GetBufferPointer(compiledBuf);

    // Keep raw offsets from retail.
    const uint8* pDescData = *(const uint8**)((const uint8*)actorDesc + 0x130);
    const uint8 flags = *(const uint8*)(pDescData + 0x64);
    const DWORD* pDecl = (flags & 1) ? _vsDeclBlendedPrimVertex : _vsDeclPrimVertex;

    int usageTmp = 0;
    if (bDisplayInfo.hwVertexShaders != 0)
        usageTmp = 1;
    usageTmp = usageTmp - 1;
    usageTmp = usageTmp & 0x10;

    hr = bDisplayInfo.d3dDevice->CreateVertexShader(
        pDecl,
        pFunction,
        (DWORD*)((uint8*)actorDesc + 0x10),
        (DWORD)usageTmp);

    compiledBuf->lpVtbl->Release(compiledBuf);
    if (errorBuf != NULL)
        errorBuf->lpVtbl->Release(errorBuf);

    if (hr < 0)
    {
        bkPrintf("baGenerateShader: Vertex Creation FAILED: '%s':\n", DXGetErrorString8A(hr));
        bkPrintf(codeBuffer);
    }
}

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bInitShaderSelector
	Purpose : initialise shader selector
	Parameters : 
	Returns : 
	Info : 
*/
