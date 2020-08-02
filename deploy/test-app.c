//#!gcc test-app.c -o test-app -g -rdynamic
#include <stdio.h>
#include <unistd.h>

int main(int argc, const char* argv[])
{
    if (argc < 2) {
        printf("Missing parameter: test-app <mapper>|<reducer>");
        return -1;
    }
    char buffer[4096];
    while (!feof(stdin) && !ferror(stdin)) {
        const char* ptr = fgets(buffer, sizeof(buffer), stdin);
        if (NULL != ptr) {
            fprintf(stdout, "%s %d: ", argv[1], getpid());
            fputs(ptr, stdout);
        }
    }

    printf("------------\n");
    FILE* f = fopen("netlist.txt", "r");
    if (NULL != f) {
        fgets(buffer, sizeof(buffer), f);
        fputs(buffer, stdout);
        fclose(f);
    }

    return 0;
}

