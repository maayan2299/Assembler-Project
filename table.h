/**
* @file table.h
 * @brief Definitions and functions for managing symbol tables in the assembler.
 */


#ifndef _TABLE_H
#define _TABLE_H

/** 
 * @enum symbol_type
 * @brief Represents different types of symbols used in the assembler.
 */

typedef enum symbol_type {
	CODE_SYMBOL,
	DATA_SYMBOL,
	EXTERNAL_SYMBOL,
	/** Address that contains a reference to (a usage of) external symbol */
	EXTERNAL_REFERENCE,
	ENTRY_SYMBOL
} symbol_type;

/** 
 * @struct entry
 * @brief Represents a single entry in the symbol table.
 */
typedef struct entry* table;


typedef struct entry {
	table next;
	long value;
	char *key;
	symbol_type type;
} table_entry;


/**
 * Adds a new item to the table, maintaining sorted order by value.
 *
 * @param tab Pointer to the head of the table.
 * @param key The key associated with the new item.
 * @param value The value associated with the new item.
 * @param type The type of the new item.
 * 
 */

void add_table_item(table *tab, char *key, long value, symbol_type type);

/**
 * Frees all memory allocated for the table, including the keys and table entries.
 *
 * @param tab Pointer to the table to be freed.
 */

void free_table(table tab);


/**
 * Adds a specified value to all entries of a given type in the table.
 *
 * @param tab Pointer to the table in which the values are updated.
 * @param to_add The value to add.
 * @param type The type of entries to update.
 */

void add_value_to_type(table tab, long to_add, symbol_type type);


/**
 * Creates and returns a new table containing only entries of a specific type.
 *
 * @param tab Pointer to the original table.
 * @param type The type of entries to filter.
 * @return A pointer to the new table containing only the filtered entries.
 */

table filter_table_by_type(table tab, symbol_type type);


/**
 * Searches for a table entry that matches a specified key and is of one of the provided types.
 *
 * @param tab Pointer to the table to search in.
 * @param key The key of the entry to find.
 * @param symbol_count The number of types to check for.
 * @param ... Variable list of valid symbol types.
 * @return A pointer to the matching table entry, or NULL if no match is found.
 */

table_entry *find_by_types(table tab,char *key, int symbol_count, ...);

#endif

