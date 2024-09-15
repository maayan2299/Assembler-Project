/**
 * File: code.c
 * 
 * Description:
 *  This file contains functions for analyzing and validating operands
 *
 * Functions:
 *  - analyze_operands(): Analyzes the operands in a line of code and extracts them.
 *  - get_opcode_func(): Retrieves the opcode and function for a given command.
 *  - get_addressing_type(): Determines the addressing type of an operand.
 *  - validate_operand_by_opcode(): Validates the addressing types of operands based on the current opcode.
 *  - get_code_word(): Builds a code word representing the operation and its operands.
 *  - build_data_word_*(): Constructs data words for various operand types (immediate, register, direct).
 *
 * No direct parameters or return values, as this file operates as part of the larger assembler system.
 */


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include "code.h"
#include "utils.h"



static bool validate_op_addr(line_info line, addressing_type op1_addressing, addressing_type op2_addressing,
		int op1_valid_addr_count, int op2_valid_addr_count,...);


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

bool analyze_operands(line_info line, int i, char **destination, int *operand_count, char *c) {
	int j;
        char *current_operand;
	*operand_count = 0;
	destination[0] = destination[1] = NULL;
	MOVE_TO_NOT_WHITE(line.content, i) /* Skip spaces at the beginning of the line*/

	if (line.content[i] == ',') /*Check if there is an unnecessary comma at the beginning of the line*/ {
		printf_line_error(line, "Unexpected comma after command.");
		return FALSE; 
	}

	
	for (*operand_count = 0; line.content[i] != EOF && line.content[i] != '\n' && line.content[i];) {
		if (*operand_count == 2)  {
			printf_line_error(line, "Too many operands for operation (got >%d)", *operand_count);
			free(destination[0]);
			free(destination[1]);
			return FALSE; 
		}

		/*Allocate memory for the current operand*/
		current_operand = malloc_with_check(MAX_LINE_LENGTH);

		for (j = 0; line.content[i] && line.content[i] != '\t' && line.content[i] != ' ' && line.content[i] != '\n' && line.content[i] != EOF &&
		            line.content[i] != ','; i++, j++) {
			current_operand[j] = line.content[i];
		}
		current_operand[j] = '\0';
                destination[*operand_count] =  current_operand;
                printf("Operand: %s count: %d, i=%d\n",current_operand, *operand_count, i);
		(*operand_count)++; 
		MOVE_TO_NOT_WHITE(line.content, i)

		if (line.content[i] == '\n' || line.content[i] == EOF || !line.content[i]) break;
		else if (line.content[i] != ',') {
			
			printf_line_error(line, "Expecting ',' between operands");

			free(destination[0]);
			if (*operand_count > 1) {
				free(destination[1]);
			}
			return FALSE;
		}
		i++;
		MOVE_TO_NOT_WHITE(line.content, i)
		
		if (line.content[i] == '\n' || line.content[i] == EOF || !line.content[i])
			printf_line_error(line, "Missing operand after comma.");
		else if (line.content[i] == ',') printf_line_error(line, "Multiple consecutive commas.");
		else continue; 
		{ 
			free(destination[0]);
			if (*operand_count > 1) {
				free(destination[1]);
			}
			return FALSE;
		}
	}
	return TRUE;
}

/* structure to keep a command name, operation code and its function*/
struct cmd_lookup_element {
	char *cmd;
	opcode op;
	funct fun;
};

/*Lookup table for various commands and action codes*/
static struct cmd_lookup_element lookup_table[] = {
		{"mov", MOV_OP, NONE_FUNCT},
		{"cmp",CMP_OP, NONE_FUNCT},
		{"add",ADD_OP, ADD_FUNCT},
		{"sub",SUB_OP, SUB_FUNCT},
		{"lea",LEA_OP, NONE_FUNCT},
		{"clr",CLR_OP, CLR_FUNCT},
		{"not",NOT_OP, NOT_FUNCT},
		{"inc",INC_OP, INC_FUNCT},
		{"dec",DEC_OP, DEC_FUNCT},
		{"jmp",JMP_OP, JMP_FUNCT},
		{"bne",BNE_OP, BNE_FUNCT},
		{"jsr",JSR_OP, JSR_FUNCT},
		{"red",RED_OP, NONE_FUNCT},
		{"prn",PRN_OP, NONE_FUNCT},
		{"rts",RTS_OP, NONE_FUNCT},
		{"stop",STOP_OP, NONE_FUNCT},
		{NULL, NONE_OP, NONE_FUNCT}
};


/** 
 * Finds the opcode and funct associated with a given assembly command.
 *
 * @param cmd - The assembly command (e.g., "mov", "cmp").
 * @param opcode_out - Pointer to store the found opcode.
 * @param funct_out - Pointer to store the found function code.
 */

void get_opcode_func(char *cmd, opcode *opcode_out, funct *funct_out) {
	struct cmd_lookup_element *e;
	*opcode_out = NONE_OP;
	*funct_out = NONE_FUNCT;
	
	for (e = lookup_table; e->cmd != NULL; e++) {
		if (strcmp(e->cmd, cmd) == 0) {
			*opcode_out = e->op;
			*funct_out = e->fun;
			return;
		}
	}
}


/** 
 * Determines the addressing type of a given operand in assembly code.
 *
 * @param operand - The operand to analyze (e.g., register, immediate value).
 * 
 * @return addressing_type - The type of addressing used by the operand.
 */

addressing_type get_addressing_type(char *operand) {
	if (operand[0] == '\0') return NONE_ADDR;
	if (operand[0] == '#' && is_int(operand + 1)) return IMMEDIATE_ADDR;
	else if (operand[0] == '*' && operand[1] == 'r' && operand[2] >= '0' && operand[2] <= '7' && operand[3] == '\0') return REGISTER_INDIRECT_ADDR;
	else if (operand[0] == 'r' && operand[1] >= '0' && operand[1] <= '7' && operand[2] == '\0') return REGISTER_ADDR;
	else if (is_valid_label_name(operand)) return DIRECT_ADDR;
	else return NONE_ADDR;
}


/**
 * Validates operands based on the opcode and the number of operands.
 *
 * @param line - The current line of assembly code.
 * @param first_addressing - The addressing type of the first operand.
 * @param second_addressing - The addressing type of the second operand.
 * @param curr_opcode - The opcode of the current instruction.
 * @param op_count - The number of operands for the instruction.
 * 
 * @return bool - TRUE if the operands are valid for the opcode, FALSE otherwise.
 */

bool validate_operand_by_opcode(line_info line, addressing_type first_addressing,
                                addressing_type second_addressing, opcode curr_opcode, int op_count) {
	if (curr_opcode >= MOV_OP && curr_opcode <= LEA_OP) {
		
		if (op_count != 2) {
			printf_line_error(line, "Operation requires 2 operands (got %d)", op_count);
			return FALSE;
		}

		if (curr_opcode == CMP_OP) {
			return validate_op_addr(line, first_addressing, second_addressing, 4, 4,
						    IMMEDIATE_ADDR, DIRECT_ADDR,REGISTER_INDIRECT_ADDR,REGISTER_ADDR,
			                            IMMEDIATE_ADDR, DIRECT_ADDR, REGISTER_ADDR, REGISTER_INDIRECT_ADDR);
		} else if (curr_opcode == ADD_OP || curr_opcode == MOV_OP) { 
			if (first_addressing == NONE_ADDR || second_addressing == NONE_ADDR) {
				printf_line_error(line, "Invalid operand addressing type.");
				return FALSE;
			}
			return validate_op_addr(line, first_addressing, second_addressing, 4, 3,
						    IMMEDIATE_ADDR, DIRECT_ADDR,REGISTER_INDIRECT_ADDR,REGISTER_ADDR,
			                            DIRECT_ADDR, REGISTER_ADDR, REGISTER_INDIRECT_ADDR);
		} else if (curr_opcode == LEA_OP) {

			return validate_op_addr(line, first_addressing, second_addressing, 2, 3, 
						    DIRECT_ADDR, REGISTER_INDIRECT_ADDR,
			                            DIRECT_ADDR, REGISTER_ADDR, REGISTER_INDIRECT_ADDR);
		} else if ( curr_opcode == SUB_OP) {
			return validate_op_addr(line, first_addressing, second_addressing, 4, 3,
					IMMEDIATE_ADDR, DIRECT_ADDR,REGISTER_INDIRECT_ADDR,REGISTER_ADDR,	
					DIRECT_ADDR, REGISTER_INDIRECT_ADDR, REGISTER_ADDR);
		}
		
	} else if (curr_opcode >= CLR_OP && curr_opcode <= JSR_OP) {

		if (op_count != 1) {
			if (op_count < 1) printf_line_error(line, "Operation requires 1 operand (got %d)", op_count);
			return FALSE;
		}

		if (curr_opcode == RED_OP || curr_opcode == CLR_OP || curr_opcode == NOT_OP || curr_opcode == INC_OP || curr_opcode == DEC_OP) { 
			return validate_op_addr(line, first_addressing, NONE_ADDR, 3, 0, DIRECT_ADDR, REGISTER_ADDR, REGISTER_INDIRECT_ADDR);
		} else if (curr_opcode == JMP_OP || curr_opcode == BNE_OP|| curr_opcode == JSR_OP) {
			return validate_op_addr(line, first_addressing, NONE_ADDR, 2, 0, DIRECT_ADDR, REGISTER_INDIRECT_ADDR);
		} else {
			return validate_op_addr(line, first_addressing, NONE_ADDR, 4, 0, IMMEDIATE_ADDR, DIRECT_ADDR, REGISTER_ADDR, REGISTER_INDIRECT_ADDR);
		}
	} else if (curr_opcode == STOP_OP || curr_opcode == RTS_OP) {
		
		if (op_count > 0) {
			printf_line_error(line, "Operation requires no operands (got %d)", op_count);
			return FALSE;
		}
	}
	return TRUE;
}


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

code_word *get_code_word(line_info line, opcode curr_opcode, funct curr_funct, int op_count, char *operands[2]) {
	code_word *codeword;
	addressing_type first_addressing = op_count >= 1 ? get_addressing_type(operands[0]) : NONE_ADDR;
	addressing_type second_addressing = op_count == 2 ? get_addressing_type(operands[1]) : NONE_ADDR;
	if (!validate_operand_by_opcode(line, first_addressing, second_addressing, curr_opcode, op_count)) {
		return NULL;
	}
	

	codeword = (code_word *) malloc_with_check(sizeof(code_word));
	codeword->opcode = curr_opcode;
	codeword->funct = curr_funct; 
	codeword->ARE = 4; 
	codeword->src_addressing = first_addressing;
	codeword->dest_addressing = second_addressing;

	codeword->src_register = first_addressing == REGISTER_ADDR ? get_register_by_name(operands[0]) : 0;
	codeword->dest_register = second_addressing == REGISTER_ADDR ? get_register_by_name(operands[1]) : 0;

	return codeword;
error: 
	free(codeword);
	return NULL;
}


/**
 * Validates operand addressing modes for specific instructions.
 *
 * @param line - The current line of assembly code.
 * @param op1_addressing - Addressing mode of the first operand.
 * @param op2_addressing - Addressing mode of the second operand.
 * @param op1_valid_addr_count - Number of valid addressing modes for the first operand.
 * @param op2_valid_addr_count - Number of valid addressing modes for the second operand.
 * 
 * @return bool - TRUE if addressing modes are valid, FALSE otherwise.
 */

static bool validate_op_addr(line_info line, addressing_type op1_addressing, addressing_type op2_addressing, int op1_valid_addr_count,
                      int op2_valid_addr_count, ...) {
	int i;
	bool is_valid;
	va_list list;

	addressing_type op1_valids[4], op2_valids[4];
	memset(op1_valids, NONE_ADDR, sizeof(op1_valids));
	memset(op2_valids, NONE_ADDR, sizeof(op2_valids));

	va_start(list, op2_valid_addr_count);
	
	for (i = 0; i < op1_valid_addr_count && i <= 3 ;i++)
		op1_valids[i] = va_arg(list, int);
	for (; op1_valid_addr_count > 5; va_arg(list,
	                                        int), op1_valid_addr_count--); 
	for (i = 0; i < op2_valid_addr_count && i <= 3 ;i++)
		op2_valids[i] = va_arg(list, int);

	va_end(list);  
	is_valid = op1_valid_addr_count == 0 && op1_addressing == NONE_ADDR;
	for (i = 0; i < op1_valid_addr_count && !is_valid; i++) {
		if (op1_valids[i] == op1_addressing) {
			is_valid = TRUE;
		}
	}
	if (!is_valid) {
		printf_line_error(line, "Invalid addressing mode for first operand.");
		return FALSE;
	}
	/* Same */
	is_valid = op2_valid_addr_count == 0 && op2_addressing == NONE_ADDR;
	for (i = 0; i < op2_valid_addr_count && !is_valid; i++) {
		if (op2_valids[i] == op2_addressing) {
			is_valid = TRUE;
		}
	}
	if (!is_valid) {
		printf_line_error(line, "Invalid addressing mode for second operand.");
		return FALSE;
	}
	return TRUE;
}


/**
 * Gets the register number associated with a register name string.
 *
 * @param name - The name of the register (e.g., "r0", "r1").
 * 
 * @return reg - The corresponding register number, or NONE_REG if invalid.
 */

reg get_register_by_name(char *name) {
	if (name[0] == 'r' && isdigit(name[1]) && name[2] == '\0') {
		int digit = name[1] - '0';
		if (digit >= 0 && digit <= 7) return digit;
	}
	return NONE_REG; 
}


/**
 * Checks if an operand is a register indirect address.
 *
 * @param name - The name of the operand (e.g., "*r3").
 * 
 * @return bool - TRUE if the operand is a register indirect address, FALSE otherwise.
 */

bool is_register_indirect_addr (char *name) {
	if (name[0] == '*') {
		 char *reg_name = name + 1;
		return (get_register_by_name(reg_name) != NONE_REG);
	}
	return FALSE;
}


/**
 * Builds a data word for an immediate value.
 *
 * @param value - The immediate value to be encoded in the data word.
 * 
 * @return data_word* - Pointer to the created data word.
 */

data_word *build_data_word_immediate(long value) {
	data_word *dw = malloc_with_check(sizeof(data_word));
	dw->ARE = 4;
	dw->data = value & 0xFFF;
	return dw;
}


/**
 * Builds a data word for a register.
 *
 * @param reg - The register number to be encoded in the data word.
 * 
 * @return data_word* - Pointer to the created data word.
 */

data_word *build_data_word_register(int reg) {
	data_word *dw = malloc_with_check(sizeof(data_word));
	dw->ARE = 4;
	dw->data = reg & 0xF;
	return dw;
}



/**
 * Builds a data word for a direct address (e.g., label or symbol).
 *
 * @param value - The address or value associated with the symbol.
 * @param is_extern_symbol - Boolean flag indicating if the symbol is external.
 * 
 * @return data_word* - Pointer to the created data word.
 */

data_word *build_data_word_direct(long value, bool is_extern_symbol) {
	data_word *dw = malloc_with_check(sizeof(data_word));
	dw->ARE = is_extern_symbol ? 1 : 4;
	dw->data = value & 0xFFF;
	return dw;
}


/**
 * Frees the memory allocated for operand strings.
 *
 * @param operand1 - Pointer to the first operand string.
 * @param operand2 - Pointer to the second operand string.
 */

void free_operands(char *operand1, char *operand2) {

	if (operand1) free(operand1);
	if (operand2) free(operand2);
}



