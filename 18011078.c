#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.c"
#include "assert.h"

/**
 * @brief bir streamden prefix agacı yapısını oluşturup sonra onu grapha çevirip merge eden algoritma.
 * 
 * fstream örneği
 * (Ben,bir,$(ariyim,$)(inegim,$)(tavugum,$))
 * (pandemic,$(definition,$)(vs,epidemic,$)(in,history,$))
 */ 

// #include "linkedlist.c"

#define STREAM_FILE_NAME "stream.txt"
#define STREAM_LINE_SZ 1000
#define MAX_WORD_LENGTH 25
#define MAX_GROUP_COUNT 10
#define MAX_STRING_COUNT 15
#define MAX_WORD_COUNT 1000 // arama butonuna yazılan maksimum kelime 
#define TYPE_PREFIX_HOLDER 1
#define TYPE_WORD_HOLDER 0 // sonra geliştirilecek
#define DEFAULT_COUNT 0

typedef int boolean;

// for '$' word types gives search count.
typedef struct Node {
    char* word;
    int typeORcount; // adv.this takes 1 or 0 for new word or preffix 
    struct Node* neighboors; // word $ iken cast Linked list, yoksa prefix tree
} PreffixTree;


typedef struct LinkedList {
    struct Node* string; // this actually word not string
    struct LinkedList* next;
} LinkedList;

typedef struct graph_node
{
    char* string;
    struct graph_node *next;
    struct graph_node *neighboors;
} GraphNode ;

typedef struct AdjGraph
{
    GraphNode* last_string_node;
    GraphNode* strings;
    int string_count;
} AdjGraph;

void insert_after_ll(struct Node * insertable_node, struct Node * data) {
    if (insertable_node->neighboors == NULL)
    {
        insertable_node->neighboors = (struct LinkedList*) malloc(sizeof(LinkedList));
        ( (LinkedList*) insertable_node->neighboors )->string = data;
        return;
    }
    LinkedList * ll =  ( (LinkedList*) insertable_node->neighboors );
    while( ll->next != NULL )
    {
        ll = ll->next ;
    }
    ll->next = (struct LinkedList*) malloc(sizeof(LinkedList));
    ll->next->string = data;
    
    // debug purpose
    ll->next->next = NULL;
}

PreffixTree* read_stream_and_treelize(FILE *fp) 
{
    char ch;

        // using three stack
        
        // paranthesis , end of string, nodes (memory)
    struct Stack* parenthesis_stack = createStack(MAX_GROUP_COUNT);
    struct Stack* end_of_string_stack = createStack(MAX_STRING_COUNT);
    struct Stack* word_stack = createStack(MAX_STRING_COUNT);

    struct Stack* string_stack_inorder = createStack(MAX_WORD_LENGTH);

    int itr_word = 0;

    PreffixTree* root;
    int i = 0;
    char word[MAX_WORD_LENGTH];

    while( (ch = fgetc(fp)) != EOF )
    {

        if( ch == '(' ) 
        {
            push(parenthesis_stack, ch);
        }
        else if( ch == ')' )
        {
        
            pop(parenthesis_stack);
            pop(end_of_string_stack);

            if( !isEmpty(word_stack)  ) 
            {
                // last element out from here root


                push(string_stack_inorder, pop(word_stack));
                
                // long int a = peek(word_stack);
                // struct Node *b = a;
                // printf("\n%s", b->word);

                while( !isEmpty(word_stack) && strcmp(((struct Node*) peek(word_stack))->word , "$"))
                {
                    push(string_stack_inorder, pop(word_stack));                
                }

                //write the neighboors that pulled stack order.
                boolean IsStringStackEmpty = isEmpty(string_stack_inorder);

                struct Node* itr_prev_node;
                struct Node* string_start;

                int k = 0;

                if(!IsStringStackEmpty) 
                {
                    string_start = (struct Node*) pop(string_stack_inorder);
                    itr_prev_node = string_start;
                }

                while(!isEmpty(string_stack_inorder))
                {
                    struct Node* m_Node = (struct Node*) pop(string_stack_inorder); 
                    
                    
                    itr_prev_node->neighboors = m_Node;
                    itr_prev_node = itr_prev_node->neighboors;

                }
                
                // add string to neighboor of $ word of more generic one.
                struct Node* end_of_generic_string = peek(end_of_string_stack);

                if( end_of_generic_string == NULL )
                {
                    root = (PreffixTree*) string_start;
                    return root;
                }


                //printf("a0");
                insert_after_ll(end_of_generic_string, string_start);

                // m_Node->neighboors->word = word_;
            } 
            else
            {
                printf("Paranthesis Error");
                
            } 
        }
        else if( ch == ',' )
        {
            int len_word = i;
            struct Node* m_Node = (struct Node*) malloc(sizeof(struct Node*));
            m_Node->word = (char*) malloc(len_word * sizeof(char));

            m_Node->typeORcount = TYPE_PREFIX_HOLDER;
            strcpy(m_Node->word ,word);
            m_Node->word[i] = '\0';
            
            push(word_stack, m_Node);
            i = 0;
        }
        else if( ch == '$' )
        {
            word[i] = '\0';
            struct Node* m_NeighborrHolderNode = (struct Node*) malloc(sizeof(struct Node*));
            m_NeighborrHolderNode->typeORcount = DEFAULT_COUNT ;// this is count
            m_NeighborrHolderNode->word = (char*) malloc(sizeof(char)*2);
            m_NeighborrHolderNode->word = "$\0";
            push(word_stack, m_NeighborrHolderNode);
            push(end_of_string_stack, m_NeighborrHolderNode);
            // i = 0;
        }
        else {
            word[i++] = ch;
        }

        printf("%c", ch);
    }

    return root;
}


void create_string_with_buffer_by_reference( GraphNode* graph_node, char* buffer, struct Node** itr, int * count )
{
    int sz = 0; // <reent_data> warning cygwin
    while(strcmp((*itr)->word, "$")) {
        strcat(buffer, (*itr)->word);
        *itr = (*itr)->neighboors;
        strcat(buffer, " ");
        sz++;
    }
    char* m_char = (char*) malloc(sizeof(sz));
    strcpy(m_char, buffer);
    graph_node->string = m_char;
    (*count)++;
}

struct Node * create_string_with_buffer_by_value( GraphNode* graph_node, char* buffer, struct Node* itr, int * count )
{
    int sz = 0; // ı don t get <reent_data> error here but also putting
    while(strcmp(itr->word, "$")) {
        strcat(buffer, itr->word);
        itr = itr->neighboors;
        strcat(buffer, " ");
        sz++;
    }
    char* m_char = (char*) malloc(sizeof(sz));
    strcpy(m_char, buffer);
    graph_node->string = m_char;
    (*count)++;
    return itr;
}

/*

    Prefix agacında tüm kelimelerin bir kez geçtiğini varsayıyorum .

*/
void add_sub_strings_to_adj_list(PreffixTree * tree, AdjGraph * graph) 
{
    // assert tree node is '$'
    // assert(strcmp(tree->word,"$") == 0 );
    LinkedList * neighboors = ((LinkedList*) tree->neighboors);
    if(neighboors == NULL)
    {
        return;
    }

    while(neighboors != NULL)
    {
        char string[STREAM_LINE_SZ]; // buffer
        string[0] = '\0';
    
        // for all neighboors of $
        struct Node* itr = neighboors->string;

        GraphNode* last_node = graph->last_string_node;
        graph->last_string_node->next = (GraphNode*) malloc(sizeof(GraphNode));

        itr = create_string_with_buffer_by_value(last_node->next, string, itr, &graph->string_count);

        graph->last_string_node = last_node->next;
        // end for

        add_sub_strings_to_adj_list(itr, graph);
        neighboors = neighboors->next;
    }
    
    
}

AdjGraph * init_adj_list(PreffixTree * tree)
{
    AdjGraph *graph = (AdjGraph*) malloc(sizeof(AdjGraph));
    graph->strings = (GraphNode*) malloc(sizeof(GraphNode));    
    PreffixTree *itr = tree;
    char string[STREAM_LINE_SZ];
    string[0] = '\0'; 
    // read first string from tree
    create_string_with_buffer_by_reference(graph->strings, string, &itr,  &(graph->string_count));

    graph->last_string_node = graph->strings;
    graph->strings->next = NULL;
    // graph->string_count = 1;

    add_sub_strings_to_adj_list(itr, graph);


    return graph;
}

void init_graph_connected_component(PreffixTree* tree, AdjGraph* graph)
{

}

int main() 
{
    FILE *fp;
    // char ch;

    fp = fopen(STREAM_FILE_NAME, "r"); // read mode


    /*
    debug konsolundan kontrol edilebilir.
    */

    // stream txt de verilen streami okur. ve grafa dönüştürülebilecek ve sorgu için kullanılabilecek ağaç oluşturur.
    // debug konsolundan görülüyor.
    PreffixTree * tree_line1 = read_stream_and_treelize(fp);
    fgetc(fp); fgetc(fp);

    PreffixTree * tree_line2 = read_stream_and_treelize(fp);

    // first iteration for find strings and creating adj list of nodes that used as reference
    AdjGraph* graph1 = init_adj_list(tree_line1);


    init_graph_connected_component(tree_line1, graph1);


    // prefix_tree_to_show(tree_line1);
    
    // // 
    // //PreffixTree * tree_line2 = read_stream_and_treelize(fp);

    // prefix_tree_to_show(tree_line2);


    // AdjGraph * graph = tree_to_adj_graph(tree_line1);
    // AdjGraph * graph2 = tree_to_adj_graph(tree_line2);
    
    // AdjGraph * graph_merged = merge_graph(graph1, graph2);

    // printf("%s", tree->word);
    
    // echo_neighboors(graph_merged, simpleNode);
    
    fclose(fp);
    return 0;
}



// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include "stack.c"
// #include "linkedlist.c"

// #define STREAM_FILE_NAME "stream.txt"
// #define STREAM_LINE_SZ 1000
// #define MAX_WORD_LENGTH 25
// #define MAX_GROUP_COUNT 10
// #define MAX_STRING_COUNT 15
// #define MAX_WORD_COUNT 1000 // arama butonuna yazılan maksimum kelime 
// #define TYPE_PREFIX_HOLDER 1
// #define TYPE_WORD_HOLDER 0 // sonra geliştirilecek
// #define DEFAULT_COUNT 0

// typedef int boolean;

// typedef struct LinkedList {
//     SNode *list;
//     int count;
// } LinkedList;

// // for '$' word types gives search count.
// typedef struct Node {
//     char* word;
//     int typeORcount; // this takes 1 or 0 for new word or preffix
//     // int neighboorsCount;

//     LinkedList* neighboors; // word $ iken komşuluk, yoksa prefix tree
// } PreffixTree;



// PreffixTree* read_stream_and_treelize() 
// {
//     FILE *fp;
//     char ch;

//     fp = fopen(STREAM_FILE_NAME, "r"); // read mode

//     if (fp == NULL)
//     {
//         perror("Error while opening the file.\n");
//         exit(EXIT_FAILURE);
//     }

//         // using three stack
        
//         // paranthesis , end of string, nodes (memory)
//     struct Stack* parenthesis_stack = createStack(MAX_GROUP_COUNT);
//     struct Stack* end_of_string_stack = createStack(MAX_STRING_COUNT);
//     struct Stack* word_stack = createStack(MAX_STRING_COUNT);

//     struct Stack* string_stack_inorder = createStack(MAX_WORD_LENGTH);

//     int itr_word = 0;

//     PreffixTree* root;
//     int i = 0;
//     char word[MAX_WORD_LENGTH];

//     while( (ch = fgetc(fp)) != EOF )
//     {

//         if( ch == '(' ) 
//         {
//             push(parenthesis_stack, ch);
//         }
//         else if( ch == ')' )
//         {
        
//             char isParenthesis = (char) pop(parenthesis_stack);
//             char isEndOfString = (char) pop(end_of_string_stack);

//             if( isParenthesis  && isEndOfString  ) 
//             {
//                 // last element out from here root
//                 if( isEmpty(word_stack)  )
//                 {
//                     return root;
//                 }

//                 push(string_stack_inorder, pop(word_stack));

//                 // long int a = peek(word_stack);
//                 // struct Node *b = a;
//                 // printf("\n%s", b->word);

//                 while( strcmp(((struct Node*) peek(word_stack))->word , "$"))
//                 {
//                     push(string_stack_inorder, pop(word_stack));                
//                 }

//                 //write the neighboors that pulled stack order.
//                 boolean IsStringStackEmpty = isEmpty(string_stack_inorder);

//                 struct Node* itr_prev_node;
//                 struct Node* string_start;

//                 int k = 0;

//                 if(!IsStringStackEmpty) 
//                 {
//                     string_start = (struct Node*) pop(string_stack_inorder);
//                     itr_prev_node = string_start;
//                 }

//                 while(!isEmpty(string_stack_inorder))
//                 {
//                     struct Node* m_Node = (struct Node*) pop(string_stack_inorder); 
                    
                    
//                     itr_prev_node->neighboors = m_Node;
//                     itr_prev_node = itr_prev_node->neighboors;

//                 }
                
//                 // add string to neighboor of $ word of more generic one.
//                 struct Node* end_of_generic_string = peek(end_of_string_stack);
//                 printf("a0");
//                 while( end_of_generic_string->neighboors != NULL)
//                 {
//                     end_of_generic_string = end_of_generic_string->neighboors ;
//                 }
//                 // if(end_of_generic_string->neighboors == NULL) 
//                 // {
//                 //     end_of_generic_string->neighboors = (SNode*) malloc(sizeof(SNode*));
//                 // }
//                 end_of_generic_string->neighboors = insert_back_sll(end_of_generic_string->neighboors->list, string_start);
//                 end_of_generic_string->neighboors->count++;
//                 printf("%d", ((struct Node*) end_of_generic_string->neighboors->list->data)->word);
//                 printf("s");
//                 // m_Node->neighboors->word = word_;
//             } 
//             else
//             {
//                 printf("STREAM FAULT EXCEPTION");
//             } 
//         }
//         else if( ch == ',' )
//         {
//             int len_word = i;
//             struct Node* m_Node = (struct Node*) malloc(sizeof(struct Node*));
//             m_Node->word = (char*) malloc(len_word * sizeof(char));
//             m_Node->neighboors = NULL;
//             m_Node->typeORcount = TYPE_PREFIX_HOLDER;
//             strcpy(m_Node->word ,word);
//             m_Node->word[i] = '\0';
            
//             push(word_stack, m_Node);
//             i = 0;
//         }
//         else if( ch == '$' )
//         {
//             word[i] = '\0';
//             struct Node* m_NeighborrHolderNode = (struct Node*) malloc(sizeof(struct Node*));
//             m_NeighborrHolderNode->typeORcount = DEFAULT_COUNT ;// this is count
//             m_NeighborrHolderNode->word = (char*) malloc(sizeof(char)*2);
//             m_NeighborrHolderNode->word = "$\0";
//             m_NeighborrHolderNode->neighboors = NULL;
//             push(word_stack, m_NeighborrHolderNode);
//             push(end_of_string_stack, m_NeighborrHolderNode);
//             // i = 0;
//         }
//         else {
//             word[i++] = ch;
//         }

//         printf("%c", ch);
//     }

//     fclose(fp);
//     return root;
// }




// void read_stream_and_treelize() 
// {
//     FILE *fp;
//     char ch;

//     fp = fopen(STREAM_FILE_NAME, "r"); // read mode

//     if (fp == NULL)
//     {
//         perror("Error while opening the file.\n");
//         exit(EXIT_FAILURE);
//     }

//         // using three stack

//         // paranthesis , end of string, nodes (memory)
//     struct Stack* parenthesis_stack = createStack(MAX_GROUP_COUNT);
//     struct Stack* end_of_string_stack = createStack(MAX_STRING_COUNT);
//     struct Stack* word_stack = createStack(MAX_WORD_CHAR_COUNT);

//     int itr_word = 0;

//     while((ch = fgetc(fp)) != EOF)
//     {
//         if( ch == '(' ) 
//         {
//             push(parenthesis_stack, ch);
//         }
//         else if( ch == ')' )
//         {
//             // lots of magic

//             char isParenthesis = (char) pop(end_of_string_stack);

//             if( isParenthesis > 0 ) 
//             {
//                 // // reversing stack
//                 // struct Stack* string_stack_inorder = createStack(MAX_WORD_CHAR_COUNT);
//                 // int word_size_arr[MAX_WORD_COUNT];
//                 // int i = 0;
//                 // int word_length = 0; 

//                 // while(!isEmpty(word_stack))
//                 // {
//                 //     if( ch == ',' )
//                 //     {
//                 //         word_size_arr[i] = word_length; // first goes 0
//                 //         i++;
//                 //         word_length = 0;
//                 //         continue;
//                 //     }
//                 //     word_length++;
//                 //     push(string_stack_inorder, pop(word_stack));                
//                 // }
//                 // word_size_arr[i] = word_length;


//                 // PreffixTree* graph = (PreffixTree*) malloc(sizeof(PreffixTree)); 
//                 // char* word = (char*) malloc(sizeof(word_size_arr[i]));

//                 // int j = 0;
//                 // while(!isEmpty(string_stack_inorder))
//                 // {
//                 //     char popped_ch = pop(string_stack_inorder); 
//                 //     if( popped_ch == ',')
//                 //     {
//                 //         graph->word = word;
//                 //         j = 0;
//                 //     }
//                 //     word[j++] = popped_ch;
//                 // }

//             } 
//             else
//             {
//                 printf("STREAM FAULT EXCEPTION");
//             } 
//         }
//         else if( ch == '$' )
//         {
//             push(end_of_string_stack, ch);
//         }
//         else {
//             push(word_stack, ch);
//         }

//         printf("%c", ch);
//     }

//     fclose(fp);
// }


// int main() 
// {
//     read_stream_and_treelize();
//     return 0;
// }






// #define FIRST_MANUAL_GRAPH_SZ 4
// #define INIT_NODE_STRING_SZ 1
// #define INIT_NODE_NEIGHBOOR_SZ 3

// typedef struct node {
//     char* string;
//     int count;
//     Node* next;
// } Node;

// typedef struct adjacency_list {
//     char * string;
//     int count;
//     Node *childs;
// } AdjList;

// typedef struct graph {
//     int size_node;
//     AdjList *initial_node;
// } Graph;

// // int create_manual_test_graph() 
// // {
// //     Graph* graph = (Graph*) malloc(sizeof(Graph));
// //     graph->size_node = FIRST_MANUAL_GRAPH_SZ;
// //     graph->initial_node = (AdjList*) malloc(sizeof(AdjList));
// //     graph->initial_node->string = (char*) malloc(sizeof(char) * INIT_NODE_STRING_SZ);
// //     graph->initial_node->string = 'A';
// //     graph->initial_node->count = 0;
// //     graph->initial_node->childs = (Node*) malloc(sizeof(Node) * INIT_NODE_NEIGHBOOR_SZ );
// //     graph->initial_node->childs->string = (char*) malloc(sizeof(char) * INIT_NODE_STRING_SZ);
// //     graph->initial_node->childs->string->

// // }

// int createStreamFromTxt() {

// }


