// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2001 Blitz Games Ltd.
//
//      File : CBAnimSplash.h
//   Purpose : animated splash screen base class
// Component : Generic BFC Effects
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __FEFFECTS_CBANIMSPLASH_H__
#define __FEFFECTS_CBANIMSPLASH_H__

// ********************************************************************************
// Constants and macros

// maximum processors on a single animated splash instance
#define BMAXANIMSPLASHFX			8

// attrib IDs
typedef enum EBAnimSplashAttrib {
	EBANIMSPLASHATTRIB_POSITION,
	EBANIMSPLASHATTRIB_DIFFUSE,
	EBANIMSPLASHATTRIB_SPECULAR,
	EBANIMSPLASHATTRIB_UV,

	EBANIMSPLASHATTRIB_NOOF
};

// attrib mask flags
#define BANIMSPLASHATTRIBMASK_POSITION		0x00000001
#define BANIMSPLASHATTRIBMASK_DIFFUSE_R		0x00000002
#define BANIMSPLASHATTRIBMASK_DIFFUSE_G		0x00000004
#define BANIMSPLASHATTRIBMASK_DIFFUSE_B		0x00000008
#define BANIMSPLASHATTRIBMASK_DIFFUSE_A		0x00000010
#define BANIMSPLASHATTRIBMASK_SPECULAR_R	0x00000020
#define BANIMSPLASHATTRIBMASK_SPECULAR_G	0x00000040
#define BANIMSPLASHATTRIBMASK_SPECULAR_B	0x00000080
#define BANIMSPLASHATTRIBMASK_SPECULAR_A	0x00000100
#define BANIMSPLASHATTRIBMASK_UV			0x00000200


// ********************************************************************************
// Types, Structures and Classes

// base class for anim splash FX processors
typedef class CBAnimSplashFX {
	private:
		int							refCount;							// reference count for this instance

	public:

									// constructor
									CBAnimSplashFX() { refCount = 1; }

									// destructor
		virtual						~CBAnimSplashFX() { };

									// enter the effect - sets view and render states
		virtual void				Enter() { };

									// exit the effect - tidies up render states
		virtual void				Exit() { };

									// process the attrib buffer
		virtual void				Process(float time, class CBAnimSplash *splash) = 0;

									// return a mask of which attributes this effect sets
		virtual uint32				SetAttribMask() = 0;

									// return a mask of which attributes this effect modifies
		virtual uint32				ModAttribMask() = 0;

									// increment this instance's reference count
		int							AddRef() { return ++refCount; };

									// decrement this instance's reference count
		int							Release() { int ref = --refCount; if (refCount == 0) delete this; return ref; }
} CBAnimSplashFX;


// a single vertex grid attribute
typedef struct {
	float							x, y, z;					// position
	uchar							rd,gd,bd, ad;				// diffuse colour
	uchar							rs,gs,bs, as;				// specular colour
	float							u, v;						// texture coords (normalised over visible texture area)
	float							s, t;						// normalised attribute screen pos
} TBAnimSplashAttrib;


// a fragment
typedef struct {
	int								noofAttribs;				// number of attribs in this fragment
	int								firstAttrib;				// index of first attribute
} TBAnimSplashFragment;


// base class for fragment generators
typedef class CBAnimSplashFragGen {
	private:
		int							refCount;

	public:
									// constructor
									CBAnimSplashFragGen() { refCount = 1; }

									// destructor
		virtual						~CBAnimSplashFragGen() { };

									// generate fragments
		virtual int					MakeFragments(class CBAnimSplash *splash) = 0;

									// increment this instance's reference count
		int							AddRef() { return ++refCount; };

									// decrement this instance's reference count
		int							Release() { int ref = --refCount; if (refCount == 0) delete this; return ref; }
} CBAnimSplashFragGen;


// animated splash screen base class
typedef class CBAnimSplash {
	private:
									// setup default positions for the attributes
		void						SetDefaultPositions();

									// setup default diffuse colours for the attributes
		void						SetDefaultDiffuseRed();
		void						SetDefaultDiffuseGreen();
		void						SetDefaultDiffuseBlue();
		void						SetDefaultDiffuseAlpha();

									// setup default specular colours for the attributes
		void						SetDefaultSpecularRed();
		void						SetDefaultSpecularGreen();
		void						SetDefaultSpecularBlue();
		void						SetDefaultSpecularAlpha();

									// setup default UV coords for the attributes
		void						SetDefaultUV();

	public:
		UBContext32					context;						// client context value

		CBAnimSplashFragGen			*fragGen;						// fragment generator
		CBAnimSplashFX				*fx[BMAXANIMSPLASHFX];			// array of FX processors
		int							noofFx;							// number of processors in above array
		TBTexture					*texture;						// texture we are using
		float						uVisTex, vVisTex;				// factor to scale normalised (u,v) to visible area of texture
		int							entered;						// TRUE if inside an enter/exit block
		uint32						setMask;						// mask of attributes that are set by a processor
		uint32						modMask;						// mask of attributes that are modified by a processor

		TBAnimSplashAttrib			*attribs;						// attribute array
		int							noofAttribs;					// #attributes
		TBAnimSplashFragment		*fragments;						// fragment array
		int							noofFragments;					// #fragments

		CBAnimSplashRenderer		renderer;						// animated splash screen renderer
	
									// constructor
									CBAnimSplash(TBTexture *texturePtr, CBAnimSplashFragGen *fragGenPtr);

									// destructor
									~CBAnimSplash();

									// set the visible texture area
		void						SetVisibleArea(int texWidth, int texHeight);

									// render this animated splash screen in it's current state
		void						Render();

									// set the animation to a time value [0..1]
		void						SetAnim(float time, int render=TRUE);

									// enter the effect - creates fragments, sets view and render states
		int							Enter();

									// exit the effect - tidies up render states and free's fragments
		void						Exit();

									// add an effects processor to this animated splash
		int							AddFX(CBAnimSplashFX *fxPtr);

									// allocate attribute/fragment buffer
		int							Alloc(int noofAttribs, int noofFragments);
} CBAnimSplash;



// ********************************************************************************
// stock fragment generators
// ********************************************************************************

typedef class CBAnimSplashFragGen_Grid : public CBAnimSplashFragGen {
	private:
		int							xRes, yRes, xBreak;

	public:
									// constructor
									CBAnimSplashFragGen_Grid(int xGridRes, int yGridRes, int xBreak=FALSE);

									// destructor
		virtual						~CBAnimSplashFragGen_Grid();

									// generate fragments
		virtual int					MakeFragments(CBAnimSplash *splash);
} CBAnimSplashFragGen_Grid;


// ********************************************************************************
// stock FX processors
// ********************************************************************************

// actions the fade effect can perform on each component
typedef enum EBAnimSplashFXFade {
	EBANIMSPLASHFXFADE_SET,
	EBANIMSPLASHFXFADE_CUR,
	EBANIMSPLASHFXFADE_PASS,
};

// fade the vertices diffuse colour
typedef class CBAnimSplashFX_Fade : public CBAnimSplashFX {
	public:
		EBAnimSplashFXFade			r,g,b, a;					// what to do with each component

									// constructor
									CBAnimSplashFX_Fade(EBAnimSplashFXFade rFade=EBANIMSPLASHFXFADE_SET,
														EBAnimSplashFXFade gFade=EBANIMSPLASHFXFADE_SET,
														EBAnimSplashFXFade bFade=EBANIMSPLASHFXFADE_SET,
														EBAnimSplashFXFade aFade=EBANIMSPLASHFXFADE_PASS);

									// destructor
		virtual						~CBAnimSplashFX_Fade() { }

									// enter the effect
		virtual void				Enter();

									// exit the effect
		virtual void				Exit();

									// process the attrib buffer
		virtual void				Process(float time, CBAnimSplash *splash);

									// return a mask of which attributes this effect sets
		virtual uint32				SetAttribMask();

									// return a mask of which attributes this effect modifies
		virtual uint32				ModAttribMask();
} CBAnimSplashFX_Fade;


// slide in the fragments from the edges of the screen
typedef class CBAnimSplashFX_SlideOn : public CBAnimSplashFX {
	public:
									// constructor
									CBAnimSplashFX_SlideOn();

									// destructor
		virtual						~CBAnimSplashFX_SlideOn() { }

									// enter the effect
		virtual void				Enter();

									// exit the effect
		virtual void				Exit();

									// process the attrib buffer
		virtual void				Process(float time, CBAnimSplash *splash);

									// return a mask of which attributes this effect sets
		virtual uint32				SetAttribMask();

									// return a mask of which attributes this effect modifies
		virtual uint32				ModAttribMask();
} CBAnimSplashFX_SlideOn;

#endif		// __FEFFECTS_CBANIMSPLASH_H__
