/**
 * File: global.h
 * 
 * Description:
 *  This header file contains definitions and declarations for various data types and constants used

 *  throughout the assembler project. It includes enumerations for addressing types, opcodes, functions,
 *  and registers, as well as structures for representing code words, data words, and machine words.
 
 *
 * @defgroup Enums Enumerations
 *  Enumerations used in the assembler, including addressing types, opcodes, and functions.
 * 
 * @defgroup Structs Structures
 *  Structures used to represent different aspects of the machine code and source lines.
 * 
 * @defgroup Constants Constants
 *  Constants used for various limits and initial values.
 * 
 * @details This file is included in all modules that require access to shared data types, enumerations, and constants.
 */

#ifndef _GLOBALS_H
#define _GLOBALS_H



typedef enum booleans {
    FALSE = 0,
    TRUE = 1
} bool;


#define CODE_ARR_IMG_LENGTH 1200
#define MAX_LINE_LENGTH 80
#define IC_INIT_VALUE 100


/*Operand addressing type */
typedef enum addressing_types {
	IMMEDIATE_ADDR = 0,
	DIRECT_ADDR = 1,
	REGISTER_INDIRECT_ADDR = 2,
	REGISTER_ADDR = 3,
	NONE_ADDR = -1
} addressing_type;

/* Commands opcode */
typedef enum opcodes {
	/* First Group */
	MOV_OP = 0,
	CMP_OP = 1,

	ADD_OP = 2,
	SUB_OP = 3,

	LEA_OP = 4,
	/* END First Group */

	/* Second Group */
	CLR_OP = 5,
	NOT_OP = 6,
	INC_OP = 7,
	DEC_OP = 8,

	JMP_OP = 9,
	BNE_OP = 10,
	RED_OP = 11,
	PRN_OP = 12,
	JSR_OP = 13,
	/* END Second Group */

	/* Third Group */
	
	RTS_OP = 14,
	STOP_OP = 15,
	/* END Third Group */

	/* Failed/Error */
	NONE_OP = -1
} opcode;

/* Commands funct */
typedef enum funct {
	/* OPCODE 2 */
	ADD_FUNCT = 1,
	SUB_FUNCT = 2,

	/* OPCODE 5 */
	CLR_FUNCT = 1,
	NOT_FUNCT = 2,
	INC_FUNCT = 3,
	DEC_FUNCT = 4,

	/* OPCODE 9 */
	JMP_FUNCT = 1,
	BNE_FUNCT = 2,
	JSR_FUNCT = 3,

	/* Default (No need/Error) */
	NONE_FUNCT = 0
} funct;

/* Registers - r0->r1 + not found */
typedef enum registers {
	R0 = 0,
	R1,
	R2,
	R3,
	R4,
	R5,
	R6,
	R7,
	NONE_REG = -1
} reg;
/** Represents a single code word */
typedef struct code_word {
	/* First byte: ARE+funct */
	unsigned int ARE: 3;
	unsigned int funct: 5;
	/* 2nd byte: destination+addressing, source */
	unsigned int dest_register: 3;
	unsigned int dest_addressing: 2;
	unsigned int src_register: 3;
	/* Third byte: source addressing, opcode */
	unsigned int src_addressing: 2;
	unsigned int opcode: 6;

} code_word;

/* Represents a single data word. */
typedef struct data_word {
	unsigned int ARE: 3;
	
	unsigned long data;
} data_word;

/* Represents a general machine code word contents */
typedef struct machine_word {
	
	short length;

	union word {
		data_word *data;
		code_word *code;
	} word;
} machine_word;


typedef enum instruction {
	/** .data instruction */
	DATA_INST,
	/** .extern instruction */
	EXTERN_INST,
	/** .entry instruction */
	ENTRY_INST,
	/** .string instruction */
	STRING_INST,
	/** Not found */
	NONE_INST,
	/** Parsing/syntax error */
	ERROR_INST
} instruction;


 /* Represents a single source line, including it's details */

typedef struct line_info {

	long line_number;

	char *file_name;

	char *content;
} line_info;


#endif

