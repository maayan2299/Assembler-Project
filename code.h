/**
 * code.h
 * Provides functions for handling machine code operations, including encoding instructions and data words.
 * 
 * This header file includes function declarations for:
 * - Retrieving opcode and function codes.
 * - Determining addressing types.
 * - Building code and data words from operands.
 * - Analyzing and freeing operands.
 */

#ifndef _CODE_H
#define _CODE_H
#include "table.h"
#include "globals.h"



/** 
 * Finds the opcode and funct associated with a given assembly command.
 *
 * @param cmd - The assembly command (e.g., "mov", "cmp").
 * @param opcode_out - Pointer to store the found opcode.
 * @param funct_out - Pointer to store the found function code.
 */

void get_opcode_func(char* cmd, opcode *opcode_out, funct *funct_out);


/** 
 * Determines the addressing type of a given operand in assembly code.
 *
 * @param operand - The operand to analyze (e.g., register, immediate value).
 * 
 * @return addressing_type - The type of addressing used by the operand.
 */

addressing_type get_addressing_type(char *operand);


/** 
 * Constructs a code word for the given opcode and operands.
 *
 * @param line - The current line of assembly code.
 * @param curr_opcode - The opcode of the instruction.
 * @param curr_funct - The function code associated with the instruction.
 * @param op_count - The number of operands provided for the instruction.
 * @param operands - The array of operand strings.
 * 
 * @return code_word* - Pointer to the constructed code word, or NULL on failure.
 */

code_word *get_code_word(line_info line, opcode curr_opcode, funct curr_funct, int op_count, char *operands[2]);


/**
 * Gets the register number associated with a register name string.
 *
 * @param name - The name of the register (e.g., "r0", "r1").
 * 
 * @return reg - The corresponding register number, or NONE_REG if invalid.
 */

reg get_register_by_name(char *name);


/**
 * Builds a data word for an immediate value.
 *
 * @param value - The immediate value to be encoded in the data word.
 * 
 * @return data_word* - Pointer to the created data word.
 */

data_word *build_data_word_immediate(long value);


/**
 * Builds a data word for a register.
 *
 * @param reg - The register number to be encoded in the data word.
 * 
 * @return data_word* - Pointer to the created data word.
 */

data_word *build_data_word_register(int reg);


/**
 * Builds a data word for a direct address (e.g., label or symbol).
 *
 * @param value - The address or value associated with the symbol.
 * @param is_extern_symbol - Boolean flag indicating if the symbol is external.
 * 
 * @return data_word* - Pointer to the created data word.
 */

data_word *build_data_word_direct(long value, bool is_extern_symbol) ;



/**
 * Analyzes the operands for a given line of assembly code.
 *
 * @param line - The current line of assembly code being processed.
 * @param i - The index in the line's content where operand analysis begins.
 * @param destination - Array to store the operands found in the line.
 * @param operand_count - Pointer to store the count of operands found.
 * @param c - Character pointer for further use in operand processing.
 * 
 * @return bool - TRUE if operand analysis is successful, FALSE otherwise.
 */

bool analyze_operands(line_info line, int i, char **destination, int *operand_count, char *command);

/**
 * Frees the memory allocated for operand strings.
 *
 * @param operand1 - Pointer to the first operand string.
 * @param operand2 - Pointer to the second operand string.
 */

void free_operands(char *operand1, char *operand2);

#endif

