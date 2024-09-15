/**
 * File: main.c
 * 
 * Description:
 *  This file is the main entry point for the macro processing program. It handles the file processing logic,
 *  where the input file is processed to replace macros with their definitions, and the resulting output is written
 *  to a new file with the extension ".am".
 *
 * Functions:
 *  process_file(): Processes the input file, replacing macros with their content and saving the result in a new output file.
 *  main(): The main function of the program, handling command-line input and initiating file processing.
 */

#include <stdio.h>
#include "macr.h"      
#include "table.h"     
#include "globals.h" 
#include "utils.h"    


/**
 * Processes the input file, replacing macros with their content and writing the result to an output file.
 *
 * @param inputFile The name of the input file (without extension).
 *                  The function generates an output file with the ".am" extension.
 */

void process_file(char *inputFile) {
    char outputFile[SIZE_LINE];
    snprintf(outputFile, SIZE_LINE, "%s.am", inputFile);  
    macr(inputFile, outputFile);
 
}


/**
 * The main function of the program, responsible for handling command-line arguments and initiating the file processing.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 *             The first argument should be the input file name.
 * @return Returns 0 on successful execution, or 1 on error (e.g., incorrect usage).
 */

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
        return 1;
    }
    process_file(argv[1]);
    return 0;
}


