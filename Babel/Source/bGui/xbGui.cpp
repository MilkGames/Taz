#define __BABEL__
#define _BGUI_

#include <babel.h>

int _xMul = 0;
int _yMul = 0;
TBGuiHALInfo bGuiHalInfo;

/* --------------------------------------------------------------------------------
   Function : bGuiLockScreen
   Purpose : lock the screen for gui access
   Parameters : 
   Returns : 
   Info : 
*/

void bGuiLockScreen(){
  int pitch;
  
  bGuiHalInfo.lpSurface =
       (uint *)bLockSurface((D3DSurface *)bDisplayInfo.backBuffer,(int *)0x0,(int *)0x0,(int *)0x0,
                            (int *)0x0,&pitch);
  if (bGuiHalInfo.lpSurface == (uint *)0x0) {
    bkPrintf("bGuiLockScreen: Lock failure\n");
  }
  else {
    bGuiHalInfo.lPitch = pitch >> 2;
  }
  return;
}


/* --------------------------------------------------------------------------------
   Function : bGuiUnlockScreen
   Purpose : unlock the screen for gui access
   Parameters : 
   Returns : 
   Info : 
*/

void bGuiUnlockScreen(){
  bUnlockSurface(bDisplayInfo.backBuffer);
}


/* --------------------------------------------------------------------------------
   Function : bGuiPrintText
   Purpose : print some text
   Parameters : font, xpos, ypos, text
   Returns : 
   Info : 
*/

void bGuiPrintText(struct _TBGuiFont *font, int xPos, int yPos, char *text){
    int textLen;
    D3DCOLOR foreColour, backColour;
    uint *surface;
    int pitch;
    int xOffset, yOffset;
    int drawX;
    int i, row, col;
    char ch;
    TBGuiFontChar *glyph;
    const char *pData;
    D3DCOLOR color;
    uint *dst;

    if (font == NULL) {
        font = &bDefGuiFont;
    }

    textLen = strlen(text);
    foreColour = (bGuiInfo.textFore[0] << 16) | (bGuiInfo.textFore[1] << 8) | bGuiInfo.textFore[2];
    backColour = (bGuiInfo.textBack[0] << 16) | (bGuiInfo.textBack[1] << 8) | bGuiInfo.textBack[2];

    surface = bGuiHalInfo.lpSurface;
    pitch = bGuiHalInfo.lPitch;
    
    // MG: tf is a pitch? why we don't have it in xOffset? removed it for now
    xOffset = xPos * _xMul;
    yOffset = yPos * _yMul;
    
    uint *baseDst = surface + yOffset * pitch + xOffset; // MG: easier :3
    drawX = 0;

    for (i = 0; i < textLen; ++i) {
        ch = text[i];
        if (ch < 32 || ch > 126) {
			// MG: I'm sure they skipped them, but let me replace them with placeholders so we can at least see them
            ch = '?';
        }
        
        glyph = &font->chars[ch - 32];
        pData = glyph->data;

        for (row = 0; row < font->height; ++row) {
            dst = baseDst + row * pitch + drawX; // MG: isn't it better?

            for (col = 0; col < glyph->width; ++col) {
                color = 0; // MG: transparent by default
                if (*pData == '1') {
                    color = foreColour;
                } else if (bGuiInfo.fillMode == BGUIFILLMODE_OPAQUE) {
                    color = backColour;
                }
                
                // MG: only draw if not transparent (extra precaution)
                if (color != 0 || bGuiInfo.fillMode == BGUIFILLMODE_OPAQUE) {
                    if (_xMul == 2 && _yMul == 2) {
						// MG: now that's where I had problems:
                        // MG: this assumes _yMul is handled by the caller logic (e.g. font height is pre-scaled)
                        // MG: this block seems to imply 2x2 pixels are drawn for each font pixel, which would need y-scaling here.
                        // MG: however, the original code structure suggests y-scaling is on the yPos, which is now fixed.
                        // MG: let's assume the provided glyph data corresponds to 1 final pixel.
                        dst[0] = color;
                        dst[1] = color;
                        dst += 2;
                    } else if (_xMul == 2) {
                        dst[0] = color;
                        dst[1] = color;
                        dst += 2;
                    } else {
                        dst[0] = color;
                        dst += 1;
                    }
                } else {
                     // MG: advance pointer even if not drawing
                    dst += _xMul > 1 ? _xMul : 1;
                }
                ++pData;
            }
        }
        drawX += glyph->width * _xMul;
    }
}


/* --------------------------------------------------------------------------------
   Function : bInitGuiHAL
   Purpose : initialise hardware specific GUI
   Parameters : 
   Returns : 
   Info : 
*/

void bInitGuiHAL(){
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
  _xMul = bDisplayInfo.xAAMul;
  _yMul = bDisplayInfo.yAAMul;
  return;
}


/* --------------------------------------------------------------------------------
   Function : bShutdownGuiHAL
   Purpose : shutdown hardware specific GUI
   Parameters : 
   Returns : 
   Info : 
*/

void bShutdownGuiHAL(){

}


/* --------------------------------------------------------------------------------
   Function : bGuiFillRect
   Purpose : fill a rectangle with a colour
   Parameters : pos, size, colour
   Returns : 
   Info : 
*/

void bGuiFillRect(int xPos,int yPos, int width, int height, int r, int g, int b){
  // MG: _yMul should scale yPos, not lPitch
  uint* p_base = bGuiHalInfo.lpSurface + (yPos * _yMul) * bGuiHalInfo.lPitch + (xPos * _xMul);
  uint* p_line;
  uint color = r << 16 | g << 8 | b;
  int x, y;

  width *= _xMul;
  height *= _yMul;
  
  for (y = 0; y < height; ++y) {
    p_line = p_base + y * bGuiHalInfo.lPitch;
    // MG: the original loop condition below is funny btw
    for (x = 0; x < width; ++x) {
      p_line[x] = color;
    }
  }
}


/* --------------------------------------------------------------------------------
   Function : bGuiDrawFrame
   Purpose : draw a GUI frame
   Parameters : pos, size, style, base colour
   Returns : 
   Info : 
*/

void bGuiDrawFrame(int xPos,int yPos, int width, int height, enum EBGuiFrameStyle style, int r, int g, int b){
	// MG: is it just me, or is this function overly complicated?
	// MG: I rewrote it from scratch, let's see how it turns out.
	// MG: I don't know how to work with assembler and .obj files yet, sorry, only how to delete them lmao
    int i;
    int pitch = bGuiHalInfo.lPitch;
    uint colorTop, colorBottom, flatColor;
    
    int realX = xPos * _xMul;
    int realY = yPos * _yMul;
    int realWidth = width * _xMul;
    int realHeight = height * _yMul;

    if (style == BGUIFRAME_RAISEDX2 || style == BGUIFRAME_SUNKENX2) {
        EBGuiFrameStyle innerStyle = (style == BGUIFRAME_RAISEDX2) ? BGUIFRAME_RAISED : BGUIFRAME_SUNKEN;

        bGuiDrawFrame(xPos, yPos, width, height, innerStyle, r, g, b);
        if (width > 2 && height > 2) {
            bGuiDrawFrame(xPos + 1, yPos + 1, width - 2, height - 2, innerStyle, r, g, b);
        }
        return;
    }
    
    uint *surface = bGuiHalInfo.lpSurface;
    uint *pTop = surface + realY * pitch + realX;
    uint *pBot = surface + (realY + realHeight - 1) * pitch + realX;

    if (style == BGUIFRAME_RAISED || style == BGUIFRAME_SUNKEN) {
        int brightness = (style == BGUIFRAME_RAISED) ? 0x40 : -0x40;
        int topR, topG, topB, botR, botG, botB;

        bGuiDeltaRGB(r, g, b, &topR, &topG, &topB, brightness);
        colorTop = (topR << 16) | (topG << 8) | topB;

        bGuiDeltaRGB(r, g, b, &botR, &botG, &botB, -brightness);
        colorBottom = (botR << 16) | (botG << 8) | botB;
        
        // Top and Left borders
        for (i = 0; i < realWidth; i++) pTop[i] = colorTop;
        for (i = 1; i < realHeight - 1; i++) {
             uint* pLine = pTop + i * pitch;
             pLine[0] = colorTop;
             if (_xMul > 1) pLine[1] = colorTop;
        }

        // Bottom and Right borders
        for (i = 0; i < realWidth; i++) pBot[i] = colorBottom;
        for (i = 1; i < realHeight - 1; i++) {
             uint* pLine = pTop + i * pitch;
             pLine[realWidth - 1] = colorBottom;
             if (_xMul > 1) pLine[realWidth - 2] = colorBottom;
        }

    }
    else if (style == BGUIFRAME_FLAT) {
        flatColor = (r << 16) | (g << 8) | b;
        
        // Top and Bottom borders
        for (i = 0; i < realWidth; i++) {
            pTop[i] = flatColor;
            pBot[i] = flatColor;
        }

        // Left and Right borders
        for (i = 1; i < realHeight - 1; i++) {
             uint* pLine = pTop + i * pitch;
             pLine[0] = flatColor;
             pLine[realWidth - 1] = flatColor;
             if (_xMul > 1) {
                pLine[1] = flatColor;
                pLine[realWidth-2] = flatColor;
             }
        }
    }
}



/* --------------------------------------------------------------------------------
   Function : bGuiDrawHBar
   Purpose : draw a horizontal bar
   Parameters : pos, width, base colour
   Returns : 
   Info : 
*/

void bGuiDrawHBar(int xPos,int yPos, int width, int r, int g, int b){
  int realW;
  uint *p;
  int y;
  uint shadowColour;
  uint highlightColour;
  int newR, newG, newB;
  int x;
  
  // MG: fixed???
  p = bGuiHalInfo.lpSurface + (yPos * _yMul) * bGuiHalInfo.lPitch + (xPos * _xMul);
  
  bGuiDeltaRGB(r,g,b,&newR,&newG,&newB,48);
  highlightColour = newR << 16 | newG << 8 | newB; // MG: why 0x10 before?
  
  bGuiDeltaRGB(r,g,b,&newR,&newG,&newB,-32);
  shadowColour = newR << 16 | newG << 8 | newB;
  
  realW = width * _xMul;
  
  // Draw shadow line(s) // MG: ++ LIVES
  for (y = 0; y < _yMul; y++) {
    for (x = 0; x < realW; x++) {
      p[x] = shadowColour;
    }
    p += bGuiHalInfo.lPitch; // MG: += LIVES
  }
  
  // Draw highlight line(s)
  for (y = 0; y < _yMul; y++) {
    for (x = 0; x < realW; x++) {
      p[x] = highlightColour;
    }
    p += bGuiHalInfo.lPitch;
  }
}


/* --------------------------------------------------------------------------------
   Function : bGuiDrawVBar
   Purpose : draw a vertical bar
   Parameters : pos, height, base colour
   Returns : 
   Info : 
*/

void bGuiDrawVBar(int xPos,int yPos, int height, int r, int g, int b){
  uint *p;
  uint shadowColour;
  uint highlightColour;
  int newR, newG, newB;
  int y;
  
  // MG: see bGuiDrawHBar
  p = bGuiHalInfo.lpSurface + (yPos * _yMul) * bGuiHalInfo.lPitch + (xPos * _xMul);
  
  bGuiDeltaRGB(r,g,b,&newR,&newG,&newB,0x30);
  highlightColour = newR << 16 | newG << 8 | newB;
  
  bGuiDeltaRGB(r,g,b,&newR,&newG,&newB,-0x20);
  shadowColour = newR << 16 | newG << 8 | newB;

  int realHeight = height * _yMul;
  
  for (y = 0; y < realHeight; y++) {
    p[0] = shadowColour;
    if (_xMul > 1) {
        p[1] = shadowColour;
    }
    
    p[_xMul] = highlightColour;
    if (_xMul > 1) {
        p[_xMul + 1] = highlightColour;
    }
    
    p += bGuiHalInfo.lPitch;
  }
}


/* --------------------------------------------------------------------------------
   Function : bGuiPrintIcon
   Purpose : print a GUI icon
   Parameters : icon ID, xpos, ypos
   Returns : 
   Info : 
*/

void bGuiPrintIcon(enum EBGuiIcons icon, int xPos, int yPos){
  D3DCOLOR foreColour;
  D3DCOLOR backColour;
  uint *p_line, *p_base;
  const char *iconData;
  int x, y;
  
  foreColour = bGuiInfo.textFore[0] << 16 | bGuiInfo.textFore[1] << 8 | bGuiInfo.textFore[2];
  backColour = bGuiInfo.textBack[0] << 16 | bGuiInfo.textBack[1] << 8 | bGuiInfo.textBack[2];
  
  // MG: see bGuiDrawHBar
  p_base = bGuiHalInfo.lpSurface + (yPos * _yMul) * bGuiHalInfo.lPitch + (xPos * _xMul);
  iconData = bGuiIcons[icon];
  
  const int iconWidth = 8;
  const int iconHeight = 12;

  int realHeight = iconHeight * _yMul;

  for (y = 0; y < realHeight; y++) {
      p_line = p_base + y * bGuiHalInfo.lPitch;
      const char* pDataRow = iconData + (y / _yMul) * iconWidth;

      for (x = 0; x < iconWidth; x++) {
          D3DCOLOR color = 0;
          if (pDataRow[x] == '1') {
              color = foreColour;
          } else if (bGuiInfo.fillMode == BGUIFILLMODE_OPAQUE) {
              // MG: backColour for background, not foreColour
              color = backColour;
          }

          if (color != 0 || bGuiInfo.fillMode == BGUIFILLMODE_OPAQUE) {
              if (_xMul == 2) {
                  p_line[x * 2] = color;
                  p_line[x * 2 + 1] = color;
              } else {
                  p_line[x] = color;
              }
          }
      }
  }
}