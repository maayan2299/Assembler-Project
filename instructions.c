/**
 * File: instructions.c
 * 
 * Description:
 *  This file contains functions for processing assembly language instructions. It handles parsing and validating
 *  instructions related to string and data operations. The functions process string instructions, convert
 *  data values into integers, and update the data image and counters.
 *
 * Functions:
 *  find_instruction_from_index(): Finds and returns the instruction type based on the index position in the given line.
 *  process_string_instruction(): Processes a string instruction, storing the string data into the data image.
 *  process_data_instruction(): Processes a data instruction, converting the data values into integers and storing them.
 *
 * No direct parameters or return values for the whole file, as it operates as part of the larger assembler system.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"


/**
 * Finds and returns the instruction type based on the index position in the given line.
 * 
 * This function searches for an instruction starting at the given index in the line's content.
 * It identifies and returns the instruction type if valid, or `NONE_INST` if the instruction is not found.
 * 
 * @param line_info line Structure containing the line content and associated metadata.
 * @param int *index Pointer to the index position in the line content.
 * 
 * @return instruction The type of instruction found or `NONE_INST` if no valid instruction is found.
 */

instruction find_instruction_from_index(line_info line, int *index) {
	char temp[MAX_LINE_LENGTH];
	int j;
	instruction result;
 	printf("find instruction from index %d : \n",line);	

	MOVE_TO_NOT_WHITE(line.content, *index) 
	if (line.content[*index] != '.') return NONE_INST;

	
        for (j = 0; line.content[*index] && line.content[*index] != '\t' && line.content[*index] != ' '; (*index)++, j++) {
		temp[j] = line.content[*index];
	}
	temp[j] = '\0'; 
        printf("Instruction: %s\n", temp);

	if ((result = find_instruction_by_name(temp+1)) != NONE_INST) return result;
	printf_line_error(line, "Invalid instruction name: %s", temp);
	return ERROR_INST; 
}


/**
 * Processes a string instruction from the line, storing the string data into the data image.
 * 
 * This function parses a string instruction starting from the given index, checking for correct
 * quotation marks, and stores the string data into the data image array. It updates the data counter.
 * 
 * @param line_info line Structure containing the line content and associated metadata.
 * @param int index The starting index in the line content for processing.
 * @param long *data_img Pointer to the data image array where string data will be stored.
 * @param long *dc Pointer to the data counter, which will be updated with the number of data items processed.
 * 
 * @return bool `TRUE` if processing is successful, `FALSE` otherwise.
 */

bool process_string_instruction(line_info line, int index, long *data_img, long *dc) {
	char temp_str[MAX_LINE_LENGTH];
	char *last_quote_location = strrchr(line.content, '"');
	MOVE_TO_NOT_WHITE(line.content, index)
	
        if (line.content[index] != '"') {
		
		printf_line_error(line, "Missing opening quote of string");
		return FALSE;
	} else if (&line.content[index] == last_quote_location) { 
		printf_line_error(line, "Missing closing quote of string");
		return FALSE;
	} else {
		int i, j = 0;
		 temp_str[j++] = line.content[index++];
	
		for (;line.content[index] && line.content[index] != '\n' &&
		       line.content[index] != EOF; index++,i++) {
				data_img[*dc] = line.content[index];
                                (*dc)++;
		}

		
                printf("dc  %s", data_img);

                
	}
	printf("Processing string: %s\n", temp_str);

	return TRUE;
}


/**
 * @brief Processes a data instruction from the line, converting the data values into integers 
 *        and storing them in the data image.
 * 
 * This function parses a data instruction starting from the given index, converting the values
 * into integers, and storing them in the data image array. It updates the data counter.
 * 
 * @param line_info line Structure containing the line content and associated metadata.
 * @param int index The starting index in the line content for processing.
 * @param long *data_img Pointer to the data image array where data values will be stored.
 * @param long *dc Pointer to the data counter, which will be updated with the number of data items processed.
 * 
 * @return bool `TRUE` if processing is successful, `FALSE` otherwise.
 */

bool process_data_instruction(line_info line, int index, long *data_img, long *dc) {
	char temp[80], *temp_ptr;
	long value;
	int i;
	MOVE_TO_NOT_WHITE(line.content, index)
	if (line.content[index] == ',') {
		printf_line_error(line, "Unexpected comma after .data instruction");
	}
	do {
		for (i = 0;
		     line.content[index] && line.content[index] != EOF && line.content[index] != '\t' &&
		     line.content[index] != ' ' && line.content[index] != ',' &&
		     line.content[index] != '\n'; index++, i++) {
			temp[i] = line.content[index];
		}
		temp[i] = '\0'; 

		if (!is_int(temp)) {
			printf_line_error(line, "Expected integer for .data instruction (got '%s')", temp);
			return FALSE;
		}

		value = strtol(temp, &temp_ptr, 10);

		data_img[*dc] = value;

		(*dc)++; 
		MOVE_TO_NOT_WHITE(line.content, index)
		if (line.content[index] == ','){
			index++;
			MOVE_TO_NOT_WHITE(line.content, index)
		}else if (!line.content[index] || line.content[index] == '\n' || line.content[index] == EOF){
			break; 
			 printf_line_error(line, "Unexpected character '%c' after data value", line.content[index]);
			return FALSE;
		}
	} while (line.content[index] != '\n' && line.content[index] != EOF);

	return TRUE;
}

