//#!gcc test-echo.c -o test-echo -g -rdynamic
#include <stdio.h>
#include <unistd.h>

int main(int argc, const char* argv[])
{
    char buffer[4096];
    while (!feof(stdin) && !ferror(stdin)) {
        const char* ptr = fgets(buffer, sizeof(buffer), stdin);
        if (NULL != ptr) {
            fprintf(stdout, "%d: ", getpid());
            fputs(ptr, stdout);
        }
    }

    printf("------------\n");
    FILE* f = fopen("netlist.txt", "r");
    if (NULL != f) {
        fgets(buffer, sizeof(buffer), f);
        fputs(buffer, stdout);
    }
    fclose(f);

    return 0;
}

