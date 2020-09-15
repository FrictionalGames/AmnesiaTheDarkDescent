/*
 * Copyright © 2009-2020 Frictional Games
 * 
 * This file is part of Amnesia: The Dark Descent.
 * 
 * Amnesia: The Dark Descent is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version. 

 * Amnesia: The Dark Descent is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Amnesia: The Dark Descent.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef HPL_BINTREE_H
#define HPL_BINTREE_H

#include <list>
#include <vector>

namespace hpl {

	enum eBinTreeNode {
		eBinTreeNode_Left,
		eBinTreeNode_Right
	};

	

	//////////////////////////////////////////////////////////////////////////
	// TREE NODE CLASS
	//////////////////////////////////////////////////////////////////////////

	template<class T> class BinTreeNode {
	public:
		T* GetData()
		{
			return &mData;
		}
	
		BinTreeNode(T aData, BinTreeNode<T> *aParent, eBinTreeNode aParentDir)
		{
			for(int i=0;i<2;i++) mChild[i]=NULL;
			mData = aData;
			mParent = aParent;
			mParentDir = aParentDir;
		}

		BinTreeNode<T>* AddChild(eBinTreeNode i, T aData)
		{
			if(mChild[i]==NULL){
				mChild[i] = hplNew( BinTreeNode<T>, (aData, this,i) );
				return mChild[i];
			}
			return NULL;
		}

		BinTreeNode<T>* GetChild(eBinTreeNode i)
		{
			return mChild[i];
		}

		BinTreeNode<T>* GetParent()
		{
			return mParent;
		}
	
	private:
		BinTreeNode<T>* mChild[2];
		BinTreeNode<T>* mParent;
		T mData;
		eBinTreeNode mParentDir;
	};

	//////////////////////////////////////////////////////////////////////////
	// MAIN TREE CLASS
	//////////////////////////////////////////////////////////////////////////
	
	
	template<class T> class BinTree {
	public:
		BinTree(){
			mlNumOfNodes =0;
			mFirstNode = NULL;
		}

		~BinTree()
		{
			Clear();
		}

		/**
		 * Clears the entire tree
		 * \return number of nodes deleted
		 */
		int Clear()
		{
			mlNum=0;
			DeleteNode(mFirstNode);
			mFirstNode = NULL;
			return mlNum;
		}

		/**
		 * Insert a node to the tree.
		 * \todo only works to set the root node.
		 * \param aData the data to insert
		 * \return 
		 */
		BinTreeNode<T>* Insert(T aData)
		{
			if(mFirstNode==NULL)
			{
				mFirstNode = hplNew( BinTreeNode<T>, (aData, NULL,eBinTreeNode_Left) );
				mlNumOfNodes++;

				return mFirstNode;
			}

			//Insertion other then at the root is not supported!
			BinTreeNode<T>* Node = mFirstNode;
			eBinTreeNode c;
			while(true)
			{
				//if(Node->GetData()<aData)
					c = eBinTreeNode_Left;
				//else
				//	c = eBinTreeNode_Right;

                if(Node->GetChild(c)==NULL)
				{
					Node = Node->AddChild(c, aData);
					break;
				}
				else
				{
					Node = Node->GetChild(c);
				}
			}
			mlNumOfNodes++;
			
			return Node;
		}

		/**
		 * Insert a node into a certain node in the tree
		 * \param aData the data to insert
		 * \param aNode the node to insert the data in
		 * \param aChild what child to insert at
		 * \return 
		 */
		BinTreeNode<T>* InsertAt(T aData,BinTreeNode<T>* aNode, eBinTreeNode aChild=eBinTreeNode_Left)
		{
			if(aNode == NULL)return NULL;

			if(aNode->GetChild(aChild)!=NULL) {
				aChild = aChild==eBinTreeNode_Left ? eBinTreeNode_Right : eBinTreeNode_Left;
				if(aNode->GetChild(aChild)!=NULL)return NULL;
			}

            return aNode->AddChild(aChild, aData);
		}

		/**
		 * Get the size of the tree
		 * \return 
		 */
		int Size()
		{
			return mlNumOfNodes;
		}

		const std::vector<BinTreeNode<T>*>& GetLeafList()
		{
			mvNodes.clear(); 
			mvNodes.reserve(mlNumOfNodes);
			PopulateLeafList(mFirstNode);
			return  mvNodes;
		}

		/**
		 * Get a list of all the nodes in the tree
		 * \return 
		 */
		const std::vector<BinTreeNode<T>*>& GetNodeList()
		{
			mvNodes.clear();
			mvNodes.reserve(mlNumOfNodes);
			PopulateNodeList(mFirstNode);
			return  mvNodes;
		}
	    
	private:
		int mlNumOfNodes;
		BinTreeNode<T>* mFirstNode;
		int mlNum;
		
		std::vector<BinTreeNode<T>*> mvNodes; 

		void DeleteNode(BinTreeNode<T>* aNode)
		{
			if(aNode==NULL)	return;

			DeleteNode(aNode->GetChild(eBinTreeNode_Left));
			DeleteNode(aNode->GetChild(eBinTreeNode_Right));
			
			hplDelete(aNode);
			mlNum++;
		}

		void PopulateNodeList(BinTreeNode<T>* aNode)
		{
			if(aNode==NULL)	return;

			PopulateNodeList(aNode->GetChild(eBinTreeNode_Left));
			mvNodes.push_back(aNode);			
			PopulateNodeList(aNode->GetChild(eBinTreeNode_Right));
		}

		void PopulateLeafList(BinTreeNode<T>* aNode)
		{
			if(aNode==NULL)	return;
			
			if(aNode->GetChild(eBinTreeNode_Left)==NULL &&
				aNode->GetChild(eBinTreeNode_Right)==NULL)
			{
				mvNodes.push_back(aNode);
			}

			PopulateLeafList(aNode->GetChild(eBinTreeNode_Left));
			PopulateLeafList(aNode->GetChild(eBinTreeNode_Right));
		}


	};

};
#endif // HPL_BINTREE_H
