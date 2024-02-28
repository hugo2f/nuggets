#ifndef _VALIDATORS_H_
#define _VALIDATORS_H_

#include <stdbool.h>

bool validate_gold_count(const int count, const int maximum);

bool validate_player_symbol(const char symbol);

#endif