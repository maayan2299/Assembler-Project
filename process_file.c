/**
 * File: process_file.c
 * 
 * Description:
 *  This file contains the implementation of the process_file function, which is responsible for processing an
 *  assembly file. The function handles macro expansion, performs first pass and second pass processing of the
 *  assembly code, and writes the output files. It also manages file allocation and deallocation, memory management
 *  for code and data images, and symbol table operations.
 *
 * Functions:
 *  process_file(): Processes the specified assembly file by performing macro expansion, first pass and second pass
 *  processing, and writing output files. It also manages memory allocation for the file names and cleans up after
 *  processing.
 *
 * No direct parameters or return values for the whole file, as it operates as part of the larger assembler system.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "writefiles.h"
#include "utils.h"
#include "first_pass.h"
#include "second_pass.h"
#include "macr.h"
#include "process_file.h"


bool process_file(char *filename);


/**
 * @brief Processes the specified assembly file by performing macro expansion, first pass and second pass processing,
 *        and writing output files.
 * 
 * @param filename The name of the assembly file to be processed.
 * @return true if the processing is successful, false otherwise.
 * 
 * This function:
 * - Calls macro() to expand macros in the file.
 * - Creates file names with .as and .am extensions.
 * - Opens the macro file and processes it line by line.
 * - Performs the first pass to parse and validate instructions.
 * - Performs the second pass to handle additional processing and generates output files.
 * - Frees allocated memory and closes open files.
 */

bool process_file(char *filename) {

	int temp_c;
	long ic = IC_INIT_VALUE, dc = 0, icf, dcf;
	bool is_success = TRUE; 
	char *input_filename;
	char *macro_filename;
	FILE *file_des; 
	long data_img[CODE_ARR_IMG_LENGTH]; 
	machine_word *code_img[CODE_ARR_IMG_LENGTH];

	table symbol_table = NULL;
	line_info curr_line_info;
	
	printf("Before macro call\n");
	macro(filename);
	printf("After macro call\n");
	printf("Before strallocat for input_filename\n");
	input_filename = strallocat(filename, ".as");
	printf("After strallocat for input_filename: %s\n", input_filename);

	printf("Before strallocat for macro_filename\n");
	macro_filename = strallocat(filename, ".am");
	printf("After strallocat for macro_filename: %s\n", macro_filename);

	printf("Allocated macro filename: %s\n", macro_filename);

	printf("Trying to open file: %s\n", macro_filename);
	
	file_des = fopen(macro_filename, "r");

	if (file_des == NULL) {
		printf("Error: file \"%s.am\" is inaccessible for reading. skipping it.\n", macro_filename);
		free(input_filename);
		free(macro_filename);
		return FALSE;
	}
	

	curr_line_info.file_name = input_filename;
	curr_line_info.content = temp_line; 
	for (curr_line_info.line_number = 1;
	     fgets(temp_line, MAX_LINE_LENGTH + 2, file_des) != NULL; curr_line_info.line_number++) {
		
		if (strchr(temp_line, '\n') == NULL && !feof(file_des)) {
			
			printf_line_error(curr_line_info, "Line too long to process. Maximum line length should be %d.",
			                  MAX_LINE_LENGTH);
			is_success = FALSE;

			do {
				temp_c = fgetc(file_des);
			} while (temp_c != '\n' && temp_c != EOF);
		} else {
			if (!process_line_fpass(curr_line_info, &ic, &dc, code_img, data_img, &symbol_table)) {
				if (is_success) {
	
					icf = -1;
					is_success = FALSE;
				}
			}
		}
	}
	

	icf = ic;
	dcf = dc;
	printf("ic: %d\n dc: %d\n",ic ,dc);

	if (is_success) {
	
		ic = IC_INIT_VALUE;

		add_value_to_type(symbol_table, icf, DATA_SYMBOL);

		rewind(file_des); 

		for (curr_line_info.line_number = 1; !feof(file_des); curr_line_info.line_number++) {
			int i = 0;
			fgets(temp_line, MAX_LINE_LENGTH, file_des);
			MOVE_TO_NOT_WHITE(temp_line, i)
			if (code_img[ic - IC_INIT_VALUE] != NULL || temp_line[i] == '.'){
		                		printf("Trying to open file: %s\n",temp_line);
				is_success &= process_line_spass(curr_line_info, &ic, code_img, &symbol_table);
		        }
		}
		
		if (is_success) {
			
			is_success = write_output_files(code_img, data_img, icf, dcf, filename, symbol_table);

		}
	}

	fclose(file_des);

	free(input_filename);
	free_table(symbol_table);
	free_code_image(code_img, icf);

	free(macro_filename);
	return is_success;
}

