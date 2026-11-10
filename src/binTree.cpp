#include "../include/binTree.h"

#include <stdio.h>

WORK_RES TreeCtor(binTree_t *tree
     ON_DBG(, char *variableName, char *fileCreationName, char *functionCreationName, int lineCreationNumber)
)
{
    assert(tree);
    ON_DBG(
        assert(variableName);
        assert(fileCreationName);
        assert(functionCreationName);
        assert(lineCreationNumber > 0);
    )

    $INFO("Starting initing of tree");

    tree -> data = 0;

    ON_DBG(
        INIT_DEBUGGING_INFO(
            tree -> debuggingData,
            variableName,
            fileCreationName,
            functionCreationName,
            lineCreationNumber
        );
    )
    $INFO("Ending initing of tree");

    return OK;
}

WORK_RES TreeDtor(binTree_t *tree)
{
    assert(tree);

    $INFO("Starting tree deconstruction");

    WORK_RES statusCode = OK;

    RUN_COMMAND(TreeNodeDelete(&(tree -> data)), "PROBLEM WHILE DELETING ALL TREE");

    $INFO("Ending tree deconstruction");

    return statusCode;
}

WORK_RES TreeDump(const binTree_t *tree)
{
    assert(tree);

    WORK_RES statusCode = OK;
    LOG_START();

    $INFO("Starting dump of stack <%s>", (tree -> debuggingData).variableName);
    
    ON_DBG(
        LOG_PRINTF("Hi, I'm dump off tree called because of err:%d and has err message <%s>\n", (tree -> debuggingData).err, (tree -> debuggingData).errorText);
    )

    FILE *graphvizOutFile = fopen(GRAPHVIZ_TEXT_FILE, "w");
    if (graphvizOutFile == NULL) {
        return $ERR(FILEERR, "CAN'T OPEN GRAPHVIZ TEXT FILE");
    }

    fprintf(graphvizOutFile, "digraph {\n");
    fprintf(graphvizOutFile, "node [shape=Mcircle, style=filled, color=\"#800000\", fillcolor=\"#FF0000\"];\n");
    fprintf(graphvizOutFile, "edge [arrowsize=0.8, color=\"#030352\", penwidth=1];\n");

    static size_t dumpNumber = 0;
    dumpNumber++;
    TreeNodeDump(tree -> data, graphvizOutFile, logFile);

    ON_DBG(
        if (tree -> ptrToLastUsedNode)
            fprintf(graphvizOutFile, "N%p [color=\"#9e4f05\", fillcolor=\"#fa8d3f\"]\n", tree -> ptrToLastUsedNode);
    )
    fprintf(graphvizOutFile, "}\n");
    fclose(graphvizOutFile);

    char commandName[COMMAND_MAX_LEN] = {};
    sprintf(commandName, "\"\"C:\\Program Files\\Graphviz\\bin\\dot.exe\" .\\%s -Tpng -o .\\%s%zu.png\"", GRAPHVIZ_TEXT_FILE, GRAPHVIZ_OUT_BASE, dumpNumber);
    system(commandName);

    LOG_END();
    $INFO("Ending dump of stack <%s>", (tree -> debuggingData).variableName);
    return statusCode;
}

WORK_RES TreeNodeDump(const binTreeNode_t *node, FILE *outGraphvizFile, FILE *logFile)
{
    assert(outGraphvizFile);
    assert(logFile);

    WORK_RES statusCode = OK;

    if (node == 0)
        return OK;

    fprintf(outGraphvizFile, "N%p [shape=Mrecord, label=\"{My addr: %p | Val: " TREE_PRINTF_STRING " | {left: %p | right: %p}}\", fillcolor=\"#C0C0FF\" color=\"#000080\"]\n",
                    node, node, node -> val, node -> left, node -> right);
    if (node -> left)
        fprintf(outGraphvizFile, "N%p -> N%p\n", node, node -> left);
    if (node -> right)
        fprintf(outGraphvizFile, "N%p -> N%p\n", node, node -> right);
    
    
    LOG_PRINTF("(");
    RUN_COMMAND(TreeNodeDump(node -> left, outGraphvizFile, logFile), "PROBLEM WHILE PRINTING LEFT CHILD");

    LOG_PRINTF(TREE_PRINTF_STRING, node -> val);

    RUN_COMMAND(TreeNodeDump(node -> right, outGraphvizFile, logFile), "PROBLEM WHILE PRINTING RIGHT CHILD");
    LOG_PRINTF(")");

    return OK;
}

WORK_RES TreeNodeCreat(binTreeNodeVal_t val, binTreeNode_t **createdNodePtr)
{
    assert(createdNodePtr);
    $INFO("Starting creating new node which will be connected to <%p>", createdNodePtr);

    binTreeNode_t* newNode = (binTreeNode_t *) calloc(1, sizeof(binTreeNode_t));
    if (newNode == NULL) {
        return $ERR(NOMEM, "NOT ENOUGH MEMORY TO CREATE NEW NODE");
    }

    newNode -> val = val;
    *createdNodePtr = newNode;
    
    $INFO("Ending creating new node which will be connected to <%p>", createdNodePtr);
    return OK;
}

WORK_RES TreeNodeDelete(binTreeNode_t **deleteNodePtr)
{
    assert(deleteNodePtr);
    $INFO("Staring deleting node which will is connected to <%p>", deleteNodePtr);

    if (*deleteNodePtr == NULL)
        return OK;

    TreeNodeDelete(&((*deleteNodePtr) -> left));
    TreeNodeDelete(&((*deleteNodePtr) -> right));

    free(*deleteNodePtr);
    *deleteNodePtr = 0;

    $INFO("Ending deleting node which will is connected to <%p>", deleteNodePtr);
    return OK;
}

WORK_RES TreePrintPreorder(const binTreeNode_t *node)
{
    if (!node)
        return OK;

    printf("(");

    printf(TREE_PRINTF_STRING, node -> val);
    TreePrintPreorder(node -> left);
    TreePrintPreorder(node -> right);

    printf(")");

    return OK;
}

WORK_RES TreePrintInorder(const binTreeNode_t *node)
{
    if (!node)
        return OK;

    printf("(");

    TreePrintInorder(node -> left);
    printf(TREE_PRINTF_STRING, node -> val);
    TreePrintInorder(node -> right);

    printf(")");

    return OK;
}

WORK_RES TreePrintPostorder(const binTreeNode_t *node)
{
    if (!node)
        return OK;

    printf("(");

    TreePrintPostorder(node -> left);
    TreePrintPostorder(node -> right);
    printf(TREE_PRINTF_STRING, node -> val);

    printf(")");

    return OK;
}