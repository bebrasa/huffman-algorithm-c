#include <stdio.h>
#include "code.h"
#include "decode.h"
int main(int argc, char *argv[]) {
    char *action = argv[1];
    char *inputName = argv[2];
    char *outputName = argv[3];

    FILE *outbmp;
    FILE *output = fopen(outputName, "w+");

    if (strcmp(action, "c") == 0)
    {
        Code(inputName, output);
    }
    else if (strcmp(action, "d") == 0)
    {
        Decode(inputName, output);
    }
    fclose(output);

    return 0;
}
