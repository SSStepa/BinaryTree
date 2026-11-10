#include "../include/binTree.h"
#include "../include/basic.h"

int main()
{
    InitLogDir();
    SetUpLog();
    binTree_t tree = {};

    TREE_NODE_CREATE(1, &(tree.data), tree);
    TREE_NODE_CREATE(2, &(tree.data -> left),  tree);
    TREE_NODE_CREATE(3, &(tree.data -> right), tree);

    TREE_NODE_DELETE(&(tree.data -> left), tree);

    TreeDump(&tree);

    return 0;
}