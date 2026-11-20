#ifndef AKINATOR
#define AKINATOR

#include "basic.h"
#include "stack.h"

#include <stdio.h>
#include <Windows.h>

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

// -------------------------------------------------------------------------------   My Input Line -------------------------------------------------------

const int INPUT_START_Y = 400;
const int INPUT_START_X = GAP_X;
const int INPUT_END_Y   = 500;
const int INPUT_END_X   = OUT_WINDOW_X - GAP_X;

/**
 * key info to store in memory. doesn't contain keystate because it is changeble.
 */
struct keyInfo {
    char enVal;
    char ruVal;
};

const int KEYS_TO_CHECK_LEN = 45;
keyInfo keysToCheck[] = {
//   WinCode(en)   | ru 
    {VK_SPACE,      ' '},
    {'A',           'Ф'},
    {'B',           'И'},
    {'C',           'С'},
    {'D',           'В'},
    {'E',           'У'},
    {'F',           'А'},
    {'G',           'П'},
    {'H',           'Р'},
    {'I',           'Ш'},
    {'J',           'О'},
    {'K',           'Л'},
    {'L',           'Д'},
    {'M',           'Ь'},
    {'N',           'Т'},
    {'O',           'Щ'},
    {'P',           'З'},
    {'Q',           'Й'},
    {'R',           'К'},
    {'S',           'Ы'},
    {'T',           'Е'},
    {'U',           'Г'},
    {'V',           'М'},
    {'W',           'Ц'},
    {'X',           'Ч'},
    {'Y',           'Н'},
    {'Z',           'Я'},
    {VK_OEM_3,      'Ё'},
    {VK_OEM_7,      'Э'},
    {VK_OEM_6,      'Ъ'},
    {VK_OEM_1,      'Ж'},
    {VK_OEM_COMMA,  'Б'},
    {VK_OEM_PERIOD, 'Ю'},
    {VK_OEM_5,      'Ё'},

    {'0',           '0'},
    {'1',           '1'},
    {'2',           '2'},
    {'3',           '3'},
    {'4',           '4'},
    {'5',           '5'},
    {'6',           '6'},
    {'7',           '7'},
    {'8',           '8'},
    {'9',           '9'},
    {'\0', '\0'}
};

const int EN_LAYOUT_CODE = 0x0409;

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
    char word[MAX_IN_STRING_LEN];
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
errorStatus_e GetUserInput(char *outChars);
errorStatus_e OutUserLine(const char *line);

#endif