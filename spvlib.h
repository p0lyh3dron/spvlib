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

typedef enum {
    _TYPE_VOID = 19,
    _TYPE_BOOL,
    _TYPE_INT,
    _TYPE_FLOAT,
    _TYPE_VECTOR,
    _TYPE_MATRIX,
    _TYPE_IMAGE,
    _TYPE_SAMPLER,
    _TYPE_SAMPLED_IMAGE,
    _TYPE_ARRAY,
    _TYPE_RUNTIME_ARRAY,
    _TYPE_STRUCT,
    _TYPE_OPAQUE,
    _TYPE_POINTER,
} _type_e;

typedef struct {

} _type_void_t;

typedef struct {
    
} _type_bool_t;

typedef struct {
    unsigned int width;
    unsigned int signedness;
} _type_int_t;

typedef struct {
    unsigned int width;
} _type_float_t;

typedef struct {
    unsigned int component_type;
    unsigned int component_count;
} _type_vector_t;

typedef struct {
    unsigned int column_type;
    unsigned int column_count;
} _type_matrix_t;

typedef struct {
    unsigned int sampled_type;
    unsigned int dim;
    unsigned int depth;
    unsigned int arrayed;
    unsigned int ms;
    unsigned int sampled;
    unsigned int image_format;
} _type_image_t;

typedef struct {
    
} _type_sampler_t;

typedef struct {
    unsigned int image_type;
} _type_sampled_image_t;

typedef struct {
    unsigned int element_type;
    unsigned int length;
} _type_array_t;

typedef struct {
    unsigned int element_type;
} _type_runtime_array_t;

typedef struct {
    unsigned short  member_count;
    unsigned int   *member_types;
} _type_struct_t;

typedef struct {
    unsigned int storage_class;
    unsigned int type;
} _type_pointer_t;

typedef struct {
    _type_e       type;
    unsigned int  id;
    union {
        _type_void_t          void_type;
        _type_bool_t          bool_type;
        _type_int_t           int_type;
        _type_float_t         float_type;
        _type_vector_t        vector_type;
        _type_matrix_t        matrix_type;
        _type_image_t         image_type;
        _type_sampler_t       sampler_type;
        _type_sampled_image_t sampled_image_type;
        _type_array_t         array_type;
        _type_runtime_array_t runtime_array_type;
        _type_struct_t        struct_type;
        _type_pointer_t       pointer_type;
    };
} _type_t;

typedef struct {
    unsigned int result;
    unsigned int type;
    unsigned int value;
} _constant_t;

typedef struct {
    unsigned int result;
    unsigned int id;
    unsigned int storage_class;
    unsigned int initializer;
} _variable_t;

typedef struct {
    _type_t       *types;
    unsigned long  types_size;
    _variable_t   *variables;
    unsigned long  variables_size;
    _constant_t   *constants;
    unsigned long  constants_size;
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