#ifndef RBBST_H
#define RBBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor and setting
* the color to red since every new node will be red when it is first inserted.
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{
    
}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    AVLNode<Key, Value>* internalFind(const Key& k) const;
    static AVLNode<Key, Value>* predecessor(AVLNode<Key, Value>* current);
    void clear(); 
	void clearHelper(AVLNode<Key, Value>* root);
    void insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* child);
    void removeFix(AVLNode<Key, Value>* parent, int8_t diff);
    void rotateRight(AVLNode<Key, Value>* node);
    void rotateLeft(AVLNode<Key, Value>* node);
    bool zigOrZag(AVLNode<Key, Value>* node);
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    Key inKey = new_item.first;
	Value inValue = new_item.second;

    //std::cout << "Inserting " << inKey << std::endl;

    AVLNode<Key, Value>* root = static_cast<AVLNode<Key, Value>*>(this->root_);
	AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(inKey, inValue, nullptr);
	if (root == NULL){
		this->root_ = newNode;
        newNode->setBalance(0);
		return;
	}

	AVLNode<Key, Value>* iterator = root;
	while (iterator != NULL){
		Key itrKey = iterator->getKey();
	
		if (inKey < itrKey){
			if (iterator->getLeft() == NULL){
				iterator->setLeft(newNode);
				newNode->setParent(iterator);
                newNode->setBalance(0);
                if (iterator){
                    if (iterator->getBalance() != 0){
                        iterator->setBalance(0);
                    }
                    else {
                        iterator->setBalance(-1);
                        insertFix(iterator, newNode);
                    }    
                }
				return;
			}
			else{
				iterator = iterator->getLeft();
			}
		} 
		else if (inKey > itrKey){
			if (iterator->getRight() == NULL){
				iterator->setRight(newNode);
				newNode->setParent(iterator);
                newNode->setBalance(0);
                if (iterator){
                    if (iterator->getBalance() != 0){
                        iterator->setBalance(0);
                    }
                    else {
                        iterator->setBalance(1);
                        insertFix(iterator, newNode);
                    }    
                }
				return;
			}
			else{
				iterator = iterator->getRight();
			}
		}
		else if (inKey == itrKey){
			iterator->setValue(inValue);
			return;
		}		
	}
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    //std::cout << "REMOVING " << key << std::endl;
    AVLNode<Key, Value>* root = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* nodeToRemove = internalFind(key);
	if (nodeToRemove){
        AVLNode<Key, Value>* parent = nodeToRemove->getParent();
		if (!nodeToRemove->getLeft() && !nodeToRemove->getRight()){ //if 0 children
			if (nodeToRemove == root){
				clear();
			}
			else if(parent){
				if (parent->getLeft() == nodeToRemove){ //is left child
					parent->setLeft(NULL);
				}
				else if (parent->getRight() == nodeToRemove){ //is right child
					parent->setRight(NULL);
				}
                delete nodeToRemove;
            }
		}
		else if (!nodeToRemove->getRight()){ //if 1 left child
			AVLNode<Key, Value>* child = nodeToRemove->getLeft();
			if(parent){
				if (parent->getLeft() == nodeToRemove){ //is left child
					parent->setLeft(child);
					child->setParent(parent);
				}
				else if (parent->getRight() == nodeToRemove){ //is right child
					parent->setRight(child);
					child->setParent(parent);	
				}
			}
			else {
				root = child;
				child->setParent(NULL);
			}
			delete nodeToRemove;
		}
		else if (!nodeToRemove->getLeft()){ //if 1 right child
			AVLNode<Key, Value>* child = nodeToRemove->getRight();
			if(parent){
				if (parent->getLeft() == nodeToRemove){ //is left child
					parent->setLeft(child);
					child->setParent(parent);
				}
				else if (parent->getRight() == nodeToRemove){ //is right child
					parent->setRight(child);	
					child->setParent(parent);
				}
			}
			else {
				root = child;
				child->setParent(NULL);
			}
			delete nodeToRemove;
		}
		else if (nodeToRemove->getLeft() && nodeToRemove->getRight()){ //if 2 children
			AVLNode<Key, Value>* pred = predecessor(nodeToRemove);
            nodeSwap(nodeToRemove, pred);

			if (!nodeToRemove->getLeft() && !nodeToRemove->getRight()){ //if 0 children
				if (nodeToRemove == root){
					clear();
				}
				if(parent){
					if (parent->getLeft() == nodeToRemove){ //is left child
						parent->setLeft(NULL);
					}
					else if (parent->getRight() == nodeToRemove){ //is right child
						parent->setRight(NULL);
					}
                    delete nodeToRemove;
				}
			}
			else if (nodeToRemove->getLeft() && !nodeToRemove->getRight()){ //if 1 left child
				AVLNode<Key, Value>* child = nodeToRemove->getLeft();
				if(parent){
					if (parent->getLeft() == nodeToRemove){ //is left child
						parent->setLeft(child);
						child->setParent(parent);
					}
					else if (parent->getRight() == nodeToRemove){ //is right child
						parent->setRight(child);	
						child->setParent(parent);
					}
				}
				else {
					root = child;
					child->setParent(NULL);
				}
				delete nodeToRemove;
			}
			else if (!nodeToRemove->getLeft() && nodeToRemove->getRight()){ //if 1 right child
				AVLNode<Key, Value>* child = nodeToRemove->getRight();
				if(parent){
					if (parent->getLeft() == nodeToRemove){ //is left child
						parent->setLeft(child);
						child->setParent(parent);
					}
					else if (parent->getRight() == nodeToRemove){ //is right child
						parent->setRight(child);	
						child->setParent(parent);
					}
				}
				else {
					root = child;
					child->setParent(NULL);
				}	
				delete nodeToRemove;
			}
		}
        if(parent){
            if (parent->getLeft() == nodeToRemove){ //is left child
                removeFix(parent, 1);
            }
            else if (parent->getRight() == nodeToRemove){ //is right child
                removeFix(parent, 1);
            }
        }
	} else {
		return;
	}
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

template<typename Key, typename Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::internalFind(const Key& key) const
{
    AVLNode<Key, Value>* iterator = static_cast<AVLNode<Key, Value>*>(this->root_);
    while (iterator != NULL) {
			if (iterator->getKey() != key){
				if(key < iterator->getKey()){
            iterator = iterator->getLeft();
        }
        else if (key > iterator->getKey()){
            iterator = iterator->getRight();
        }
			}
			else {
				break;
			}
    }
    return iterator;
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::predecessor(AVLNode<Key, Value>* current)
{
	if (!current) return NULL;
    if (!current->getLeft() && !current->getRight()) return NULL;
    if (current->getLeft()){
        current = current->getLeft();
        while (current->getRight() != NULL){
            current = current->getRight();
        }
        return current;
    }
    else {
			AVLNode<Key, Value>* parent = current->getParent();
				while(parent!=NULL && parent->getRight()!=current){
					current = parent;
					parent = parent->getParent();
				}
        return parent;
    }
}

template<typename Key, typename Value>
void AVLTree<Key, Value>::clear()
{
    AVLNode<Key, Value>* root = static_cast<AVLNode<Key, Value>*>(this->root_);
    clearHelper(root);
    root = NULL;
}

template<typename Key, typename Value>
void AVLTree<Key, Value>::clearHelper(AVLNode<Key, Value>* root){
	if (root == NULL) return;
	clearHelper(root->getLeft());
	clearHelper(root->getRight());
	delete root;
}

template<typename Key, typename Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* child){
    // std::cout << "IN INSERT FIX" << std::endl;
    // std::cout << "Parent is " << parent->getKey() << std::endl;
    // std::cout << "Child is " << child->getKey() << std::endl;
    // this->printRoot(this->root_);
    if (parent == NULL) return;    
    AVLNode<Key, Value>* grand = parent->getParent();
    if (grand == NULL) return;
    if (grand->getLeft() == parent){ // p is left child of g
        grand->setBalance(grand->getBalance() - 1);
        if (grand->getBalance() == 0){
            return;
        }
        else if (grand->getBalance() == -1){
            insertFix(grand, parent);
        }
        else if (grand->getBalance() == -2){
            if (zigOrZag(grand)){
                rotateRight(grand);
                parent->setBalance(0);
                grand->setBalance(0);
            }
            else if(!zigOrZag(grand)){
                rotateLeft(parent);
                rotateRight(grand);
                if (child->getBalance() == -1){
                    parent->setBalance(0);
                    grand->setBalance(1); 
                    child->setBalance(0);  
                }
                else if (child->getBalance() == 0){
                    parent->setBalance(0);
                    grand->setBalance(0); 
                    child->setBalance(0);  
                }
                else if (child->getBalance() == 1){
                    parent->setBalance(-1);
                    grand->setBalance(0); 
                    child->setBalance(0);  
                }
            }
        }
    }
    else if (grand->getRight() == parent){ // p is right child of g
        grand->setBalance(grand->getBalance() + 1);
        if (grand->getBalance() == 0){
            return;
        }
        else if (grand->getBalance() == 1){
            insertFix(grand, parent);
        }
        else if (grand->getBalance() == 2){
            if (zigOrZag(grand)){
                rotateLeft(grand);
                parent->setBalance(0);
                grand->setBalance(0);
            }
            else if(!zigOrZag(grand)){
                rotateRight(parent);
                rotateLeft(grand);
                if (child->getBalance() == 1){
                    parent->setBalance(0);
                    grand->setBalance(-1); 
                    child->setBalance(0);  
                }
                else if (child->getBalance() == 0){
                    parent->setBalance(0);
                    grand->setBalance(0); 
                    child->setBalance(0);  
                }
                else if (child->getBalance() == -1){
                    parent->setBalance(1);
                    grand->setBalance(0); 
                    child->setBalance(0);  
                }
            }
        }
    }
}

template<typename Key, typename Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node){
    AVLNode<Key, Value>* child = node->getLeft();

    if (!node->getParent()){
        this->root_ = child;
        child->setParent(NULL);
    }
    else {
        AVLNode<Key, Value>* parent = node->getParent();
        if (parent->getLeft() == node){
            parent->setLeft(child);
        }
        else if (parent->getRight()==node){
            parent->setRight(child);
        }
        child->setParent(parent);
    }
    node->setLeft(child->getRight()); 
    if (node->getRight()){
        node->getRight()->setParent(node);
    }
    child->setRight(node);
    node->setParent(child);
}

template<typename Key, typename Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node){
    AVLNode<Key, Value>* child = node->getRight();

    if (!node->getParent()){
        this->root_ = child;
        child->setParent(NULL);
    }
    else {
        AVLNode<Key, Value>* parent = node->getParent();
        if (parent->getLeft() == node){
            parent->setLeft(child);
        }
        else if (parent->getRight()==node){
            parent->setRight(child);
        }
        child->setParent(parent);
    }
    node->setRight(child->getLeft()); 
    if (node->getLeft()){
        node->getLeft()->setParent(node);
    }
    child->setLeft(node);
    node->setParent(child);
}

template<typename Key, typename Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* node, int8_t diff){
    //std::cout << "IN REMOVE FIX" << std::endl;
    if (!node) return;
    AVLNode<Key, Value>* parent = node->getParent();
    int8_t nextDiff = 0;   
    if (parent){
        if (parent->getLeft() == node){
            nextDiff = 1;    
        }
        else if (parent->getRight() == node){
            nextDiff = -1;
        }
    }
    if (diff == -1){
        if (abs(node->getBalance() + diff) == 2){
            if (node->getBalance() + diff == -2){
                AVLNode<Key, Value>* child = node->getLeft();
                if (child->getBalance() == -1){
                    rotateRight(node);
                    node->setBalance(0);
                    child->setBalance(0);
                    removeFix(parent, nextDiff);
                }
                else if (child->getBalance() == 0){
                    rotateRight(node);
                    node->setBalance(-1);
                    child->setBalance(1);
                    return;
                }
                else if (child->getBalance() == 1){
                    AVLNode<Key, Value>* grand = child->getRight();
                    rotateLeft(child);
                    rotateRight(node);
                    if (grand->getBalance() == 1){
                        node->setBalance(0);
                        child->setBalance(-1);
                        grand->setBalance(0);
                    }
                    else if (grand->getBalance() == 0){
                        node->setBalance(0);
                        child->setBalance(0);
                        grand->setBalance(0);
                    }
                    else if (grand->getBalance() == -1){
                        node->setBalance(1);
                        child->setBalance(0);
                        grand->setBalance(0);
                    }
                    removeFix(parent, nextDiff);
                }
            }
            else if (node->getBalance() + diff == 2){

            }
        }
        else if (abs(node->getBalance() + diff) == 1){
            node->setBalance(-1); 
        }
        else if (node->getBalance() + diff == 0){
            node->setBalance(0);
            removeFix(parent, nextDiff);
        }
    }

}

/*
 *  Returns true if ZigZig, Returns false if ZigZag
 */
template<typename Key, typename Value>
bool AVLTree<Key, Value>::zigOrZag(AVLNode<Key, Value>* node){
    if (node->getLeft()){
        if(node->getLeft()->getLeft()) return true;
        if(node->getLeft()->getRight()) return false;
    }
    else if (node->getRight()){
        if(node->getRight()->getLeft()) return false;
        if(node->getRight()->getRight()) return true;
    }
    return false;
}

#endif
