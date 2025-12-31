#define __BABEL__
#define _BGUI_

#include <babel.h>

static inline int clampi(int v, int lo, int hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static inline unsigned PackARGB(int r, int g, int b)
{
    return (0xFFu << 24)
        | ((unsigned)clampi(r,0,255) << 16)
        | ((unsigned)clampi(g,0,255) << 8)
        |  (unsigned)clampi(b,0,255);
}

static void DeltaRGB(int r, int g, int b, int delta,
                     int* ro, int* go, int* bo)
{
    *ro = clampi(r + delta, 0, 255);
    *go = clampi(g + delta, 0, 255);
    *bo = clampi(b + delta, 0, 255);
}

static int   s_gui2DActive = 0;
static DWORD sVS = 0;
static DWORD sLighting = 0, sFog = 0, sCull = 0;
static DWORD sZEnable = 0, sZWrite = 0;
static DWORD sAlphaBlend = 0, sSrcBlend = 0, sDstBlend = 0, sAlphaTest = 0, sColorWrite = 0;
static DWORD sTSS0_ColorOp = 0, sTSS0_ColorArg1 = 0, sTSS0_ColorArg2 = 0;
static DWORD sTSS0_AlphaOp = 0, sTSS0_AlphaArg1 = 0, sTSS0_AlphaArg2 = 0;
static DWORD sTSS1_ColorOp = 0, sTSS1_AlphaOp = 0;
static IDirect3DBaseTexture8* sTex0 = 0;

static void guiBegin2D()
{
    if (s_gui2DActive) return;
    s_gui2DActive = 1;

    bDisplayInfo.d3dDevice->GetVertexShader(&sVS);
    bDisplayInfo.d3dDevice->GetTexture(0, &sTex0);

    bDisplayInfo.d3dDevice->GetRenderState(D3DRS_LIGHTING,        &sLighting);
    bDisplayInfo.d3dDevice->GetRenderState(D3DRS_FOGENABLE,       &sFog);
    bDisplayInfo.d3dDevice->GetRenderState(D3DRS_CULLMODE,        &sCull);
    bDisplayInfo.d3dDevice->GetRenderState(D3DRS_ZENABLE,         &sZEnable);
    bDisplayInfo.d3dDevice->GetRenderState(D3DRS_ZWRITEENABLE,    &sZWrite);
    bDisplayInfo.d3dDevice->GetRenderState(D3DRS_ALPHABLENDENABLE,&sAlphaBlend);
    bDisplayInfo.d3dDevice->GetRenderState(D3DRS_SRCBLEND,        &sSrcBlend);
    bDisplayInfo.d3dDevice->GetRenderState(D3DRS_DESTBLEND,       &sDstBlend);
    bDisplayInfo.d3dDevice->GetRenderState(D3DRS_ALPHATESTENABLE, &sAlphaTest);
    bDisplayInfo.d3dDevice->GetRenderState(D3DRS_COLORWRITEENABLE,&sColorWrite);

    bDisplayInfo.d3dDevice->GetTextureStageState(0, D3DTSS_COLOROP,   &sTSS0_ColorOp);
    bDisplayInfo.d3dDevice->GetTextureStageState(0, D3DTSS_COLORARG1, &sTSS0_ColorArg1);
    bDisplayInfo.d3dDevice->GetTextureStageState(0, D3DTSS_COLORARG2, &sTSS0_ColorArg2);
    bDisplayInfo.d3dDevice->GetTextureStageState(0, D3DTSS_ALPHAOP,   &sTSS0_AlphaOp);
    bDisplayInfo.d3dDevice->GetTextureStageState(0, D3DTSS_ALPHAARG1, &sTSS0_AlphaArg1);
    bDisplayInfo.d3dDevice->GetTextureStageState(0, D3DTSS_ALPHAARG2, &sTSS0_AlphaArg2);
    bDisplayInfo.d3dDevice->GetTextureStageState(1, D3DTSS_COLOROP,   &sTSS1_ColorOp);
    bDisplayInfo.d3dDevice->GetTextureStageState(1, D3DTSS_ALPHAOP,   &sTSS1_AlphaOp);

    bSetVertexShader(BVERTTYPE_SINGLE2D, NULL);
    bDisplayInfo.d3dDevice->SetTexture(0, NULL);

    bDisplayInfo.d3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    bDisplayInfo.d3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
    bDisplayInfo.d3dDevice->SetRenderState(D3DRS_CULLMODE,  D3DCULL_NONE);
    bDisplayInfo.d3dDevice->SetRenderState(D3DRS_ZENABLE,   D3DZB_FALSE);
    bDisplayInfo.d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

    bDisplayInfo.d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
    bDisplayInfo.d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
    bDisplayInfo.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
    bDisplayInfo.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
    bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
    bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

    bDisplayInfo.d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    bDisplayInfo.d3dDevice->SetRenderState(
        D3DRS_COLORWRITEENABLE,
        D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_ALPHA);
}

static void guiEnd2D()
{
    if (!s_gui2DActive) return;

    bDisplayInfo.d3dDevice->SetVertexShader(sVS);
    bDisplayInfo.d3dDevice->SetTexture(0, sTex0);
    if (sTex0) { sTex0->Release(); sTex0 = 0; }

    bDisplayInfo.d3dDevice->SetRenderState(D3DRS_LIGHTING, sLighting);
    bDisplayInfo.d3dDevice->SetRenderState(D3DRS_FOGENABLE, sFog);
    bDisplayInfo.d3dDevice->SetRenderState(D3DRS_CULLMODE,  sCull);
    bDisplayInfo.d3dDevice->SetRenderState(D3DRS_ZENABLE,   sZEnable);
    bDisplayInfo.d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, sZWrite);
    bDisplayInfo.d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, sAlphaBlend);
    bDisplayInfo.d3dDevice->SetRenderState(D3DRS_SRCBLEND,  sSrcBlend);
    bDisplayInfo.d3dDevice->SetRenderState(D3DRS_DESTBLEND, sDstBlend);
    bDisplayInfo.d3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, sAlphaTest);
    bDisplayInfo.d3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, sColorWrite);

    bDisplayInfo.d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,   sTSS0_ColorOp);
    bDisplayInfo.d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, sTSS0_ColorArg1);
    bDisplayInfo.d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, sTSS0_ColorArg2);
    bDisplayInfo.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   sTSS0_AlphaOp);
    bDisplayInfo.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, sTSS0_AlphaArg1);
    bDisplayInfo.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, sTSS0_AlphaArg2);
    bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_COLOROP,   sTSS1_ColorOp);
    bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   sTSS1_AlphaOp);

    s_gui2DActive = 0;
}

static void drawSolidRect(float x, float y, float w, float h, unsigned color)
{
    const float hp = 0.5f; // half-pixel bias
    float L = x - hp;
    float T = y - hp;
    float R = x + w - hp;
    float B = y + h - hp;

    TBPrimVertex2D v[4];

    v[0].x = L; v[0].y = T; v[0].z = 0.0f; v[0].rhw = 1.0f; v[0].colour = color;
    v[1].x = R; v[1].y = T; v[1].z = 0.0f; v[1].rhw = 1.0f; v[1].colour = color;
    v[2].x = L; v[2].y = B; v[2].z = 0.0f; v[2].rhw = 1.0f; v[2].colour = color;
    v[3].x = R; v[3].y = B; v[3].z = 0.0f; v[3].rhw = 1.0f; v[3].colour = color;

    bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, v, 4);
}

static void guiSolidRect_i(int x, int y, int w, int h, unsigned color)
{
    if (w <= 0 || h <= 0) return;
    drawSolidRect((float)x, (float)y, (float)w, (float)h, color);
}

// ---------------------------------------------------------------------
// PUBLIC API
// ---------------------------------------------------------------------

/* --------------------------------------------------------------------------------
   Function : bInitGuiHAL
   Purpose : initialise hardware specific GUI
   Parameters : 
   Returns : 
   Info : 
*/
void bInitGuiHAL(void)
{
	int input;

	for (input = 0; input < 4; input = input + 1) {
		biBindInput(bGuiInfo.chLeft,input << 16 | 0xbf,0);
		biBindInput(bGuiInfo.chRight,input << 16 | 0xc0,0);
		biBindInput(bGuiInfo.chUp,input << 16 | 0xbd,0);
		biBindInput(bGuiInfo.chDown,input << 16 | 0xbe,0);
		biBindInput(bGuiInfo.chSelect,input << 16 | 0xc5,0);
		biBindInput(bGuiInfo.chDeselect,input << 16 | 0xc6,0);
		biBindInput(bGuiInfo.chPrior,input << 16 | 0xcb,0);
		biBindInput(bGuiInfo.chNext,input << 16 | 0xcc,0);
		biBindInput(bGuiInfo.chExit,input << 16 | 0xc1,0);
	}

    bkPrintf("bInitGuiHAL: xRes=%d yRes=%d\n",
             bDisplayInfo.xRes, bDisplayInfo.yRes);

    bkPrintf("GUI colours: textFore=(%d,%d,%d) textBack=(%d,%d,%d) fillMode=%d\n",
        bGuiInfo.textFore[0], bGuiInfo.textFore[1], bGuiInfo.textFore[2],
        bGuiInfo.textBack[0], bGuiInfo.textBack[1], bGuiInfo.textBack[2],
        (int)bGuiInfo.fillMode);
}

/* --------------------------------------------------------------------------------
   Function : bShutdownGuiHAL
   Purpose : shutdown hardware specific GUI
   Parameters : 
   Returns : 
   Info : 
*/
void bShutdownGuiHAL(void)
{
    bkPrintf("bShutdownGuiHAL\n");
}

/* --------------------------------------------------------------------------------
   Function : bGuiLockScreen
   Purpose : lock the screen for gui access
   Parameters : 
   Returns : 
   Info : 
*/
void bGuiLockScreen(void)
{
    /*bkPrintf("GUI lock: xRes=%d yRes=%d backBuf=%p\n",
             bDisplayInfo.xRes, bDisplayInfo.yRes,
             (void*)bDisplayInfo.backBuffer);*/

    guiBegin2D();
}

/* --------------------------------------------------------------------------------
   Function : bGuiUnlockScreen
   Purpose : unlock the screen for gui access
   Parameters : 
   Returns : 
   Info : 
*/
void bGuiUnlockScreen(void)
{
    guiEnd2D();
}

/* --------------------------------------------------------------------------------
   Function : bGuiFillRect
   Purpose : fill a rectangle with a colour
   Parameters : pos, size, colour
   Returns : 
   Info : 
*/
void bGuiFillRect(int xPos, int yPos, int width, int height, int r, int g, int b)
{
    /*bkPrintf("FillRect: x=%d y=%d w=%d h=%d col=%02X%02X%02X\n",
             xPos, yPos, width, height, r&255, g&255, b&255);*/

    guiSolidRect_i(xPos, yPos, width, height, PackARGB(r,g,b));
}

/* --------------------------------------------------------------------------------
   Function : bGuiDrawFrame
   Purpose : draw a GUI frame
   Parameters : pos, size, style, base colour
   Returns : 
   Info : 
*/
void bGuiDrawFrame(int xPos, int yPos, int width, int height,
                   EBGuiFrameStyle style, int r, int g, int b)
{
    /*bkPrintf("DrawFrame: x=%d y=%d w=%d h=%d style=%d rgb=%02X%02X%02X\n",
             xPos, yPos, width, height, (int)style, r&255,g&255,b&255);*/

    if (width <= 0 || height <= 0) return;

    if (style == BGUIFRAME_RAISEDX2 || style == BGUIFRAME_SUNKENX2)
    {
        EBGuiFrameStyle s = (style == BGUIFRAME_RAISEDX2) ? BGUIFRAME_RAISED : BGUIFRAME_SUNKEN;
        bGuiDrawFrame(xPos, yPos, width, height, s, r,g,b);
        if (width > 2 && height > 2)
            bGuiDrawFrame(xPos+1, yPos+1, width-2, height-2, s, r,g,b);
        return;
    }

    if (style == BGUIFRAME_FLAT)
    {
        unsigned c = PackARGB(r,g,b);
        guiSolidRect_i(xPos, yPos, width, 1, c);                 // top
        guiSolidRect_i(xPos, yPos + height - 1, width, 1, c);    // bottom
        if (height > 2) guiSolidRect_i(xPos, yPos+1, 1, height-2, c);                  // left
        if (height > 2) guiSolidRect_i(xPos + width - 1, yPos+1, 1, height-2, c);      // right
        return;
    }

    // RAISED / SUNKEN
    int tr,tg,tb, br,bg,bb;
    int sign = (style == BGUIFRAME_RAISED) ? +1 : -1;
    DeltaRGB(r,g,b,  0x40 * sign, &tr,&tg,&tb);
    DeltaRGB(r,g,b, -0x40 * sign, &br,&bg,&bb);

    unsigned cTop = PackARGB(tr,tg,tb);
    unsigned cBot = PackARGB(br,bg,bb);

    guiSolidRect_i(xPos, yPos, width, 1, cTop);                   // top
    guiSolidRect_i(xPos, yPos + height - 1, width, 1, cBot);      // bottom
    if (height > 2) guiSolidRect_i(xPos, yPos+1, 1, height-2, cTop);                // left
    if (height > 2) guiSolidRect_i(xPos + width - 1, yPos+1, 1, height-2, cBot);    // right
}

/* --------------------------------------------------------------------------------
   Function : bGuiDrawHBar
   Purpose : draw a horizontal bar
   Parameters : pos, width, base colour
   Returns : 
   Info : 
*/
void bGuiDrawHBar(int xPos, int yPos, int width, int r, int g, int b)
{
    int sr,sg,sb, hr,hg,hb;
    DeltaRGB(r,g,b, -0x20, &sr,&sg,&sb);
    DeltaRGB(r,g,b,  +0x30, &hr,&hg,&hb);

    /*bkPrintf("HBar: x=%d y=%d w=%d shadow=%02X%02X%02X hi=%02X%02X%02X\n",
             xPos, yPos, width, sr&255,sg&255,sb&255, hr&255,hg&255,hb&255);*/

    bGuiFillRect(xPos, yPos, width, 1, sr,sg,sb);
    bGuiFillRect(xPos, yPos+1, width, 1, hr,hg,hb);
}

/* --------------------------------------------------------------------------------
   Function : bGuiDrawVBar
   Purpose : draw a vertical bar
   Parameters : pos, height, base colour
   Returns : 
   Info : 
*/
void bGuiDrawVBar(int xPos, int yPos, int height, int r, int g, int b)
{
    int sr,sg,sb, hr,hg,hb;
    DeltaRGB(r,g,b, -0x20, &sr,&sg,&sb);
    DeltaRGB(r,g,b,  +0x30, &hr,&hg,&hb);

    /*bkPrintf("VBar: x=%d y=%d h=%d shadow=%02X%02X%02X hi=%02X%02X%02X\n",
             xPos, yPos, height, sr&255,sg&255,sb&255, hr&255,hg&255,hb&255);*/

    bGuiFillRect(xPos,     yPos, 1, height, sr,sg,sb);
    bGuiFillRect(xPos + 1, yPos, 1, height, hr,hg,hb);
}

/* --------------------------------------------------------------------------------
   Function : bGuiPrintText
   Purpose : print some text
   Parameters : font, xpos, ypos, text
   Returns : 
   Info : 
*/
void bGuiPrintText(TBGuiFont* font, int xPos, int yPos, char* text)
{
    if (!font) font = &bDefGuiFont;
    if (!text || !*text) return;

    const int foreR = bGuiInfo.textFore[0];
    const int foreG = bGuiInfo.textFore[1];
    const int foreB = bGuiInfo.textFore[2];
    const int backR = bGuiInfo.textBack[0];
    const int backG = bGuiInfo.textBack[1];
    const int backB = bGuiInfo.textBack[2];

    /*bkPrintf("PrintText: \"%s\" at %d,%d h=%d\n",
             text, xPos, yPos, font->height);*/

    int totalW = 0;
    {
        const char* p = text;
        while (*p) {
            int ch = (unsigned char)*p++;
            if (ch < 32 || ch > 127) ch = '?';
            totalW += font->chars[ch - 32].width;
        }
    }

    if (bGuiInfo.fillMode == BGUIFILLMODE_OPAQUE) {
        bGuiFillRect(xPos, yPos, totalW, font->height, backR, backG, backB);
    }

    int penX = 0;
    const char* p = text;
    while (*p) {
        int ch = (unsigned char)*p++;
        if (ch < 32 || ch > 127) ch = '?';

        const TBGuiFontChar* glyph = &font->chars[ch - 32];
        const char* data = glyph->data;
        int gw = glyph->width;
        int gh = font->height;

        int row, col;
        for (row = 0; row < gh; ++row) {
            for (col = 0; col < gw; ++col) {
                if (data[row*gw + col] == '1') {
                    bGuiFillRect(xPos + penX + col, yPos + row, 1, 1, foreR,foreG,foreB);
                }
            }
        }
        penX += gw;
    }
}

/* --------------------------------------------------------------------------------
   Function : bGuiPrintIcon
   Purpose : print a GUI icon
   Parameters : icon ID, xpos, ypos
   Returns : 
   Info : 
*/
void bGuiPrintIcon(EBGuiIcons icon, int xPos, int yPos)
{
    const int foreR = bGuiInfo.textFore[0];
    const int foreG = bGuiInfo.textFore[1];
    const int foreB = bGuiInfo.textFore[2];
    const int backR = bGuiInfo.textBack[0];
    const int backG = bGuiInfo.textBack[1];
    const int backB = bGuiInfo.textBack[2];

    //bkPrintf("PrintIcon: id=%d at %d,%d\n", (int)icon, xPos, yPos);

    const char* mask = bGuiIcons[icon];

    if (bGuiInfo.fillMode == BGUIFILLMODE_OPAQUE) {
        bGuiFillRect(xPos, yPos, 8, 12, backR,backG,backB);
    }

    int iy, ix;
    for (iy = 0; iy < 12; ++iy) {
        for (ix = 0; ix < 8; ++ix) {
            if (mask[iy*8 + ix] == '1') {
                bGuiFillRect(xPos + ix, yPos + iy, 1, 1, foreR,foreG,foreB);
            }
        }
    }
}
