// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : guiHal.cpp
//   Purpose : Declares platform-specific GUI HAL functions for locking/unlocking the screen, printing text, initializing/shutting down the HAL, filling rectangles, drawing frames, horizontal/vertical bars, and printing icons
// Component : Babel GUI
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Globals

TBGuiHALInfo		bGuiHalInfo;

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bGuiLockScreen
	Purpose : lock GUI screen
	Parameters : 
	Returns : 
	Info : 
*/
void bGuiLockScreen()
{
	int32 locked = bGuiHalInfo.locked;

	if (locked > 0) return;

	bGuiHalInfo.locked++;
	bGuiHalInfo.savedProjectionMode = bViewInfo.projectionMode;

	bdSetProjectionMode(0, 0);
	bdSetObjectMatrix(bIdentityMatrix);
	bdSetTexture(0, NULL);

	bGuiHalInfo.savedZTest = bRenderState.renderState[BDRENDERSTATE_ZTEST][0];
	bdSetRenderState(BDRENDERSTATE_ZTEST, 1, 0);

	bGuiHalInfo.savedTextureFilter = bRenderState.renderState[BDRENDERSTATE_TEXTUREFILTER][0];
	bdSetRenderState(BDRENDERSTATE_TEXTUREFILTER, 2, 0);

	bGuiHalInfo.savedAlphaEnable = bRenderState.renderState[BDRENDERSTATE_ALPHAENABLE][0];
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, 1, 0);

	bGuiHalInfo.savedAlphaBlendMode = bRenderState.renderState[BDRENDERSTATE_ALPHABLENDMODE][0];
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, 4, 0);

	if (bDisplayInfo.inScene == 0) {
		int32 hr = bDisplayInfo.d3dDevice->BeginScene();
		if (hr < 0) {
			bkPrintf("bGuiLockScreen: BeginScene() FAILED\n");
			return;
		}
		bDisplayInfo.inScene = 1;
	}
}

/*	--------------------------------------------------------------------------------
	Function : bGuiUnlockScreen
	Purpose : unlock GUI screen
	Parameters : 
	Returns : 
	Info : 
*/
void bGuiUnlockScreen()
{
	if (bDisplayInfo.inScene != 0) {
		int32 hr = bDisplayInfo.d3dDevice->EndScene();
		if (hr < 0) {
			bkPrintf("bGuiUnlockScreen: EndScene() FAILED\n");
		}
		bDisplayInfo.inScene = 0;
	}

	if (bGuiHalInfo.locked > 0) {
		bGuiHalInfo.locked--;

		bdSetRenderState(BDRENDERSTATE_ZTEST, bGuiHalInfo.savedZTest, 0);
		bdSetRenderState(BDRENDERSTATE_TEXTUREFILTER, bGuiHalInfo.savedTextureFilter, 0);
		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, bGuiHalInfo.savedAlphaEnable, 0);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, bGuiHalInfo.savedAlphaBlendMode, 0);

		bdSetProjectionMode(bGuiHalInfo.savedProjectionMode, 0);
	}
}

/*	--------------------------------------------------------------------------------
	Function : bGuiPrintText
	Purpose : print GUI text
	Parameters : font, xPos, yPos, text
	Returns : 
	Info : 
*/
void bGuiPrintText(struct _TBGuiFont *font, int xPos, int yPos, char *text)
{
	int32 dispW, dispH;

	if (font == (TBGuiFont*)0)
		font = &bDefGuiFont;

	bdGetDisplayInfo(&dispW, &dispH, (int32*)0, (int32*)0);

	float baseX = (float)(xPos - (dispW >> 1));
	const float baseY = (float)((dispH >> 1) - yPos);

	const uint32 fore =
		bRColLUT[bGuiInfo.textFore[0]] |
		bGColLUT[bGuiInfo.textFore[1]] |
		bBColLUT[bGuiInfo.textFore[2]] |
		bAColLUT[0xff];

	const uint32 back =
		bRColLUT[bGuiInfo.textBack[0]] |
		bGColLUT[bGuiInfo.textBack[1]] |
		bBColLUT[bGuiInfo.textBack[2]] |
		bAColLUT[0xff];

	for (; *text != '\0'; ++text)
	{
		const int32 c = (int32)(signed char)(*text);
		const int32 glyphW = *(int32*)(((char*)font) + c * 8 - 0xFC);
		char *glyph = *(char**)(((char*)font) + c * 8 - 0xF8);

		for (int32 yy = 0; yy < font->height; ++yy)
		{
			for (int32 xx = 0; xx < glyphW; ++xx)
			{
				const char px = *glyph++;
				uint32 col;

				if (px == '1')
				{
					col = fore;
				}
				else
				{
					if (bGuiInfo.fillMode != BGUIFILLMODE_OPAQUE)
						continue;
					col = back;
				}

				TBPrimVertex v;
				v.x = baseX + (float)xx;
				v.y = baseY - (float)yy;
				v.z = 0.0f;
				v.colour = col;

				bdDrawPrimitive(BDPRIMTYPE_POINTLIST, &v, 1, BVERTTYPE_SINGLE);
			}
		}

		baseX += (float)glyphW;
	}
}

/*	--------------------------------------------------------------------------------
	Function : bInitGuiHAL
	Purpose : initialise GUI HAL
	Parameters : 
	Returns : 
	Info : 
*/
void bInitGuiHAL()
{
	int32 padId;
	int32 attempt;
	uint32 padPrefix;

	bkPrintf("bInitGuiHAL\n");

	// Keyboard bindings
	biBindInput(bGuiInfo.chLeft, BIINPUT_KEY_LEFT, 0);
	biBindInput(bGuiInfo.chRight, BIINPUT_KEY_RIGHT, 0);
	biBindInput(bGuiInfo.chUp, BIINPUT_KEY_UP, 0);
	biBindInput(bGuiInfo.chDown, BIINPUT_KEY_DOWN, 0);
	biBindInput(bGuiInfo.chSelect, BIINPUT_KEY_RETURN, 0);
	biBindInput(bGuiInfo.chDeselect, BIINPUT_KEY_ESCAPE, 0);
	biBindMulti(bGuiInfo.chPrior, 2, BIINPUT_KEY_TAB, BIINPUT_KEY_LSHIFT);
	biBindInput(bGuiInfo.chNext, BIINPUT_KEY_TAB, 0);
	biBindInput(bGuiInfo.chExit, BIINPUT_KEY_F10, 0);

	padId = biFindPad(0);
	for (attempt = 0; (padId == -1) && (attempt < 10); attempt++) {
		bkPrintf("bInitGuiHAL: Test pad %d\n", attempt);
		biReadDevices();
		padId = biFindPad(0);
	}

	attempt = biFindPad(0);
	if (attempt != -1) {
		padPrefix = BIINPUT_DEV(attempt);

		biBindInput(bGuiInfo.chLeft, padPrefix | BIINPUT_PS_LEFT, 0);
		biBindInput(bGuiInfo.chRight, padPrefix | BIINPUT_PS_RIGHT, 0);
		biBindInput(bGuiInfo.chUp, padPrefix | BIINPUT_PS_UP, 0);
		biBindInput(bGuiInfo.chDown, padPrefix | BIINPUT_PS_DOWN, 0);
		biBindInput(bGuiInfo.chSelect, padPrefix | BIINPUT_PS_CROSS, 0);
		biBindInput(bGuiInfo.chDeselect, padPrefix | BIINPUT_PS_TRIANGLE, 0);
		biBindInput(bGuiInfo.chPrior, padPrefix | BIINPUT_PS_L1, 0);
		biBindInput(bGuiInfo.chNext, padPrefix | BIINPUT_PS_R1, 0);
		biBindInput(bGuiInfo.chExit, padPrefix | BIINPUT_PS_START, 0);
		biBindInput(bGuiInfo.chExit, padPrefix | BIINPUT_PS_SELECT, 0);
	}

	memset(&bGuiHalInfo, 0, sizeof(TBGuiHALInfo));
}
/*	--------------------------------------------------------------------------------
	Function : bShutdownGuiHAL
	Purpose : shutdown GUI HAL
	Parameters : 
	Returns : 
	Info : 
*/
void bShutdownGuiHAL()
{
    return; // empty, confirmed
}

/*	--------------------------------------------------------------------------------
	Function : bGuiFillRect
	Purpose : fill GUI rectangle
	Parameters : xPos, yPos, width, height, r, g, b
	Returns : 
	Info : 
*/
void bGuiFillRect(int xPos, int yPos, int width, int height, int r, int g, int b)
{
    int32 dispW, dispH;

    bdGetDisplayInfo(&dispW, &dispH, (int32*)0, (int32*)0);

    const float x0 = (float)(xPos - (dispW >> 1));
    const float y0 = (float)((dispH >> 1) - yPos);

    const float w = (float)(width - 1);
    const float h = (float)(height - 1);

    const uint32 col = bRColLUT[r] | bGColLUT[g] | bBColLUT[b] | bAColLUT[0xff];

    TBPrimVertex v[4];

    // top-left
    v[0].x = x0;      v[0].y = y0;      v[0].z = 0.0f;
    // top-right
    v[1].x = x0 + w;  v[1].y = y0;      v[1].z = 0.0f;
    // bottom-left (NOTE: swapped)
    v[2].x = x0;      v[2].y = y0 - h;  v[2].z = 0.0f;
    // bottom-right (NOTE: swapped)
    v[3].x = x0 + w;  v[3].y = y0 - h;  v[3].z = 0.0f;

    for (int32 i = 0; i < 4; ++i)
    {
        v[i].xn = 0.0f; v[i].yn = 0.0f; v[i].zn = 0.0f;
        v[i].colour = col;
        v[i].u = 0.0f;  v[i].v = 0.0f;
    }

    bdDrawPrimitive(BDPRIMTYPE_QUADLIST, v, 4, BVERTTYPE_SINGLE);
}

/*	--------------------------------------------------------------------------------
	Function : bGuiDrawFrame
	Purpose : draw GUI frame
	Parameters : xPos, yPos, width, height, style, r, g, b
	Returns : 
	Info : 
*/
void bGuiDrawFrame(int xPos, int yPos, int width, int height, enum EBGuiFrameStyle style, int r, int g, int b)
{
	if (style == BGUIFRAME_RAISEDX2)
	{
		bGuiDrawFrame(xPos, yPos, width, height, BGUIFRAME_RAISED, r, g, b);

		if (width > 2 && height > 2)
			bGuiDrawFrame(xPos + 1, yPos + 1, width - 2, height - 2, BGUIFRAME_RAISED, r, g, b);
		return;
	}

	if (style == BGUIFRAME_SUNKENX2)
	{
		bGuiDrawFrame(xPos, yPos, width, height, BGUIFRAME_SUNKEN, r, g, b);

		if (width > 2 && height > 2)
			bGuiDrawFrame(xPos + 1, yPos + 1, width - 2, height - 2, BGUIFRAME_SUNKEN, r, g, b);
		return;
	}

	int32 dispW, dispH;
	bdGetDisplayInfo(&dispW, &dispH, (int32*)0, (int32*)0);

	const float x0 = (float)(xPos - (dispW >> 1));
	const float y0 = (float)((dispH >> 1) - yPos);

	const float w = (float)(width - 1);
	const float h = (float)(height - 1);

	if ((int32)style < 0)
		return;

	if ((int32)style < 2)
	{
		int rr1, gg1, bb1;
		int rr2, gg2, bb2;

		const int32 d1 = (-(uint32)(style != BGUIFRAME_RAISED) & 0xffffff80) + 0x40;
		const int32 d2 = (-(uint32)(style != BGUIFRAME_RAISED) & 0x00000080) - 0x40;

		bGuiDeltaRGB(r, g, b, &rr1, &gg1, &bb1, d1);
		bGuiDeltaRGB(r, g, b, &rr2, &gg2, &bb2, d2);

		const uint32 col1 = bRColLUT[rr1] | bGColLUT[gg1] | bBColLUT[bb1] | bAColLUT[0xff];
		const uint32 col2 = bRColLUT[rr2] | bGColLUT[gg2] | bBColLUT[bb2] | bAColLUT[0xff];

		// strip 1: bottom-left -> top-left -> top-right
		{
			TBPrimVertex v[3];

			v[0].x = x0;      v[0].y = y0 - h;  v[0].z = 0.0f;
			v[1].x = x0;      v[1].y = y0;      v[1].z = 0.0f;
			v[2].x = x0 + w;  v[2].y = y0;      v[2].z = 0.0f;

			for (int i = 0; i < 3; ++i)
			{
				v[i].xn = 0.0f; v[i].yn = 0.0f; v[i].zn = 0.0f;
				v[i].colour = col1;
				v[i].u = 0.0f;  v[i].v = 0.0f;
			}

			bdDrawPrimitive(BDPRIMTYPE_LINESTRIP, v, 3, BVERTTYPE_SINGLE);
		}

		// strip 2: top-right -> bottom-right -> bottom-left
		{
			TBPrimVertex v[3];

			v[0].x = x0 + w;  v[0].y = y0;      v[0].z = 0.0f;
			v[1].x = x0 + w;  v[1].y = y0 - h;  v[1].z = 0.0f;
			v[2].x = x0;      v[2].y = y0 - h;  v[2].z = 0.0f;

			for (int i = 0; i < 3; ++i)
			{
				v[i].xn = 0.0f; v[i].yn = 0.0f; v[i].zn = 0.0f;
				v[i].colour = col2;
				v[i].u = 0.0f;  v[i].v = 0.0f;
			}

			bdDrawPrimitive(BDPRIMTYPE_LINESTRIP, v, 3, BVERTTYPE_SINGLE);
		}

		return;
	}

	// style == BGUIFRAME_FLAT only
	if (style != BGUIFRAME_FLAT)
		return;

	{
		const uint32 col =
			bRColLUT[r] | bGColLUT[g] | bBColLUT[b] | bAColLUT[0xff];

		// matches the 4-vertex strip layout
		TBPrimVertex v[4];

		// top-left -> top-right -> bottom-right -> bottom-left
		v[0].x = x0;      v[0].y = y0;      v[0].z = 0.0f;
		v[1].x = x0 + w;  v[1].y = y0;      v[1].z = 0.0f;
		v[2].x = x0 + w;  v[2].y = y0 - h;  v[2].z = 0.0f;
		v[3].x = x0;      v[3].y = y0 - h;  v[3].z = 0.0f;

		for (int i = 0; i < 4; ++i)
		{
			v[i].xn = 0.0f; v[i].yn = 0.0f; v[i].zn = 0.0f;
			v[i].colour = col;
			v[i].u = 0.0f;  v[i].v = 0.0f;
		}

		bdDrawPrimitive(BDPRIMTYPE_LINESTRIP, v, 4, BVERTTYPE_SINGLE);
	}
}

/*	--------------------------------------------------------------------------------
	Function : bGuiDrawHBar
	Purpose : draw GUI horizontal bar
	Parameters : xPos, yPos, width, r, g, b
	Returns : 
	Info : 
*/
void bGuiDrawHBar(int xPos, int yPos, int width, int r, int g, int b)
{
    return; // empty, confirmed
}

/*	--------------------------------------------------------------------------------
	Function : bGuiDrawVBar
	Purpose : draw GUI vertical bar
	Parameters : xPos, yPos, height, r, g, b
	Returns : 
	Info : 
*/
void bGuiDrawVBar(int xPos, int yPos, int height, int r, int g, int b)
{
        bkPrintf("*** WARNING *** bGuiDrawVBar was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bGuiPrintIcon
	Purpose : print GUI icon
	Parameters : icon, xPos, yPos
	Returns : 
	Info : 
*/
void bGuiPrintIcon(enum EBGuiIcons icon, int xPos, int yPos)
{
	int32 dispW;
	int32 dispH;
	float baseX;
	float baseY;
	const char* pIcon;
	int32 ix;
	int32 iy;
	uint32 col;
	TBPrimVertex v;

	bdGetDisplayInfo(&dispW, &dispH, (int*)0, (int*)0);

	baseX = (float)(xPos - (dispW >> 1));
	baseY = (float)((dispH >> 1) - yPos);

	pIcon = bGuiIcons[icon];

	for (iy = 0; iy < 12; iy++) {
		for (ix = 0; ix < 8; ix++) {
			char c = *pIcon++;

			if (c == '1') {
				col = bBColLUT[bGuiInfo.textFore[2]] |
				      bGColLUT[bGuiInfo.textFore[1]] |
				      bRColLUT[bGuiInfo.textFore[0]] |
				      bAColLUT[0xff];
			} else if (bGuiInfo.fillMode == BGUIFILLMODE_OPAQUE) {
				col = bBColLUT[bGuiInfo.textBack[2]] |
				      bGColLUT[bGuiInfo.textBack[1]] |
				      bRColLUT[bGuiInfo.textBack[0]] |
				      bAColLUT[0xff];
			} else {
				continue;
			}

			v.x = (float)ix + baseX;
			v.y = baseY - (float)iy;
			v.z = 0.0f;
			v.colour = col;

			bdDrawPrimitive(0, &v, 1, 0x152);
		}
	}
}
