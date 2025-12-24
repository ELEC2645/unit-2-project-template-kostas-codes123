#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "funcs.h"

// Prototypes only in this file (static = private to main.c)
static void main_menu(void);
static void print_main_menu(void);
static int  get_user_input(void);
static void select_menu_item(int input);
static void go_back_to_main(void);
static int  is_integer(const char *s);

// Global password buffer shared across menu items
static char password[MAX_LENGTH] = "";

int main(void)
{
    // Main program loop it shows menu, handles selection, repeats until Exit
    for (;;) {
        main_menu();
    }
    return 0; // Unreachable (exit() used), but keeps main well-formed
}

static void main_menu(void)
{
    // One menu cycle: print menu, read choice, dispatch to handler
    print_main_menu();
    {
        int input = get_user_input();
        select_menu_item(input);
    }
}

static void print_main_menu(void)
{
    // Keep menu text in one place for easy editing
    puts("");
    puts("==== Password Strength Menu ====");
    puts("1) Enter password");
    puts("2) Compute score + print feedback");
    puts("3) Show basic checks (min length / char types / common / sequence)");
    puts("4) Clear password");
    puts("5) Exit");
    puts("================================");
}

static int get_user_input(void)
{
    // Read a full line (fgets) and essecially avoids scanf() input issues
    char buf[64];

    for (;;) {
        printf("Select option: ");
        if (fgets(buf, sizeof buf, stdin) == NULL) {
            /* EOF / error => exit */
            return 5;
        }
// Strip trailing newline so validation is predictable
        buf[strcspn(buf, "\r\n")] = '\0';
// Accept only integers
        if (is_integer(buf)) {
            return atoi(buf);
        }

        puts("Invalid input (enter a number).");
    }
}

static void select_menu_item(int input)
{
    // Dispatch based on the user's menu choice
    switch (input) {
    case 1:
    // Store the password for later
        read_password_from_user(password, (int)sizeof password);
        go_back_to_main();
        break;

    case 2:
        if (password[0] == '\0') {
            puts("No password set yet. Choose option 1 first.");
        } else {
            int score = compute_password_score(password);
            print_password_feedback(password, score);
        }
        go_back_to_main();
        break;

    case 3:
    // Show each rule check result for the current password
        if (password[0] == '\0') {
            puts("No password set yet. Choose option 1 first.");
        } else {
            printf("Password: \"%s\"\n", password);
            printf("Minimum length (%d): %s\n", MIN_LENGTH,
                   check_minimum_length(password) ? "PASS" : "FAIL");
            printf("Required character types: %s\n",
                   check_required_character_types(password) ? "PASS" : "FAIL");
            printf("Top 20 common password: %s\n",
                   is_common_password_top20(password) ? "YES (bad)" : "NO");
            printf("Simple sequence found: %s\n",
                   has_simple_sequence(password) ? "YES (bad)" : "NO");
        }
        go_back_to_main();
        break;

    case 4:
    // Clear the saved password by turning it into an empty string
        password[0] = '\0';
        puts("Password cleared.");
        go_back_to_main();
        break;

    case 5:
    // this acts as Immediate program termination
        puts("Goodbye.");
        exit(0);

    default:
    // Any number not in 1 to 5 is invalid
        puts("Invalid menu option.");
        go_back_to_main();
        break;
    }
}

static void go_back_to_main(void)
{
    // A pause so output stays visible until the user presses Enter
    puts("\n(Press Enter to continue...)");
    {
        char tmp[8];
        (void)fgets(tmp, sizeof tmp, stdin);
    }
}

static int is_integer(const char *s)
{
    // Skip leading whitespace
    while (*s && isspace((unsigned char)*s)) s++;

    // it reject empty strings
    if (*s == '\0') return 0;

    // Must have at least one digit
    if (*s == '+' || *s == '-') s++;

    if (!isdigit((unsigned char)*s)) return 0;
    // all the remaining characters must be digits
    while (*s) {
        if (!isdigit((unsigned char)*s)) return 0;
        s++;
    }
    return 1;
}
