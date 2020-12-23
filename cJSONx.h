/*
  Copyright (c) 2020 CodeBiang
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/
/**
 * cJSONx
 * Enhanced JSON converter based on cJSON
 * 
 * @version 1.0.1
 * @author  CodeBiang@163.com
 * @date    2020/10/20
 */
#ifndef __CJSONX_H_
#define __CJSONX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "cJSON.h"

#define CJSONX_VERSION_MAJOR        1
#define CJSONX_VERSION_MINOR        0
#define CJSONX_VERSION_PATCH        0

/* Debug info will output by this function `xx(fmt, ...)` */
/* e.g. #define CJSONX_DBG(...)  printf(__VA_ARGS__) */
/* Empty means that there is no debugging infomation output */
#define CJSONX_DBG(...)

#define CJSONX_ERR_CODE_MAP(XX)                         \
    XX(NONE, 0, "success")                              \
    XX(MEMORY, -1, "malloc failed")                     \
    XX(TYPE, -2, "type matching error")                 \
    XX(MISSING_FIELD, -3, "field not found")            \
    XX(FORMAT, -4, "input json string format error")    \
    XX(ARGS, -5, "args error")                          \
    XX(OVERFLOW, -6, "value overflow")

enum cjsonx_err_code {
#define XX(name, code, desc)    ERR_CJSONX_##name = code,
    CJSONX_ERR_CODE_MAP(XX)
#undef XX
};

/** Json Type Enum For Reflection **/
typedef enum cjsonx_type_e {
    CJSONX_OBJECT = 0,
    CJSONX_ARRAY,
    CJSONX_STRING,
    CJSONX_INTEGER,
    CJSONX_REAL,
    CJSONX_TRUE,
    CJSONX_FALSE,
    CJSONX_NULL
} cjsonx_type_e;

/** Json Reflection Extension **/
typedef struct cjsonx_annotation_t {
    /* when `nullalbe` is false, (de)serializing operation will not be termenated by missing field */
    /* default true */
    bool nullable;
    
    /* when `serialized` is false, field will be ignored when serializing */
    /* default true */
    bool serialized;

    /* when `deserialized` is false, field will be ignored when deserializing */
    /* default true */
    bool deserialized;

    /* field name mapping, if it's not NULL, real field name will be ignored */
    /* default NULL */
    const char* serialized_name;
} cjsonx_annotation_t;


/** Reflection **/
typedef struct cjsonx_reflect_t {
    const char* field;                      /* field name */
    size_t offset;                          /* field address offset */
    size_t size;                            /* field size */
    cjsonx_type_e type;                     /* json element type */

    /* item reflection of array item or buffered string */
    const struct cjsonx_reflect_t* reflection;

    /* array counter field name */
    /* notice! this field should not be reflected */
    const char* arr_count_field;

    /* array item size */
    size_t item_size;

    /* data will be created by `malloc` or `malloc or cJSON_Hooks` */
    bool constructed;

    /* expanded reflection */
    const cjsonx_annotation_t annotation;
} cjsonx_reflect_t;

/* Basic Structure reflection */
extern const cjsonx_reflect_t _cjsonx_reflect_int[];
extern const cjsonx_reflect_t _cjsonx_reflect_string_ptr[];
extern const cjsonx_reflect_t _cjsonx_reflect_string_bufferred[];
extern const cjsonx_reflect_t _cjsonx_reflect_real[];
extern const cjsonx_reflect_t _cjsonx_reflect_double[];
extern const cjsonx_reflect_t _cjsonx_reflect_float[];
extern const cjsonx_reflect_t _cjsonx_reflect_bool[];

/*** Private Definitions ***/

#define _cjsonx_offset(type, field)             (size_t)(&(((type*)0)->field))
#define _cjsonx_size(type, field)               (sizeof(((type*)0)->field))
#define _cjsonx_ptr_item_size(type, field)      (sizeof(*(((type*)0)->field)))

#define _reflect_ex_default                     {true, true, true, NULL}

/* Field Ex */
#define __cjsonx_ex(type, field, jtype, refelct, item_size, constructed)\
        {#field, _cjsonx_offset(type, field), _cjsonx_size(type, field),\
        jtype, refelct, NULL, item_size, constructed, {
/* Field */
#define __cjsonx(type, field, jtype, refelct, item_size, constructed)\
        {#field, _cjsonx_offset(type, field), _cjsonx_size(type, field),\
        jtype, refelct, NULL, item_size, constructed, _reflect_ex_default}

/* Pointer Field Ex */
#define __cjsonx_ptr_ex(type, field, jtype, refelct)\
        {#field, _cjsonx_offset(type, field), _cjsonx_size(type, field),\
        jtype, refelct, NULL, _cjsonx_ptr_item_size(type, field), true, {
/* Pointer Field */
#define __cjsonx_ptr(type, field, jtype, refelct)\
        {#field, _cjsonx_offset(type, field), _cjsonx_size(type, field),\
        jtype, refelct, NULL, _cjsonx_ptr_item_size(type, field), true, _reflect_ex_default}

/* Basic Ex */
#define __cjsonx_basic_ex(type, field, jtype, refelct)\
        __cjsonx_ex(type, field, jtype, refelct, 0, false)
/* Basic */
#define __cjsonx_basic(type, field, jtype, refelct)\
        __cjsonx(type, field, jtype, refelct, 0, false)

/* Array Ex */
#define __cjsonx_array_ex(type, field, item_count_field, refelct, constructed)\
        {#item_count_field, _cjsonx_offset(type, item_count_field), _cjsonx_size(type, item_count_field),\
        CJSONX_INTEGER, _cjsonx_reflect_int, NULL, 0, false, {0}},\
        {#field,\
        _cjsonx_offset(type, field),\
        _cjsonx_size(type, field),\
        CJSONX_ARRAY,\
        refelct,\
        _cjsonx_size(type, item_count_field) > 0 ? #item_count_field : NULL,\
        _cjsonx_ptr_item_size(type, field),\
        constructed, {

/* Pointer Array Ex */
#define __cjsonx_array_ptr_ex(type, field, item_count_field, refelct)\
    __cjsonx_array_ex(type, field, item_count_field, refelct, true)

/* Bufferred Array Ex */
#define __cjsonx_array_bufferred_ex(type, field, item_count_field, refelct)\
    __cjsonx_array_ex(type, field, item_count_field, refelct, false)

/* Array */
#define __cjsonx_array(type, field, item_count_field, refelct, constructed)\
        {#field,\
        _cjsonx_offset(type, field),\
        _cjsonx_size(type, field),\
        CJSONX_ARRAY,\
        refelct,\
        _cjsonx_size(type, item_count_field) > 0 ? #item_count_field : NULL,\
        _cjsonx_ptr_item_size(type, field),\
        constructed,\
        _reflect_ex_default},\
        {#item_count_field, _cjsonx_offset(type, item_count_field), _cjsonx_size(type, item_count_field),\
        CJSONX_INTEGER, _cjsonx_reflect_int, NULL, 0, false, {0}}

/* Pointer Array */
#define __cjsonx_array_ptr(type, field, item_count_field, refelct)\
        __cjsonx_array(type, field, item_count_field, refelct, true)

/* Bufferred Array */
#define __cjsonx_array_bufferred(type, field, item_count_field, refelct)\
        __cjsonx_array(type, field, item_count_field, refelct, false)

/* End */
#define __cjsonx_end()\
        {NULL, 0, 0, CJSONX_NULL, NULL, NULL, 0, false, {0}}



/*** Public Definitions ****/
#define __nullable                                  .nullable = true,
#define __deserialized                              .deserialized = true,
#define __serialized                                .serialized = true,
#define __serialized_name(__name)                   .serialized_name = __name, 
#define __cjsonx_ex_end                              }}

/** Basic Reflections **/
#define __cjsonx_int(type, field)               __cjsonx_basic(type, field, CJSONX_INTEGER, _cjsonx_reflect_int)
#define __cjsonx_real(type, field)              __cjsonx_basic(type, field, CJSONX_REAL, _cjsonx_reflect_real)
#define __cjsonx_bool(type, field)              __cjsonx_basic(type, field, CJSONX_TRUE, _cjsonx_reflect_bool)
#define __cjsonx_str_bufferred(type, field)     __cjsonx(type, field, CJSONX_STRING, _cjsonx_reflect_string_bufferred, sizeof(char), false)
#define __cjsonx_str_ptr(type, field)           __cjsonx(type, field, CJSONX_STRING, _cjsonx_reflect_string_ptr, sizeof(char), true)
#define __cjsonx_str(type, field)               __cjsonx_str_bufferred(type, field)
#define __cjsonx_object(type, field, refelct) __cjsonx_basic(type, field, CJSONX_OBJECT, refelct)
#define __cjsonx_object_ptr(type, field, refelct)\
        {#field, _cjsonx_offset(type, field), _cjsonx_size(type, field),\
        CJSONX_OBJECT, refelct, NULL, _cjsonx_ptr_item_size(type, field), true, _reflect_ex_default}

/** Array Pointer Reflections **/
#define __cjsonx_array_ptr_int(type, field, item_count_field)\
        __cjsonx_array_ptr(type, field, item_count_field, _cjsonx_reflect_int)
#define __cjsonx_array_ptr_real(type, field, item_count_field)\
        __cjsonx_array_ptr(type, field, item_count_field, _cjsonx_reflect_real)
#define __cjsonx_array_ptr_bool(type, field, item_count_field)\
        __cjsonx_array_ptr(type, field, item_count_field, _cjsonx_reflect_bool)
#define __cjsonx_array_ptr_str_ptr(type, field, item_count_field)\
        __cjsonx_array_ptr(type, field, item_count_field, _cjsonx_reflect_string_ptr)
#define __cjsonx_array_ptr_object(type, field, item_count_field, refelct)\
        __cjsonx_array_ptr(type, field, item_count_field, refelct)

/** Array(Bufferred) Reflections **/
#define __cjsonx_array_int(type, field, item_count_field)\
        __cjsonx_array_bufferred(type, field, item_count_field, _cjsonx_reflect_int)
#define __cjsonx_array_real(type, field, item_count_field)\
        __cjsonx_array_bufferred(type, field, item_count_field, _cjsonx_reflect_real)
#define __cjsonx_array_bool(type, field, item_count_field)\
        __cjsonx_array_bufferred(type, field, item_count_field, _cjsonx_reflect_bool)
#define __cjsonx_array_str(type, field, item_count_field)\
        __cjsonx_array_bufferred(type, field, item_count_field, _cjsonx_reflect_string_bufferred)
#define __cjsonx_array_str_ptr(type, field, item_count_field)\
        __cjsonx_array_bufferred(type, field, item_count_field, _cjsonx_reflect_string_ptr)
#define __cjsonx_array_object(type, field, item_count_field, refelct)\
        __cjsonx_array_bufferred(type, field, item_count_field, refelct)


/** Basic Reflections with annotations **/
#define __cjsonx_int_ex(type, field)\
        __cjsonx_basic_ex(type, field, CJSONX_INTEGER, _cjsonx_reflect_int)
#define __cjsonx_real_ex(type, field)\
        __cjsonx_basic_ex(type, field, CJSONX_REAL, _cjsonx_reflect_real)
#define __cjsonx_bool_ex(type, field)\
        __cjsonx_basic_ex(type, field, CJSONX_TRUE, _cjsonx_reflect_bool)
#define __cjsonx_object_ex(type, field, refelct)\
        __cjsonx_basic_ex(type, field, CJSONX_OBJECT, refelct)
#define __cjsonx_object_ptr_ex(type, field, refelct)\
        {#field, _cjsonx_offset(type, field), _cjsonx_size(type, field),\
        CJSONX_OBJECT, refelct, NULL, _cjsonx_ptr_item_size(type, field), true, {

/** Array Pointer Reflections with Annotations **/
#define __cjsonx_array_ptr_int_ex(type, field, item_count_field)\
        __cjsonx_array_ptr_ex(type, field, item_count_field, _cjsonx_reflect_int)
#define __cjsonx_array_ptr_real_ex(type, field, item_count_field)\
        __cjsonx_array_ptr_ex(type, field, item_count_field, _cjsonx_reflect_real)
#define __cjsonx_array_ptr_bool_ex(type, field, item_count_field)\
        __cjsonx_array_ptr_ex(type, field, item_count_field, _cjsonx_reflect_bool)
#define __cjsonx_array_ptr_str_ptr_ex(type, field, item_count_field)\
        __cjsonx_array_ptr_ex(type, field, item_count_field, _cjsonx_reflect_string_ptr)
#define __cjsonx_array_ptr_object_ex(type, field, item_count_field, refelct)\
        __cjsonx_array_ptr_ex(type, field, item_count_field, refelct)

/** Array Pointer Reflections with Annotations **/
#define __cjsonx_array_int_ex(type, field, item_count_field)\
        __cjsonx_array_bufferred_ex(type, field, item_count_field, _cjsonx_reflect_int)
#define __cjsonx_array_real_ex(type, field, item_count_field)\
        __cjsonx_array_bufferred_ex(type, field, item_count_field, _cjsonx_reflect_real)
#define __cjsonx_array_bool_ex(type, field, item_count_field)\
        __cjsonx_array_bufferred_ex(type, field, item_count_field, _cjsonx_reflect_bool)
#define __cjsonx_array_str_ex(type, field, item_count_field)\
        __cjsonx_array_bufferred_ex(type, field, item_count_field, _cjsonx_reflect_string_bufferred)
#define __cjsonx_array_str_ptr_ex(type, field, item_count_field)\
        __cjsonx_array_bufferred_ex(type, field, item_count_field, _cjsonx_reflect_string_ptr)
#define __cjsonx_array_object_ex(type, field, item_count_field, refelct)\
        __cjsonx_array_bufferred_ex(type, field, item_count_field, refelct)


/*** Interfaces ***/

/* Error code decription */
const char* cjsonx_err_str(int code);

/**
 * Convert Json string to struct
 * 
 * @param jstr Json string
 * @param output Struct address
 * @param tbl Reflection table
 * @return Error code 
 */
int cjsonx_str2struct(const char* jstr, void* output, const cjsonx_reflect_t* tbl);

/**
 * Convert Json string with length to struct
 * 
 * @param jstr Json string bufferred
 * @param len Buffer length
 * @param output Struct address
 * @param tbl Reflection table
 * @return Error code 
 */
int cjsonx_nstr2struct(const char* jstr, size_t len, void* output, const cjsonx_reflect_t* tbl);

/**
 * Convert cJSON object to struct
 * 
 * @param jo Json object
 * @param output Struct address
 * @param tbl Reflection table
 * @return Error code 
 */
int cjsonx_obj2struct(cJSON* jo, void* output, const cjsonx_reflect_t* tbl);

/**
 * Convert struct to json string(malloc)
 * 
 * @param jstr Pointer to place string address
 * @param input Struct address
 * @param tbl Reflection table
 * @return error code
 */
int cjsonx_struct2str(char** jstr, void* input, const cjsonx_reflect_t* tbl);

/**
 * Convert struct to json string(malloc)
 * 
 * @param jstr String buffer
 * @param size Buffer size
 * @param input Struct address
 * @param tbl Reflection table
 * @return error code
 */
int cjsonx_struct2str_bufferred(char* jstr, const int size, void* input, const cjsonx_reflect_t* tbl);

/**
 * Convert struct to cJSON object
 * 
 * @param obj cJSON object
 * @param input Struct address
 * @param tbl Reflection table
 * @return error code
 */
int cjsonx_struct2obj(cJSON* obj, void* input, const cjsonx_reflect_t* tbl);

/**
 * Struct info print
 * 
 * @param data Struct address
 * @param tbl Reflection table
 */
void cjsonx_reflection_print(void* data, const cjsonx_reflect_t* tbl);

/**
 * Struct field memory free
 * Before using this function, make sure struct's pointer filed is created by
 * `malloc` or `cJSON_malloc(malloc of cJSON_Hooks)`
 * 
 * @param data Struct address
 * @param tbl Reflection table
 */
void cjsonx_reflection_ptr_free(void* list, const cjsonx_reflect_t* tbl);

#ifdef __cplusplus
}
#endif
#endif
