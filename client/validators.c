/*
 * validators.c
 *
 * Description: contains function to validate values recevied by server, namely the gold counts and
 * player syumbol.
 * 
 * Author: Joseph Hirsh
 * Date: March 1st, 2024
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "validators.h"

/*
 * Ensures gold value is between zero and the maximum possible gold value.
 */
bool 
validate_gold_count(const int count, const int maximum) 
{
    return count > 0 && count <= maximum;
}

/*
 * Ensure player symbol is alphabetical and capital 
 */
bool
validate_player_symbol(const char symbol)
{
    return symbol >= 'A' && symbol <= 'Z';
}