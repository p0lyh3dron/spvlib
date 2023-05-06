/*
 *    spvlib.c    --    Source file for SPV library
 *
 *    Authored by Karl "p0lyh3dron" Kreuze on May 5th, 2023.
 * 
 *    This file is part of the SPV library.
 * 
 *    This file defines the functions and types used by the SPV library.
 */
#include "spvlib.h"

#define _OP_NOP         0
#define _OP_NAME        5
#define _OP_MEMBER_NAME 6

#define _PARSE(buf, type, pos) (*(type *)(buf + pos)); pos += sizeof(type)

#include <stdlib.h>

const char *spv_current_error = "";

void (*spv_error_callback)(const char *) = (void (*)(const char *))0x0;

/*
 *    Sets the error callback function.
 *
 *    @param void (*callback)(const char *)    The callback function to set.
 */
void spv_set_error_callback(void (*callback)(const char *)) {
    spv_error_callback = callback;
}

/*
 *    Gets the last error message.
 *
 *    @return const char *    The last error message.
 */
const char *spv_get_last_error() {
    return spv_current_error;
}

/*
 *    Parses spirv binary data into a spv_t struct.
 *
 *    @param const char *data    The spirv binary data to parse.
 * 
 *    @return spv_t *            A pointer to the parsed spirv data.
 */
spv_t *spv_parse(const char *data) {
    spv_t *spv = (spv_t *)malloc(sizeof(spv_t));

    unsigned long pos = 0;

    unsigned int magic      = _PARSE(data, unsigned int, pos);
    unsigned int version    = _PARSE(data, unsigned int, pos);
    unsigned int generator  = _PARSE(data, unsigned int, pos);
    unsigned int bound      = _PARSE(data, unsigned int, pos);
    unsigned int schema     = _PARSE(data, unsigned int, pos);

    if (magic != 0x07230203) {
        free(spv);

        spv_current_error = "Invalid magic number.";
        if (spv_error_callback)
            spv_error_callback(spv_current_error);

        return (spv_t *)0x0;
    }

    do {
        unsigned short opcode     = _PARSE(data, unsigned short, pos);
        unsigned short word_count = _PARSE(data, unsigned short, pos);

        switch (opcode) {
            case _OP_NAME: {
                unsigned int id = _PARSE(data, unsigned int, pos);
                printf("Name: %s\n", data + pos);

                for (unsigned short i = 0; i < word_count - 2; i++) {
                    _PARSE(data, unsigned int, pos);
                }
            } break;

            case _OP_MEMBER_NAME: {
                unsigned int id     = _PARSE(data, unsigned int, pos);
                unsigned int member = _PARSE(data, unsigned int, pos);
                printf("Member Name: %s\n", data + pos);

                for (unsigned short i = 0; i < word_count - 3; i++) {
                    _PARSE(data, unsigned int, pos);
                }
            } break;

            default: {
                printf("Unknown opcode: %d\n", opcode);
                for (unsigned short i = 0; i < word_count - 1; i++) {
                    _PARSE(data, unsigned int, pos);
                }
            } break;
        }
    } while(data[pos] != (char)0x0);

    return spv;
}

/*
 *    Frees the memory allocated by spv_parse.
 *
 *    @param spv_t *spv    The spv_t struct to free.
 */
void spv_free(spv_t *spv) {

}