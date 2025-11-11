#include <stdio.h>
#include <stdlib.h>
#include <uchar.h>
#include "gramatica_defines.h"
/************************************
    PARSER IMPLEMENTATION
    Description: This program implements a shift-reduce parser
    for arithmetic expressions using grammar tables
    Files needed:
    - gramatica_productie.txt (grammar productions)
    - tabel_actiuni.txt (action table)
    Input: Arithmetic expressions like "id+id*id $"
    Output: Parsing steps and acceptance/rejection
*************************************/
/************************************
        Define Macros
*************************************/
/************************************
    Assign fixed size vars in data segment
*************************************/
/************************************
        Define Enums
*************************************/
/************************************
        Define Structs
*************************************/
/************************************
        Define Global Variables
*************************************/
/************************************
        Define Global functions
*************************************/
//check if header exists with guard word GRAMATICA_DEFINES
#ifndef GRAMATICA_DEFINES
    #define GRAMATICA_DEFINES
    #define ROWS 12
    #define COLUMNS 9
    #define MAX_SYMBOL_LENGTH 4
#endif // GRAMATICA_DEFINES
static FILE* gramatica_productie = NULL; //visible only in .c
static FILE* tabel_actiuni = NULL;
size_t termen_size, factor_size, init_symbol_size;
volatile static char *entry_word = NULL;
typedef enum{
    HANDLE_SUCCES = 0xAAAAAA33,
    HANDLE_MALLOC_FAIL = 0xAAAAAAFF,
    HANDLE_FILE_FAIL = 0xAAAAAACC,
    HANDLE_GENERAL_ERROR = 0xC0000005
}handleError_t;
typedef struct{
    /*************************************************************
        Members Declaration + Functions that might be needed later
    *************************************************************/
    char ***symbols;
    unsigned char init_symbol, termen, factor;
    char *stack,*left,**right;
    void (*initializer_members)(char ***symbols,char **left,char** right, char **stack, unsigned char init_symbol, unsigned char termen, unsigned char factor); //pointer to function
    handleError_t (*read_table)(void);
}elemente_gramatica;

elemente_gramatica *gramatica = NULL;
/**********************************************************
    This can be left as NULL and in main read from keyboard
**********************************************************/
char entry_message[50] = "id+id+id+id+id*id $";

/***********************************
    Global Functions Description:
    1. Declare local variables
    2. Allocate memory if needed, init local vars
    3. Validate input parameters
    4. Process the data and validate them
    5. Cleanup and return status
***********************************/
handleError_t initializer_list_vars(char *(***symbols),char **left,char *(**right),char **stack, unsigned char init_symbol, unsigned char termen, unsigned char factor) {
    /**************************************************************
       Memory alocation for table: ROWS x COLUMNS x STRINGS
       Each string has a max of 4 symbols
       *symbol = the string from cell symbol[i][j]
       **symbol = the 2D array
       ***symbol = the pointer to 2D array
       NOTE: If the symbols would have been simple chars then **symbol
    *************************************************************/
    *symbols = malloc(ROWS * sizeof(char**));
    if(!(*symbols)) {
        perror("Initialize malloc failed for rows");
        return HANDLE_MALLOC_FAIL;
    }
    for(unsigned i = 0; i < ROWS; i++) {
        (*symbols)[i] = malloc(COLUMNS * sizeof(char*));
        if(!(*symbols)[i]) {
            perror("Initialize malloc failed for columns");
            return HANDLE_MALLOC_FAIL;
        }
        for(unsigned j = 0; j < COLUMNS; j++) {
            (*symbols)[i][j] = malloc(MAX_SYMBOL_LENGTH * sizeof(char));
            if(!(*symbols)[i][j]) {
                perror("Initialize malloc failed for strings");
                return HANDLE_MALLOC_FAIL;
            }
            (*symbols)[i][j][0] = '\0'; // initialize empty string
        }
    }
    /*****************************************
        Allocate memory enough as it needs changes
        *stack -> 50 chars
        *left -> 9 chars
        *right -> 36 chars
    ******************************************/
    *stack = malloc(50*sizeof(char));
    *left = malloc(COLUMNS*sizeof(char));
    *right = malloc(COLUMNS*MAX_SYMBOL_LENGTH*sizeof(char*));
    if (!stack || !left || !right) {
        perror("Memory allocation failed");
        return HANDLE_MALLOC_FAIL;
    }
    for(unsigned i=0;i<COLUMNS*MAX_SYMBOL_LENGTH; i++){
        (*right)[i]=malloc(MAX_SYMBOL_LENGTH*sizeof(char));
        if(!(*right)[i]) {
            perror("Memory allocation failed");
            return HANDLE_MALLOC_FAIL;
        }
        (*right)[i][0] = '\0'; // NOTE: Put another * as it get's error for subscripted value
    }
    gramatica_productie = fopen("gramatica_productie.txt","r");
    tabel_actiuni = fopen("tabel_actiuni.txt","r");
    if(tabel_actiuni == NULL || gramatica_productie == NULL) {
        printf("One of files failed to open");
        perror("Error while opening file");
        return HANDLE_FILE_FAIL;
    }
    return HANDLE_SUCCES;
}
handleError_t read_table(elemente_gramatica *param_gramatica){
    //Prevent pointer dereference
    if(!param_gramatica || !param_gramatica->symbols) {
        return HANDLE_GENERAL_ERROR;
    }

    /*************************************
        Init startings 1 byte vars
    ***************************************/
    param_gramatica->init_symbol = fgetc(gramatica_productie); fgetc(gramatica_productie); //SKIP SPACE
    param_gramatica->termen = fgetc(gramatica_productie); fgetc(gramatica_productie); //SKIP SPACE
    param_gramatica->factor = fgetc(gramatica_productie);
    strcpy(param_gramatica->stack,"$ 0 ");

    /***************************************
        Allocate every char from tabel_actiuni
    ****************************************/
    char temp_buffer[MAX_SYMBOL_LENGTH];
    for(unsigned i=0;i<ROWS;i++){
        for(unsigned j = 0; j<COLUMNS;j++){
            unsigned idx = 0;
            int current_char = fgetc(tabel_actiuni); // Using it to handle EOF
            while(current_char!=' ' && current_char != '\n' && current_char != EOF && idx < MAX_SYMBOL_LENGTH) {
                temp_buffer[idx++] = (char)current_char;
                current_char = fgetc(tabel_actiuni);
            }

            // Null terminate the buffer
            temp_buffer[idx] = '\0';
            // Copy all from temp buffer into symbols
            for(unsigned k = 0; k < idx && k < MAX_SYMBOL_LENGTH; k++){
                param_gramatica->symbols[i][j][k] = temp_buffer[k];
            }
            param_gramatica->symbols[i][j][idx] = '\0'; // Null terminator
            // TODO : Test it locally
            printf("Symbols[%d][%d] = ", i, j);
            for(unsigned k=0; k<MAX_SYMBOL_LENGTH && param_gramatica->symbols[i][j][k] != '\0'; k++){
                printf("%c", param_gramatica->symbols[i][j][k]);
            }
            printf("\n");
            // Clear temp_buffer
            for(unsigned k=0; k<MAX_SYMBOL_LENGTH; k++) {
                temp_buffer[k] = '\0';
            }
        }
    }
    //free(temp_buffer); DO NOT FREE
    return HANDLE_SUCCES;
}
handleError_t read_production(elemente_gramatica *param_gramatica) {
    fgetc(gramatica_productie); //simulate to get ENTER
    char ch = fgetc(gramatica_productie), temp_buffer[MAX_SYMBOL_LENGTH];
    unsigned length_right,length_left,idx,row;
    length_left = length_right = idx = row = 0;
    while (ch != 0x0A && ch != EOF){
        ch = fgetc(gramatica_productie);
    }
    param_gramatica->init_symbol = fgetc(gramatica_productie);
    fgetc(gramatica_productie); //simulate ENTER
    ch = fgetc(gramatica_productie);
    while(ch != EOF) {
        *(param_gramatica->left+(length_left++)) = ch;
        fgetc(gramatica_productie); //simulate SPACE
        ch = fgetc(gramatica_productie);
        idx = 0;
        while(ch != 0x0A && ch != EOF) {
            temp_buffer[idx++] = ch;
            ch = fgetc(gramatica_productie);
        }
        //Copy from buffer to right storages
        for(unsigned k = 0; k < idx && k < MAX_SYMBOL_LENGTH; k++){
            param_gramatica->right[row][k] = temp_buffer[k];
        }
        //MUST Put terminator char as they differ in length
        param_gramatica->right[row][idx] = '\0';
        for(unsigned k=0; k<MAX_SYMBOL_LENGTH; k++) {
                temp_buffer[k] = '\0';
        }
        row++;
        ch = fgetc(gramatica_productie); //simulate ENTER
    }
    //TODO : Test it locally
    printf("\n");
    for(unsigned i = 0; i < row; i++) {
        printf("Production %d: %c -> %s\n", i, param_gramatica->left[i], param_gramatica->right[i]);
    }
    printf("Total productions: %d\n", row);
    return HANDLE_SUCCES;
}
handleError_t special_case(elemente_gramatica *param_gramatica)
{
    /***************************************************************
        This case shall handle if we have double digits in the back
        As stack_top is a char, we atoi to integer and read from back
    ****************************************************************/
    unsigned digits = 0, last_char = 1, column = 0, stack_length = strlen(param_gramatica->stack);
    char temp_buffer[MAX_SYMBOL_LENGTH];

    if (param_gramatica == NULL || param_gramatica->stack == NULL) {
        perror("Memory allocation failed");
        return HANDLE_MALLOC_FAIL;
    }

    // Count digits from the end of stack
    while(last_char <= stack_length &&
          isdigit(param_gramatica->stack[stack_length - last_char])) {
        last_char++;
    }
    last_char--;

    // Allocate memory for digits
    char *concat_digits = malloc((last_char + 1) * sizeof(char));
    if (concat_digits == NULL) {
        perror("Memory allocation failed");
        return HANDLE_MALLOC_FAIL;
    }

    // Copy digits and null terminate
    strncpy(concat_digits, param_gramatica->stack + stack_length - last_char, last_char);
    concat_digits[last_char] = '\0';

    digits = atoi(concat_digits);

    // Find column with "$"
    for(column = 0; strcmp(param_gramatica->symbols[0][column], "$") != 0; column++);

    // Get intersection
    char intersection[MAX_SYMBOL_LENGTH];
    strcpy(intersection, param_gramatica->symbols[digits + 1][column]);

    if(intersection[0] == 'd') {
    } else {
        last_char++;
        last_char *= 2;
        stack_length -= last_char;
        param_gramatica->stack[++stack_length] = '\0';
    }
    free(concat_digits);
    printf("\nStiva %s",param_gramatica->stack);
    strcpy(intersection,param_gramatica->symbols[stack_length][5]);
    if(strcmp(intersection,"x")){
        printf("respingere");
        return HANDLE_GENERAL_ERROR;
        exit(1);
    }
    last_char = 1;
    while(isdigit(param_gramatica->stack[strlen(param_gramatica->stack)-last_char]) &&
          isalpha(param_gramatica->stack[strlen(param_gramatica->stack)-last_char-1]) &&
          strcmp(intersection,"x")!=0){
        last_char++;
        last_char *=2;
        stack_length -= last_char;
        param_gramatica->stack[stack_length] = '\0';
        printf("\nStiva %s",param_gramatica->stack);
        strcpy(intersection,param_gramatica->symbols[stack_length+1][5]);
        if(strcmp(intersection,"x")==0){
            printf("\nrespingere");
        }
    }

    //Special case handle
    if(strcmp(" $",entry_message)==0 || strcmp("id $",entry_message)){
        printf("\nacceptare");
    }else{
        printf("\respins");
    }
    exit(1);
    return HANDLE_SUCCES;
}
handleError_t automat_evo(elemente_gramatica *param_gramatica) {
    if(!param_gramatica || !param_gramatica->symbols || !param_gramatica->stack
       || !param_gramatica->left || !param_gramatica->right) {
           perror("Memory allocation failed");
            exit(HANDLE_MALLOC_FAIL);
    }
    for(unsigned i=0;i<strlen(entry_message)-1;i++){
        if(strchr("*-()+",entry_message[i]) && strchr("*-()+",entry_message[i+1])){
            printf("respingere");
            exit(HANDLE_GENERAL_ERROR);
        }
    }
    /*************************************************
        FIND INTERSECTION of entry string and stack top
        Copy into temp buffer -> clear temp buffer
        Assign to a normal char the top of stack, it's easier to see
        Assign to a normal char* the intersection, it's easier to see
        Run the while till it's equal to final $
        Concatenate into temp_buffer if it's with 'd' from "deplasament"
        Reduce if with 'r' , pop from stack
        Find the intersection of rows and columns based on stack_top as row
        For reduce 'r', mark the last stack top from the last 'd' deplasament
    ***************************************************/
    char temp_buffer[MAX_SYMBOL_LENGTH],stack_top='\0';
    char *intersection;
    unsigned idx=0,stack_length=strlen(param_gramatica->stack),column,d_stack_top,find_char;
    unsigned ascii=0; //Used for conversions to int
    unsigned first_run = 1, special_case_row = 0;
    while(strcmp(entry_message,"")!=0){
        /*****************************************************************
            Reset vars -> find stack top -> find intersection -> establish
        ******************************************************************/
        idx=0; column=0;
        intersection = malloc(MAX_SYMBOL_LENGTH*sizeof(char));
        while(entry_message[idx] != '+' && entry_message[idx] != '*'
              && entry_message[idx] != ' ' && entry_message[idx] != '$' &&
              entry_message[idx] != '-' && entry_message[idx] != '/' &&
              entry_message[idx] != '(' && entry_message[idx] != ')')
                { idx++; }
        //Particular case
        if(entry_message[0]=='+' || entry_message[0]=='*'){ idx++; }
        strncpy(temp_buffer,entry_message,idx);
        //Point to the top of the stack
        if(first_run){
            stack_top=param_gramatica->stack[strlen(param_gramatica->stack)-2]-'0'; //Convert to numeric from ASCII
            first_run = 0;
        }else{
            stack_top=param_gramatica->stack[strlen(param_gramatica->stack)-1]-'0'; //Convert to numeric from ASCII
        }
        //Find the column with the right symbol
        //Have to check for the particular case where only $ is left
        if(strcmp(entry_message," $")==0){
            strcpy(intersection,param_gramatica->symbols[stack_top+1][5]);
        }else{
            for(column=0;strcmp(temp_buffer,param_gramatica->symbols[0][column])!=0;column++);
            strcpy(intersection,param_gramatica->symbols[stack_top+1][column]);
        }

        //At least last 2 chars are digits
        if(isdigit(param_gramatica->stack[stack_length-1]) && isdigit(param_gramatica->stack[stack_length-2])){
           special_case(param_gramatica);
           goto clear;
        }
        if(intersection[0]=='d'){
            strcat(param_gramatica->stack,temp_buffer); //add the buffer
            stack_length+=idx; //adjust the stack length
            param_gramatica->stack[stack_length]=intersection[1]; //add numeric index to stack
            param_gramatica->stack[stack_length+1] ='\0'; //mark end
            strcpy(entry_message+0,entry_message+idx);
            if(idx==1){ stack_length++; } //Save + 1 for the next element
            d_stack_top = stack_top; // Pass the new stack_top for the next reduce
        }else{
            stack_length-=2;
            //Put the left i-th element and null terminator
            ascii = intersection[1] - '0';
            param_gramatica->stack[stack_length++] = param_gramatica->left[ascii-1];
            //Make it eligible for strcmp
            temp_buffer[0] = param_gramatica->left[ascii-1];
            temp_buffer[1] ='\0';
            for(find_char=0;strcmp(temp_buffer,param_gramatica->symbols[0][find_char]);find_char++);
            //Display all chars of intersection , some of them are '10'
            unsigned intersect_length =0;
            while(intersect_length<strlen(param_gramatica->symbols[d_stack_top+1][find_char])){
                param_gramatica->stack[stack_length++] = param_gramatica->symbols[d_stack_top+1][find_char][intersect_length];
                intersect_length++;
            }
            param_gramatica->stack[stack_length] = '\0';
            //Find intersection between F E T etc. and
        }
        //Clear all temp resources
    clear:
        for(unsigned k=0;k<MAX_SYMBOL_LENGTH;k++){
            temp_buffer[k]='\0';
        }
        if(param_gramatica->stack[strlen(param_gramatica->stack)-1]=='x'){
                special_case(param_gramatica);
        }
        free(intersection);
        printf("\nStiva: %s",param_gramatica->stack);
    }
    //printf("%s",param_gramatica->symbols[stack_top+1][0]);
    return HANDLE_SUCCES;
}
/************************************
    MAIN EXECUTION FLOW:
    1. Initialize grammar structure
    2. Allocate memory for symbols table
    3. Read action table from file
    4. Read grammar productions  
    5. Execute parser algorithm
    6. Cleanup and return status
*************************************/
int main()
{
    gramatica = malloc(sizeof(elemente_gramatica));
    if(!gramatica) {
        printf("Error Code: %#x\n", HANDLE_GENERAL_ERROR);
        return HANDLE_GENERAL_ERROR;
    }
    /**********************************
        Init Struct members with NULL
    ***********************************/
    gramatica->symbols = NULL;
    gramatica->init_symbol = gramatica->termen = gramatica->factor = 0;

    /*************************************************
        Function -> returns HANDLE id -> test if SUCCES
    **************************************************/
    handleError_t result = initializer_list_vars(&gramatica->symbols,&gramatica->left,&gramatica->right,&gramatica->stack,
                                                 gramatica->init_symbol,gramatica->termen,gramatica->factor);
    if(result != HANDLE_SUCCES){ perror("Error in function"); return HANDLE_GENERAL_ERROR; }

    result = read_table(gramatica);
    if(result != HANDLE_SUCCES){ perror("Error in function"); return HANDLE_GENERAL_ERROR; }

    //TODO : Test it locally for entry vars
    printf("Initial Symbol %c , Termen %c, Factor %c",gramatica->init_symbol,gramatica->termen,gramatica->factor);
    printf("\nStack: %s",gramatica->stack);
    //If we want to read it from keyboard
    //printf("\nWaiting for entry string: "); scanf("%s",entry_message);
    //printf("\nEntry string is: %s",entry_message);
    result = read_production(gramatica);
    if(result != HANDLE_SUCCES){ perror("Error in function"); return HANDLE_GENERAL_ERROR; }

    result = automat_evo(gramatica);
    if(result != HANDLE_SUCCES){ perror("Error in function");printf("\nrespingere"); return HANDLE_GENERAL_ERROR; }

exit:
    return HANDLE_SUCCES;
}
