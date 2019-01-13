/***
 * The content of this file or document is CONFIDENTIAL and PROPRIETARY
 * to ChengZhen(Anyou).  It is subject to the terms of a
 * License Agreement between Licensee and ChengZhen(Anyou).
 * restricting among other things, the use, reproduction, distribution
 * and transfer.  Each of the embodiments, including this information and
 * any derivative work shall retain this copyright notice.
 *
 * Copyright (c) 2014-2015 ChengZhen(Anyou). All Rights Reserved.
 *
 */
#ifndef _NS_VISION_CORE_BASE_LIST_H
#define _NS_VISION_CORE_BASE_LIST_H

#include "base_types.h"

namespace core {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// LinkedList class

/** 
 * 一个简单的双向链接列表模板类.
 * List template class. 
 *
 * @author ChengZhen (anyou@msn.com)
 */
template<class TYPE, class ARG_TYPE = const TYPE&>
class LinkedList
{
protected:
	/** List Note. */
	class ListNode 
	{
	public:
		ListNode() 
		{
			fNext = NULL;
			fPrev = NULL;
			fData = TYPE();
		}

	public:
		ListNode* fNext;	///< Point to the next node
		ListNode* fPrev;	///< Point to the fPrev node
		TYPE	  fData;	///< Note fData
	};

public:
	class ListIterator 
	{
	public:
		/** 构建方法. */
		ListIterator(LinkedList* list) : fList(list) {
			fPrevNode		= NULL;
			fCurrentNode	= NULL;
			fNextNode		= list ? list->fStartNode : NULL;
		}

		/** 构建方法. */
		ListIterator(const ListIterator& l)  {
			operator = (l);
		}

		~ListIterator() {}

		/** 重载操作符. */
		ListIterator& operator = (const ListIterator& right) 
		{
			if (this == &right) {
				return *this;
			}

			fCurrentNode	= right.fCurrentNode;
			fList			= right.fList;
			fNextNode		= right.fNextNode;
			fPrevNode		= right.fPrevNode;

			return *this;
		}

		void Add(ARG_TYPE newElement) {
			if (fCurrentNode) {
				fList->Add(fCurrentNode, newElement);
				fPrevNode = fCurrentNode->fPrev;

			} else if (fNextNode) {
				fList->Add(fNextNode, newElement);
				fPrevNode = fNextNode->fPrev;

			} else {
				fList->AddLast(newElement);
				fPrevNode = fList->fEndNode;
			}
		}

		BOOL HasNext() {
			if (fNextNode == NULL) {
				return FALSE;
			}

			return TRUE;
		}

		BOOL HasPrev() {
			if (fPrevNode == NULL) {
				return FALSE;
			}

			return TRUE;
		}

		TYPE& Next() {
			if (fCurrentNode) {
				fPrevNode = fCurrentNode;
			}

			fCurrentNode = fNextNode;
			if (fCurrentNode) {
				fNextNode = fCurrentNode->fNext;
			}

			return fCurrentNode->fData;
		}

		TYPE& Prev() {
			if (fCurrentNode) {
				fNextNode = fCurrentNode;
			}

			fCurrentNode = fPrevNode;
			if (fCurrentNode) {
				fPrevNode = fCurrentNode->fPrev;
			}

			return fCurrentNode->fData;
		}

		void Remove() {
			if (fCurrentNode) {
				fList->Remove(fCurrentNode);
				fCurrentNode = NULL;
			}
		}

		void Set(ARG_TYPE newElement) {
			if (fCurrentNode) {
				fCurrentNode->fData = newElement;
			}
		}

	private:
		LinkedList* fList;
		ListNode* fPrevNode;
		ListNode* fCurrentNode;
		ListNode* fNextNode;
	};

public:
// Construction -----------------------------------------------
	LinkedList();
	~LinkedList();

	typedef const TYPE& ConstReference;		///< 常量引用类型
	typedef TYPE&	 Reference;				///< 引用类型
	typedef TYPE	 Type;					///< 值类型

// Attributes ------------------------------------------------
public:
	ListIterator GetListIterator() { return ListIterator(this); }

	const TYPE GetFirst() const;	///< TODO:
	const TYPE GetLast() const;	///< TODO:

	TYPE& GetFirst();	///< TODO:
	TYPE& GetLast();	///< TODO:
	int   GetSize() const;	///< TODO:
	BOOL  IsEmpty() const;	///< TODO:

// Operations -------------------------------------------------
public:
	void Remove(ListNode* node);	///< TODO:
	void Add(ListNode* node, ARG_TYPE newElement);	///< TODO:
	void AddFirst(ARG_TYPE newElement);	///< TODO:
	void AddLast (ARG_TYPE newElement);	///< TODO:
	void Clear();	///< TODO:
	void Remove(ARG_TYPE value);	///< TODO:
	void RemoveFirst();	///< TODO:
	void RemoveLast();	///< TODO:

// Implementation ---------------------------------------------
private:
	ListNode* NewNode(ListNode* node1, ListNode* node2);
	void FreeNode(ListNode* node);

// Data Members -----------------------------------------------
private:
	ListNode* fStartNode;		///< Point to the head node
	ListNode* fEndNode;			///< Point to the tail node
	ListNode* fFreeNodes;		///< Point to the tail node

	int	 fFreeNodeCount;	///< 
	int  fSize;				///< ListNode Count
};

// ############################################################################
// List<TYPE, ARG_TYPE> inline functions

#define LIST_TEMPLATE template<class TYPE, class ARG_TYPE>
#define LIST_TYPE LinkedList<TYPE, ARG_TYPE>

LIST_TEMPLATE LIST_TYPE::LinkedList()
{
	fEndNode		= NULL;
	fFreeNodeCount	= 0;
	fFreeNodes		= NULL;
	fSize			= 0;
	fStartNode		= NULL;
}

LIST_TEMPLATE LIST_TYPE::~LinkedList()
{
	Clear();
}

/** Adds an element to the End of a list. */
LIST_TEMPLATE void LIST_TYPE::Add(ListNode* currentNode, ARG_TYPE newElement)
{
	if (currentNode == NULL) {
		return;

	} else if (currentNode == fStartNode) {
		return AddFirst(newElement);
	}

	ListNode* prevNode = currentNode->fPrev;
	ListNode* newNode  = NewNode(prevNode, currentNode);
	newNode->fData = newElement;

	if (prevNode) {
		prevNode->fNext = newNode;
	}

	currentNode->fPrev = newNode;
	fSize++;
}

/** Adds an element to the beginning of a list. */
LIST_TEMPLATE void LIST_TYPE::AddFirst(ARG_TYPE newElement)
{
	ListNode* newNode = NewNode(NULL, fStartNode);
	newNode->fData = newElement;

	if (fStartNode != NULL) {
		fStartNode->fPrev = newNode;
	}

	fStartNode = newNode;
	++fSize;
}

/** Adds an element to the End of a list. */
LIST_TEMPLATE void LIST_TYPE::AddLast(ARG_TYPE newElement)
{
	ListNode* newNode = NewNode(fEndNode, NULL);
	newNode->fData = newElement;

	if (fEndNode) {
		fEndNode->fNext = newNode;
	}

	fEndNode = newNode;

	if (fStartNode == NULL) {
		fStartNode = newNode;
	}

	++fSize;
}

/** Erases all the fSize of a list. */
LIST_TEMPLATE void LIST_TYPE::Clear()
{
	ListNode* nextNode = fStartNode;
	
	fStartNode		= NULL;
	fEndNode		= NULL;
	fSize			= 0;
	fFreeNodeCount	= 0;

	// clear nodes
	while (nextNode) {
		ListNode* freeNode = nextNode;
		nextNode = freeNode->fNext;

		freeNode->fPrev = NULL;
		freeNode->fNext = NULL;
		delete freeNode;
	}

	// clear free nodes
	nextNode = fFreeNodes;
	fFreeNodes = NULL;

	while (nextNode) {
		ListNode* freeNode = nextNode;
		nextNode = freeNode->fNext;

		delete freeNode;
		freeNode = NULL;
	}
}

/**
 * 
 * @param pNode 
 * @return LIST_TEMPLATE void 
 */
LIST_TEMPLATE void LIST_TYPE::FreeNode(ListNode* pNode)
{
	if (pNode == NULL) {
		return;

#if 1
	} else if (fFreeNodeCount < 500) {
		pNode->fPrev = NULL;
		pNode->fData = TYPE();
		pNode->fNext = fFreeNodes;
		fFreeNodes = pNode;
		fFreeNodeCount++;
#endif

	} else {
		delete pNode;
		pNode = NULL;
	}
}

/** Returns the number of fSize in a list. */
LIST_TEMPLATE int LIST_TYPE::GetSize() const 
{ 
	return fSize; 
}

/** Returns a reference to the first element in a list. */
LIST_TEMPLATE TYPE& LIST_TYPE::GetFirst() 
{
	if (fStartNode == NULL) {
		printf("Empty List!");
	}

	return fStartNode->fData; 
}

/** Returns a reference to the first element in a list. */
LIST_TEMPLATE const TYPE LIST_TYPE::GetFirst() const 
{
	if (fStartNode == NULL) {
		printf("Empty List!");
	}

	return fStartNode->fData; 
}

/** Returns a reference to the last element of a list. */
LIST_TEMPLATE TYPE& LIST_TYPE::GetLast() 
{ 
	if (fEndNode == NULL) {
		printf("Empty List!");
	}

	return fEndNode->fData; 
}

/** Returns a reference to the last element of a list. */
LIST_TEMPLATE const TYPE LIST_TYPE::GetLast() const 
{ 
	if (fEndNode == NULL) {
		printf("Empty List!");
	}

	return fEndNode->fData; 
}

/** Tests if a list is empty. */
LIST_TEMPLATE BOOL LIST_TYPE::IsEmpty() const 
{ 
	return (fSize == 0); 
}

LIST_TEMPLATE typename LIST_TYPE::ListNode*
LIST_TYPE::NewNode(ListNode* prevNode, ListNode* nextNode)
{
	ListNode* newNode = NULL;
	if (fFreeNodes) {
		newNode = fFreeNodes;
		fFreeNodes = fFreeNodes->fNext;
		fFreeNodeCount--;
	}
	
	if (newNode == NULL) {
		newNode = new ListNode();
	}

	newNode->fPrev = prevNode;
	newNode->fNext = nextNode;
	return newNode;
}

/** Removes an element or a range of fSize in a list from specified positions. */
LIST_TEMPLATE void LIST_TYPE::Remove(ListNode* removeNode )
{
	if (removeNode == NULL) {
		return;

	} else if (fStartNode == NULL) {
		return;

	} else if (fEndNode == NULL) {
		return;
	}

	if (removeNode == fStartNode) {
		// 删除第一个节点
		fStartNode = fStartNode->fNext;
		if (fStartNode) {
			fStartNode->fPrev = NULL;

		} else {
			fEndNode = NULL;
		}

	} else if (removeNode == fEndNode) {
		// 删除最后一个节点
		fEndNode = fEndNode->fPrev;
		if (fEndNode) {
			fEndNode->fNext = NULL;

		} else {
			fStartNode = NULL;
		}

	} else {
		// 删除中间节点
		ListNode* prevNode = removeNode->fPrev;
		if (prevNode) {
			prevNode->fNext = removeNode->fNext;
		}

		ListNode* nextNode = removeNode->fNext;
		if (nextNode) {
			nextNode->fPrev = removeNode->fPrev;
		}
	}

	// 
	--fSize;
	if (fSize <= 0) {
		fStartNode = NULL;
		fEndNode   = NULL;
	}

	// free
	removeNode->fPrev = NULL;
	removeNode->fNext = NULL;
	FreeNode(removeNode);
}

/** Erases fSize in a list that match a specified value. */
LIST_TEMPLATE void LIST_TYPE::Remove(ARG_TYPE value)
{
	ListIterator iter = GetListIterator();
	while (iter.HasNext()) {
		if (iter.Next() == value) {
			iter.Remove();
			break;
		}
	}
}

/** Deletes the element at the beginning of a list. */
LIST_TEMPLATE void LIST_TYPE::RemoveFirst()
{
	if (fStartNode == NULL) {
		return;
	}

	Remove(fStartNode);
}

/** Deletes the element at the End of a list. */
LIST_TEMPLATE void LIST_TYPE::RemoveLast()
{
	if (fEndNode == NULL) {
		return;
	}

	Remove(fEndNode);
}

}

#endif // _NS_VISION_CORE_BASE_LIST_H

