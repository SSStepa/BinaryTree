#define  TX_USE_SPEAK
#include "TXLib.h"

typedef char* binTreeNodeVal_t;
#define TREE_PRINTF_STRING "%s"

#include "binTree.cpp"
#include "../include/akinator.h"
#include <stdarg.h>

int main(int argc, char *const * argv)
{
    InitLogDir();

    errorStatus_e statusCode = OK;

    const char *fileToReadFrom = "../in.txt";
    const char *fileToWriteTo  = "";
    FlagsParse(argc, argv, &fileToReadFrom, &fileToWriteTo);

    RUN_COMMAND(Akinator(fileToReadFrom, fileToWriteTo), "WORK OF AKINATOR FINISHED WITH ERROR");

    return 0;
}

errorStatus_e Akinator(const char *fileToReadFrom, const char *fileToWriteTo)
{
    assert(fileToReadFrom);
    assert(fileToWriteTo);

    $INFO("starting process of akinator work");
    errorStatus_e statusCode = OK;

    binTree_t tree = {};
    string_t inString = {};
    RUN_COMMAND(TakeDataAndInitTree(fileToReadFrom, &tree, &inString), "ERROR WHILE TRYING TO GET DATA");

    RUN_COMMAND(InitOutWindow(), "PROBLEM WHILE WINDOWS CREATION");
    RUN_COMMAND(ShowTextInWindow("Привет. Я Акинатор и я могу угадать твоё слово и много чего ещё:\n"), "Problem while showing greeting");

    while(true) {
        int userAns = 0;
        ChooseGameMode(&userAns);
        
        switch (userAns) {
        case GUESS:     RUN_COMMAND(Guess(&tree), "PROBLEM WHILE GUESSING");
            break;
        case DEFINE:    RUN_COMMAND(Define(&tree), "PROBLEM WHILE GIVING DEFINITION");
            break;
        case COMPARE:   RUN_COMMAND(Compare(&tree), "PROBLEM WHILE COMPARING TWO WORDS");
            break;
        case SHOW_TREE: RUN_COMMAND(ShowTreeInGraphviz(&tree), "PROBLEM WHILE SHOWING TREE");
            break;
        case QUIT:      return QuitProgram(&tree, &inString, fileToWriteTo);
        case WRONG_INPUT: continue;
        default:
            printf(RED "Такой команды нет... Увидимся в следующий раз\n" COLOR_RESET);
            return $ERR(WRIN, "UNEXPECTED CHARACTER %d GOT", userAns);
        }
    }
}

errorStatus_e ChooseGameMode(int *userChoose)
{
    assert(userChoose);

    ShowTextInWindow("Выбирай режим:");
    while(txMouseButtons() != 1) {
        size_t ind = 0;
        while(MENU_BUTTONS[ind].sizeX != 0) {
            if (MouseIsInButton(MENU_BUTTONS[ind]))  CreateButton(MENU_BUTTONS[ind], TX_CYAN);
            else                                   CreateButton(MENU_BUTTONS[ind], TX_LIGHTCYAN);
            ind++;
        }
    }
    if      (MouseIsInButton(MENU_BUTTONS[0]))  *userChoose = GUESS;        // guess
    else if (MouseIsInButton(MENU_BUTTONS[1]))  *userChoose = DEFINE;       // define
    else if (MouseIsInButton(MENU_BUTTONS[2]))  *userChoose = SHOW_TREE;    // out tree
    else if (MouseIsInButton(MENU_BUTTONS[3]))  *userChoose = COMPARE;      // compare
    else if (MouseIsInButton(MENU_BUTTONS[4]))  *userChoose = QUIT;         // quit
    else                                        *userChoose = WRONG_INPUT;  // wrong input
    
    return OK;
}

errorStatus_e GetUserAnsInOneChar(int *userChoose)
{
    assert(userChoose);
    
    if (scanf("%c", (char *) userChoose) != 1)
        return $ERR(WRIN, "PROBLEM WHILE GETTING USER ANS");
    ClearBuffer();

    return OK;
}

errorStatus_e TakeDataAndInitTree(const char *fileToReadFrom, binTree_t *tree, string_t *inString)
{
    assert(fileToReadFrom);
    assert(tree);
    assert(inString);

    $INFO("starting getting info from file and creating tree");
    errorStatus_e statusCode = OK;

    GetFileFull(fileToReadFrom, inString);

    TREE_CTOR(tree);

    RUN_COMMAND(AddNodeToAkinator(&(tree -> data), &(inString -> str)), "ERROR WHILE RECURSIVELY TRANSLATING STRING TO TREE");
    inString -> str -= inString -> len; // put ptr back to first char

    TreeDump(tree);

    $INFO("ending getting info from file and creating tree");
    return statusCode;
}

errorStatus_e AddNodeToAkinator(binTreeNode_t **ptrToConnect, char **ptrToRead)
{
    assert(ptrToRead);
    assert(*ptrToRead);
    assert(ptrToConnect);

    SkipSpaces(ptrToRead);

    if (CheckSymbolAndMove(ptrToRead, '(') || CheckSymbolAndMove(ptrToRead, '"'))
        return $ERR(WRIN, "SYNTAX ERROR IN FILE");

    TreeNodeCreate(*ptrToRead, ptrToConnect);

    while (CheckSymbolAndMove(ptrToRead, '"') != OK) {
        (*ptrToRead)++; 
        if (**ptrToRead == '\0')
            return $ERR(WRIN, "NO CLOSING \"");
    }
    
    *((*ptrToRead) - 1) = '\0';
    SkipSpaces(ptrToRead);
    
    if (CheckSymbolAndMove(ptrToRead, ')') == OK)
        return OK;
    
    SkipSpaces(ptrToRead);

    if (AddNodeToAkinator(&((*ptrToConnect) -> left),  ptrToRead) != OK)
        return $ERR(WRIN, "PROBLEM IN LEFT SON RECURSION");
    if (AddNodeToAkinator(&((*ptrToConnect) -> right), ptrToRead) != OK)
        return $ERR(WRIN, "PROBLEM IN RIGHT SON RECURSION");
    
    if (CheckSymbolAndMove(ptrToRead, ')'))
        return $ERR(WRIN, "NO CLOSIND ) FOR NODE");

    return OK;
}

errorStatus_e CheckSymbolAndMove(char **ptrLineToCheck, char Symbol)
{
    assert(ptrLineToCheck);
    assert(*ptrLineToCheck);

    if (**ptrLineToCheck != Symbol) 
        return WRIN;
    
    (*ptrLineToCheck)++;

    return OK;
}

errorStatus_e Guess(binTree_t *tree)
{
    assert(tree);

    $INFO("Starting guessing process");
    errorStatus_e statusCode = OK;

    txSetFillColor(TX_WHITE);
    txClear();
    ShowTextInWindow("Отвечай Да или Нет на мои вопросы. В конце ты получишь ответ...\n");

    binTreeNode_t *node = tree -> data;
    binTreeNode_t *prevNode = 0;

    RUN_COMMAND(FindWordToCheck(&node, &prevNode), "PROBLEM WHILE FINDING ELEMENT IN TREE");

    txSetFillColor(TX_WHITE);
    txClear();
    ShowTextInWindow("Это %s? ", node -> val);

    int ans = 0;
    while (!(txMouseButtons() == 1 && (MouseIsInButton(Y_N_BUTTON[0]) || MouseIsInButton(Y_N_BUTTON[1])))) {
        size_t ind = 0;
        while(Y_N_BUTTON[ind].sizeX != 0) {
            if (MouseIsInButton(Y_N_BUTTON[ind]))  CreateButton(Y_N_BUTTON[ind], TX_CYAN);
            else                                   CreateButton(Y_N_BUTTON[ind], TX_LIGHTCYAN);
            ind++;
        }
    }
    if      (MouseIsInButton(Y_N_BUTTON[0])) ans = 'y';
    else if (MouseIsInButton(Y_N_BUTTON[1])) ans = 'n';
    else return $ERR(WRIN, "UNEXPECTED BUTTON PRESS");

    txSetFillColor(TX_WHITE);
    txClear();
    switch (ans) {
        case YES:
            ShowTextInWindow("Ну я же говорил\n");
            return OK;
        case NO:
            RUN_COMMAND(AddUnknownWord(prevNode, node), "PROBLEM WHILE ADDING NEW NODE");
            ShowTextInWindow("В следующий раз не проведёшь\n");
            return OK;
        default:
            return $ERR(WRIN, "WRONG USER INPUT IN CHOOSING THE WORD");
    }
}

errorStatus_e FindWordToCheck(binTreeNode_t **node, binTreeNode_t **prevNode)
{
    assert(node);
    assert(*node);
    assert(prevNode);

    while ((*node) -> left != NULL) {
        txSetFillColor(TX_WHITE);
        txClear();
        ShowTextInWindow("%s? ", (*node) -> val);

        while (!(txMouseButtons() == 1 && (MouseIsInButton(Y_N_BUTTON[0]) || MouseIsInButton(Y_N_BUTTON[1])))) {
            size_t ind = 0;
            while(Y_N_BUTTON[ind].sizeX != 0) {
                if (MouseIsInButton(Y_N_BUTTON[ind]))  CreateButton(Y_N_BUTTON[ind], TX_CYAN);
                else                                   CreateButton(Y_N_BUTTON[ind], TX_LIGHTCYAN);
                ind++;
            }
        }

        *prevNode = *node;
        if      (MouseIsInButton(Y_N_BUTTON[0])) *node = (*node) -> left;
        else if (MouseIsInButton(Y_N_BUTTON[1])) *node = (*node) -> right;
        else return $ERR(WRIN, "UNEXPECTED BUTTON PRESS");
    }

    return OK;
}

errorStatus_e AddUnknownWord(binTreeNode_t *prevNode, binTreeNode_t *node)
{
    assert(prevNode);
    assert(prevNode);

    $INFO("starting adding new node to tree");

    char buff[MAX_IN_STRING_LEN] = {};
    char *newObject   = NULL;
    char *newQuestion = NULL;

    txSetColor(TX_WHITE);
    txClear();

    ShowTextInWindow("Что же ты загадал(а): ");
    GetUserInput(buff);
    newObject = strdup(buff);

    buff[0] = '\0';
    
    ShowTextInWindow("А чем %s отличается от %s он: ", newObject, node -> val);
    GetUserInput(buff);
    newQuestion = strdup(buff);

    binTreeNode_t *connectionPlace = 0;
    if (prevNode -> left == node) {
        TreeNodeCreate(newQuestion, &(prevNode -> left));
        connectionPlace = prevNode -> left;
    }
    else {
        TreeNodeCreate(newQuestion, &(prevNode -> right));
        connectionPlace = prevNode -> right;
    }

    connectionPlace -> right = node; 
    TreeNodeCreate(newObject, &(connectionPlace -> left));

    $INFO("ending adding new node to tree");
    return OK;
}

errorStatus_e Define(binTree_t *tree)
{
    assert(tree);

    errorStatus_e statusCode = OK;

    treeWordQuality_t inWord = {};
    STACK_CTOR(&(inWord.descriptionStack), INIT_STACK_CAPACITY);

    txSetFillColor(TX_WHITE);
    txClear();
    ShowTextInWindow("И ты хочешь что бы я определил: ");

    GetUserInput(inWord.word);

    RUN_COMMAND(GiveDefinitionTo(tree, &inWord), "PROBLEM WHILE GIVING DEFINITION OCCUR");

    StackDtor(&(inWord.descriptionStack));

    return OK;
}

errorStatus_e Compare(binTree_t *tree)
{
    assert(tree);

    $INFO("starting comparing two words");
    errorStatus_e statusCode = OK;

    treeWordQuality_t firstInWord = {};
    treeWordQuality_t secondInWord = {};
    STACK_CTOR(&(firstInWord.descriptionStack), INIT_STACK_CAPACITY);
    STACK_CTOR(&(secondInWord.descriptionStack), INIT_STACK_CAPACITY);

    txSetFillColor(TX_WHITE);
    txClear();
    ShowTextInWindow("Первое слово для сравнения: ");
    GetUserInput(firstInWord.word);

    txSetFillColor(TX_WHITE);
    txClear();
    ShowTextInWindow("Второе слово для сравнения: ");
    GetUserInput(secondInWord.word);

    RUN_COMMAND(CimpareTwoWords(tree, &firstInWord, &secondInWord), "PROBLEM WHILE COMPARING");

    StackDtor(&(firstInWord.descriptionStack));
    StackDtor(&(secondInWord.descriptionStack));

    free(firstInWord.word);
    free(secondInWord.word);

    $INFO("ending comparing two words");
    return OK;
}

errorStatus_e QuitProgram(binTree_t *tree, string_t *inString, const char *fileToWriteTo)
{
    assert(tree);
 
    OutTreeToFile(tree, fileToWriteTo);

    DeleteAkinatorNode(inString, &(tree -> data));
    free(inString -> str);
    
    txDisableAutoPause(); 
    $INFO("ending process of akinator work");
    return OK;
}

errorStatus_e OutTreeToFile(binTree_t *tree, const char *fileToWriteTo)
{
    assert(fileToWriteTo);

    errorStatus_e statusCode = OK;
    FILE *outFile = fopen(fileToWriteTo, "w");
    if (outFile == NULL) {
        return $ERR(FILEERR, "CANT OPEN OUT FILE");
    }

    RUN_COMMAND(OutNodeToFile(tree -> data, outFile), "PROPLEM WHILE ADDING NODES TO FILE");
    
    fclose(outFile);

    return statusCode;
}

errorStatus_e OutNodeToFile(binTreeNode_t *node, FILE *outFile)
{
    assert(outFile);

    if (!node)
        return OK;

    fprintf(outFile, "(");

    fprintf(outFile, "\"%s\"", node -> val);
    OutNodeToFile(node -> left, outFile);
    OutNodeToFile(node -> right, outFile);

    fprintf(outFile, ")");

    return OK;
}

errorStatus_e ShowTreeInGraphviz(const binTree_t *tree)
{
    assert(tree);

    errorStatus_e statusCode = OK;

    FILE *graphvizOutFile = fopen(GRAPHVIZ_TEXT_FILE, "w");
    if (graphvizOutFile == NULL) {
        return $ERR(FILEERR, "CAN'T OPEN GRAPHVIZ TEXT FILE");
    }

    fprintf(graphvizOutFile, "digraph {\n");
    fprintf(graphvizOutFile, "node [shape=Mcircle, style=filled, color=\"#800000\", fillcolor=\"#FF0000\"];\n");
    fprintf(graphvizOutFile, "edge [arrowsize=0.8, color=\"#030352\", penwidth=1];\n");

    TreeNodeShow(tree -> data, graphvizOutFile);

    fprintf(graphvizOutFile, "}\n");
    fclose(graphvizOutFile);

    char commandName[COMMAND_MAX_LEN] = {};
    sprintf(commandName, "iconv -f CP1251 -t UTF-8 %s >RECODE%s", GRAPHVIZ_TEXT_FILE, GRAPHVIZ_TEXT_FILE);
    system(commandName);
    sprintf(commandName, "\"\"C:\\Program Files\\Graphviz\\bin\\dot.exe\" RECODE%s -Tpng -o .\\%s.png\"", GRAPHVIZ_TEXT_FILE, "SHOW_TREE");
    system(commandName);
    system("SHOW_TREE.png");

    return statusCode;
}

errorStatus_e TreeNodeShow(const binTreeNode_t *node, FILE *outGraphvizFile)
{
    assert(outGraphvizFile);

    errorStatus_e statusCode = OK;

    if (node == 0)
        return OK;

    fprintf(outGraphvizFile, "N%p [shape=Mrecord, label=\"{" TREE_PRINTF_STRING "}\", fillcolor=\"#C0C0FF\" color=\"#000080\"]\n", node, node -> val);
    if (node -> left)
        fprintf(outGraphvizFile, "N%p -> N%p[color=\"#00ff22\", label=\"Да\"]\n", node, node -> left);
    if (node -> right)
        fprintf(outGraphvizFile, "N%p -> N%p[color=\"#fd4800\", label=\"Нет\"]\n", node, node -> right);
    else
        fprintf(outGraphvizFile, "N%p [color=\"#00561e\", fillcolor=\"#92ff84\"]", node);
    
    RUN_COMMAND(TreeNodeShow(node -> left,  outGraphvizFile), "PROBLEM WHILE PRINTING LEFT CHILD");
    RUN_COMMAND(TreeNodeShow(node -> right, outGraphvizFile), "PROBLEM WHILE PRINTING RIGHT CHILD");

    return OK;
}

errorStatus_e DeleteAkinatorNode(string_t *startString, binTreeNode_t **nodePtr)
{
    assert(startString);
    assert(nodePtr);

    if (!(*nodePtr)) {
        return OK;
    }

    DeleteAkinatorNode(startString, &((*nodePtr) -> left));
    DeleteAkinatorNode(startString, &((*nodePtr) -> right));

    if (!(startString -> str <= (*nodePtr) -> val && (*nodePtr) -> val < startString -> str + startString -> len))
        free((*nodePtr) -> val);
    
    free(*nodePtr);

    return OK;
}

errorStatus_e GiveDefinitionTo(binTree_t *tree, treeWordQuality_t *wordToDef)
{
    assert(tree);
    assert(wordToDef);

    $INFO("starting giving definition to %s", wordToDef -> word);
    errorStatus_e statusCode = OK;

    if (GetWordInfoFromTree(tree, wordToDef)) return WRIN;
 
    RUN_COMMAND(OutStackOfQualities(wordToDef), "ERROR WHILE OUTING STACK");

    $INFO("ending giving definition to %s", wordToDef -> word);
    return statusCode;
}

errorStatus_e GetWordInfoFromTree(binTree_t *tree, treeWordQuality_t *wordToDef)
{
    assert(tree);
    assert(wordToDef);

    if (FindNodeQualities(tree -> data, wordToDef) != OK) {
        ShowTextInWindow("Слова <%s> нет в моей базе", wordToDef -> word);
        return WRIN;
    }

    return OK;
}

errorStatus_e CimpareTwoWords(binTree_t *tree, treeWordQuality_t *firstWord, treeWordQuality_t *secondWord)
{
    assert(tree);
    assert(firstWord);
    assert(secondWord);

    $INFO("starting comparing words", firstWord -> word, secondWord -> word);
    errorStatus_e statusCode = OK;

    char outLine[MAX_LINE_OF_ELEMS_SIZE] = {};
    int ind = 0;
    
    if (GetWordInfoFromTree(tree, firstWord))  return WRIN;
    if (GetWordInfoFromTree(tree, secondWord)) return WRIN;

    char *firstQuality  = 0;
    char *secondQuality = 0;

    bool hasCommonQualities = false;

    while ((firstWord->descriptionStack).size > 1 && (secondWord->descriptionStack).size > 1) {
        RUN_COMMAND(STACK_POP(&(firstWord->descriptionStack), &firstQuality), "ERROR POP FIRST");
        RUN_COMMAND(STACK_POP(&(secondWord->descriptionStack), &secondQuality), "ERROR POP SECOND");

        if (strcmp(firstQuality, secondQuality) == 0) {
            if (!hasCommonQualities) {
                ind += sprintf(outLine + ind, "Они оба: ");
                hasCommonQualities = true;
            }
            ind += sprintf(outLine + ind, "%s", firstQuality);
        } else {
            RUN_COMMAND(STACK_PUSH(&(firstWord->descriptionStack), firstQuality), "ERROR PUSH FIRST");
            RUN_COMMAND(STACK_PUSH(&(secondWord->descriptionStack), secondQuality), "ERROR PUSH SECOND");
            break;
        }
    }

    if (!hasCommonQualities) 
        ind += sprintf(outLine + ind, "У них нет общих черт ");

    if ((firstWord -> descriptionStack).size != 0) { 
        ind += sprintf(outLine + ind, "\nНо ");
        RUN_COMMAND(OutStackIntoString(firstWord, &ind, outLine), "PROBLEM WHILE OUTING FIRST WORD STACK");
    }
    if ((secondWord -> descriptionStack).size != 0) { 
        ind += sprintf(outLine + ind, "\nА ");
        RUN_COMMAND(OutStackIntoString(secondWord, &ind, outLine), "PROBLEM WHILE OUTING SECOND WORD STACK");
    }
    sprintf(outLine + ind, "\n");

    txSelectFont("Comic Sans MS", USUAL_TEXT_Y_SIZE, USUAL_TEXT_X_SIZE, MAIN_TEXT_BOLD, true);
    ShowTextInWindow(outLine);
    txSelectFont("Comic Sans MS", MAIN_TEXT_Y_SIZE, MAIN_TEXT_X_SIZE, MAIN_TEXT_BOLD, true);
    
    $INFO("starting comparing words", firstWord -> word, secondWord -> word);
    return statusCode;
}

errorStatus_e OutStackOfQualities(treeWordQuality_t *wordToDef)
{
    assert(wordToDef);

    char wholeLine[MAX_LINE_OF_ELEMS_SIZE] = {};
    int ind = 0;
    
    errorStatus_e statusCode = OK;

   RUN_COMMAND(OutStackIntoString(wordToDef, &ind, &*wholeLine), "problem while outing to line");

    sprintf(wholeLine + ind, "\n");
    
    txSelectFont("Comic Sans MS", USUAL_TEXT_Y_SIZE, USUAL_TEXT_X_SIZE, MAIN_TEXT_BOLD, true);
    ShowTextInWindow(wholeLine);
    txSelectFont("Comic Sans MS", MAIN_TEXT_Y_SIZE, MAIN_TEXT_X_SIZE, MAIN_TEXT_BOLD, true);

    return OK;
}

errorStatus_e OutStackIntoString(treeWordQuality_t *wordToDef, int *ind, char *wholeLine)
{
    assert(wordToDef);
    assert(ind);
    assert(wholeLine);

    errorStatus_e statusCode = OK;

    if ((wordToDef -> descriptionStack).size != 0) {
            *ind += sprintf(wholeLine + *ind, "%s", wordToDef -> word);
            *ind += sprintf(wholeLine + *ind, ": ");
            char *quality = 0;

            while((wordToDef -> descriptionStack).size != 1) {
                RUN_COMMAND(STACK_POP(&(wordToDef -> descriptionStack), &quality), "ERROR WHILE TRYING TO POP DATA FROM STACK");
                assert(quality); 

                *ind += sprintf(wholeLine + *ind, "%s", quality);
                if (*ind == MAX_LINE_OF_ELEMS_SIZE) break;
            }
        }
    
    return statusCode;
}

errorStatus_e FindNodeQualities(binTreeNode_t *node, treeWordQuality_t *inWord)
{
    assert(inWord);
    assert(node);

    $INFO("starting checking node %s", node -> val);

    if (node -> left) {
        if (FindNodeQualities(node -> left, inWord) == OK) {
            STACK_PUSH(&(inWord -> descriptionStack), (char *) ", ");
            STACK_PUSH(&(inWord -> descriptionStack), node -> val);
            return OK;
        }
        if (FindNodeQualities(node -> right, inWord) == OK) {
            STACK_PUSH(&(inWord -> descriptionStack), (char *) ", ");
            STACK_PUSH(&(inWord -> descriptionStack), node -> val);
            STACK_PUSH(&(inWord -> descriptionStack), (char *) "не ");
            return OK;
        }

        return WRIN;
    }

    if (strncmp(inWord -> word, node -> val, MAX_ANS_LEN) == 0) {
        return OK;
    }

    return WRIN;
}

errorStatus_e InitOutWindow()
{
    txCreateWindow(OUT_WINDOW_X, OUT_WINDOW_Y);
    txClearConsole();

    txSetFillColor(TX_WHITE);
    txClear();

    txSetColor(TX_BLACK);
    txSelectFont("Comic Sans MS", MAIN_TEXT_Y_SIZE, MAIN_TEXT_X_SIZE, MAIN_TEXT_BOLD, true);

    return OK;
}

errorStatus_e ShowTextInWindow(const char *inString, ...)
{
    assert(inString);

    va_list args = {};
    va_start(args, inString);

    char *outString = (char *) calloc(MAX_IN_STRING_LEN, sizeof(char));

    vsnprintf(outString, MAX_IN_STRING_LEN, inString, args);

    txSetFillColor(TX_WHITE);
    txSetColor(TX_WHITE);
    txRectangle(MAIN_START_X, MAIN_START_Y, MAIN_END_X,   MAIN_END_Y);
    txSetColor(TX_BLACK);

    txDrawText(MAIN_START_X, MAIN_START_Y, 
               MAIN_END_X,   MAIN_END_Y,
               outString
    );
    txDC();

    txSpeak(outString);

    va_end(args);

    return OK;
}

errorStatus_e CreateButton(button_t buttonInfo, COLORREF color)
{
    txBegin();
    txSetFillColor(color);

    txRectangle(
        buttonInfo.startX, buttonInfo.startY, 
        buttonInfo.startX + buttonInfo.sizeX, buttonInfo.startY + buttonInfo.sizeY
    );
    txDrawText(
        buttonInfo.startX, buttonInfo.startY, 
        buttonInfo.startX + buttonInfo.sizeX, buttonInfo.startY + buttonInfo.sizeY,
        buttonInfo.inText
    );
    txEnd();

    return OK;
}

bool MouseIsInButton(button_t button)
{
    if (((txMouseX() >= button.startX) && (txMouseX() <= button.startX + button.sizeX)) && 
        ((txMouseY() >= button.startY) && (txMouseY() <= button.startY + button.sizeY)))
        return true;
    else 
        return false;
}

errorStatus_e GetUserInput(char *outChars)
{
    assert(outChars);

    int len = 0;
    int ind = 0;

    bool keyIsDown[KEYS_TO_CHECK_LEN + 1] = {}; // last for backspace
    bool isDown = false;
    
    DWORD threadId = GetWindowThreadProcessId(txWindow(), NULL);
    
    OutUserLine(outChars);

    while(!txGetAsyncKeyState(VK_RETURN) && len < MAX_IN_STRING_LEN - 1) {
        if (keysToCheck[ind].enVal == 0) {
            ind = 0;
            continue;
        }

        isDown = txGetAsyncKeyState(keysToCheck[ind].enVal);

        if (isDown && !keyIsDown[ind]) {
            char upper = (txGetAsyncKeyState(VK_SHIFT) ^ (GetKeyState(VK_CAPITAL) & 1)) ? 0 : 32;
            if (txGetAsyncKeyState(VK_SPACE)) upper = 0;

            outChars[len++] = (LOWORD(GetKeyboardLayout(threadId)) == EN_LAYOUT_CODE) ? keysToCheck[ind].enVal : keysToCheck[ind].ruVal + upper;
            OutUserLine(outChars);
        } 
        keyIsDown[ind] = isDown;

        isDown = txGetAsyncKeyState(VK_BACK);
        if (isDown &&!keyIsDown[KEYS_TO_CHECK_LEN] && len > 0) {
            outChars[--len] = '\0';
            OutUserLine(outChars);
        }
        keyIsDown[KEYS_TO_CHECK_LEN] = isDown;

        ind++;
    }
    outChars[len] = '\0';

    txSetFillColor(TX_WHITE);
    txClear();

    return OK;
}

errorStatus_e OutUserLine(const char *line)
{
    assert(line);

    txSetColor(TX_CYAN);
    txSetFillColor(TX_WHITE);
    txRectangle(INPUT_START_X, INPUT_START_Y, INPUT_END_X, INPUT_END_Y);

    txSetColor(TX_BLACK);
    txSelectFont("Comic Sans MS", USUAL_TEXT_Y_SIZE, USUAL_TEXT_X_SIZE, MAIN_TEXT_BOLD, true);
    txDrawText(INPUT_START_X, INPUT_START_Y, INPUT_END_X, INPUT_END_Y, line);
    txSelectFont("Comic Sans MS", MAIN_TEXT_Y_SIZE, MAIN_TEXT_X_SIZE, MAIN_TEXT_BOLD, true);

    return OK;
}