/**
 * File: writefiles.c
 * Description:
 *  Contains functions for writing output files related to the assembler.
 *
 * Functions:
*   write_output_files: Writes output files including the object file (.ob), external symbols (.ext), and entry symbols (.ent).
 *  write_ob: Writes the object file (.ob) with the code and data images.
 *  write_table_to_file: Writes a table of symbols to a file with a specified extension.
 *
 * No direct parameters or return values, as this file operates as part of the larger assembler system.
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "table.h"

#define KEEP_ONLY_15_LSB(value) ((value) & 0x7FFF)

static bool write_ob(machine_word **code_img, long *data_img, long icf, long dcf, char *filename);

static bool write_table_to_file(table tab, char *filename, char *file_extension);


/**
 * Writes output files including the object file (.ob), external symbols (.ext), and entry symbols (.ent).
 *
 * @param code_img Array of pointers to machine_word structures representing code image.
 * @param data_img Array of long integers representing data image.
 * @param icf Instruction counter final value.
 * @param dcf Data counter final value.
 * @param filename Base name for output files.
 * @param symbol_table Table of symbols to be written to .ext and .ent files.
 *
 * @return TRUE if all files are written successfully, FALSE otherwise.
 */

int write_output_files(machine_word **code_img, long *data_img, long icf, long dcf, char *filename,
                       table symbol_table) {
	int i;
	printf("Inside write_output_files\n");
	printf("ICF: %ld, DCF: %ld\n", icf, dcf);

	if (code_img == NULL) {
		printf("Error: code_img is NULL\n");
	}
	if (data_img == NULL) {
		printf("Error: data_img is NULL\n");
	}
	if (symbol_table == NULL) {
		printf("Error: symbol_table is NULL\n");
	}



        printf("Inside write_output_files , p2\n");
	bool result, result2;
	table externals = filter_table_by_type(symbol_table, EXTERNAL_REFERENCE);
        printf("Inside write_output_files , p3\n");
	table entries = filter_table_by_type(symbol_table, ENTRY_SYMBOL);
        printf("Inside write_output_files , p4\n");
	/* Write .ob file */

	for (i = 0; i < icf - IC_INIT_VALUE; i++) {
            if (code_img[i] == NULL) {
                printf("Error: code_img[%d] is NULL\n", i);
                    
            }
	    else{
                printf("Processing code_img[%d] %d\n", i,code_img[i]);
	    }
            printf("BBB");
	}
	result = write_ob(code_img, data_img, icf, dcf, filename);
        printf("Inside write_output_files , p5\n");
        result2 = result &&
	         
	         write_table_to_file(externals, filename, ".ext") &&
	         write_table_to_file(entries, filename, ".ent");

	free_table(externals);
	free_table(entries);
	return result2;
}


/**
 * Writes the object file (.ob) with the code and data images.
 *
 * @param code_img Array of pointers to machine_word structures representing code image.
 * @param data_img Array of long integers representing data image.
 * @param icf Instruction counter final value.
 * @param dcf Data counter final value.
 * @param filename Base name for the object file.
 *
 * @return TRUE if the file is written successfully, FALSE otherwise.
 */

static bool write_ob(machine_word **code_img, long *data_img, long icf, long dcf, char *filename) {
	printf("write_ob start");
        int i;
	long val;
	FILE *file_desc;
        code_word *codeword;

	char *output_filename = strallocat(filename, ".ob");

	file_desc = fopen(output_filename, "w");
	if (file_desc == NULL) {
		printf("Can't create or rewrite to file %s.", output_filename);
		free(output_filename);
		return FALSE;
	}
	free(output_filename);


	fprintf(file_desc, "%ld %ld", icf - IC_INIT_VALUE, dcf);

	for (i = 0; i < icf - IC_INIT_VALUE; i++) {
		
                if (code_img[i] != NULL){
			codeword = code_img[i]->word.code;
			printf("Opcode: %d, Funct: %d, Src Addressing: %d, Dest Addressing: %d, lien %d\n", codeword->opcode, codeword->funct, codeword->src_addressing, codeword->dest_addressing, i);
			
		        if (code_img[i]->length > 0) {
				 val = (codeword->opcode << 10) |
		          		(codeword->src_addressing << 8) |
		          		(codeword->src_register << 6) |
		         		(codeword->dest_addressing << 3) |
		         		(codeword->dest_register << 0) |
					(codeword->funct << 3) |
		         		(codeword->ARE);
			} else {	
			
				 val = (KEEP_ONLY_15_LSB(code_img[i]->word.data->data) << 3) | (code_img[i]->word.data->ARE);
				printf("Writing to file: Address: %07d, Value: %06lo\n", i + 100, val);
			}
                }

		fprintf(file_desc, "\n%.7d %.6lo", i + 100, val);
	}


	for (i = 0; i < dcf; i++) {

		val = KEEP_ONLY_15_LSB(data_img[i]);

		fprintf(file_desc, "\n%.7ld %.6lo", icf + i, val);
	}

	/* Close the file */
	fclose(file_desc);
	return TRUE;
}


/**
 * Writes a table of symbols to a file with a specified extension.
 *
 * @param tab Table of symbols to be written.
 * @param filename Base name for the file.
 * @param file_extension Extension for the file (e.g., ".ext" or ".ent").
 *
 * @return TRUE if the file is written successfully, FALSE otherwise.
 */

static bool write_table_to_file(table tab, char *filename, char *file_extension) {
	FILE *file_desc;
	char *full_filename = strallocat(filename, file_extension);
	file_desc = fopen(full_filename, "w");
	free(full_filename);

	if (file_desc == NULL) {
		printf("Can't create or rewrite to file %s.", full_filename);
		return FALSE;
	}

	if (tab == NULL) return TRUE;


	fprintf(file_desc, "%s %.7ld", tab->key, tab->value);
	while ((tab = tab->next) != NULL) {
		fprintf(file_desc, "\n%s %.7ld", tab->key, tab->value);
	}
	fclose(file_desc);
	return TRUE;
}

