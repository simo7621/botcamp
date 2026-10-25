#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

int main(void)
{
    char password[256];
    bool strong;

    do {
        // إعادة ضبط العلامات قبل فحص كل محاولة جديدة
        bool upper = false, lower = false;
        bool digit = false, symbol = false, space = false;

        printf("Enter password: ");

        if (fgets(password, sizeof(password), stdin) == NULL)
            return 1;

        size_t len = strcspn(password, "\n");
        bool newline = password[len] == '\n';
        password[len] = '\0';

        if (!newline) {
            int ch = getchar();
            if (ch != '\n' && ch != EOF) {
                while ((ch = getchar()) != '\n' && ch != EOF) {}
                puts("Password too long. Try again.");
                continue;
            }
        }

        len = strlen(password);

        // فحص كل حرف وتحديد نوعه
        for (size_t i = 0; i < len; i++) {
            unsigned char c = (unsigned char)password[i];

            if (isupper(c)) upper = true;
            else if (islower(c)) lower = true;
            else if (isdigit(c)) digit = true;
            else if (isspace(c)) space = true;
            else symbol = true;
        }

        // قبول كلمة المرور فقط عند تحقق جميع الشروط
        strong = len >= 12 && upper && lower &&
                 digit && symbol && !space;

        if (!strong)
            puts("Use 12+ characters, uppercase, lowercase, a number, a symbol, and no spaces. Try again.");

    } while (!strong);

    puts("Password accepted!");
    return 0;
}
