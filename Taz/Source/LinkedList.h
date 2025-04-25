// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : LinkedList.h
//   Purpose : Linked List class - please use it!
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#include "global.h"
#include "main.h"

// PP: this define enables linked-list debugging
#define LINKEDLISTDEBUG		// PP: UNREMOUT: why not? It's handy now & again

void breakpoint();


template <typename T> class LINKEDLIST
{
public:

#ifdef LINKEDLISTDEBUG
	char						name[32];			// PP: name of list, for debugging// PP: comes first for 'watch'ability
#endif

private:

	union
	{
		T*						item;				// PP: pointer to data (in nodes other than the head node)
		LINKEDLIST*				last;				// PP: pointer to the last node in the list (in head node)
	}itemLast;										
													
	LINKEDLIST*					next;				// PP: pointer to the next node in the list
													
	LINKEDLIST*					lastAccessedNode;	// PP: pointer to the last node that was 'got' from the list (in head node).  In nodes other than the head node this is currently wasted space.
	int32						lastAccessedIndex;	// PP: index of the last node that was 'got' from the list (in head node).  In nodes other than the head node this is currently wasted space.


	/*	--------------------------------------------------------------------------------
		Function 	: LINKEDLIST::getNode
		Purpose 	: get a pointer to the node at a specified index
		Parameters 	: node index
		Returns 	: pointer to the specified node, or NULL if not found
		Info 		: PRIVATE! DO NOT CALL THIS!
	*/
	LINKEDLIST*	getNode(const int pos);// PP: can't be const, right?!


	/*	--------------------------------------------------------------------------------
		Function 	: LINKEDLIST::getNode
		Purpose 	: get a pointer to the node pointing at a specified address
		Parameters 	: address to look for
		Returns 	: pointer to the specified node, or NULL if not found
		Info 		: PRIVATE! DO NOT CALL THIS!
	*/
	LINKEDLIST*	getNode(const T* const ptr);// PP: can't be const, right?!


	/*	--------------------------------------------------------------------------------
		Function 	: LINKEDLIST::removeNode
		Purpose 	: remove a node from the list
		Parameters 	: pointer to node to remove
		Returns 	: 
		Info 		: PRIVATE! DO NOT CALL THIS!
	*/
	void			removeNode(LINKEDLIST* const list);


	/*	--------------------------------------------------------------------------------
		Function 	: LINKEDLIST::LINKEDLIST
		Purpose 	: constructor for nodes other than the head node
		Parameters 	: ptrs to nodes after which and before which to insert the node
		Returns 	: new LINKEDLIST instance
		Info 		: PRIVATE! DO NOT CALL THIS!
	*/
					LINKEDLIST(LINKEDLIST* const prv, T* const ptr, LINKEDLIST* const nxt);


	union
	{
		// PP: "Getting *friendly* with oneself are we Sir?  *Ooh* Sir!  Suit you Sir!"
		friend 	LINKEDLIST;														// PP: I konw this seems odd, but just you try taking it out - I dare ya, ya pesky varmint!
		// PP: "Well actually, I'm just looking for a brown suit."
private:
		// PP: "Brown, Sir?"  "Brown, Kenneth?"
		LINKEDLIST*		prev;													// PP: pointer to previous node (in nodes other than the head node)
		// PP: "OOH!"
public:
		// PP: "Your *lady*, Sir, does she...*want* it Sir?"  "(Ooh!)"
		long			size;																// PP: size of the list (in head node)
		// PP: "I beg your pardon?!"
	}sizePrev;
		// PP: "Does your *lady* want the brown suit Sir?"
		// PP: "Er, no, it's for me."
		// PP: "The Gentleman does seem to *treat* himself rather well, doesn't he Kenneth?"
		// PP: "OOH!"


public:


	/*	--------------------------------------------------------------------------------
		Function 	: LINKEDLIST::LINKEDLIST
		Purpose 	: [default] constructor
		Parameters 	: 
		Returns 	: new LINKEDLIST instance
		Info 		: 
	*/
					LINKEDLIST(char * const name = NULL);


	/*	--------------------------------------------------------------------------------
		Function 	: LINKEDLIST::setName
		Purpose 	: set the name of the linked list
		Parameters 	: name format string, name argument list
		Returns 	: 
		Info 		: 
	*/
	void			setName(const char* const nameFmt, ...);


	/*	--------------------------------------------------------------------------------
		Function 	: LINKEDLIST::init
		Purpose 	: initialise the list
		Parameters 	: (opt/NULL=none) name format string, optional argument list
		Returns 	: 
		Info 		: this is called by the constructor anyway
	*/
	void			init(const char* const nameFmt=NULL, ...);


	/*	--------------------------------------------------------------------------------
		Function 	: LINKEDLIST::get
		Purpose 	: get the data at a specified index
		Parameters 	: element index
		Returns 	: pointer to the specified data, or NULL if the index is invalid
		Info 		: 
	*/
	T*			get(const int pos);// PP: can't be const, right?!


	/*	--------------------------------------------------------------------------------
		Function 	: LINKEDLIST::set
		Purpose 	: set the data at a specified index
		Parameters 	: element index, new data ptr for that index
		Returns 	: the new data ptr passed in
		Info 		: 
	*/
	T*			set(const int pos, T* const item);// PP: can't be const, right?!


	/*	--------------------------------------------------------------------------------
		Function 	: LINKEDLIST::free
		Purpose 	: delete an item and null its pointer
		Parameters 	: index of element to free and null
		Returns 	: 
		Info 		: // PP: wanted to call this 'free' but couldn't
	*/
	void		safeDelete(const int pos);// PP: can't be const, right?!


	/*	--------------------------------------------------------------------------------
		Function 	: LINKEDLIST::operator []
		Purpose 	: get the data at a specified index
		Parameters 	: element index
		Returns 	: ref to the specified data, or NULL if the index is invalid
		Info 		: 
	*/
	inline T&		operator [] (const int pos)// PP: can't be const, right?!
	{
		int		shortfall;

		shortfall=(pos-this->getSize())+1;

		// PP: add the items that don't exist yet
		for(int i=0; i<shortfall; i++)
		{
			this->append(new T);
		}


		return *(this->get(pos));
	}


	/*	--------------------------------------------------------------------------------
		Function 	: LINKEDLIST::getFirst
		Purpose 	: get the first item in the list
		Parameters 	: 
		Returns 	: pointer to item at index 0
		Info 		: 
	*/
	T*			getFirst(void) const;


	/*	--------------------------------------------------------------------------------
		Function 	: LINKEDLIST::getLast
		Purpose 	: get the last item in the list
		Parameters 	: 
		Returns 	: pointer to item at last index
		Info 		: 
	*/
	T*			getLast(void) const;


	/*	--------------------------------------------------------------------------------
		Function 	: LINKEDLIST::getSize
		Purpose 	: get the number of elements in the list
		Parameters 	: 
		Returns 	: number of elements in the list
		Info 		: 
	*/
	inline int getSize(void) const
	{
		return this->sizePrev.size;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: LINKEDLIST::append
		Purpose 	: append an element onto the list
		Parameters 	: pointer to data to append
		Returns 	: just what you passed in (I'm not sure why that's useful but it must have seemed like a good idea at the time)
		Info 		: 
	*/
	T*			append(T* const ptr);


	/*	--------------------------------------------------------------------------------
		Function 	: LINKEDLIST::remove
		Purpose 	: remove an element from the list
		Parameters 	: index of element to remove
		Returns 	: 
		Info 		: 
	*/
	void			remove(const int pos);// PP:  const;


	/*	--------------------------------------------------------------------------------
		Function 	: LINKEDLIST::remove
		Purpose 	: remove an element from the list
		Parameters 	: pointer to data whose node is to be removed
		Returns 	: 
		Info 		: 
	*/
	void			remove(const T* const ptr);


	/*	--------------------------------------------------------------------------------
		Function 	: LINKEDLIST::shutdown
		Purpose 	: free up space used by the list (excluding the head)
		Parameters 	: 
		Returns 	: 
		Info 		: this deletes all nodes but leaves their items alone
	*/
	void			shutdown(void);


	/*	--------------------------------------------------------------------------------
		Function 	: LINKEDLIST::kill
		Purpose 	: free up space used by the list, and by data pointed to by the list
		Parameters 	: 
		Returns 	: 
		Info 		: this deletes all nodes and their items
	*/
	void			kill(void);


};


/*	--------------------------------------------------------------------------------
	Function 	: LINKEDLIST::setName
	Purpose 	: set the name of the linked list
	Parameters 	: name format string, name argument list
	Returns 	: 
	Info 		: 
*/
template <typename T> void LINKEDLIST<T>::setName(const char* const nameFmt, ...)
{
#ifdef LINKEDLISTDEBUG

	va_list			argp;

	va_start(argp, nameFmt);
	vsprintf(this->name, nameFmt, argp);
	va_end(argp);

#endif // PP: def LINKEDLISTDEBUG
}


/*	--------------------------------------------------------------------------------
	Function 	: LINKEDLIST::init
	Purpose 	: initialise the list
	Parameters 	: (opt/NULL=none) name format string, optional argument list
	Returns 	: 
	Info 		: this is called by the constructor anyway
*/
template <typename T> void LINKEDLIST<T>::init(const char* const nameFmt, ...)
{
#ifdef LINKEDLISTDEBUG

	if(nameFmt != NULL)// PP: if a name was specified
	{
		// PP: read in the name...


		va_list			argp;

		va_start(argp, nameFmt);
		vsprintf(this->name, nameFmt, argp);
		va_end(argp);
	}

#endif // PP: def LINKEDLISTDEBUG

	this->next=this;					//end of list points to beginning
	this->itemLast.last=this;			//beginning of list points to end
	this->sizePrev.size=0;				//size is zero
	this->lastAccessedNode=NULL;		// PP: no nodes have been accessed yet
	this->lastAccessedIndex= -2;		// PP: no nodes have been accessed yet// PP: -2 because -1+1=0 which is a valid index
}


/*	--------------------------------------------------------------------------------
	Function 	: LINKEDLIST::getNode
	Purpose 	: get a pointer to the node at a specified index
	Parameters 	: node index
	Returns 	: pointer to the specified node, or NULL if not found
	Info 		: PRIVATE! DO NOT CALL THIS!
*/
template <typename T> LINKEDLIST<T>* LINKEDLIST<T>::getNode(const int pos)// PP: can't be const, right?!
{
	if(pos == (this->lastAccessedIndex-1))// PP: *** THIS IS NEARLY ALWAYS THE CASE ***
	{
		this->lastAccessedIndex=pos;
		return (this->lastAccessedNode=this->lastAccessedNode->sizePrev.prev);
	}
	else if(pos == this->lastAccessedIndex)
	{
		return this->lastAccessedNode;
	}
	else if(pos == (this->lastAccessedIndex+1))
	{
		this->lastAccessedIndex=pos;
		return (this->lastAccessedNode=this->lastAccessedNode->next);
	}
	else if(pos == (this->sizePrev.size-1))
	{
		this->lastAccessedIndex=pos;
		return (this->lastAccessedNode=this->itemLast.last);
	}
	else
	{
		LINKEDLIST*		tempList;

		// PP: go through the list to get the node at the specified position
		
		tempList=this->next;

		for(int i=pos-1;i>=0;i--)
		{
			tempList=tempList->next;
		}

		this->lastAccessedIndex=pos;
		return (this->lastAccessedNode=tempList);
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: LINKEDLIST::getNode
	Purpose 	: get a pointer to the node pointing at a specified address
	Parameters 	: address to look for
	Returns 	: pointer to the specified node, or NULL if not found
	Info 		: PRIVATE! DO NOT CALL THIS!
*/
template <typename T> LINKEDLIST<T>* LINKEDLIST<T>::getNode(const T* const ptr)// PP: can't be const, right?!
{
	LINKEDLIST* tempList;

	for(tempList=this->next;
		tempList != this;
		tempList=tempList->next)
	{
		if (tempList->itemLast.item == ptr)
		{
			return tempList;
		}
	}

	return NULL;//not found
}


/*	--------------------------------------------------------------------------------
	Function 	: LINKEDLIST::getFirst
	Purpose 	: get the first item in the list
	Parameters 	: 
	Returns 	: pointer to item at index 0
	Info 		: 
*/
template <typename T> inline T* LINKEDLIST<T>::getFirst(void) const
{
	if(this->sizePrev.size > 0)
	{
		// PP: return item of first node after head node
		return this->next->itemLast.item;
	}
	else
	{
		// PP: there are no items in the list; return NULL
		return NULL;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: LINKEDLIST::getLast
	Purpose 	: get the last item in the list
	Parameters 	: 
	Returns 	: pointer to item at last index
	Info 		: 
*/
template <typename T> inline T* LINKEDLIST<T>::getLast(void) const
{
	if(this->sizePrev.size > 0)
	{
		// PP: last node is not head node; return last node's item
		return this->itemLast.last->itemLast.item;
	}
	else
	{
		// PP: there are no items in the list; return NULL
		return NULL;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: LINKEDLIST::get
	Purpose 	: get the data at a specified index
	Parameters 	: element index
	Returns 	: pointer to the specified data, or NULL if the index is invalid
	Info 		: 
*/
template <typename T> inline T* LINKEDLIST<T>::get(const int pos)// PP: can't be const, right?!
{
	if(pos < 0)
	{
		return NULL;
	}
	else if(pos >= this->sizePrev.size)
	{
		return NULL;
	}
	else
	{
		return this->getNode(pos)->itemLast.item;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: LINKEDLIST::set
	Purpose 	: set the data at a specified index
	Parameters 	: element index, new data ptr for that index
	Returns 	: the new data ptr passed in
	Info 		: 
*/
template <typename T> T* LINKEDLIST<T>::set(const int pos, T* const item)// PP: can't be const, right?!
{
	if(pos < 0)
	{
		return NULL;
	}
	else if(pos >= this->sizePrev.size)
	{
		return NULL;
	}
	else
	{
		return (this->getNode(pos)->itemLast.item = item);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: LINKEDLIST::safeDelete
	Purpose 	: delete an item and null its pointer
	Parameters 	: index of element to free and null
	Returns 	: 
	Info 		: // PP: wanted to call this 'free' but couldn't
*/
template <typename T> void LINKEDLIST<T>::safeDelete(const int pos)// PP: can't be const, right?!
{
	if(pos < 0)
	{
		return;
	}
	else if(pos >= this->sizePrev.size)
	{
		return;
	}
	else
	{
		LINKEDLIST*		tempList;

		tempList=this->getNode(pos);

		SAFE_DELETE(tempList->itemLast.item);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: LINKEDLIST::append
	Purpose 	: append an element onto the list
	Parameters 	: pointer to data to append
	Returns 	: just what you passed in (I'm not sure why that's useful but it must have seemed like a good idea at the time)
	Info 		: 
*/
template <typename T> T* LINKEDLIST<T>::append(T* const ptr)
{
#ifdef LINKEDLISTDEBUG
	bkHeapGroupPush(name);
#endif
	this->itemLast.last->next=new LINKEDLIST(this->itemLast.last, ptr, this);//prev, item, next
#ifdef LINKEDLISTDEBUG

	// PP: print the name of the list and the address of the node just added
// PP: REMOUT: would get very annoying very quickly			bkPrintf("############################################ LINKEDLIST::append \"%s\" node %x\n", this->name, this->itemLast.last->next);

	bkHeapGroupPop();
#endif
	ASSERT(this->itemLast.last->next);

	this->itemLast.last=this->itemLast.last->next;
	this->sizePrev.size++;

	return ptr;
}


/*	--------------------------------------------------------------------------------
	Function 	: LINKEDLIST::removeNode
	Purpose 	: remove a node from the list
	Parameters 	: pointer to node to remove
	Returns 	: 
	Info 		: PRIVATE! DO NOT CALL THIS!
*/
template <typename T> void LINKEDLIST<T>::removeNode(LINKEDLIST<T>* const list)
{
	if(list == this->itemLast.last)
	{
		this->itemLast.last=list->sizePrev.prev;
		this->itemLast.last->next=this;
	}
	else
	{
		list->next->sizePrev.prev=list->sizePrev.prev;
	}

	if(list == this->lastAccessedNode)
	{
		this->lastAccessedNode=NULL;
		this->lastAccessedIndex=-2;// PP: -2 because -1+1=0 which is a valid index
	}

	list->sizePrev.prev->next=list->next;

	delete list;

	this->sizePrev.size--;
}


/*	--------------------------------------------------------------------------------
	Function 	: LINKEDLIST::remove
	Purpose 	: remove an element from the list
	Parameters 	: index of element to remove
	Returns 	: 
	Info 		: 
*/
template <typename T> void LINKEDLIST<T>::remove(const int pos)// PP:  const
{
	this->removeNode(this->getNode(pos));
}


/*	--------------------------------------------------------------------------------
	Function 	: LINKEDLIST::remove
	Purpose 	: remove an element from the list
	Parameters 	: pointer to data whose node is to be removed
	Returns 	: 
	Info 		: 
*/
template <typename T> void LINKEDLIST<T>::remove(const T* const ptr)
{
	LINKEDLIST* tempList=this->getNode(ptr);

	if(tempList != NULL)
	{
		this->removeNode(tempList);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: LINKEDLIST::shutdown
	Purpose 	: free up space used by the list (excluding the head)
	Parameters 	: 
	Returns 	: 
	Info 		: this deletes all nodes but leaves their items alone
*/
template <typename T> void LINKEDLIST<T>::shutdown(void)
{
	LINKEDLIST<T>*	tempList;
	LINKEDLIST<T>* prevList;

	if(this == NULL) return;

	for(tempList=this->itemLast.last; tempList!=this; tempList=prevList)
	{
		prevList=tempList->sizePrev.prev;
		SAFE_DELETE(prevList->next);
	}

	this->init();
}


/*	--------------------------------------------------------------------------------
	Function 	: LINKEDLIST::kill
	Purpose 	: free up space used by the list, and by data pointed to by the list
	Parameters 	: 
	Returns 	: 
	Info 		: this deletes all nodes and their items
*/
template <typename T> void LINKEDLIST<T>::kill(void)
{
	LINKEDLIST*	tempList;
	LINKEDLIST*	tempList2;

	if(this == NULL) return;

	for(tempList=this->itemLast.last; tempList!=this; tempList=tempList2)
	{
		SAFE_DELETE(tempList->itemLast.item);
			
		tempList2=tempList->sizePrev.prev;
		
		SAFE_DELETE(tempList2->next);
	}

	this->init();
}


/*	--------------------------------------------------------------------------------
	Function 	: LINKEDLIST::LINKEDLIST
	Purpose 	: [default] constructor
	Parameters 	: (opt/NULL=none) name
	Returns 	: new LINKEDLIST instance
	Info 		: 
*/
template <typename T> LINKEDLIST<T>::LINKEDLIST<T>(char * const name)
{
#ifdef LINKEDLISTDEBUG
	if (name) strcpy(this->name,name);
	else this->name[0]=0;
#endif
	this->init();
}


/*	--------------------------------------------------------------------------------
	Function 	: LINKEDLIST::TAG_LINKEDLIST
	Purpose 	: constructor for nodes other than the head node
	Parameters 	: ptrs to nodes after which and before which to insert the node
	Returns 	: new LINKEDLIST instance
	Info 		: PRIVATE! DO NOT CALL THIS!
*/
template <typename T> LINKEDLIST<T>::LINKEDLIST<T>(LINKEDLIST<T>* const prv, T* const ptr, LINKEDLIST<T>* const nxt)
{
	this->sizePrev.prev=prv;
	this->itemLast.item=ptr;
	this->next=nxt;
}

#endif //ndef __LINKEDLIST_H__