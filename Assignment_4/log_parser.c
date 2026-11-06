/*
 * log_parser.c - Security Log File Parser (multiple keywords)
 *
 * Build:
 *   gcc -Wall -Wextra -o log_parser log_parser.c
 *
 * Run:
 *   ./log_parser              (asks for the file name; Enter = test.log)
 *   ./log_parser auth.log     (uses the given file directly)
 *
 * Keywords are typed separated by commas, for example:
 *   FAILED, 404, denied
 * A line is reported when it contains ANY of the keywords.
 * The search is not case-sensitive.
 */

#define _CRT_SECURE_NO_WARNINGS   /* silences Visual Studio warnings about fopen/strtok */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define LINE_SIZE     512           /* buffer for one log line */
#define INPUT_SIZE    512           /* buffer for what the user types */
#define TAGS_SIZE     600           /* buffer for the "matched keywords" label */
#define MAX_KEYWORDS  10
#define MAX_KEY_LEN   50
#define DEFAULT_FILE  "test.log"
#define DEFAULT_KEYS  "FAILED,404"

typedef struct {
    char text[MAX_KEY_LEN + 1];     /* keyword as the user typed it */
    char lower[MAX_KEY_LEN + 1];    /* lowercase copy used for matching */
    unsigned long hits;             /* number of lines that contain it */
} Keyword;

/* Copies src into dst without ever overflowing dst. */
static void safe_copy(char *dst, size_t size, const char *src)
{
    strncpy(dst, src, size - 1);
    dst[size - 1] = '\0';
}

/* Removes spaces from both ends of s (in place). */
static void trim(char *s)
{
    char *start = s;
    char *end;

    while (*start != '\0' && isspace((unsigned char)*start))
        start++;
    end = start + strlen(start);
    while (end > start && isspace((unsigned char)*(end - 1)))
        end--;
    *end = '\0';
    if (start != s)
        memmove(s, start, (size_t)(end - start) + 1);
}

/* Copies src into dst in lowercase (dst must be at least as large as src). */
static void to_lower(char *dst, const char *src)
{
    while (*src != '\0')
        *dst++ = (char)tolower((unsigned char)*src++);
    *dst = '\0';
}

/*
 * Reads one line from the keyboard safely and trims it.
 * Returns: 1 = ok, 0 = input too long, -1 = end of input (EOF)
 */
static int read_input(const char *prompt, char *buf, size_t size)
{
    size_t len;
    int c;

    printf("%s", prompt);
    fflush(stdout);

    if (fgets(buf, (int)size, stdin) == NULL)
        return -1;

    len = strlen(buf);
    if (len > 0 && buf[len - 1] != '\n' && !feof(stdin)) {
        /* input did not fit: throw away the rest of the line */
        while ((c = getchar()) != '\n' && c != EOF)
            ;
        return 0;
    }

    buf[strcspn(buf, "\r\n")] = '\0';
    trim(buf);
    return 1;
}

/* Asks a yes/no question until the answer is valid. Returns 1 for yes, 0 for no/EOF. */
static int ask_yes_no(const char *prompt)
{
    char answer[INPUT_SIZE];
    char lower[INPUT_SIZE];
    int r;

    for (;;) {
        r = read_input(prompt, answer, sizeof(answer));
        if (r < 0)
            return 0;
        if (r == 1) {
            to_lower(lower, answer);
            if (strcmp(lower, "y") == 0 || strcmp(lower, "yes") == 0)
                return 1;
            if (strcmp(lower, "n") == 0 || strcmp(lower, "no") == 0)
                return 0;
        }
        printf("Please answer y or n.\n");
    }
}

/*
 * Splits "FAILED, 404 ,denied" into separate keywords.
 * Returns the number of keywords, or -1 if the input is not valid.
 */
static int parse_keywords(const char *input, Keyword kw[])
{
    char copy[INPUT_SIZE];
    char *token;
    int count = 0;

    safe_copy(copy, sizeof(copy), input);

    token = strtok(copy, ",");
    while (token != NULL) {
        trim(token);
        if (token[0] != '\0') {
            if (count >= MAX_KEYWORDS) {
                printf("Too many keywords (max %d). Try again.\n", MAX_KEYWORDS);
                return -1;
            }
            if (strlen(token) > MAX_KEY_LEN) {
                printf("Keyword '%.20s...' is too long (max %d characters). Try again.\n",
                       token, MAX_KEY_LEN);
                return -1;
            }
            safe_copy(kw[count].text, sizeof(kw[count].text), token);
            to_lower(kw[count].lower, token);
            kw[count].hits = 0;
            count++;
        }
        token = strtok(NULL, ",");
    }

    if (count == 0) {
        printf("Please enter at least one keyword. Try again.\n");
        return -1;
    }
    return count;
}

/* Scans the file line by line and prints every suspicious line with its number. */
static void scan_file(FILE *fp, const char *filename, Keyword kw[], int nkw)
{
    char line[LINE_SIZE];
    char lower_line[LINE_SIZE];
    char tags[TAGS_SIZE];
    int line_hit[MAX_KEYWORDS];
    unsigned long line_no = 0;   /* current line number */
    unsigned long alerts = 0;    /* lines that matched at least one keyword */
    int at_line_start = 1;       /* is this chunk the start of a real new line? */
    int reported = 0;            /* was this line already printed? */
    int ends_with_newline;
    size_t len;
    int i;

    rewind(fp);                  /* go back to the start so we can search again */

    printf("\n=== Scanning '%s' ===\n\n", filename);

    while (fgets(line, sizeof(line), fp) != NULL) {
        len = strlen(line);
        ends_with_newline = (len > 0 && line[len - 1] == '\n');

        /* A line longer than 511 characters is read in several chunks,
           so the line number only grows when a real new line begins. */
        if (at_line_start) {
            line_no++;
            reported = 0;
            for (i = 0; i < nkw; i++)
                line_hit[i] = 0;
        }

        to_lower(lower_line, line);
        tags[0] = '\0';

        for (i = 0; i < nkw; i++) {
            if (!line_hit[i] && strstr(lower_line, kw[i].lower) != NULL) {
                line_hit[i] = 1;
                kw[i].hits++;
                if (!reported) {
                    if (tags[0] != '\0')
                        strcat(tags, ", ");
                    strcat(tags, kw[i].text);
                }
            }
        }

        if (!reported && tags[0] != '\0') {
            alerts++;
            reported = 1;
            line[strcspn(line, "\r\n")] = '\0';
            printf("[Line %lu] [%s] %s\n", line_no, tags, line);
        }

        at_line_start = ends_with_newline;
    }

    printf("\n=== Summary ===\n");
    printf("Total lines scanned : %lu\n", line_no);
    printf("Suspicious entries  : %lu\n", alerts);
    printf("Matches per keyword :\n");
    for (i = 0; i < nkw; i++)
        printf("  %-20s : %lu\n", kw[i].text, kw[i].hits);
    if (alerts == 0)
        printf("No matches found.\n");
}

int main(int argc, char *argv[])
{
    char filename[INPUT_SIZE];
    char input[INPUT_SIZE];
    Keyword kw[MAX_KEYWORDS];
    FILE *fp = NULL;
    int r;
    int nkw;

    printf("=== Security Log Parser ===\n\n");

    /* 1. Open the log file (from the command line, or ask the user) */
    if (argc >= 2) {
        safe_copy(filename, sizeof(filename), argv[1]);
        fp = fopen(filename, "r");
        if (fp == NULL)
            printf("Cannot open '%s' (%s).\n", filename, strerror(errno));
    }

    while (fp == NULL) {
        r = read_input("Log file name (press Enter for " DEFAULT_FILE "): ",
                       filename, sizeof(filename));
        if (r < 0) {
            printf("\nNo input received. Exiting.\n");
            return 1;
        }
        if (r == 0) {
            printf("Input is too long. Try again.\n");
            continue;
        }
        if (filename[0] == '\0')
            safe_copy(filename, sizeof(filename), DEFAULT_FILE);

        fp = fopen(filename, "r");
        if (fp == NULL)
            printf("Cannot open '%s' (%s). Check the name or path and try again.\n",
                   filename, strerror(errno));
    }

    /* 2. Ask for keywords, scan, and offer to search again */
    for (;;) {
        printf("\nEnter keywords separated by commas (example: FAILED, 404, denied)\n");
        r = read_input("Press Enter to use " DEFAULT_KEYS ": ", input, sizeof(input));
        if (r < 0)
            break;
        if (r == 0) {
            printf("Input is too long. Try again.\n");
            continue;
        }
        if (input[0] == '\0')
            safe_copy(input, sizeof(input), DEFAULT_KEYS);

        nkw = parse_keywords(input, kw);
        if (nkw < 0)
            continue;

        scan_file(fp, filename, kw, nkw);

        if (!ask_yes_no("\nSearch again with other keywords? (y/n): "))
            break;
    }

    fclose(fp);
    printf("\nGoodbye.\n");
    return 0;
}
