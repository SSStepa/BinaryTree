#include "../include/binTree.h"

int main()
{
    binTree_t tree = {};
    binTreeNode_t a = {};
    binTreeNode_t b = {};
    binTreeNode_t c = {};
    a.left = &b;
    a.right = &c;
    tree.data = &a;

    TreeDump(&tree);

    return 0;
}