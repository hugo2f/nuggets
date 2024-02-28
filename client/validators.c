#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "validators.h"

bool 
validate_gold_count(const int count, const int maximum) 
{
    return count > 0 && count <= maximum;
}

bool
validate_player_symbol(const char symbol)
{
    return symbol >= 'A' && symbol <= 'Z';
}