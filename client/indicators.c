#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "graphics.h"
#include "indicators.h"

void 
indicate_invalid_key(const char key) 
{
    char message[20];
    snprintf(message, sizeof(message), "Invalid keystroke %c", key);

    remove_from_banner();
    append_to_banner(message);
}

void
indicate_nuggets_collected(const int collected) 
{
    char message[45];
    snprintf(message, sizeof(message), "You collected %d nuggets!", collected);

    append_to_banner(message);
}