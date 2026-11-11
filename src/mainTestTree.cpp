typedef double binTreeNodeVal_t;
#undef  TREE_PRINTF_STRING
#define TREE_PRINTF_STRING "%lg"

#include "../include/basic.h"
#include "../include/binTree.h"
#include "../include/search.h"

#include "./binTree.cpp"

int main()
{
    InitLogDir();
    SetUpLog();
    binTree_t tree = {};
    TREE_CTOR(&tree);


    AddNodeToSortedTree(&tree, 67);
    AddNodeToSortedTree(&tree, 20);
    AddNodeToSortedTree(&tree, 52);
    AddNodeToSortedTree(&tree, 102);
    AddNodeToSortedTree(&tree, 7);
    AddNodeToSortedTree(&tree, 1);
    AddNodeToSortedTree(&tree, 32);
    AddNodeToSortedTree(&tree, 29);

    TreePrintPreorder(tree.data);
    printf("\n");
    TreePrintInorder(tree.data);
    printf("\n");
    TreePrintPostorder(tree.data);
    printf("\n");

    TreeDump(&tree);

    return 0;
}