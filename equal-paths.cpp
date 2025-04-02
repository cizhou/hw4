#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here


bool equalPaths(Node* root) {
    // Case for null or single node
    if (root == nullptr || (root->left == nullptr && root->right == nullptr)) 
    {
        return true;
    }
    return equalPathsHelper(root) != -1; // -1 signifies unequal paths
}


int equalPathsHelper(Node* root) {
    // base case
    if (root == nullptr) 
    {
        return 0;
    }

    int leftDepth = equalPathsHelper(root->left);
    int rightDepth = equalPathsHelper(root->right);

    // If either side reports unequal paths (-1), propagate that
    if (leftDepth == -1 || rightDepth == -1 || (leftDepth != rightDepth && root->left != nullptr && root->right != nullptr)) {
        return -1;
    }

    return 1 + (leftDepth > rightDepth ? leftDepth : rightDepth); // Return the depth of the current subtree
}
