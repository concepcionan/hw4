#include "equal-paths.h"
#include <algorithm>
#include <cmath>
using namespace std;


// You may add any prototypes of helper functions here
int calculateHeight(Node * root);

bool equalPaths(Node * root)
{
    // Add your code below
	if (root == nullptr) return true;
	if ((calculateHeight(root->right) == 0 && equalPaths(root->left)) || (calculateHeight(root->left) == 0 && equalPaths(root->right))) return true;
	return calculateHeight(root->right) == calculateHeight(root->left);
}

int calculateHeight(Node * root){
	if (root == nullptr) return 0;
	return max(calculateHeight(root->left), calculateHeight(root->right)) + 1;
}