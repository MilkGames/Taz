// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcxbShaderSelector.cpp
//   Purpose : platform specific shader selector functions
// Component : Xbox Actor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>
#include <d3dx8.h>

// ********************************************************************************
// Locals

BDEFINE_SHADERFRAGMENTEX(CBShaderFragment_Raw2,            3, 0, "%0 %1, %2\n");};
BDEFINE_SHADERFRAGMENTEX(CBShaderFragment_Raw3,            4, 0, "%0 %1, %2, %3\n");};
BDEFINE_SHADERFRAGMENTEX(CBShaderFragment_Raw4,            5, 0, "%0 %1, %2, %3, %4\n");};
BDEFINE_SHADERFRAGMENTEX(CBShaderFragment_LoadA0,          1, 0, "mov a0.x, %0\n");};

BDEFINE_SHADERFRAGMENTEX(CBShaderFragment_Blend,           4, 0, "mul %0, %0, %1\n"
															   "mad %0, %2, %3, %0\n");};

BDEFINE_SHADERFRAGMENTEX(CBShaderFragment_BoneVertex,      1, 0, "dp4 %0.x, v0, c[a0.x + 0]\n"
															   "dp4 %0.y, v0, c[a0.x + 1]\n"
															   "dp4 %0.z, v0, c[a0.x + 2]\n"
															   "mov %0.w, v0.w\n");};

BDEFINE_SHADERFRAGMENTEX(CBShaderFragment_BoneNormal,      1, 0, "dp3 %0.x, v3, c[a0.x + 0]\n"
															   "dp3 %0.y, v3, c[a0.x + 1]\n"
															   "dp3 %0.z, v3, c[a0.x + 2]\n"
															   "mov %0.w, v0.w\n");};

BDEFINE_SHADERFRAGMENTEX(CBShaderFragment_CartoonShade,    0, 1, "dp3 r11.w, %c0, r2\n"
															   "max oT1.x, v5.z, r11.w\n"
															   "mov oT1.y, %c0.w\n");
	void RegisterData(const struct _TBShaderContext * const context);
    void ActivateFragment(const struct _TBShaderContext * const context);
    void DeactivateFragment(const struct _TBShaderContext * const context);
};

BDEFINE_SHADERFRAGMENTEX(CBShaderFragment_Outline,         0, 2, "mov oD0, %c0\n"
															   "mad r0.xyz, r2, %c1.w, r0\n");
	uint32 savedBackfaceCull;

	void RegisterData(const struct _TBShaderContext * const context);
    void ActivateFragment(const struct _TBShaderContext * const context);
    void DeactivateFragment(const struct _TBShaderContext * const context);
};

BDEFINE_SHADERFRAGMENTEX(CBShaderFragment_Shell,           0, 2, "dp3 r11.w, r2, %c1\n"
															   "mov oD0, %c0\n"
															   "mul oD0.w, %c0.w, r11.w\n"
															   "mad r0.xyz, r2, %c1.w, r0\n");
	void RegisterData(const struct _TBShaderContext * const context);
    void ActivateFragment(const struct _TBShaderContext * const context);
    void DeactivateFragment(const struct _TBShaderContext * const context);
};

BDEFINE_SHADERFRAGMENT(CBShaderFragment_ZOnly,           0, 1, "mov oD0, %c0\n");

BDEFINE_SHADERFRAGMENTEX(CBShaderFragment_EnvironmentMap,  0, 6, "dp4 r10.x, v0, c92\n"
															   "dp4 r10.y, v0, c93\n"
															   "dp4 r10.z, v0, c94\n"
															   "dp3 r10.w, r10, r10\n"
															   "rsq r10.w, r10.w\n"
															   "mul r11, r10, r10.w\n"
															   "dp3 r10.x, v3, %c3\n"
															   "dp3 r10.y, v3, %c4\n"
															   "dp3 r10.z, v3, %c5\n"
															   "dp3 r10.w, r10, r10\n"
															   "rsq r10.w, r10.w\n"
															   "mul r10, r10, r10.w\n"
															   "dp3 r9, r10, r11\n"
															   "mul r9, r9, r9\n"
															   "mul r10, r10, r9\n"
															   "mul r11, r11, %c0.xy\n"
															   "mul r10, r10, %c0.xy\n"
															   "add r11, r11, r10\n"
															   "add oT1.xy, r11, %c0.x\n");
	void RegisterData(const struct _TBShaderContext * const context);
    void ActivateFragment(const struct _TBShaderContext * const context);
    void DeactivateFragment(const struct _TBShaderContext * const context);
};

BDEFINE_SHADERFRAGMENT(CBShaderFragment_AmbientLight,    0, 1, "mov r3, %c0\n");

BDEFINE_SHADERFRAGMENT(CBShaderFragment_DirectionalLight,0, 2, "dp3 r11.w, %c0, r2\n"
															   "max r11.w, %c1.w, r11.w\n"
															   "mad r3.xyz, %c1.xyz, r11.www, r3.xyz\n");

BDEFINE_SHADERFRAGMENT(CBShaderFragment_PointLight,      0, 3, "sub r11, r0, %c0\n"
															   "dp3 r11.w, r11, r11\n"
															   "rcp r11.w, r11.w\n"
															   "dp3 r10.w, r11, -r2\n"
															   "max r10.w, v5.z, r10.w\n"
															   "mul r10.z, r11.w, r10.w\n"
															   "mul r10.z, r10.z, %c2.x\n"
															   "mad r3.xyz, %c1, r10.z, r3\n");

BDEFINE_SHADERFRAGMENT(CBShaderFragment_SpotLight,       0, 4, "sub r11, r0, %c0\n"
															   "dp3 r11.w, r11, r11\n"
															   "rsq r9.w, r11.w\n"
															   "mul r11.xyz, r11.xyz, r9.w\n"
															   "dp3 r10.w, %c1, r11\n"
															   "sub r10.w, r10.w, %c3.x\n"
															   "mul r10.w, r10.w, %c3.z\n"
															   "max r10.w, r10.w, %c3.w\n"
															   "dp3 r9.z, -r11, r2\n"
															   "max r9.z, %c3.w, r9.z\n"
															   "mul r10.w, r9.z, r10.w\n"
															   "mul r9.z, %c3.y, r9.w\n"
															   "mul r10.w, r10.w, r9.z\n"
															   "mad r3.xyz, %c2, r10.w, r3\n");

BDEFINE_SHADERFRAGMENT(CBShaderFragment_Fog,             0, 2, "sub r11, %c0, r0\n"
															   "dp3 r11.w, r11, r11\n"
															   "rsq r11.w, r11.w\n"
															   "rcp r11.w, r11.w\n"
															   "sub r11.w, %c1.x, r11.w\n"
															   "mul oFog.x, r11.w, %c1.y\n");

BDEFINE_SHADERFRAGMENTEX(CBShaderFragment_OutPosition,     1, 0, "dp4 oPos.x, %0, c[92]\n"
															   "dp4 oPos.y, %0, c[93]\n"
															   "dp4 oPos.z, %0, c[94]\n"
															   "dp4 oPos.w, %0, c[95]\n");};

BDEFINE_SHADERFRAGMENT(CBShaderFragment_OutColour,       1, 1, "mul oD0, %0, %c0\n");
BDEFINE_SHADERFRAGMENTEX(CBShaderFragment_OutTex0,         1, 0, "mov oT0.xy, %0\n");};

BDEFINE_SHADERFRAGMENTEX(CBShaderFragment_Shadow,          0, 1, "mov oD0, %c0\n");
	void RegisterData(const struct _TBShaderContext * const context);
    void ActivateFragment(const struct _TBShaderContext * const context);
    void DeactivateFragment(const struct _TBShaderContext * const context);
};

BDEFINE_SHADERFRAGMENTEX(CBShaderFragment_NormaliseNormal, 0, 0, "dp3 r11.w, r2, r2\n"
															   "rsq r11.w, r11.w\n"
															   "mul r2.xyz, r2.xyz, r11.w\n");};

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

// ----------------------
// 2/4 group
// ----------------------

void CBShaderFragment_OutColour::RegisterData(const struct _TBShaderContext * const context)
{
    TBVector outColour;

	outColour[0] = 1.0f;
	outColour[1] = 1.0f;
	outColour[2] = 1.0f;
	outColour[3] = (float)bGlobalActorAlphaScale * RECIP255;

	bDisplayInfo.d3dDevice->SetVertexShaderConstant(context->startingRegister, outColour, 1);
}

void CBShaderFragment_OutColour::ActivateFragment(const struct _TBShaderContext * const context)
{
	return; // empty, confirmed
}

void CBShaderFragment_Fog::RegisterData(const struct _TBShaderContext * const context)
{
    TBVector viewPos;
    TBVector eyeObj;
    TBVector fogParams;

    // view position in world space (w = 1)
    viewPos[0] = bViewInfo.xPos;
    viewPos[1] = bViewInfo.yPos;
    viewPos[2] = bViewInfo.zPos;
    viewPos[3] = 1.0f;

    // transform into object space using instance->worldToObject (base + 0x80)
    bmMatMultiplyVector2(eyeObj, context->instance->worldToObject, viewPos);

    // VS constant: startingRegister = eye position (object space)
    bDisplayInfo.d3dDevice->SetVertexShaderConstant(context->startingRegister, eyeObj, 1);

    // VS constant: startingRegister + 1 = fog parameters
    fogParams[0] = bRenderState.fogFar;
    fogParams[1] = bRenderState.fogRange;
    fogParams[2] = 0.0f;
    fogParams[3] = 1.0f;

    bDisplayInfo.d3dDevice->SetVertexShaderConstant(context->startingRegister + 1, fogParams, 1);
}

void CBShaderFragment_Fog::ActivateFragment(const struct _TBShaderContext * const context)
{
    return; // empty, confirmed
}

void CBShaderFragment_SpotLight::RegisterData(const struct _TBShaderContext * const context)
{
    bkPrintf("*** WARNING *** CBShaderFragment_SpotLight::RegisterData was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
}

void CBShaderFragment_SpotLight::ActivateFragment(const struct _TBShaderContext * const context)
{
    return; // empty, confirmed
}

void CBShaderFragment_PointLight::RegisterData(const struct _TBShaderContext * const context)
{
    bkPrintf("*** WARNING *** CBShaderFragment_PointLight::RegisterData was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
}

void CBShaderFragment_PointLight::ActivateFragment(const struct _TBShaderContext * const context)
{
    return; // empty, confirmed
}

void CBShaderFragment_DirectionalLight::RegisterData(const struct _TBShaderContext * const context)
{
    bkPrintf("*** WARNING *** CBShaderFragment_DirectionalLight::RegisterData was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
}

void CBShaderFragment_DirectionalLight::ActivateFragment(const struct _TBShaderContext * const context)
{
    return; // empty, confirmed
}

void CBShaderFragment_AmbientLight::RegisterData(const struct _TBShaderContext * const context)
{
    bkPrintf("*** WARNING *** CBShaderFragment_AmbientLight::RegisterData was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
}

void CBShaderFragment_AmbientLight::ActivateFragment(const struct _TBShaderContext * const context)
{
    return; // empty, confirmed
}

void CBShaderFragment_ZOnly::RegisterData(const struct _TBShaderContext * const context)
{
    TBVector rgba;

    rgba[0] = 0.0f;
    rgba[1] = 0.0f;
    rgba[2] = 0.0f;
    rgba[3] = RECIP255;

    bDisplayInfo.d3dDevice->SetVertexShaderConstant(context->startingRegister, rgba, 1);
}

void CBShaderFragment_ZOnly::ActivateFragment(const struct _TBShaderContext * const context)
{
    return; // empty, confirmed
}

// ----------------------
// 1/4 group
// ----------------------

void CBShaderFragment_Shadow::RegisterData(const struct _TBShaderContext * const context)
{
    bkPrintf("*** WARNING *** CBShaderFragment_Shadow::RegisterData was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
}

void CBShaderFragment_Shadow::ActivateFragment(const struct _TBShaderContext * const context)
{
    bkPrintf("*** WARNING *** CBShaderFragment_Shadow::ActivateFragment was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
}

void CBShaderFragment_Shadow::DeactivateFragment(const struct _TBShaderContext * const context)
{
    bkPrintf("*** WARNING *** CBShaderFragment_Shadow::DeactivateFragment was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
}

void CBShaderFragment_EnvironmentMap::RegisterData(const struct _TBShaderContext * const context)
{
    bkPrintf("*** WARNING *** CBShaderFragment_EnvironmentMap::RegisterData was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
}

void CBShaderFragment_EnvironmentMap::ActivateFragment(const struct _TBShaderContext * const context)
{
    bkPrintf("*** WARNING *** CBShaderFragment_EnvironmentMap::ActivateFragment was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
}

void CBShaderFragment_EnvironmentMap::DeactivateFragment(const struct _TBShaderContext * const context)
{
    bkPrintf("*** WARNING *** CBShaderFragment_EnvironmentMap::DeactivateFragment was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
}

void CBShaderFragment_Shell::RegisterData(const struct _TBShaderContext * const context)
{
    bkPrintf("*** WARNING *** CBShaderFragment_Shell::RegisterData was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
}

void CBShaderFragment_Shell::ActivateFragment(const struct _TBShaderContext * const context)
{
    bkPrintf("*** WARNING *** CBShaderFragment_Shell::ActivateFragment was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
}

void CBShaderFragment_Shell::DeactivateFragment(const struct _TBShaderContext * const context)
{
    bkPrintf("*** WARNING *** CBShaderFragment_Shell::DeactivateFragment was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
}

void CBShaderFragment_Outline::RegisterData(const struct _TBShaderContext * const context)
{
    const TBActRenderMode_Outlined * const parms =
        (const TBActRenderMode_Outlined *)context->instance->renderParms[context->renderStage];

    TBVector rgba;
    rgba[0] = (float)parms->r * RECIP255;
    rgba[1] = (float)parms->g * RECIP255;
    rgba[2] = (float)parms->b * RECIP255;
    rgba[3] = ((float)bGlobalActorAlphaScale * RECIP255) * ((float)parms->a * RECIP255);

    bDisplayInfo.d3dDevice->SetVertexShaderConstant(context->startingRegister, rgba, 1);

    TBVector thicknessReg;
    thicknessReg[0] = 0.0f;
    thicknessReg[1] = 0.0f;
    thicknessReg[2] = 0.0f;
    thicknessReg[3] = parms->thickness;

    bDisplayInfo.d3dDevice->SetVertexShaderConstant(context->startingRegister + 1, thicknessReg, 1);
}

void CBShaderFragment_Outline::ActivateFragment(const struct _TBShaderContext * const context)
{
    (void)context;
    savedBackfaceCull = bRenderState.renderState[BDRENDERSTATE_BACKFACECULL][0];
    bdSetRenderState(BDRENDERSTATE_BACKFACECULL, 2, 0);
}

void CBShaderFragment_Outline::DeactivateFragment(const struct _TBShaderContext * const context)
{
    (void)context;
    bdSetRenderState(BDRENDERSTATE_BACKFACECULL, savedBackfaceCull, 0);
}

void CBShaderFragment_CartoonShade::RegisterData(const struct _TBShaderContext * const context)
{
    TBActRenderMode_CartoonLit* parms =
        (TBActRenderMode_CartoonLit*)context->instance->renderParms[context->renderStage];

    TBVector v;
    TBVector origin;

    // load directional unit vector from light source
    v[0] = parms->lightsource->directional.unitDirection[0];
    v[1] = parms->lightsource->directional.unitDirection[1];
    v[2] = parms->lightsource->directional.unitDirection[2];
    v[3] = 1.0f;

    // origin point (0,0,0,1)
    origin[0] = 0.0f;
    origin[1] = 0.0f;
    origin[2] = 0.0f;
    origin[3] = 1.0f;

    // transform both points by worldToObject matrix
    bmMatMultiplyVector(context->instance->worldToObject, v);
    bmMatMultiplyVector(context->instance->worldToObject, origin);

    // subtract origin to remove translation component
    v[0] -= origin[0];
    v[1] -= origin[1];
    v[2] -= origin[2];
    v[3]  = 1.0f;

    // normalize
    float len = bmSqrtApprox(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    float invLen = 1.0f / len;

    v[0] *= invLen;
    v[1] *= invLen;
    v[2] *= invLen;

    // w = toon ramp v-coordinate
    if (parms->rampTexture == NULL)
    {
        v[3] = 0.0f;
    }
    else
    {
        float invY = 1.0f / (float)parms->rampTexture->yDim;
        v[3] = (float)parms->rampNumber * invY + invY * 0.5f;
    }

    // upload constant (one register, 4 floats)
    bDisplayInfo.d3dDevice->SetVertexShaderConstant(context->startingRegister, v, 1);
}

void CBShaderFragment_CartoonShade::ActivateFragment(const struct _TBShaderContext * const context)
{
    // Sampler/state setup for stage 1
    bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP);
    bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_NONE);
    bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_POINT);
    bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_POINT);

    // Texture pointer comes from renderParms[renderStage], first dword is TBTexture*
    bdSetTexture(1, *(TBTexture **)context->instance->renderParms[context->renderStage]);

    // Engine-side render state tracking
    bRenderState.textureEnable[1] = 0;
    bdLockRenderState(2);

    // Combiner: texture * current
    bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_MODULATE);
    bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
}

void CBShaderFragment_CartoonShade::DeactivateFragment(const struct _TBShaderContext * const context)
{
    (void)context;

    bdUnlockRenderState(2);

    bRenderState.textureEnable[1] = 1;
    bdSetTexture(1, NULL);

    bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP);
    bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
    bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
}

void bAssembleShader(TBShaderActorDesc* actorDesc, TBActorInstance* instance)
{
    LPD3DXBUFFER compiledBuf = NULL;
    LPD3DXBUFFER errorBuf = NULL;

    uint32 scanCount = 0xFFFFFFFF;
    const char* p = codeBuffer;
    char c;
    do {
        if (scanCount == 0) break;
        --scanCount;
        c = *p++;
    } while (c != '\0');

    HRESULT hr = D3DXAssembleShader(codeBuffer, (UINT)((~scanCount) - 1), 0, NULL, &compiledBuf, &errorBuf);
    if (hr < 0)
    {
        bkPrintf("baGenerateShader: Assembly failed: '%s':\n", DXGetErrorString8A(hr));
        bkPrintf(codeBuffer);

        bkPrintf((char*)errorBuf->GetBufferPointer());
        errorBuf->Release();
        return;
    }

    const DWORD* pFunction = (const DWORD*)compiledBuf->GetBufferPointer();
    const DWORD* pDecl = (instance->actor->flags & 1) ? _vsDeclBlendedPrimVertex : _vsDeclPrimVertex;

    int usageTmp = 0;
    if (bDisplayInfo.hwVertexShaders != 0)
        usageTmp = 1;
    usageTmp = (usageTmp - 1) & 0x10;

    hr = bDisplayInfo.d3dDevice->CreateVertexShader(
        pDecl,
        pFunction,
        (DWORD*)&actorDesc->type,
        (DWORD)usageTmp);

    compiledBuf->Release();
    if (errorBuf != NULL)
        errorBuf->Release();

    if (hr < 0)
    {
        bkPrintf("baGenerateShader: Vertex Creation FAILED: '%s':\n", DXGetErrorString8A(hr));
        bkPrintf(codeBuffer);
    }
}

// ********************************************************************************
// Function Implementations

CBShaderFragment::CBShaderFragment(const uint32 _pC, const uint32 constantRegsUsed, const char * const _cF)
{
    this->opcodeCount = 0;

	char        c = *_cF;
    const char *p = _cF;

    while (c != '\0')
    {
		++p;
        if (c == '\n') ++this->opcodeCount;
        c = *p;
    }

    this->codeFragment     = _cF;
    this->paramCount       = _pC;
    this->constantRegsUsed = constantRegsUsed;
}

uint32 CBShaderFragment::AppendFragment(struct _TBShaderActorDesc * const actor, char * const buffer, ...)
{
    if (paramCount > CODEFRAGMENT_MAXPARAMS)
    {
        bkPrintf("CBShaderFragment::AppendFragment: Code fragments must not need more than %d parameters\n",
                 CODEFRAGMENT_MAXPARAMS);
        return 0;
    }

    const char* args[CODEFRAGMENT_MAXPARAMS];
    for (uint32 i = 0; i < CODEFRAGMENT_MAXPARAMS; ++i)
        args[i] = NULL;

    va_list va;
    va_start(va, buffer);
    for (uint32 y = 0; y < paramCount; ++y)
        args[y] = va_arg(va, const char*);
    va_end(va);

    const uint32 startReg = actor->fragmentList.AppendFragment(actor, this);

    const char* src = codeFragment;
    const char* percent = strchr(src, '%');

    while (percent != NULL)
    {
        strncat(buffer, src, (size_t)(percent - src));

        if (percent[1] == 'c')
        {
            const int digit = (int)(unsigned char)percent[2] - '0';
            char tmp[32];
            sprintf(tmp, "c[%d]", (int)startReg + digit);
            strcat(buffer, tmp);
            src = percent + 3;
        }
        else
        {
            const int idx = (int)(unsigned char)percent[1] - '0';
            if ((uint32)idx < CODEFRAGMENT_MAXPARAMS && args[idx] != NULL)
                strcat(buffer, args[idx]);
            src = percent + 2;
        }

        percent = strchr(src, '%');
    }

    strcat(buffer, src);
    return opcodeCount;
}

/*	--------------------------------------------------------------------------------
	Function : bInitShaderSelector
	Purpose : initialise shader selector
	Parameters : 
	Returns : 
	Info : 
*/
void bInitShaderSelector(void)
{
    bInitShaderSelectorPlatformSpecific();

    bkHeapGroupPush("Shader Fragment");

    codeFragments[bcfRaw2]             = new CBShaderFragment_Raw2();
    codeFragments[bcfRaw3]             = new CBShaderFragment_Raw3();
    codeFragments[bcfRaw4]             = new CBShaderFragment_Raw4();
    codeFragments[bcfLoadA0]           = new CBShaderFragment_LoadA0();
    codeFragments[bcfBlend]            = new CBShaderFragment_Blend();
    codeFragments[bcfBoneVertex]       = new CBShaderFragment_BoneVertex();
    codeFragments[bcfBoneNormal]       = new CBShaderFragment_BoneNormal();
    codeFragments[bcfNormaliseNormal]  = new CBShaderFragment_NormaliseNormal();
	codeFragments[bcfCartoonShade]     = new CBShaderFragment_CartoonShade();
    codeFragments[bcfOutline]          = new CBShaderFragment_Outline();
    codeFragments[bcfShell]            = new CBShaderFragment_Shell();
    codeFragments[bcfZOnly]			   = new CBShaderFragment_ZOnly();
    codeFragments[bcfSphereMap]        = new CBShaderFragment_EnvironmentMap();
    codeFragments[bcfAmbientLight]	   = new CBShaderFragment_AmbientLight();
    codeFragments[bcfDirectionalLight] = new CBShaderFragment_DirectionalLight();
    codeFragments[bcfPointLight]       = new CBShaderFragment_PointLight();
    codeFragments[bcfSpotLight]        = new CBShaderFragment_SpotLight();
    codeFragments[bcfFog]              = new CBShaderFragment_Fog();
    codeFragments[bcfOutPosition]      = new CBShaderFragment_OutPosition();
    codeFragments[bcfOutColour]        = new CBShaderFragment_OutColour();
    codeFragments[bcfOutTex0]          = new CBShaderFragment_OutTex0();
    codeFragments[bcfShadow]           = new CBShaderFragment_Shadow();

    bkHeapGroupPop();
}

/*	--------------------------------------------------------------------------------
	Function : bShutdownShaderSelector
	Purpose : shutdown shader selector
	Parameters : 
	Returns : 
	Info : 
*/
void bShutdownShaderSelector(void)
{
        bkPrintf("*** WARNING *** bShutdownShaderSelector was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baShaderSelect
	Purpose : Selects the appropriate shader for the actor instance that was passed.
	Parameters : [in] the instance to evaluate. (the object->world matrix may be validated)
				[in] the render mode of that instance to evaluate.
	Returns : TRUE if there will be a point to rendering,
				FALSE if there is no effect from rendering (or rendering will produce undefined results)
	Info : 
*/
int baShaderSelect(TBActorInstance * const instance, const int modeNum, TBMesh *mesh)
{
	TBActor* actor;
	TBShaderActorDesc actorDescKey;
	TBShaderLightingDesc lightingKey;
	TBLightsource* l;
	uint32 lightingStateIndex;
	uint32 vertexShaderType;
	uint32 i;
	TBVertexBuffer* vb;
	TBShaderContext ctx;

	actor = instance->actor;

	memset(&actorDescKey, 0, 0x118);

	actorDescKey.boneCount = (int)actor->softSkin.bonesPerVertex;
	if ((actorDescKey.boneCount != 0) && (bForceHardSkinning != 0))
	{
		actorDescKey.boneCount = 1;
	}

	actorDescKey.renderMode = modeNum;
	actorDescKey.isDrawingActor = (modeNum == 0);

	switch (instance->renderMode[modeNum])
	{
	case BACTRENDERMODE_NONE:
		if (actorDescKey.isDrawingActor == 0)
		{
			return 0;
		}
		break;

	case BACTRENDERMODE_NORMAL:
		break;

	case BACTRENDERMODE_CARTOONLIT:
		if (actorDescKey.isDrawingActor == 0)
		{
			return 0;
		}
		actorDescKey.isCartoonLit = 1;
		break;

	case BACTRENDERMODE_ZONLY:
		if (actorDescKey.isDrawingActor == 0)
		{
			return 0;
		}
		actorDescKey.isZOnly = 1;
		break;

	case BACTRENDERMODE_OUTLINED:
		if (actorDescKey.isDrawingActor != 0)
		{
			return 0;
		}
		actorDescKey.isOutlined = 1;
		break;

	case BACTRENDERMODE_SHELL:
		actorDescKey.isShell = 1;
		break;

	case BACTRENDERMODE_ENVIRONMENT:
		actorDescKey.isSphereMap = 1;
		break;

	case BACTRENDERMODE_CUSTOM:
		actorDescKey.isCustom = (int)instance->renderParms[modeNum];
		break;

	case BACTRENDERMODE_SHADOW:
		actorDescKey.isShadow = 1;
		break;

	default:
		break;
	}

	memset(&lightingKey, 0, sizeof(lightingKey));
	lightingKey.fogEnabled = bRenderState.renderState[BDRENDERSTATE_DISTANCEFOG][0];
	lightingKey.lightingEnabled = bRenderState.renderState[BDRENDERSTATE_LIGHTING][0];
	lightingKey.directionalLightCount = 0;
	lightingKey.pointLightCount = 0;
	lightingKey.spotLightCount = 0;

	if (lightingKey.lightingEnabled != 0)
	{
		l = bActiveLights.next;
		while (l != &bActiveLights)
		{
			switch (l->type)
			{
			case BLIGHTTYPE_DIRECTIONAL:
				lightingKey.directionalLightCount++;
				break;
			case BLIGHTTYPE_POINT:
				lightingKey.pointLightCount++;
				break;
			case BLIGHTTYPE_SPOT:
				lightingKey.spotLightCount++;
				break;
			default:
				break;
			}

			l = l->next;
		}
	}

	if ((currentLightingState == NULL) || (memcmp(&lightingKey, currentLightingState, 0x14) != 0))
	{
		lightingStateIndex = 0;
		while (lightingStateIndex < vtxShaderLightStateCount)
		{
			if (memcmp(&lightingKey, &vtxShaderCache[lightingStateIndex], 0x14) == 0)
			{
				currentLightingState = &vtxShaderCache[lightingStateIndex];
				break;
			}
			lightingStateIndex++;
		}

		if (lightingStateIndex >= vtxShaderLightStateCount)
		{
			currentLightingState = baShaderAddLightingState(&lightingKey);
			if (currentLightingState == NULL)
			{
				bkPrintf("baShaderSelect: Evaluation of the lighting state failed\n");
				return 0;
			}
		}

		currentLightingState->lastUsed = bFlipCount;
		currentLightingState->useCount++;
	}

	currentMode = (TBShaderActorDesc*)currentLightingState->modeList;

	if (currentLightingState->modeCount > 0)
	{
		i = 0;
		while (i < currentLightingState->modeCount)
		{
			if (memcmp(&actorDescKey, currentMode, 0x10) == 0)
			{
				vertexShaderType = currentMode->type;
				goto found_mode;
			}

			currentMode = (TBShaderActorDesc*)((uchar*)currentMode + 0x118);
			i++;
		}
	}

	currentMode = baGenerateShader(currentLightingState, &actorDescKey, instance, modeNum);
	if (currentMode == NULL)
	{
		bkPrintf("baShaderSelect: baGenerateShader failed\n");
		return 0;
	}

	vertexShaderType = currentMode->type;

found_mode:
	if ((instance->flags & BACTORINSTFLAG_WTOOVALID) == 0)
	{
		bmMatInverse(instance->worldToObject, instance->objectToWorld);
		instance->flags = (ushort)(instance->flags | BACTORINSTFLAG_WTOOVALID);
	}

	directionalLightNum = 0;
	pointLightNum = 0;
	spotLightNum = 0;

	ctx.instance = instance;
	ctx.renderStage = (uint32)currentMode->renderMode;

	if (currentMode->fragmentList.fragmentCount > 0)
	{
		for (i = 0; i < currentMode->fragmentList.fragmentCount; i++)
		{
			ctx.startingRegister = currentMode->fragmentList.startingReg[i];
			currentMode->fragmentList.fragment[i]->ActivateFragment(&ctx);
			currentMode->fragmentList.fragment[i]->RegisterData(&ctx);
		}
	}

	if (mesh != NULL)
	{
		vb = (TBVertexBuffer *) mesh->vertexBuffer;
	}
	else
	{
		vb = actor->softSkin.vertexBuffer;
	}

	bSetVertexShader((int)vertexShaderType, vb);
	return 1;
}

/*	--------------------------------------------------------------------------------
	Function : baShaderDeselect
	Purpose : Deselect the shader for the actor instance at the render mode.
	Parameters : [in] the actor instance that we're playing with
				[in] the render stage that we're on
	Returns : 
	Info : 
*/
void baShaderDeselect(TBActorInstance * const instance, const int modeNum)
{
    TBShaderContext ctx;
    ctx.instance          = instance;
    ctx.renderStage       = modeNum;

    const uint32 count = currentMode->fragmentList.fragmentCount;
    for (uint32 i = 0; i < count; ++i)
    {
        ctx.startingRegister = currentMode->fragmentList.startingReg[i];
        currentMode->fragmentList.fragment[i]->DeactivateFragment(&ctx);
    }
}

/*	--------------------------------------------------------------------------------
	Function : baShaderEvaluateLightingState
	Purpose : checks the list of lighting states available and matches to the current lighting state.
				if a previous state wasn't found, create it.
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/
int baShaderEvaluateLightingState(void)
{
        bkPrintf("*** WARNING *** baShaderEvaluateLightingState was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : baShaderAddLightingState
	Purpose : Add a lighting state to the cache, and generate the shaders for the actor modes that are cached.
	Parameters : [in] the description of this mode.
	Returns : pointer to the newly cached lighting state, or NULL.
	Info : 
*/

TBShaderLightingDesc *baShaderAddLightingState(const TBShaderLightingDesc * const desc)
{
	TBShaderLightingDesc *target;
	int count;

	count = vtxShaderLightStateCount;

	if (count < vtxShaderLightStateMax)
	{
		target = &vtxShaderCache[count];
		vtxShaderLightStateCount = count + 1;
	}
	else
	{
		target = vtxShaderCache;

		if (count > 1)
		{
			TBShaderLightingDesc *cur;
			int remaining;

			cur = &vtxShaderCache[1];
			remaining = count - 1;

			while (remaining != 0)
			{
				if (cur->lastUsed < target->lastUsed)
				{
					target = cur;
				}
				else if ((cur->lastUsed == target->lastUsed) && (cur->useCount < target->useCount))
				{
					target = cur;
				}

				cur++;
				remaining--;
			}
		}
	}

	memcpy(target, desc, 0x14);

	target->modeCount = 0;
	target->useCount = 0;
	target->lastUsed = bFlipCount;

	return target;
}

/*	--------------------------------------------------------------------------------
	Function : baGenerateShader
	Purpose : Generate a shader for the lighting description for the actor description.
	Parameters : [in/out] the lighting description to place the new shader into
				[in] the actor description that helps,
				actor instance,
				mode number
	Returns : pointer to the new TBShaderActorDesc or NULL on error.
	Info : 
*/

struct _TBShaderActorDesc *baGenerateShader(TBShaderLightingDesc * const lighting,
																	const struct _TBShaderActorDesc * const actorDesc,
																	TBActorInstance * const instance, const int modeNum)
{
	if (lighting->modeCount == vtxShaderModeMax)
	{
		bkPrintf("baGenerateShader: Couldn't add new actor description because all slots are used\n");
		return NULL;
	}

	TBShaderActorDesc *outDesc = &lighting->modeList[lighting->modeCount];

	struct TBShaderActorDescWordView
	{
		int		boneCount;		// 0x00
		int		renderMode;		// 0x04
		uint32	flagsWord;		// 0x08
		int		isCustom;		// 0x0C
	};

	const TBShaderActorDescWordView *inW  = (const TBShaderActorDescWordView *)actorDesc;
	TBShaderActorDescWordView       *outW = (TBShaderActorDescWordView *)outDesc;

	outW->boneCount  = inW->boneCount;
	outW->renderMode = inW->renderMode;
	outW->flagsWord  = inW->flagsWord;
	outW->isCustom   = inW->isCustom;

	outDesc->fragmentList.fragmentCount = 0;

	{
		const uint32 flags = outW->flagsWord;

		uchar needNormal = 0;
		uchar doLights   = 0;

		if ((flags & 0x10) == 0)
		{
			if ((lighting->lightingEnabled != 0) ||
				((flags & 0x2e) != 0) ||
				(outDesc->isCustom != 0) ||
				((flags & 0x40) != 0))
			{
				needNormal = 1;
			}

			if ((bRenderState.renderState[BDRENDERSTATE_LIGHTING][0] != 0) &&
				((flags & 0x44) == 0))
			{
				doLights = 1;
			}
		}

		{
			const char *hdr = "vs.1.1\n";
			int i = 0;
			char c;
			do
			{
				c = hdr[i];
				codeBuffer[i] = c;
				++i;
			} while (c != '\0');
		}

		{
			const int boneCount = outDesc->boneCount;

			if (boneCount == 0)
			{
				codeFragments[bcfRaw2]->AppendFragment(outDesc, codeBuffer, "mov", "r0", "v0");

				if (needNormal)
					codeFragments[bcfRaw2]->AppendFragment(outDesc, codeBuffer, "mov", "r2", "v3");
			}
			else
			{
				codeFragments[bcfLoadA0]->AppendFragment(outDesc, codeBuffer, "v2.x");
				codeFragments[bcfBoneVertex]->AppendFragment(outDesc, codeBuffer, "r0");

				if (needNormal)
					codeFragments[bcfBoneNormal]->AppendFragment(outDesc, codeBuffer, "r2");

				if (boneCount > 1)
				{
					codeFragments[bcfLoadA0]->AppendFragment(outDesc, codeBuffer, "v2.y");
					codeFragments[bcfBoneVertex]->AppendFragment(outDesc, codeBuffer, "r1");
					codeFragments[bcfBlend]->AppendFragment(outDesc, codeBuffer, "r0", "v1.x", "r1", "v1.y");

					if (needNormal)
					{
						codeFragments[bcfBoneNormal]->AppendFragment(outDesc, codeBuffer, "r3");
						codeFragments[bcfBlend]->AppendFragment(outDesc, codeBuffer, "r2", "v1.x", "r3", "v1.y");
					}
				}

				if (boneCount > 2)
				{
					codeFragments[bcfLoadA0]->AppendFragment(outDesc, codeBuffer, "v2.z");
					codeFragments[bcfBoneVertex]->AppendFragment(outDesc, codeBuffer, "r1");
					codeFragments[bcfRaw4]->AppendFragment(outDesc, codeBuffer, "mad", "r0", "r1", "v1.z", "r0");

					if (needNormal)
					{
						codeFragments[bcfBoneNormal]->AppendFragment(outDesc, codeBuffer, "r3");
						codeFragments[bcfRaw4]->AppendFragment(outDesc, codeBuffer, "mad", "r2", "r3", "v1.z", "r2");
					}
				}

				if (needNormal && boneCount > 0)
					codeFragments[bcfNormaliseNormal]->AppendFragment(outDesc, codeBuffer);
			}
		}

		if (doLights)
		{
			codeFragments[bcfAmbientLight]->AppendFragment(outDesc, codeBuffer);

			for (TBLightsource *l = bActiveLights.next; l != &bActiveLights; l = l->next)
			{
				if (l->type == BLIGHTTYPE_DIRECTIONAL)
					codeFragments[bcfDirectionalLight]->AppendFragment(outDesc, codeBuffer);
				else if (l->type == BLIGHTTYPE_POINT)
					codeFragments[bcfPointLight]->AppendFragment(outDesc, codeBuffer);
				else if (l->type == BLIGHTTYPE_SPOT)
					codeFragments[bcfSpotLight]->AppendFragment(outDesc, codeBuffer);
			}
		}

		uchar usedCustom = 0;
		uchar skipTail   = 0;

		if ((outW->flagsWord & 0x2) != 0)
		{
			codeFragments[bcfCartoonShade]->AppendFragment(outDesc, codeBuffer);
		}
		else if ((outW->flagsWord & 0x20) != 0)
		{
			codeFragments[bcfSphereMap]->AppendFragment(outDesc, codeBuffer);
		}
		else if ((outW->flagsWord & 0x4) != 0)
		{
			skipTail = 1;
			codeFragments[bcfOutline]->AppendFragment(outDesc, codeBuffer);
		}
		else if ((outW->flagsWord & 0x8) != 0)
		{
			skipTail = 1;
			codeFragments[bcfShell]->AppendFragment(outDesc, codeBuffer);
		}
		else if ((outW->flagsWord & 0x40) != 0)
		{
			skipTail = 1;
			codeFragments[bcfShadow]->AppendFragment(outDesc, codeBuffer);
		}
		else if ((outW->flagsWord & 0x10) != 0)
		{
			skipTail = 1;
			codeFragments[bcfZOnly]->AppendFragment(outDesc, codeBuffer);
		}
		else if (outDesc->isCustom != 0)
		{
			usedCustom = 1;
			skipTail   = 1;

			CBShaderFragment *customFrag = *(CBShaderFragment **)instance->renderParms[modeNum];
			customFrag->AppendFragment(outDesc, codeBuffer);
		}

		if (bRenderState.renderState[BDRENDERSTATE_DISTANCEFOG][0] != 0)
			codeFragments[bcfFog]->AppendFragment(outDesc, codeBuffer);

		codeFragments[bcfOutPosition]->AppendFragment(outDesc, codeBuffer, "r0");

		if (!usedCustom)
			codeFragments[bcfOutTex0]->AppendFragment(outDesc, codeBuffer, "v5");

		if (!skipTail)
		{
			if (doLights)
			{
				codeFragments[bcfRaw3]->AppendFragment(outDesc, codeBuffer, "mul", "r11", "v4", "r3");
				codeFragments[bcfOutColour]->AppendFragment(outDesc, codeBuffer, "r11");
			}
			else
			{
				codeFragments[bcfOutColour]->AppendFragment(outDesc, codeBuffer, "v4");
			}
		}
	}

	bAssembleShader(outDesc, instance);
	lighting->modeCount = lighting->modeCount + 1;

	return outDesc;
}