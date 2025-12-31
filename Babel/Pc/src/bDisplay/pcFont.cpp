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
	_TBPrimVertex        *vertex;
	const _TBFontGlyphInfo     *glyph;
	int                   glyphIndex;
	_TBFontCallbackInfo   callbackInfo;
	short                 glyphHeight;
	short                 glyphWidth;
	short                 glyphX;
	short                 glyphY;
	float                 invTextureHeight;
	float                 invTextureWidth;
	float                 lineHeight;
	float                 u0;
	float                 u1;
	float                 v0;
	float                 v1;

	invTextureWidth  = 1.0f / (float)font->texture->xDim;
	invTextureHeight = 1.0f / (float)font->texture->yDim;

	vertex = (_TBPrimVertex *)bdVertexBufferLock(buffer, 0);

	callbackInfo.context     = callbackContext;
	callbackInfo.lineNumber  = 0;
	callbackInfo.glyphIndex  = 0;
	glyphIndex               = 0;
	callbackInfo.vertexBase  = vertex;

	if (strLen > 0)
	{
		do
		{
			glyph = (_TBFontGlyphInfo *)((int)font + (uint)string[glyphIndex] * 8 + -0xd0);
			if (*(short *)((int)font + (uint)string[glyphIndex] * 8 + -0xcc) == 0)
			{
				glyph = font->glyph + 0x80;
			}

			glyphX      = glyph->xPos;
			glyphWidth  = glyph->width;
			glyphY      = glyph->yPos;
			glyphHeight = glyph->height;

			vertex->x = x;
			u0        = (float)(int)glyphX      * invTextureWidth;
			u1        = (float)(int)glyphWidth  * invTextureWidth + u0;
			v0        = (float)(int)glyphY      * invTextureHeight;
			v1        = (float)(int)glyphHeight * invTextureHeight + v0;

			lineHeight   = font->lineHeight;
			vertex->u    = u0;
			vertex->v    = v1;
			vertex->y    = y - lineHeight;
			vertex->z    = 0.0f;
			vertex->colour =
				  bGColLUT[g]
				| bAColLUT[a]
				| bBColLUT[b]
				| bRColLUT[r];

			vertex[1].x = x;
			vertex[1].y = y;
			vertex[1].u = u0;
			vertex[1].v = v0;
			vertex[1].z = 0.0f;
			vertex[1].colour =
				  bGColLUT[g]
				| bAColLUT[a]
				| bBColLUT[b]
				| bRColLUT[r];

			vertex[2].x = (float)(int)glyph->width + x;
			lineHeight  = font->lineHeight;
			vertex[2].u = u1;
			vertex[2].y = y - lineHeight;
			vertex[2].z = 0.0f;
			vertex[2].v = v1;
			vertex[2].colour =
				  bGColLUT[g]
				| bAColLUT[a]
				| bBColLUT[b]
				| bRColLUT[r];

			glyphX      = glyph->width;
			vertex[3].y = y;
			vertex[3].u = u1;
			vertex[3].v = v0;
			vertex[3].z = 0.0f;
			vertex[3].x = (float)(int)glyphX + x;
			vertex[3].colour =
				  bGColLUT[g]
				| bAColLUT[a]
				| bBColLUT[b]
				| bRColLUT[r];

			if (callback != NULL)
			{
				callbackInfo.glyphIndex++;
				callbackInfo.glyphNumber = glyphIndex;
				callbackInfo.curVertex   = vertex;

				callback(EBFONTCALLBACKREASON_POLY, &callbackInfo);
			}

			glyphIndex++;
			x       = (float)(int)glyph->width + x;
			vertex += 4;

		} while (glyphIndex < strLen);
	}

	bdVertexBufferUnlock(buffer);
	bdSetTexture(0, font->texture);
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
    // Clamp input string length to internal buffer size (per disasm)
    if (strLen > BMAX_PRINT_CHARS)
    {
        strLen = BMAX_PRINT_CHARS;
        bkPrintf("bdPrintFont: *** WARNING : String length truncated to %d chars ***\n",
                 BMAX_PRINT_CHARS);
    }

    // Prepare callback info
    TBFontCallbackInfo cb;
    cb.vertexBase   = vertBase;
    cb.curVertex    = vertBase;
    cb.glyphs       = bPrintFontGlyph;
    cb.curGlyph     = 0;
    cb.escapeString = 0;
    cb.lineNumber   = 0;
    cb.glyphNumber  = 0;
    cb.glyphIndex   = 0;
    cb.context      = callbackContext;

    // Texture size inverse (per disasm; assumed non?NULL when fonts are valid)
    float invTexW = 1.0f;
    float invTexH = 1.0f;
    if (font && font->texture)
    {
        invTexW = 1.0f / (float)font->texture->xDim;
        invTexH = 1.0f / (float)font->texture->yDim;
    }

    // -------------------------------------------------------------------------
    // 1) Build glyph array (bPrintFontGlyph) from the input string.
    //    This must follow the original Xbox semantics very closely:
    //      - '@['name']'  > texture escape + TEXTURE callback
    //      - '@(name)'    > generic escape + ESCAPE callback
    //      - '@@'         > literal '@'
    //      - '\\\\'       > literal '\\'
    //      - '\\n'        > forced newline (charCode = -2)
    //      - 1 / 2        > encoded '@' / '\\' characters
    // -------------------------------------------------------------------------

    TBPrintFontGlyph *glyphCursor = bPrintFontGlyph;
    int i = 0;

    while (i < strLen)
    {
        ushort ch = string[i];

        // ---------------------------------------------------------------------
        // '@'–based escape sequences
        // ---------------------------------------------------------------------
        if (ch == '@' && (i + 1) < strLen && string[i + 1] != '@')
        {
            ushort nextCh = string[i + 1];
            int j = i + 1;

            // @[textureName]
            if (nextCh == '[')
            {
                // Extract name between '[' and ']'
                char nameBuf[256];
                char *dst = nameBuf;
                int k = i + 2;
                while (k < strLen && string[k] != ']')
                {
                    if ((dst - nameBuf) < (int)(sizeof(nameBuf) - 1))
                    {
                        *dst++ = (char)string[k];
                    }
                    k++;
                }
                *dst = '\0';

                // Describe this as a "glyph" with special charCode -1
                glyphCursor->charCode = -1;

                // Load texture; fall back to glyph 0x37 width if missing (per disasm)
                TBTexture *tex = (TBTexture*)bLoadResource(0,
                                                           nameBuf,
                                                           EBRESTYPE_TEXTURE,
                                                           BDEFAULTGROUP);
                glyphCursor->texture = tex;

                if (!tex)
                {
                    const TBFontGlyphInfo *fgi = &font->glyph[0x37];
                    glyphCursor->width  = (float)(int)fgi->width;
                    glyphCursor->height = font->lineHeight;
                }
                else
                {
                    glyphCursor->width  = (float)tex->xDim;
                    glyphCursor->height = (float)tex->yDim;
                }

                // Full?texture quad [0..1]x[0..1]
                glyphCursor->u[0] = 0.0f; glyphCursor->v[0] = 0.0f;
                glyphCursor->u[1] = 0.0f; glyphCursor->v[1] = 1.0f;
                glyphCursor->u[2] = 1.0f; glyphCursor->v[2] = 1.0f;
                glyphCursor->u[3] = 1.0f; glyphCursor->v[3] = 0.0f;

                // TEXTURE callback – may veto this glyph (return 0 ? drop)
                if (callback)
                {
                    cb.escapeString = nameBuf;
                    cb.curGlyph     = glyphCursor;

                    if (!callback(EBFONTCALLBACKREASON_TEXTURE, &cb))
                    {
                        // Drop this glyph entirely
                        // (disasm decrements glyphCursor before the global increment)
                        // i.e. behave as if it was never emitted.
                        // We simply do NOT advance glyphCursor here.
                    }
                    else
                    {
                        ++glyphCursor;
                    }
                }
                else
                {
                    ++glyphCursor;
                }

                // Skip to the char after closing ']'
                i = (k < strLen) ? (k + 1) : strLen;
                continue;
            }

            // @(escapeName) – generic ESCAPE callback
            if (nextCh == '(')
            {
                char nameBuf[256];
                char *dst = nameBuf;
                int k = i + 2;
                while (k < strLen && string[k] != ')')
                {
                    if ((dst - nameBuf) < (int)(sizeof(nameBuf) - 1))
                    {
                        *dst++ = (char)string[k];
                    }
                    k++;
                }
                *dst = '\0';

                glyphCursor->charCode = -1;
                glyphCursor->width    = 0.0f;
                glyphCursor->height   = 0.0f;
                glyphCursor->texture  = 0;
                glyphCursor->u[0] = glyphCursor->u[1] =
                glyphCursor->u[2] = glyphCursor->u[3] = 0.0f;
                glyphCursor->v[0] = glyphCursor->v[1] =
                glyphCursor->v[2] = glyphCursor->v[3] = 0.0f;

                if (callback)
                {
                    cb.escapeString = nameBuf;
                    cb.curGlyph     = glyphCursor;

                    // For ESCAPE: only keep glyph when callback returns non?zero
                    if (callback(EBFONTCALLBACKREASON_ESCAPE, &cb))
                    {
                        ++glyphCursor;
                    }
                }

                // Skip to the char after closing ')'
                i = (k < strLen) ? (k + 1) : strLen;
                continue;
            }

            // Unknown '@X' escape – emit warning and skip the two chars
            bkPrintf("Unknown escape sequence '%c'\n", (uint)nextCh);
            i += 2;
            continue;
        }

        // ---------------------------------------------------------------------
        // Backslash handling:
        //   "\\\\" > literal '\\'
        //   "\\n"  > newline sentinel glyph (charCode = -2)
        //   others > pair skipped
        // ---------------------------------------------------------------------
        if (ch == '\\' && (i + 1) < strLen && string[i + 1] != '\\')
        {
            ushort esc = string[i + 1];

            if (esc == 'n')
            {
                // newline sentinel – width/height zero
                glyphCursor->charCode = -2;
                glyphCursor->width    = 0.0f;
                glyphCursor->height   = 0.0f;
                glyphCursor->texture  = 0;
                glyphCursor->u[0] = glyphCursor->u[1] =
                glyphCursor->u[2] = glyphCursor->u[3] = 0.0f;
                glyphCursor->v[0] = glyphCursor->v[1] =
                glyphCursor->v[2] = glyphCursor->v[3] = 0.0f;

                ++glyphCursor;
            }
            // any other backslash escape (e.g. '\x') is simply ignored as a pair

            i += 2;
            continue;
        }

        // ---------------------------------------------------------------------
        // Normal glyphs (including '@@' -> '@' and '\\\\' -> '\\')
        // ---------------------------------------------------------------------
        int srcIndex = i;
        if ((ch == '@' || ch == '\\') && (i + 1) < strLen && string[i + 1] == ch)
        {
            // '@@' or '\\\\' – consume both, but glyph comes from the second char
            srcIndex = i + 1;
        }

        ushort rawCh = string[srcIndex];
        ushort chCode;
        if (rawCh == 1)
            chCode = '@';
        else if (rawCh == 2)
            chCode = '\\';
        else
            chCode = rawCh;

        // Map to font glyph index (charCode used for wrapping/justify logic)
        int glyphIndex = (int)chCode - 0x20;
        if (glyphIndex < 0)
            glyphIndex = 0;

        const TBFontGlyphInfo *gi = &font->glyph[glyphIndex];
        if (gi->width == 0)
            gi = &font->glyph[0x80];

        glyphCursor->charCode = glyphIndex;
        glyphCursor->texture  = (TBTexture*)font->texture;
        glyphCursor->width    = (float)(int)gi->width;
        glyphCursor->height   = font->lineHeight;

        // NOTE: original Xbox code applies a half?texel offset to all glyphs
        //       (xPos * invTex + invTex * 0.5f, etc). Keep that behaviour.
        float u0 = (float)(int)gi->xPos * invTexW + invTexW * 0.5f;
        float v0 = (float)(int)gi->yPos * invTexH + invTexH * 0.5f;
        float u1 = (float)(int)gi->width  * invTexW + u0;
        float v1 = (float)(int)gi->height * invTexH + v0;

        // Index convention in TBPrintFontGlyph: 0=TL,1=BL,2=BR,3=TR
        glyphCursor->u[0] = u0; glyphCursor->v[0] = v0; // TL
        glyphCursor->u[1] = u0; glyphCursor->v[1] = v1; // BL
        glyphCursor->u[2] = u1; glyphCursor->v[2] = v1; // BR
        glyphCursor->u[3] = u1; glyphCursor->v[3] = v0; // TR

        ++glyphCursor;

        // Advance character index:
        //   - normal char: +1
        //   - '@@' / '\\\\' pair: +2
        if ((srcIndex == i) || (srcIndex >= strLen))
            i += 1;
        else
            i += 2;
    }

    const int glyphCount = (int)(glyphCursor - bPrintFontGlyph);
    if (glyphCount <= 0)
        return 0;

    // -------------------------------------------------------------------------
    // 2) Line layout: no?wrap case + wrapping case (including justify support)
    // -------------------------------------------------------------------------

    int lineCount = 0;

    if (!formatting->wrap)
    {
        TBFontLineInfo *dst = line;
        dst->glyph      = bPrintFontGlyph;
        dst->noofGlyphs = 0;
        dst->width      = 0.0f;
        dst->height     = (int)font->lineHeight;

        for (int giIdx = 0; giIdx < glyphCount; ++giIdx)
        {
            if (bPrintFontGlyph[giIdx].charCode == -2)
            {
                // newline sentinel – terminate current line and start a new one
                ++lineCount;
                ++dst;

                dst->glyph      = &bPrintFontGlyph[giIdx + 1];
                dst->noofGlyphs = 0;
                dst->width      = 0.0f;
                dst->height     = (int)font->lineHeight;
            }
            else
            {
                dst->noofGlyphs += 1;
                dst->width      += bPrintFontGlyph[giIdx].width;
                if ((int)bPrintFontGlyph[giIdx].height > dst->height)
                    dst->height = (int)bPrintFontGlyph[giIdx].height;
            }
        }

        ++lineCount;
    }
    else
    {
        // Wrapping: greedy with backwards search for last space (charCode == 0)
        TBFontLineInfo *dst = line;
        int giIdx = 0;

        dst->glyph      = &bPrintFontGlyph[0];
        dst->noofGlyphs = 0;
        dst->width      = 0.0f;
        dst->height     = (int)font->lineHeight;

        while (giIdx < glyphCount)
        {
            if (bPrintFontGlyph[giIdx].charCode == -2)
            {
                // Forced newline
                ++lineCount;
                ++dst;

                dst->glyph      = &bPrintFontGlyph[giIdx + 1];
                dst->noofGlyphs = 0;
                dst->width      = 0.0f;
                dst->height     = (int)font->lineHeight;

                ++giIdx;
                continue;
            }

            float nextW = dst->width + bPrintFontGlyph[giIdx].width;

            if (dst->noofGlyphs > 0 && nextW > formatting->width)
            {
                // Find last breakable glyph (space) on this line
                int back = dst->noofGlyphs - 1;
                while (back >= 0 && dst->glyph[back].charCode != 0)
                    --back;

                if (back >= 0)
                {
                    const int baseIndex = (int)(dst->glyph - bPrintFontGlyph);
                    giIdx = baseIndex + back + 1;

                    dst->width      = 0.0f;
                    dst->noofGlyphs = back;

                    for (int t = 0; t < back; ++t)
                        dst->width += dst->glyph[t].width;
                }

                ++dst;
                ++lineCount;

                dst->glyph      = &bPrintFontGlyph[giIdx];
                dst->noofGlyphs = 0;
                dst->width      = 0.0f;
                dst->height     = (int)font->lineHeight;
                continue;
            }

            dst->noofGlyphs += 1;
            dst->width = nextW;
            if ((int)bPrintFontGlyph[giIdx].height > dst->height)
                dst->height = (int)bPrintFontGlyph[giIdx].height;

            ++giIdx;
        }

        if (dst->noofGlyphs > 0)
            ++lineCount;
    }

    // -------------------------------------------------------------------------
    // 3) Vertical block offset (TOP / CENTRE / BOTTOM) – matches disasm:
    //       blockH = sum(line.height) + (lineCount - 1) * lineSpace
    // -------------------------------------------------------------------------

    float blockH = 0.0f;
    for (int ln = 0; ln < lineCount; ++ln)
        blockH += (float)line[ln].height;
    blockH += (float)(lineCount - 1) * formatting->lineSpace;

    float vOff = 0.0f;
    if (formatting->vertFormat == EBFONTFORMATTING_CENTRE)
        vOff = (formatting->height - blockH) * 0.5f;
    else if (formatting->vertFormat == EBFONTFORMATTING_BOTTOM)
        vOff = formatting->height - blockH;
    // TOP => vOff = 0

    // -------------------------------------------------------------------------
    // 4) Emit positioned vertices into VB and issue POLY callbacks
    // -------------------------------------------------------------------------

    TBPrimVertex *out = vertBase;
    const unsigned int colourPacked =
        bGColLUT[(g & 255)] | bAColLUT[(a & 255)] |
        bBColLUT[(b & 255)] | bRColLUT[(r & 255)];

    float yBase = formatting->y - vOff;

    for (int x = 0; x < lineCount; ++x)
    {
        TBFontLineInfo *L = &line[x];

        // Horizontal alignment / justification
        float hOff = 0.0f;
        if (formatting->horzFormat == EBFONTFORMATTING_CENTRE)
        {
            hOff = (formatting->width - L->width) * 0.5f;
        }
        else if (formatting->horzFormat == EBFONTFORMATTING_RIGHT)
        {
            hOff = (formatting->width - L->width);
        }
        else if (formatting->horzFormat == EBFONTFORMATTING_JUSTIFY)
        {
            int spaces = 0;
            for (int k = 0; k < L->noofGlyphs; ++k)
            {
                if (L->glyph[k].charCode == 0)
                    ++spaces;
            }

            if (spaces > 1)
            {
                float extra = formatting->width - L->width;
                if (extra > 0.0f)
                    hOff = extra / (float)(spaces - 1);
            }
        }

        const float lineH    = (float)L->height;
        const float yTopLine = yBase - lineH;
        float x = formatting->x;
        float justifyStep = 0.0f;

        if (formatting->horzFormat == EBFONTFORMATTING_JUSTIFY)
        {
            // For JUSTIFY, we spread extra width between spaces;
            // base x is left edge, hOff is per?space increment.
            justifyStep = hOff;
        }
        else
        {
            x += hOff;
        }

        cb.lineNumber = x;

        for (int k = 0; k < L->noofGlyphs; ++k)
        {
            const TBPrintFontGlyph &gph = L->glyph[k];
            if (gph.charCode == -2)
                continue;

            if (formatting->horzFormat == EBFONTFORMATTING_JUSTIFY &&
                gph.charCode == 0 && justifyStep > 0.0f)
            {
                // Space – add justification gap before glyph
                x += justifyStep;
            }

            const float yTopGlyph = yTopLine + (lineH - gph.height) * 0.5f;
            const float yBotGlyph = yTopGlyph + gph.height;

            // TL
            out[0].x = x;
            out[0].y = yTopGlyph;
            out[0].z = 0.0f;
            out[0].u = gph.u[1];
            out[0].v = gph.v[1];
            out[0].colour = colourPacked;

            // BL
            out[1].x = x;
            out[1].y = yBotGlyph;
            out[1].z = 0.0f;
            out[1].u = gph.u[0];
            out[1].v = gph.v[0];
            out[1].colour = colourPacked;

            // TR
            out[2].x = x + gph.width;
            out[2].y = yTopGlyph;
            out[2].z = 0.0f;
            out[2].u = gph.u[2];
            out[2].v = gph.v[2];
            out[2].colour = colourPacked;

            // BR
            out[3].x = x + gph.width;
            out[3].y = yBotGlyph;
            out[3].z = 0.0f;
            out[3].u = gph.u[3];
            out[3].v = gph.v[3];
            out[3].colour = colourPacked;

            if (callback)
            {
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