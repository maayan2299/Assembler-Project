/**
 * File: second_pass.c
 * 
 * Description:
 *  This file contains functions for processing the second pass of assembly. It handles the final assembly of code lines,
 *  processes operands, and manages the symbol table during the second pass.
 *
 * Functions:
 *  - process_spass_operand(): Processes operands during the second pass, updating instruction counters and code image.
 *  - process_direct_operand(): Processes direct operands, handling symbol resolution and adding entries to the symbol table.
 *  - process_line_spass(): Processes a single line of code during the second pass, handling .entry instructions and symbol addition.
 *  - add_symbols_to_code(): Adds symbols to the code image, updating the instruction counters and resolving symbols.
 *  
 * No direct parameters or return values for the whole file, as it operates as part of the larger assembler system.
*/


#include <stdio.h>
#include <stdlib.h>
#include "second_pass.h"
#include "code.h"
#include "utils.h"
#include "string.h"



int process_spass_operand(line_info line, long *curr_ic, long *ic, char *operand1, char *operand2, machine_word **code_img,
                          table *symbol_table);


bool process_direct_operand(line_info line, long curr_ic, char *operand, machine_word **code_img, table *symbol_table);


/**
 * Processes a single line of code during the second pass of assembly.
 * Handles .entry instructions and symbol addition to the code image.
 * 
 * @param line The line of code to process.
 * @param ic Pointer to the instruction counter.
 * @param code_img Pointer to the code image array.
 * @param symbol_table Pointer to the symbol table.
 *
 * @return Returns TRUE if successful, FALSE otherwise.
 */

bool process_line_spass(line_info line, long *ic, machine_word **code_img, table *symbol_table) {
	char *indexOfColon;
	char *token;
	long i = 0;
	printf("process_line_spass - code image %d :\n", code_img[(*ic) - IC_INIT_VALUE]);


	/* Move the pointer to the beginning of the line content */
	MOVE_TO_NOT_WHITE(line.content, i)

	if (line.content[i] == ';' || line.content[i] == '\n') return TRUE;
	indexOfColon = strchr(line.content, ':');

	if (indexOfColon != NULL) {
		i = indexOfColon - line.content;
		i++;
	}

	MOVE_TO_NOT_WHITE(line.content, i)
	
	/* check if there is a tab */
	if (line.content[i] == '.') {
		if (strncmp(".entry", line.content, 6) == 0) {
			i += 6;
			MOVE_TO_NOT_WHITE(line.content, i)
			token = strtok(line.content + i, " \n\t");
			if (token == NULL) {
				printf_line_error(line, "You have to specify a label name for .entry instruction.");
				return FALSE;
			}
			if (find_by_types(*symbol_table, token, 1, ENTRY_SYMBOL) == NULL) {
				table_entry *entry;
				token = strtok(line.content + i, "\n");
				if (token[0] == '&') token++;

				if ((entry = find_by_types(*symbol_table, token, 2, DATA_SYMBOL, CODE_SYMBOL)) == NULL) {
				
					if ((entry = find_by_types(*symbol_table, token, 1, EXTERNAL_SYMBOL)) != NULL) {
						printf_line_error(line, "The symbol %s can be either external or entry, but not both.",
						                  entry->key);
						return FALSE;
					}

					printf_line_error(line, "The symbol %s for .entry is undefined.", token);
					return FALSE;
				}
				add_table_item(symbol_table, token, entry->value, ENTRY_SYMBOL);
			}
		}
		printf("is_success \n");
		return TRUE;
	}
	return add_symbols_to_code(line, ic, code_img, symbol_table);
}


/**
 * Adds symbols to the code image during the second pass of assembly.
 * Updates instruction counters and processes operands.
 * 
 * @param line The line of code being processed.
 * @param ic Pointer to the instruction counter.
 * @param code_img Pointer to the code image array.
 * @param symbol_table Pointer to the symbol table.
 *
 * @return Returns TRUE if successful, FALSE otherwise.
 */

bool add_symbols_to_code(line_info line, long *ic, machine_word **code_img, table *symbol_table) {
	char temp[80];
	char *operands[2];
	int i = 0, operand_count, t;
	bool isvalid = TRUE;
	long curr_ic = *ic; 
	int length = code_img[(*ic) - IC_INIT_VALUE]->length;

	printf("add_symbols_to_code code_img %d length %d\n",  *code_img[(*ic) - IC_INIT_VALUE], length);
        if (length > 0) {
		
		MOVE_TO_NOT_WHITE(line.content, i)
		find_label(line, temp);
		if (temp[0] != '\0') { 

			for (; line.content[i] && line.content[i] != '\n' && line.content[i] != EOF && line.content[i] != ' ' &&
			       line.content[i] != '\t'; i++);
			i++;
                        printf("Label: %s i=%d\n, ",temp, i);
		}

		MOVE_TO_NOT_WHITE(line.content, i)

                char command[80];
		for (t=0; line.content[i] && line.content[i] != ' ' && line.content[i] != '\t' && line.content[i] != '\n' &&
		       line.content[i] != EOF; i++, t++){
                              command[t]=line.content[i] ;
                }
                command[t] = '\0';
		MOVE_TO_NOT_WHITE(line.content, i)
                printf("command %s, i=%d, ",command , i);
		
		analyze_operands(line, i, operands, &operand_count, NULL);
		
		printf("process_spass_operand for  code img at %d to: %d \n" , ic, code_img[(*ic) - IC_INIT_VALUE]);
		if(operand_count > 0){
                        char *operand2  = NULL;
                        if (operand_count >1){ operand2 = operands[1];}
			isvalid = process_spass_operand(line, ic, ic, operands[0], operand2 , code_img, symbol_table);
			free(operands[0]);
                	if(operand2 != NULL){
				free(operands[1]);
			}
		}
		if (!isvalid) return FALSE;
			
	}
        
	(*ic) = ++(*ic);
	return TRUE;
}


/**
 * Processes operands during the second pass of assembly.
 * Updates the instruction counter and code image based on the operand types and values.
 * 
 * @param line The line of code being processed.
 * @param curr_ic Pointer to the current instruction counter.
 * @param ic Pointer to the instruction counter.
 * @param operand1 The first operand.
 * @param operand2 The second operand (if any).
 * @param code_img Pointer to the code image array.
 * @param symbol_table Pointer to the symbol table.
 *
 * @return Returns TRUE if successful, FALSE otherwise.
 */

int process_spass_operand(line_info line, long *curr_ic, long *ic, char *operand1, char *operand2, machine_word **code_img,
                          table *symbol_table) {


	addressing_type addr1 = get_addressing_type(operand1);
	addressing_type addr2 = NONE_ADDR;

        printf("process_spass_operand  1 ic %d addr1 %d \n", *curr_ic, addr1);

         printf("process_spass_operand op2 = %s\n", operand2);
	
        if(operand2 != NULL){
		addr2 = get_addressing_type(operand2);
                
	} 
        
	if (addr1 == REGISTER_INDIRECT_ADDR || addr1 == REGISTER_ADDR){
            (*curr_ic)++;
            if (addr2 != NONE_ADDR && (addr2 == REGISTER_INDIRECT_ADDR || addr2 == REGISTER_ADDR)) { 
                 printf("process_spass_operand 2 (2 registers) ic %d addr2 %d \n", *ic, addr2);

		  printf("process_spass_operand ic at end = %d\n", *curr_ic); 
                 return TRUE;
            } 
           
        }
	printf("process_spass_operand addr2 = %d\n", addr2);
        if (addr1 == IMMEDIATE_ADDR ) (*curr_ic)++;
        if (addr1 == DIRECT_ADDR  ) {
              (*curr_ic)++;
             process_direct_operand(line, *curr_ic, operand1, code_img, symbol_table);
        }

        if (operand2 == NULL){ printf("process_spass_operand ic at end = %d\n", *curr_ic); return TRUE;}

       if (addr2 == REGISTER_INDIRECT_ADDR || addr2 == REGISTER_ADDR || addr2 == IMMEDIATE_ADDR){
            (*curr_ic)++;
           
        }

        if (addr2 == DIRECT_ADDR  ) {
              (*curr_ic)++;
             process_direct_operand(line, *curr_ic ,  operand2, code_img, symbol_table);
        }
        printf("process_spass_operand ic at end = %d\n", *curr_ic);
        return TRUE;
}


/**
 * Processes direct operands during the second pass of assembly.
 * Handles symbol resolution for direct operands and updates the code image.
 * 
 * @param line The line of code being processed.
 * @param curr_ic The current instruction counter.
 * @param operand The direct operand to process.
 * @param code_img Pointer to the code image array.
 * @param symbol_table Pointer to the symbol table.
 *
 * @return Returns TRUE if successful, FALSE otherwise.
 */

bool process_direct_operand(line_info line, long curr_ic, char *operand, machine_word **code_img, table *symbol_table) {        
        printf("process_direct_operand started for ic %d\n", curr_ic);

	machine_word *word_to_write;

	if (TRUE  ) {    
                       
		long data_to_add;
		table_entry *entry = find_by_types(*symbol_table, operand, 3, DATA_SYMBOL, CODE_SYMBOL, EXTERNAL_SYMBOL);
		if (entry == NULL) {
			printf_line_error(line, "The symbol %s not found", operand);
			return FALSE;
		} 
                printf("process_direct_operand found entry\n");

		data_to_add = entry->value;

		if (entry->type == EXTERNAL_SYMBOL) {
			add_table_item(symbol_table, operand, (curr_ic)+1 , EXTERNAL_REFERENCE);
		}
                printf("process_direct_operand added table item \n");

		word_to_write = (machine_word *) malloc_with_check(sizeof(machine_word));
		word_to_write->length = 0;
		word_to_write->word.data = build_data_word_direct(data_to_add, entry->type == EXTERNAL_SYMBOL);
                
		printf("process_direct_operand saved word data\n");
		code_img[curr_ic - IC_INIT_VALUE] = word_to_write;

                		
	}
	printf("process_spass_operand ic at end = %d\n", curr_ic);
	return TRUE;
}

