#define  TX_USE_SPEAK
#include "TXLib.h"

typedef char * binTreeNodeVal_t;
#define TREE_PRINTF_STRING "%s"

#include "binTree.cpp"
#include "../include/akinator.h"
#include <windows.h>

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

    txSpeak("\vПривет. Я Акинатор и я могу угадать твоё слово и много чего ещё:\n");

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

        default:
            printf(RED "Такой команды нет... Увидимся в следующий раз\n" COLOR_RESET);
            return $ERR(WRIN, "UNEXPECTED CHARACTER %d GOT", userAns);
        }
    }
}

errorStatus_e ChooseGameMode(int *userChoose)
{
    assert(userChoose);

    errorStatus_e statusCode = OK;

    txSpeak("\v\ag - Угадаю слово\n");
    txSpeak("\v\ad - Дам определение задуманному слову\n");
    txSpeak("\v\ac - Сравню два выбранных тобой слова\n");
    txSpeak("\v\ao - Выведу дерево ответов\n");
    txSpeak("\v\aq - Закончу работу даже не начав((\n");
    txSpeak("\vВаш ответ: ");

    RUN_COMMAND(GetUserAnsInOneChar(userChoose), "PROBLEM WHILE CHOOSING GAME MODE");
    
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

    txSpeak("\vОтвечай y(Да) или n(Нет) на мои вопросы. В конце ты получишь ответ...\n");
    binTreeNode_t *node = tree -> data;
    binTreeNode_t *prevNode = 0;
    int ans = 0;

    while (node -> left != NULL) {
        txSpeak("\v%s? ", node -> val);

        RUN_COMMAND(GetUserAnsInOneChar(&ans), "PROBLEM WHILE GETTING Y/N ANSWER IN GUESSING");
        prevNode = node;
        switch (ans) {
        case YES:
            node = node -> left;
            break;
        case NO:
            node = node -> right;
            break;
        default:
            return $ERR(WRIN, "UNEXPECTED ANSWER TO MY QUESTION %c", ans);
        }
    }

    txSpeak("\vЭто %s? ", node -> val);

    RUN_COMMAND(GetUserAnsInOneChar(&ans), "PROBLEM WHILE GETTING Y/N ANSWER IN GUESSING");
    switch (ans) {
        case YES:
            printf(GRN);
            txSpeak("\vНу я же говорил\n");
            printf(COLOR_RESET);
            return OK;
        case NO:
            RUN_COMMAND(AddUnknownWord(prevNode, node), "PROBLEM WHILE ADDING NEW NODE");
            printf(YEL);
            txSpeak("\vВ следующий раз не проведёшь\n");
            printf(COLOR_RESET);
            return OK;
        default:
            return $ERR(WRIN, "WRONG USER INPUT IN CHOOSING THE WORD");
    }
}

errorStatus_e AddUnknownWord(binTreeNode_t *prevNode, binTreeNode_t *node)
{
    assert(prevNode);
    assert(prevNode);

    $INFO("starting adding new node to tree");

    char *newObject   = NULL;
    char *newQuestion = NULL;
    size_t buffLen    = 0;

    txSpeak("\vЧто же ты загадал(а): ");
    if (my_getline(&newObject, &buffLen, stdin) == -1)
        return $ERR(NOMEM, "NOT ENOUGH MEMORY FOR NEW STRING");

    buffLen = 0; 

    txSpeak("\vА чем %s отличается от %s он: ", newObject, node -> val);
    if (my_getline(&newQuestion, &buffLen, stdin) == -1)
        return $ERR(NOMEM, "NOT ENOUGH MEMORY FOR NEW STRING");

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

    txSpeak("\vИ ты хочешь что бы я определил: ");
    scanf("%" NUM_TO_STR(MAX_ANS_LEN) "[^\n]", inWord.word);
    ClearBuffer();

    RUN_COMMAND(GiveDefinitionTo(tree, &inWord), "PROBLEM WHILE GIVING DEFINITION OCCUR");

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

    txSpeak("\vПервое слово для сравнения: ");
    scanf("%" NUM_TO_STR(MAX_ANS_LEN) "[^\n]", firstInWord.word);
    ClearBuffer();
    txSpeak("\vВторое слово для сравнения: ");
    scanf("%" NUM_TO_STR(MAX_ANS_LEN) "[^\n]", secondInWord.word);
    ClearBuffer();

    RUN_COMMAND(CimpareTwoWords(tree, &firstInWord, &secondInWord), "PROBLEM WHILE COMPARING");

    $INFO("ending comparing two words");
    return OK;
}

errorStatus_e QuitProgram(binTree_t *tree, string_t *inString, const char *fileToWriteTo)
{
    assert(tree);
 
    OutTreeToFile(tree, fileToWriteTo);

    DeleteAkinatorNode(inString, &(tree -> data));
    free(inString -> str);
    
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

    if (FindNodeQualities(tree -> data, wordToDef) != OK) {
        printf(RED);
        txSpeak("\vСлова которое ты ввёл нет в моей базе\n");
        printf(COLOR_RESET);
        return OK;
    }
 
    RUN_COMMAND(OutStackOfQualities(wordToDef), "ERROR WHILE OUTING STACK");

    $INFO("ending giving definition to %s", wordToDef -> word);
    return statusCode;
}

errorStatus_e CimpareTwoWords(binTree_t *tree, treeWordQuality_t *firstWord, treeWordQuality_t *secondWord)
{
    assert(tree);
    assert(firstWord);
    assert(secondWord);

    $INFO("starting comparing words", firstWord -> word, secondWord -> word);
    errorStatus_e statusCode = OK;
    
    if (FindNodeQualities(tree -> data, firstWord) != OK) {
        printf(RED);
        txSpeak("\vСлова <%s> нет в моей базе", firstWord -> word);
        printf(COLOR_RESET);
        return OK;
    }
    if (FindNodeQualities(tree -> data, secondWord) != OK) {
        printf(RED);
        txSpeak("\vСлова <%s> нет в моей базе", secondWord -> word);
        printf(COLOR_RESET);
        return OK;
    }

    char *firstQuality = 0;
    char *secondQuality = 0;
    if ((firstWord -> descriptionStack).size != 0 && (secondWord -> descriptionStack).size != 0 && 
            strcmp(firstWord -> word, secondWord -> word) == 0) {
        printf(GRN);
        txSpeak("\vОни оба: ");
        printf(COLOR_RESET);
        do {
            RUN_COMMAND(STACK_POP(&(firstWord -> descriptionStack), &firstQuality), "ERROR WHILE TRYING TO POP DATA FROM STACK TO FIRST");
            RUN_COMMAND(STACK_POP(&(secondWord -> descriptionStack), &secondQuality), "ERROR WHILE TRYING TO POP DATA FROM STACK TO SECOND");
            assert(firstQuality);
            assert(secondQuality);

            txSpeak("\v%s", firstQuality);
        } while((firstWord -> descriptionStack).size != 0 && (secondWord -> descriptionStack).size != 0 && 
            strcmp(firstWord -> word, secondWord -> word) == 0);
    
        printf("\b\b  \n");
    } else 
        txSpeak("\vУ них нет Общих черт\n");

    if ((firstWord -> descriptionStack).size != 0) { 
        txSpeak("\vНо ");
        RUN_COMMAND(OutStackOfQualities(firstWord), "PROBLEM WHILE OUTING FIRST WORD STACK");
    }
    if ((secondWord -> descriptionStack).size != 0) { 
        txSpeak("\vA ");
        RUN_COMMAND(OutStackOfQualities(secondWord), "PROBLEM WHILE OUTING SECOND WORD STACK");
    }
    
    $INFO("starting comparing words", firstWord -> word, secondWord -> word);
    return statusCode;
}

errorStatus_e OutStackOfQualities(treeWordQuality_t *wordToDef)
{
    assert(wordToDef);
    
    errorStatus_e statusCode = OK;

    if ((wordToDef -> descriptionStack).size != 0) {
        printf(GRN);
        txSpeak("\v%s", wordToDef -> word);
        printf(COLOR_RESET ": ");
        char *quality = 0;
        do {
            RUN_COMMAND(STACK_POP(&(wordToDef -> descriptionStack), &quality), "ERROR WHILE TRYING TO POP DATA FROM STACK");
            assert(quality);

            txSpeak("\v%s", quality);
        } while((wordToDef -> descriptionStack).size != 0);

        printf("\b\b  \n");
    }

    return OK;
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
