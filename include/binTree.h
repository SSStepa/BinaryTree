#ifndef BIN_TREE
#define BIN_TREE

#include "basic.h"

const char *const GRAPHVIZ_TEXT_FILE = "graphvizInfo.txt";
const char *const GRAPHVIZ_OUT_BASE  = "dumpGraphviz";

typedef double binTreeNodeVal_t;
#define binTreeNodePrintf "%lg"

struct binTreeNode_t {
   binTreeNodeVal_t val;
   binTreeNode_t   *left;
   binTreeNode_t   *right;
};

struct binTree_t {
    binTreeNode_t *data;

    ON_DBG(
        debuggingInfo_t debuggingData;

        binTreeNode_t *ptrToLastUsedNode;
    )
};

/**
 * creation of tree. Initializes debugging info.
 * do almost nothing in basic realization
 * 
 * @param [out] tree pointer to tree to create.
 * 
 * When debugging:
 * @param [in] variableName         name of variable in which tree is created.
 * @param [in] fileCreationName     name of file in which tree is created.
 * @param [in] functionCreationName name of function in which tree is created 
 * @param [in] lineCreationNumber   number of line in which tree is created 
 */
WORK_RES TreeCtor(binTree_t *tree ON_DBG(, char *variableName, char *fileCreationName, char *functionCreationName, int lineCreationNumber));

/**
 * Dtor of tree. frees all nodes in dynamic memory.
 * 
 * @param [out] tree pointer to tree to deconstruct.
 */
WORK_RES TreeDtor(binTree_t *tree);

#if 0
/**
 * Verify of tree as whole. Also calls TreeNodeVerify
 * 
 * @param [in] tree pointer to tree for to verify
 */
WORK_RES TreeVerify(const binTree_t *tree);

/**
 * Verify of one node. recursively calls itself while all tree is not verified 
 * 
 * @param [in] node node to verify.
 */
WORK_RES TreeNodeVerify(const binTreeNode_t *node);


/**
 * Dump for all Tree. Calls recursive TreeNodeDump for all tree. 
 * 
 * @param [in] tree tree to dump
 */
WORK_RES TreeDump(const binTree_t *tree);


/**
 * Recursive function to dump one node
 * As a result dumps all tree to graphviz and out.
 * 
 * @param [in] node node to dump
 */
WORK_RES TreeNodeDump(const binTreeNode_t *node, FILE *outFile);

/**
 * Creates node and gives value to it.
 * Sets left and right children to 0. connects created node to passed pointer
 * 
 * @param [in] val value which is given to new node 
 * @param [out] createdNodePtr pointer in which new node pointer to is saved(so it can be field in parent node) 
 */
WORK_RES TreeNodeCreat(binTreeNodeVal_t val, binTreeNode_t **createdNodePtr);

/**
 * deletes node and all it's children recursively.
 * 
 * @param [in, out] deleteNodePtr pointer to node to delete
 */
WORK_RES TreeNodeDelete(binTreeNode_t **deleteNodePtr);


/**
 * prints tree recursively in preorder form
 * 
 * @param [in] node node to print
 */
WORK_RES TreePrintPreorder(const binTreeNode_t *node);

/**
 * prints tree recursively in inorder form
 * 
 * @param [in] node node to print
 */
WORK_RES TreePrintInorder(const binTreeNode_t *node);

/**
 * prints tree recursively in postorder form
 * 
 * @param [in] node node to print
 */
WORK_RES TreePrintPostorder(const binTreeNode_t *node);
#endif

/**
 * macro to call TreeCtor with debugging info args auto adding.
 */
#define TREE_CTOR(tree) TreeCtor(tree ON_DBG(, TO_STR(tree), __FILE__, __FUNCTION__, __LINE__));

#endif