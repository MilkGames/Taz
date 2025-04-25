// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : xbDrawprim.h
//   Purpose : render primitives
// Component : Xbox Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BDISPLAY_XBDRAWPRIM_H__
#define __BDISPLAY_XBDRAWPRIM_H__

// ********************************************************************************
// Constants and Macros

// macro to increment the number of polys processed (debugging value only)
#if defined(_DEBUG)
	#define BINCNOOFPOLYSPROCESSED(X) bDisplayInfo.noofPolysProcessed += (X)
#else
	#define BINCNOOFPOLYSPROCESSED(X)
#endif


// Primitive types we can draw
#define BDPRIMTYPE_POINTLIST		0
#define BDPRIMTYPE_LINELIST			1
#define BDPRIMTYPE_LINESTRIP		2
#define BDPRIMTYPE_TRIANGLELIST		3
#define BDPRIMTYPE_TRIANGLESTRIP	4
#define BDPRIMTYPE_TRIANGLEFAN		5
#define BDPRIMTYPE_QUADLIST			6


// ---------------------------------------------
// macros for setting vertex texture coordinates
// ---------------------------------------------
#define BDVERTEX_SETUV(vertex, su, sv)			{ (vertex)->u = (su); (vertex)->v = (sv); }
#define BDVERTEX_SETU(vertex, su)				  (vertex)->u = (su);
#define BDVERTEX_SETV(vertex, sv)				  (vertex)->v = (sv);

#define BDVERTEX_SETUV2(vertex, su, sv)			{ (vertex)->u2 = (su); (vertex)->v2 = (sv); }
#define BDVERTEX_SETU2(vertex, su)				  (vertex)->u2 = (su);
#define BDVERTEX_SETV2(vertex, sv)				  (vertex)->v2 = (sv);

#define BDVERTEX_SETUV3(vertex, su, sv)			{ (vertex)->u3 = (su); (vertex)->v3 = (sv); }
#define BDVERTEX_SETU3(vertex, su)				  (vertex)->u3 = (su);
#define BDVERTEX_SETV3(vertex, sv)				  (vertex)->v3 = (sv);

#define BDVERTEX_SETUV4(vertex, su, sv)			{ (vertex)->u4 = (su); (vertex)->v4 = (sv); }
#define BDVERTEX_SETU4(vertex, su)				  (vertex)->u4 = (su);
#define BDVERTEX_SETV4(vertex, sv)				  (vertex)->v4 = (sv);

#define BDVERTEX_SETUV12(vertex, u1,v1, u2,v2)	{ (vertex)->u = (u1); (vertex)->v = (v1); \
												  (vertex)->u2 = (u2); (vertex)->v2 = (v2); }

#define BDVERTEX_SETUV123(vertex, u1,v1, u2,v2, u3,v3)	{ (vertex)->u = (u1); (vertex)->v = (v1); \
														  (vertex)->u2 = (u2); (vertex)->v2 = (v2); \
														  (vertex)->u3 = (u3); (vertex)->v3 = (v3); }

#define BDVERTEX_SETUV1234(vertex, u1,v1, u2,v2, u3,v3, u4,v4)	{ (vertex)->u = (u1); (vertex)->v = (v1); \
																  (vertex)->u2 = (u2); (vertex)->v2 = (v2); \
																  (vertex)->u3 = (u3); (vertex)->v3 = (v3); \
																  (vertex)->u4 = (u4); (vertex)->v4 = (v4); }

// ---------------------------------
// macros for setting vertex colours
// ---------------------------------
#define BDVERTEX_SETRGBA(vertex, r, g, b, a)	{ (vertex)->colour = (bRColLUT[(int)(r)] | bGColLUT[(int)(g)] | bBColLUT[(int)(b)] | bAColLUT[(int)(a)]); }

#define BDVERTEX_SETR(vertex, r)				{ (vertex)->colour = (((vertex)->colour & 0xff00ffff) | bRColLUT[(int)(r)]); }

#define BDVERTEX_SETG(vertex, g)				{ (vertex)->colour = (((vertex)->colour & 0xffff00ff) | bGColLUT[(int)(g)]); }

#define BDVERTEX_SETB(vertex, b)				{ (vertex)->colour = (((vertex)->colour & 0xffffff00) | bBColLUT[(int)(b)]); }

#define BDVERTEX_SETA(vertex, a)				{ (vertex)->colour = (((vertex)->colour & 0x00ffffff) | bAColLUT[(int)(a)]); }



// -----------------------------------
// macros for setting vertex positions
// -----------------------------------
#define BDVERTEX_SETXYZW(vertex, sx, sy, sz, sw)	{ (vertex)->x = sx; \
													  (vertex)->y = sy; \
												      (vertex)->z = sz; }

#define BDVERTEX_SETXYZWVEC(vertex, vector)			{ (vertex)->x = (vector)[0]; \
													  (vertex)->y = (vector)[1]; \
												      (vertex)->z = (vector)[2]; }

#define BDVERTEX_SETX(vertex, sx)					  (vertex)->x = sx;

#define BDVERTEX_SETY(vertex, sy)					  (vertex)->y = sy;

#define BDVERTEX_SETZ(vertex, sz)					  (vertex)->z = sz;

#define BDVERTEX_SETW(vertex, sw)					  

#define BDVERTEX_SETRHW(vertex, sw)					  (vertex)->rhw = sw;


// ---------------------------------
// macros for setting vertex normals
// ---------------------------------
#define BDVERTEX_SETNORMAL(vertex, x, y, z)		{ (vertex)->xn = x; \
												  (vertex)->yn = y; \
												  (vertex)->zn = z; }

#define BDVERTEX_SETNORMALVEC(vertex, vector)	{ (vertex)->xn = (vector)[0]; \
												  (vertex)->yn = (vector)[1]; \
												  (vertex)->zn = (vector)[2]; }

#define BDVERTEX_SETNORMALX(vertex, x)			  (vertex)->xn = x;

#define BDVERTEX_SETNORMALY(vertex, y)			  (vertex)->yn = y;

#define BDVERTEX_SETNORMALZ(vertex, z)			  (vertex)->zn = z;



// ------------------------------------------------
// macros to retrieve vertex texture mapping coords
// ------------------------------------------------
#define BDVERTEX_GETUV(vertex, su, sv)			{ su = (vertex)->u; sv = (vertex)->v; }
#define BDVERTEX_GETU(vertex, su)				  su = (vertex)->u;
#define BDVERTEX_GETV(vertex, sv)				  sv = (vertex)->v;

#define BDVERTEX_GETUV2(vertex, su, sv)			{ su = (vertex)->u2; sv = (vertex)->v2; }
#define BDVERTEX_GETU2(vertex, su)				  su = (vertex)->u2;
#define BDVERTEX_GETV2(vertex, sv)				  sv = (vertex)->v2;

#define BDVERTEX_GETUV3(vertex, su, sv)			{ su = (vertex)->u3; sv = (vertex)->v3; }
#define BDVERTEX_GETU3(vertex, su)				  su = (vertex)->u3;
#define BDVERTEX_GETV3(vertex, sv)				  sv = (vertex)->v3;

#define BDVERTEX_GETUV4(vertex, su, sv)			{ su = (vertex)->u4; sv = (vertex)->v4; }
#define BDVERTEX_GETU4(vertex, su)				  su = (vertex)->u4;
#define BDVERTEX_GETV4(vertex, sv)				  sv = (vertex)->v4;

#define BDVERTEX_GETUV12(vertex, u1,v1, u2,v2)	{ u1 = (vertex)->u; v1 = (vertex)->v; \
												  u2 = (vertex)->u2; v2 = (vertex)->v2; }

#define BDVERTEX_GETUV123(vertex, u1,v1, u2,v2, u3,v3)	{ u1 = (vertex)->u; v1 = (vertex)->v; \
														  u2 = (vertex)->u2; v2 = (vertex)->v2; \
														  u3 = (vertex)->u3; v3 = (vertex)->v3; }

#define BDVERTEX_GETUV1234(vertex, u1,v1, u2,v2, u3,v3, u4,v4)	{ u1 = (vertex)->u; v1 = (vertex)->v; \
																  u2 = (vertex)->u2; v2 = (vertex)->v2; \
																  u3 = (vertex)->u3; v3 = (vertex)->v3; \
																  u4 = (vertex)->u4; v4 = (vertex)->v4; }

// --------------------------------------------
// macros to retrieve vertex colour information
// --------------------------------------------
#define BDVERTEX_GETRGBA(vertex, r, g, b, a)	{ r = bRGBInvColLUT[((vertex)->colour >> 16) & 0xff]; \
												  g = bRGBInvColLUT[((vertex)->colour >> 8) & 0xff];  \
												  b = bRGBInvColLUT[(vertex)->colour & 0xff];         \
												  a = bAInvColLUT[((vertex)->colour >> 24) & 0xff]; }

#define BDVERTEX_GETR(vertex, r)				  r = bRGBInvColLUT[((vertex)->colour >> 16) & 0xff];

#define BDVERTEX_GETG(vertex, g)				  g = bRGBInvColLUT[((vertex)->colour >> 8) & 0xff];

#define BDVERTEX_GETB(vertex, b)				  b = bRGBInvColLUT[(vertex)->colour & 0xff];

#define BDVERTEX_GETA(vertex, a)				  a = bAInvColLUT[((vertex)->colour >> 24) & 0xff];



// ---------------------------------------------
// macros to retrieve vertex position information
// ---------------------------------------------
#define BDVERTEX_GETXYZW(vertex, sx, sy, sz, sw)	{ sx = (vertex)->x; \
												      sy = (vertex)->y; \
												      sz = (vertex)->z; \
												      sw = 1.0f; }

#define BDVERTEX_GETXYZWVEC(vertex, vector)			{ (vector)[0] = (vertex)->x; \
													  (vector)[1] = (vertex)->y; \
													  (vector)[2] = (vertex)->z; \
													  (vector)[3] = 1.0f; }

#define BDVERTEX_GETX(vertex, sx)					  sx = (vertex)->x;

#define BDVERTEX_GETY(vertex, sy)					  sy = (vertex)->y;

#define BDVERTEX_GETZ(vertex, sz)					  sz = (vertex)->z;

#define BDVERTEX_GETW(vertex, sw)					  sw = 1.0f;

#define BDVERTEX_GETRHW(vertex, sw)					  sw = (vertex)->rhw;


// --------------------------------------------
// macros to retrieve vertex normal information
// --------------------------------------------
#define BDVERTEX_GETNORMAL(vertex, x, y, z)		{ x = (vertex)->xn; \
												  y = (vertex)->yn; \
												  z = (vertex)->zn; }

#define BDVERTEX_GETNORMALVEC(vertex, vector)	{ (vector)[0] = (vertex)->xn; \
												  (vector)[1] = (vertex)->yn; \
												  (vector)[2] = (vertex)->zn; \
												  (vector)[3] = 1.0f; }

#define BDVERTEX_GETNORMALX(vertex, x)			  x = (vertex)->xn;

#define BDVERTEX_GETNORMALY(vertex, y)			  y = (vertex)->yn;

#define BDVERTEX_GETNORMALZ(vertex, z)			  z = (vertex)->zn;


// vertex buffer flags
#define BVERTBUF_READWRITE			0x00000001				// the vertex data will read from as well as written to
#define BVERTBUF_READONLY			0x00000002				// the vertex data will only be read from
#define BVERTBUF_DISCARD			0x00000004				// discard the old contents of the buffer
#define BVERTBUF_APPEND				0x00000008				// we will only be appending to the buffer
#define BVERTBUF_STATIC				0x00000010				// the buffer will not be modified frequently
#define BVERTBUF_NEEDVERTEXSHADER	0x00000020				// these vertices require vertex shader support (PC only)


// index buffer flags
#define BINDEXBUF_READWRITE			0x00000001				// the index data will read from as well as written to
#define BINDEXBUF_READONLY			0x00000002				// the index data will only be read from
#define BINDEXBUF_DISCARD			0x00000004				// discard the old contents of the buffer
#define BINDEXBUF_APPEND			0x00000008				// we will only be appending to the buffer
#define BINDEXBUF_STATIC			0x00000010				// the buffer will not be modified frequently


// single-texture 3D vertex
#define BVERTTYPE_SINGLE		  (	D3DFVF_DIFFUSE |			\
									D3DFVF_NORMAL |				\
									D3DFVF_XYZ |				\
									D3DFVF_TEX1 |				\
									D3DFVF_TEXCOORDSIZE2(0) )

// single-texture 2D vertex (post-transform)
#define BVERTTYPE_SINGLE2D		  (	D3DFVF_DIFFUSE |			\
									D3DFVF_XYZRHW |				\
									D3DFVF_TEX1 |				\
									D3DFVF_TEXCOORDSIZE2(0) )			

// dual-texture 3D vertex
#define BVERTTYPE_DUAL			  (	D3DFVF_DIFFUSE |			\
									D3DFVF_NORMAL |				\
									D3DFVF_XYZ |				\
									D3DFVF_TEX2 |				\
									D3DFVF_TEXCOORDSIZE2(0) |	\
									D3DFVF_TEXCOORDSIZE2(1) )

// dual-texture 2D vertex (post-transform)
#define BVERTTYPE_DUAL2D		  (	D3DFVF_DIFFUSE |			\
									D3DFVF_XYZRHW |				\
									D3DFVF_TEX2 |				\
									D3DFVF_TEXCOORDSIZE2(0) |	\
									D3DFVF_TEXCOORDSIZE2(1) )			

// tri-texture 3D vertex
#define BVERTTYPE_TRI			  (	D3DFVF_DIFFUSE |			\
									D3DFVF_NORMAL |				\
									D3DFVF_XYZ |				\
									D3DFVF_TEX3 |				\
									D3DFVF_TEXCOORDSIZE2(0) |	\
									D3DFVF_TEXCOORDSIZE2(1) |	\
									D3DFVF_TEXCOORDSIZE2(2) )

// tri-texture 2D vertex
#define BVERTTYPE_TRI2D			  (	D3DFVF_DIFFUSE |			\
									D3DFVF_XYZRHW |				\
									D3DFVF_TEX3 |				\
									D3DFVF_TEXCOORDSIZE2(0) |	\
									D3DFVF_TEXCOORDSIZE2(1) |	\
									D3DFVF_TEXCOORDSIZE2(2) )

// quad-texture 3D vertex
#define BVERTTYPE_QUAD			  (	D3DFVF_DIFFUSE |			\
									D3DFVF_NORMAL |				\
									D3DFVF_XYZ |				\
									D3DFVF_TEX4 |				\
									D3DFVF_TEXCOORDSIZE2(0) |	\
									D3DFVF_TEXCOORDSIZE2(1) |	\
									D3DFVF_TEXCOORDSIZE2(2) |	\
									D3DFVF_TEXCOORDSIZE2(3) )

// quad-texture 2D vertex
#define BVERTTYPE_QUAD2D		  (	D3DFVF_DIFFUSE |			\
									D3DFVF_XYZRHW |				\
									D3DFVF_TEX4 |				\
									D3DFVF_TEXCOORDSIZE2(0) |	\
									D3DFVF_TEXCOORDSIZE2(1) |	\
									D3DFVF_TEXCOORDSIZE2(2) |	\
									D3DFVF_TEXCOORDSIZE2(3) )

// Begin specular types
// single-texture 3D vertex
#define BVERTTYPE_SPECULAR_SINGLE (	D3DFVF_DIFFUSE |			\
									D3DFVF_SPECULAR |			\
									D3DFVF_NORMAL |				\
									D3DFVF_XYZ |				\
									D3DFVF_TEX1 |				\
									D3DFVF_TEXCOORDSIZE2(0) )

// single-texture 2D vertex (post-transform)
#define BVERTTYPE_SPECULAR_SINGLE2D	 (	D3DFVF_DIFFUSE |			\
									D3DFVF_XYZRHW |				\
									D3DFVF_TEX1 |				\
									D3DFVF_TEXCOORDSIZE2(0) |	\
									D3DFVF_SPECULAR )			

// dual-texture 3D vertex
#define BVERTTYPE_SPECULAR_DUAL	  (	D3DFVF_DIFFUSE |			\
									D3DFVF_SPECULAR |			\
									D3DFVF_NORMAL |				\
									D3DFVF_XYZ |				\
									D3DFVF_TEX2 |				\
									D3DFVF_TEXCOORDSIZE2(0) |	\
									D3DFVF_TEXCOORDSIZE2(1) )

// dual-texture 2D vertex (post-transform)
#define BVERTTYPE_SPECULAR_DUAL2D (	D3DFVF_DIFFUSE |			\
									D3DFVF_XYZRHW |				\
									D3DFVF_TEX2 |				\
									D3DFVF_TEXCOORDSIZE2(0) |	\
									D3DFVF_TEXCOORDSIZE2(1) |	\
									D3DFVF_SPECULAR )			

// tri-texture 3D vertex
#define BVERTTYPE_SPECULAR_TRI	  (	D3DFVF_DIFFUSE |			\
									D3DFVF_SPECULAR |			\
									D3DFVF_NORMAL |				\
									D3DFVF_XYZ |				\
									D3DFVF_TEX3 |				\
									D3DFVF_TEXCOORDSIZE2(0) |	\
									D3DFVF_TEXCOORDSIZE2(1) |	\
									D3DFVF_TEXCOORDSIZE2(2) )

// tri-texture 2D vertex
#define BVERTTYPE_SPECULAR_TRI2D  (	D3DFVF_DIFFUSE |			\
									D3DFVF_XYZRHW |				\
									D3DFVF_TEX3 |				\
									D3DFVF_TEXCOORDSIZE2(0) |	\
									D3DFVF_TEXCOORDSIZE2(1) |	\
									D3DFVF_TEXCOORDSIZE2(2) |	\
									D3DFVF_SPECULAR )

// quad-texture 3D vertex
#define BVERTTYPE_SPECULAR_QUAD	  (	D3DFVF_DIFFUSE |			\
									D3DFVF_SPECULAR |			\
									D3DFVF_NORMAL |				\
									D3DFVF_XYZ |				\
									D3DFVF_TEX4 |				\
									D3DFVF_TEXCOORDSIZE2(0) |	\
									D3DFVF_TEXCOORDSIZE2(1) |	\
									D3DFVF_TEXCOORDSIZE2(2) |	\
									D3DFVF_TEXCOORDSIZE2(3) )

// quad-texture 2D vertex
#define BVERTTYPE_SPECULAR_QUAD2D  (	D3DFVF_DIFFUSE |			\
									D3DFVF_XYZRHW |				\
									D3DFVF_TEX4 |				\
									D3DFVF_TEXCOORDSIZE2(0) |	\
									D3DFVF_TEXCOORDSIZE2(1) |	\
									D3DFVF_TEXCOORDSIZE2(2) |	\
									D3DFVF_TEXCOORDSIZE2(3) |	\
									D3DFVF_SPECULAR )
// End specular types

// three-matrix blended single-texture 3D vertex
#define BVERTTYPE_BLENDEDSINGLE	  (	D3DFVF_DIFFUSE |			\
									D3DFVF_XYZB4 |				\
									D3DFVF_NORMAL |				\
									D3DFVF_TEX1 |				\
									D3DFVF_TEXCOORDSIZE2(0) )

// three-matrix blended dual-texture 3D vertex
#define BVERTTYPE_BLENDEDDUAL	  (	D3DFVF_DIFFUSE |			\
									D3DFVF_XYZB4 |				\
									D3DFVF_NORMAL |				\
									D3DFVF_TEX2 |				\
									D3DFVF_TEXCOORDSIZE2(0) |	\
									D3DFVF_TEXCOORDSIZE2(1) )

// three-matrix blended tri-texture 3D vertex
#define BVERTTYPE_BLENDEDTRI	  (	D3DFVF_DIFFUSE |			\
									D3DFVF_XYZB4 |				\
									D3DFVF_NORMAL |				\
									D3DFVF_TEX3 |				\
									D3DFVF_TEXCOORDSIZE2(0) |	\
									D3DFVF_TEXCOORDSIZE2(1) |	\
									D3DFVF_TEXCOORDSIZE2(2) )

// three-matrix blended quad-texture 3D vertex
#define BVERTTYPE_BLENDEDQUAD	  (	D3DFVF_DIFFUSE |			\
									D3DFVF_XYZB4 |				\
									D3DFVF_NORMAL |				\
									D3DFVF_TEX4 |				\
									D3DFVF_TEXCOORDSIZE2(0) |	\
									D3DFVF_TEXCOORDSIZE2(1) |	\
									D3DFVF_TEXCOORDSIZE2(2) |	\
									D3DFVF_TEXCOORDSIZE2(3) )


// vertices for rendering point sprites.
#define BVERTTYPE_POINTSPRITE	  ( D3DFVF_XYZ |				\
									D3DFVF_DIFFUSE )


// maximum number of vertex/index buffers in a chain
#define BMAXVIBBUFFERS			8


// ********************************************************************************
// Types, Structures and Classes


// BVERTTYPE_SINGLE vertex structure
#pragma pack(push, 4)
typedef struct _TBPrimVertex {
	float		x,y,z;				// position
	float		xn, yn, zn;			// normal
	uint32		colour;				// diffuse colour
	float		u,v;				// texture coordinates
} TBPrimVertex;
#pragma pack(pop)


// BVERTTYPE_SINGLE2D vertex structure
#pragma pack(push, 4)
typedef struct _TBPrimVertex2D {
	float		x,y,z;				// position
	float		rhw;				// 1/w
	uint32		colour;				// diffuse colour
	float		u,v;				// texture coordinates
} TBPrimVertex2D;
#pragma pack(pop)


// BVERTTYPE_DUAL vertex structure
#pragma pack(push, 4)
typedef struct _TBDualPrimVertex {
	float		x,y,z;				// position
	float		xn, yn, zn;			// normal
	uint32		colour;				// diffuse colour
	float		u,v;				// texture coordinate set 0
	float		u2,v2;				// texture coordinate set 1
} TBDualPrimVertex;
#pragma pack(pop)


// BVERTTYPE_DUAL2D vertex structure
#pragma pack(push, 4)
typedef struct _TBDualPrimVertex2D {
	float		x,y,z;				// position
	float		rhw;				// 1/w
	uint32		colour;				// diffuse colour
	float		u,v;				// texture coordinate set 0
	float		u2,v2;				// texture coordinate set 1
} TBDualPrimVertex2D;
#pragma pack(pop)


// BVERTTYPE_TRI vertex structure
#pragma pack(push, 4)
typedef struct _TBTriPrimVertex {
	float		x,y,z;				// position
	float		xn, yn, zn;			// normal
	uint32		colour;				// diffuse colour
	float		u,v;				// texture coordinate set 0
	float		u2,v2;				// texture coordinate set 1
	float		u3,v3;				// texture coordinate set 2
} TBTriPrimVertex;
#pragma pack(pop)


// BVERTTYPE_TRI2D vertex structure
#pragma pack(push, 4)
typedef struct _TBTriPrimVertex2D {
	float		x,y,z;				// position
	float		rhw;				// 1/w
	uint32		colour;				// diffuse colour
	float		u,v;				// texture coordinate set 0
	float		u2,v2;				// texture coordinate set 1
	float		u3,v3;				// texture coordinate set 2
} TBTriPrimVertex2D;
#pragma pack(pop)


// BVERTTYPE_QUAD vertex structure
#pragma pack(push, 4)
typedef struct _TBQuadPrimVertex {
	float		x,y,z;				// position
	float		xn, yn, zn;			// normal
	uint32		colour;				// diffuse colour
	float		u,v;				// texture coordinate set 0
	float		u2,v2;				// texture coordinate set 1
	float		u3,v3;				// texture coordinate set 2
	float		u4,v4;				// texture coordinate set 3
} TBQuadPrimVertex;
#pragma pack(pop)


// BVERTTYPE_QUAD2D vertex structure
#pragma pack(push, 4)
typedef struct _TBQuadPrimVertex2D {
	float		x,y,z;				// position
	float		rhw;				// 1/w
	uint32		colour;				// diffuse colour
	float		u,v;				// texture coordinate set 0
	float		u2,v2;				// texture coordinate set 1
	float		u3,v3;				// texture coordinate set 2
	float		u4,v4;				// texture coordinate set 3
} TBQuadPrimVertex2D;
#pragma pack(pop)

// Specular versions
// BVERTTYPE_SINGLE vertex structure
#pragma pack(push, 4)
typedef struct _TBSpecularPrimVertex {
	float		x,y,z;				// position
	float		xn, yn, zn;			// normal
	uint32		colour;				// diffuse colour
	uint32		specular;			// specular colour
	float		u,v;				// texture coordinates
} TBSpecularPrimVertex;
#pragma pack(pop)


// BVERTTYPE_SINGLE2D vertex structure
#pragma pack(push, 4)
typedef struct _TBSpecularPrimVertex2D {
	float		x,y,z;				// position
	float		rhw;				// 1/w
	uint32		colour;				// diffuse colour
	uint32		specular;			// specular colour
	float		u,v;				// texture coordinates
} TBSpecularPrimVertex2D;
#pragma pack(pop)


// BVERTTYPE_DUAL vertex structure
#pragma pack(push, 4)
typedef struct _TBSpecularDualPrimVertex {
	float		x,y,z;				// position
	float		xn, yn, zn;			// normal
	uint32		colour;				// diffuse colour
	uint32		specular;			// specular colour
	float		u,v;				// texture coordinate set 0
	float		u2,v2;				// texture coordinate set 1
} TBSpecularDualPrimVertex;
#pragma pack(pop)


// BVERTTYPE_DUAL2D vertex structure
#pragma pack(push, 4)
typedef struct _TBSpecularDualPrimVertex2D {
	float		x,y,z;				// position
	float		rhw;				// 1/w
	uint32		colour;				// diffuse colour
	uint32		specular;			// specular colour
	float		u,v;				// texture coordinate set 0
	float		u2,v2;				// texture coordinate set 1
} TBSpecularDualPrimVertex2D;
#pragma pack(pop)


// BVERTTYPE_TRI vertex structure
#pragma pack(push, 4)
typedef struct _TBSpecularTriPrimVertex {
	float		x,y,z;				// position
	float		xn, yn, zn;			// normal
	uint32		colour;				// diffuse colour
	uint32		specular;			// specular colour
	float		u,v;				// texture coordinate set 0
	float		u2,v2;				// texture coordinate set 1
	float		u3,v3;				// texture coordinate set 2
} TBSpecularTriPrimVertex;
#pragma pack(pop)


// BVERTTYPE_TRI2D vertex structure
#pragma pack(push, 4)
typedef struct _TBSpecularTriPrimVertex2D {
	float		x,y,z;				// position
	float		rhw;				// 1/w
	uint32		colour;				// diffuse colour
	uint32		specular;			// specular colour
	float		u,v;				// texture coordinate set 0
	float		u2,v2;				// texture coordinate set 1
	float		u3,v3;				// texture coordinate set 2
} TBSpecularTriPrimVertex2D;
#pragma pack(pop)


// BVERTTYPE_QUAD vertex structure
#pragma pack(push, 4)
typedef struct _TBSpecularQuadPrimVertex {
	float		x,y,z;				// position
	float		xn, yn, zn;			// normal
	uint32		colour;				// diffuse colour
	uint32		specular;			// specular colour
	float		u,v;				// texture coordinate set 0
	float		u2,v2;				// texture coordinate set 1
	float		u3,v3;				// texture coordinate set 2
	float		u4,v4;				// texture coordinate set 3
} TBSpecularQuadPrimVertex;
#pragma pack(pop)


// BVERTTYPE_QUAD2D vertex structure
#pragma pack(push, 4)
typedef struct _TBSpecularQuadPrimVertex2D {
	float		x,y,z;				// position
	float		rhw;				// 1/w
	uint32		colour;				// diffuse colour
	uint32		specular;			// specular colour
	float		u,v;				// texture coordinate set 0
	float		u2,v2;				// texture coordinate set 1
	float		u3,v3;				// texture coordinate set 2
	float		u4,v4;				// texture coordinate set 3
} TBSpecularQuadPrimVertex2D;
#pragma pack(pop)
// End specular versions

// BVERTTYPE_BLENDEDSINGLE vertex structure
#pragma pack(push, 4)
typedef struct _TBBlendedPrimVertex {
	float		x,y,z;				// position
	float		weight[3];			// blending beta-weights
	ushort		matrixIndex[4];		// matrix indices
	float		xn, yn, zn;			// normal
	uint32		colour;				// diffuse colour
	float		u,v;				// texture coordinates
} TBBlendedPrimVertex;
#pragma pack(pop)


// BVERTTYPE_BLENDEDDUAL vertex structure
#pragma pack(push, 4)
typedef struct _TBDualBlendedPrimVertex {
	float		x,y,z;				// position
	float		weight[3];			// blending beta-weights
	ushort		matrixIndex[4];		// matrix indices
	float		xn, yn, zn;			// normal
	uint32		colour;				// diffuse colour
	float		u,v;				// texture coordinate set 0
	float		u2,v2;				// texture coordinate set 1
} TBDualBlendedPrimVertex;
#pragma pack(pop)


// BVERTTYPE_BLENDEDTRI vertex structure
#pragma pack(push, 4)
typedef struct _TBTriBlendedPrimVertex {
	float		x,y,z;				// position
	float		weight[3];			// blending beta-weights
	ushort		matrixIndex[4];		// matrix indices
	float		xn, yn, zn;			// normal
	uint32		colour;				// diffuse colour
	float		u,v;				// texture coordinate set 0
	float		u2,v2;				// texture coordinate set 1
	float		u3,v3;				// texture coordinate set 2
} TBTriBlendedPrimVertex;
#pragma pack(pop)


// BVERTTYPE_BLENDEDQUAD vertex structure
#pragma pack(push, 4)
typedef struct _TBQuadBlendedPrimVertex {
	float		x,y,z;				// position
	float		weight[3];			// blending beta-weights
	ushort		matrixIndex[4];		// matrix indices
	float		xn, yn, zn;			// normal
	uint32		colour;				// diffuse colour
	float		u,v;				// texture coordinate set 0
	float		u2,v2;				// texture coordinate set 1
	float		u3,v3;				// texture coordinate set 2
	float		u4,v4;				// texture coordinate set 3
} TBQuadBlendedPrimVertex;
#pragma pack(pop)


// BVERTTYPE_POINTSPRITE vertex structure
#pragma pack(push, 4)
typedef struct _TBPointSpritePrimVertex {
	float		x,y,z;				// position
	uint32		colour;				// diffuse colour
} TBPointSpritePrimVertex;
#pragma pack(pop)


// a permanent vertex buffer
typedef struct _TBVertexBuffer {
	D3DVertexBuffer				*vertexBuffer[BMAXVIBBUFFERS];	// D3D vertex buffer object (follow struct in memory)

	int							noofVertices;					// #vertices
	int							vertexSize;						// size of a vertex
	uint32						flags;							// flags
	int							vertexType;						// type of vertex

	struct _TBVertexBuffer		*next, *prev;					// list links
	int							noofBuffers;					// how many buffers in this chain
	int							nextLock;						// buffer to be locked next

	int							lastLock;						// buffer that was locked last
	void						*d3dData;						// D3D data block
	void						*lastDataPtr;					// ptr from last lock
} TBVertexBuffer;


// a permanent index buffer
typedef struct _TBIndexBuffer {
	int							noofIndices;			// #indices
	int							indexBits;				// size of an index in bits (16 or 32)
	D3DFORMAT					indexFormat;			// D3D index format
	struct _TBIndexBuffer		*next, *prev;			// list links
	D3DIndexBuffer				d3dIdxBuffer;			// D3D index buffer
} TBIndexBuffer;


// ********************************************************************************
// Globals

extern TBVertexBuffer			bVertexBufferList;			// vertex buffer list
extern int						bCurrentVertexShader;		// current vertex shader ID (BVERTTYPE_)
extern TBVertexBuffer			*bLastVertexBuffer;			// ptr to last set vertex buffer
extern TBIndexBuffer			bIndexBufferList;			// index buffer list


// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : bdDrawPrimitive
   Purpose : draw a primitive
   Parameters : primitive type, vertex array, number of vertices, vertex type
   Returns : 
   Info : 
*/

void bdDrawPrimitive(uint32 primType, void *vertices, int vertCount, uint vertType);
void bdDrawPrimitive(uint32 primType, TBPrimVertex *vertices, int vertCount);
void bdDrawPrimitive(uint32 primType, TBPrimVertex2D *vertices, int vertCount);
void bdDrawPrimitive(uint32 primType, TBDualPrimVertex *vertices, int vertCount);
void bdDrawPrimitive(uint32 primType, TBDualPrimVertex2D *vertices, int vertCount);
void bdDrawPrimitive(uint32 primType, TBTriPrimVertex *vertices, int vertCount);
void bdDrawPrimitive(uint32 primType, TBTriPrimVertex2D *vertices, int vertCount);
void bdDrawPrimitive(uint32 primType, TBQuadPrimVertex *vertices, int vertCount);
void bdDrawPrimitive(uint32 primType, TBQuadPrimVertex2D *vertices, int vertCount);
void bdDrawPrimitive(uint32 primType, TBSpecularPrimVertex *vertices, int vertCount);
void bdDrawPrimitive(uint32 primType, TBSpecularPrimVertex2D *vertices, int vertCount);
void bdDrawPrimitive(uint32 primType, TBSpecularDualPrimVertex *vertices, int vertCount);
void bdDrawPrimitive(uint32 primType, TBSpecularDualPrimVertex2D *vertices, int vertCount);
void bdDrawPrimitive(uint32 primType, TBSpecularTriPrimVertex *vertices, int vertCount);
void bdDrawPrimitive(uint32 primType, TBSpecularTriPrimVertex2D *vertices, int vertCount);
void bdDrawPrimitive(uint32 primType, TBSpecularQuadPrimVertex *vertices, int vertCount);
void bdDrawPrimitive(uint32 primType, TBSpecularQuadPrimVertex2D *vertices, int vertCount);


/* --------------------------------------------------------------------------------
   Function : bdDrawPrimitiveIndexed
   Purpose : draw a primitive from an indexed array of vertices
   Parameters : primitive type, vertex array, number of vertices, index array, number of indices, vertex type
   Returns : 
   Info : 
*/

void bdDrawPrimitiveIndexed(uint32 primType, void *vertices, int vertCount, ushort *indices, int indexCount, uint vertType);


/* --------------------------------------------------------------------------------
   Function : bdDrawPrimitiveVB
   Purpose : draw a primitive from a vertex buffer
   Parameters : primitive type, vertex buffer, first vertex to use, number of vertices to use
   Returns : 
   Info : 
*/

void bdDrawPrimitiveVB(uint32 primType, TBVertexBuffer *vbPtr, int firstVertex, int vertCount);


/* --------------------------------------------------------------------------------
   Function : bdDrawPrimitiveIndexedVB
   Purpose : draw an indexed primitive from a vertex buffer
   Parameters : primitive type, vertex buffer, first vertex to use, number of vertices, index array, number of indices
   Returns : 
   Info : 
*/

void bdDrawPrimitiveIndexedVB(uint32 primType, TBVertexBuffer *vbPtr, int firstVertex, int vertCount, ushort *indices, int indexCount);


/* --------------------------------------------------------------------------------
   Function : bdVertexBufferCreate
   Purpose : create a new vertex buffer
   Parameters : ptr to fill with vertex buffer ptr, #vertices, vertex type, flags, #buffers
   Returns : OK/FAIL
   Info : 
*/

int bdVertexBufferCreate(TBVertexBuffer **vbPtr, int32 noofVertices, uint vertexType, uint32 flags, int noofBuffers=1);


/* --------------------------------------------------------------------------------
   Function : bdVertexBufferDestroy
   Purpose : destroy a vertex buffer
   Parameters : ptr to vertex buffer
   Returns : 
   Info : 
*/

void bdVertexBufferDestroy(TBVertexBuffer *vbPtr);


/* --------------------------------------------------------------------------------
   Function : bdVertexBufferLock
   Purpose : lock a vertex buffer for access
   Parameters : ptr to vertex buffer, flags
   Returns : ptr to vertex data or NULL for failure
   Info : 
*/

void *bdVertexBufferLock(TBVertexBuffer *vbPtr, uint32 flags);


/* --------------------------------------------------------------------------------
   Function : bdVertexBufferUnlock
   Purpose : unlock a vertex buffer
   Parameters : ptr to vertex buffer
   Returns : 
   Info : 
*/

void bdVertexBufferUnlock(TBVertexBuffer *vbPtr);


/* --------------------------------------------------------------------------------
   Function : bdVertexBufferGetSize
   Purpose : return the size of a vertex buffer
   Parameters : ptr to vertex buffer
   Returns : #verts that can fit in buffer
   Info : 
*/

int bdVertexBufferGetSize(TBVertexBuffer *vbPtr);


/* --------------------------------------------------------------------------------
   Function : bdDrawFlatSprite
   Purpose : draw a camera-aligned sprite
   Parameters : object-space centre-point, xDim, yDim, texture, uFlip?, vFlip, colour, dest verts or NULL, zBias
   Returns : OK=valid, FAIL=clipped
   Info : 
*/

int bdDrawFlatSprite(TBVector centre, float xDim, float yDim, float ang, struct _TBTexture *texture, int32 uFlip, int32 vFlip,
					 int r,int g,int b,int a, TBPrimVertex2D *destVerts, float zBias);


/* --------------------------------------------------------------------------------
   Function : bdDrawGouraudSprite
   Purpose : draw a camera-aligned sprite
   Parameters : object-space centre-point, xDim, yDim, texture, uFlip?, vFlip, colours, dest verts or NULL, zBias
   Returns : OK=valid, FAIL=clipped
   Info : 
*/

int bdDrawGouraudSprite(TBVector centre, float xDim, float yDim, float ang, struct _TBTexture *texture, int32 uFlip, int32 vFlip,
						int *colour, TBPrimVertex2D *destVerts, float zBias);


/* --------------------------------------------------------------------------------
   Function : bPrimCount
   Purpose : return the prim count from a prim type and vertex count
   Parameters : primitive type, number of vertices
   Returns : prim count
   Info : 
*/

int bPrimCount(uint32 primType, int vertCount);


/* --------------------------------------------------------------------------------
   Function : bdDrawFixedFlatSprite
   Purpose : draw a camera-aligned sprite of constant size
   Parameters : object-space centre-point, xDim, yDim, texture, uFlip?, vFlip, colour, dest verts or NULL, zBias
   Returns : OK=valid, FAIL=clipped
   Info : dimensions are in pixels
*/

int bdDrawFixedFlatSprite(TBVector centre, float xDim, float yDim, float ang, struct _TBTexture *texture, int32 uFlip,
						  int32 vFlip, int r,int g,int b,int a, TBPrimVertex2D *destVerts, float zBias);


/* --------------------------------------------------------------------------------
   Function : bdDrawFixedGouraudSprite
   Purpose : draw a camera-aligned sprite of constant size
   Parameters : object-space centre-point, xDim, yDim, texture, uFlip?, vFlip, colours, dest verts or NULL, zBias
   Returns : OK=valid, FAIL=clipped
   Info : dimensions are in pixels
*/

int bdDrawFixedGouraudSprite(TBVector centre, float xDim, float yDim, float ang, struct _TBTexture *texture, int32 uFlip,
							 int32 vFlip, int *colour, TBPrimVertex2D *destVerts, float zBias);


/* --------------------------------------------------------------------------------
   Function : bdDrawStreak
   Purpose : draw a camera-aligned streak-sprite
   Parameters : start point, start (xDim,yDim), start colour, end point, end (xDim,yDim), end colour, texture, 
				dest verts or NULL, zBias
   Returns : OK=valid, FAIL=clipped
   Info : positions are object space, dimensions are in world space
*/

int bdDrawStreak(TBVector startPos, float xStartDim, float yStartDim, int *startColour, TBVector endPos,
					  float xEndDim, float yEndDim, int *endColour, struct _TBTexture *texture, TBPrimVertex2D *destVerts,
					  float zBias);


/* --------------------------------------------------------------------------------
   Function : bdDrawStreakClipped
   Purpose : draw a camera-aligned streak-sprite, clipped to projection plane
   Parameters : start point, start (xDim,yDim), start colour, end point, end (xDim,yDim), end colour, texture, 
				dest verts or NULL, zBias
   Returns : OK=valid, FAIL=clipped
   Info : positions are object space, dimensions are in world space
*/

int bdDrawStreakClipped(TBVector startPos, float xStartDim, float yStartDim, int *startColour, TBVector endPos,
					  float xEndDim, float yEndDim, int *endColour, struct _TBTexture *texture, TBPrimVertex2D *destVerts,
					  float zBias);


/* --------------------------------------------------------------------------------
   Function : bdDrawFixedStreak
   Purpose : draw a camera-aligned fixed size streak-sprite
   Parameters : start point, start (xDim,yDim), start colour, end point, end (xDim,yDim), end colour, texture,
				dest verts or NULL, zBias
   Returns : OK=valid, FAIL=clipped
   Info : positions are object space, dimensions are in pixels
*/

int bdDrawFixedStreak(TBVector startPos, float xStartDim, float yStartDim, int *startColour, TBVector endPos,
					  float xEndDim, float yEndDim, int *endColour, struct _TBTexture *texture, TBPrimVertex2D *destVerts,
					  float zBias);


/* --------------------------------------------------------------------------------
   Function : bdDrawMultiStreak
   Purpose : draw a camera-aligned multi-streak-sprite
   Parameters : noof points, point array, size array, screen size work array, colour array, dest verts, zBias
   Returns : number of verts used (can be zero)
   Info : positions are object space, dimensions are in world space,
		  all points must be within near/far limits or no drawing will result
*/

int bdDrawMultiStreak(int noofPoints, TBVector pointArray, float *sizeArray, float *screenSizeArray,
					  int *colourArray, TBPrimVertex2D *destVerts, float zBias);


/* --------------------------------------------------------------------------------
   Function : bdDrawMultiStreakClipped
   Purpose : draw a camera-aligned multi-streak-sprite
   Parameters : noof points, point array, size array, screen size work array, colour array, dest verts, zBias
   Returns : number of verts used (can be zero)
   Info : positions are object space, dimensions are in world space,
*/

int bdDrawMultiStreakClipped(int noofPoints, TBVector pointArray, float *sizeArray, float *screenSizeArray, int *colourArray,
					  TBPrimVertex2D *destVerts, float zBias);


/* --------------------------------------------------------------------------------
   Function : bdDrawFixedMultiStreak
   Purpose : draw a camera-aligned fixed size multi-streak-sprite
   Parameters : noof points, point array, size array, colour array, dest verts, zBias
   Returns : number of verts used (can be zero)
   Info : positions are object space, dimensions are in screen space,
		  all points must be within near/far limits or no drawing will result
*/

int bdDrawFixedMultiStreak(int noofPoints, TBVector pointArray, float *sizeArray, int *colourArray,
						   TBPrimVertex2D *destVerts, float zBias);


/* --------------------------------------------------------------------------------
   Function : bSetVertexShader
   Purpose : set the current vertex shader type
   Parameters : type of vertex shader to set, ptr to vertex buffer to set or NULL for none
   Returns : 
   Info : 
*/

void bSetVertexShader(int type, TBVertexBuffer *vertexBuffer);


/* --------------------------------------------------------------------------------
   Function : bdGetPolyCount
   Purpose : return the approximate number of triangles draw this frame so far
   Parameters : 
   Returns : 
   Info : 
*/

int bdGetPolyCount();


/* --------------------------------------------------------------------------------
   Function : bGetVertexSize
   Purpose : determine the size of a vertex type
   Parameters : vertex type
   Returns : vertex size
   Info : 
*/

int bGetVertexSize(int vertType);


/* --------------------------------------------------------------------------------
   Function : bdIndexBufferCreate
   Purpose : create a new index buffer
   Parameters : ptr to fill with index buffer ptr, #indices, flags (see BINDEXBUF_)
   Returns : OK/FAIL
   Info : 
*/

int bdIndexBufferCreate(TBIndexBuffer **ibPtr, int noofIndices, uint32 flags);


/* --------------------------------------------------------------------------------
   Function : bdIndexBufferDestroy
   Purpose : destroy a index buffer
   Parameters : ptr to index buffer
   Returns : 
   Info : 
*/

void bdIndexBufferDestroy(TBIndexBuffer *ibPtr);


/* --------------------------------------------------------------------------------
   Function : bdIndexBufferLock
   Purpose : lock an index buffer for access
   Parameters : ptr to index buffer, flags (see BINDEXBUF_)
   Returns : ptr to index data or NULL for failure
   Info : 
*/

void *bdIndexBufferLock(TBIndexBuffer *ibPtr, uint32 flags);


/* --------------------------------------------------------------------------------
   Function : bdIndexBufferUnlock
   Purpose : unlock an index buffer
   Parameters : ptr to index buffer
   Returns : 
   Info : 
*/

void bdIndexBufferUnlock(TBIndexBuffer *ibPtr);


/* --------------------------------------------------------------------------------
   Function : bdIndexBufferGetSize
   Purpose : return the size of an index buffer
   Parameters : ptr to index buffer
   Returns : #indices that can fit in buffer
   Info : 
*/

int bdIndexBufferGetSize(TBIndexBuffer *ibPtr);


/*	--------------------------------------------------------------------------------
	Function : bdDrawPointSpriteVB
	Purpose : Render a vertex buffer of points as a point sprite buffer. The points are rendered with the current texture.
	Parameters : [in] the point buffer to render
				[in] the starting vertex in the buffer to render.
				[in] the number of vertices in the buffer to render.
				[in] the size of the points to render.
	Returns : 
	Info : 
*/

void bdDrawPointSpriteVB(TBVertexBuffer *buffer, const int startVertex, const int vertexCount, const float pointSize);


/* --------------------------------------------------------------------------------
   Function : bdSetIndexBuffer
   Purpose : set an index buffer as current
   Parameters : ptr to index buffer (NULL to set none), base vertex offset
   Returns : 
   Info : 
*/

void bdSetIndexBuffer(TBIndexBuffer *ibPtr, int baseIndex=0);


/* --------------------------------------------------------------------------------
   Function : bdSetVertexBuffer
   Purpose : set a vertex buffer as current
   Parameters : ptr to vertex buffer (NULL to set none)
   Returns : 
   Info : 
*/

void bdSetVertexBuffer(TBVertexBuffer *vbPtr);

#endif		// __BDISPLAY_XBDRAWPRIM_H__
