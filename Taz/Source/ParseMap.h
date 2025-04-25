// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of OBEFile.dlu, (c) 2000 Interactive Studios Ltd.
//
//    File : ParseMap.h
// Purpose : map file parser header
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __PARSEMAP__H
#define __PARSEMAP__H


// --------------------
// Constants and macros

// level level modes
#define PMLOAD_FILE		1
#define PMLOAD_MEMORY	2

// map file access modes
#define PMMODE_READ_FILE	1
#define PMMODE_READ_MEMORY	2
#define PMMODE_WRITE		3

// node enumeration returns
#define PMENUM_CONTINUE		0
#define PMENUM_END			1
#define PMENUM_END_PATH		2

// --- IH: GameCube Only ---
#if BPLATFORM==GAMECUBE
#define ROUNDUP32(_X)			(((u32)(_X) + 32 - 1) & ~(32 - 1))
// IH: 6meg should be enough
#define ARAM_BLOCK_SIZE			(ROUNDUP32(6*1024*1024))
#define UNDEFINED_ARAM_ADDRESS	(-1)
// IH: Unrem to use cache
//#define USE_CACHE
// IH: Node cache size and attrib cache size have to be prime
#define	NODE_CACHE_SIZE			(2003)
#define ATTRIB_CACHE_SIZE		(2003)
#define NODE_ARRAY_SIZE			(100)
#define ATTRIB_ARRAY_SIZE		(200)
#define CACHE_THRASH			(3)
#define PARSEMAP_LONGEST_STRING	(120)
#endif


// attribute types
enum
{
	PMATTRIB_NULL,
	PMATTRIB_STRING,
	PMATTRIB_INT,
	PMATTRIB_FLOAT,
	PMATTRIB_BOOL,
};


// -----------------
// Types and classes

// enumerate node function callback type
typedef int (TPMEnumNodeFunc)(class CPMNode *node, void *context);

// enumerate node attribute function callback type
typedef int (TPMEnumAttribFunc)(class CPMAttrib *attrib, void *context);

// file buffer structure
typedef struct _TPMFile
{
	_TPMFile()
	{
		buf = NULL;
		fp = NULL;
		mode = 0;
	}
	char	filename[260];
	char	*buf, *sp, *ep;
	int		mode;
	FILE	*fp;
} TPMFile;


// attribute class [linked list]
typedef class CPMAttrib
{
	public:
		// IH: TARAMBlocks on GameCube
#if BPLATFORM==GAMECUBE
		// linked list vars
		int							prev, next;
		// IH: Init function to move the object to ARAM
		void						Init(void);
		void						Init(int root);
		void						CreateARAMObject(void);
		void						CopyToARAM(bool useCache);
		void						CopyFromARAM(bool useCache);
		void						CopyFromARAM(int _ARAMAddress, bool useCache);
		int							GetARAMAddress(void);
		bool						StoredInCache(void);
		bool						UseCacheSlot(void);
		void						CopyToCache(void);
		void						CopyFromCache(CPMAttrib	*attribPtr);
		void						CopyAttrib(CPMAttrib *dest, CPMAttrib *src);
		CPMAttrib					*MyNew(void);
		void						MyDelete(void);
		int							ARAMAddress;
		int							copyCount;
#else
		// linked list vars
		CPMAttrib					*prev, *next;
#endif

									// attribute identifier
		char						ident[20];


		// attributes
		union
		{
#if BPLATFORM==GAMECUBE
			char					string[PARSEMAP_LONGEST_STRING];		// string
#else
			char					*string;
#endif
			int						ival;									// integer
			float					fval;									// float
			bool					bval;									// boolean
		};

									// attribute type
		char						type;

									// constructor / destructor
									CPMAttrib();
									CPMAttrib(CPMAttrib *root);
									~CPMAttrib();

									// find attribute from ident [lowercase comparison]
		CPMAttrib					*Find(char *ident);

									// delete all attributes
		void						DeleteAll();
} CPMAttrib;

enum 
{
	NODETYPE_LEVEL = 0,
	NODETYPE_WORLD,
	NODETYPE_OBJECT,
	NODETYPE_INSTANCE,
	NODETYPE_ROOT,
};

// node class [tree structure]
typedef class CPMNode
{
	public:
									// tree vars
#if BPLATFORM==GAMECUBE
		int							parent;
		int							prev, next;
		int							children;
		// IH: Each object knows its aram address after initialisation
		int							ARAMAddress;
		// IH: Init function to move the object to ARAM
		void						Init(void);
		void						Init(CPMNode *parentNode);
		void						CreateARAMObject(void);
		void						CopyToARAM(bool useCache);
		void						CopyFromARAM(bool useCache);
		void						CopyFromARAM(int _ARAMAddress, bool useCache);
		int							GetARAMAddress(void);
		bool						StoredInCache(void);
		bool						UseCacheSlot(void);
		void						CopyToCache(void);
		void						CopyFromCache(CPMNode *nodePtr);
		void						CopyNode(CPMNode *dest, CPMNode *src);
		CPMNode						*MyNew(void);
		void						MyDelete(void);
		int							copyCount;
		int							attribs;
#else

		CPMNode						*parent;
		CPMNode						*prev, *next;
		CPMNode						*children;
									// this nodes attributes
		CPMAttrib					*attribs;
#endif

									// node identification and unique name
		char						type;

									CPMNode();
									CPMNode(CPMNode *parentNode);
									~CPMNode();

									// read and create map file using this node as root
		int							Read(TPMFile *file);
									// write map file from this node including all children
		int							Write(TPMFile *file);

									// set node type
		void						SetType(char *type);

									// makes the specified node a child of this node.
		void						AttachChild(CPMNode *child);

									// add attributes to node
		void						AddAttrib(char *ident, char *string);
		void						AddAttrib(char *ident, int ival);
		void						AddAttrib(char *ident, float fval);
		void						AddAttrib(char *ident, bool bval);

									// get attribute from node
		char						*GetAttrib(char *ident);
		int							GetAttrib(char *ident, int& ival);
		int							GetAttrib(char *ident, float& fval);
		int							GetAttrib(char *ident, bool& bval);

									// get attribute type
		int							GetAttribType(char *ident);

									// delete attribute
		int							DelAttrib(char *ident);

									// enumerate this nodes attributes with context callback function
		int							EnumAttribs(TPMEnumAttribFunc *funcPtr, void *context);

									// enumerate nodes with context callback functions
		int							EnumNodes(TPMEnumNodeFunc *preFuncPtr, void *preContext, TPMEnumNodeFunc *postFuncPtr, void *postContext, char *type = NULL);
		int							EnumNodes(TPMEnumNodeFunc *funcPtr, void *context, char *type = NULL);

} CPMNode;


// level
typedef class CPMLevel
{
	public:
#if BPLATFORM==GAMECUBE
		int							root;
#else
		CPMNode						*root;
#endif

									CPMLevel();
									~CPMLevel();

		void						Read(TPMFile *stream);
		void						Write(TPMFile *stream);

									// enumerate nodes with context callback functions
		int							EnumNodes(TPMEnumNodeFunc *preFuncPtr, void *preContext, TPMEnumNodeFunc *postFuncPtr, void *postContext, char *type = NULL);
		int							EnumNodes(TPMEnumNodeFunc *funcPtr, void *context, char *type = NULL);

} CPMLevel;


char *FindChar(char *text,int c);


/* --------------------------------------------------------------------------------
   Function : pmLoadLevel
   Purpose : load in level objects
   Parameters : filename or file memory stream pointer,
				file input type [PMLOAD_FILE, PMLOAD_MEMORY]. default = file,
				file length if loadType == PMLOAD_MEMORY. default = 0
   Returns : instance of level class or NULL if failed
   Info : 
*/

CPMLevel *pmLoadLevel(char *fileIn, int loadType = PMLOAD_FILE, int fileLen = 0);


/* --------------------------------------------------------------------------------
   Function : pmDeleteLevel
   Purpose : delete previously loaded level
   Parameters : level class pointer, (from pmLoadLevel function)
   Returns : 
   Info : 
*/

void pmDeleteLevel(CPMLevel *level);


/* --------------------------------------------------------------------------------
   Function : pmOpenFile
   Purpose : open level file into file struct
   Parameters : filename or file memory stream pointer,
				mode [PMMODE_READ_FILE = read file, PMMODE_READ_MEMORY = read memory, PMMODE_WRITE = write],
				file length if mode == PMMODE_READ_MEMORY. default = 0
   Returns : file struct pointer, or NULL if fail
   Info : 
*/
TPMFile	*pmOpenFile(char *fileIn, int mode = PMMODE_READ_FILE, int fileLen = 0);


/* --------------------------------------------------------------------------------
   Function : pmCloseFile
   Purpose : close previously open file structure
   Parameters : file struct pointer [returned from pmOpenFile]
   Returns : 
   Info : 
*/

void pmCloseFile(TPMFile *file);



/*  --------------------------------------------------------------------------------
	Function	: WaitForARAMUpload
				  WaitForARAMDownload
	Purpose		: Called when a node has been copied into ARAM
	Parameters	: 
	Returns		: 
	Info		: Only on GAMECUBE
*/
#if BPLATFORM==GAMECUBE
void WaitForARAMUpload(void);
void WaitForARAMDownload(void);
#endif

/*  --------------------------------------------------------------------------------
	Function	: ARAMRequestCallback
	Purpose		: Called when a node has been copied into ARAM
	Parameters	: 
	Returns		: 
	Info		: Only on GAMECUBE
*/
#if BPLATFORM==GAMECUBE
void ARAMRequestCallback(char *name, char *parms, void *data, void *context);
#endif


/*  --------------------------------------------------------------------------------
	Function	: GetNextPrime
	Purpose		: Get the next prime
	Parameters	: a number 
	Returns		: the next prime
	Info		: Only on GAMECUBE
*/
#if BPLATFORM==GAMECUBE
int GetNextPrime(int n);
#endif

#endif