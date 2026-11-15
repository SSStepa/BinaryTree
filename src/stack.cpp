#include "../include/stack.h"

errorStatus_e StackCtor(
        stack_t *stk, size_t capacity 
        ON_DBG(, const char *MyName, const char *fileCreationName, int creationLineName, const  char *creationFunctionName)
        )
{
    MY_ASSERT(stk != NULL);

    ON_DBG(
        MY_ASSERT(fileCreationName);
        MY_ASSERT(creationFunctionName);
        MY_ASSERT(MyName);
    )

    if (capacity == 0) {
        return $ERR(WRIN, "CAN'T CREATE STACK WITH 0 CAPACITY");
    }

    if (stk -> capacity != 0 || stk -> data != 0 || stk -> size != 0) {
        return $ERR(WRIN, "INCORRECT STACK TO INITIALIZE");
    }

    stk -> capacity = capacity;
    stk -> data = (stackDataType *) calloc(capacity ON_CANARY(+2), sizeof(stackDataType));
    if (stk -> data == NULL) {
        return $ERR(NOMEM, "NOT ENOUGH MEMORY TO CREATE STACK");
    }

    ON_CANARY(
        stk -> canaryData = stk -> data;
        stk -> data++;
        *(stk -> canaryData) = STACK_CANARY_FIRST;
        *(stk -> data + stk -> capacity) = STACK_CANARY_SECOND;

        stk -> leftCanary = STACK_STRUCT_LEFT_CANARY;
        stk -> rightCanary = STACK_STRUCT_RIGHT_CANARY;
    )
    ON_DBG(
        stk -> MyName = MyName;
        stk -> fileCreationName = fileCreationName;
        stk -> creationLineName = creationLineName;
        stk -> creationFunctionName = creationFunctionName;
    )
    ON_HASH(

        stk -> hashOfData   = HASHER(stk -> data, (stk -> capacity) * sizeof(stackDataType));
        stk -> hashOfStruct = HASHER(stk, sizeof(stack_t));
    )

    errorStatus_e statusCode = OK;
    if ((statusCode = StackIsValid(*stk)) != OK) {
        ON_DBG(
            debuggingInfo_t errInfo = {};
            StackDump(stk, errInfo);
        )
        
        return $ERR(statusCode, "WRONG DATA IN STACK");
    }

    return OK;
}

errorStatus_e StackPush(stack_t *stk, stackDataType Elem ON_DBG(, const char *fileCall, int lineCall)) 
{
    MY_ASSERT(stk);

    errorStatus_e statusCode = OK;

    if ((statusCode = StackIsValid(*stk)) != OK) {
        ON_DBG(
            LOG_START();
            LOG_PRINTF("Error in push called from %s:%d", fileCall, lineCall);
            LOG_END();
        )
        return $ERR(statusCode, "NOT VALID STACK GOT BY FUNCTION");
    }

    if (stk -> capacity == stk -> size) {
        if ((statusCode = ResizeUp(stk)) != OK)
            return statusCode;
    }

    (stk -> data)[(stk -> size)++] = Elem;

    ON_HASH(
        stk -> hashOfData = HASHER(stk -> data, (stk -> capacity) * sizeof(stackDataType));
        stk -> hashOfStruct = 0;
        stk -> hashOfStruct = HASHER(stk, sizeof(stack_t));
    )

    if ((statusCode = StackIsValid(*stk)) != OK) {
        ON_DBG(
            debuggingInfo_t errInfo = {};
            StackDump(stk, errInfo);
            LOG_START();
            LOG_PRINTF("Error in push called from %s:%d", fileCall, lineCall);
            LOG_END();
        )
        return $ERR(statusCode,"NOT VALID STACK AFTER PUSH");
    }

    return OK;
}

errorStatus_e ResizeUp(stack_t *stk)
{
    MY_ASSERT(stk);
    
    stk -> capacity *= 2;
    
    stackDataType *temp = (stackDataType *) realloc(ON_DBG(--)(stk -> data), (stk -> capacity ON_DBG(+2))*sizeof(stackDataType));
    if (temp == NULL) {
        ON_DBG(
            debuggingInfo_t errInfo = {};
            StackDump(stk, errInfo);
        )
        return $ERR(NOMEM, "NOT ENOUGH MEMORY");
    }
    stk -> data = temp;

    ON_CANARY(
        stk -> canaryData = (stk -> data)++;
        *(stk -> canaryData) = STACK_CANARY_FIRST;
        *(stk -> data + stk -> capacity) = STACK_CANARY_SECOND;

        for (size_t ind = stk -> size; ind < stk -> capacity; ind++) {
            (stk -> data)[ind] = STACK_EL_POISON;
        }
    )
    return OK;

}

errorStatus_e StackPop(stack_t *stk, stackDataType *elem ON_DBG(, const char *fileCall, int lineCall))
{
    assert(stk);
    assert(elem); 

    errorStatus_e statusCode = OK;

    // if ((statusCode = StackIsValid(*stk)) != OK) {
    //     ON_DBG(
    //         LOG_START();
    //         LOG_PRINTF("Error in pop called from %s:%d", fileCall, lineCall);
    //         LOG_END();

    //         debuggingInfo_t errInfo = {};
    //         StackDump(stk, errInfo);
    //     )
    //     return $ERR(statusCode, "NOT VALID STACK GOT BY FUNCTION");
    // }

    if (stk -> size == 0) {
        ON_DBG(
            debuggingInfo_t errInfo = {};
            StackDump(stk, errInfo);
        )
        return $ERR(STACK_UNDERFLOW, "CAN'T GET ELEMENT FROM STACK WITHOUT ELEMENTS");
    }

    // if ((stk ->capacity)/4 > stk -> size) {
    //     if ((statusCode = ResizeDown(stk)) != OK) {
    //         return statusCode;
    //     }
    // }

    *elem = (stk -> data)[--(stk -> size)];

    // ON_HASH(
    //     (stk -> data)[(stk -> size)] = STACK_EL_POISON;
    //     stk -> hashOfData = HASHER(stk -> data, (stk -> capacity) * sizeof(stackDataType));
    //     stk -> hashOfStruct = 0;
    //     stk -> hashOfStruct = HASHER(stk, sizeof(stack_t));
    // )

    // if ((statusCode = StackIsValid(*stk)) != OK) {
    //     ON_DBG(
    //         LOG_START();
    //         LOG_PRINTF("Error in pop called from %s:%d", fileCall, lineCall);
    //         LOG_END();

    //         debuggingInfo_t errInfo = {};
    //         StackDump(stk, errInfo);
    //     )
    //     return $ERR(statusCode, "NOT VALID STACK AFTER POP");
    // }

    return OK;
}

errorStatus_e ResizeDown(stack_t *stk)
{
    MY_ASSERT(stk);

    stk -> capacity = stk -> size;
    
    stackDataType *temp = (stackDataType *) realloc(ON_DBG(--)(stk -> data), (stk -> capacity ON_DBG(+2))*sizeof(stackDataType));
    if (temp == NULL) {
        ON_DBG(
            debuggingInfo_t errInfo = {};
            StackDump(stk, errInfo);
        )
        return $ERR(NOMEM, "PROBLEM WHILE TRYING TO REALLOC TO SMALLER SIZE");
    }
    stk -> data = temp;

    ON_CANARY(
        stk -> canaryData = (stk -> data)++;
        *(stk -> canaryData) = STACK_CANARY_FIRST;
        *(stk -> data + stk -> capacity) = STACK_CANARY_SECOND;
    )

    return OK;
}

errorStatus_e StackDtor(stack_t *stk)
{
    MY_ASSERT(stk);

    errorStatus_e statusCode = OK;
    if ((statusCode = StackIsValid(*stk)) != OK) {
        ON_DBG(
            debuggingInfo_t errInfo = {};
            StackDump(stk, errInfo);
        )
        return $ERR(statusCode, "WRONG DATA IN STACK");
    }

    free(ON_CANARY(--)(stk -> data));
    stk -> data = 0;
    stk -> capacity = 0;
    stk -> size = 0;

    return OK;
}

errorStatus_e GetStackCapacity(size_t *capacity)
{
    MY_ASSERT(capacity);

    printf("Hi. What capacity of the stack do you need: ");

    if (scanf("%llu", capacity) == 1)
        return OK;
    else
        return WRIN;

}

errorStatus_e StackIsValid(stack_t stk)
{
    return OK;
    if (stk.data == 0) {
        return $ERR(STACK_DATA, "STACK DATA PTR IS 0");
    }

    if (stk.capacity == 0) {
        return $ERR(STACK_CAP, "STACK CAPACITY IS 0");
    }

    if (stk.size > stk.capacity) {
        return $ERR(STACK_OVERFLOW, "STACK SIZE OF BIGGER THAN CAPACITY");
    }
}

errorStatus_e StackDump(stack_t *stk, debuggingInfo_t errInfo)
{
    return OK;
}

unsigned long long djb2(const unsigned char *data, size_t dataLength) 
{
    unsigned long long hash = 5381;

    for (size_t ind = 0; ind < dataLength; ind++) {
        hash = ((hash << 5) + hash) + data[ind];
    }

    return hash;
}

