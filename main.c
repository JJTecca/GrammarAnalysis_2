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
    #define ROWS 11
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
    /*****************************
        UNICODE UTF-32 chars
    ******************************/
    char ***symbols;
    unsigned char init_symbol, termen, factor;
    void (*initializer_members)(char ***symbols, unsigned char init_symbol, unsigned char termen, unsigned char factor); //pointer to function
    handleError_t (*read_table)(void);
}elemente_gramatica;

elemente_gramatica *gramatica = NULL;
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
     if(!param_gramatica || !param_gramatica->symbols) { return HANDLE_GENERAL_ERROR; }
    /*************************************
        Init startings 1 byte vars
    ***************************************/
    param_gramatica->init_symbol = fgetc(gramatica_productie);
    param_gramatica->termen = fgetc(gramatica_productie);
    param_gramatica->factor = fgetc(gramatica_productie);
    /***************************************
        Allocate every char from tabel_actiuni
    ****************************************/
    char temp_buffer[MAX_SYMBOL_LENGTH];
    int current_symbol=0;
    for(unsigned i=0; i< ROWS; i++){
        for(unsigned j=0;j< COLUMNS; j++){
            unsigned idx = 0;
            unsigned char current_char = fgetc(tabel_actiuni);
            while(tabel_actiuni != '\n') {
                while(current_char != ' '){
                    temp_buffer[idx++] = current_char;
                    current_char = fgetc(tabel_actiuni);
                }
                param_gramatica->symbols[i][j][current_symbol++];
                //for(unsigned k=0;k<MAX_SYMBOL_LENGTH;k++)
                    //printf("%c",param_gramatica[i][k][current_symbol]);
                //printf("\n");
                for(unsigned i=0;i<MAX_SYMBOL_LENGTH;i++)
                    temp_buffer[i]='\0';
                idx=0;
                current_char = fgetc(tabel_actiuni);
            }
        }
    }
}
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
    gramatica->init_symbol = gramatica->termen =gramatica->factor = 0;

    handleError_t result = initializer_list_vars(&gramatica->symbols,gramatica->init_symbol,gramatica->termen,gramatica->factor);
    result = read_table(gramatica);

    return HANDLE_GENERAL_ERROR;
}
