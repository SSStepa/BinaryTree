typedef char * binTreeNodeVal_t;
#define TREE_PRINTF_STRING "%s"

#include "binTree.cpp"
#include "../include/akinator.h"
#include <windows.h>

int main(int argc, char *const *argv)
{
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    InitLogDir();

    WORK_RES statusCode = OK;

    const char *fileToReadFrom = "../in.txt";
    const char *fileToWriteTo  = "";
    FlagsParse(argc, argv, &fileToReadFrom, &fileToWriteTo);

    RUN_COMMAND(Akinator(fileToReadFrom, fileToWriteTo), "WORK OF AKINATOR FINISHED WITH ERROR");

    return 0;
}

WORK_RES Akinator(const char *fileToReadFrom, const char *fileToWriteTo)
{
    assert(fileToReadFrom);
    assert(fileToWriteTo);

    $INFO("starting process of akinator work");
    WORK_RES statusCode = OK;

    binTree_t tree = {};
    string_t inString = {};
    RUN_COMMAND(TakeDataAndInitTree(fileToReadFrom, &tree, &inString), "ERROR WHILE TRYING TO GET DATA");

    printf("Привет. Я Акинатор и я могу угадать твоё слово и много чего ещё:\n");

    while(true) {
        printf("[g]uess     - Угадаю слово\n");
        // printf("[d]efine    - дам определение задуманному слову\n");
        // printf("[c]ompare   - сравню два выбранных тобой слова\n");
        printf("[o]ut       - выведу дерево, как оно лежит в моей память\n");
        printf("[q]uit      - Закончу работу даже не начав((\n");
        printf("Ваш ответ: ");

        int ans = getchar();
        getchar();
        switch (ans) {
        case 'g':
            RUN_COMMAND(Guess(&tree), "PROBLEM WHILE GUESSING OCCUR");
            break;
        // case 'd':
        //     RUN_COMMAND(GiveDefinitionTo(&tree), "PROBLEM WHILE GIVING DEFINITION OCCUR");
        //     break;
        // case 'c':
        //     RUN_COMMAND(CompareTwoObjects(&tree), "PROBLEM WHILE COMPARING OCCUR");
        //     break;
        case 'o':
            RUN_COMMAND(ShowTreeInGraphviz(&tree), "PROBLEM WHILE SHOWING TREE");
            system("SHOW_TREE.png");
            break;
        case 'q':
            OutTreeToFile(&tree, fileToWriteTo);

            DeleteAkinatorNode(&inString, &(tree.data));
            free(inString.str);
            
            $INFO("ending process of akinator work");
            return OK;
        default:
            return $ERR(WRIN, "UNEXPECTED CHARACTER %c GOT", ans);
        }
    }
}

WORK_RES TakeDataAndInitTree(const char *fileToReadFrom, binTree_t *tree, string_t *inString)
{
    assert(fileToReadFrom);
    assert(tree);
    assert(inString);

    $INFO("starting getting info from file and creating tree");
    WORK_RES statusCode = OK;

    GetFileFull(fileToReadFrom, inString);

    TREE_CTOR(tree);

    RUN_COMMAND(AddNodeToAkinator(&(tree -> data), &(inString -> str)), "ERROR WHILE RECURSIVELY TRANSLATING STRING TO TREE");

    TreeDump(tree);

    $INFO("ending getting info from file and creating tree");
    return statusCode;
}

WORK_RES AddNodeToAkinator(binTreeNode_t **ptrToConnect, char **ptrToRead)
{
    if (*((*ptrToRead)) != '(' || *((*ptrToRead) + 1) != '"')
        return $ERR(WRIN, "SYNTAX ERROR IN FILE");
    (*ptrToRead) += 2;

    TreeNodeCreat(*ptrToRead, ptrToConnect);

    while (**ptrToRead != '"' && **ptrToRead != '\0')
        (*ptrToRead)++;
    
    if (**ptrToRead == '\0')
        return $ERR(WRIN, "NO CLOSING \"");
    
    **ptrToRead = '\0';
    (*ptrToRead)++;
    
    if (**ptrToRead == ')') {
        (*ptrToRead)++;
        return OK;
    }

    AddNodeToAkinator(&((*ptrToConnect) -> left),  ptrToRead); 
    AddNodeToAkinator(&((*ptrToConnect) -> right), ptrToRead); 
    
    if (**ptrToRead != ')')
        return $ERR(WRIN, "NO CLOSIND ) FOR NODE");

    (*ptrToRead)++;

    return OK;
}

WORK_RES Guess(binTree_t *tree)
{
    assert(tree);

    $INFO("Starting guessing process");
    WORK_RES statusCode = OK;

    printf("Отвечай y(Да) или n(Нет) на мои вопросы. В конце ты получишь ответ...\n");
    binTreeNode_t *node = tree -> data;
    binTreeNode_t *prevNode = 0;
    int ans = 0;
    while (node -> left != NULL) {
        printf("%s? ", node -> val);

        ans = getchar();
        getchar();
        prevNode = node;
        switch (ans) {
        case 'y':
            node = node -> left;
            break;
        case 'n':
            node = node -> right;
            break;
        default:
            return $ERR(WRIN, "UNEXPECTED ANSWER TO MY QUESTION %c", ans);
        }
    }

    printf("Это %s? ", node -> val);
    ans = getchar();
    getchar();

    switch (ans) {
        case 'y':
            printf(GRN "Ну я же говорил\n" COLOR_RESET);
            return OK;
        case 'n':
            RUN_COMMAND(AddUnknownWord(prevNode, node), "PROBLEM WHILE ADDING NEW NODE");
            printf(YEL "В следующий раз не проведёшь\n" COLOR_RESET);
            return OK;
        default:
            return $ERR(WRIN, "WRONG USER INPUT IN CHOOSING THE WORD");
    }
}

WORK_RES AddUnknownWord(binTreeNode_t *prevNode, binTreeNode_t *node)
{
    assert(prevNode);
    assert(prevNode);

    $INFO("starting adding new node to tree");

    char ans[MAX_ANS_LEN] = {};

    printf("Что же ты загадал(а): ");
    scanf("%s", ans);
    getchar();
    char *newObject = strdup(ans);

    printf("А чем %s отличается от %s он: ", newObject, node -> val);
    scanf("%s", ans);
    getchar();
    char *newQuestion = strdup(ans);

    binTreeNode_t *connectionPlace = 0;
    if (prevNode -> left == node) {
        TreeNodeCreat(newQuestion, &(prevNode -> left));
        connectionPlace = prevNode -> left;
    }
    else {
        TreeNodeCreat(newQuestion, &(prevNode -> right));
        connectionPlace = prevNode -> right;
    }

    connectionPlace -> right = node; 

    TreeNodeCreat(newObject, &(connectionPlace -> left));

    $INFO("ending adding new node to tree");
    return OK;
}

WORK_RES OutTreeToFile(binTree_t *tree, const char *fileToWriteTo)
{
    assert(fileToWriteTo);

    WORK_RES statusCode = OK;
    FILE *outFile = fopen(fileToWriteTo, "w");
    if (outFile == NULL) {
        return $ERR(FILEERR, "CANT OPEN OUT FILE");
    }

    RUN_COMMAND(OutNodeToFile(tree -> data, outFile), "PROPLEM WHILE ADDING NODES TO FILE");
    
    fclose(outFile);

    return statusCode;
}

WORK_RES OutNodeToFile(binTreeNode_t *node, FILE *outFile)
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

WORK_RES ShowTreeInGraphviz(const binTree_t *tree)
{
    assert(tree);

    WORK_RES statusCode = OK;

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
    sprintf(commandName, "\"\"C:\\Program Files\\Graphviz\\bin\\dot.exe\" .\\%s -Tpng -o .\\%s.png\"", GRAPHVIZ_TEXT_FILE, "SHOW_TREE");
    system(commandName);

    return statusCode;
}

WORK_RES TreeNodeShow(const binTreeNode_t *node, FILE *outGraphvizFile)
{
    assert(outGraphvizFile);

    WORK_RES statusCode = OK;

    if (node == 0)
        return OK;

    fprintf(outGraphvizFile, "N%p [shape=Mrecord, label=\"{" TREE_PRINTF_STRING "}\", fillcolor=\"#C0C0FF\" color=\"#000080\"]\n", node, node -> val);
    if (node -> left)
        fprintf(outGraphvizFile, "N%p -> N%p[color=\"#00ff22\", label=\"Да\"]\n", node, node -> left);
    if (node -> right)
        fprintf(outGraphvizFile, "N%p -> N%p[color=\"#fd4800\", label=\"Нет\"]\n", node, node -> right);
    else
        fprintf(outGraphvizFile, "N%p [color=\"#00561e\", filecolor=\"#92ff84\"]", node);
    
    RUN_COMMAND(TreeNodeShow(node -> left,  outGraphvizFile), "PROBLEM WHILE PRINTING LEFT CHILD");
    RUN_COMMAND(TreeNodeShow(node -> right, outGraphvizFile), "PROBLEM WHILE PRINTING RIGHT CHILD");

    return OK;
}

WORK_RES DeleteAkinatorNode(string_t *startString, binTreeNode_t **nodePtr)
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