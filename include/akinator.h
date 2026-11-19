#ifndef AKINATOR
#define AKINATOR

#include "basic.h"
#include "stack.h"

#include <stdio.h>

#define MAX_ANS_LEN 100

const int INIT_STACK_CAPACITY = 20;
const int MAX_IN_STRING_LEN   = 200;

//--------------------------------------------------------------------------------------------------------------------------------------------------------
//                                                   Consts for graphics
//--------------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * Out window init parameters
 */
const int OUT_WINDOW_X     = 800;
const int OUT_WINDOW_Y     = 600;

/**
 * Main akinator text size variables
 */
const int MAIN_TEXT_Y_SIZE = 60;
const int MAIN_TEXT_X_SIZE = 20;
const int MAIN_TEXT_BOLD   = 400;

const int USUAL_TEXT_Y_SIZE = 30;
const int USUAL_TEXT_X_SIZE = 10;

const int MAIN_START_X     = 50;
const int MAIN_START_Y     = 20;
const int MAIN_END_X       = OUT_WINDOW_X - 50;
const int MAIN_END_Y       = MAIN_START_Y + 3 * MAIN_TEXT_Y_SIZE;

/**
 * buttons in menu coordinates
 */
const int BUTTON_X_SIZE = 220;
const int BUTTON_Y_SIZE = 100;
const int GAP_X         = 100;
const int GAP_Y         = 25;

struct button_t {
    int   startX;
    int   startY;
    int   sizeX;
    int   sizeY;
    char *inText;
};

const button_t MENU_BUTTONS[] = {
    {GAP_X, MAIN_END_Y + GAP_Y,                     BUTTON_X_SIZE, BUTTON_Y_SIZE, "Угадать"},
    {GAP_X, MAIN_END_Y + 2 * GAP_Y + BUTTON_Y_SIZE, BUTTON_X_SIZE, BUTTON_Y_SIZE, "Определить"},

    {OUT_WINDOW_X - GAP_X - BUTTON_X_SIZE, MAIN_END_Y + GAP_Y,                                    BUTTON_X_SIZE, BUTTON_Y_SIZE, "Вывести базу"},
    {OUT_WINDOW_X - GAP_X - BUTTON_X_SIZE, MAIN_END_Y + 2 * GAP_Y + BUTTON_Y_SIZE, BUTTON_X_SIZE, BUTTON_Y_SIZE, "Сравнить"},

    {(OUT_WINDOW_X - BUTTON_X_SIZE)/2, MAIN_END_Y + 3 * GAP_Y + 2 * BUTTON_Y_SIZE, BUTTON_X_SIZE, BUTTON_Y_SIZE, "Выйти"},
    {0, 0, 0, 0, ""},
};

const button_t Y_N_BUTTON[] = {
    {GAP_X, MAIN_END_Y + GAP_Y,                                BUTTON_X_SIZE, BUTTON_Y_SIZE, "Да"},
    {OUT_WINDOW_X - GAP_X - BUTTON_X_SIZE, MAIN_END_Y + GAP_Y, BUTTON_X_SIZE, BUTTON_Y_SIZE, "Нет"},
    {0, 0, 0, 0, ""},
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------

enum gameMode_e {
    GUESS       = 'g',
    DEFINE      = 'd',
    COMPARE     = 'c',
    SHOW_TREE   = 'o',
    QUIT        = 'q',
    WRONG_INPUT = '\0',
};

enum userQuestionAns_e {
    YES = 'y',
    NO  = 'n',
};

struct treeWordQuality_t {
    char *word;
    stack_t descriptionStack;
};

const int MAX_LINE_OF_ELEMS_SIZE = 2048;

errorStatus_e Akinator(const char *fileToReadFrom, const char *fileToWriteTo);

errorStatus_e ChooseGameMode(int *userChoose);
errorStatus_e GetUserAnsInOneChar(int *userChoose);

errorStatus_e TakeDataAndInitTree(const char *fileToReadFrom, binTree_t *tree, string_t *inString);
errorStatus_e AddNodeToAkinator(binTreeNode_t **ptrToConnect, char **ptrToRead);
errorStatus_e CheckSymbolAndMove(char **ptrLineToCheck, char Symbol);

errorStatus_e Guess(binTree_t *tree);
errorStatus_e FindWordToCheck(binTreeNode_t **node, binTreeNode_t **prevNode);
errorStatus_e AddUnknownWord(binTreeNode_t *prevNode, binTreeNode_t *node);

errorStatus_e Define(binTree_t *tree); 
errorStatus_e GiveDefinitionTo(binTree_t *tree, treeWordQuality_t *inWord);
errorStatus_e FindNodeQualities(binTreeNode_t *node, treeWordQuality_t *inWord);
errorStatus_e OutStackOfQualities(treeWordQuality_t *wordToDef);
errorStatus_e GetWordInfoFromTree(binTree_t *tree, treeWordQuality_t *wordToDef);
errorStatus_e OutStackIntoString(treeWordQuality_t *wordToDef, int *ind, char *wholeLine);

errorStatus_e Compare(binTree_t *tree);
errorStatus_e CimpareTwoWords(binTree_t *tree, treeWordQuality_t *firstWord, treeWordQuality_t *secondWord);

errorStatus_e ShowTreeInGraphviz(const binTree_t *tree);
errorStatus_e TreeNodeShow(const binTreeNode_t *node, FILE *outGraphvizFile);

errorStatus_e QuitProgram(binTree_t *tree, string_t *inString, const char *fileToWriteTo);
errorStatus_e DeleteAkinatorNode(string_t *startString, binTreeNode_t **nodePtr);
errorStatus_e OutTreeToFile(binTree_t *tree, const char *fileToWriteTo);
errorStatus_e OutNodeToFile(binTreeNode_t *node, FILE *outFile);

errorStatus_e InitOutWindow();
errorStatus_e ShowTextInWindow(const char *inString, ...);
errorStatus_e CreateButton(button_t buttonInfo, COLORREF color);
bool MouseIsInButton(button_t button);

#endif