/**
 * File: macros.c
 * 
 * Description:
 *  This file provides functionality for handling macros in an assembly language processing system. It includes 
 *  functions for managing macro definitions, storing macro content, and substituting macro invocations with their
 *  content. The file implements a hash table to efficiently store and retrieve macros.
 *
 * Functions:
 *  hash(): Computes a hash value for a given string to index into the hash table.
 *  init_table(): Initializes the hash table for storing macros.
 *  add_macro(): Adds a new macro to the hash table with a specified name.
 *  find_macro(): Retrieves a macro from the hash table by its name.
 *  free_table1(): Frees all allocated memory associated with the hash table and its elements.
 *  macro(): Processes the given file to replace macro invocations with their definitions and save the result to an output file.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define SIZE_LINE 82
#define TABLE_SIZE 100

typedef struct {
    char name[SIZE_LINE];
    int lineCount;
    char lines[SIZE_LINE][SIZE_LINE];
} Macro;

typedef struct HashNode {
    Macro *macro;
    struct HashNode *next;
} HashNode;

typedef struct {
    HashNode *buckets[TABLE_SIZE];
} HashTable;

HashTable *table = NULL;


/**
 * Computes a hash value for a given string.
 * 
 * @param str The string to hash.
 * @return The computed hash value.
 */

unsigned int hash(char *str) {
    unsigned int hash = 0;
    while (*str) {
        hash = (hash * 31) + *str++;
    }
    return hash % TABLE_SIZE;
}


/**
 * Initializes the hash table for storing macros.
 */

void init_table() {
    int i;
    table = (HashTable *)malloc(sizeof(HashTable));
    if (!table) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    for ( i = 0; i < TABLE_SIZE; i++) {
        table->buckets[i] = NULL;
    }
}


/**
 * Adds a new macro to the hash table with a specified name.
 * 
 * @param name The name of the macro.
 * @return Pointer to the newly created Macro structure.
 */

Macro *add_macro(char *name) {
    unsigned int index = hash(name);
    HashNode *newNode = (HashNode *)malloc(sizeof(HashNode));
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    newNode->macro = (Macro *)malloc(sizeof(Macro));
    if (newNode->macro == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    strcpy(newNode->macro->name, name);
    newNode->macro->lineCount = 0;
    newNode->next = NULL;
    printf("macro added at index %d", index);
    HashNode *node = table->buckets[index];
    if (node == NULL) {
        table->buckets[index] = newNode;
        return newNode->macro;
    }
    while (node->next != NULL) {
           node=node->next;
    }
       node->next = newNode;
   
    return newNode->macro;
}


/**
 * Retrieves a macro from the hash table by its name.
 * 
 * @param name The name of the macro to find.
 * @return Pointer to the Macro structure if found, NULL otherwise.
 */

Macro *find_macro(char *name) {
    unsigned int index = hash(name);
    HashNode *node = table->buckets[index];
    while (node != NULL) {
        printf("Node %s\n", node->macro->name);
        if (strcmp(node->macro->name, name) == 0) {
            return node->macro;
        }
        node = node->next;
    }
    printf("macro not found at index %d", index);
    return NULL;
}


/**
 * Frees all allocated memory associated with the hash table and its elements.
 */

void free_table1() {
    int i;
    for (i = 0; i < TABLE_SIZE; i++) {
        HashNode *node = table->buckets[i];
        while (node != NULL) {
            HashNode *temp = node;
            node = node->next;
            free(temp->macro);  
            free(temp);  
        }
    }
    free(table);  
}


/**
 * Processes the given file to replace macro invocations with their definitions and save the result to an output file.
 * 
 * @param fileName The base name of the input file (without extension).
 */

void macro(char *fileName) {
    FILE *fromUser;
    FILE *outputFile;
    int i;
    char line[SIZE_LINE] = {0};
    char *asFileName = malloc(strlen(fileName) + 4);
    char *amFileName = malloc(strlen(fileName) + 4);
    if (asFileName == NULL || amFileName == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    strcpy(asFileName, fileName);
    strcat(asFileName, ".as");

    fromUser = fopen(asFileName, "r");
    if (fromUser == NULL) {
        fprintf(stderr, "Error opening file: %s\n", asFileName);
        free(asFileName);
        free(amFileName);
        return;
    }

    strcpy(amFileName, fileName);
    strcat(amFileName, ".am");

    outputFile = fopen(amFileName, "w");
    if (outputFile == NULL) {
        fprintf(stderr, "Error opening output file: %s\n", amFileName);
        fclose(fromUser);
        free(asFileName);
        free(amFileName);
        return;
    }

    init_table();

    bool isMacroOpen = false;
    Macro *currentMacro = NULL;

    while (fgets(line, SIZE_LINE, fromUser)) {
         printf( "line **%s**",line);
        char *ms = strstr(line, "macr ");
        if (ms != NULL && ms == line) {
            char macroName[SIZE_LINE];
            sscanf(line, "macr %s\n", macroName);
            printf( "starting macro entry **%s**\n",macroName);
            currentMacro = add_macro(macroName);
            isMacroOpen = true;
            continue;
        }else if (strstr(line, "endmacr") != NULL) {
            printf( "ending macro entry\n");
            isMacroOpen = false;
            currentMacro = NULL;
            continue;
        } else if (isMacroOpen) {
            printf( "reading in macro line\n");
            if (currentMacro != NULL && currentMacro->lineCount < SIZE_LINE) {
                strcpy(currentMacro->lines[currentMacro->lineCount++], line);
            } else {
                fprintf(stderr, "Macro %s exceeded maximum number of lines\n", currentMacro->name);
            }
        } else {
            char firstWord[SIZE_LINE];
            sscanf(line, "%s\n", firstWord);
            Macro *foundMacro = find_macro(firstWord);
            if (foundMacro != NULL) {
                printf( "found macro- replacing with content\n");
                for ( i = 0; i < foundMacro->lineCount; i++) {
                    fputs(foundMacro->lines[i], outputFile);
                }
            } else {
                printf( "copying simple line\n");
                fputs(line, outputFile);
            }
        }
     

    }

    fclose(fromUser);
    fclose(outputFile);
    free(asFileName);
    free(amFileName);
    free_table1();
}




