#include <stdio.h>
#include <string.h>
#include "funcs.h"
#include <ctype.h>
#include <stddef.h>

/* ---------- small internal helpers which are private to funcs.c ---------- */

static void flush_rest_of_line(void)
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
        // discard
    }
}

static void analyse_character_types(const char password[],
                                    int *has_lower,
                                    int *has_upper,
                                    int *has_digit,
                                    int *has_special)
{
    *has_lower = 0;
    *has_upper = 0;
    *has_digit = 0;
    *has_special = 0;

    if (!password) return;

    for (const unsigned char *p = (const unsigned char *)password; *p != '\0'; ++p) {
        if (islower(*p)) *has_lower = 1;
        else if (isupper(*p)) *has_upper = 1;
        else if (isdigit(*p)) *has_digit = 1;
        else if (ispunct(*p)) *has_special = 1; // this is used f or punctuation only
        // whitespace doesn't count as special here 
    }
}

/* ---------- required functions ---------- */

void read_password_from_user(char password[], int max_length)
{
    if (!password || max_length <= 0) return;

    password[0] = '\0';

    printf("Enter password (max %d characters): ", max_length - 1);

    if (fgets(password, max_length, stdin) == NULL) {
        //error
        return;
    }

    // Remove trailing newline if present 
    password[strcspn(password, "\r\n")] = '\0';

    
//If input was longer than the buffer, fgets won't consume the newline.
      //This would break later menu input.
    if ((int)strlen(password) == max_length - 1) {
        flush_rest_of_line();
    }
}

int check_minimum_length(const char password[])
{
    if (!password) return 0;
    return (strlen(password) >= MIN_LENGTH) ? 1 : 0;
}

int check_required_character_types(const char password[])
{
    int has_lower, has_upper, has_digit, has_special;
    analyse_character_types(password, &has_lower, &has_upper, &has_digit, &has_special);

    return (has_lower && has_upper && has_digit && has_special) ? 1 : 0;
}

// Detect any 3-character ascending or descending sequence for digits or letters for example 123. 
int has_simple_sequence(const char password[])
{
    if (!password) return 0;

    size_t n = strlen(password);
    if (n < 3) return 0;

    for (size_t i = 0; i + 2 < n; i++) {
        unsigned char a = (unsigned char)password[i];
        unsigned char b = (unsigned char)password[i + 1];
        unsigned char c = (unsigned char)password[i + 2];

        // digits 
        if (isdigit(a) && isdigit(b) && isdigit(c)) {
            if ((b == a + 1 && c == b + 1) || (b == a - 1 && c == b - 1)) return 1;
        }

        // lowercase letters
        if (islower(a) && islower(b) && islower(c)) {
            if ((b == a + 1 && c == b + 1) || (b == a - 1 && c == b - 1)) return 1;
        }

        // uppercase letters
        if (isupper(a) && isupper(b) && isupper(c)) {
            if ((b == a + 1 && c == b + 1) || (b == a - 1 && c == b - 1)) return 1;
        }
    }

    return 0;
}

int is_common_password_top20(const char password[])
{
    if (!password || password[0] == '\0') return 0;

    // Embedded list without file dependency
    static const char *top20[] = {
        "123456", "123456789", "12345", "qwerty", "password",
        "12345678", "111111", "123123", "1234567", "dragon",
        "1234567890", "1234", "iloveyou", "admin", "welcome",
        "monkey", "login", "abc123", "starwars", "letmein"
    };

    for (size_t i = 0; i < (sizeof(top20) / sizeof(top20[0])); i++) {
        if (strcmp(password, top20[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

/*
Score idea (0..10):
  +2 if length >= MIN_LENGTH
  +1 if length >= 12
  +1 each: lower/upper/digit/special (max +4)
  -3 if in top20 common list
  -2 if simple sequence exists
  clamp to 0..10
*/
// Like Pokemon: we store stats in a struct, then fill them via a pointer
struct PasswordReport {
    int score;
    int length;

    int has_lower;
    int has_upper;
    int has_digit;
    int has_special;

    int is_top20;
    int has_sequence;
};

// inspired to ammend the follwing from stucks with pointer Pokemon example.
static void analyse_password(const char password[], struct PasswordReport *rep)
{
    // performs safety checks
    if (rep == NULL) return;


    // resets everything to 0 
    rep->score = 0;
    rep->length = 0;
    rep->has_lower = 0;
    rep->has_upper = 0;
    rep->has_digit = 0;
    rep->has_special = 0;
    rep->is_top20 = 0;
    rep->has_sequence = 0;

    // handles NULL password 
  if (password == NULL) return;
 {

    }

    // tests the length
    rep->length = (int) strlen(password);

    // scans characters and set type flags 
    for (int i = 0; password[i] != '\0'; i++) {
        unsigned char uc = (unsigned char)password[i];

        if (islower(uc)) {
            rep->has_lower = 1;
        } else if (isupper(uc)) {
            rep->has_upper = 1;
        } else if (isdigit(uc)) {
            rep->has_digit = 1;
        } else if (ispunct(uc)) {
            rep->has_special = 1; // punctuation only 
        }
    }

    //  checks penalties using your other functions 
    rep->is_top20 = is_common_password_top20(password);
    rep->has_sequence = has_simple_sequence(password);

    // computes score using your rule set 
    rep->score = 0;

    if (rep->length >= MIN_LENGTH) rep->score +=2;
    if (rep->length >= 12) rep->score += 1;

    rep->score += rep->has_lower;
    rep->score += rep->has_upper;
    rep->score += rep->has_digit;
    rep->score += rep->has_special;

    if (rep->is_top20)     rep->score -= 3;
    if (rep->has_sequence) rep->score -= 2;

    // Step H: clamp 0..10 
    if (rep->score < 0) rep->score = 0;
    if (rep->score > 10) rep->score = 10;
}

// Public function: returns only the score percieved level returned, keeps Application Programming Interface simple.
int compute_password_score(const char password[])
{
    struct PasswordReport rep;

    // calls analyser using '&' like Pokemon example 
    analyse_password(password, &rep);

    //  return the computed score
    return rep.score;
}

void print_password_feedback(const char password[], int score)
{
    // performs safety check 
    if (password == NULL) {
        printf("No password provided.\n");
        return;
    }

    // it e ssencially print basic summary
    int len = (int)strlen(password);
    printf("Password length: %d\n", len);
    printf("Score: %d/10\n", score);

    // gives the user a strength label based on score
    if (score <= 3) {
        printf("Strength: Weak\n");
    } else if (score <= 6) {
        printf("Strength: Medium\n");
    } else if (score <= 8) {
        printf("Strength: Strong\n");
    } else {
        printf("Strength: Very strong\n");
    }

    // gathers rule info for tips
    int has_lower = 0, has_upper = 0, has_digit = 0, has_special = 0;
    analyse_character_types(password, &has_lower, &has_upper, &has_digit, &has_special);

    // prints improvement tips
    if (len < MIN_LENGTH) {
        printf("- Use at least %d characters.\n", MIN_LENGTH);
    }

    if (!has_lower)   printf("- Add lowercase letters (a-z).\n");
    if (!has_upper)   printf("- Add uppercase letters (A-Z).\n");
    if (!has_digit)   printf("- Add digits (0-9).\n");
    if (!has_special) printf("- Add symbols (e.g. ! @ #).\n");

    if (is_common_password_top20(password)) {
        printf("- This password is too common (top 20). Avoid it.\n");
    }

    if (has_simple_sequence(password)) {
        printf("- Avoid simple sequences like 123 or abc.\n");
    }

    // leaves a closing message 
    if (score >= 7) {
        printf("Nice! This is a solid password.\n");
    } else {
        printf("Consider improving it using the tips above.\n");
    }
}
