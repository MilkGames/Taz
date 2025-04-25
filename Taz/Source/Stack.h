// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : Stack.h
//   Purpose : Stack class - please use it!
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __STACK_H__
#define __STACK_H__

#include "global.h"
#include "main.h"

// PP: debug the workings of the stack
#ifdef _TAZDEBUG
#define STACK_DEBUG
#endif// PP: def _TAZDEBUG

// PP: default name for a stack if none is specified
#define STACK_DEFAULT_NAME		"STACK noname"

// PP: maximum length of a name for a stack
#define STACK_NAME_LENGTH		64

template <typename T, uint32 S> class STACK
{

private:

	// PP: name of the stack, for debugging

	char			name[STACK_NAME_LENGTH];

	const T*		items[S];		// PP: pointer to data

	uint32			depth;			// PP: current depth of the stack


public:


	/*	--------------------------------------------------------------------------------
		Function 	: STACK::push
		Purpose 	: push an item onto the stack
		Parameters 	: ptr to item to push onto the stack
		Returns 	: item ptr you pass in
		Info 		: 
	*/
	T*	push(const T* const itemIn)
	{

#ifdef STACK_DEBUG
//		bkPrintf("STACK \"%s\" PUSH 0x%08x\n", this->name, itemIn);

		// PP: make sure the stack isn't full
		ASSERT(this->depth < (S-1));
#endif// PP: def STACK_DEBUG

		if(this->depth == S)
		{
			bkPrintf("STACK::push: (\"%s\") OVERFLOW!!!! (max depth %s)\n", this->name, this->depth);
			return NULL;
		}

		this->items[this->depth++]=itemIn;

		return const_cast<T*>(itemIn);	// PP: CONST_CAST: see below
	}


	/*	--------------------------------------------------------------------------------
		Function 	: STACK::pop
		Purpose 	: pop an item off the stack
		Parameters 	: 
		Returns 	: ptr to item popped off the stack
		Info 		: Mmmmmm,  Red Leicester...room-temperature-soft...mmmnaargm
	*/
	T*	pop(void)
	{
		const T*	rtnItem;

		// PP: make sure the stack isn't empty
		if(this->depth == 0)
		{
			return NULL;
		}

		// PP: going to return a ptr to the item popped off the stack
		rtnItem=this->items[--this->depth];

		return const_cast<T*>(rtnItem);// PP: CONST_CAST: The STACK class promises not to alter any of the data to which it points, and it enforces this promise by using 'const's.
										// PP:	But when that data gets returned back into the big wide world, it has to lose its 'const' to be of any use to anyone.
										// PP:	Now, that there is the only time you'll see me casting-away 'const's - when RETURNING data.
										// PP:	So I'm not doing anything dirty with it, see?!
										// PP:	Hell, it's all academic anyway - most people don't even USE 'const's!

										// PP: If that 'const_cast' operator doesn't exist on your platform, just a (T*) cast should do it.
										// PP:	I just thought I might as well use the tool for the job.  (And I thought it'd make me look dead brainy like)
	}


	/*	--------------------------------------------------------------------------------
		Function 	: STACK::getTopItem
		Purpose 	: get a ptr to the topmost item on the stack
		Parameters 	: 
		Returns 	: um, a ptr to the topmost item on the stack
		Info 		: 
	*/
	inline T* getTopItem(void)
	{
		if(this->depth == 0)
		{
			return NULL;
		}

		return const_cast<T*>(this->items[this->depth-1]);// PP: CONST_CAST: see above
	}


	/*	--------------------------------------------------------------------------------
		Function 	: STACK::init
		Purpose 	: initialise the stack
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void init(void)
	{
		// PP: base node
		this->depth=0;

#ifdef STACK_DEBUG
		this->name[0]='\0';
#endif// PP: def STACK_DEBUG
	}


	/*	--------------------------------------------------------------------------------
		Function 	: STACK::shutdown
		Purpose 	: shut-down the stack
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void shutdown(void)
	{
		// PP: hello
	}


	/*	--------------------------------------------------------------------------------
		Function 	: STACK::STACK
		Purpose 	: default constructor: initialises a new STACK instance
		Parameters 	: (opt/NULL=default)name of stack
		Returns 	: new STACK instance
		Info 		: 
	*/
	STACK(const char* const nameIn=NULL)
	{
		this->init();

#ifdef STACK_DEBUG
		if(nameIn == NULL)
		{
			// PP: default name
			STRNCPY(this->name, STACK_DEFAULT_NAME, STACK_NAME_LENGTH);
		}
		else
		{
			STRNCPY(this->name, nameIn, STACK_NAME_LENGTH);
		}
#endif// PP: def STACK_DEBUG
	}


	/*	--------------------------------------------------------------------------------
		Function 	: STACK::~STACK
		Purpose 	: destructor: shuts-down the STACK instance
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	~STACK(void)
	{
		this->shutdown();
	}


	/*	--------------------------------------------------------------------------------
		Function 	: STACK::getDepth
		Purpose 	: get the depth of the stack
		Parameters 	: 
		Returns 	: the depth of the stack
		Info 		: // PP: Don't use this for anything dirty - ya hear?!
	*/
	inline uint32 getDepth(void) const
	{
		return this->depth;
	}
	

};


#endif// PP: ndef __STACK_H__