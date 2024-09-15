/**
 * File: assembler.c
 * 
 * Description:
 *  This file contains the main function of the assembler program. It processes one or more input files
 *  function to handle the processing of each file, and manages the overall execution flow of the assembler program.
 *
 * Functions:
 *  main(): Entry point of the assembler program. Processes each input file provided as a command-line argument,
 *  calling the process_file function for each file. Returns 0 upon successful completion.
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


/**
 * Main function of the assembler program that processes one or more input files.
 *
 * @param argc - The number of command-line arguments passed, including the program name.
 * @param argv - Array of command-line arguments. The first argument is the program name,
 * and the subsequent arguments represent the files to be processed.
 * 
 * @return int - Returns 0 when the assembler finishes running successfully.
 */

int main(int argc, char *argv[]) {
	int i;

	bool succeeded = TRUE;

	for (i = 1; i < argc; ++i) {
	
		if (!succeeded) puts("");

		succeeded = process_file(argv[i]);

	}
	return 0;
}



