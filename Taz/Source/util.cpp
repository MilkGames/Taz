// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : utils.cpp
//   Purpose : general utility functions
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "main.h"

#include "util.h"


/*	--------------------------------------------------------------------------------
	Function 	: util3DText
	Purpose 	: display a text string at a position in 3D
	Parameters 	: string, position
	Returns 	: 
	Info 		: 
*/
void util3DText(char* const string, const TBVector pos)
{
	ushort	string16[256];

	bkString8to16(string16, string);

	TBVector	dest;
	TBVector	src;

	bmVectorCopy(src, pos);

	src[W]=1.0f;

	// PP: IMPORTANT: SET 3D MODE BEFORE CALLING bdProjectVertices!!!!!!
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetIdentityObjectMatrix();
	bdProjectVertices(dest, src, 1);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);

	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	bdPrintFontLite(standardFont, string16, strlen(string), dest[X], dest[Y], 0, 128, 0, 128, NULL, NULL);

	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
}


/*	--------------------------------------------------------------------------------
	Function 	: util3DText
	Purpose 	: display a text string at a position in 3D
	Parameters 	: position (TBVector or ref to VEC), format string, argument list
	Returns 	: 
	Info 		: 
*/
void util3DText(const VEC& pos, const char * const fmt, ...)
{
	char			string8[256];

	va_list			argp;

	va_start(argp, fmt);
	vsprintf(&string8[0], fmt, argp);
	va_end(argp);

	util3DText(string8, pos.v);
}


/*	--------------------------------------------------------------------------------
	Function 	: utilWait
	Purpose 	: delay the game for a specified length of time
	Parameters 	: (opt/one second) length fo time for which to delay the game
	Returns 	: 
	Info 		: 
*/
void utilWait(const float time)
{
	TBTimerValue	startTime, endTime;
	
	startTime=bkTimerRead();

	do
	{
		endTime=bkTimerRead();
	}
	while(bkTimerDelta(startTime, endTime)/(float)bkTimerGetFrequency() < time);
}


/*	--------------------------------------------------------------------------------
	Function 	: utilFontPrint
	Purpose 	: Print a font on the screen, with left, right or centering
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void utilFontPrint(char *s, int x, int y, float scale, int align)
{
	ushort pString16[128];
	float xs, ys;
	TBMatrix mScaled;

	ASSERT(s);

	bmMatCopy(mScaled, bIdentityMatrix);
	bmMatScale(mScaled, scale, scale, scale);
	bdSetObjectMatrix(mScaled);

	bkString8to16(pString16, s);


	switch(align)
	{
	case UTILFONTPRINT_LEFTALIGN:
		bdPrintFontLite(standardFont, pString16, bkStringLength16(pString16), ((float)x)/scale, ((float)y)/scale, 127,127,127, 127, NULL, NULL);
		break;

	case UTILFONTPRINT_RIGHTALIGN:
		bdFontExtent(standardFont, pString16, bkStringLength16(pString16), &xs, &ys);
		bdPrintFontLite(standardFont, pString16, bkStringLength16(pString16), ((float)x)/scale - xs, ((float)y)/scale, 127,127,127, 127, NULL, NULL);
		break;

	case UTILFONTPRINT_CENTRE:
		bdFontExtent(standardFont, pString16, bkStringLength16(pString16), &xs, &ys);
		bdPrintFontLite(standardFont, pString16, bkStringLength16(pString16), ((float)x)*scale-(xs/2), ((float)y)*scale+(ys/2), 127,127,127, 127, NULL, NULL);
		break;

	default:
		bkAlert("Your mum");
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: utilGetPrettyTime
	Purpose 	: Convert a floating point seconds timer into user defined format
	Parameters 	: s = dest string, time = float timer, f = user defined format string (NULL = default)
	Returns 	: 
	Info 		:
*/

int utilGetPrettyTime(char *s, float time, char *f)
{
	int centiseconds = (int)(10.0f * bmFMod(time, 1.0));
	int seconds = (int)bmFMod(time, 60.0);
	int minutes = (int)(time/60.0);

	ASSERT(s);

	if(!f)
		f = "%d:%02d:%1d";

	return(sprintf(s, f, minutes, seconds, centiseconds));
}





/*	--------------------------------------------------------------------------------
	Function 	: utilBaseStringtoLong
	Purpose 	: Converts an ASCII string into a number FAST, while allowing a number base to be specified
	Parameters 	: s = source string, base = base (16 = hex, 10 = decimal etc), len = number of chars
	Returns 	: 
	Info 		: Passing base < 2 or > 32 returns 0. Effect of string characters outside base limits is undefined.
	            : The routine will operate faster if the base is a power of two.
*/

char hexTable[255];
char *hexChars[] = {"0123456789ABCDEFGHIJKLMNOPQRSTUV", "0123456789abcdefghijklmnopqrstuv"};
char shiftTable[33] = {0, 0, 1, 0, 2, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5};

uint32 utilBaseStringtoLong(char *s, int base, int len)
{	
	int i;
	uint32 result;
	int shift;

	static int calledB4 = 0;

	ASSERTM(s, "You passed a NULL string into utilBaseStringtoLong()");
	ASSERTM(len, "You passed a zero length into utilBaseStringtoLong()");

	if(!calledB4)
	{
		// CPW: generate lookup table
		int i,j;

		for(j=0; j<2; j++)
		{
			for(i=0; i<32; i++)
				hexTable[hexChars[j][i]] = i;
		}
		calledB4++;
	}

	if(base < 2)
		return 0;

	if(base > 32)
		return 0;

	result = hexTable[s[0]];

	shift = shiftTable[base];
	if(shift)
	{
		for(i=1; i<len; i++)
		{
			result = result << shift;
			result|=hexTable[s[i]];
		}
	}
	else
	{
		for(i=1; i<len; i++)
		{
			result = result * base;
			result+=hexTable[s[i]];
		}
	}

	return result;
}

/*	--------------------------------------------------------------------------------
	Function 	: utilLongtoBaseString
	Purpose 	: Converts a number into an ASCII string FAST, while allowing a number base to be specified
	Parameters 	: s = source string, num = the number, base = base (16 = hex, 10 = decimal etc)
	Returns 	: Length of final string (not including terminating zero)
	Info 		: Passing base < 2 or > 32 returns -1.
	            : The routine will operate faster if the base is a power of two.
*/

int utilLongtoBaseString(char *s, int32 num, int base)
{
	char sBuff[34];
	char *pb = sBuff;
	char *ss = s;

	int mod;
	char c;
	int shift;
	int d;

	if(base < 2)
		return -1;

	if(base > 32)
		return -1;


	shift = shiftTable[base];
	if(shift)
	{
		do
		{
			mod = num & (base-1);
			c = hexChars[0][mod];

			*(pb++) = c;
			num = num >> shift;
		}while(num);
	}
	else
	{
		do
		{
			mod = num % base;
			c = hexChars[0][mod];

			*(pb++) = c;
			num = num / base;
		}while(num);
	}
	*pb = 0;

	d=0;
	// CPW: turn string round!
	do
	{
		c = *(--pb);
		*(s++) = c;
		d++;
	}while(pb!=sBuff);
	*s = 0;

	return d;
}

/*	--------------------------------------------------------------------------------
	Function 	: utilFindChar
	Purpose 	: Find a character in a string
	Parameters 	: s = dest string, c = character
	Returns 	: Index of char in string, -1 if not found
	Info 		: NB. Calling this with a 2nd parameter of 0 will essentially perform a strlen()
*/

int utilFindChar(char *s, char c)
{
	for(int i=0;;i++)
	{
		if(s[i] == c)
			return i;

		if(s[i] == 0)
			return -1;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: utilDraw2DSprite
	Purpose 	: draw a 2D icon
	Parameters 	: Suree Bob, we got parameters - big ones!  We got a whole field full of parameters out back.  Wanna see 'em?
					texture ptr, 2D pos (TBVector2D!), (opt/ICON_WIDTH)width, (opt/0.0f=width)height,
					(opt/0.0f)ang, (opt/solid white)colour, (opt/false)u flip,
					(opt/false)v flip, (opt/NULL)verts to fill in, (opt/0.0f)zBias,centerOffx -1 -> +1,centerOffy -1 -> +1
	Returns 	: 
	Info 		: this replaces bdDrawFlatSprite which doesn't work in 2D projection mode.
					***** PLEASE USE THIS NOW INSTEAD OF 'DRAW2DSPRITE' *****
*/
void utilDraw2DSprite(	TBTexture* const	texture,
						const TBVector2D	pos,
						const float			width,
						float				height,
						const float			ang,
						const COLOUR&		col,
						const bool			uFlip,
						const bool			vFlip,
						float* const		destVerts,
						const float			zBias,
						const float			centerOffx,
						const float			centerOffy)
{ 
	TBPrimVertex	verts[4];
//	float			penX;
//	float			penY;
//	float			theta;			// PP: angle from centre to a corner, disregarding icon tilt
//	float			hypot;
	TBMatrix		matrix,rotMat,tempMatrix,rmatrix;

	// PP: KINDA IMPORTANT!
	// PP: a height of zero means height=width
	if(height == 0.0f)
	{
		height=width;
	}

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
//	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 0);
//	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
	
	if (texture)
		bdSetTexture(0,texture);

	for(int v=0; v<4; v++)
	{ 
		BDVERTEX_SETXYZW(&verts[v],(((v&1)?1.0f:-1.0f)-centerOffx)*width*0.5f,(((v/2)?-1.0f:1.0f)-centerOffy)*height*0.5f,0.0f,1.0f);
		BDVERTEX_SETCOLOUR(&verts[v],col);
		BDVERTEX_SETUV(&verts[v],(float)(v&1), (float)(v/2));
	} 

	// TP: Made a few alterations in here to support an center point for the sprite, hope it doesn't cause any problems
	bmMatTranslate(matrix,pos[X],pos[Y], 0.0f);

	if (ang)
	{
		bmMatZRotation(rotMat,-ang);
		bmMatMultiply(rmatrix,matrix,rotMat);
		bmMatCopy(matrix,rmatrix);
	}
	
	// TP: Keep copy of current matrix
	bdGetObjectMatrix(tempMatrix);

	// TP: Multiply new matrix by current
	bmMatMultiply(rmatrix,tempMatrix,matrix);


	bdSetObjectMatrix(rmatrix);
 	bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP,verts,4,BVERTTYPE_SINGLE);

	// TP: Better set this back just in case
	bdSetObjectMatrix(tempMatrix);
}


/*	--------------------------------------------------------------------------------
	Function 	: utilDrawSphere
	Purpose 	: draw a sphere
	Parameters 	: pos VEC, (opt/small)radius, (opt/grey) colour
	Returns 	: 
	Info 		: taken from Xed
*/
#define SPIRALSPHERE_NUM_TURNS			4
#define SPIRALSPHERE_LINES_PER_TURN		20
#define SPIRALSPHERE_MAX_VERTS			(SPIRALSPHERE_NUM_TURNS*SPIRALSPHERE_LINES_PER_TURN)

void utilDrawSphere(const VEC& pos, const float radius, const COLOUR& col)
{
	TBPrimVertex vtx[SPIRALSPHERE_MAX_VERTS];
				
	float turnRadius;
	float theta=TWOPI/(float)SPIRALSPHERE_LINES_PER_TURN;
	float angle=0.0f;
	float temp;
	int index=0;

	//FOR EACH TURN
	for(int turn=SPIRALSPHERE_NUM_TURNS-1;turn>=0;turn--)
	{
		// DRAW A CIRCLE
		for(int line=SPIRALSPHERE_LINES_PER_TURN-1;line>=0;line--)
		{
			temp=((((turn*SPIRALSPHERE_LINES_PER_TURN)+line)/(float)(SPIRALSPHERE_NUM_TURNS*SPIRALSPHERE_LINES_PER_TURN))*(radius*2.0f))-radius;
			turnRadius=bmSqrt(SQR(radius)-SQR(temp));

			BDVERTEX_SETXYZW(&vtx[index], pos[X] + (turnRadius*bmSin(angle)),pos[Y]+temp, pos[Z] + (turnRadius*bmCos(angle)), 1.0f);
			//BDVERTEX_SETXYZW(&vtx[index], 0.0f, 0.0f, 0.0f, 1.0f);
			BDVERTEX_SETCOLOUR(&vtx[index],col);
			angle+=theta;

			index++;
		}
	}

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetIdentityObjectMatrix();
	bdSetTexture(0, NULL);

	ASSERT(index);
	bdDrawPrimitive(BDPRIMTYPE_LINESTRIP, &vtx, index, BVERTTYPE_SINGLE);

	bdSetZBias(0.0f);
}


/*	--------------------------------------------------------------------------------
	Function 	: utilDrawSparkler
	Purpose 	: draw a pretty sparkly thing
	Parameters 	: pos VEC, (opt/small)radius, (opt/default) colour
	Returns 	: 
	Info 		: taken from LUST
*/

#define SPARKLER_NUM_LINES		30

void utilDrawSparkler(const VEC& pos, const float radius, const COLOUR& col)
{
	static TBPrimVertex		vtx[SPARKLER_NUM_LINES*2];
	VEC						vec;

	BDVERTEX_SETPOS(&vtx[0], pos);
	BDVERTEX_SETCOLOUR(&vtx[0], col);

	for(int i=1; i<SPARKLER_NUM_LINES*2; i++)
	{
		vtx[i]=vtx[0];

		i++;

		vec=(VEC(bmRandfSigned(), bmRandfSigned(), bmRandfSigned())*radius)+pos;

		BDVERTEX_SETVEC(&vtx[i], vec);
		BDVERTEX_SETCOLOUR(&vtx[i], col);
	}

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetIdentityObjectMatrix();
	bdSetTexture(0, NULL);

	bdDrawPrimitive(BDPRIMTYPE_LINESTRIP, &vtx, SPARKLER_NUM_LINES*2, BVERTTYPE_SINGLE);
}


/*	--------------------------------------------------------------------------------
	Function 	: utilDrawArrow
	Purpose 	: draw a 3D arrow
	Parameters 	: pos VEC, (opt/straight along Z) ang VEC, (opt/default) length, (opt/default) colour
	Returns 	: 
	Info 		: taken from LUST
*/

#define ARROW_HEAD_HALFWIDTH_RATIO	0.3f
#define ARROW_STALK_LENGTH_RATIO	0.6f
#define ARROW_STALK_WIDTH_RATIO		0.35f
#define ARROW_TURN_TIME				3.0f

void utilDrawArrow(const VEC& pos, const VEC& ang, const float length, const COLOUR& col)
{
	static VEC				head[3]={	VEC(0,0,length),
										VEC(-length*ARROW_HEAD_HALFWIDTH_RATIO,0,length*ARROW_STALK_LENGTH_RATIO),
										VEC(length*ARROW_HEAD_HALFWIDTH_RATIO,0,length*ARROW_STALK_LENGTH_RATIO)
									};


	static VEC				stalk[4]={
										VEC(-(length*ARROW_HEAD_HALFWIDTH_RATIO*ARROW_STALK_WIDTH_RATIO),0,length*ARROW_STALK_LENGTH_RATIO),
										VEC(length*ARROW_HEAD_HALFWIDTH_RATIO*ARROW_STALK_WIDTH_RATIO,0,length*ARROW_STALK_LENGTH_RATIO),
										VEC(length*ARROW_HEAD_HALFWIDTH_RATIO*ARROW_STALK_WIDTH_RATIO,0,0),
										VEC(-(length*ARROW_HEAD_HALFWIDTH_RATIO*ARROW_STALK_WIDTH_RATIO),0,0),
									};

	static TBPrimVertex		vtx[4];

	int						i;
	VEC						vec;
	float					zAng;

	zAng=(bmFMod(gameStatus.appTime,ARROW_TURN_TIME)/ARROW_TURN_TIME)*TWOPI;

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetIdentityObjectMatrix();
	bdSetTexture(0, NULL);

	for(float a=0.0f; a<=HALFPI; a+=HALFPI)
	{
		for(i=0; i<3; i++)
		{
			vec=head[i].rotateZ(a+zAng).rotate(ang)+pos;

			BDVERTEX_SETVEC(&vtx[i], vec);
			BDVERTEX_SETCOLOUR(&vtx[i], col);
		}

		bdDrawPrimitive(BDPRIMTYPE_TRIANGLEFAN, &vtx, 3, BVERTTYPE_SINGLE);

		for(i=0; i<4; i++)
		{
			vec=stalk[i].rotateZ(a+zAng).rotate(ang)+pos;

			BDVERTEX_SETVEC(&vtx[i], vec);
			BDVERTEX_SETCOLOUR(&vtx[i], col);
		}

		bdDrawPrimitive(BDPRIMTYPE_TRIANGLEFAN, &vtx, 4, BVERTTYPE_SINGLE);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: utilDrawSelectionMarker
	Purpose 	: draw a LUST-style selection marker
	Parameters 	: pos VEC, (opt/default) radius, (opt/false) feint and flickering
	Returns 	: 
	Info 		: parm1 can't be const cos of babel
*/

#define SELECTIONMARKER_DEFAULT_RADIUS	150.0f

#define SELECTIONMARKER_ROT_TIME		2.0f
#define SELECTIONMARKER_ALPHA			60
#define SELECTIONMARKER_ALPHA_FEINT		40
#define SELECTIONMARKER_COLOUR1			COLOUR(255,100,0,SELECTIONMARKER_ALPHA)
#define SELECTIONMARKER_COLOUR2			COLOUR(0,255,100,SELECTIONMARKER_ALPHA)
#define SELECTIONMARKER_TEXTURE			"lightselection.bmp"

void utilDrawSelectionMarker(VEC& pos, const float radius, const bool feint)
{
	static COLOUR	col;
	float			ang;

	if(feint)
	{
		if(gameStatus.frameNumber&1)
		{
			return;
		}
	}

	ang=(bmFMod(gameStatus.appTime, SELECTIONMARKER_ROT_TIME)/SELECTIONMARKER_ROT_TIME)*TWOPI;

	if(int(ang/HALFPI)&1)
	{
		col=SELECTIONMARKER_COLOUR1;
	}
	else
	{
		col=SELECTIONMARKER_COLOUR2;
	}

	if(feint)
	{
		col.a=SELECTIONMARKER_ALPHA_FEINT;
	}

	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, 0);

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetIdentityObjectMatrix();

	bdSetTexture(0, bkLoadTexture(NULL, SELECTIONMARKER_TEXTURE, 0));// PP: don't give a shit about speed in this debug func

	static TBPrimVertex		vtx[4];
	static VEC				vec;

	for(int v=0; v<4; v++)
	{
		vec=(VEC((v/2)?1.f:-1.f, (v&1)?1.f:-1.f)*radius).rotateZ(ang).rotate(VEC(bViewInfo.xPos, bViewInfo.yPos, bViewInfo.zPos).ang(pos))+pos;

		BDVERTEX_SETVEC(&vtx[v], vec);
		BDVERTEX_SETUV(&vtx[v], (v/2)?1.0f:0.0f, (v&1)?1.0f:0.0f);
		BDVERTEX_SETCOLOUR(&vtx[v], col);
	}

	bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, &vtx, 4, BVERTTYPE_SINGLE);

	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, 0);
}


/*	--------------------------------------------------------------------------------
	Function 	: utilDrawLine
	Purpose 	: draw a line
	Parameters 	: 3d start pos, 3d end pos, (opt/GREEN)ref to colour at start pos, (opt/NULL=start colour) ptr to colour at end pos
	Returns 	: 
	Info 		: 
*/
void utilDrawLine(const TBVector start, const TBVector end, const COLOUR& startCol, const COLOUR* const endCol)
{
	static TBPrimVertex		verts[2];

	bdSetTexture(0, NULL);

	BDVERTEX_SETPOS(&verts[0], start);
	BDVERTEX_SETCOLOUR(&verts[0], startCol);

	BDVERTEX_SETPOS(&verts[1], end);

	if(endCol == NULL)
	{
		BDVERTEX_SETCOLOUR(&verts[1], startCol);
	}
	else
	{
		BDVERTEX_SETCOLOUR(&verts[1], *endCol);
	}

	bdDrawPrimitive(BDPRIMTYPE_LINELIST, &verts, 2, BVERTTYPE_SINGLE);
}

void utilDrawThickLine(const TBVector start, const TBVector end, const float thickness, const COLOUR& Col)
{
	static TBPrimVertex		verts[4];

	TBVector	temp1, temp2;

	float		rot;

	bdSetTexture(0, NULL);

	bmVectorSub(temp1, end, start);

	if(temp1[X])
	{
		rot = bmATan(temp1[Y]/temp1[X]);
	}
	else
	{
		rot = PI/2;
	}

	ZEROVECTOR(temp2);

	// JW: Vertex 1 : BR

	SETVECTOR(temp2, 0.0f, thickness/2, 0.0f, 1.0f);
	bmVectorRotateZ(temp2,temp2,rot);
	bmVectorAdd(temp2,temp2,start);
	BDVERTEX_SETPOS(&verts[0], temp2);
	BDVERTEX_SETCOLOUR(&verts[0], Col);

	// JW: Vertex 1 : BL

	SETVECTOR(temp2, 0.0f, -thickness/2, 0.0f, 1.0f);
	bmVectorRotateZ(temp2,temp2,rot);
	bmVectorAdd(temp2,temp2,start);
	BDVERTEX_SETPOS(&verts[1], temp2);
	BDVERTEX_SETCOLOUR(&verts[1], Col);

	// JW: Vertex 1 : TR

	SETVECTOR(temp2, 0.0f, thickness/2, 0.0f, 1.0f);
	bmVectorRotateZ(temp2,temp2,rot);
	bmVectorAdd(temp2,temp2,end);
	BDVERTEX_SETPOS(&verts[2], temp2);
	BDVERTEX_SETCOLOUR(&verts[2], Col);

	// JW: Vertex 1 : TL

	SETVECTOR(temp2, 0.0f, -thickness/2, 0.0f, 1.0f);
	bmVectorRotateZ(temp2,temp2,rot);
	bmVectorAdd(temp2,temp2,end);
	BDVERTEX_SETPOS(&verts[3], temp2);
	BDVERTEX_SETCOLOUR(&verts[3], Col);
	
	bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, &verts, 4, BVERTTYPE_SINGLE);
}

/*	--------------------------------------------------------------------------------
	Function 	: utilDrawCircle
	Purpose 	: draw a horizontal circle in 3D space
	Parameters 	: 3D position of centre point, (opt/small)radius, (opt/grey) colour
	Returns 	: 
	Info 		: 
*/
void utilDrawCircle(const TBVector pos, const float radius, const COLOUR& col)
{
	static TBPrimVertex		verts[CIRCLE_NUM_LINES+1];
	static float			thetaStep=TWOPI/(float)CIRCLE_NUM_LINES;
	float					theta=0.0f;
	
	for(int v=CIRCLE_NUM_LINES; v>0; v--, theta+=thetaStep)
	{
		BDVERTEX_SETXYZW(&verts[v], pos[X]+(radius*bmCos(theta)), pos[Y], pos[Z]+(radius*bmSin(theta)), 0.0f);
		BDVERTEX_SETCOLOUR(&verts[v], col);
	}

	verts[0]=verts[CIRCLE_NUM_LINES];

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetIdentityObjectMatrix();
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetTexture(0, NULL);
	bdSetZBias(-0.2f);

	ASSERT(CIRCLE_NUM_LINES+1);
#if BPLATFORM == PS2
	bdDrawPrimitive(BDPRIMTYPE_LINELIST, &verts, CIRCLE_NUM_LINES+1, BVERTTYPE_SINGLE);// PP: linestrips aren't supported on PS2 I'm told
#else
	bdDrawPrimitive(BDPRIMTYPE_LINESTRIP, &verts, CIRCLE_NUM_LINES+1, BVERTTYPE_SINGLE);
#endif

	bdSetZBias(0.0f);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
}


/*	--------------------------------------------------------------------------------
	Function 	: utilDrawCircleSector
	Purpose 	: draw a horizontal circle sector in 3D space
	Parameters 	: 3D position of centre point, (opt/0.0f)facing angle, (opt/few metres) radius, (opt/small) field in either direction,
					(opt/grey) colour
	Returns 	: 
	Info 		: 
*/
void utilDrawCircleSector(const TBVector	pos,
						  const float		ang,
						  const float		radius,
						  const float		field,
						  const COLOUR&		col)
{
	static TBPrimVertex		verts[CIRCLE_SECTOR_NUM_LINES+1];
	float			thetaStep=(field*2)/(float)(CIRCLE_SECTOR_NUM_LINES-2);
	float					theta=ang-field-HALFPI;			// PP: not sure why the HALFPI
		
	for(int v=CIRCLE_SECTOR_NUM_LINES-1; v>0; v--, theta+=thetaStep)
	{
		BDVERTEX_SETXYZW(&verts[v], pos[X]+(radius*bmCos(theta)), pos[Y], pos[Z]+(radius*bmSin(theta)), 0.0f);
		BDVERTEX_SETCOLOUR(&verts[v], col);
	}

	BDVERTEX_SETXYZW(&verts[0], pos[X], pos[Y], pos[Z], 0.0f);
	BDVERTEX_SETCOLOUR(&verts[0], col);
	verts[CIRCLE_SECTOR_NUM_LINES]=verts[0];

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetIdentityObjectMatrix();
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetTexture(0, NULL);
	bdSetZBias(-0.2f);

	ASSERT(CIRCLE_SECTOR_NUM_LINES+1);
#if BPLATFORM == PS2
	bdDrawPrimitive(BDPRIMTYPE_LINELIST, &verts, CIRCLE_SECTOR_NUM_LINES+1, BVERTTYPE_SINGLE);// PP: linestrips aren't supported on PS2 I'm told
#else
	bdDrawPrimitive(BDPRIMTYPE_LINESTRIP, &verts, CIRCLE_SECTOR_NUM_LINES+1, BVERTTYPE_SINGLE);
#endif

	bdSetZBias(0.0f);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
}


/*	--------------------------------------------------------------------------------
	Function 	: utilDrawBoundingBox
	Purpose 	: draw a bounding box
	Parameters 	: (opt/GREEN) colour in which to draw the bounding box
	Returns 	: 
	Info 		: // PP: This is a debug facility, and not guaranteed to be fast, so check
						what it does if you're going to use it in the final game for any reason.
						That applies to all the Util draw functions!
						Hell, in fact it applies to ALL my code! heheh ;o)
*/
void utilDrawBoundingBox(const BOUNDINGBOX& box, const COLOUR& col)
{
	utilDrawLine(VEC(box.xmin, box.ymin, box.zmin), VEC(box.xmin, box.ymin, box.zmax), col);
	utilDrawLine(VEC(box.xmin, box.ymin, box.zmin), VEC(box.xmin, box.ymax, box.zmin), col);
	utilDrawLine(VEC(box.xmin, box.ymin, box.zmin), VEC(box.xmax, box.ymin, box.zmin), col);

	utilDrawLine(VEC(box.xmax, box.ymax, box.zmax), VEC(box.xmax, box.ymax, box.zmin), col);
	utilDrawLine(VEC(box.xmax, box.ymax, box.zmax), VEC(box.xmax, box.ymin, box.zmax), col);
	utilDrawLine(VEC(box.xmax, box.ymax, box.zmax), VEC(box.xmin, box.ymax, box.zmax), col);

	utilDrawLine(VEC(box.xmax, box.ymin, box.zmin), VEC(box.xmax, box.ymax, box.zmin), col);
	utilDrawLine(VEC(box.xmax, box.ymin, box.zmin), VEC(box.xmax, box.ymin, box.zmax), col);

	utilDrawLine(VEC(box.xmin, box.ymax, box.zmin), VEC(box.xmax, box.ymax, box.zmin), col);
	utilDrawLine(VEC(box.xmin, box.ymax, box.zmin), VEC(box.xmin, box.ymax, box.zmax), col);

	utilDrawLine(VEC(box.xmin, box.ymin, box.zmax), VEC(box.xmax, box.ymin, box.zmax), col);
	utilDrawLine(VEC(box.xmin, box.ymin, box.zmax), VEC(box.xmin, box.ymax, box.zmax), col);
}


/* --------------------------------------------------------------------------------
   Function : MATHS_NormaliseQuaternion
   Purpose : called to normalise a quaternion
   Parameters : 
   Returns : 
   Info : Quaternions go all weird if not normalised 
   Author : crushforth
*/
void utilNormaliseQuaternion(TBQuaternion q)
{
	float	dist, square;

	square = q[X]*q[X] + q[Y]*q[Y] + q[Z]*q[Z] + q[W]*q[W];
	
	if (square > 0.0)
		dist = (float) (1.0f / bmSqrt(square));
	else
	{
		dist = 1.0f;
	}

    q[X] *= dist;
    q[Y] *= dist;
    q[Z] *= dist;
    q[W] *= dist;
}

/*	--------------------------------------------------------------------------------
	Function 	: utilGenerateRandomNonZeroVector
	Purpose 	: Turn a number into a formatted text string with ,'s
	Parameters 	: number to convert (integers only), string to convert to (char *)
	Returns 	: nowt
	Info 		: If anyone thinks of a better way to do it they're welcome to take a shot
*/

void utilFormatPrettyNumbers(int value, char string[])
{
	char *ptr;

	sprintf(string, "%d",value);

	if (strlen(string) < 4)	return;

	int	i = 0, j = 0, k = 0, l = 0;	// JW: k = number of ,'s inserted

	do
	{
		k ++;

		l = strlen(string);

		i = l - (3 * k) - (k - 1);	// JW: from the end character go back 3 (thousand) and 1 comma for each new iteration
									// JW: no commas to begin with hence the (k - 1)

		if (i < 1)	break;			// JW: We've put in all the commas so break

		ptr = &string[i];

		for (j = l; j > i; j--)
		{
			string[j] = string[j - 1];
		}

		string[l + 1] = 0;

		switch(gameStatus.currentLanguage)
		{
		/*	BLANGUAGEID_UK;	break;
			BLANGUAGEID_E;	break;*/

		case BLANGUAGEID_F:		string[i] = ' ';	break;
		
		case BLANGUAGEID_E:
		case BLANGUAGEID_D:
		case BLANGUAGEID_IT:	string[i] = '.';	break;
		
		default:	string[i] = ',';	break;
		}
	}
	while (true);
}

/*	--------------------------------------------------------------------------------
	Function 	: utilGenerateRandomNonZeroVector
	Purpose 	: Generate a random vector that hopefully won't cause divide by zero errors
	Parameters 	: Your mum
	Returns 	: Your dad
	Info 		: Your sister
*/

void utilGenerateRandomNonZeroVector(TBVector v, float range, int negative)
{
	v[X] = (bmRand()%((int)range-1)) + 1.0f;
	v[Y] = (bmRand()%((int)range-1)) + 1.0f;
	v[Z] = (bmRand()%((int)range-1)) + 1.0f;

	if(negative)
	{
		int f = bmRand();

		if(f&1) v[X] = -v[X];
		if(f&2) v[Y] = -v[Y];
		if(f&4) v[Z] = -v[Z];
	}

	v[W] = 1.0f;
}

/*	--------------------------------------------------------------------------------
	Function 	: UtilGenerateRandomNumber
	Purpose 	: Generates a random number between given bounds quickly
	Parameters 	: upper, lower bounds
	Returns 	: the number that you're after
*/

float UtilGenerateRandomNumber(float upperLimit, float lowerLimit)
{
	return (((upperLimit-lowerLimit)*bmRandf())+lowerLimit);
}


/*	--------------------------------------------------------------------------------
	Function 	: utilFindStringInArray
	Purpose 	: finds a string in an array of strings
	Parameters 	: array in which to search, number of elements in array, string to find
	Returns 	: array index of string if found, or -1 if not found
	Info 		: 
*/
int utilFindStringInArray(const char *const *const array, const int arraySize, const char* const string)
{
	for(int i=0; i<arraySize; i++)
	{
		if(stricmp(array[i], string) == 0)
		{
			// PP: string found
			return i;
		}
	}

	// PP: string not found
	return -1;
}


/*	--------------------------------------------------------------------------------
	Function 	: utilSwap
	Purpose 	: swap two elements of an int32 array
	Parameters	: ptr to array of int32, two indices to swap
	Returns		: 
	Info		: 
*/
void utilSwap(int32* const v, const int32 i, const int32 j)
{
	int32 temp;

	temp=v[i];
	v[i]=v[j];
	v[j]=temp;
}


/*	--------------------------------------------------------------------------------
	Function 	: utilQuickSort
	Purpose 	: sort an array of int32's using a quick-sort algorithm
	Parameters	: ptr to array of int32s, left index of array (0), right index of array (arraySize-1)
	Returns		: 
	Info		: sorts into ascending order
	Shoe-horn	: This was such a Hoare to get working
*/
void utilQuickSort(int32* const a, const int32 left, const int32 right)
{
	int32 i, last;

	if(left >= right) return;// PP: feck off

	utilSwap(a, left, (left+right)/2);
	last=left;
	for(i=left+1;i<=right;i++)
		if(a[i]<a[left])
			utilSwap(a, ++last, i);
	utilSwap(a, left, last);
	utilQuickSort(a, left, last-1);
	utilQuickSort(a, last+1, right);
}