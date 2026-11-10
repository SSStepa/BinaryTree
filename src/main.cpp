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
    TREE_NODE_CREATE(4, &(tree.data -> right -> left),  tree);
    TREE_NODE_CREATE(5, &(tree.data -> right -> right), tree);

    TreePrintPreorder(tree.data);
    printf("\n");
    TreePrintInorder(tree.data);
    printf("\n");
    TreePrintPostorder(tree.data);
    printf("\n");

    TreeDump(&tree);

    return 0;
}