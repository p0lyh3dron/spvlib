/*
 *    example.c    --    Source file for example program
 *
 *    Authored by Karl "p0lyh3dron" Kreuze on May 5th, 2023.
 */
#include <stdio.h>

#include "spvlib.h"

int main() {
    spv_set_error_callback((void (*)(const char *))0x0);

    FILE *file = fopen("example.spv", "rb");

    fseek(file, 0, SEEK_END);

    unsigned long size = ftell(file);

    fseek(file, 0, SEEK_SET);

    char *data = (char *)malloc(size);

    fread(data, 1, size, file);

    fclose(file);

    spv_t *spv = spv_parse(data);

    spv_free(spv);

    free(data);

    return 0;
}