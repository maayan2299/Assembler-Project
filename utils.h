#ifndef _UTILS_H
#define _UTILS_H

#include "globals.h"

/**
 * @file utils.h
 * @brief Utility functions for string manipulation, memory management, and error handling.
 */

/**
 * @macro MOVE_TO_NOT_WHITE
 * @brief Macro to skip white spaces and tabs in a string.
 * 
 * @param string The string to process.
 * @param index The current index in the string. This will be updated to the first non-whitespace character.
 */
#define MOVE_TO_NOT_WHITE(string, index) \
        for (;string[(index)] && (string[(index)] == '\t' || string[(index)] == ' '); (++(index)))\
        ;

/**
 * @brief Concatenates two strings and returns the result.
 *
 * @param s0 First string.
 * @param s1 Second string.
 *
 * @return Pointer to the newly allocated concatenated string.
 */
char *strallocat(char *s0, char* s1);

/**
 * @brief Extracts a label from a line of code.
 *
 * @param line The line of code containing the label.
 * @param symbol_dest Buffer to store the extracted label.
 *
 * @return TRUE if a label was found, FALSE otherwise.
 */
bool find_label(line_info line, char *symbol_dest);

/**
 * @brief Finds the instruction corresponding to a given name.
 *
 * @param name The name of the instruction.
 *
 * @return The instruction type if found, NONE_INST otherwise.
 */
instruction find_instruction_by_name(char *name);

/**
 * @brief Checks if a string represents an integer.
 *
 * @param string The string to check.
 *
 * @return TRUE if the string represents an integer, FALSE otherwise.
 */
bool is_int(char* string);

/**
 * @brief Allocates memory and checks for allocation failure.
 *
 * @param size The size of memory to allocate.
 *
 * @return Pointer to the allocated memory.
 */
void *malloc_with_check(long size);

/**
 * @brief Validates a label name based on specific rules.
 *
 * @param name The label name to validate.
 *
 * @return TRUE if the label name is valid, FALSE otherwise.
 */
bool is_valid_label_name(char* name);

/**
 * @brief Checks if a string contains only alphanumeric characters.
 *
 * @param string The string to check.
 *
 * @return TRUE if the string is alphanumeric, FALSE 

