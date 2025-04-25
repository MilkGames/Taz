// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of OBEFile.dlu, (c) 2000 Interactive Studios Ltd.
//
//    File : ParseMap.cpp
// Purpose : map file parser
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "babel.h"
#include "debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ParseMap.h"
#include "global.h"
#include "main.h"

// -------------------
// Function prototypes

// char line functions
static void LoadLine(char *cp, char *ep, char *line);
static char *NextCharLine(char *cp, char *ep);
static char *TidyLine(char *line);

// get tab indentation from level
static char *GetIndent(int level);

#if BPLATFORM==GAMECUBE
TBEventClient	*ARAMEventClient;
TARAMBlock		*theARAMBlock;
bool			ARAMRequestFinished = false;
int				ARAMPtr				= 0;
int				longestString		= 0;
int				cacheHit			= 0;
int				cacheMiss			= 0;
int				cacheThrash			= 0;
int				NoofNodeObjects		= 0;
int				NoofAttribObjects	= 0;
int				chunkSize;

// IH: If the cache is enabled setup the cache arrays, else just set up a pointer to stop it complaining
#ifdef USE_CACHE
CPMNode			*nodeCache[NODE_CACHE_SIZE];
CPMAttrib		*attribCache[ATTRIB_CACHE_SIZE];
bool			nodeCacheEnabled[NODE_CACHE_SIZE];
bool			attribCacheEnabled[ATTRIB_CACHE_SIZE];
#else
CPMNode			**nodeCache;
CPMAttrib		**attribCache;
bool			*nodeCacheEnabled;
bool			*attribCacheEnabled;
#endif

CPMNode			*nodeArray[NODE_ARRAY_SIZE];
CPMNode			*nodeArrayPtr[NODE_ARRAY_SIZE];
CPMAttrib		*attribArray[ATTRIB_ARRAY_SIZE];
CPMAttrib		*attribArrayPtr[ATTRIB_ARRAY_SIZE];
int				nodeArrayIndex		= 0;
int				attribArrayIndex	= 0;

#endif

// -----------------
// Types and classes

// ** 'CPMAttrib' class

/* --------------------------------------------------------------------------------
   Function : CPMAttrib::CPMAttrib
   Purpose : constructor
   Parameters : 
   Returns : 
   Info : 
*/
CPMAttrib::CPMAttrib()
{
#if BPLATFORM==GAMECUBE
	// IH: Clear the memory
	memset(this, 0, ROUNDUP32(sizeof(CPMAttrib)));
#else
	next = prev = this;
	type = PMATTRIB_NULL;
	string = NULL;
	ival = 0;
	fval = 0.f;
	strcpy(ident, "ROOT");
#endif
}


/* --------------------------------------------------------------------------------
   Function : CPMAttrib::CPMAttrib
   Purpose : constructor
   Parameters : 
   Returns : 
   Info : 
*/
CPMAttrib::CPMAttrib(CPMAttrib *root)
{
#if BPLATFORM==GAMECUBE
	// IH: Clear the memory
	memset(this, 0, ROUNDUP32(sizeof(CPMAttrib)));
#else
	next = root;
//	prev = root->prev->next;
	prev = root->prev;
	root->prev->next = this;
	root->prev = this;

	type = PMATTRIB_NULL;
	string = NULL;
	ival = 0;
	fval = 0.f;
	strcpy(ident, "");
#endif
}


/* --------------------------------------------------------------------------------
   Function : CPMAttrib::~CPMAttrib
   Purpose : destructor
   Parameters : 
   Returns : 
   Info : 
*/
CPMAttrib::~CPMAttrib()
{
#if BPLATFORM==GAMECUBE
	// IH: Do nothing
#else
	if (type == PMATTRIB_STRING)
	{
		FREE(string);
	}

	prev->next = next;
	next->prev = prev;
#endif
}


/* --------------------------------------------------------------------------------
   Function : CPMAttrib::Init
   Purpose : Initialise the object after creation
   Parameters : 
   Returns : A pointer to the block in ARAM
   Info : GameCube only
*/
#if BPLATFORM==GAMECUBE
void CPMAttrib::Init(void)
{
	int i;

	ARAMAddress = UNDEFINED_ARAM_ADDRESS;

	CreateARAMObject();

	next = prev = ARAMAddress;
	type = PMATTRIB_NULL;
//	strcpy( string, "/0" );
//	string = NULL;
	ival = 0;
	fval = 0.0f;
	strcpy(ident, "ROOT");

	CopyToARAM(true);
}
#endif


/* --------------------------------------------------------------------------------
   Function : CPMAttrib::Init
   Purpose : Initialise the object after creation
   Parameters : 
   Returns : A pointer to the object in ARAM
   Info : GameCube only
*/
#if BPLATFORM==GAMECUBE
void CPMAttrib::Init(int root)
{
	int i;

	CPMAttrib		*rootPrev;
	CPMAttrib		*localRoot;

	ARAMAddress = UNDEFINED_ARAM_ADDRESS;

	// IH: Create a new block so we have a pointer to refer to
	CreateARAMObject();

	// IH: Create root and root->prev and get them from ARAM
	localRoot = localRoot->MyNew();

//	localRoot = new localRoot;
//	ASSERT(localRoot);
	localRoot->CopyFromARAM(root, true);

	// IH: Problems having two copies of 'this'
	if( localRoot->prev != localRoot->ARAMAddress )
	{
		rootPrev = rootPrev->MyNew();
//		rootPrev = new CPMAttrib();
//		ASSERT(rootPrev);
		rootPrev->CopyFromARAM(localRoot->prev, true);
		
		// IH: Update the pointers
		next = root;
		prev = localRoot->prev;
		rootPrev->next = ARAMAddress;
		localRoot->prev = ARAMAddress;

		// IH: Copy them back to aram
		localRoot->CopyToARAM(true);
		rootPrev->CopyToARAM(true);

		// IH: Clean up
//		SAFE_DELETE(rootPrev);
//		SAFE_DELETE(localRoot);
		rootPrev->MyDelete();
		localRoot->MyDelete();
	}
	else
	{
		// IH: Update the pointers
		prev = localRoot->prev;
		localRoot->next = ARAMAddress;
		localRoot->prev = ARAMAddress;

		// IH: Copy back to aram
		localRoot->CopyToARAM(true);

		next = root;

		// IH: Clean up
//		SAFE_DELETE(localRoot);
		localRoot->MyDelete();
	}

	// IH: Initialise this
	type = PMATTRIB_NULL;
//	strcpy( string, "\0" );
//	string = NULL;
	ival = 0;
	fval = 0.f;
	strcpy(ident, "");

	// IH: Finally move the ARAMObject to ARAM
	CopyToARAM(true);
}
#endif


/* --------------------------------------------------------------------------------
   Function : CPMAttrib::DeleteAll
   Purpose : destructor
   Parameters : 
   Returns : 
   Info : 
*/
void CPMAttrib::DeleteAll()
{
#if BPLATFORM==GAMECUBE
	// IH: Dont need to do anything, all ARAM will be removed when the level is destroyed
#else
	while (next != this)
		delete next;
	delete this;
#endif
}


/* --------------------------------------------------------------------------------
   Function : CPMAttrib::DeleteAll
   Purpose : destructor
   Parameters : 
   Returns : 
   Info : 
*/
CPMAttrib *CPMAttrib::Find(char *ident)
{
	CPMAttrib	*attrib;

#if BPLATFORM==GAMECUBE
	CPMAttrib		*localAttrib;
	
//	localAttrib = new CPMAttrib();
//	ASSERT(localAttrib);
	localAttrib = localAttrib->MyNew();
	localAttrib->CopyFromARAM(this->ARAMAddress, true);

//	attrib = new CPMAttrib();
//	ASSERT(attrib);
	attrib = attrib->MyNew();
	attrib->CopyFromARAM(this->next, true);

	for( attrib = attrib; attrib->ARAMAddress != localAttrib->ARAMAddress; )
	{
		if (stricmp(ident, attrib->ident) == NULL)
		{
//			SAFE_DELETE(localAttrib);
			localAttrib->MyDelete();
			// IH: Doesn't really delete it
			attrib->MyDelete();

			return attrib;
		}
		attrib->CopyFromARAM(attrib->next, true);
	}

	localAttrib->MyDelete();
	attrib->MyDelete();
//	SAFE_DELETE(localAttrib);
//	SAFE_DELETE(attrib);
	return NULL;

#else

	for (attrib = this->next; attrib != this; attrib = attrib->next)
		if (stricmp(ident, attrib->ident) == NULL)
			return attrib;
	return NULL;

#endif
}


/* --------------------------------------------------------------------------------
   Function : CPMAttrib::CreateARAMObject
   Purpose : Creates a block of ARAM
   Parameters : 
   Returns : 
   Info : only on GameCube
*/
#if BPLATFORM==GAMECUBE
void CPMAttrib::CreateARAMObject(void)
{
	ARAMAddress = GetARAMAddress();
	ASSERTM( (ARAMAddress != UNDEFINED_ARAM_ADDRESS), "Error: Couldn't Malloc ARAM Block\n" );

	copyCount = 0;

//	NoofAttribObjects++;

//	bkPrintf( "ARAM Free Space: %d\n", bkARAMHeapFreeSpace() );

//	bkARAMHeapShow(100);
}
#endif


/* --------------------------------------------------------------------------------
   Function : CPMAttrib::GetARAMAddress
   Purpose : Gets a pointer to some ARAM
   Parameters : 
   Returns : 
   Info : only on GameCube
*/
#if BPLATFORM==GAMECUBE
int CPMAttrib::GetARAMAddress(void)
{
	int		retValue;

	retValue = ARAMPtr;
	ARAMPtr += ROUNDUP32(sizeof(CPMAttrib));

	return retValue;
}
#endif


/* --------------------------------------------------------------------------------
   Function : CPMAttrib::CopyToARAM
   Purpose : copys to aram
   Parameters : 
   Returns : 
   Info : only on GameCube
*/
#if BPLATFORM==GAMECUBE
void CPMAttrib::CopyToARAM(bool useCache)
{
	int			localARAMAddress;
	int			retVal;
#ifndef USE_CACHE
	useCache = false;
#endif

	if( useCache && (StoredInCache() || UseCacheSlot()) )
	{
		CopyToCache();
//		cacheHit++;
	}
	else
	{
		// IH: Create an ARAM object if needed
		if( ARAMAddress == UNDEFINED_ARAM_ADDRESS )
			CreateARAMObject();

		localARAMAddress = ARAMAddress;

		ASSERTM(ARAMAddress >= 0 && ARAMAddress < ARAM_BLOCK_SIZE, "Attrib CopyFromARAM Error");

		// IH: Load the files to the aram and check
		retVal = bkARAMUpload( (uchar*)this, theARAMBlock, "ARAMMovement", localARAMAddress, ROUNDUP32(sizeof(CPMAttrib)) );
		WaitForARAMUpload();
		ASSERTM( retVal, "Error: Couldn't upload to ARAM\n" );

		ARAMAddress = localARAMAddress;
//		cacheMiss++;
	}
}
#endif


/* --------------------------------------------------------------------------------
   Function : CPMAttrib::CopyFromARAM
   Purpose : copys from aram
   Parameters : 
   Returns : 
   Info : Used instead of Init(), only on GameCube
*/
#if BPLATFORM==GAMECUBE
void CPMAttrib::CopyFromARAM(bool useCache)
{
	int			localARAMAddress;
	int			retVal;
	localARAMAddress = ARAMAddress;

	ASSERTM(ARAMAddress >= 0 && ARAMAddress < ARAM_BLOCK_SIZE, "Attrib CopyFromARAM Error");

#ifndef USE_CACHE
	useCache = false;
#endif

	if( useCache && StoredInCache() )
	{
		CopyFromCache(this);
//		cacheHit++;
	}
	else
	{
		ASSERTM( (ARAMAddress != UNDEFINED_ARAM_ADDRESS), "Error: ARAMAddress does not exist\n" );

		// IH: Load the files from aram and check
		retVal = bkARAMDownload( (uchar*)this, theARAMBlock, "ARAMMovement", localARAMAddress, ROUNDUP32(sizeof(CPMAttrib)) );
		WaitForARAMDownload();
		ASSERTM( retVal, "Error: Couldn't download from ARAM\n" );
//		cacheMiss++;
	}

	ARAMAddress = localARAMAddress;
}
#endif


/* --------------------------------------------------------------------------------
   Function : CPMAttrib::CopyFromARAM
   Purpose : copys from aram
   Parameters : An address to copy from
   Returns : 
   Info : Used instead of Init(), only on GameCube
*/
#if BPLATFORM==GAMECUBE
void CPMAttrib::CopyFromARAM(int _ARAMAddress, bool useCache)
{
	int			localARAMAddress;
	int			retVal;

	localARAMAddress = _ARAMAddress;

	ARAMAddress = _ARAMAddress;

	ASSERTM(ARAMAddress >= 0 && ARAMAddress < ARAM_BLOCK_SIZE, "Attrib CopyFromARAM Error");

#ifndef USE_CACHE
	useCache = false;
#endif

	if( useCache && StoredInCache() )
	{
		CopyFromCache(this);
//		cacheHit++;
	}
	else
	{
		ASSERTM( (ARAMAddress != UNDEFINED_ARAM_ADDRESS), "Error: ARAMAddress does not exist\n" );

		// IH: Load the files from aram and check
		retVal = bkARAMDownload( (uchar*)this, theARAMBlock, "ARAMMovement", localARAMAddress, ROUNDUP32(sizeof(CPMAttrib)) );
		WaitForARAMDownload();
		ASSERTM( retVal, "Error: Couldn't download from ARAM\n" );
//		cacheMiss++;
	}
	
	ARAMAddress = localARAMAddress;
}
#endif


/* --------------------------------------------------------------------------------
   Function : CPMAttrib::StoredInCache
   Purpose : Is there a copy of this in cache?
   Parameters : 
   Returns : yes or no
   Info : only on GameCube
*/
#if BPLATFORM==GAMECUBE
bool CPMAttrib::StoredInCache(void)
{

	int cachePosition = ARAMAddress%ATTRIB_CACHE_SIZE;

	return (attribCacheEnabled[cachePosition] && attribCache[cachePosition]->ARAMAddress == ARAMAddress);
}
#endif


/* --------------------------------------------------------------------------------
   Function : CPMAttrib::UseCacheSlot
   Purpose : Is this slot ready to be used
   Parameters : 
   Returns : yes or no
   Info : only on GameCube
*/
#if BPLATFORM==GAMECUBE
bool CPMAttrib::UseCacheSlot(void)
{
	int cachePosition = ARAMAddress%ATTRIB_CACHE_SIZE;

	copyCount++;

	// IH: If the cache contains nothing
	if( !attribCacheEnabled[cachePosition] )
	{
		attribCacheEnabled[cachePosition] = true;
//		bkPrintf( "Copied Attrib to cache\n" );
		cacheHit++;
		return true;
	}
	// IH: Is this one used more than the one in the cache?
	else if( attribCache[cachePosition]->copyCount+CACHE_THRASH < copyCount )
	{
		// IH: Copy from cache to ARAM
		attribCache[cachePosition]->CopyToARAM(false);
		// IH: Copy to Cache
//		bkPrintf( "CACHE THRASH - Copied Attrib to cache - CACHE THRASH\n" );
//		cacheThrash++;
		return true;
	}
	else
	{
//		bkPrintf( "Attrib not in cache\n" );
		cacheMiss++;
		return false;
	}
}
#endif



/* --------------------------------------------------------------------------------
   Function : CPMAttrib::CopyToCache
   Purpose : copys to cache
   Parameters : Address of object and sizeofobject
   Returns : 
   Info : only on GameCube
*/
#if BPLATFORM==GAMECUBE
void CPMAttrib::CopyToCache(void)
{
	int cachePosition = ARAMAddress%ATTRIB_CACHE_SIZE;

	CopyAttrib(attribCache[cachePosition], this);

}
#endif


/* --------------------------------------------------------------------------------
   Function : CPMAttrib::CopyFromCache
   Purpose : copys from cache
   Parameters : An address to copy from and size
   Returns : 
   Info : only on GameCube
*/
#if BPLATFORM==GAMECUBE
void CPMAttrib::CopyFromCache(CPMAttrib *attribPtr)
{
	int cachePosition = ARAMAddress%ATTRIB_CACHE_SIZE;

	CopyAttrib(attribPtr, attribCache[cachePosition]);
}
#endif


/* --------------------------------------------------------------------------------
   Function : CPMAttrib::CopyAttrib
   Purpose : copys from one place to another
   Parameters : An address to copy to and from
   Returns : 
   Info : only on GameCube
*/
#if BPLATFORM==GAMECUBE
void CPMAttrib::CopyAttrib(CPMAttrib *dest, CPMAttrib *src)
{
//	dest = src;
//	memcpy(dest, src, ROUNDUP32(sizeof(CPMAttrib)));

	// IH: TEMPORARY: Would memcpy be quicker than this?
	dest->ARAMAddress	= src->ARAMAddress;
	dest->copyCount		= src->copyCount;
	
	strcpy(dest->ident, src->ident);

	// IH: only copy the data if needed
	switch (src->type)
	{
	case PMATTRIB_STRING:
		strcpy(dest->string, src->string);
		break;
	case PMATTRIB_INT:
		dest->ival		= src->ival;
		break;
	case PMATTRIB_FLOAT:
		dest->fval		= src->fval;
		break;
	case PMATTRIB_BOOL:
		dest->bval		= src->bval;
		break;
	}
}
#endif


/* --------------------------------------------------------------------------------
   Function : CPMAttrib::MyNew
   Purpose : gets space for an attrib
   Parameters : 
   Returns : a pointer
   Info : only on GameCube
*/
#if BPLATFORM==GAMECUBE
CPMAttrib *CPMAttrib::MyNew(void)
{
	attribArray[attribArrayIndex] = attribArrayPtr[attribArrayIndex];
	memset(attribArray[attribArrayIndex], 0, ROUNDUP32(sizeof(CPMAttrib)));
	return (attribArray[attribArrayIndex++]);
}
#endif


/* --------------------------------------------------------------------------------
   Function : CPMAttrib::MyDelete
   Purpose : deletes the attrib
   Parameters : 
   Returns : a pointer
   Info : only on GameCube
*/
#if BPLATFORM==GAMECUBE
void CPMAttrib::MyDelete(void)
{
	attribArrayIndex--;
}
#endif



// ** 'CPMNode' class

/* --------------------------------------------------------------------------------
   Function : CPMNode::CPMNode
   Purpose : constructor
   Parameters : 
   Returns : 
   Info : 
*/

CPMNode::CPMNode()
{
#if BPLATFORM==GAMECUBE
	// IH: Clear the memory
	memset(this, 0, ROUNDUP32(sizeof(CPMNode)));
#else
	next = prev = this;
	parent = NULL;
	children = NULL;

	type = NODETYPE_ROOT;
//	strcpy(type, "ROOT");

	attribs = new CPMAttrib();
	ASSERT(attribs);
#endif

}


/* --------------------------------------------------------------------------------
   Function : CPMNode::CPMNode
   Purpose : constructor
   Parameters : 
   Returns : 
   Info : 
*/
CPMNode::CPMNode(CPMNode *parentNode)
{
#if BPLATFORM==GAMECUBE
	// IH: Clear the memory
	memset(this, 0, ROUNDUP32(sizeof(CPMNode)));
#else
	next = prev = this;
	parent = NULL;
	children = NULL;

	type = -1;
//	strcpy(type, "");

	attribs = new CPMAttrib();
	ASSERT(attribs);

	if (parentNode != NULL)
		parentNode->AttachChild(this);
#endif
}


/* --------------------------------------------------------------------------------
   Function : CPMNode::Init
   Purpose : to initialise the object and move to aram
   Parameters : 
   Returns : a pointer to the aram object
   Info : 
*/
#if BPLATFORM==GAMECUBE
void CPMNode::Init(void)
{
	CPMAttrib	*localAttribs;

	ARAMAddress = UNDEFINED_ARAM_ADDRESS;

	CreateARAMObject();

	next = prev = ARAMAddress;
	parent = UNDEFINED_ARAM_ADDRESS;
	children = UNDEFINED_ARAM_ADDRESS;

	type = NODETYPE_ROOT;

	// IH: Create attribs and move them to ARAM
//	localAttribs = new CPMAttrib();
//	ASSERT(localAttribs);
	localAttribs = localAttribs->MyNew();
	localAttribs->Init();
	attribs = localAttribs->ARAMAddress;

	// IH: Finally move the ARAMObject to ARAM and return it's pointer
	CopyToARAM(true);

//	SAFE_DELETE(localAttribs);
	localAttribs->MyDelete();
}
#endif


/* --------------------------------------------------------------------------------
   Function : CPMNode::Init
   Purpose : to initialise the object and move to aram
   Parameters : 
   Returns : a pointer to the 
   Info : 
*/
#if BPLATFORM==GAMECUBE
void CPMNode::Init(CPMNode *parentNode)
{
	CPMAttrib	*localAttribs;

	ARAMAddress = UNDEFINED_ARAM_ADDRESS;

	CreateARAMObject();

	next = prev = ARAMAddress;
	parent = UNDEFINED_ARAM_ADDRESS;
	children = UNDEFINED_ARAM_ADDRESS;

	type = -1;

	// IH: Create attribs and send to ARAM
//	localAttribs = new CPMAttrib();
//	ASSERT(localAttribs);
	localAttribs = localAttribs->MyNew();
	localAttribs->Init();

	attribs = localAttribs->ARAMAddress;

//	SAFE_DELETE(localAttribs);
	localAttribs->MyDelete();

	CopyToARAM(true);

	if (parentNode != NULL)
		parentNode->AttachChild(this);

	parentNode->CopyToARAM(true);
}
#endif


/* --------------------------------------------------------------------------------
   Function : CPMNode::CPMNode
   Purpose : destructor
   Parameters : 
   Returns : 
   Info : 
*/

CPMNode::~CPMNode()
{
#if BPLATFORM==GAMECUBE
	// IH: Nothing to destroy, all of the ARAM will be cleared in one go
#else

	// delete all children
	if (children)
	{
		while (children->next != children)
			delete children->next;
		delete children;
	}

	// disconnect from siblings
	next->prev = prev;
	prev->next = next;

	// fix the parent's children pointer if necessary
	if ((parent) && (parent->children == this))
		parent->children = (next == prev) ? NULL : next;

	// delete all this nodes attributes
	attribs->DeleteAll();
#endif
}

char GetNodeType(char *type)
{
	char retVal=-1;

	if (strcmpi(type,"level")==0) retVal = NODETYPE_LEVEL;
	else if (strcmpi(type,"world")==0) retVal = NODETYPE_WORLD;
	else if (strcmpi(type,"object")==0) retVal = NODETYPE_OBJECT;
	else if (strcmpi(type,"instance")==0) retVal = NODETYPE_INSTANCE;

	return retVal;
}

/* --------------------------------------------------------------------------------
   Function : CPMNode::SetType
   Purpose : set node type
   Parameters : type string
   Returns : 
   Info : 
*/

void CPMNode::SetType(char *type)
{
	this->type = GetNodeType(type);
	
//	if ((strlen(type)+1)>16) ASSERTM(0,"Node name is too long!\n");
//	strcpy(this->type, type);
}


/* --------------------------------------------------------------------------------
   Function : CPMNode::AttachChild
   Purpose : makes the specified node a child of this node.
   Parameters : child node pointer
   Returns : 
   Info : 
*/
void CPMNode::AttachChild(CPMNode *child)
{
#if BPLATFORM==GAMECUBE

	parent = this->ARAMAddress;
	if (children == UNDEFINED_ARAM_ADDRESS)
	{
		children = child->ARAMAddress;
	}
	else
	{
		CPMNode *localChildren;
		CPMNode *localChildrenPrev;

//		localChildren = new CPMNode();
//		ASSERT(localChildren);
		localChildren = localChildren->MyNew();
		localChildren->CopyFromARAM(children, true);

		// IH: Problems with having two copies of 'this'
		if( localChildren->prev != localChildren->ARAMAddress )
		{
//			localChildrenPrev = new CPMNode();
//			ASSERT(localChildrenPrev);
			localChildrenPrev = localChildrenPrev->MyNew();
			localChildrenPrev->CopyFromARAM(localChildren->prev, true);
	
			// IH: Update the pointers
			child->next = children;
			child->prev = localChildren->prev;
			localChildrenPrev->next = child->ARAMAddress;
			localChildren->prev = child->ARAMAddress;

			localChildren->CopyToARAM(true);

			// IH: Copy them back to aram
			localChildrenPrev->CopyToARAM(true);
			
			// IH: Clean up
//			SAFE_DELETE(localChildrenPrev);
//			SAFE_DELETE(localChildren);
			localChildren->MyDelete();
			localChildrenPrev->MyDelete();
		}
		else
		{
			// IH: Update the pointers
			child->prev = localChildren->prev;
			localChildren->next = child->ARAMAddress;
			localChildren->prev = child->ARAMAddress;

			// IH: Copy back to aram
			localChildren->CopyToARAM(true);

			child->next = children;

			// IH: Clean up
//			SAFE_DELETE(localChildren);
			localChildren->MyDelete();
		}
	}

#else

	parent = this;
	if (children == NULL)
		children = child;
	else
	{
		child->prev = children->prev;
		child->next = children;
		children->prev->next = child;
		children->prev = child;
	}

#endif
}


char *FindChar(char *text,int c)
{
	while((*text)!=0)
	{
		if ((*text)==c) return text;
		text++;
	}
	return 0;
}

/* --------------------------------------------------------------------------------
   Function : CPMNode::Read
   Purpose : read in map file
   Parameters : file struct pointer
   Returns : 
   Info : 
*/

int CPMNode::Read(TPMFile *file)
{
//	static char		path[_MAX_PATH], drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
#if 0
	TPMFile			*newFile;
#endif

	CPMNode			*node;
#if BPLATFORM==GAMECUBE
	int				ARAMNode;
#endif

	char			line[1024], block[32];
	char			*rp, *tp, *ap, *wp1, *wp2;


	while (file->sp < file->ep)
	{
		LoadLine(file->sp, file->ep, line);
		file->sp = NextCharLine(file->sp, file->ep);

		rp = TidyLine(line);

		// ** Parse this packed line

//		bkPrintf( "%s\n", line);

		switch (*rp)
		{
		// directive?
		case '#':
#if 0
			// #include
			if (stricmp(rp+1, "include") == NULL)
			{
				// isolate include file
				wp1 = strchr(rp, '"');
				if (wp1)
					wp2 = strchr(wp1+1, '"');
				if (wp2 && (wp2-wp1) > 1)
				{
					// include file is relative to current file (??!!)
					_splitpath(file->filename, drive, dir, fname, ext);
					*wp2 = 0;
					sprintf(path, "%s%s%s", drive, dir, wp1+1);

					// 
					if ((newFile = pmOpenFile(path)) != NULL)
					{
						Read(newFile);
						pmCloseFile(newFile);
						newFile = NULL;
					}
				}
			}
#endif

			// #?
			break;

		// block start
		case '{':
#if BPLATFORM==GAMECUBE
//			node = new CPMNode();
//			ASSERT(node);
			node = node->MyNew();
			node->Init(this);
			node->Read(file);
			node->type = GetNodeType(block);
			// IH: Upload and remove
			node->CopyToARAM(true);
//			SAFE_DELETE(node);
			node->MyDelete();
#else
			node = new CPMNode(this);
			ASSERT(node);
			node->Read(file);
//			if ((strlen(block)+1) > 16) ASSERTM(0,"Node name is too long!\n");
//			strcpy(node->type, block);
			node->type = GetNodeType(block);
#endif
			break;

		// block end
		case '}':
			return 1;

		// could be a tag or block name
		default:
			if (strlen(rp) > 0)
			{
				if ((ap = FindChar(rp, '=')) != NULL)
//				if ((ap = strchr(rp, '=')) != NULL)
				{
					// get the tag assignment variable name
					tp = ap;
					while (tp > rp && (*tp == '=' || *tp == ' ' || *tp == '\t'))	// /t
						tp--;
					if (tp >= rp)
					{
						// terminate tag name
						*(tp+1) = 0;

						// ** The line seems to be a tag assignment. We need to determine what type
						// ** of assignment this is.. [string, float, int, bool]

						ap++;								// skip equals sign
						wp1 = wp2 = NULL;					// null working char pointers

						// as strings are always enclosed within a pair of quotes..
						wp1 = strchr(ap, '"');
						if (wp1)
							wp2 = strchr(wp1+1, '"');
						if (wp2 && (wp2-wp1) > 1)
						{
							// attribute: string
							*wp2 = 0;
							AddAttrib(rp, wp1+1);
							break;
						}

						// boolean true?
						if (stricmp(ap, "true") == NULL)
						{
							// attribute: boolean true
							AddAttrib(rp, true);
							break;
						}
						// boolean false?
						if (stricmp(ap, "false") == NULL)
						{
							// attribute: boolean false
							AddAttrib(rp, false);
							break;
						}

						// as floats always have a point..
						wp1 = strchr(ap, '.');
						if (wp1)
						{
							for (;;ap++)
								if ((*ap >= '0' && *ap <= '9') || *ap == '.' || *ap == '-')
								{
									break;
								}

							// attribute: float
							AddAttrib(rp, (float)atof(ap));
							break;
						}
						else if ((*ap >= '0' && *ap <= '9') || *ap == '-')
						{
							// attribute: integer
							AddAttrib(rp, atoi(ap));
							break;
						}

						// ** Cannot determine assignment type!!!
						bkPrintf("Error in mapfile\n");
					}
				}
				else
				{
					// assume block name
					strcpy(block, rp);
				}
			}
			break;
		}
	}
	return 1;
}


/* --------------------------------------------------------------------------------
   Function : CPMNode::Write
   Purpose : write out node to file
   Parameters : file struct pointer
   Returns : 1 okay, 0 error
   Info : 
*/
int CPMNode::Write(TPMFile *file)
{
// IH: Won't work on GameCube
#if BPLATFORM!=GAMECUBE
	static int	level = 0;
	char		*indent;
	CPMNode		*node;
	CPMAttrib	*attrib;

	// write node type name and opening brace
	indent = GetIndent(level);
	fprintf(file->fp, "\n%s%s\n%s{\n", indent, type, indent);

	// write all this nodes attributes
	indent = GetIndent(level+1);
	for (attrib = attribs->next; attrib != attribs; attrib = attrib->next)
	{
		switch (attrib->type)
		{
		case PMATTRIB_STRING:
			fprintf(file->fp, "%s%s = \"%s\"\n", indent, attrib->ident, attrib->string);
			break;
		case PMATTRIB_INT:
			fprintf(file->fp, "%s%s = %d\n", indent, attrib->ident, attrib->ival);
			break;
		case PMATTRIB_FLOAT:
			fprintf(file->fp, "%s%s = %.2f\n", indent, attrib->ident, attrib->fval);
			break;
		case PMATTRIB_BOOL:
			fprintf(file->fp, "%s%s = %s\n", indent, attrib->ident, (attrib->bval == true) ? "true" : "false");
			break;
		}
	}

	// write all nodes children
	if ((node = children) != NULL)
	{
		level++;
		do
			{
			node->Write(file);
			node = node->next;
			}
		while (node != children);
		level--;
	}

	// write nodes matching closing brace
	indent = GetIndent(level);
	fprintf(file->fp, "%s}\n", indent);
#endif

	return 1;
}



/* --------------------------------------------------------------------------------
   Function : CPMNode::AddAttrib
   Purpose : add tagged string attribute to node
   Parameters : attribute ident string pointer, string attribute
   Returns : 
   Info : 
*/

void CPMNode::AddAttrib(char *ident, char *string)
{
	CPMAttrib	*attrib;
#if BPLATFORM==GAMECUBE

//	attrib = new CPMAttrib();
//	ASSERT(attrib);
	attrib = attrib->MyNew();
	attrib->Init(attribs);

	attrib->type = PMATTRIB_STRING;
	if ((strlen(ident)+1)>20) ASSERTM(0,"Attrib identifier is too long!\n");
	strcpy(attrib->ident, ident);
	strlwr(attrib->ident);

//	if( strlen(string) > longestString )
//		longestString = strlen(string);
	if ((strlen(string)+1)>PARSEMAP_LONGEST_STRING) ASSERTM( 0, "Error: String too long\n");
//	attrib->string = (char*)ZALLOC(strlen(string)+1);
//	ASSERT(attrib->string);
	strcpy(attrib->string, string);
	strlwr(attrib->string);

	attrib->CopyToARAM(true);
	attrib->MyDelete();
//	SAFE_DELETE(attrib);

#else
	
	attrib = new CPMAttrib(attribs);
	ASSERT(attrib);
	attrib->type = PMATTRIB_STRING;
	if ((strlen(ident)+1)>20) ASSERTM(0,"Attrib identifier is too long!\n");
	strcpy(attrib->ident, ident);
	strlwr(attrib->ident);
	attrib->string = (char*)ZALLOC(strlen(string)+1);
	ASSERT(attrib->string);
	strcpy(attrib->string, string);
	strlwr(attrib->string);
#endif
}


/* --------------------------------------------------------------------------------
   Function : CPMNode::AddAttrib
   Purpose : add tagged integer attribute to node
   Parameters : attribute ident string pointer, string attribute
   Returns : 
   Info : 
*/

void CPMNode::AddAttrib(char *ident, int ival)
{
	CPMAttrib	*attrib;

#if BPLATFORM==GAMECUBE
	
//	attrib = new CPMAttrib();
//	ASSERT(attrib);
	attrib = attrib->MyNew();
	attrib->Init(attribs);

	attrib->type = PMATTRIB_INT;
	if ((strlen(ident)+1)>20) ASSERTM(0,"Attrib identifier is too long!\n");
	strcpy(attrib->ident, ident);
//	strcpy(attrib->string, "");
	attrib->ival = ival;

	attrib->CopyToARAM(true);
//	SAFE_DELETE(attrib);
	attrib->MyDelete();

#else
	attrib = new CPMAttrib(attribs);
	ASSERT(attrib);
	attrib->type = PMATTRIB_INT;
	if ((strlen(ident)+1)>20) ASSERTM(0,"Attrib identifier is too long!\n");
	strcpy(attrib->ident, ident);
	attrib->ival = ival;
#endif
}


/* --------------------------------------------------------------------------------
   Function : CPMNode::AddAttrib
   Purpose : add tagged float attribute to node
   Parameters : attribute ident string pointer, string attribute
   Returns : 
   Info : 
*/

void CPMNode::AddAttrib(char *ident, float fval)
{
	CPMAttrib	*attrib;
#if BPLATFORM==GAMECUBE
	
//	attrib = new CPMAttrib();
//	ASSERT(attrib);
	attrib = attrib->MyNew();
	attrib->Init(attribs);

	attrib->type = PMATTRIB_FLOAT;
	if ((strlen(ident)+1)>20) 
	{
		bkPrintf("Attrib %s is too long!\n",ident);
		ASSERTM(0,"Attrib identifier is too long!\n");
	}
	strcpy(attrib->ident, ident);
//	strcpy(attrib->string, "");
	attrib->fval = fval;

	attrib->CopyToARAM(true);
//	SAFE_DELETE(attrib);
	attrib->MyDelete();
	
#else
	attrib = new CPMAttrib(attribs);
	ASSERT(attrib);
	attrib->type = PMATTRIB_FLOAT;
	if ((strlen(ident)+1)>20) 
	{
		bkPrintf("Attrib %s is too long!\n",ident);
		ASSERTM(0,"Attrib identifier is too long!\n");
	}
	strcpy(attrib->ident, ident);
	attrib->fval = fval;
#endif
}


/* --------------------------------------------------------------------------------
   Function : CPMNode::AddAttrib
   Purpose : add tagged boolean attribute to node
   Parameters : attribute ident string pointer, boolean value
   Returns : 
   Info : 
*/

void CPMNode::AddAttrib(char *ident, bool bval)
{
	CPMAttrib	*attrib;
#if BPLATFORM==GAMECUBE
	
//	attrib = new CPMAttrib();
//	ASSERT(attrib);
	attrib = attrib->MyNew();
	attrib->Init(attribs);

	attrib->type = PMATTRIB_BOOL;
	if ((strlen(ident)+1)>20) ASSERTM(0,"Attrib identifier is too long!\n");
	strcpy(attrib->ident, ident);
//	strcpy(attrib->string, "");
	attrib->bval = bval;

	attrib->CopyToARAM(true);
//	SAFE_DELETE(attrib);
	attrib->MyDelete();

#else

	attrib = new CPMAttrib(attribs);
	ASSERT(attrib);
	attrib->type = PMATTRIB_BOOL;
	if ((strlen(ident)+1)>20) ASSERTM(0,"Attrib identifier is too long!\n");
	strcpy(attrib->ident, ident);
	attrib->bval = bval;

#endif
}


/* --------------------------------------------------------------------------------
   Function : CPMNode::GetAttrib
   Purpose : get string attribute from this node
   Parameters : attribute ident string pointer, string pointer
   Returns : NULL - no such attribute, else string pointer
   Info : 
*/

char *CPMNode::GetAttrib(char *ident)
{
	CPMAttrib	*attrib;

#if BPLATFORM==GAMECUBE
	
	CPMAttrib	*localAttribs;

	if ((strlen(ident)+1)>20) ASSERTM(0,"Attrib identifier is too long!\n");

//	localAttribs = new CPMAttrib();
//	ASSERT(localAttribs);
	localAttribs = localAttribs->MyNew();
	localAttribs->CopyFromARAM(attribs, true);

	if( (attrib = localAttribs->Find(ident)) == NULL )
	{
//		SAFE_DELETE(localAttribs);
		localAttribs->MyDelete();
		return NULL;
	}

//	SAFE_DELETE(localAttribs);
	localAttribs->MyDelete();
	if( attrib->type != PMATTRIB_STRING )
	{
		return NULL;
	}
	// IH: Doesn't really delete it, just changes the pointer
	return attrib->string;

#else

	if ((strlen(ident)+1)>20) ASSERTM(0,"Attrib identifier is too long!\n");
	
	if ((attrib = attribs->Find(ident)) == NULL)
		return NULL;
	if (attrib->type != PMATTRIB_STRING)
		return NULL;
	return attrib->string;

#endif
}


/* --------------------------------------------------------------------------------
   Function : CPMNode::GetAttrib
   Purpose : get integer attribute from this node
   Parameters : attribute ident string pointer, integer reference
   Returns : 0 - no such attribute, else 1 and integer ref is set
   Info : 
*/

int CPMNode::GetAttrib(char *ident, int& ival)
{
	CPMAttrib	*attrib;
	
	if ((strlen(ident)+1)>20) ASSERTM(0,"Attrib identifier is too long!\n");

#if BPLATFORM==GAMECUBE

	CPMAttrib	*localAttribs;

//	localAttribs = new CPMAttrib();
//	ASSERT(localAttribs);
	localAttribs = localAttribs->MyNew();
	localAttribs->CopyFromARAM(attribs, true);

	if( (attrib = localAttribs->Find(ident)) == NULL )
	{
//		SAFE_DELETE(attrib);
//		SAFE_DELETE(localAttribs);
		localAttribs->MyDelete();
		return 0;
	}

//	SAFE_DELETE(localAttribs);
	localAttribs->MyDelete();

	if( attrib->type != PMATTRIB_INT )
	{
		return 0;
	}

	ival = attrib->ival;
	return 1;

#else

	if ((attrib = attribs->Find(ident)) == NULL)
		return 0;
	if (attrib->type != PMATTRIB_INT)
		return 0;
	ival = attrib->ival;
	return 1;

#endif
}


/* --------------------------------------------------------------------------------
   Function : CPMNode::GetAttrib
   Purpose : get float attribute from this node
   Parameters : attribute ident string pointer, float reference
   Returns : 0 - no such attribute, else 1 and float ref is set
   Info : 
*/

int CPMNode::GetAttrib(char *ident, float& fval)
{
	CPMAttrib	*attrib;
	
	if ((strlen(ident)+1)>20) ASSERTM(0,"Attrib identifier is too long!\n");

#if BPLATFORM==GAMECUBE
	
	CPMAttrib	*localAttribs;

//	localAttribs = new CPMAttrib();
//	ASSERT(localAttribs);
	localAttribs = localAttribs->MyNew();
	localAttribs->CopyFromARAM(attribs, true);

	if( (attrib = localAttribs->Find(ident)) == NULL )
	{
//		SAFE_DELETE(localAttribs);
		localAttribs->MyDelete();
		return 0;
	}

	localAttribs->MyDelete();
//	SAFE_DELETE(localAttribs);

	if( attrib->type != PMATTRIB_FLOAT )
	{
		return 0;
	}

	fval = attrib->fval;
	return 1;

#else

	if ((attrib = attribs->Find(ident)) == NULL)
		return 0;
	if (attrib->type != PMATTRIB_FLOAT)
		return 0;
	fval = attrib->fval;
	return 1;

#endif
}


/* --------------------------------------------------------------------------------
   Function : CPMNode::GetAttrib
   Purpose : get boolean attribute from this node
   Parameters : attribute ident string pointer, boolean reference
   Returns : 0 - no such attribute, else 1 and boolean ref is set
   Info : 
*/

int CPMNode::GetAttrib(char *ident, bool& bval)
{
	CPMAttrib	*attrib;
	
	if ((strlen(ident)+1)>20) ASSERTM(0,"Attrib identifier is too long!\n");

#if BPLATFORM==GAMECUBE
	
	CPMAttrib	*localAttribs;

//	localAttribs = new CPMAttrib();
//	ASSERT(localAttribs);
	localAttribs = localAttribs->MyNew();
	localAttribs->CopyFromARAM(attribs, true);

	if( (attrib = localAttribs->Find(ident)) == NULL )
	{
//		SAFE_DELETE(localAttribs);
		localAttribs->MyDelete();
		return 0;
	}

//	SAFE_DELETE(localAttribs);
	attrib->MyDelete();

	if( attrib->type != PMATTRIB_BOOL )
	{
		return 0;
	}

	bval = attrib->bval;
	return 1;

#else

	if ((attrib = attribs->Find(ident)) == NULL)
		return 0;
	if (attrib->type != PMATTRIB_BOOL)
		return 0;
	bval = attrib->bval;
	return 1;

#endif
}


/* --------------------------------------------------------------------------------
   Function : CPMNode::GetAttrib
   Purpose : get attribute type from this node
   Parameters : ident pointer
   Returns : PMATTRIB_NULL - unknown ident, or PMATTRIB_STRING, PMATTRIB_INT, PMATTRIB_FLOAT, PMATTRIB_BOOL
   Info : 
*/

int CPMNode::GetAttribType(char *ident)
{
	CPMAttrib	*attrib;
	
	if ((strlen(ident)+1)>20) ASSERTM(0,"Attrib identifier is too long!\n");

#if BPLATFORM==GAMECUBE
	int			attribType;
	CPMAttrib	*localAttribs;

//	localAttribs = new CPMAttrib();
//	ASSERT(localAttribs);
	localAttribs = localAttribs->MyNew();
	localAttribs->CopyFromARAM(attribs, true);

	if( (attrib = localAttribs->Find(ident)) == NULL )
	{
//		SAFE_DELETE(localAttribs);
		localAttribs->MyDelete();
		return PMATTRIB_NULL;
	}

//	SAFE_DELETE(localAttribs);
	localAttribs->MyDelete();
	
	attribType = attrib->type;
	return attrib->type;

#else

	if ((attrib = attribs->Find(ident)) == NULL)
		return PMATTRIB_NULL;
	return attrib->type;

#endif
}


/* --------------------------------------------------------------------------------
   Function : CPMNode::DelAttrib
   Purpose : delete attribute from node with ident
   Parameters : ident pointer
   Returns : 0 - no such attribute, else 1 and deleted
   Info : 
*/

int CPMNode::DelAttrib(char *ident)
{
	CPMAttrib	*attrib;
#if BPLATFORM==GAMECUBE
	// IH : TODO
	return 0; // CHANGE ME!
#else
	if ((attrib = attribs->Find(ident)) != NULL)
		delete attrib;
	return (attrib != NULL);
#endif
}


/* --------------------------------------------------------------------------------
   Function : CPMNode::EnumAttribs
   Purpose : enumerate the this nodes attributes, calling context user function
   Parameters : attribute ident string pointer, float reference
   Returns : PMENUM_CONTINUE = continue enumeration, PMENUM_END = end
   Info : 
*/

int CPMNode::EnumAttribs(TPMEnumAttribFunc *funcPtr, void *context)
{
	CPMAttrib	*attrib;

#if BPLATFORM==GAMECUBE

	CPMAttrib		*localAttribs;

//	localAttribs = new CPMAttrib();
//	ASSERT(localAttribs);
	localAttribs = localAttribs->MyNew();
	localAttribs->CopyFromARAM(attribs, true);

//	attrib = new CPMAttrib();
//	ASSERT(attrib);
	attrib = attrib->MyNew();
	attrib->CopyFromARAM(localAttribs->next, true);

	for( attrib = attrib; attrib->ARAMAddress != localAttribs->ARAMAddress; )
	{
		funcPtr(attrib, context);
		attrib->CopyFromARAM(attrib->next, true);
	}

//	SAFE_DELETE(attrib);
//	SAFE_DELETE(localAttribs);
	attrib->MyDelete();
	localAttribs->MyDelete();

	return PMENUM_CONTINUE;

#else

	for (attrib = attribs->next; attrib != attribs; attrib = attrib->next)
		funcPtr(attrib, context);
	return PMENUM_CONTINUE;

#endif
}


/* --------------------------------------------------------------------------------
   Function : CPMNode::EnumNodes
   Purpose : recursively enumerate the node tree for specific node idents, calling user function on entry
   Parameters : function pointer, context, type of node to find
   Returns : PMENUM_CONTINUE = continue enumeration, PMENUM_END = end, PMENUM_END_PATH = just end this path
   Info : 
*/

int CPMNode::EnumNodes(TPMEnumNodeFunc *funcPtr, void *context, char *type)
{
	int		ret;
	CPMNode	*node;
	char	typeWanted = GetNodeType(type);

#if BPLATFORM==GAMECUBE

//	bkPrintf( "ARAMAddress: %d, children: %d\n", this->ARAMAddress, this->children );

	if ((type == NULL) || (typeWanted==this->type))
	{
		if ((ret = funcPtr(this, context)) != PMENUM_CONTINUE)
		{
			return ret;
		}
	}

	if( children != UNDEFINED_ARAM_ADDRESS )
	{
		// IH: Copy the children into the node
//		node = new CPMNode();
//		ASSERT(node);
		node = node->MyNew();
		node->CopyFromARAM(children, true);

		do
		{
			if((ret = node->EnumNodes(funcPtr, context, type)) == PMENUM_END)
			{
//				SAFE_DELETE(node);
				node->MyDelete();
				return ret;
			}
			node->CopyFromARAM(node->next, true);
		}
		while( node->ARAMAddress != children );
//		SAFE_DELETE(node);
		node->MyDelete();
	}
	return PMENUM_CONTINUE;

#else

	if ((type == NULL) || (typeWanted==this->type))
	{
		if ((ret = funcPtr(this, context)) != PMENUM_CONTINUE)
		{
			return ret;
		}
	}
	if ((node = children) != NULL)
	{
		do
		{
			if ((ret = node->EnumNodes(funcPtr, context, type)) == PMENUM_END)
			{
				return ret;
			}
			node = node->next;
		}
		while (node != children);
	}
	return PMENUM_CONTINUE;

#endif
}


/* --------------------------------------------------------------------------------
   Function : CPMNode::EnumNodes
   Purpose : recursively enumerate the node tree for specific node idents, calling user function on entry (pre) and exit (post)
   Parameters : pre function pointer, pre context, post function pointer, post context, type of node to find
   Returns : PMENUM_CONTINUE = continue enumeration, PMENUM_END = end, PMENUM_END_PATH = just end this path
   Info : 
*/

int CPMNode::EnumNodes(TPMEnumNodeFunc *preFuncPtr, void *preContext, TPMEnumNodeFunc *postFuncPtr, void *postContext, char *type)
{
	int		ret;
	CPMNode	*node;

#if BPLATFORM==GAMECUBE

	char	typeWanted = GetNodeType(type);

	if ((type == NULL) || (typeWanted == this->type))
	{
		if ((ret = preFuncPtr(this, preContext)) != PMENUM_CONTINUE)
		{
			return ret;
		}
	}
	if( children != UNDEFINED_ARAM_ADDRESS )
	{
		// IH: Copy the children into the node
//		node = new CPMNode();
//		ASSERT(node);
		node = node->MyNew();
		node->CopyFromARAM(children, true);

		do
		{
			if ((ret = node->EnumNodes(preFuncPtr, preContext, postFuncPtr, postContext, type)) == PMENUM_END)
			{
				return ret;
			}
			node->CopyFromARAM(node->next, true);
		}
		while( node->ARAMAddress != children );
	}
	return 0;

#else

	char	typeWanted = GetNodeType(type);

	if ((type == NULL) || (typeWanted == this->type))
	{
		if ((ret = preFuncPtr(this, preContext)) != PMENUM_CONTINUE)
		{
			return ret;
		}
	}
	if ((node = children) != NULL)
	{
		do
		{
			if ((ret = node->EnumNodes(preFuncPtr, preContext, postFuncPtr, postContext, type)) == PMENUM_END)
			{
				return ret;
			}
			node = node->next;
		}
		while (node != children);
	}
	if ((type == NULL) || (typeWanted == this->type))
	{
		if ((ret = postFuncPtr(this, postContext)) != PMENUM_CONTINUE)
		{
			return ret;
		}
	}
	return 0;

#endif
}


/* --------------------------------------------------------------------------------
   Function : CPMNode::CreateARAMObject
   Purpose : Creates a block of ARAM
   Parameters : 
   Returns : 
   Info : only on GameCube
*/
#if BPLATFORM==GAMECUBE
void CPMNode::CreateARAMObject(void)
{
	ARAMAddress = GetARAMAddress();
	ASSERTM( (ARAMAddress != UNDEFINED_ARAM_ADDRESS), "Error: Couldn't Malloc ARAM Block\n" );

	copyCount = 0;

//	NoofNodeObjects++;

//	bkARAMHeapShow(1);
}
#endif


/* --------------------------------------------------------------------------------
   Function : CPMNode::GetARAMAddress
   Purpose : Gets the address of a block of ARAM
   Parameters : 
   Returns : 
   Info : only on GameCube
*/
#if BPLATFORM==GAMECUBE
int CPMNode::GetARAMAddress(void)
{
	int		retValue;

	retValue = ARAMPtr;
	ARAMPtr += ROUNDUP32(sizeof(CPMNode));

	return retValue;
}
#endif


/* --------------------------------------------------------------------------------
   Function : CPMNode::CopyToARAM
   Purpose : copys to aram
   Parameters : 
   Returns : 
   Info : only on GameCube
*/
#if BPLATFORM==GAMECUBE
void CPMNode::CopyToARAM(bool useCache)
{
	int			localARAMAddress;
	int			retVal;

#ifndef USE_CACHE
	useCache = false;
#endif

	if( useCache && (StoredInCache() || UseCacheSlot()) )
	{
		CopyToCache();
//		cacheHit++;
	}
	else
	{
		// IH: Create an ARAM object if needed
		if( ARAMAddress == UNDEFINED_ARAM_ADDRESS )
			CreateARAMObject();

		localARAMAddress = ARAMAddress;

		ASSERT(ARAMAddress >= 0);

		// IH: Load the files to the aram and check
		retVal = bkARAMUpload( (uchar*)this, theARAMBlock, "ARAMMovement", localARAMAddress, ROUNDUP32(sizeof(CPMNode)) );
		WaitForARAMUpload();
		ASSERTM( retVal, "Error: Couldn't upload to ARAM\n" );

		ARAMAddress = localARAMAddress;
//		cacheMiss++;
	}
}
#endif


/* --------------------------------------------------------------------------------
   Function : CPMNode::CopyFromARAM
   Purpose : copys from aram
   Parameters : 
   Returns : 
   Info : Used instead of Init(), only on GameCube
*/
#if BPLATFORM==GAMECUBE
void CPMNode::CopyFromARAM(bool useCache)
{
	int			localARAMAddress;
	int			retVal;
	localARAMAddress = ARAMAddress;

	ASSERTM(ARAMAddress >= 0 && ARAMAddress < ARAM_BLOCK_SIZE, "Node CopyFromARAM Error");

#ifndef USE_CACHE
	useCache = false;
#endif

	if( useCache && StoredInCache() )
	{
		CopyFromCache(this);
//		cacheHit++;
	}
	else
	{
		ASSERTM( (ARAMAddress != UNDEFINED_ARAM_ADDRESS), "Error: ARAMAddress does not exist\n" );
	
		// IH: Load the files from aram and check
		retVal = bkARAMDownload( (uchar*)this, theARAMBlock, "ARAMMovement", localARAMAddress, ROUNDUP32(sizeof(CPMNode)) );
		WaitForARAMDownload();
		ASSERTM( retVal, "Error: Couldn't download from ARAM\n" );
//		cacheMiss++;
	}

	ARAMAddress = localARAMAddress;
}
#endif


/* --------------------------------------------------------------------------------
   Function : CPMNode::CopyFromARAM
   Purpose : copys from aram
   Parameters : An address to copy from
   Returns : 
   Info : Used instead of Init(), only on GameCube
*/
#if BPLATFORM==GAMECUBE
void CPMNode::CopyFromARAM(int _ARAMAddress, bool useCache)
{
	int			localARAMAddress;
	int			retVal;
	localARAMAddress = _ARAMAddress;

	ARAMAddress = _ARAMAddress;

	ASSERTM(ARAMAddress >= 0 && ARAMAddress < ARAM_BLOCK_SIZE, "Node CopyFromARAM Error");

#ifndef USE_CACHE
	useCache = false;
#endif

	if( useCache && StoredInCache() )
	{
		CopyFromCache(this);
//		cacheHit++;
	}
	else
	{
		ASSERTM( (ARAMAddress != UNDEFINED_ARAM_ADDRESS), "Error: ARAMAddress does not exist\n" );
	
		// IH: Load the files from aram and check
		retVal = bkARAMDownload( (uchar*)this, theARAMBlock, "ARAMMovement", localARAMAddress, ROUNDUP32(sizeof(CPMNode)) );
		WaitForARAMDownload();
		ASSERTM( retVal, "Error: Couldn't download from ARAM\n" );
//		cacheMiss++;
	}

	ARAMAddress = localARAMAddress;
}
#endif


/* --------------------------------------------------------------------------------
   Function : CPMNode::StoredInCache
   Purpose : Is there a copy of this in cache?
   Parameters : 
   Returns : yes or no
   Info : only on GameCube
*/
#if BPLATFORM==GAMECUBE
bool CPMNode::StoredInCache(void)
{
	int cachePosition = ARAMAddress%NODE_CACHE_SIZE;

	return (nodeCacheEnabled[cachePosition] && nodeCache[cachePosition]->ARAMAddress == ARAMAddress);
}
#endif


/* --------------------------------------------------------------------------------
   Function : CPMNode::UseCacheSlot
   Purpose : Is this slot ready to be used
   Parameters : 
   Returns : yes or no
   Info : only on GameCube
*/
#if BPLATFORM==GAMECUBE
bool CPMNode::UseCacheSlot(void)
{
	int cachePosition = ARAMAddress%NODE_CACHE_SIZE;

	copyCount++;

	// IH: If the cache contains nothing
	if( !nodeCacheEnabled[cachePosition] )
	{
		nodeCacheEnabled[cachePosition] = true;
//		bkPrintf( "Copied Node to cache\n" );
		return true;
	}
	// IH: Is this one used more than the one in the cache?
	else if( nodeCache[cachePosition]->copyCount+CACHE_THRASH < copyCount )
	{
		// IH: Copy from cache to ARAM
		nodeCache[cachePosition]->CopyToARAM(false);
		// IH: Copy to Cache
//		bkPrintf( "CACHE THRASH - Copied Node to cache - CACHE THRASH\n" );
//		cacheThrash++;
		return true;
	}
	else
	{
//		bkPrintf( "Node not in cache\n" );
		return false;
	}
}
#endif


/* --------------------------------------------------------------------------------
   Function : CPMNode::CopyToCache
   Purpose : copys to cache
   Parameters : 
   Returns : 
   Info : Assumes the node is already setup in the cache, only on GameCube
*/
#if BPLATFORM==GAMECUBE
void CPMNode::CopyToCache(void)
{
	int cachePosition = ARAMAddress%NODE_CACHE_SIZE;

	CopyNode(nodeCache[cachePosition], this);
}
#endif


/* --------------------------------------------------------------------------------
   Function : CPMNode::CopyNode
   Purpose : copys from one place to another
   Parameters : An address to copy to and from
   Returns : 
   Info : only on GameCube
*/
#if BPLATFORM==GAMECUBE
void CPMNode::CopyNode(CPMNode *dest, CPMNode *src)
{
//	dest = src;
	dest->parent		= src->parent;
	dest->prev			= src->prev;
	dest->next			= src->next;
	dest->children		= src->children;
	dest->ARAMAddress	= src->ARAMAddress;
	dest->type			= src->type;
	dest->copyCount		= src->copyCount;
	dest->attribs		= src->attribs;
}
#endif


/* --------------------------------------------------------------------------------
   Function : CPMNode::CopyFromCache
   Purpose : copys from cache
   Parameters : An address to copy from and size
   Returns : 
   Info : only on GameCube
*/
#if BPLATFORM==GAMECUBE
void CPMNode::CopyFromCache(CPMNode *nodePtr)
{
	int cachePosition = ARAMAddress%NODE_CACHE_SIZE;

	CopyNode(nodePtr, nodeCache[cachePosition]);
}
#endif


/* --------------------------------------------------------------------------------
   Function : CPMNode::MyNew
   Purpose : gets space for a node
   Parameters : 
   Returns : a pointer
   Info : only on GameCube
*/
#if BPLATFORM==GAMECUBE
CPMNode *CPMNode::MyNew(void)
{
	nodeArray[nodeArrayIndex] = nodeArrayPtr[nodeArrayIndex];
	memset(nodeArray[nodeArrayIndex], 0, ROUNDUP32(sizeof(CPMNode)));
	return (nodeArray[nodeArrayIndex++]);
}
#endif


/* --------------------------------------------------------------------------------
   Function : CPMNode::MyDelete
   Purpose : deletes the node
   Parameters : 
   Returns : a pointer
   Info : only on GameCube
*/
#if BPLATFORM==GAMECUBE
void CPMNode::MyDelete(void)
{
	nodeArrayIndex--;
}
#endif


// ** 'CPMLevel' class

/* --------------------------------------------------------------------------------
   Function : CPMLevel::CPMLevel
   Purpose : constructor
   Parameters : 
   Returns : 
   Info : 
*/
CPMLevel::CPMLevel()
{
#if BPLATFORM==GAMECUBE
	CPMNode	*localRoot;

//	localRoot = new CPMNode();
//	ASSERT(localRoot);
	localRoot = localRoot->MyNew();
	localRoot->Init();
	root = localRoot->ARAMAddress;
	
//	SAFE_DELETE(localRoot);
	localRoot->MyDelete();
#else
	root = new CPMNode;
	ASSERT(root);
#endif
}


/* --------------------------------------------------------------------------------
   Function : CPMLevel::~CPMLevel
   Purpose : destructor
   Parameters : 
   Returns : 
   Info : 
*/
CPMLevel::~CPMLevel()
{
#if BPLATFORM==GAMECUBE

	int i;

/*	bkPrintf("Wasted Ticks: %d\n", (int)wastedTicks);
	bkPrintf("longest String: %d, \n", (int)longestString);
	bkPrintf("Noof Node objects: %d, Noof Attrib objects: %d\n", (int)NoofNodeObjects, (int)NoofAttribObjects);
	bkPrintf("CACHE: Hit: %d, Miss: %d, Thrash: %d\n", (int)cacheHit, (int)cacheMiss, (int)cacheThrash );
	bkPrintf("NodeArrayIndex: %d, AttribArrayIndex: %d\n", nodeArrayIndex, attribArrayIndex);
	bkPrintf("ARAMPtr: %d\n", (int)ARAMPtr);*/

	// IH: Delete the node and attrib arrays
	for( i = 0; i < NODE_ARRAY_SIZE; i++ )
	{
		SAFE_DELETE(nodeArray[i]);
		nodeArray[i] = NULL;
		nodeArrayPtr[i] = NULL;
	}

	for( i = 0; i < ATTRIB_ARRAY_SIZE; i++ )
	{
		SAFE_DELETE(attribArray[i]);
		attribArray[i] = NULL;
		attribArrayPtr[i] = NULL;
	}

#ifdef USE_CACHE
	// IH: Delete the cache
	for( i = 0; i < NODE_CACHE_SIZE; i++ )
	{
		SAFE_DELETE(nodeCache[i]);
		nodeCacheEnabled[i] = false;
	}
	
	for( i = 0; i < ATTRIB_CACHE_SIZE; i++ )
	{
		SAFE_DELETE(attribCache[i]);
		attribCacheEnabled[i] = false;
	}
#endif
	ARAM_FREE(theARAMBlock);
//	ARQSetChunkSize( chunkSize );
	bkDeleteEvent("ARAMMovement");
#else
	delete root;
#endif
}


/* --------------------------------------------------------------------------------
   Function : CPMLevel::Read
   Purpose : read file to create node tree
   Parameters : file struct pointer
   Returns : 
   Info : 
*/
void CPMLevel::Read(TPMFile *file)
{
#if BPLATFORM==GAMECUBE
	CPMNode	*localRoot;

//	localRoot = new CPMNode();
//	ASSERT(localRoot);
	localRoot = localRoot->MyNew();
	localRoot->CopyFromARAM(root, true);

	localRoot->Read(file);

//	SAFE_DELETE(localRoot);
	localRoot->MyDelete();
#else
	root->Read(file);
#endif
}


/* --------------------------------------------------------------------------------
   Function : CPMLevel::Write
   Purpose : write node tree to file
   Parameters : file struct pointer
   Returns : 
   Info : 
*/
void CPMLevel::Write(TPMFile *file)
{
#if BPLATFORM==GAMECUBE
#else
	fprintf(file->fp, "\n");
	fprintf(file->fp, "// Level: %s\n", file->filename);
	fprintf(file->fp, "// file generated by OBEFile\n");
	fprintf(file->fp, "\n");

	if (root->children)
		root->children->Write(file);
#endif
}


/* --------------------------------------------------------------------------------
   Function : CPMLevel::EnumNodes
   Purpose : recursively enumerate the node tree for specific node idents, calling user function on node entry
   Parameters : pre function pointer, pre context, post function pointer, post context, type of node to find
   Returns : PMENUM_CONTINUE = continue enumeration, PMENUM_END = end, PMENUM_END_PATH = just end this path
   Info : 
*/
int CPMLevel::EnumNodes(TPMEnumNodeFunc *funcPtr, void *context, char *type)
{
#if BPLATFORM==GAMECUBE

	CPMNode	*localRoot;
	int		retValue;

//	localRoot = new CPMNode();
//	ASSERT(localRoot);

	localRoot = localRoot->MyNew();
	localRoot->CopyFromARAM(root, true);

//	bkPrintf( "localRoot ARAMAddress: %d, localRoot Children: %d\n", localRoot->ARAMAddress, localRoot->children);

	retValue = localRoot->EnumNodes(funcPtr, context, type);

//	SAFE_DELETE(localRoot);
	localRoot->MyDelete();

	return retValue;

#else

	return root->EnumNodes(funcPtr, context, type);

#endif
}


/* --------------------------------------------------------------------------------
   Function : CPMLevel::EnumNodes
   Purpose : recursively enumerate the node tree for specific node idents, calling user function on node entry (pre) and exit (post)
   Parameters : pre function pointer, pre context, post function pointer, post context, type of node to find
   Returns : PMENUM_CONTINUE = continue enumeration, PMENUM_END = end, PMENUM_END_PATH = just end this path
   Info : 
*/
int CPMLevel::EnumNodes(TPMEnumNodeFunc *preFuncPtr, void *preContext, TPMEnumNodeFunc *postFuncPtr, void *postContext, char *type)
{
#if BPLATFORM==GAMECUBE

	CPMNode	*localRoot;
	int		retValue;
	
//	localRoot = new CPMNode();
//	ASSERT(localRoot);
	localRoot = localRoot->MyNew();
	localRoot->CopyFromARAM(root, true);

	retValue = localRoot->EnumNodes(preFuncPtr, preContext, postFuncPtr, postContext, type);

//	SAFE_DELETE(localRoot);
	localRoot->MyDelete();

	return retValue;

#else

	return root->EnumNodes(preFuncPtr, preContext, postFuncPtr, postContext, type);

#endif
}


// ---------------
// Other functions

/* --------------------------------------------------------------------------------
   Function : LoadLine
   Purpose : load a line of text from the a char strean
   Parameters : current char pointer, end char stream pointer, text line to load
   Returns : 
   Info : 
*/

static void LoadLine(char *cp, char *ep, char *line)
{
	while (cp < ep)
	{
		if ((*cp == 0x0a) || (*cp == 0x0d))
			break;
		*line++ = *cp++;
	}
	*line = 0;
}


/* --------------------------------------------------------------------------------
   Function : NextCharLine
   Purpose : return the next line from the char stream
   Parameters : current char pointer, end char stream pointer
   Returns : pointer to next char line
   Info : 
*/
static char *NextCharLine(char *cp, char *ep)
{
	while (cp < ep)
	{
		if (*cp == 0x0a || *cp == 0x0d)
			break;
		cp++;
	}
	if (*(cp+1) == 0x0a || *(cp+1) == 0x0d)
		return cp+2;
	return cp+1;
}


/* --------------------------------------------------------------------------------
   Function : TidyLine
   Purpose : tidy up the line by removing all trailing chars and fillers
   Parameters : pointer to text line to tidy up (ASCIIZ)
   Returns : tidied line pointer
   Info : 
*/
static char *TidyLine(char *line)
{
	static char	buf[260];
	char		c, *sp, *dp, *rp, q;

	// save line in temp buffer
	strcpy(buf, line);

	sp = buf;
	// strip all leading spaces and tabs
	while ((c = *sp) != 0)
	{
		if (c != 0x09 && c != 0x20)
		{
			break;
		}
		sp++;
	}
	if (c == 0)
	{
		// if zero found.. use NULL line
		line[0] = 0;
		return line;
	}

	rp = dp = line;
	// strip all filler spaces and comments
	q = 0;
	while ((c = *sp) != 0)
	{
		// don't strip inbetween strings
		if (q)
		{
			*dp++ = c;
			if (c == '"')
				q = 0;
		}
		else
		{
			if (c == '"')
				q = 1;
			if (c == '/' && *(sp+1) == '/')
				break;
			if (c != 0x09 && c != 0x20)
				*dp++ = c;
		}
		sp++;
	}

	// terminate packed line and return
	*dp = 0;
	return rp;
}


/* --------------------------------------------------------------------------------
   Function : GetIndent
   Purpose : return tab indentation from current level
   Parameters : current level [0,1,2...]
   Returns : tab indentation string
   Info : 
*/

static char *GetIndent(int level)
{
	static char	indent[32];
	indent[level] = 0;
	while (level--)
		indent[level] = '\t';
	return indent;
}


// ------------------
// Shortcut functions

/* --------------------------------------------------------------------------------
   Function : pmLoadLevel
   Purpose : load in level objects
   Parameters : filename or file memory stream pointer,
				file input type [PMLOAD_FILE, PMLOAD_MEMORY]. default = file,
				file length if loadType == PMLOAD_MEMORY. default = 0
   Returns : instance of level class or NULL if failed
   Info : 
*/
CPMLevel *pmLoadLevel(char *fileIn, int loadType, int fileLen)
{
	CPMLevel	*level;
	TPMFile		*file;

	if ((file = pmOpenFile(fileIn, (loadType == PMLOAD_FILE) ? PMMODE_READ_FILE : PMMODE_READ_MEMORY,fileLen)) == NULL)
		return NULL;


#if BPLATFORM==GAMECUBE

	int i;

	// IH: Create an event and check that it is valid
	ASSERTM( bkCreateEvent("ARAMMovement"), "Error: Couldn't create event 'ARAMMovement'\n" );
	// IH: Context??
	ARAMEventClient = bkTrapEventCallback("ARAMMovement", ARAMRequestCallback, NULL);

	// IH: do one big ARAM_MALLOC
	theARAMBlock = ARAM_MALLOC( ARAM_BLOCK_SIZE );
	ASSERTM( theARAMBlock, "Error: Couldn't get that ARAM\n" );
	ARAMPtr = 0;

//	chunkSize = ARQGetChunkSize();

	// IH: TEMPORARY: CHANGE CHUNK SIZE FOR SPEED. THIS SHOULD BE IN BABEL
//	if( ROUNDUP32(sizeof(CPMNode)) > ROUNDUP32(sizeof(CPMAttrib)) )
//		ARQSetChunkSize( ROUNDUP32(sizeof(CPMNode)) );
//	else
//		ARQSetChunkSize( ROUNDUP32(sizeof(CPMAttrib)) );

	// IH: Init node array
	for( i = 0; i < NODE_ARRAY_SIZE; i++ )
	{
		nodeArray[i] = new CPMNode();
		ASSERTM(nodeArray[i], "Couldn't create a new CPMNode in pmLoadLevel()\n" );
		nodeArrayPtr[i] = nodeArray[i];
	}

	// IH: Init Attrib array
	for( i = 0; i < ATTRIB_ARRAY_SIZE; i++ )
	{
		attribArray[i] = new CPMAttrib();
		ASSERTM(attribArray[i], "Couldn't create a new CPMAttrib in pmLoadLevel()\n" );
		attribArrayPtr[i] = attribArray[i];
	}

#ifdef USE_CACHE
	// IH: initialise the cache system
	for( i = 0; i < NODE_CACHE_SIZE; i++ )
	{
		nodeCache[i] = new CPMNode();
		nodeCacheEnabled[i] = false;
	}
		
	for( i = 0; i < ATTRIB_CACHE_SIZE; i++ )
	{
		attribCache[i] = new CPMAttrib();
		attribCacheEnabled[i] = false;
	}
#endif

#endif

	level = new CPMLevel();
	ASSERT(level);
	level->Read(file);
	pmCloseFile(file);
	return level;
}


/* --------------------------------------------------------------------------------
   Function : pmDeleteLevel
   Purpose : delete previously loaded level
   Parameters : level class pointer, (from pmLoadLevel function)
   Returns : 
   Info : 
*/
void pmDeleteLevel(CPMLevel *level)
{
	if (level)
		delete level;
}


/* --------------------------------------------------------------------------------
   Function : pmOpenFile
   Purpose : open level file into file struct
   Parameters : filename or file memory stream pointer,
				mode [PMMODE_READ_FILE = read file, PMMODE_READ_MEMORY = read memory, PMMODE_WRITE = write],
				file length if mode == PMMODE_READ_MEMORY. default = 0
   Returns : file struct pointer, or NULL if fail
   Info : 
*/
TPMFile	*pmOpenFile(char *fileIn, int mode, int fileLen)
{
	TPMFile		*file;
	FILE		*fp;
	int			len;
	char		*buf;

	switch (mode)
	{
	case PMMODE_READ_FILE:
#if BPLATFORM != GAMECUBE
		// open and read in the file
		if ((fp = fopen(fileIn, "rt")) == NULL)
			return NULL;
		// calculate file length, allocate buffer and read file in
		fseek(fp, 0, SEEK_END);
		len = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		if ((buf = (char *)ZALLOC(len)) == NULL)
		{
			fclose(fp);
			return NULL;
		}
		len = fread(buf, 1, len, fp);			// re-calc file length
		fclose(fp);

		// allocate and init file read buffer
		file = (TPMFile *)ZALLOC(sizeof(TPMFile));
		ASSERT(file);

		file->buf = file->sp = buf;
		file->ep = buf + len;
		file->fp = NULL;
		file->mode = mode;
		strcpy(file->filename, fileIn);

		// return our file handle
		return file;
#endif
		break;
	case PMMODE_READ_MEMORY:

		// use the pre-opened memory stream
		file = (TPMFile *)ZALLOC(sizeof(TPMFile));
		ASSERT(file);

		file->buf = file->sp = fileIn;
		file->ep = fileIn + fileLen;
		file->fp = NULL;
		file->mode = mode;
		strcpy(file->filename, "");

		// return our file handle
		return file;

	case PMMODE_WRITE:
#if BPLATFORM != GAMECUBE
		// open file for writing
		if ((fp = fopen(fileIn, "wt")) == NULL)
			return NULL;

		// allocate and init file write buffer
		file = (TPMFile *)ZALLOC(sizeof(TPMFile));
		ASSERT(file);

		file->buf = file->sp = file->ep = NULL;
		file->fp = fp;
		file->mode = mode;
		strcpy(file->filename, fileIn);

		// return our file handle
		return file;
#endif
		break;
	}

	// unknown file mode
	return NULL;
}


/* --------------------------------------------------------------------------------
   Function : pmCloseFile
   Purpose : close previously open file structure
   Parameters : file struct pointer [returned from pmOpenFile]
   Returns : 
   Info : 
*/

void pmCloseFile(TPMFile *file)
{
	if (file == NULL)
		return;
	switch (file->mode)
	{
	case PMMODE_READ_FILE:
		if (file->buf)
			FREE(file->buf);
		break;
	case PMMODE_READ_MEMORY:
		break;
	case PMMODE_WRITE:
		if (file->fp)
			fclose(file->fp);
		break;
	}
	FREE(file);
}


/* --------------------------------------------------------------------------------
   Function : WaitForARAMUpload
   Purpose : Waits for the ARAM Request to finish
   Parameters : 
   Returns : 
   Info : Only on GAMECUBE
*/
#if BPLATFORM==GAMECUBE
void WaitForARAMUpload(void)
{
	while(!ARAMRequestFinished)
	{
		bkUpdate();
		// IH: Sleep and give the other thread a chance
		bkSleep(0);
	}

	ARAMRequestFinished = false;
}
#endif


/* --------------------------------------------------------------------------------
   Function : WaitForARAMDownload
   Purpose : Waits for the ARAM Request to finish
   Parameters : 
   Returns : 
   Info : Only on GAMECUBE
*/
#if BPLATFORM==GAMECUBE
void WaitForARAMDownload(void)
{
	while(!ARAMRequestFinished)
	{
		// IH: Sleep and give the other thread a chance
		bkSleep(0);
	}

	ARAMRequestFinished = false;
}
#endif


/* --------------------------------------------------------------------------------
   Function : ARAMRequestCallback
   Purpose : Callback for the ARAM event
   Parameters : the name, the paramaters and the context
   Returns : 
   Info : Only on GAMECUBE
*/
#if BPLATFORM==GAMECUBE
void ARAMRequestCallback(char *name, char *parms, void *data, void *context)
{
	ARAMRequestFinished = true;
}
#endif


/* --------------------------------------------------------------------------------
   Function : GetNextPrime
   Purpose : Calculate the next prime number 
   Parameters : the number
   Returns : the prime
   Info : Only on GAMECUBE, not for real time, used to get a good cache size
*/
#if BPLATFORM==GAMECUBE
int GetNextPrime(int n)
{
	int		i = n;
	int		j;
	bool found = false;

	// IH: If even make it odd
	if( (i%2) == 0 )
		i++;

	// IH: start off at n and increase
	do
	{
		found = true;
		for( j = 3; j < (i/2); j+=2 )
		{
			if( (i%j) == 0 )
			{
				found = false;
				break;
			}
		}
		// IH: Keep it odd
		if( !found )
			i+=2;
	}
	while( !found );
	return i;
}
#endif