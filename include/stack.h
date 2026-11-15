#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "basic.h"

#ifdef DEBUGGER
#define ON_DBG(...) __VA_ARGS__
#else
#define ON_DBG(...)
#endif

#ifdef CANARY_PROT
#define ON_CANARY(...) __VA_ARGS__
#else
#define ON_CANARY(...)
#endif

#ifdef HASH_PROT
#define ON_HASH(...) __VA_ARGS__
#else
#define ON_HASH(...)
#endif

#define STACK_CTOR(stk, capacity) StackCtor(stk, capacity \
    ON_DBG(         \
        ,TO_STR(stk), \
        __FILE__,     \
        __LINE__,     \
        __FUNCTION__  \
    ) \
)

#define STACK_PUSH(stk, elem) StackPush(stk, elem ON_DBG(, __FILE__, __LINE__))
#define STACK_POP(stk, elem) StackPop(stk, elem ON_DBG(, __FILE__, __LINE__))

#define HASHER(data, dataLength) djb2((const unsigned char *) data, dataLength)

typedef char *stackDataType;
#define FILL_FOR_PRINTF "%s"

struct stack_t {
    ON_CANARY(unsigned long long leftCanary;)

    stackDataType *data;
    size_t size;
    size_t capacity;

    ON_DBG(
    const char *MyName;
    const char *fileCreationName;
    int   creationLineName;
    const char *creationFunctionName;
    )
    ON_CANARY(
    stackDataType *canaryData;
    )
    ON_HASH(
    unsigned long long hashOfData;
    unsigned long long hashOfStruct;
    )

    ON_CANARY(unsigned long long rightCanary;)
};

errorStatus_e StackCtor(
        stack_t *stk, size_t capacity 
        ON_DBG(,const char *Myname, const char *fileCreationName, int creationLineName, const  char *creationFunctionName)
    );
errorStatus_e StackDtor(stack_t *stk);

errorStatus_e StackPush(stack_t *stk, stackDataType Elem ON_DBG(, const char *fileCall, int lineCall));
errorStatus_e ResizeUp(stack_t *stk);

errorStatus_e StackPop(stack_t *stk, stackDataType *elem ON_DBG(, const char *fileCall, int lineCall));
errorStatus_e ResizeDown(stack_t *stk);

errorStatus_e GetStackCapacity(size_t *capacity);

errorStatus_e StackIsValid(stack_t stk);

errorStatus_e StackDump(stack_t *stk, debuggingInfo_t errInfo);

unsigned long long djb2(const unsigned char *data, size_t dataLength);

#endif
