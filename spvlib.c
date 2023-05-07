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

#define _OP_NOP             0
#define _OP_NAME            5
#define _OP_MEMBER_NAME     6
#define _OP_DECORATE        71
#define _OP_MEMBER_DECORATE 72

#define _DEC_BLOCK          2
#define _DEC_BUILTIN        11
#define _DEC_LOCATION       30
#define _DEC_OFFSET         35

#define _PARSE(buf, type, pos) (*(type *)(buf + pos)); pos += sizeof(type)

#include <stdlib.h>
#include <stdio.h>

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
 *    Adds a variable to the spv_t struct.
 *
 *    @param spv_t *spv    The spv_t struct to add the variable to.
 *    @param char *name    The name of the variable.
 *    @param int id        The id of the variable.
 *
 *    @return int          0 on success, -1 on failure.
 */
int spv_add_var(spv_t *spv, char *name, int id) {
    spv->vars = (_op_var_t *)realloc(spv->vars, sizeof(_op_var_t) * (spv->var_count + 1));

    if (spv->vars == (_op_var_t *)0x0) {
        spv_current_error = "Failed to allocate memory for variable.";
        if (spv_error_callback != (void (*)(const char *))0x0)
            spv_error_callback(spv_current_error);

        return -1;
    }

    spv->vars[spv->var_count].name = name;
    spv->vars[spv->var_count].id   = id;

    spv->var_count++;

    return 0;
}

/*
 *    Returns a variable from a given id.
 *
 *    @param spv_t *spv    The spv_t struct to get the variable from.
 *    @param int id        The id of the variable to get.
 * 
 *    @return _op_var_t *  A pointer to the variable.
 */
_op_var_t *spv_get_var(spv_t *spv, int id) {
    for (unsigned long i = 0; i < spv->var_count; i++)
        if (spv->vars[i].id == id)
            return &spv->vars[i];

    return (_op_var_t *)0x0;
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

    spv->var_count = 0;
    spv->vars      = (_op_var_t *)malloc(sizeof(_op_var_t) * 1);

    if (spv->vars == (_op_var_t *)0x0) {
        free(spv);

        spv_current_error = "Failed to allocate memory for spv_t struct.";
        if (spv_error_callback != (void (*)(const char *))0x0)
            spv_error_callback(spv_current_error);

        return (spv_t *)0x0;
    }

    unsigned long pos = 0;

    unsigned int magic      = _PARSE(data, unsigned int, pos);
    unsigned int version    = _PARSE(data, unsigned int, pos);
    unsigned int generator  = _PARSE(data, unsigned int, pos);
    unsigned int bound      = _PARSE(data, unsigned int, pos);
    unsigned int schema     = _PARSE(data, unsigned int, pos);

    if (magic != 0x07230203) {
        free(spv);

        spv_current_error = "Invalid magic number.";
        if (spv_error_callback != (void (*)(const char *))0x0)
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

                spv_add_var(spv, (char *)(data + pos), id);

                for (unsigned short i = 0; i < word_count - 2; i++) {
                    _PARSE(data, unsigned int, pos);
                }
            } break;

            case _OP_MEMBER_NAME: {
                unsigned int id     = _PARSE(data, unsigned int, pos);
                unsigned int member = _PARSE(data, unsigned int, pos);
                printf("Member Name: %s\n", data + pos);

                spv_add_var(spv, (char *)(data + pos), id);

                for (unsigned short i = 0; i < word_count - 3; i++) {
                    _PARSE(data, unsigned int, pos);
                }
            } break;

            case _OP_DECORATE: {
                unsigned int id = _PARSE(data, unsigned int, pos);
                unsigned int dec = _PARSE(data, unsigned int, pos);
                printf("Decoration: %d\n", dec);

                switch (dec) {
                    case _DEC_BLOCK: {
                        printf("Block\n");
                    } break;

                    case _DEC_BUILTIN: {
                        unsigned int builtin = _PARSE(data, unsigned int, pos);
                        printf("Builtin: %d\n", builtin);

                        word_count--;
                    } break;

                    case _DEC_LOCATION: {
                        unsigned int location = _PARSE(data, unsigned int, pos);
                        printf("Location: %d\n", location);

                        word_count--;
                    } break;

                    case _DEC_OFFSET: {
                        unsigned int offset = _PARSE(data, unsigned int, pos);
                        printf("Offset: %d\n", offset);

                        word_count--;
                    } break;

                    default: {
                        printf("Unknown\n");
                    } break;
                }

                _op_var_t *var = spv_get_var(spv, id);

                printf("Variable: %s\n", var->name);

                for (unsigned short i = 0; i < word_count - 3; i++) {
                    _PARSE(data, unsigned int, pos);
                }
            } break;

            case _OP_MEMBER_DECORATE: {
                unsigned int id     = _PARSE(data, unsigned int, pos);
                unsigned int member = _PARSE(data, unsigned int, pos);
                unsigned int dec    = _PARSE(data, unsigned int, pos);
                printf("Member Decoration: %d\n", dec);

                switch (dec) {
                    case _DEC_BLOCK: {
                        printf("Block\n");
                    } break;

                    case _DEC_BUILTIN: {
                        unsigned int builtin = _PARSE(data, unsigned int, pos);
                        printf("Builtin: %d\n", builtin);

                        word_count--;
                    } break;

                    case _DEC_LOCATION: {
                        unsigned int location = _PARSE(data, unsigned int, pos);
                        printf("Location: %d\n", location);

                        word_count--;
                    } break;

                    case _DEC_OFFSET: {
                        unsigned int offset = _PARSE(data, unsigned int, pos);
                        printf("Offset: %d\n", offset);

                        word_count--;
                    } break;

                    default: {
                        printf("Unknown\n");
                    } break;
                }

                _op_var_t *var = spv_get_var(spv, id);

                printf("Variable: %s\n", var->name);

                for (unsigned short i = 0; i < word_count - 4; i++) {
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