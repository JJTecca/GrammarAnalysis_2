#include <stdio.h>
#include <stdlib.h>
#include <uchar.h>
#include "gramatica_defines.h"

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
    char *stack;
    void (*initializer_members)(char ***symbols, unsigned char init_symbol, unsigned char termen, unsigned char factor); //pointer to function
    handleError_t (*read_table)(void);
}elemente_gramatica;

elemente_gramatica *gramatica = NULL;
char *entry_message = NULL;
handleError_t initializer_list_vars(char *(***symbols), unsigned char init_symbol, unsigned char termen, unsigned char factor) {
    /**************************************************************
       Memory alocation for table: ROWS x COLUMNS x STRINGS
       Each string has a max of 4 symbols
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
    param_gramatica->stack = malloc(50*sizeof(char));
    strcpy(param_gramatica->stack,"$ 0");

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
                printf("%c ", param_gramatica->symbols[i][j][k]);
            }
            printf("\n");
            // Clear temp_buffer
            for(unsigned k=0; k<MAX_SYMBOL_LENGTH; k++) {
                temp_buffer[k] = '\0';
            }
        }
    }
    return HANDLE_SUCCES;
}
int main()
{
    gramatica = malloc(sizeof(elemente_gramatica));
    entry_message = malloc(50*sizeof(char)); //Allocate enough size for message
    if(!gramatica) {
        printf("Error Code: %#x\n", HANDLE_GENERAL_ERROR);
        return HANDLE_GENERAL_ERROR;
    }
    /**********************************
        Init Struct members with NULL
    ***********************************/
    gramatica->symbols = NULL;
    gramatica->init_symbol = gramatica->termen =gramatica->factor = 0;

    /*************************************************
        Function -> returns HANDLE id -> test if SUCCES
    **************************************************/
    handleError_t result = initializer_list_vars(&gramatica->symbols,gramatica->init_symbol,gramatica->termen,gramatica->factor);
    if(result != HANDLE_SUCCES){ perror("Error in function"); return HANDLE_GENERAL_ERROR; }
    result = read_table(gramatica);
    if(result != HANDLE_SUCCES){ perror("Error in function"); return HANDLE_GENERAL_ERROR; }

    //TODO : Test it locally for entry vars
    printf("Initial Symbol %c , Termen %c, Factor %c",gramatica->init_symbol,gramatica->termen,gramatica->factor);
    printf("\nWaiting for entry string: "); scanf("%s",entry_message);
    printf("\nEntry string is: %s",entry_message);
    //NOTA: Nr Stare - 1 este starea adv (pt intersectie)
    return HANDLE_SUCCES;
}
