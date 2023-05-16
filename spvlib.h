/*
 *    spvlib.h    --    Header file for SPV library
 *
 *    Authored by Karl "p0lyh3dron" Kreuze on May 5th, 2023.
 * 
 *    This file is part of the SPV library.
 * 
 *    This file declares the functions and types used by the SPV library.
 */
#ifndef _SPVLIB_H
#define _SPVLIB_H

typedef struct  {
    unsigned int id;
    unsigned int bits;
} _op_float_t;

typedef struct {
    unsigned int id;
    unsigned int float_id;
} _op_vec_t;

typedef struct {
    unsigned int id;
    unsigned int samp_id;
    unsigned int dim;
    unsigned int depth;
    unsigned int arrayed;
    unsigned int ms;
    unsigned int sampled;
    unsigned int img_format;
    unsigned int aq;
} _op_image_t;

typedef struct {
    unsigned int id;
    unsigned int type;
} _op_sampled_image_t;

typedef struct {
    unsigned int id;
    unsigned int type_id;
} _op_ptr_t;

typedef enum {
    _OP_VAR_TYPE_FLOAT,
    _OP_VAR_TYPE_VEC2,
    _OP_VAR_TYPE_VEC3,
    _OP_VAR_TYPE_VEC4,
} _op_var_type_e;

typedef enum {
    _OP_VAR_STORAGE_UNIFORMCONSTANT = 0,
    _OP_VAR_STORAGE_INPUT           = 1,
    _OP_VAR_STORAGE_UNIFORM         = 2,
    _OP_VAR_STORAGE_OUTPUT          = 3,
} _op_var_storage_e;

typedef struct {
    char          *name;
    unsigned int   id;
} _op_name_t;

typedef struct {
    unsigned int ptr_id;
    unsigned int id;
    unsigned int storage;
} _op_var_t;

typedef struct {
    _op_float_t         *floats;
    unsigned long        float_count;
    _op_vec_t           *vecs;
    unsigned long        vec_count;
    _op_image_t         *images;
    unsigned long        image_count;
    _op_sampled_image_t *sampled_images;
    unsigned long        sampled_image_count;
    _op_ptr_t           *ptrs;
    unsigned long        ptr_count;
    _op_name_t          *names;
    unsigned long        name_count;
    _op_var_t           *vars;
    unsigned long        var_count;
} spv_t;

/*
 *    Sets the error callback function.
 *
 *    @param void (*callback)(const char *)    The callback function to set.
 */
void spv_set_error_callback(void (*callback)(const char *));

/*
 *    Gets the last error message.
 *
 *    @return const char *    The last error message.
 */
const char *spv_get_last_error(void);

/*
 *    Parses spirv binary data into a spv_t struct.
 *
 *    @param const char *data    The spirv binary data to parse.
 * 
 *    @return spv_t *            A pointer to the parsed spirv data.
 */
spv_t *spv_parse(const char *data);

/*
 *    Dumps the information about the inputs, outputs, and uniforms in a spv_t struct.
 *
 *    @param spv_t *spv    The spv_t struct to dump.
 */
void spv_dump(spv_t *spv);

/*
 *    Frees the memory allocated by spv_parse.
 *
 *    @param spv_t *spv    The spv_t struct to free.
 */
void spv_free(spv_t *spv);

#endif /* _SPVLIB_H  */