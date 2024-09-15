/**
 * File: utils.c
 * Description:
 * Contains utility functions for handling strings, labels, and memory management.
 *
 * Functions:
 * - strallocat: Concatenates two strings and returns the result.
 * - find_label: Extracts a label from a line of code.
 * - find_instruction_by_name: Finds the instruction corresponding to a given name.
 * - is_int: Checks if a string represents an integer.
 * - malloc_with_check: Allocates memory and checks for allocation failure.
 * - is_label: Determines if a line contains a label.
 * - is_valid_label_name: Validates a label name based on specific rules.
 * - is_alphanumeric_str: Checks if a string contains only alphanumeric characters.
 * - is_reserved_word: Determines if a name is a reserved word.
 * - printf_line_error: Prints an error message with file and line information.
 * - free_code_image: Frees allocated memory for a code image.
 * No direct parameters or return values, as this file operates as part of the larger assembler system.
 */



#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "utils.h"
#include "code.h" 

#define ERR_OUTPUT_FILE stderr


/**
 * Concatenates two strings and returns the result.
 *
 * @param s0 First string.
 * @param s1 Second string.
 *
 * @return Pointer to the newly allocated concatenated string.
 */

char *strallocat(char *s0, char* s1) {
	char *str = (char *)malloc_with_check(strlen(s0) + strlen(s1) + 1);
	strcpy(str, s0);
	strcat(str, s1);
	if (s0 == NULL || s1 == NULL) {
		fprintf(stderr, "Error: NULL pointer passed to strallocat.\n");
		 exit(EXIT_FAILURE);
	}
	return str;
}


/**
 * Extracts a label from a line of code.
 *
 * @param line The line of code containing the label.
 * @param symbol_dest Buffer to store the extracted label.
 *
 * @return TRUE if a label was found, FALSE otherwise.
 */

bool find_label(line_info line, char *symbol_dest) {
	int j, i;
	i = j = 0;


	MOVE_TO_NOT_WHITE(line.content, i)

	for (; line.content[i] && line.content[i] != ':' && line.content[i] != EOF && i <= MAX_LINE_LENGTH; i++, j++) {
		symbol_dest[j] = line.content[i];
	}
	symbol_dest[j] = '\0'; 

	if (line.content[i] == ':') {
		if (!is_valid_label_name(symbol_dest)) {
			printf_line_error(line,
			                  "Invalid label name - cannot be longer than 32 chars, may only start with letter be alphanumeric.");
			symbol_dest[0] = '\0';
			return TRUE; 
		}
		return FALSE;
	}
	symbol_dest[0] = '\0';
	return FALSE; 
}

struct instruction_lookup_item {
	char *name;
	instruction value;
};

static struct instruction_lookup_item
		instructions_lookup_table[] = {
		{"string", STRING_INST},
		{"data",   DATA_INST},
		{"entry",  ENTRY_INST},
		{"extern", EXTERN_INST},
		{NULL, NONE_INST}
};


/**
 * Finds the instruction corresponding to a given name.
 *
 * @param name The name of the instruction.
 *
 * @return The instruction type if found, NONE_INST otherwise.
 */

instruction find_instruction_by_name(char *name) {
	struct instruction_lookup_item *curr_item;
	for (curr_item = instructions_lookup_table; curr_item->name != NULL; curr_item++) {
		if (strcmp(curr_item->name, name) == 0) {
			return curr_item->value;
		}
	}
	return NONE_INST;
}


/**
 * Checks if a string represents an integer.
 *
 * @param string The string to check.
 *
 * @return TRUE if the string represents an integer, FALSE otherwise.
 */

bool is_int(char *string) {
	int i = 0;
	if (string[0] == '-' || string[0] == '+') string++; 
	for (; string[i]; i++) { 
		if (!isdigit(string[i])) {
			return FALSE;
		}
	}
	return i > 0; 
}


/**
 * Allocates memory and checks for allocation failure.
 *
 * @param size The size of memory to allocate.
 *
 * @return Pointer to the allocated memory.
 */

void *malloc_with_check(long size) {
	void *ptr = malloc(size);
	if (ptr == NULL) {
		printf("Error: Fatal: Memory allocation failed.");
		exit(1);
	}
	return ptr;
}


/**
 * Determines if a line contains a label.
 *
 * @param line The line to check.
 * @param label Buffer to store the label if found.
 *
 * @return TRUE if a label is found, FALSE otherwise.
 */

bool is_label(const char *line, char *label) {
	int i = 0;
	while (line[i] && line[i] != ':' && !isspace(line[i])) {
		label[i] = line[i];
		i++;
	}
	if (line[i] == ':') {
		label[i] = '\0';
		return TRUE;
	}
	return FALSE;
}


/**
 * Validates a label name based on specific rules.
 *
 * @param name The label name to validate.

 * @return TRUE if the label name is valid, FALSE otherwise.
 */

bool is_valid_label_name(char *name) {
	
	return name[0] && strlen(name) <= 31 && isalpha(name[0]) && is_alphanumeric_str(name + 1) &&
	       !is_reserved_word(name);
}


/**
 * Checks if a string contains only alphanumeric characters.
 *
 * @param string The string to check.

 * @return TRUE if the string is alphanumeric, FALSE otherwise.
 */

bool is_alphanumeric_str(char *string) {
	int i;
	
	for (i = 0; string[i]; i++) {
		if (!isalpha(string[i]) && !isdigit(string[i])) return FALSE;
	}
	return TRUE;
}


/**
 * Determines if a name is a reserved word.
 *
 * @param name The name to check.

 * @return TRUE if the name is a reserved word, FALSE otherwise.
 */

bool is_reserved_word(char *name) {
	int fun, opc;

	get_opcode_func(name, &opc, (funct *) &fun);
	if (opc != NONE_OP || get_register_by_name(name) != NONE_REG || find_instruction_by_name(name) != NONE_INST || is_register_indirect_addr(name)) return TRUE;

	return FALSE;
}


/**
 * Prints an error message with file and line information.
 *
 * @param line The line info for error reporting.
 * @param message The error message format string.
 * @param ... Additional arguments for the format string.
 *
 * @return The number of characters printed.
 */

int printf_line_error(line_info line, char *message, ...) { 
	int result;
	va_list args; 
	fprintf(ERR_OUTPUT_FILE,"Error In %s:%ld: ", line.file_name, line.line_number);

	va_start(args, message);
	result = vfprintf(ERR_OUTPUT_FILE, message, args);
	va_end(args);

	fprintf(ERR_OUTPUT_FILE, "\n");
	return result;
}


/**
 * @brief Frees allocated memory for a code image.
 *
 * @param code_image Array of pointers to machine_word structures.
 * @param fic Number of elements in the code_image array.
 */

void free_code_image(machine_word **code_image, long fic) {
	long i;
	
	for (i = 0; i < fic; i++) {
		machine_word *curr_word = code_image[i];
		if (curr_word != NULL) {

			if (curr_word->length > 0) {
				free(curr_word->word.code);
			} else {
				free(curr_word->word.data);
			}

			free(curr_word);
			code_image[i] = NULL;
		}
	}
}

