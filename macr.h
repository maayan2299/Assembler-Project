/**
 * File: macr.h
 * 
 * Description:
 *  This header file defines the structures and function declarations used for handling macros in the assembly language
 *  processing system. It provides data structures for storing macro definitions and function prototypes for adding,
 *  searching, and printing macros, as well as processing a file to replace macros with their content.
 *
 * Data Structures:
 *  Macro: Represents a macro with its name, the number of lines it contains, and the lines of code within the macro.
 *
 * Functions:
 *  add_macro(): Adds a new macro to the macro table with a specified name.
 *  find_macro(): Finds and returns a pointer to a macro by its name.
 *  print_macros(): Prints all macros that have been added to the macro table.
 *  macro(): Processes the given file, replacing macro invocations with their definitions and saving the result to an output file.
 */

#ifndef MACR_H
#define MACR_H

#include <stdbool.h>

#define SIZE_LINE 82

/**
 * Structure representing a macro with a name and its associated lines of code.
 */

typedef struct {
    char name[SIZE_LINE];   
    int lineCount;          
    char lines[SIZE_LINE][SIZE_LINE];  
} Macro;


/**
 * Adds a new macro to the hash table with a specified name.
 * 
 * @param name The name of the macro.
 * @return Pointer to the newly created Macro structure.
 */
void add_macro(char *name);


/**
 * Retrieves a macro from the hash table by its name.
 * 
 * @param name The name of the macro to find.
 * @return Pointer to the Macro structure if found, NULL otherwise.
 */

Macro* find_macro(char *name);


/**
 * Prints all macros that have been added to the macro table.
 */

void print_macros(void);


/**
 * Processes the given file to replace macro invocations with their definitions and save the result to an output file.
 * 
 * @param fileName The base name of the input file (without extension).
 */

void macro(char *fileName);



#endif

