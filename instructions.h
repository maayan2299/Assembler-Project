/**
 * @file instructions.h
 * Provides functions for handling different types of instructions in the assembler.
 * 
 * This header file includes function declarations for:
 * - Finding instructions from an index.
 * - Processing string and data instructions.
 */

#ifndef _INSTRUCTIONS_H
#define _INSTRUCTIONS_H
#include "globals.h"


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

instruction find_instruction_from_index(line_info line, int *index);


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

bool process_string_instruction(line_info line, int index, long *data_img, long *dc);


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

bool process_data_instruction(line_info line, int index, long *data_img, long *dc);

#endif


