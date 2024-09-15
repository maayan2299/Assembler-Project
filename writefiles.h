
#ifndef _WRITEFILES_H
#define _WRITEFILES_H
#include "globals.h"
#include "table.h"



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
                       table symbol_table);

#endif

