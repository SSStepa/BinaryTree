#ifndef AKINATOR
#define AKINATOR

#include "basic.h"

const int MAX_ANS_LEN = 100;

WORK_RES Akinator(const char *fileToReadFrom, const char *fileToWriteTo);

WORK_RES TakeDataAndInitTree(const char *fileToReadFrom, binTree_t *tree, string_t *inString);
WORK_RES AddNodeToAkinator(binTreeNode_t **ptrToConnect, char **ptrToRead);

WORK_RES Guess(binTree_t *tree);
WORK_RES AddUnknownWord(binTreeNode_t *prevNode, binTreeNode_t *node);

WORK_RES OutTreeToFile(binTree_t *tree, const char *fileToWriteTo);
WORK_RES OutNodeToFile(binTreeNode_t *node, FILE *outFile);

WORK_RES ShowTreeInGraphviz(const binTree_t *tree);
WORK_RES TreeNodeShow(const binTreeNode_t *node, FILE *outGraphvizFile);

WORK_RES DeleteAkinatorNode(string_t *startString, binTreeNode_t **nodePtr);

#endif