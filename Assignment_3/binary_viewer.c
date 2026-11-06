#include <stdio.h>

int main(int argc, char *argv[])
{
    /* 1. التحقق من وجود اسم الملف */
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    /* 2. فتح الملف في الوضع الثنائي */
    FILE *fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        perror("Error opening file");
        return 1;
    }

    unsigned char buffer[16];
    size_t bytesRead;
    unsigned long offset = 0;

    /* 3. قراءة 16 بايت في كل دورة */
    while ((bytesRead = fread(buffer, 1, 16, fp)) > 0) {

        /* العنوان (Offset) */
        printf("%08lX  ", offset);

        /* 4. عمود الهكس */
        for (size_t i = 0; i < 16; i++) {
            if (i < bytesRead)
                printf("%02X ", buffer[i]);
            else
                printf("   ");          /* مسافة بديلة للسطر الأخير القصير */
            if (i == 7)
                printf(" ");            /* فاصل في منتصف السطر */
        }

        /* 5. عمود ASCII */
        printf(" |");
        for (size_t i = 0; i < bytesRead; i++) {
            if (buffer[i] >= 32 && buffer[i] <= 126)
                putchar(buffer[i]);
            else
                putchar('.');
        }
        printf("|\n");

        offset += bytesRead;
    }

    /* 6. إغلاق الملف */
    fclose(fp);
    return 0;
}
