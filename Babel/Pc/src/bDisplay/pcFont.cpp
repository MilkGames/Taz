// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcFont.cpp
//   Purpose : Defines the _TBFont struct for font resources. Declares functions for font initialization, shutdown, deletion, loading, printing (formatted and lite), extent calculation, formatted rectangle calculation, printing to vertex arrays, showing font textures, and ROM font handling
// Component : Babel Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Locals

TBVertexBuffer   *bFontVertexBuffer;
TBPrintFontGlyph  bPrintFontGlyph[1024];
TBPrintFontGlyph* glyphCursor = bPrintFontGlyph;

// ********************************************************************************
// Helper Functions

void PrintFont(TBVertexBuffer *buffer, const TBFont *font, const ushort *string, int strLen,
			   const TBFontFormatting *formatting, int r, int g, int b, int a,
			   TBPrintFontCallback callback, void *callbackContext)
{
    TBTexture     *bLastTexture = (_TBTexture*)1; // sentinel as in disasm (local_404 = 1)
    TBPrimVertex  *vertBase;
    int            bRemainingLines;              // mirrors local_408 as a counter
    TBFontLineInfo bLines[64];                   // local_400

    // Lock the vertex buffer
    vertBase = (_TBPrimVertex*)bdVertexBufferLock(buffer, 0);
    if (vertBase == (_TBPrimVertex*)0) {
        return;
    }

    // Build vertices & line layout
    bRemainingLines = bPrintFontVerts(vertBase,
                                      bLines,
                                      font,
                                      string,
                                      strLen,
                                      (_TBFontFormatting*)formatting,
                                      r,g,b,a,
                                      callback,
                                      callbackContext);

    // Unlock immediately after building the vertices
    bdVertexBufferUnlock(buffer);

    // Iterate lines and glyphs to issue draw calls (and texture binds)
    int firstVertex = 0;
    if (bRemainingLines > 0) {
        int li = 0;
        do {
            int glyphsInLine = bLines[li].noofGlyphs;
            int gi = 0;
            int byteOfs = 0; // EBX increments by 0x30 per glyph

            while (gi < glyphsInLine) {
                TBPrintFontGlyph* base = bLines[li].glyph;

                // Skip newline sentinels (charCode at +0x2C equals -2)
                int code = *(int*)((char*)base + byteOfs + 0x2C);
                if (code != -2) {
                    // Texture handle at +0x00
                    TBTexture* handle = *(TBTexture**)((char*)base + byteOfs + 0x00);
                    if (handle != bLastTexture) {
                        bdSetTexture(0, handle);
                        bLastTexture = handle;
                    }
                    bdDrawPrimitiveVB(4, buffer, firstVertex, 4);
                }

                firstVertex += 4;
                gi += 1;
                byteOfs += 0x30;
            }

            li += 1;
            bRemainingLines -= 1;
        } while (bRemainingLines != 0);
    }
}

void PrintFontLite(TBVertexBuffer *buffer, const TBFont *font, const ushort *string, int strLen,
				   float x,float y,int r,int g,int b,int a,
                   TBPrintFontCallback callback,void *callbackContext)
{
    // These Look-Up Tables for colour are assumed to be engine globals, as per the original code.
    extern uint32 bRColLUT[256], bGColLUT[256], bBColLUT[256], bAColLUT[256];

    // --- Safety checks (added for robustness, not present in original) ---
    if (!buffer || !font || !string || strLen <= 0)
    {
        return;
    }

    const float invTexW = 1.0f / (float)font->texture->xDim;
    const float invTexH = 1.0f / (float)font->texture->yDim;

    // --- Optimization (present in your version): Pack colour once, as it's the same for all vertices. ---
    const uint32 colour =
        bRColLUT[(r & 0xFF)] |
        bGColLUT[(g & 0xFF)] |
        bBColLUT[(b & 0xFF)] |
        bAColLUT[(a & 0xFF)];

    TBPrimVertex* out = (TBPrimVertex*)bdVertexBufferLock(buffer, 0);

    // --- Safety check (added for robustness) ---
    if (!out) 
    {
        return;
    }

    // Initialize the callback structure
    TBFontCallbackInfo cb; 
    cb.context     = callbackContext;
    cb.lineNumber  = 0;
    cb.glyphIndex  = 0; // Will be incremented inside the callback block
    cb.vertexBase  = out;

    const float lineH = (float)font->lineHeight;
    TBPrimVertex* v  = out;

    int i;
    for (i = 0; i < strLen; ++i)
    {
        const ushort ch = string[i];

        // Get glyph info. If width is 0, the glyph is invalid/disabled.
        // The original code falls back to glyph 0x80 in this case.
        const TBFontGlyphInfo* gi = &font->glyph[ch];  
        if (gi->width == 0)
        {
            gi = &font->glyph[0x80];
        }

        // Calculate vertex positions for the quad
        const float x0 = x;
        const float x1 = x + (float)gi->width;
        const float y0 = y - lineH; // Top of the quad
        const float y1 = y;         // Bottom of the quad (baseline)

        // Calculate texture coordinates from the atlas
        const float u0 = (float)gi->xPos * invTexW;
        const float u1 = (float)(gi->xPos + gi->width) * invTexW;
        const float v_top_atlas = (float)gi->yPos * invTexH;
        const float v_bottom_atlas = (float)(gi->yPos + gi->height) * invTexH;

        // Build the 4 vertices for the quad (TL, BL, TR, BR)
        // NOTE: The V coordinates are swapped (y0 uses v_bottom, y1 uses v_top)
		// This is often due to texture coordinate systems vs. screen coordinate systems

        // Top-Left vertex
        v[0].x = x0; v[0].y = y0; v[0].z = 0.0f; v[0].colour = colour;
        v[0].u = u0; v[0].v = v_bottom_atlas;

        // Bottom-Left vertex
        v[1].x = x0; v[1].y = y1; v[1].z = 0.0f; v[1].colour = colour;
        v[1].u = u0; v[1].v = v_top_atlas;

        // Top-Right vertex
        v[2].x = x1; v[2].y = y0; v[2].z = 0.0f; v[2].colour = colour;
        v[2].u = u1; v[2].v = v_bottom_atlas;

        // Bottom-Right vertex
        v[3].x = x1; v[3].y = y1; v[3].z = 0.0f; v[3].colour = colour;
        v[3].u = u1; v[3].v = v_top_atlas;
        
        // callback logic
        if (callback)
        {
            // Populate the remaining fields for the callback context
            cb.glyphNumber = i;       // The index of the character in the input string
            cb.curVertex   = v;       // Pointer to the 4 vertices just generated

            // Invoke the callback. glyphIndex is only incremented
            // when the callback is active
            callback(EBFONTCALLBACKREASON_POLY, &cb);
            cb.glyphIndex++; 
        }

        // Advance pen position for the next character
        x = x1;
        v += 4;
    }

    bdVertexBufferUnlock(buffer);

    // Final draw calls, as seen in the decompiled code.
    bdSetTexture(0, font->texture);
    // The value '6' likely corresponds to an enum for primitive type, e.g., TRIANGLELIST
    bdDrawPrimitiveVB(6, buffer, 0, strLen * 4);
}

// ********************************************************************************
// Function Implementations

// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : bInitFont
   Purpose : initialise the font functions
   Parameters : 
   Returns : 
   Info : called by bdDisplayOpen
*/

void bInitFont()
{
    bdVertexBufferCreate(&bFontVertexBuffer,0x1000,0x152,0x40,1);
}


/* --------------------------------------------------------------------------------
   Function : bShutdownFont
   Purpose : shutdown the font functions
   Parameters : 
   Returns : 
   Info : called by bdDisplayClose
*/

void bShutdownFont()
{
    if (bFontVertexBuffer != NULL) {
		bdVertexBufferDestroy(bFontVertexBuffer);
	}
	bFontVertexBuffer = NULL;
}


/* --------------------------------------------------------------------------------
   Function : bDeleteFont
   Purpose : delete a font resource
   Parameters : ptr to font
   Returns : 
   Info : 
*/

void bDeleteFont(TBFont *font)
{
    if (!font) return;

    bDeleteTexture(font->texture);
    bkHeapFree(font);
}


/* --------------------------------------------------------------------------------
   Function : bLoadFontByCRC
   Purpose : load a font resource from a package
   Parameters : package index, crc
   Returns : ptr to font or NULL for failure
   Info : 
*/

TBFont *bLoadFontByCRC(TBPackageIndex *pakIndex, uint32 crc)
{
    // Load raw file data for the font+texture bundle
    uchar* fileData = bkLoadFileByCRC(pakIndex, crc, (uchar*)0, (int*)0, (TBFileTagInfo*)0, 0);
    if (!fileData) {
        return (TBFont*)0;
    }

    // Allocate a TBFont object; zero-initialised
    TBFont* font = (TBFont*)CALLOCEX((uint)sizeof(TBFont), 0, (uint32)"Font");

    // Copy the on-disk TBFont header into the heap object
    memcpy((void*)font, (const void*)fileData, (size_t)sizeof(TBFont));

    // Fix up the embedded texture immediately after the TBFont header
    TBTexture* tex = bFixupTexture((void*)(fileData + sizeof(TBFont)));
    if (!tex) {
        // On failure, free the font object and exit. Blob lifetime is handled after success only.
        bkHeapFree((void*)font);
        return (TBFont*)0;
    }

    // Wire up the font to its texture and initialise the texture's intrusive list links
    font->texture = tex;
    tex->resInfo.globalPrev = &tex->resInfo;
    tex->resInfo.globalNext = &tex->resInfo;
    tex->resInfo.localPrev  = &tex->resInfo;
    tex->resInfo.localNext  = &tex->resInfo;

    // Manage the raw blob lifetime depending on package provenance
    if (!pakIndex->id.loaded) {
        // Opened-from-disk package -> free temporary blob
        bkHeapFree((void*)fileData);
    } else {
        // Loaded (RAM) package -> keep pointer for the texture loader side
        tex->loadDesc = (void*)(fileData + sizeof(TBFont));
    }

    return font;
}


/* --------------------------------------------------------------------------------
   Function : bdPrintFont
   Purpose : print a string in a particular font
   Parameters : font, 16bit string, #chars in string not including NULL, formatting, colour, callback, callback context
   Returns : 
   Info : Backslash character denotes escape sequence :
				\\			   - insert a single backslash
				\[textureName] - insert a quad using the specified texture
				\(string)	   - callback based insertion
*/

void bdPrintFont(const TBFont *font, const ushort *string, int strLen, const TBFontFormatting *formatting, int r,int g,int b, int a,
																	TBPrintFontCallback callback, void *callbackContext)
{
    PrintFont(bFontVertexBuffer,font,string,strLen,formatting,r,g,b,a,callback,callbackContext);
}


/* --------------------------------------------------------------------------------
   Function : bdPrintFontLite
   Purpose : print a string in a particular font
   Parameters : font, 16bit string, #chars in string not including NULL, position, colour, callback, callback context
   Returns : 
   Info : 
*/

void bdPrintFontLite(const TBFont *font, const ushort *string, int strLen, float x, float y, int r,int g,int b, int a,
																	TBPrintFontCallback callback, void *callbackContext)
{
    PrintFontLite(bFontVertexBuffer,font,string,strLen,x,y,r,g,b,a,callback,callbackContext);
}

/* --------------------------------------------------------------------------------
   Function : bdFontExtent
   Purpose : determine the extent of a string printed in a particular font
   Parameters : font, 16bit string, #chars in string not including NULL, ptr to receive width, ptr to receive height
				callback, callback context
   Returns : 
   Info : 
*/

void bdFontExtent(const TBFont *font, const ushort *string, int strLen, float *width, float *height, TBPrintFontCallback callback, void *callbackContext)
{
    // Local formatting with defaults per disassembly
    _TBFontFormatting fmt;
    fmt.horzFormat = EBFONTFORMATTING_LEFT;
    fmt.width      = 0.0f;
    fmt.height     = 0.0f;
    fmt.x          = 0.0f;
    fmt.y          = 0.0f;
    fmt.wrap       = 0;
    fmt.lineSpace  = 0.0f;
    fmt.vertFormat = EBFONTFORMATTING_TOP;

    // Rect as 4 contiguous floats: [minX, minY, maxX, maxY]
    float rect[4];
    bdFontFormattedRect(rect, font, string, strLen, &fmt, callback, callbackContext);

    // Return extents (width = maxX - minX, height = maxY - minY)
    *width  = rect[2] - rect[0];
    *height = rect[3] - rect[1];
}



/* --------------------------------------------------------------------------------
   Function : bdFontFormattedRect
   Purpose : determine the rectangle that would be occupied by a formatted text string
   Parameters : rect for limits, font, 16bit string, #chars in string not including NULL, formatting, callback,
				callback context
   Returns : rect valid? TRUE/FALSE
   Info : rect is [0]=minx, [1]=miny, [2]=maxx, [3]=maxy
*/

int bdFontFormattedRect(float *rect, const TBFont *font, const ushort *string, int strLen, const TBFontFormatting *formatting,
																TBPrintFontCallback callback, void *callbackContext)
{
    // Lock shared font VB
    TBPrimVertex* vbBase = (TBPrimVertex*)bdVertexBufferLock(bFontVertexBuffer, 0);
    if (!vbBase) return 0;

    // Local line info (stack 0x400 in asm -> 64 entries * 16 bytes)
    TBFontLineInfo lines[64];

    // Build glyph quads into VB; colors are zero per disasm
    int lineCount = bPrintFontVerts(
        vbBase, lines, font, string, strLen, formatting,
        0, 0, 0, 0, callback, callbackContext);            // r,g,b,a = 0

    // Initialize rect: minx/miny = +FLT_MAX, maxx/maxy = -FLT_MAX
    rect[0] =  FLT_MAX;   // minX
    rect[1] =  FLT_MAX;   // minY
    rect[2] = -FLT_MAX;   // maxX
    rect[3] = -FLT_MAX;   // maxY

    // Scan emitted vertices exactly like the asm does:
    // - 4 verts per glyph
    // - vertex stride = 0x24 bytes
    unsigned char* p = (unsigned char*)vbBase;
    unsigned char* pStart = p;

    if (lineCount > 0)
    {
        for (int li = 0; li < lineCount; ++li)
        {
            int vertsToScan = (lines[li].noofGlyphs << 2); // 4 verts per glyph
            while (vertsToScan-- > 0)
            {
                // read x,y from the first two floats of the vertex
                float vx = *(float*)(p + 0x00);
                float vy = *(float*)(p + 0x04);

                if (vx < rect[0]) rect[0] = vx;      // minX
                if (vx > rect[2]) rect[2] = vx;      // maxX
                if (vy < rect[1]) rect[1] = vy;      // minY
                if (vy > rect[3]) rect[3] = vy;      // maxY

                p += 0x24; // advance by vertex stride (36 bytes)
            }
        }
    }

    // Unlock VB
    bdVertexBufferUnlock(bFontVertexBuffer);

    // Return 1 iff any vertices were emitted
    return (p != pStart);
}

/* --------------------------------------------------------------------------------
   Function : bPrintFontVerts
   Purpose : print a string in a particular font to a vertex array
   Parameters : font, 16bit string, #chars in string not including NULL, formatting, colour, callback, callback context
   Returns : #verts written
   Info :
*/

int bPrintFontVerts(TBPrimVertex *vertBase, TBFontLineInfo *line, const TBFont *font, const ushort *string, int strLen,
					 const TBFontFormatting *formatting, int r,int g,int b, int a, TBPrintFontCallback callback,
					 void *callbackContext)
{
    if (strLen > BMAX_PRINT_CHARS) {
        strLen = BMAX_PRINT_CHARS;
        bkPrintf("bdPrintFont: *** WARNING : String length truncated to %d chars ***\n", BMAX_PRINT_CHARS);
    }

    TBFontCallbackInfo cb;
    cb.vertexBase    = vertBase;
    cb.curVertex     = vertBase;
    cb.glyphs        = bPrintFontGlyph;
    cb.glyphIndex    = 0;
    cb.glyphNumber   = 0;
    cb.lineNumber    = 0;
    cb.context       = callbackContext;
    cb.curGlyph      = 0;
    cb.escapeString  = 0;

    float invTexW = 1.0f, invTexH = 1.0f;
    if (font && font->texture) {
        invTexW = 1.0f / (float)font->texture->xDim;
        invTexH = 1.0f / (float)font->texture->yDim;
    }

    TBPrintFontGlyph* wr = bPrintFontGlyph;
    int i = 0;
    while (i < strLen && (wr - bPrintFontGlyph) < BMAX_PRINT_CHARS) {
        unsigned short ch = string[i];

        // Escape: @@ => literal '@'
        if (ch == '@' && (i + 1) < strLen && string[i + 1] == '@') {
            ch = '@'; i += 2;
        } else if (ch == '\\') {
            // Escapes: \\ => '\', \n => newline sentinel, others skip-pair
            if ((i + 1) < strLen) {
                unsigned short e = string[i + 1];
                if (e == '\\') { ch = '\\'; i += 2; }
                else if (e == 'n') {
                    wr->charCode = -2;
                    wr->width = wr->height = 0.0f;
                    wr->texture = 0;
                    wr->u[0]=wr->u[1]=wr->u[2]=wr->u[3]=0.0f;
                    wr->v[0]=wr->v[1]=wr->v[2]=wr->v[3]=0.0f;
                    ++wr; i += 2; continue;
                } else { i += 2; continue; }
            } else { ch = '\\'; i += 1; }
        } else {
            i += 1;
        }

        if (ch == 1) ch = '@';
        else if (ch == 2) ch = '\\';

        int giIndex = (int)ch - 0x20;
        if (giIndex < 0) giIndex = 0;

        const TBFontGlyphInfo* gi = &font->glyph[giIndex];
        if (gi->width == 0) gi = &font->glyph[0x80];

        wr->charCode = giIndex;
        wr->texture  = (TBTexture*)font->texture;
        wr->width    = (float)(int)gi->width;
        wr->height   = (float)font->lineHeight;

        const float u0 = (float)(int)gi->xPos * invTexW;
        const float v0 = (float)(int)gi->yPos * invTexH;
        const float u1 = u0 + (float)(int)gi->width  * invTexW;
        const float v1 = v0 + (float)(int)gi->height * invTexH;

        // Atlas corners (no half-texel offsets; retail PC path)
        // Index convention: 0=TL,1=BL,2=BR,3=TR
        wr->u[0] = u0; wr->v[0] = v0; // TL
        wr->u[1] = u0; wr->v[1] = v1; // BL
        wr->u[2] = u1; wr->v[2] = v1; // BR
        wr->u[3] = u1; wr->v[3] = v0; // TR

        ++wr;
    }

    const int glyphCount = (int)(wr - bPrintFontGlyph);
    if (glyphCount <= 0) return 0;

    // Lines (no wrapping branch + simple wrapping)
    int lineCount = 0;
    if (!formatting->wrap) {
        TBFontLineInfo* dst = line;
        dst->glyph      = bPrintFontGlyph;
        dst->noofGlyphs = 0;
        dst->width      = 0.0f;
        dst->height     = (int)font->lineHeight;
        for (int gi = 0; gi < glyphCount; ++gi) {
            if (bPrintFontGlyph[gi].charCode == -2) {
                ++lineCount; ++dst;
                dst->glyph      = &bPrintFontGlyph[gi + 1];
                dst->noofGlyphs = 0;
                dst->width      = 0.0f;
                dst->height     = (int)font->lineHeight;
            } else {
                dst->noofGlyphs += 1;
                dst->width      += bPrintFontGlyph[gi].width;
                if ((int)bPrintFontGlyph[gi].height > dst->height)
                    dst->height = (int)bPrintFontGlyph[gi].height;
            }
        }
        ++lineCount;
    } else {
        TBFontLineInfo* dst = line;
        int gi = 0;
        dst->glyph      = &bPrintFontGlyph[0];
        dst->noofGlyphs = 0;
        dst->width      = 0.0f;
        dst->height     = (int)font->lineHeight;
        lineCount = 1;

        while (gi < glyphCount) {
            if (bPrintFontGlyph[gi].charCode == -2) {
                ++gi; ++dst; ++lineCount;
                dst->glyph      = &bPrintFontGlyph[gi];
                dst->noofGlyphs = 0;
                dst->width      = 0.0f;
                dst->height     = (int)font->lineHeight;
                continue;
            }
            const float nextW = dst->width + bPrintFontGlyph[gi].width;
            if (dst->noofGlyphs > 0 && nextW > formatting->width) {
                int back = dst->noofGlyphs - 1;
                while (back >= 0 && dst->glyph[back].charCode != 0) --back;
                if (back >= 0) {
                    const int base = (int)(dst->glyph - bPrintFontGlyph);
                    gi = base + back + 1;
                    dst->width = 0.0f; dst->noofGlyphs = back;
                    for (int t = 0; t < back; ++t) dst->width += dst->glyph[t].width;
                }
                ++dst; ++lineCount;
                dst->glyph      = &bPrintFontGlyph[gi];
                dst->noofGlyphs = 0;
                dst->width      = 0.0f;
                dst->height     = (int)font->lineHeight;
                continue;
            }
            dst->noofGlyphs += 1;
            dst->width = nextW;
            if ((int)bPrintFontGlyph[gi].height > dst->height)
                dst->height = (int)bPrintFontGlyph[gi].height;
            ++gi;
        }
    }

    // Vertical block offset
    float blockH = 0.0f;
    for (int ln = 0; ln < lineCount; ++ln) blockH += (float)line[ln].height;
    blockH += (float)(lineCount - 1) * formatting->lineSpace;

    float vOff = 0.0f;
    if (formatting->vertFormat == EBFONTFORMATTING_CENTRE)
        vOff = (formatting->height - blockH) * 0.5f;
    else if (formatting->vertFormat == EBFONTFORMATTING_BOTTOM)
        vOff = formatting->height - blockH;
    // TOP -> vOff=0

    // Emit vertices (TL, BL, TR, BR) — TRIANGLESTRIP quads (DrawPrimitiveVB type 6 path)
    TBPrimVertex* out = vertBase;
    const unsigned int colourPacked = bRColLUT[(r & 255)] | bGColLUT[(g & 255)] |
                                      bBColLUT[(b & 255)] | bAColLUT[(a & 255)];
    float yBase = formatting->y - vOff;

    for (int ln2 = 0; ln2 < lineCount; ++ln2) {
        TBFontLineInfo* L = &line[ln2];

        float hOff = 0.0f;
        if (formatting->horzFormat == EBFONTFORMATTING_CENTRE)
            hOff = (formatting->width - L->width) * 0.5f;
        else if (formatting->horzFormat == EBFONTFORMATTING_RIGHT)
            hOff = (formatting->width - L->width);
        else if (formatting->horzFormat == EBFONTFORMATTING_JUSTIFY) {
            int spaces = 0;
            for (int k = 0; k < L->noofGlyphs; ++k) if (L->glyph[k].charCode == 0) ++spaces;
            if (spaces > 1) {
                const float extra = (formatting->width - L->width) / (float)spaces;
                for (int k = 0; k < L->noofGlyphs; ++k)
                    if (L->glyph[k].charCode == 0) L->glyph[k].width += extra;
                L->width = formatting->width;
            }
        }

        const float lineH    = (float)L->height;
        const float yTopLine = yBase - lineH;
        float x = formatting->x + hOff;

        cb.lineNumber = ln2;

        for (int k = 0; k < L->noofGlyphs; ++k) {
            const TBPrintFontGlyph& gph = L->glyph[k];
            if (gph.charCode == -2) continue;

            const float yTopGlyph = yTopLine + (lineH - gph.height) * 0.5f;
            const float yBotGlyph = yTopGlyph + gph.height;

            // TL
			out[0].x = x;             out[0].y = yTopGlyph; out[0].z = 0.0f;
			out[0].colour = colourPacked; out[0].u = gph.u[1]; out[0].v = gph.v[1];
			// BL
			out[1].x = x;             out[1].y = yBotGlyph; out[1].z = 0.0f;
			out[1].colour = colourPacked; out[1].u = gph.u[0]; out[1].v = gph.v[0];
			// TR
			out[2].x = x + gph.width; out[2].y = yTopGlyph; out[2].z = 0.0f;
			out[2].colour = colourPacked; out[2].u = gph.u[2]; out[2].v = gph.v[2];
			// BR
			out[3].x = x + gph.width; out[3].y = yBotGlyph; out[3].z = 0.0f;
			out[3].colour = colourPacked; out[3].u = gph.u[3]; out[3].v = gph.v[3];

            if (callback) {
                cb.curVertex   = out;
                cb.curGlyph    = (TBPrintFontGlyph*)&gph;
                cb.glyphNumber = k;
                cb.glyphIndex += 1;
                (void)callback(EBFONTCALLBACKREASON_POLY, &cb);
            }

            out += 4;
            x   += gph.width;
        }

        yBase -= (lineH + formatting->lineSpace);
    }

    return lineCount;
}

/* --------------------------------------------------------------------------------
   Function : bdShowFont
   Purpose : show the font texture
   Parameters : font, x, y, width, height
   Returns : 
   Info : 
*/

void bdShowFont(const TBFont *font,float x, float y, float w, float h)
{
        bkPrintf("*** WARNING *** bdShowFont was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}