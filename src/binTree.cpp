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

    // RUN_COMMAND(TreeNodeDelete(&(tree -> data)), "PROBLEM WHILE DELETING ALL TREE");

    $INFO("Ending tree deconstruction");

    return statusCode;
}
