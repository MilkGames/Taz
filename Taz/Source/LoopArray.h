// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : LoopArray.h
//   Purpose : Looping array class - please use it!
// Component : Taz
/* Comments  : PP: This is ideal for keeping a fast dynamic list of things that expire at
				the same rate or at comparable rates.

				Use getNew() each time you want to use a new element of the array.
				If the list overflows, the OLDEST items will get returned for overwriting.
				Use the LA_FOREACH, LA_ENDFOREACH and LA_FOREACHDO macros to traverse the used portion
				of the array.
				Call discardOldest() each time you find that your oldest element has expired -
				this is not vital but it keeps traversal times to a minimum.
*/						
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __H_LOOPARRAY
#define __H_LOOPARRAY

#include "main.h"			// PP: main stuff

// PP: this enables names to be given to LOOPARRAYs, and prints out a warning when a LOOPARRAY overflows
//#define LOOPARRAYDEBUG


/*	--------------------------------------------------------------------------------
	Function 	: LA_FOREACH
	Purpose 	: start a loop-array traversal block
	Parameters 	: name of loop-array to traverse, type held by loop-array,
					name of element pointer to be declared and pointed at each element in turn
	Value	 	: 
	Info 		: array is traversed from oldest element to youngest element
					** PLEASE NOTE: ** you can't use 'continue' within an LA_FOREACH travesal block.
					Instead, put a label right before LA_ENDFOREACH, and use 'goto' with that label.
*/
#define LA_FOREACH(_loopArray, _type, _ptr)	\
		{ \
			if((_loopArray).youngest != -1) \
			{ \
				_type*				_ptr; \
				\
				for(int32 _i=(_loopArray).oldest;;_i--) \
				{ \
					if(_i < 0) \
					{ \
						_i=(_loopArray).totalArraySize-1; \
					} \
					\
					_ptr= (&(_loopArray).items[_i]);


/*	--------------------------------------------------------------------------------
	Function 	: LA_FOREACH_NEXT
	Purpose 	: get the next item that'll be encountered in the LA_FOREACH loop
	Parameters 	: name of loop-array
	Value	 	: ptr to the next item that'll be encountered in the LA_FOREACH loop, or NULL if we're at the end
	Info 		: 
*/
#define LA_FOREACH_NEXT(_loopArray)			((_i == (_loopArray).youngest)?NULL:(&(_loopArray).items[(_i==0)?((_loopArray).totalArraySize-1):(_i-1)]))


/*	--------------------------------------------------------------------------------
	Function 	: LA_ENDFOREACH
	Purpose 	: end a loop-array traversal block
	Parameters 	: name of loop-array to traverse
	Value	 	: 
	Info 		: array is traversed from oldest element to youngest element
*/
#define LA_ENDFOREACH(_loopArray) \
					if(_i == (_loopArray).youngest) break; \
					if((_loopArray).size < 1) break; /*NEW*/\
				} \
			} \
		}


/*	--------------------------------------------------------------------------------
	Function 	: LA_ENDFOREACHDO
	Purpose 	: do something (eg. call a method) on each used element in the loop-array
	Parameters 	: name of loop-array to traverse, action to perform on each element (eg. 'init()')
	Value	 	: 
	Info 		: nice & lazy
*/
#define LA_FOREACHDO(_loopArray, _action) \
		{ \
			if((_loopArray).youngest != -1) \
			{ \
				for(int32 _i=(_loopArray).oldest;;_i--) \
				{ \
					if(_i < 0) \
					{ \
						_i=(_loopArray).totalArraySize-1; \
					} \
					\
					(_loopArray).items[_i]._action; \
					\
					if(_i == (_loopArray).youngest) break; \
				} \
			} \
		}


/*	--------------------------------------------------------------------------------
	Function 	: LA_FORARRAY
	Purpose 	: start a loop-array traversal block
	Parameters 	: name of loop-array to traverse, type held by loop-array,
					name of element pointer to be declared and pointed at each element in turn
	Value	 	: 
	Info 		: ************ THIS TRAVERSES ALL ARRAY ELEMENTS
								- WHETHER THEY'RE USED OR NOT!!!!! *************

			************* PROBABLY NOT THE ONE YOU WANT TO USE *************
*/
#define LA_FORARRAY(_loopArray, _type, _ptr) \
		{ \
			_type*				_ptr; \
			\
			for(uint32 _i=0; _i<(_loopArray).totalArraySize; _i++) \
			{ \
				_ptr= &((_loopArray).items[_i]);


/*	--------------------------------------------------------------------------------
	Function 	: LA_ENDFORARRAY
	Purpose 	: end a loop-array traversal block
	Parameters 	: name of loop-array to traverse
	Value	 	: 
	Info 		: ************ LA_FORARRAY TRAVERSES ALL ARRAY ELEMENTS
								- WHETHER THEY'RE USED OR NOT!!!!! *************

			************* PROBABLY NOT THE ONE YOU WANT TO USE *************
*/
#define LA_ENDFORARRAY \
			} \
		}


/*	--------------------------------------------------------------------------------
	Function 	: LA_FORARRAYDO
	Purpose 	: do something (eg. call a method) on each used element in the loop-array
	Parameters 	: name of loop-array to traverse, action to perform on each element (eg. 'init()')
	Value	 	: 
	Info 		: ************ THIS PERFORMS THE SPECIFIED ACTION ON ALL ARRAY ELEMENTS
								- WHETHER THEY'RE USED OR NOT!!!!! *************

			************* PROBABLY NOT THE ONE YOU WANT TO USE *************
*/
#define LA_FORARRAYDO(_loopArray, _action) \
		{ \
			for(uint32 _i=0; _i<(_loopArray).totalArraySize; _i++) \
			{ \
				(_loopArray).items[_i]._action; \
			} \
		}


#define LOOPARRAY_NAME_LENGTH		256						// PP: maximum length of loop-array debug name

#define LOOPARRAY_DEFAULT_NAME		"<Nameless Loop-Array>"	// PP: default name given to loop-arrays if none is specified

				
// PP: Looping array class
// PP: T is the data type to be held, S is the maximum number of elements in the list
template <typename T, uint32 S> class LOOPARRAY
{
public:

#ifdef LOOPARRAYDEBUG
	char		name[LOOPARRAY_NAME_LENGTH];	// PP: name of loop-array, for debugging
#endif // PP: def LOOPARRAYDEBUG

	// PP: NOTE: All these members have to be accessible for the benefit of macros LA_FOREACH & LA_ENDFOREACH. hmm...

	T			items[S];						// PP: array of items											
	uint32		totalArraySize;					// PP: total array size
	int32		size;							// PP: number of used elements
												
	int32		oldest;							// PP: index of oldest element
	int32		youngest;						// PP: index of youngest element


	/*	--------------------------------------------------------------------------------
		Function 	: LOOPARRAY::init
		Purpose 	: initialise the looping array
		Parameters 	: (opt/NULL=default) name, for debugging (only used ifdef LOOPARRAYDEBUG)
		Returns 	: 
		Info 		: 
	*/
	void		init(const char* const nameIn=NULL)
	{
		this->oldest=this->youngest= -1;
		this->totalArraySize=S;
		this->size=0;

#ifdef LOOPARRAYDEBUG
		if(nameIn == NULL)
		{
			STRNCPY(this->name, LOOPARRAY_DEFAULT_NAME, LOOPARRAY_NAME_LENGTH);
		}
		else
		{
			STRNCPY(this->name, nameIn, LOOPARRAY_NAME_LENGTH);
		}
#endif // PP: def LOOPARRAYDEBUG
	}


	/*	--------------------------------------------------------------------------------
		Function 	: LOOPARRAY::LOOPARRAY
		Purpose 	: [default] constructor: calls init why not
		Parameters 	: (opt/NULL=default) name
		Returns 	: 
		Info 		: 
	*/
	LOOPARRAY(const char* const nameIn=NULL)
	{
		this->init(nameIn);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: LOOPARRAY::getNew
		Purpose 	: get a pointer to a new ('youngest') element of the array
		Parameters 	: (opt/1) number of items to make new
		Returns 	: pointer to the new item, OR NULL IF MORE THAN 1 ITEM IS SPECIFIED!  This will save confusion.
		Info 		: if the list is full, the oldest item will be returned for overwriting.
						Expect silly results if you pass in a number bigger than the maximum array size!
						Also expect shame.
	*/
	T*			getNew(const uint32 numNew=1)
	{
		if(this->youngest == -1)
		{
			this->oldest=this->totalArraySize-1;
			this->youngest=this->oldest-(numNew-1);
		}
		else
		{
			int32 lastYoungest=this->youngest;
			int32 tempLastYoungest=this->youngest;

			this->youngest -= numNew;

			int32 tempNewYoungest=this->youngest;

			if(this->youngest < 0)
			{
				if(this->oldest > tempLastYoungest)
				{
					tempLastYoungest+=this->totalArraySize;
					tempNewYoungest+=this->totalArraySize;
				}

				this->youngest+=this->totalArraySize;
			}

			int32 overflow;
			
			if(tempLastYoungest > this->oldest)
			{
				overflow=(this->oldest-tempNewYoungest)+1;
			}
			else
			{
				overflow=0;
			}

			if(overflow > 0)
			{
				// PP: ARRAY TOO SMALL - OLDEST ITEM LOST TO THE WIND!

#ifdef LOOPARRAYDEBUG
				bkPrintf("LOOPARRAY::getNew(%d): loop-array \"%s\" is overflowing by %d! (size %d total %d)\n", numNew, this->name, overflow, this->size, this->totalArraySize);
#endif // PP: def LOOPARRAYDEBUG

				this->discardOldest(overflow);
			}
		}

		this->size += numNew;

		if(numNew == 1)
		{
			return &(this->items[this->youngest]);
		}
		else
		{
			return NULL;
		}
	}


	/*	--------------------------------------------------------------------------------
		Function 	: LOOPARRAY::getYoungest
		Purpose 	: get a pointer to the current youngest element of the array
		Parameters 	: (opt/0) offset towards old end
		Returns 	: 
		Info 		: 
	*/
	inline T*	getYoungest(const int32 offset=0)
	{
		if(this->youngest == -1)
		{
			return NULL;
		}
		else
		{
			uint32		index;

			index=this->youngest;

			if(offset > 0)
			{
				index+=offset;

				if(index >= this->totalArraySize)
				{
					index=index-this->totalArraySize;
				}
			}

			return &(this->items[index]);
		}
	}


	/*	--------------------------------------------------------------------------------
		Function 	: LOOPARRAY::getOldest
		Purpose 	: get a pointer to the current oldest element of the array
		Parameters 	: (opt/0) offset towards young end
		Returns 	: 
		Info 		: 
	*/
	inline T*	getOldest(const uint32 offset=0)
	{
		if(this->oldest == -1)
		{
			return NULL;
		}
		else
		{
			int32		index;

			index=this->oldest;

			if(offset > 0)
			{
				index-=offset;

				if(index < 0)
				{
					index=this->totalArraySize+index;
				}
			}

			return &(this->items[index]);
		}
	}


	/*	--------------------------------------------------------------------------------
		Function 	: LOOPARRAY::getItem
		Purpose 	: get a pointer to the item that would be encountered at the specified
						position in an LA_FOREACH traversal
		Parameters 	: sequential position of item
		Returns 	: ptr to item
		Info 		: calls getNew if index is out of bounds
	*/
	inline T*	getItem(const int32 index)
	{
		int32 actualIndex;

		if(index >= this->size)
		{
			this->getNew((index-this->size)+1);
		}

		actualIndex=this->oldest-index;

		if(actualIndex < 0)
		{
			actualIndex+=this->totalArraySize;
		}

		return &(this->items[actualIndex]);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: LOOPARRAY::discardOldest
		Purpose 	: discard the oldest element of the array from the used portion
		Parameters 	: (opt/1) number of oldest elements to discard
		Returns 	: 
		Info 		: call this whenever you find that your oldest element has expired - 
						this will keep traversal times to a minimum.
	*/
	void	discardOldest(const uint32 numToDiscard=1)
	{
		if(this->oldest != -1)
		{
			this->oldest -= numToDiscard;

			if(this->oldest < 0)
			{
				this->oldest=this->totalArraySize+(this->oldest);
			}
			
			this->size -= numToDiscard;

			if(this->size <= 0)
			{
				this->size=0;
				this->oldest=this->youngest= -1;
			}
		}
	}


	/*	--------------------------------------------------------------------------------
		Function 	: LOOPARRAY::copyToBuffer
		Purpose 	: write the loop-array's contents into a contiguous buffer
		Parameters 	: ptr to buffer to which to write
		Returns 	: 
		Info 		: need to do this eg. if drawing verts held by a loop array
	*/
	void copyToBuffer(T* const buffer)
	{
		uint32		index=0;

		LA_FOREACH(*this, T, item)

			buffer[index++]= *item;

		LA_ENDFOREACH(*this)
	}


	/*	--------------------------------------------------------------------------------
		Function 	: LOOPARRAY::operator []
		Purpose 	: get a ref to the item at a specified LOGICAL position in the array
		Parameters 	: index
		Returns 	: a ref to the item at the specified LOGICAL position in the array!
		Info 		: acts like getNew if index is out of bounds - handy no?
	*/
	inline T& operator [] (const uint32 index)
	{
		return *(this->getItem(index));
	}


}BALIGN16;


#endif // PP: ndef #define __H_LOOPARRAY