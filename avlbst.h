#ifndef AVLBST_H
#define AVLBST_H

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
* An explicit constructor to initialize the elements by calling the base class constructor
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
    void rotateRight(AVLNode<Key, Value>* top); 
    void rotateLeft(AVLNode<Key,Value>* top);

    void insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* newNode);
    void removeFix(AVLNode<Key, Value>* child, int8_t diff);




};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO

    // If empty tree => set n as root, b(n) = 0, done!
    if (this->root_ == nullptr)
    {
        AVLNode<Key,Value>* newNode = new AVLNode<Key,Value>(new_item.first, new_item.second, nullptr);
        this->root_ = newNode;
        return;
    }

    // Key is inside tree and so we edit
    if (BinarySearchTree<Key, Value>::internalFind(new_item.first))
    {
        BinarySearchTree<Key, Value>::internalFind(new_item.first)->setValue(new_item.second);
		return;
    }
    // Else insert n (by walking the tree to a leaf, p, and inserting the new node as its child), set balance to 0, and look at its parent, p

    AVLNode<Key,Value>* newNode = new AVLNode<Key,Value>(new_item.first, new_item.second, nullptr);
    AVLNode<Key, Value>* parent = static_cast<AVLNode<Key, Value>*> (this->root_);

    while (parent != nullptr) 
    {
        // less than: left
        if (newNode->getKey() < parent->getKey()) 
        {
            // reached the end
            if (parent->getLeft() == nullptr)
            {
                parent->setLeft(newNode);
                newNode->setParent(parent);
                break;
            }
            // iterate to next
            else 
            {
                parent = parent->getLeft();
            }
        }
        // greater than: right
        else
        {
            // reached the end
            if (parent->getRight() == nullptr)
            {
                parent->setRight(newNode);
                newNode->setParent(parent);
                break;
            }
            // iterate to next
            else 
            {
                parent = parent->getRight();
            }
        }
    }

    // balancing
    // – If b(parent) was -1, then b(p) = 0. Done!
    // – If b(parent) was +1, then b(p) = 0. Done!
    if (parent->getBalance() == 1 || parent->getBalance() == -1) 
    {
        parent->setBalance(0);
        return;
    }
    // – If b(parent) was 0, then update b(parent) and call insert-fix(parent, child)  
    else if (parent->getBalance() == 0)
    {
        if (parent->getLeft() == newNode)
        {
            parent->updateBalance(-1);
        }
        else if (parent->getRight() == newNode)
        {
            parent->updateBalance(1);
        }
        insertFix(parent, newNode);
    }

}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* newNode)
{   
    if (parent == this->root_)
    {
        return;
    }

    AVLNode<Key, Value>* grandparent = parent->getParent();

    // Case 1: If grandparent is null or balanced, return
    if (grandparent == nullptr)
    {
        return;
    }

    // Assume p is left child of g [For right child swap left/right, +/-]
    if (grandparent->getLeft() == parent)
    {
        // b(g) += -1 // Update g's balance to new accurate value for now
        grandparent->setBalance(grandparent->getBalance() - 1);
        // – Case 1: b(g) == 0, return
        if (grandparent->getBalance() == 0)
        {
            return;
        }
        // – Case 2: b(g) == -1, insertFix(g, p) // recurse
        else if (grandparent->getBalance() == -1)
        {
            insertFix(grandparent, parent);
        }
        // – Case 3: b(g) == -2
        else if (grandparent->getBalance() == -2)
        {
            // If zig-zig then rotateRight(g); b(p) = b(g) = 0: left left
            if (parent->getLeft() == newNode)
            {
                rotateRight(grandparent);
                parent->setBalance(0); 
                parent->getRight()->setBalance(0);
            }
            // If zig-zag then rotateLeft(p); rotateRight(g);: left right
            else 
            {
                rotateLeft(parent);
                rotateRight(grandparent);
                // – Case 3a: b(n) == -1 
                // then b(p) = 0; b(g) = +1; b(n) = 0;
                if (newNode->getBalance() == -1)
                {
                    parent->setBalance(0);
                    grandparent->setBalance(1);
                    newNode->setBalance(0);
                } 
                // – Case 3b: b(n) == 0 
                // then b(p) = 0; b(g) = 0; b(n) = 0;
                else if (newNode->getBalance() == 0)
                {
                    parent->setBalance(0); 
                    grandparent->setBalance(0);
                    newNode->setBalance(0);
                }
                // – Case 3c: b(n) == +1 
                //then b(p)= -1; b(g) = 0; b(n) = 0;            
                else if (newNode->getBalance() == 1)
                {
                    parent->setBalance(-1); 
                    grandparent->setBalance(0);
                    newNode->setBalance(0);
                }
            }
            return;
        }
    }

    // Assume parent is right child of grandparent
    else
    {
        // b(g) += 1 // Update g's balance for right child
        grandparent->setBalance(grandparent->getBalance() + 1);
    
        // Case 1: b(g) == 0, return
        if (grandparent->getBalance() == 0)
        {
            return;
        }
        // Case 2: b(g) == 1, insertFix(g, p) // recurse
        else if (grandparent->getBalance() == 1)
        {
            insertFix(grandparent, parent);
        }
        // Case 3: b(g) == 2, imbalance on the right
        else if (grandparent->getBalance() == 2)
        {
            // If zig-zig then rotateLeft(g); b(p) = b(g) = 0: right right
            if (parent->getRight() == newNode)
            {
                rotateLeft(grandparent);
                parent->setBalance(0);
                grandparent->setBalance(0);
            }
            // If zig-zag then rotateRigth(p); rotateLeft(g);: right right
            else
            {
                rotateRight(parent);
                rotateLeft(grandparent);
    
                // – Case 3a: b(n) == 1 
                // then b(p) = 0; b(g) = 1; b(n) = 0;
                if (newNode->getBalance() == 1)
                {
                    parent->setBalance(0);
                    grandparent->setBalance(-1);
                    newNode->setBalance(0);
                }
                // – Case 3b: b(n) == 0 
                // then b(p) = 0; b(g) = 0; b(n) = 0;
                else if (newNode->getBalance() == 0)
                {
                    parent->setBalance(0);
                    grandparent->setBalance(0);
                    newNode->setBalance(0);
                }
                // – Case 3c: b(n) == -1 
                //then b(p)= 1; b(g) = 0; b(n) = 0;  
                else if (newNode->getBalance() == -1)
                {
                    parent->setBalance(1);
                    grandparent->setBalance(0);
                    newNode->setBalance(0);
                }
            }
            return;
        }
    }
    
}


/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    AVLNode<Key, Value>* temp = static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::internalFind(key));
    
    if (temp == nullptr)
    {
        return;
    }

    int diff = -1; 

    // Case: Node has two children -> Swap with predecessor
    if (temp->getLeft() != nullptr && temp->getRight() != nullptr)
    {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::predecessor(temp));
        nodeSwap(temp, pred);
    }

    AVLNode<Key, Value>* parent = temp->getParent();
    
    // Case: 0 children (Leaf node)
    if (temp->getLeft() == nullptr && temp->getRight() == nullptr)
    {
        if (temp == this->root_)
        {
            this->root_ = nullptr; // Deleting the root
        }
        else if (parent->getRight() == temp)
        {
            parent->setRight(nullptr);
            diff = -1; // Right child was removed
        }
        else
        {
            parent->setLeft(nullptr);
            diff = 1; // Left child was removed
        }
    }

    // Case: 1 child (Left)
    else if (temp->getLeft() != nullptr && temp->getRight() == nullptr)
    {
        if (temp == this->root_)
        {
            this->root_ = temp->getLeft();
            this->root_->setParent(nullptr);
        }
        else if (parent->getRight() == temp)
        {
            parent->setRight(temp->getLeft());
            diff = -1; // Right child replaced
        }
        else
        {
            parent->setLeft(temp->getLeft());
            diff = 1; // Left child replaced
        }

        temp->getLeft()->setParent(parent);
    }

    // Case: 1 child (Right)
    else if (temp->getLeft() == nullptr && temp->getRight() != nullptr)
    {
        if (temp == this->root_)
        {
            this->root_ = temp->getRight();
            this->root_->setParent(nullptr);
        }
        else if (parent->getRight() == temp)
        {
            parent->setRight(temp->getRight());
            diff = -1; // Right child replaced
        }
        else
        {
            parent->setLeft(temp->getRight());
            diff = 1; // Left child replaced
        }

        temp->getRight()->setParent(parent);
    }

    delete temp;

    removeFix(parent, diff);
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* child, int8_t diff)
{
    // If n is null, return
    if (child == nullptr)
    {
        return;
    }

    // Compute next recursive call's arguments now before altering the tree
    AVLNode<Key, Value>* parent = child->getParent();
    int ndiff = 0;

    // If parent is not NULL, determine ndiff based on left or right child
    if (parent != nullptr)
    {
        if (parent->getLeft() == child)
        {
            ndiff = 1;
        }
        else
        {
            ndiff = -1;
        }
    }

    // Assume diff = -1 and mirror the logic for diff = +1 later
    if (diff == -1)
    {
        // Case 1: b(n) + diff == -2
        if (child->getBalance() + diff == -2)
        {
            AVLNode<Key, Value>* c = child->getLeft();
            
            // Case 1a: b(c) == -1 (zig-zig case)
            if (c->getBalance() == -1)
            {
                rotateRight(child);
                child->setBalance(0);
                c->setBalance(0);
                removeFix(parent, ndiff);
            }
            // Case 1b: b(c) == 0 (zig-zig case)
            else if (c->getBalance() == 0)
            {
                rotateRight(child);
                child->setBalance(-1);
                c->setBalance(1);
            }
            // Case 1c: b(c) == +1 (zig-zag case)
            else if (c->getBalance() == 1)
            {
                AVLNode<Key, Value>* g = c->getRight();
                rotateLeft(c);
                rotateRight(child);

                if (g->getBalance() == 1)
                {
                    child->setBalance(0);
                    c->setBalance(-1);
                    g->setBalance(0);
                }
                else if (g->getBalance() == 0)
                {
                    child->setBalance(0);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                else // g->getBalance() == -1
                {
                    child->setBalance(1);
                    c->setBalance(0);
                    g->setBalance(0);
                }

                removeFix(parent, ndiff);
            }
        }
        // Case 2: b(n) + diff == -1
        else if (child->getBalance() + diff == -1)
        {
            child->setBalance(-1);
            return;
        }
        // Case 3: b(n) + diff == 0
        else if (child->getBalance() + diff == 0)
        {
            child->setBalance(0);
            removeFix(parent, ndiff);
        }
    }
    else if (diff == 1)
    {
        // Case 1: b(n) + diff == 2
        if (child->getBalance() + diff == 2)
        {
            AVLNode<Key, Value>* c = child->getRight();

            // Case 1a: b(c) == +1 (zig-zig case)
            if (c->getBalance() == 1)
            {
                rotateLeft(child);
                child->setBalance(0);
                c->setBalance(0);
                removeFix(parent, ndiff);
            }
            // Case 1b: b(c) == 0 (zig-zig case)
            else if (c->getBalance() == 0)
            {
                rotateLeft(child);
                child->setBalance(1);
                c->setBalance(-1);
            }
            // Case 1c: b(c) == -1 (zig-zag case)
            else if (c->getBalance() == -1)
            {
                AVLNode<Key, Value>* g = c->getLeft();
                rotateRight(c);
                rotateLeft(child);

                if (g->getBalance() == -1)
                {
                    child->setBalance(0);
                    c->setBalance(1);
                    g->setBalance(0);
                }
                else if (g->getBalance() == 0)
                {
                    child->setBalance(0);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                else // g->getBalance() == 1
                {
                    child->setBalance(-1);
                    c->setBalance(0);
                    g->setBalance(0);
                }

                removeFix(parent, ndiff);
            }
        }
        // Case 2: b(n) + diff == 1
        else if (child->getBalance() + diff == 1)
        {
            child->setBalance(1);
            return;
        }
        // Case 3: b(n) + diff == 0
        else if (child->getBalance() + diff == 0)
        {
            child->setBalance(0);
            removeFix(parent, ndiff);
        }
    }
}



template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* top)
{
    // taking a left child, making it the parent and making the original parent the new right child
    // need to transfer over middle's right kids over as top's new left kids
    AVLNode<Key, Value>* middle = top->getLeft();
    AVLNode<Key, Value>* bottom = middle->getRight();

    middle->setRight(top);
    top->setLeft(bottom);

    // middle does not have a right child (bottom), top does not take any of middle's kids
    if (bottom != nullptr)
    {
        bottom->setParent(top);
    }

    // replacing middle's parent to top's parent
    middle->setParent(top->getParent());

    // top was the root
    if (top->getParent() == nullptr)
    {
        this->root_ = middle;  // If top was root, middle becomes new root
    }
    // top was a left child
    else if (top == top->getParent()->getLeft())
    {
        top->getParent()->setLeft(middle);
    }
    // top was a right child
    else
    {
        top->getParent()->setRight(middle);
    }

    top->setParent(middle);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key,Value>* top){
    
    // taking a left child, making it the parent and making the original parent the new right child
    // need to transfer over middle's right kids over as top's new left kids
    AVLNode<Key, Value>* middle = top->getRight();
    AVLNode<Key, Value>* bottom = middle->getLeft();

    middle->setLeft(top);
    top->setRight(bottom);

    // middle does not have a right child (bottom), top does not take any of middle's kids
    if (bottom != nullptr)
    {
        bottom->setParent(top);
    }

    // replacing middle's parent to top's parent
    middle->setParent(top->getParent());

    // top was the root
    if (top->getParent() == nullptr)
    {
        this->root_ = middle;  // If top was root, middle becomes new root
    }
    // top was a left child
    else if (top == top->getParent()->getLeft())
    {
        top->getParent()->setLeft(middle);
    }
    // top was a right child
    else
    {
        top->getParent()->setRight(middle);
    }

    top->setParent(middle);
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
