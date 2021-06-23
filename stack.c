#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
 
#define ERROR 0

/*
    işletim sistemi 32 bit veya 64 bit olmasına göre bura ayarlanmalı
*/
// Check windows
#if _WIN32 || _WIN64
   #if _WIN64
     #define ENV64BIT
  #else
    #define ENV32BIT
  #endif
#endif

// Check GCC
#if __GNUC__
  #if __x86_64__ || __ppc64__
    #define ENV64BIT
  #else
    #define ENV32BIT
  #endif
#endif

// long int 8 byte
#ifdef ENV64BIT
    #define ADRESS_CAST long int
#endif

// int 4 byte varsayıldı.
#ifdef ENV32BIT
    #define ADRESS_CAST int
#endif

// A structure to represent a stack
struct Stack {
    int top;
    unsigned capacity;
    ADRESS_CAST* array;
};
 
// function to create a stack of given capacity. It initializes size of
// stack as 0
struct Stack* createStack(unsigned capacity)
{
    struct Stack* stack = (struct Stack*)malloc(sizeof(struct Stack));
    stack->capacity = capacity;
    stack->top = -1;
    stack->array = (ADRESS_CAST*)malloc(stack->capacity * sizeof(ADRESS_CAST));
    return stack;
}
 
// Stack is full when top is equal to the last index
int isFull(struct Stack* stack)
{
    return stack->top == stack->capacity - 1;
}
 
// Stack is empty when top is equal to -1
int isEmpty(struct Stack* stack)
{
    return stack->top == -1;
}
 
// Function to add an item to stack.  It increases top by 1
void push(struct Stack* stack, ADRESS_CAST item)
{
    if (isFull(stack))
        return;
    stack->array[++stack->top] = item;
}
 
// Function to remove an item from stack.  It decreases top by 1
ADRESS_CAST pop(struct Stack* stack)
{
    if (isEmpty(stack))
        return ERROR;
    return stack->array[stack->top--];
}
 
// Function to return the top from stack without removing it
ADRESS_CAST peek(struct Stack* stack)
{
    if (isEmpty(stack))
        return ERROR;
    return stack->array[stack->top];
}
 
// // Driver program to test above functions
// int main()
// {
//     struct Stack* stack = createStack(100);
 
//     push(stack, 10);
//     push(stack, 20);
//     push(stack, 30);
 
//     printf("%d popped from stack\n", pop(stack));
 
//     return 0;
// }