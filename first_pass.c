/**
 * File: first_pass.c
 * 
 * Description:
 *  This file contains functions for processing a single line of code during the first pass of the assembler.
 * It handles instruction processing, data allocation, and symbol table management.
 *
 * Functions:
 *  process_line_fpass(): Processes a single line of code, updating the instruction counter (IC), data counter (DC),
 *  code image, data image, and symbol table. It handles different types of instructions and labels.
 *  process_code(): Handles the processing of code lines, including the encoding of operands and updating the code image.
 *  build_extra_codeword_fpass(): Allocates and constructs additional code words for operands, handling different addressing modes.
 *
 * No direct parameters or return values, as this file operates as part of the larger assembler system.
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "code.h"
#include "utils.h"
#include "instructions.h"
#include "first_pass.h"



static bool process_code(line_info line, int i, long *ic, machine_word **code_img);


/**
 * Processes a single line of code during the first pass of assembly.
 * Handles instructions, data, and symbols, updating the symbol table and code/data images accordingly.
 * 
 * @param line The line of code to process.
 * @param IC Pointer to the instruction counter.
 * @param DC Pointer to the data counter.
 * @param code_img Pointer to the code image array.
 * @param data_img Pointer to the data image array.
 * @param symbol_table Pointer to the symbol table.
 *
 * @return Returns TRUE if successful, FALSE otherwise.
 */

bool process_line_fpass(line_info line, long *IC, long *DC, machine_word **code_img, long *data_img,
                        table *symbol_table) {
	int i = 0, j;
	char symbol[MAX_LINE_LENGTH];
	instruction instruction;
	printf("**");
	
	MOVE_TO_NOT_WHITE(line.content, i) 
	if (!line.content[i] || line.content[i] == '\n' || line.content[i] == EOF || line.content[i] == ';')
		return TRUE; 
	if (find_label(line, symbol)) {
		return FALSE;
	}


	if (symbol[0] && !is_valid_label_name(symbol)) {
		printf_line_error(line, "Illegal label name: %s", symbol);
		return FALSE;
	}

	if (symbol[0] != '\0') {
		for (; line.content[i] != ':'; i++); 
		i++;
	}

	MOVE_TO_NOT_WHITE(line.content, i) 

	if (line.content[i] == '\n') return TRUE;


	if (find_by_types(*symbol_table, symbol, 3, EXTERNAL_SYMBOL, DATA_SYMBOL, CODE_SYMBOL)) {
		printf_line_error(line, "Symbol %s is already defined.", symbol);
		return FALSE;
	}


	instruction = find_instruction_from_index(line, &i);


	if (instruction == ERROR_INST) { 
		return FALSE;
	}

	MOVE_TO_NOT_WHITE(line.content, i)
	


	if (instruction != NONE_INST) {

		
		if ((instruction == DATA_INST || instruction == STRING_INST) && symbol[0] != '\0')
			add_table_item(symbol_table, symbol, *DC, DATA_SYMBOL);{
			
		}
	
		if (instruction == STRING_INST){
			
			return process_string_instruction(line, i, data_img, DC);
		}

		else if (instruction == DATA_INST){
			
			return process_data_instruction(line, i, data_img, DC);
		}

		else if (instruction == EXTERN_INST) {
			
			MOVE_TO_NOT_WHITE(line.content, i)
			
			for (j = 0; line.content[i] && line.content[i] != '\n' && line.content[i] != '\t' 
                                          && line.content[i] != ' ' && line.content[i] != EOF; i++, j++) {
				symbol[j] = line.content[i];
			}
			symbol[j] = 0;

			if (!is_valid_label_name(symbol)) {
				printf_line_error(line, "Invalid external label name: %s", symbol);
				return TRUE;
			}
			add_table_item(symbol_table, symbol, 0, EXTERNAL_SYMBOL); 
		}

		else if (instruction == ENTRY_INST && symbol[0] != '\0') {
			printf_line_error(line, "Can't define a label to an entry instruction.");
			return FALSE;
		}

	}

	else {

		if (symbol[0] != '\0')
			add_table_item(symbol_table, symbol, *IC, CODE_SYMBOL);
                return process_code(line, i, IC, code_img);;
			

	}
        return TRUE;

}


static void build_extra_codeword_fpass(machine_word **code_img, long *ic, char *operand1, char *operand2);


/**
 * Processes a single code line during the first pass.
 * Adds the binary representation of the code to the code image array.
 * Encodes operands, handles immediate addressing, and leaves placeholders for labels.
 * 
 * @param line The line of code to process.
 * @param i Starting index for processing the line.
 * @param ic Pointer to the instruction counter.
 * @param code_img Pointer to the code image array.
 *
 * @return Returns TRUE if successful, FALSE otherwise.
 */

static bool process_code(line_info line, int i, long *ic, machine_word **code_img) {
	char operation[8]; 
	char *operands[2]; 
	opcode curr_opcode;
	funct curr_funct;
	code_word *codeword; 
	long ic_before;
	int j, operand_count;
	machine_word *word_to_write;

	MOVE_TO_NOT_WHITE(line.content, i)

	for (j = 0; line.content[i] && line.content[i] != '\t' && line.content[i] != ' ' && line.content[i] != '\n' && line.content[i] != EOF && j < 6; i++, j++) {
		operation[j] = line.content[i];
	}
	operation[j] = '\0'; 

	get_opcode_func(operation, &curr_opcode, &curr_funct);
	if (curr_opcode == NONE_OP) {
		printf_line_error(line, "Unrecognized instruction: %s.", operation);
		return FALSE; 
	}


	if (!analyze_operands(line, i, operands, &operand_count, operation))  {
		return FALSE;
	}

	if ((codeword = get_code_word(line, curr_opcode, curr_funct, operand_count, operands)) == NULL) {

		if (operands[0]) {
			free(operands[0]);
			if (operands[1]) {
				free(operands[1]);
			}
		}
		return FALSE;
	}


	ic_before = *ic;
	

	word_to_write = (machine_word *) malloc_with_check(sizeof(machine_word));
	(word_to_write->word).code = codeword;
	code_img[(*ic) - IC_INIT_VALUE] = word_to_write; 

        printf("code_img entered at ic %d : %d\n", *ic , (void*)code_img[(*ic) -IC_INIT_VALUE]);
	

	if(operand_count --){
		build_extra_codeword_fpass(code_img, ic, operands[0], operands[1]);
		free(operands[0]);
		if(operand_count){
			free(operands[1]);
		}
	}
		

	(*ic)++; 
	code_img[ic_before - IC_INIT_VALUE]->length = (*ic) - ic_before;
	

	return TRUE; 
}


/**
 * Allocates and constructs additional code words for operands in the first pass.
 * Handles different types of addressing and stores the information in the code image array.
 * 
 * @param code_img Pointer to the code image array.
 * @param ic Pointer to the instruction counter.
 * @param operand1 The first operand.
 *
 * @param operand2 The second operand (if any).
 */

static void build_extra_codeword_fpass(machine_word **code_img, long *ic, char *operand1, char *operand2) {
	addressing_type op_addr1 = get_addressing_type(operand1);
	addressing_type op_addr2 = NONE_ADDR;
	int reg1, reg2 ;
	machine_word *word_to_write;
	char *ptr;
	data_word *dw = malloc(sizeof(data_word));
	if (operand2 != NULL){
		op_addr2 = get_addressing_type(operand2);
	}
	
	
        if ( op_addr1 != NONE_ADDR ) {
                (*ic)++;
		
		if((op_addr1 == REGISTER_INDIRECT_ADDR || op_addr1 == REGISTER_ADDR)&& (op_addr2 == REGISTER_INDIRECT_ADDR || op_addr2 == REGISTER_ADDR)){
			reg1 = get_register_by_name(operand1);
			reg2 = get_register_by_name(operand2);
			dw = build_data_word_register(op_addr1, reg1, FALSE);
			dw->data |= (reg2 << 6);
			
			word_to_write = (machine_word *) malloc_with_check(sizeof(machine_word));
			word_to_write->length = 0;
			word_to_write->word.data = dw;
			code_img[(*ic) - IC_INIT_VALUE] = word_to_write;
			printf("Finished processing operands\n");
			return;
		}

		else if(op_addr1 == REGISTER_INDIRECT_ADDR || op_addr1 == REGISTER_ADDR){
			reg1 = get_register_by_name(operand1);
			dw = build_data_word_register(op_addr1, reg1, FALSE);
			word_to_write = (machine_word *) malloc_with_check(sizeof(machine_word));
			word_to_write->length = 0;
			word_to_write->word.data = dw;
			printf("find reg1 %d :\n ", reg1);
			
			code_img[(*ic) - IC_INIT_VALUE] = word_to_write;
			
		}
		if (op_addr1 == IMMEDIATE_ADDR ) {
			
			
			long value = strtol(operand1 + 1, &ptr, 10);
			dw = build_data_word_immediate(op_addr1, value, FALSE);
			word_to_write = (machine_word *) malloc_with_check(sizeof(machine_word));
			word_to_write->length = 0; 
			word_to_write->word.data = dw;

			code_img[(*ic) - IC_INIT_VALUE] = word_to_write;
                        printf("build_extra_codeword code_img entered for immediate address at ic %d : %d\n", *ic , (void*)code_img[(*ic) -IC_INIT_VALUE]);
		}
	}
	if (operand2 == NONE_ADDR) return;
	if ( op_addr2 != NONE_ADDR) {
		
                (*ic)++;
		if(op_addr2 == REGISTER_INDIRECT_ADDR || op_addr2 == REGISTER_ADDR){
			reg2 = get_register_by_name(operand2);
			dw = build_data_word_register(op_addr2, reg2, FALSE);
			word_to_write = (machine_word *) malloc_with_check(sizeof(machine_word));
			word_to_write->length = 0;
			word_to_write->word.data = dw;
			printf("find reg2 %d :\n ", reg2);
			
			code_img[(*ic) - IC_INIT_VALUE] = word_to_write;
		}
		if ( op_addr2 == DIRECT_ADDR) return;
		
		if (op_addr2 == IMMEDIATE_ADDR) {
			
			long value = strtol(operand2 + 1, &ptr, 10);
			dw = build_data_word_immediate(op_addr2, value, FALSE);
			word_to_write = (machine_word *) malloc_with_check(sizeof(machine_word));
			word_to_write->length = 0; 
			word_to_write->word.data = dw;

			code_img[(*ic) - IC_INIT_VALUE] = word_to_write;
                        printf("build_extra_codeword code_img entered for immediate address at ic %d : %d\n", *ic , (void*)code_img[(*ic) -IC_INIT_VALUE]);
		}
	}
}

