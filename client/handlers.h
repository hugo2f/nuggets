#ifndef _HANDLERS_H_
#define _HANDLERS_H_

void handle_okay(char* symbol); 

void handle_grid(char* coordinates); 

void handle_gold(char* counts);

void handle_display(char* map); 

void handle_quit(char* explanation);

void handle_error(char* error); 

#endif
