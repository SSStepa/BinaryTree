#include "../include/search.h"

WORK_RES AddNodeToSortedTree(binTree_t *tree, binTreeNodeVal_t val)
{
    assert(tree);

    $INFO("Starting adding new node to tree <%s>", tree -> debuggingData.variableName);
    WORK_RES statusCode = OK;

    binTreeNode_t **ptrToConnect = 0;

    RUN_COMMAND(FindPlaceToConnect(&(tree -> data), &ptrToConnect, val), "CANNOT FIND WHERE TO ADD NODE");
    RUN_COMMAND(TreeNodeCreat(val, ptrToConnect), "CANT ADD NEW NODE TO TREE");

    $INFO("ending adding new node to tree <%s>", tree -> debuggingData.variableName);
    return statusCode;
}

WORK_RES FindPlaceToConnect(binTreeNode_t **node, binTreeNode_t ***ptrToConnect, binTreeNodeVal_t val)
{
    if (!*node) {
    *ptrToConnect = node;
    return OK;
    }

    if ((*node) -> val > val)
        FindPlaceToConnect(&((*node) -> left), ptrToConnect, val);
    else
        FindPlaceToConnect(&((*node) -> right), ptrToConnect, val);

    return OK;
}
