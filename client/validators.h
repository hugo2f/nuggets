/*
 * validators.h
 *
 * Description: contains interface for validation module.
 * 
 * Author: Joseph Hirsh
 * Date: March 2st, 2024
 *
 */

#ifndef _VALIDATORS_H_
#define _VALIDATORS_H_

#include <stdbool.h>

/*
 * Ensures gold value is between zero and the maximum possible gold value.
 */
bool validate_gold_count(const int count, const int maximum);

/*
 * Ensure player symbol is alphabetical and capital 
 */
bool validate_player_symbol(const char symbol);

/*
 * Ensure input character is not stdin end of file (EOF)
 */
bool validate_stdin_character(const char stdinCharacter);

#endif