#ifndef SEARCH
#define SEARCH

#include "basic.h"

typedef double binTreeNodeVal_t;
#undef  TREE_PRINTF_STRING
#define TREE_PRINTF_STRING "%lg"
#include "binTree.h"

errorStatus_e AddNodeToSortedTree(binTree_t *tree, binTreeNodeVal_t val);

errorStatus_e FindPlaceToConnect(binTreeNode_t **node, binTreeNode_t ***ptrToConnect, binTreeNodeVal_t val);

#endif