#ifndef AKINATOR
#define AKINATOR

#include "basic.h"
#include "stack.h"

#include <stdio.h>
#include <stdlib.h>

#define MAX_ANS_LEN 100

const int INIT_STACK_CAPACITY = 20;

enum gameMode_e {
    GUESS     = 'g',
    DEFINE    = 'd',
    COMPARE   = 'c',
    SHOW_TREE = 'o',
    QUIT      = 'q',
};

enum userQuestionAns_e {
    YES = 'y',
    NO  = 'n',
};

struct treeWordQuality_t {
    char word[MAX_ANS_LEN + 1];
    stack_t descriptionStack;
};

errorStatus_e Akinator(const char *fileToReadFrom, const char *fileToWriteTo);

errorStatus_e ChooseGameMode(int *userChoose);
errorStatus_e GetUserAnsInOneChar(int *userChoose);

errorStatus_e TakeDataAndInitTree(const char *fileToReadFrom, binTree_t *tree, string_t *inString);
errorStatus_e AddNodeToAkinator(binTreeNode_t **ptrToConnect, char **ptrToRead);
errorStatus_e CheckSymbolAndMove(char **ptrLineToCheck, char Symbol);

errorStatus_e Guess(binTree_t *tree);
errorStatus_e AddUnknownWord(binTreeNode_t *prevNode, binTreeNode_t *node);

errorStatus_e Define(binTree_t *tree);
errorStatus_e GiveDefinitionTo(binTree_t *tree, treeWordQuality_t *inWord);
errorStatus_e FindNodeQualities(binTreeNode_t *node, treeWordQuality_t *inWord);
errorStatus_e OutStackOfQualities(treeWordQuality_t *wordToDef);

errorStatus_e Compare(binTree_t *tree);
errorStatus_e CimpareTwoWords(binTree_t *tree, treeWordQuality_t *firstWord, treeWordQuality_t *secondWord);

errorStatus_e ShowTreeInGraphviz(const binTree_t *tree);
errorStatus_e TreeNodeShow(const binTreeNode_t *node, FILE *outGraphvizFile);

errorStatus_e QuitProgram(binTree_t *tree, string_t *inString, const char *fileToWriteTo);
errorStatus_e DeleteAkinatorNode(string_t *startString, binTreeNode_t **nodePtr);
errorStatus_e OutTreeToFile(binTree_t *tree, const char *fileToWriteTo);
errorStatus_e OutNodeToFile(binTreeNode_t *node, FILE *outFile);


#endif