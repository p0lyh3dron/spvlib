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

#define _OP_NOP                0
#define _OP_NAME               5
#define _OP_MEMBER_NAME        6
#define _OP_TYPE_FLOAT         22
#define _OP_TYPE_VECTOR        23
#define _OP_TYPE_IMAGE         25
#define _OP_TYPE_SAMPLED_IMAGE 27
#define _OP_TYPE_POINTER       32
#define _OP_VARIABLE           59
#define _OP_DECORATE           71
#define _OP_MEMBER_DECORATE    72

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
 *    Parses spirv binary data into a spv_t struct.
 *
 *    @param const char *data    The spirv binary data to parse.
 * 
 *    @return spv_t *            A pointer to the parsed spirv data.
 */
spv_t *spv_parse(const char *data) {
    spv_t *spv = (spv_t *)malloc(sizeof(spv_t));

    spv->types            = (_type_t *)0x0;
    spv->types_size       = 0;
    spv->variables        = (_variable_t *)0x0;
    spv->variables_size   = 0;
    spv->constants        = (_constant_t *)0x0;
    spv->constants_size   = 0;
    spv->decorations      = (_decoration_t *)0x0;
    spv->decorations_size = 0;
    spv->names            = (_name_t *)0x0;
    spv->names_size       = 0;

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
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
            case 29:
            case 31:
            case 32: {
                _type_t *type = (_type_t *)realloc(spv->types, sizeof(_type_t) * (spv->types_size + 1));

                if (type == (_type_t *)0x0) {
                    free(spv);

                    spv_current_error = "Failed to allocate memory for type.";
                    if (spv_error_callback != (void (*)(const char *))0x0)
                        spv_error_callback(spv_current_error);

                    return (spv_t *)0x0;
                }

                spv->types = type;

                memcpy((char *)&spv->types[spv->types_size] + 4, data + pos, (word_count - 1) * sizeof(unsigned int));

                spv->types[spv->types_size].type = opcode;

                spv->types_size++;

                pos += (word_count - 1) * sizeof(unsigned int);
            } break;

            case 30: {
                _type_t *type = (_type_t *)realloc(spv->types, sizeof(_type_t) * (spv->types_size + 1));

                if (type == (_type_t *)0x0) {
                    free(spv);

                    spv_current_error = "Failed to allocate memory for type.";
                    if (spv_error_callback != (void (*)(const char *))0x0)
                        spv_error_callback(spv_current_error);

                    return (spv_t *)0x0;
                }

                spv->types = type;

                spv->types[spv->types_size].id                       = _PARSE(data, unsigned int, pos);
                spv->types[spv->types_size].type                     = opcode;
                spv->types[spv->types_size].struct_type.member_count = word_count - 2;
                spv->types[spv->types_size].struct_type.member_types = (unsigned int *)malloc(sizeof(unsigned int) * (word_count - 2));

                if (spv->types[spv->types_size].struct_type.member_types == (unsigned int *)0x0) {
                    free(spv);

                    spv_current_error = "Failed to allocate memory for type.";
                    if (spv_error_callback != (void (*)(const char *))0x0)
                        spv_error_callback(spv_current_error);

                    return (spv_t *)0x0;
                }

                for (unsigned short i = 0; i < word_count - 2; i++) {
                    spv->types[spv->types_size].struct_type.member_types[i] = _PARSE(data, unsigned int, pos);
                }

                spv->types_size++;

            } break;

            case 43: {
                _constant_t *constant = (_constant_t *)realloc(spv->constants, sizeof(_constant_t) * (spv->constants_size + 1));

                if (constant == (_constant_t *)0x0) {
                    free(spv);

                    spv_current_error = "Failed to allocate memory for constant.";
                    if (spv_error_callback != (void (*)(const char *))0x0)
                        spv_error_callback(spv_current_error);

                    return (spv_t *)0x0;
                }

                spv->constants = constant;

                spv->constants[spv->constants_size].result = _PARSE(data, unsigned int, pos);
                spv->constants[spv->constants_size].type   = _PARSE(data, unsigned int, pos);
                spv->constants[spv->constants_size].value  = _PARSE(data, unsigned int, pos);

                pos += (word_count - 4) * sizeof(unsigned int);

                spv->constants_size++;
            } break;

            case 71: {
                _decoration_t *decoration = (_decoration_t *)realloc(spv->decorations, sizeof(_decoration_t) * (spv->decorations_size + 1));

                if (decoration == (_decoration_t *)0x0) {
                    free(spv);

                    spv_current_error = "Failed to allocate memory for decoration.";
                    if (spv_error_callback != (void (*)(const char *))0x0)
                        spv_error_callback(spv_current_error);

                    return (spv_t *)0x0;
                }

                spv->decorations = decoration;

                spv->decorations[spv->decorations_size].result     = _PARSE(data, unsigned int, pos);
                spv->decorations[spv->decorations_size].decoration = _PARSE(data, unsigned int, pos);

                if (word_count > 3) {
                    spv->decorations[spv->decorations_size].value      = _PARSE(data, unsigned int, pos);
                }

                spv->decorations_size++;
            } break;

            case _OP_VARIABLE: {
                _variable_t *variable = (_variable_t *)realloc(spv->variables, sizeof(_variable_t) * (spv->variables_size + 1));

                if (variable == (_variable_t *)0x0) {
                    free(spv);

                    spv_current_error = "Failed to allocate memory for variable.";
                    if (spv_error_callback != (void (*)(const char *))0x0)
                        spv_error_callback(spv_current_error);

                    return (spv_t *)0x0;
                }

                spv->variables = variable;

                spv->variables[spv->variables_size].result        = _PARSE(data, unsigned int, pos);
                spv->variables[spv->variables_size].id            = _PARSE(data, unsigned int, pos);
                spv->variables[spv->variables_size].storage_class = _PARSE(data, unsigned int, pos);

                if (word_count > 4) {
                    spv->variables[spv->variables_size].initializer   = _PARSE(data, unsigned int, pos);
                }

                spv->variables_size++;
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
 *    Prints the value of a constant.
 *
 *    @param spv_t *spv         The spv_t struct to use.
 *    @param unsigned int id    The id of the constant.
 */
void spv_print_constant(spv_t *spv, unsigned int id) {
    for (unsigned long j = 0; j < spv->constants_size; ++j) {
        if (spv->constants[j].type == id) {
            printf("%u", spv->constants[j].value);
        }
    }
}

/*
 *    Prints the type of a variable.
 *
 *    @param spv_t *spv         The spv_t struct to use.
 *    @param unsigned int id    The id of the variable.
 */
void spv_print_type(spv_t *spv, unsigned int id) {
    for (unsigned long j = 0; j < spv->types_size; ++j) {
        if (spv->types[j].id == id) {
            switch (spv->types[j].type) {
                case _TYPE_FLOAT: {
                    printf("float");
                } break;

                case _TYPE_VECTOR: {
                    printf("vec%u", spv->types[j].vector_type.component_count);
                } break;

                case _TYPE_IMAGE: {
                    printf("image");
                } break;

                case _TYPE_SAMPLED_IMAGE: {
                    printf("sampler");
                } break;

                case _TYPE_POINTER: {
                    spv_print_type(spv, spv->types[j].pointer_type.type);
                    printf("*");
                } break;

                case _TYPE_ARRAY: {
                    spv_print_type(spv, spv->types[j].array_type.element_type);
                    printf("[");
                    spv_print_constant(spv, spv->types[j].array_type.length);
                    printf("]");
                } break;

                case _TYPE_STRUCT: {
                    printf("struct { ");

                    for (unsigned short i = 0; i < spv->types[j].struct_type.member_count; ++i) {
                        spv_print_type(spv, spv->types[j].struct_type.member_types[i]);
                        printf(" ");
                    }

                    printf("}");
                } break;

                default: {
                    printf("unknown");
                } break;
            }
        }
    }
}

/*
 *    Gets the number of inputs in a spv_t struct.
 *
 *    @param spv_t *spv    The spv_t struct to use.
 * 
 *    @return unsigned int    The number of inputs.
 */
unsigned int spv_get_input_count(spv_t *spv) {
    unsigned int count = 0;

    for (unsigned long i = 0; i < spv->variables_size; ++i) {
        if (spv->variables[i].storage_class == 0x1) {
            count++;
        }
    }

    return count;
}

/*
 *    Returns the api type of an input.
 *
 *    @param spv_t *spv         The spv_t struct to use.
 *    @param unsigned int id    The id of the input.
 * 
 *    @return _api_type_e    The api type of the input.
 */
_api_type_e spv_get_input_type(spv_t *spv, unsigned int id) {
    unsigned long pos = 0;

    for (unsigned long i = 0; i < spv->variables_size; ++i) {
        if (spv->variables[i].storage_class == 0x1) {
            if (pos != id) {
                pos++;
                continue;
            }

            _type_t type;

            for (unsigned long j = 0; j < spv->types_size; ++j) {
                if (spv->types[j].id == spv->variables[i].result) {
                    type = spv->types[j];
                    break;
                }
            }

            if (type.type == _TYPE_POINTER) {
                for (unsigned long j = 0; j < spv->types_size; ++j) {
                    if (spv->types[j].id == type.pointer_type.type) {
                        type = spv->types[j];
                        break;
                    }
                }
            }

            switch (type.type) {
                case _TYPE_FLOAT: {
                    return _API_TYPE_FLOAT;
                } break;

                case _TYPE_VECTOR: {
                    switch (type.vector_type.component_count) {
                        case 2: {
                            return _API_TYPE_VEC2;
                        } break;

                        case 3: {
                            return _API_TYPE_VEC3;
                        } break;

                        case 4: {
                            return _API_TYPE_VEC4;
                        } break;
                    }
                } break;
            }
        }
    }

    return _API_TYPE_FLOAT;
}

/*
 *    Gets the number of uniform declarations in a spv_t struct.
 *
 *    @param spv_t *spv    The spv_t struct to use.
 *
 *    @return unsigned int    The number of uniform declarations.
 */
unsigned int spv_get_uniform_count(spv_t *spv) {
    unsigned int count = 0;

    for (unsigned long i = 0; i < spv->variables_size; ++i) {
        if (spv->variables[i].storage_class == 0x0) {
            count++;
        }
    }

    return count;
}

/*
 *    Returns the api type of a uniform declaration.
 *
 *    @param spv_t *spv         The spv_t struct to use.
 *    @param unsigned int id    The id of the uniform declaration.
 *
 *    @return _api_type_e    The api type of the uniform declaration.
 */
_api_type_e spv_get_uniform_type(spv_t *spv, unsigned int id) {
    unsigned long pos = 0;

    for (unsigned long i = 0; i < spv->variables_size; ++i) {
        if (spv->variables[i].storage_class == 0x0 || spv->variables[i].storage_class == 0x2) {
            if (pos != id) {
                pos++;
                continue;
            }

            _type_t type;

            for (unsigned long j = 0; j < spv->types_size; ++j) {
                if (spv->types[j].id == spv->variables[i].result) {
                    type = spv->types[j];
                    break;
                }
            }

            if (type.type == _TYPE_POINTER) {
                for (unsigned long j = 0; j < spv->types_size; ++j) {
                    if (spv->types[j].id == type.pointer_type.type) {
                        type = spv->types[j];
                        break;
                    }
                }
            }

            for (unsigned int j = 0; j < spv->decorations_size; ++j) {
                if (spv->decorations[j].result == spv->variables[i].result) {
                    if (spv->decorations[j].decoration == 3) {
                        return _API_TYPE_STORAGE_BUFFER;
                    }

                    if (spv->decorations[j].decoration == 2) {
                        return _API_TYPE_UNIFORM_BUFFER;
                    }
                }
            }

            if (type.type == _TYPE_SAMPLED_IMAGE) {
                return _API_TYPE_SAMPLER;
            }
        }
    }

    return _API_TYPE_NONE;
}

/*
 *    Dumps the information about the inputs, outputs, and uniforms in a spv_t struct.
 *
 *    @param spv_t *spv    The spv_t struct to dump.
 */
void spv_dump(spv_t *spv) {
    for (unsigned long i = 0; i < spv->variables_size; ++i) {
        if (spv->variables[i].storage_class == 0x0) {
            printf("uniformconstant ");
            spv_print_type(spv, spv->variables[i].result);
            printf(";\n");
        } else if (spv->variables[i].storage_class == 0x1) {
            printf("in ");
            spv_print_type(spv, spv->variables[i].result);
            printf(";\n");
        } else if (spv->variables[i].storage_class == 0x2) {
            printf("uniform ");
            spv_print_type(spv, spv->variables[i].result);
            printf(";\n");
        } else if (spv->variables[i].storage_class == 0x3) {
            printf("out ");
            spv_print_type(spv, spv->variables[i].result);
            printf(";\n");
        } else if (spv->variables[i].storage_class == 0x9) {
            printf("pushconstant ");
            spv_print_type(spv, spv->variables[i].result);
            printf(";\n");
        }
    }
}

/*
 *    Frees the memory allocated by spv_parse.
 *
 *    @param spv_t *spv    The spv_t struct to free.
 */
void spv_free(spv_t *spv) {
    free(spv->types);
    free(spv);
}