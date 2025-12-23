#ifndef FUNCS_H
#define FUNCS_H

// These are the shared constants used by funcs.c and main.c
#define MAX_LENGTH 128
#define MIN_LENGTH 8

// Public Application Programming Interface implemented in funcs.c 
void read_password_from_user(char password[], int max_length);

int  check_minimum_length(const char password[]);
int  check_required_character_types(const char password[]);
int  has_simple_sequence(const char password[]);
int  is_common_password_top20(const char password[]);
int  compute_password_score(const char password[]);

void print_password_feedback(const char password[], int score);

#endif
