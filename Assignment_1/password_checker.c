/*
 * password_checker.c - Password Strength Checker
 *
 * Assignment 1: Build a Password Strength Checker tool.
 *
 * Build  : gcc -Wall -Wextra -std=c99 -o password_checker password_checker.c
 * Run    : ./password_checker
 */

#define _CRT_SECURE_NO_WARNINGS   /* for Visual Studio (scanf warning) */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_LEN 100   /* 99 characters + '\0' */
#define MIN_LEN 8     /* minimum accepted length */

/* Structure to hold the analysis result */
struct Report {
    int length;      /* password length              */
    int has_upper;   /* contains uppercase letter?   */
    int has_lower;   /* contains lowercase letter?   */
    int has_digit;   /* contains digit?              */
    int has_symbol;  /* contains symbol?             */
    int score;       /* total score                  */
};

/*
 * Main analysis function.
 * Receives the password as a char array,
 * loops over each character to classify it,
 * then returns a filled Report structure.
 */
struct Report analyze_password(const char password[])
{
    struct Report r;
    int i;

    /* Initialize all fields */
    r.length     = 0;
    r.has_upper  = 0;
    r.has_lower  = 0;
    r.has_digit  = 0;
    r.has_symbol = 0;
    r.score      = 0;

    /* Compute length using strlen */
    r.length = (int)strlen(password);

    /* Loop over every character and classify it */
    for (i = 0; i < r.length; i++) {
        if (isupper(password[i])) {
            r.has_upper = 1;
        }
        else if (islower(password[i])) {
            r.has_lower = 1;
        }
        else if (isdigit(password[i])) {
            r.has_digit = 1;
        }
        else {
            r.has_symbol = 1;   /* anything else is a symbol */
        }
    }

    /* Compute score */
    if (r.length >= MIN_LEN) {
        r.score++;              /* point for sufficient length */
    }
    r.score += r.has_upper;     /* one point per type present  */
    r.score += r.has_lower;
    r.score += r.has_digit;
    r.score += r.has_symbol;

    return r;
}

/* Print the full report to the user */
void print_report(struct Report r)
{
    int missing = 0;

    printf("\n===== Password Report =====\n");
    printf("Length           : %d\n", r.length);
    printf("Uppercase letters: %s\n", r.has_upper  ? "yes" : "no");
    printf("Lowercase letters: %s\n", r.has_lower  ? "yes" : "no");
    printf("Digits           : %s\n", r.has_digit  ? "yes" : "no");
    printf("Symbols          : %s\n", r.has_symbol ? "yes" : "no");

    /* Print missing conditions */
    printf("\nMissing conditions:\n");
    if (r.length < MIN_LEN) {
        printf(" - Length is less than %d characters\n", MIN_LEN);
        missing = 1;
    }
    if (!r.has_upper) {
        printf(" - No uppercase letter (A-Z)\n");
        missing = 1;
    }
    if (!r.has_lower) {
        printf(" - No lowercase letter (a-z)\n");
        missing = 1;
    }
    if (!r.has_digit) {
        printf(" - No digit (0-9)\n");
        missing = 1;
    }
    if (!r.has_symbol) {
        printf(" - No symbol (e.g. ! @ # $)\n");
        missing = 1;
    }
    if (!missing) {
        printf(" - none\n");
    }

    printf("\nScore            : %d\n", r.score);
}

int main(void)
{
    char buffer[MAX_LEN];
    struct Report report;

    printf("Enter password (up to 99 characters, no spaces): ");

    /* %99s limits input size and prevents buffer overflow */
    if (scanf("%99s", buffer) != 1) {
        printf("No input received.\n");
        return 1;
    }

    /* Call the analysis function on the char array */
    report = analyze_password(buffer);

    /* Print the report */
    print_report(report);

    /* Decide the final result using if / else */
    if (report.length < MIN_LEN || report.score <= 2) {
        printf("Result           : WEAK\n");
    }
    else if (report.score >= 5) {
        printf("Result           : STRONG\n");
    }
    else {
        printf("Result           : ACCEPTABLE\n");
    }

    return 0;
}