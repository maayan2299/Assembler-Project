/**
 * File: table.c
 * 
 * Description:
 *  This file contains functions for managing and handling a linked list based table.
 *  * It includes functions to add items to the table, free the table, update values based on type,
 * filter the table by type, and find entries based on types and keys.
 *
 * Functions:
 *  add_table_item(): Adds a new item to the table while maintaining sorted order by value.
 *  free_table(): Frees all memory allocated for the table, including the keys and table entries.
 *  add_value_to_type(): Adds a specified value to all entries of a given type in the table.
 *  filter_table_by_type(): Creates and returns a new table containing only entries of a specific type.
 *  find_by_types(): Searches for a table entry that matches a specified key and is of one of the provided types.
 *
 * No direct parameters or return values, as this file operates as part of the larger assembler system.
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "table.h"
#include "utils.h"

/**
 * Adds a new item to the table, maintaining sorted order by value.
 *
 * @param tab Pointer to the head of the table.
 * @param key The key associated with the new item.
 * @param value The value associated with the new item.
 * @param type The type of the new item.
 * 
 */

void add_table_item(table *tab, char *key, long value, symbol_type type) {
	char *temp_key;
	table prev_entry, curr_entry, new_entry;
	new_entry = (table) malloc_with_check(sizeof(table_entry));
	
	temp_key = (char *) malloc_with_check(strlen(key) + 1);
	strcpy(temp_key, key);
	new_entry->key = temp_key;
	new_entry->value = value;
	new_entry->type = type;

	if ((*tab) == NULL || (*tab)->value > value) {
		new_entry->next = (*tab);
		(*tab) = new_entry;
		return;
	}

	curr_entry = (*tab)->next;
	prev_entry = *tab;
	while (curr_entry != NULL && curr_entry->value < value) {
		prev_entry = curr_entry;
		curr_entry = curr_entry->next;
	}

	new_entry->next = curr_entry;
	prev_entry->next = new_entry;
}


/**
 * Frees all memory allocated for the table, including the keys and table entries.
 *
 * @param tab Pointer to the table to be freed.
 */

void free_table(table tab) {
	table prev_entry, curr_entry = tab;
	while (curr_entry != NULL) {
		prev_entry = curr_entry;
		curr_entry = curr_entry->next;
		free(prev_entry->key); 
		free(prev_entry);
	}
}


/**
 * Adds a specified value to all entries of a given type in the table.
 *
 * @param tab Pointer to the table in which the values are updated.
 * @param to_add The value to add.
 * @param type The type of entries to update.
 */

void add_value_to_type(table tab, long to_add, symbol_type type) {
	table curr_entry;

	for (curr_entry = tab; curr_entry != NULL; curr_entry = curr_entry->next) {
		if (curr_entry->type == type) {
			curr_entry->value += to_add;
		}
	}
}


/**
 * Creates and returns a new table containing only entries of a specific type.
 *
 * @param tab Pointer to the original table.
 * @param type The type of entries to filter.
 * @return A pointer to the new table containing only the filtered entries.
 */

table filter_table_by_type(table tab, symbol_type type) {
	table new_table = NULL;

	do {
		if (tab->type == type) {
			add_table_item(&new_table, tab->key, tab->value, tab->type);
		}
	} while ((tab = tab->next) != NULL);
	return new_table; 
}


/**
 * Searches for a table entry that matches a specified key and is of one of the provided types.
 *
 * @param tab Pointer to the table to search in.
 * @param key The key of the entry to find.
 * @param symbol_count The number of types to check for.
 * @param ... Variable list of valid symbol types.
 * @return A pointer to the matching table entry, or NULL if no match is found.
 */

table_entry *find_by_types(table tab, char *key, int symbol_count, ...) {
	int i;
	symbol_type *valid_symbol_types = malloc_with_check((symbol_count) * sizeof(int));

	va_list arglist;
	va_start(arglist, symbol_count);
	for (i = 0; i < symbol_count; i++) {
		valid_symbol_types[i] = va_arg(arglist, symbol_type);
	}
	va_end(arglist);

	if (tab == NULL) {
		free(valid_symbol_types);
		return NULL;
	}
	
	do {
		for (i = 0; i < symbol_count; i++) {
			if (valid_symbol_types[i] == tab->type && strcmp(key, tab->key) == 0) {
				free(valid_symbol_types);
				return tab;
			}
		}
	} while ((tab = tab->next) != NULL);


	free(valid_symbol_types);
	return NULL;
}

