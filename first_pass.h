/**
 * file: first_pass.h
 * Provides the function for processing a single line of code during the first pass of assembly.
 * 
 * This header file includes function declarations for:
 * - Processing a line of code, updating counters, code/image arrays, and symbol tables.
 */

#ifndef _FIRST_PASS_H
#define _FIRST_PASS_H
#include "globals.h"

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
                        table *symbol_table);

#endif

