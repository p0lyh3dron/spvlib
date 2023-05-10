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
#define _OP_TYPE_FLOAT      22
#define _OP_TYPE_VECTOR     23
#define _OP_TYPE_POINTER    32
#define _OP_VARIABLE        59
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
const char *spv_get_last_error(void) {
    return spv_current_error;
}

/*
 *    Adds a float to the spv_t struct.
 *
 *    @param spv_t *spv    The spv_t struct to add the float to.
 *    @param int id        The id of the float.
 *    @param int bits      The number of bits in the float.
 * 
 *    @return int          0 on success, -1 on failure.
 */
int spv_add_float(spv_t *spv, int id, int bits) {
    spv->floats = (_op_float_t *)realloc(spv->floats, sizeof(_op_float_t) * (spv->float_count + 1));

    if (spv->floats == (_op_float_t *)0x0) {
        spv_current_error = "Failed to allocate memory for float.";
        if (spv_error_callback != (void (*)(const char *))0x0)
            spv_error_callback(spv_current_error);

        return -1;
    }

    spv->floats[spv->float_count].id   = id;
    spv->floats[spv->float_count].bits = bits;

    spv->float_count++;

    return 0;
}

/*
 *    Adds a vec to the spv_t struct.
 *
 *    @param spv_t *spv    The spv_t struct to add the vec to.
 *    @param int id        The id of the vec.
 *    @param int float_id  The id of the float the vec is made of.
 * 
 *    @return int          0 on success, -1 on failure.
 */
int spv_add_vec(spv_t *spv, int id, int float_id) {
    spv->vecs = (_op_vec_t *)realloc(spv->vecs, sizeof(_op_vec_t) * (spv->vec_count + 1));

    if (spv->vecs == (_op_vec_t *)0x0) {
        spv_current_error = "Failed to allocate memory for vec.";
        if (spv_error_callback != (void (*)(const char *))0x0)
            spv_error_callback(spv_current_error);

        return -1;
    }

    spv->vecs[spv->vec_count].id       = id;
    spv->vecs[spv->vec_count].float_id = float_id;

    spv->vec_count++;

    return 0;
}

/*
 *    Adds a pointer to the spv_t struct.
 *
 *    @param spv_t *spv    The spv_t struct to add the pointer to.
 *    @param int id        The id of the pointer.
 *    @param int type_id   The id of the type of the pointer.
 * 
 *    @return int          0 on success, -1 on failure.
 */
int spv_add_ptr(spv_t *spv, int id, int type_id) {
    spv->ptrs = (_op_ptr_t *)realloc(spv->ptrs, sizeof(_op_ptr_t) * (spv->ptr_count + 1));

    if (spv->ptrs == (_op_ptr_t *)0x0) {
        spv_current_error = "Failed to allocate memory for pointer.";
        if (spv_error_callback != (void (*)(const char *))0x0)
            spv_error_callback(spv_current_error);

        return -1;
    }

    spv->ptrs[spv->ptr_count].id      = id;
    spv->ptrs[spv->ptr_count].type_id = type_id;

    spv->ptr_count++;

    return 0;
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
int spv_add_name(spv_t *spv, char *name, int id) {
    spv->names = (_op_name_t *)realloc(spv->names, sizeof(_op_name_t) * (spv->name_count + 1));

    if (spv->names == (_op_name_t *)0x0) {
        spv_current_error = "Failed to allocate memory for variable.";
        if (spv_error_callback != (void (*)(const char *))0x0)
            spv_error_callback(spv_current_error);

        return -1;
    }

    spv->names[spv->name_count].name = name;
    spv->names[spv->name_count].id   = id;

    spv->name_count++;

    return 0;
}

/*
 *    Adds a variable to the spv_t struct.
 *
 *    @param spv_t *spv    The spv_t struct to add the variable to.
 *    @param int ptr_id    The id of the pointer of the variable.
 *    @param int id        The id of the variable.
 *    @param int storage   The storage of the variable.
 *
 *    @return int          0 on success, -1 on failure.
 */
int spv_add_var(spv_t *spv, int ptr_id, int id, int storage) {
    spv->vars = (_op_var_t *)realloc(spv->vars, sizeof(_op_var_t) * (spv->var_count + 1));

    if (spv->vars == (_op_var_t *)0x0) {
        spv_current_error = "Failed to allocate memory for variable.";
        if (spv_error_callback != (void (*)(const char *))0x0)
            spv_error_callback(spv_current_error);

        return -1;
    }

    spv->vars[spv->var_count].ptr_id  = ptr_id;
    spv->vars[spv->var_count].id      = id;
    spv->vars[spv->var_count].storage = storage;

    spv->var_count++;

    return 0;
}

/*
 *    Returns a float from a given id.
 *
 *    @param spv_t *spv    The spv_t struct to get the float from.
 *    @param int id        The id of the float to get.
 * 
 *    @return _op_float_t  A pointer to the float.
 */
_op_float_t *spv_get_float(spv_t *spv, int id) {
    for (unsigned long i = 0; i < spv->float_count; i++)
        if (spv->floats[i].id == id)
            return &spv->floats[i];

    return (_op_float_t *)0x0;
}

/*
 *    Returns a vec from a given id.
 *
 *    @param spv_t *spv    The spv_t struct to get the vec from.
 *    @param int id        The id of the vec to get.
 * 
 *    @return _op_vec_t *  A pointer to the vec.
 */
_op_vec_t *spv_get_vec(spv_t *spv, int id) {
    for (unsigned long i = 0; i < spv->vec_count; i++)
        if (spv->vecs[i].id == id)
            return &spv->vecs[i];

    return (_op_vec_t *)0x0;
}

/*
 *    Returns a pointer from a given id.
 *
 *    @param spv_t *spv    The spv_t struct to get the pointer from.
 *    @param int id        The id of the pointer to get.
 * 
 *    @return _op_ptr_t *  A pointer to the pointer.
 */
_op_ptr_t *spv_get_ptr(spv_t *spv, int id) {
    for (unsigned long i = 0; i < spv->ptr_count; i++)
        if (spv->ptrs[i].id == id)
            return &spv->ptrs[i];

    return (_op_ptr_t *)0x0;
}

/*
 *    Returns a variable from a given id.
 *
 *    @param spv_t *spv    The spv_t struct to get the variable from.
 *    @param int id        The id of the variable to get.
 * 
 *    @return _op_name_t *  A pointer to the variable.
 */
_op_name_t *spv_get_name(spv_t *spv, int id) {
    for (unsigned long i = 0; i < spv->name_count; i++)
        if (spv->names[i].id == id)
            return &spv->names[i];

    return (_op_name_t *)0x0;
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

    spv->float_count = 0;
    spv->floats      = (_op_float_t *)malloc(sizeof(_op_float_t) * 1);
    spv->vec_count   = 0;
    spv->vecs        = (_op_vec_t *)malloc(sizeof(_op_vec_t) * 1);
    spv->ptr_count   = 0;
    spv->ptrs        = (_op_ptr_t *)malloc(sizeof(_op_ptr_t) * 1);
    spv->name_count  = 0;
    spv->names       = (_op_name_t *)malloc(sizeof(_op_name_t) * 1);
    spv->var_count   = 0;
    spv->vars        = (_op_var_t *)malloc(sizeof(_op_var_t) * 1);

    if (spv->names == (_op_name_t *)0x0) {
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

                spv_add_name(spv, (char *)(data + pos), id);

                for (unsigned short i = 0; i < word_count - 2; i++) {
                    _PARSE(data, unsigned int, pos);
                }
            } break;

            case _OP_MEMBER_NAME: {
                unsigned int id     = _PARSE(data, unsigned int, pos);
                unsigned int member = _PARSE(data, unsigned int, pos);

                spv_add_name(spv, (char *)(data + pos), id);

                for (unsigned short i = 0; i < word_count - 3; i++) {
                    _PARSE(data, unsigned int, pos);
                }
            } break;

            case _OP_TYPE_FLOAT: {
                unsigned int id   = _PARSE(data, unsigned int, pos);
                unsigned int bits = _PARSE(data, unsigned int, pos);

                spv_add_float(spv, id, bits);

                for (unsigned short i = 0; i < word_count - 3; i++) {
                    _PARSE(data, unsigned int, pos);
                }
            } break;

            case _OP_TYPE_VECTOR: {
                unsigned int id      = _PARSE(data, unsigned int, pos);
                unsigned int floatid = _PARSE(data, unsigned int, pos);
                unsigned int count   = _PARSE(data, unsigned int, pos);

                spv_add_vec(spv, id, floatid);

                for (unsigned short i = 0; i < word_count - 4; i++) {
                    _PARSE(data, unsigned int, pos);
                }
            } break;

            case _OP_TYPE_POINTER: {
                unsigned int id      = _PARSE(data, unsigned int, pos);
                unsigned int storage = _PARSE(data, unsigned int, pos);
                unsigned int type    = _PARSE(data, unsigned int, pos);

                spv_add_ptr(spv, id, type);

                for (unsigned short i = 0; i < word_count - 4; i++) {
                    _PARSE(data, unsigned int, pos);
                }
            } break;

            case _OP_VARIABLE: {
                unsigned int type    = _PARSE(data, unsigned int, pos);
                unsigned int id      = _PARSE(data, unsigned int, pos);
                unsigned int storage = _PARSE(data, unsigned int, pos);

                spv_add_var(spv, type, id, storage);

                for (unsigned short i = 0; i < word_count - 4; i++) {
                    _PARSE(data, unsigned int, pos);
                }
            } break;

            case _OP_DECORATE: {
                unsigned int id = _PARSE(data, unsigned int, pos);
                unsigned int dec = _PARSE(data, unsigned int, pos);
                switch (dec) {
                    case _DEC_BLOCK: {
                    } break;

                    case _DEC_BUILTIN: {
                        unsigned int builtin = _PARSE(data, unsigned int, pos);

                        word_count--;
                    } break;

                    case _DEC_LOCATION: {
                        unsigned int location = _PARSE(data, unsigned int, pos);

                        word_count--;
                    } break;

                    case _DEC_OFFSET: {
                        unsigned int offset = _PARSE(data, unsigned int, pos);

                        word_count--;
                    } break;

                    default: {
                    } break;
                }

                for (unsigned short i = 0; i < word_count - 3; i++) {
                    _PARSE(data, unsigned int, pos);
                }
            } break;

            case _OP_MEMBER_DECORATE: {
                unsigned int id     = _PARSE(data, unsigned int, pos);
                unsigned int member = _PARSE(data, unsigned int, pos);
                unsigned int dec    = _PARSE(data, unsigned int, pos);

                switch (dec) {
                    case _DEC_BLOCK: {
                    } break;

                    case _DEC_BUILTIN: {
                        unsigned int builtin = _PARSE(data, unsigned int, pos);

                        word_count--;
                    } break;

                    case _DEC_LOCATION: {
                        unsigned int location = _PARSE(data, unsigned int, pos);

                        word_count--;
                    } break;

                    case _DEC_OFFSET: {
                        unsigned int offset = _PARSE(data, unsigned int, pos);

                        word_count--;
                    } break;

                    default: {
                    } break;
                }

                for (unsigned short i = 0; i < word_count - 4; i++) {
                    _PARSE(data, unsigned int, pos);
                }
            } break;

            default: {
                for (unsigned short i = 0; i < word_count - 1; i++) {
                    _PARSE(data, unsigned int, pos);
                }
            } break;
        }
    } while(data[pos] != (char)0x0);

    return spv;
}

/*
 *    Dumps the information about the inputs, outputs, and uniforms in a spv_t struct.
 *
 *    @param spv_t *spv    The spv_t struct to dump.
 */
void spv_dump(spv_t *spv) {
    for (unsigned long i = 1; i < spv->var_count; ++i) {
        _op_var_t *var = &spv->vars[i];

        _op_ptr_t *ptr = spv_get_ptr(spv, var->ptr_id);

        _op_name_t *name = spv_get_name(spv, var->id);

        _op_vec_t *vec = spv_get_vec(spv, ptr->type_id);

        if (var->storage == _OP_VAR_STORAGE_INPUT || var->storage == _OP_VAR_STORAGE_OUTPUT) {
            char buf[256];

            if (vec != (_op_vec_t *)0x0) {
                _op_float_t *float_ = spv_get_float(spv, vec->float_id);

                sprintf(buf, "Variable \"%s\" of type vec%d going %s\n", name->name, float_->bits / 8, var->storage == _OP_VAR_STORAGE_INPUT ? "in" : "out");
            } else {
                sprintf(buf, "Variable \"%s\" of type unknown going %s\n", name->name, var->storage == _OP_VAR_STORAGE_INPUT ? "in" : "out");
            }

            printf("%s", buf);
        }
    }
}

/*
 *    Frees the memory allocated by spv_parse.
 *
 *    @param spv_t *spv    The spv_t struct to free.
 */
void spv_free(spv_t *spv) {
    free(spv->names);
    free(spv->ptrs);
    free(spv->vecs);
    free(spv->floats);
    free(spv);
}