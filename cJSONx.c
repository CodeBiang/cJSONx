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
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSONx.h"

#define CJSONX_DBG_FIELD(xx)                                  

static cJSON* cjson_impl_object_get(const cJSON* object, const char* key);
static cjsonx_type_e cjson_impl_typeof(cJSON* object);
static cJSON* cjson_impl_parse(const char* buffer, size_t buflen);
static void cjson_impl_delete(cJSON* object);
static const char* cjson_impl_string_value(const cJSON* object);
__attribute__((unused)) static size_t cjson_impl_string_length(const cJSON* object);
static long long cjson_impl_integer_value(const cJSON* object);
static double cjson_impl_real_value(const cJSON* object);
__attribute__((unused)) static char cjson_impl_bool_value(const cJSON* object);
static size_t cjson_impl_array_size(const cJSON* object);
static cJSON* cjson_impl_array_get(const cJSON* object, size_t index);
static cJSON* cjson_impl_object(void);
static char* cjson_impl_to_string(cJSON* object);
static int cjson_impl_to_string_bufferred(cJSON* object, char* buf, const int size);
static cJSON* cjson_impl_integer(long long val);
static cJSON* cjson_impl_string(const char* val);
static cJSON* cjson_impl_bool(char val);
static cJSON* cjson_impl_real(double val);
static cJSON* cjson_impl_array(void);
static int cjson_impl_array_add(cJSON* array, cJSON* obj);
static int cjson_impl_object_set_new(cJSON* rootObj, const char* field, cJSON* obj);

#define cjsonx_object_get cjson_impl_object_get
#define cjsonx_typeof cjson_impl_typeof
#define cjsonx_parse cjson_impl_parse
#define cjsonx_delete cjson_impl_delete
#define cjsonx_string_value cjson_impl_string_value
#define cjsonx_string_length cjson_impl_string_length
#define cjsonx_integer_value cjson_impl_integer_value
#define cjsonx_real_value cjson_impl_real_value
#define cjsonx_bool_value cjson_impl_bool_value
#define cjsonx_array_size cjson_impl_array_size
#define cjsonx_array_get cjson_impl_array_get
#define cjsonx_object cjson_impl_object
#define cjsonx_to_string cjson_impl_to_string
#define cjsonx_to_string_bufferred cjson_impl_to_string_bufferred
#define cjsonx_integer cjson_impl_integer
#define cjsonx_string cjson_impl_string
#define cjsonx_bool cjson_impl_bool
#define cjsonx_real cjson_impl_real
#define cjsonx_array cjson_impl_array
#define cjsonx_array_add cjson_impl_array_add
#define cjsonx_object_set_new cjson_impl_object_set_new
#define cjsonx_is_number(type) (type == CJSONX_REAL || type == CJSONX_INTEGER)
#define cjsonx_is_bool(type)    (type == CJSONX_TRUE || type == CJSONX_FALSE)

const cjsonx_reflect_t _cjsonx_reflect_int[] = {
    {"0integer", 0, sizeof(int), CJSONX_INTEGER, NULL, NULL, 0, 0, {0}}, {0}};

const cjsonx_reflect_t _cjsonx_reflect_string_ptr[] = {
    {"0strptr", 0, sizeof(char*), CJSONX_STRING, NULL, NULL, 0, 1, {0}}, {0}};

const cjsonx_reflect_t _cjsonx_reflect_real[] = {
    {"0real", 0, sizeof(double), CJSONX_REAL, NULL, NULL, 0, 0, {0}}, {0}};

const cjsonx_reflect_t _cjsonx_reflect_string_bufferred[] = {
    {"0str", 0, sizeof(char*), CJSONX_STRING, NULL, NULL, 0, 0, {0}}, {0}};
    
const cjsonx_reflect_t _cjsonx_reflect_float[] = {
    {"0float", 0, sizeof(float), CJSONX_REAL, NULL, NULL, 0, 0, {0}}, {0}};

const cjsonx_reflect_t _cjsonx_reflect_double[] = {
    {"0double", 0, sizeof(double), CJSONX_REAL, NULL, NULL, 0, 0, {0}}, {0}};

const cjsonx_reflect_t _cjsonx_reflect_bool[] = {
    {"0bool", 0, sizeof(int), CJSONX_TRUE, NULL, NULL, 0, 0, {0}}, {0}};
    
static const cjsonx_reflect_t* _get_reflection(const char* field, const cjsonx_reflect_t* tbl, int* pIndex);
static void* _cjsonx_get_field(void* obj, const char* field, const cjsonx_reflect_t* tbl, int* pIndex);
static void _cjsonx_set_field(void* obj, const char* field, void* data, const cjsonx_reflect_t* tbl);
static void _cjsonx_set_field_fast(void* obj, const void* data, const cjsonx_reflect_t* tbl);
static void _cjsonx_set_field_buffered(void* obj, const void* data, size_t data_size, const cjsonx_reflect_t* tbl);

typedef union {
    char c;
    short s;
    int i;
    long long l;
} cjsonx_int_val_t;

static long long _cjsonx_get_int_form_ptr(void* ptr, size_t size);
static int _cjsonx_get_int(cJSON* jo_tmp, size_t size, cjsonx_int_val_t* i);
static int _cjsonx_convert_int(long long val, int size, cjsonx_int_val_t* i);
static int _cjsonx_check_int(long long val, int size);

#ifndef __CJSONX_SERIALIZE_INTERFACES_
#define __CJSONX_SERIALIZE_INTERFACES_

typedef int (*cjsonx_obj_serializer)(void* input, const cjsonx_reflect_t* tbl,
                              int index, cJSON** obj);

static int _cjsonx_serialize_object(void* input, const cjsonx_reflect_t* tbl, int index,
                         cJSON** obj);
static int _cjsonx_serialize_array(void* input, const cjsonx_reflect_t* tbl, int index,
                        cJSON** obj);
static int _cjsonx_serialize_string(void* input, const cjsonx_reflect_t* tbl, int index,
                         cJSON** obj);
static int _cjsonx_serialize_integer(void* input, const cjsonx_reflect_t* tbl, int index,
                          cJSON** obj);
static int _cjsonx_serialize_real(void* input, const cjsonx_reflect_t* tbl, int index,
                       cJSON** obj);
static int _cjsonx_serialize_bool(void* input, const cjsonx_reflect_t* tbl, int index,
                       cJSON** obj);

cjsonx_obj_serializer _json_serializer_tbl[] = {
    _cjsonx_serialize_object,  
    _cjsonx_serialize_array, 
    _cjsonx_serialize_string,
    _cjsonx_serialize_integer, 
    _cjsonx_serialize_real,  
    _cjsonx_serialize_bool,
    _cjsonx_serialize_bool,    
    NULL
};

typedef int (*cjsonx_arr_item_serializer)(void* input, const cjsonx_reflect_t* tbl,
                            int index, const cjsonx_reflect_t* arr_reflect, cJSON** obj);


// static int _cjsonx_serialize_arr_object(void* input, const cjsonx_reflect_t* tbl, int index,
//                          cJSON** obj);
// static int _cjsonx_serialize_arr_array(void* input, const cjsonx_reflect_t* tbl, int index,
//                         cJSON** obj);
static int _cjsonx_serialize_arr_string(void* input, const cjsonx_reflect_t* tbl, int index,
                            const cjsonx_reflect_t* arr_reflect, cJSON** obj);
static int _cjsonx_serialize_arr_integer(void* input, const cjsonx_reflect_t* tbl, int index,
                            const cjsonx_reflect_t* arr_reflect, cJSON** obj);
static int _cjsonx_serialize_arr_real(void* input, const cjsonx_reflect_t* tbl, int index,
                            const cjsonx_reflect_t* arr_reflect, cJSON** obj);
static int _cjsonx_serialize_arr_bool(void* input, const cjsonx_reflect_t* tbl, int index,
                            const cjsonx_reflect_t* arr_reflect, cJSON** obj);

cjsonx_arr_item_serializer _json_arr_serializer_tbl[] = {
    NULL,  
    NULL, 
    _cjsonx_serialize_arr_string,
    _cjsonx_serialize_arr_integer, 
    _cjsonx_serialize_arr_real,
    _cjsonx_serialize_arr_bool,
    _cjsonx_serialize_arr_bool,    
    NULL
};

int cjsonx_struct2obj(cJSON* obj, void* input,
                              const cjsonx_reflect_t* tbl);

#endif


#ifndef __CJSONX_DESERIALIZE_INTERFACES_
#define __CJSONX_DESERIALIZE_INTERFACES_
typedef int (*cjsonx_obj_deserialzer)(cJSON* jo_tmp, void* output,
                             const cjsonx_reflect_t* tbl, int index);

static int _cjsonx_deserialize_object(cJSON* jo_tmp, void* output,
                           const cjsonx_reflect_t* tbl, int index);
static int _cjsonx_deserialize_array(cJSON* jo_tmp, void* output,
                          const cjsonx_reflect_t* tbl, int index);
static int _cjsonx_deserialize_string(cJSON* jo_tmp, void* output,
                           const cjsonx_reflect_t* tbl, int index);
static int _cjsonx_deserialize_integer(cJSON* jo_tmp, void* output,
                            const cjsonx_reflect_t* tbl, int index);
static int _cjsonx_deserialize_real(cJSON* jo_tmp, void* output,
                         const cjsonx_reflect_t* tbl, int index);
static int _cjsonx_deserialize_bool(cJSON* jo_tmp, void* output,
                         const cjsonx_reflect_t* tbl, int index);

cjsonx_obj_deserialzer _json_deserializer_tbl[] = {
    _cjsonx_deserialize_object, 
    _cjsonx_deserialize_array, 
    _cjsonx_deserialize_string, 
    _cjsonx_deserialize_integer,
    _cjsonx_deserialize_real,   
    _cjsonx_deserialize_bool,  
    _cjsonx_deserialize_bool,   
    NULL
};

static int _cjsonx_deserialize_object_default(cJSON* jo_tmp, void* output,
                                  const cjsonx_reflect_t* tbl, int index);
static int _cjsonx_deserialize_array_default(cJSON* jo_tmp, void* output,
                                 const cjsonx_reflect_t* tbl, int index);
static int _cjsonx_deserialize_string_default(cJSON* jo_tmp, void* output,
                                  const cjsonx_reflect_t* tbl, int index);
static int _cjsonx_deserialize_integer_default(cJSON* jo_tmp, void* output,
                                   const cjsonx_reflect_t* tbl, int index);
static int _cjsonx_deserialize_real_default(cJSON* jo_tmp, void* output,
                                const cjsonx_reflect_t* tbl, int index);

cjsonx_obj_deserialzer _json_deserializer_default_tbl[] = {
    _cjsonx_deserialize_object_default,  
    _cjsonx_deserialize_array_default,
    _cjsonx_deserialize_string_default,  
    _cjsonx_deserialize_integer_default,
    _cjsonx_deserialize_real_default,    
    _cjsonx_deserialize_integer_default,
    _cjsonx_deserialize_integer_default, 
    NULL
};

typedef int (*cjsonx_arr_item_deserialzer)(cJSON* jo_tmp, void* output, const cjsonx_reflect_t* tbl,
                        int index, const cjsonx_reflect_t* arr_reflect);
                             
static int _cjsonx_deserialize_arr_string(cJSON* jo_tmp, void* output, const cjsonx_reflect_t* tbl,
                        int index, const cjsonx_reflect_t* arr_reflect);
static int _cjsonx_deserialize_arr_integer(cJSON* jo_tmp, void* output, const cjsonx_reflect_t* tbl,
                        int index, const cjsonx_reflect_t* arr_reflect);
static int _cjsonx_deserialize_arr_real(cJSON* jo_tmp, void* output, const cjsonx_reflect_t* tbl,
                        int index, const cjsonx_reflect_t* arr_reflect);
static int _cjsonx_deserialize_arr_bool(cJSON* jo_tmp, void* output, const cjsonx_reflect_t* tbl,
                        int index, const cjsonx_reflect_t* arr_reflect);

cjsonx_arr_item_deserialzer _json_arr_deserializer_tbl[] = {
    NULL,
    NULL,
    _cjsonx_deserialize_arr_string, 
    _cjsonx_deserialize_arr_integer,
    _cjsonx_deserialize_arr_real,   
    _cjsonx_deserialize_arr_bool,  
    _cjsonx_deserialize_arr_bool,
    NULL
};

int cjsonx_obj2struct(cJSON* jo, void* output, const cjsonx_reflect_t* tbl);

#endif

#ifdef __CJSONX_SERIALIZE_INTERFACES_

int cjsonx_struct2str(char** jstr, void* input, const cjsonx_reflect_t* tbl) {
    cJSON* jsonPack = cjsonx_object();
    char* dumpStr = NULL;

    if (!jsonPack) return ERR_CJSONX_MEMORY;

    int ret = cjsonx_struct2obj(jsonPack, input, tbl);

    if (ret == ERR_CJSONX_NONE) {
        dumpStr = cjsonx_to_string(jsonPack);
        if (dumpStr == NULL) {
            ret = ERR_CJSONX_MEMORY;
        } else {
            *jstr = dumpStr;
        }
    }

    cjsonx_delete(jsonPack);
    return ret;
}

int cjsonx_struct2str_bufferred(char* jstr, const int size, void* input, const cjsonx_reflect_t* tbl) {
    cJSON* jsonPack = cjsonx_object();
    int ret = ERR_CJSONX_NONE;
    if (!jsonPack) return ERR_CJSONX_MEMORY;

    ret = cjsonx_struct2obj(jsonPack, input, tbl);

    if (ret == ERR_CJSONX_NONE) {
        if (!cjsonx_to_string_bufferred(jsonPack, jstr, size)) {
            ret = ERR_CJSONX_OVERFLOW;
        }
    }

    cjsonx_delete(jsonPack);
    return ret;
}

int cjsonx_struct2obj(cJSON* obj, void* input, const cjsonx_reflect_t* tbl) {
    int i = 0;
    int ret = ERR_CJSONX_NONE;
    cJSON* joTmp = NULL;
    int jsonType;

    if (!obj || !input || !tbl) return ERR_CJSONX_ARGS;

    while (1) {
        if (tbl[i].field == NULL) break;

        if (!(tbl[i].annotation.serialized)) {
            i++;
            continue;
        }

        jsonType = tbl[i].type;

        if (_json_serializer_tbl[jsonType] != NULL) {
            ret = _json_serializer_tbl[jsonType](input, tbl, i, &joTmp);
        }

        if (ret != ERR_CJSONX_NONE) {
            CJSONX_DBG("\e[0;35mSerializing error: %d [%s]\e[0m\r\n", ret, tbl[i].field);
            if (!(tbl[i].annotation.nullable)) return ret;
        } else {
            cjsonx_object_set_new(obj, tbl[i].annotation.serialized_name ? 
                    tbl[i].annotation.serialized_name : tbl[i].field, joTmp);
        }

        i++;
    }

    return ERR_CJSONX_NONE;
}

int _cjsonx_serialize_integer(void* input, const cjsonx_reflect_t* tbl, int index,
                        cJSON** obj) {
    void* pSrc = NULL;
    long long val = 0LL;

    if (tbl[index].size != sizeof(char) && tbl[index].size != sizeof(short) &&
        tbl[index].size != sizeof(int) &&
        tbl[index].size != sizeof(long long)) {
        CJSONX_DBG("\e[0;35mInteger size(=%ld) unsupported\e[0m\r\n", tbl[index].size);
        return ERR_CJSONX_OVERFLOW;
    }

    pSrc = (void*)((char*)input + tbl[index].offset);

    val = _cjsonx_get_int_form_ptr(pSrc, tbl[index].size);

    *obj = cjsonx_integer(val);

    return ERR_CJSONX_NONE;
}

int _cjsonx_serialize_string(void* input, const cjsonx_reflect_t* tbl, int index,
                        cJSON** obj) {
    void* pSrc = (void*)((char*)input + tbl[index].offset);

    CJSONX_DBG_FIELD(const cjsonx_reflect_t* r = &tbl[index]);

    if (tbl[index].constructed) {
        if (*((char**)pSrc) == NULL) 
            return ERR_CJSONX_MISSING_FIELD;
        // Pointer with *((char**)pSrc)
        *obj = cjsonx_string(*((char**)pSrc));
    } else {
        // Array with (char*)
        *obj = cjsonx_string((char*)pSrc);
    }
    return ERR_CJSONX_NONE;
}

int _cjsonx_serialize_object(void* input, const cjsonx_reflect_t* tbl, int index,
                        cJSON** obj) {
    int ret = ERR_CJSONX_NONE;
    void* pSrc = (void*)((char*)input + tbl[index].offset);
    cJSON* jotmp = cjsonx_object();
    if (tbl[index].constructed) {
        ret = cjsonx_struct2obj(jotmp, *(void**)pSrc, tbl[index].reflection);
    } else {
        ret = cjsonx_struct2obj(jotmp, pSrc, tbl[index].reflection);
    }
    
    if (ret == ERR_CJSONX_NONE) {
        *obj = jotmp;
    } else {
        cjsonx_delete(jotmp);
    }

    return ret;
}

int _cjsonx_serialize_array(void* input, const cjsonx_reflect_t* tbl, int index,
                        cJSON** obj) {
    int ret = ERR_CJSONX_NONE;
    int countIndex = -1;
    cJSON* joArray = NULL;
    long long size;
    long long successCount;
    long long i = 0;
    void* ptr = NULL;
    void* pSrc = ((void*)((char*)input + tbl[index].offset));

    if (tbl[index].constructed) {
        pSrc = *(void**)pSrc;
        if (pSrc == NULL) return ERR_CJSONX_MISSING_FIELD;
    }

    ptr = _cjsonx_get_field(input, tbl[index].arr_count_field, tbl, &countIndex);

    if (ptr == NULL || countIndex == -1) {
        return ERR_CJSONX_MISSING_FIELD;
    }
    size = _cjsonx_get_int_form_ptr(ptr, tbl[countIndex].size);

    joArray = cjsonx_array();

    successCount = 0;

    for (i = 0; i < size; i++) {
        cJSON* jotmp;

        if (tbl[index].reflection[0].field[0] ==
            '0') {
            ret = _json_arr_serializer_tbl[tbl[index].reflection[0].type](
                pSrc + (i * tbl[index].item_size),
                tbl[index].reflection, 0, &tbl[index], &jotmp);
        } else {
            jotmp = cjsonx_object();
            ret =
                cjsonx_struct2obj(jotmp, pSrc + (i * tbl[index].item_size),
                                   tbl[index].reflection);
        }

        if (ret == ERR_CJSONX_NONE) {
            successCount++;
            cjsonx_array_add(joArray, jotmp);
        } else {
            cjsonx_delete(jotmp);
        }
    }

    if (successCount == 0) {
        cjsonx_delete(joArray);
        return ERR_CJSONX_MISSING_FIELD;
    } else {
        *obj = joArray;
        return ERR_CJSONX_NONE;
    }

    return ret;
}

int _cjsonx_serialize_real(void* input, const cjsonx_reflect_t* tbl, int index,
                        cJSON** obj) {
    double temp = 0;
    char convert_cache[50];
    char* convert_pend;
    void* pSrc = NULL;

    if (tbl[index].size != sizeof(double) && tbl[index].size != sizeof(float)) {
        CJSONX_DBG("\e[0;35mReal number size(=%ld) unsupported\e[0m\r\n", tbl[index].size);
        return ERR_CJSONX_OVERFLOW;
    }

    pSrc = (void*)((char*)input + tbl[index].offset);

    if (tbl[index].size == sizeof(double)) {
        snprintf(convert_cache, sizeof(convert_cache), "%lf", *(double*)pSrc);
    } else {
        snprintf(convert_cache, sizeof(convert_cache), "%f", *(float*)pSrc);
    }
    temp = strtod(convert_cache, &convert_pend);
    *obj = cjsonx_real(temp);
    return ERR_CJSONX_NONE;
}

int _cjsonx_serialize_bool(void* input, const cjsonx_reflect_t* tbl, int index,
                        cJSON** obj) {
    void* pSrc = NULL;
    if (tbl[index].size != sizeof(char) && tbl[index].size != sizeof(short) &&
        tbl[index].size != sizeof(int) &&
        tbl[index].size != sizeof(long long)) {
        CJSONX_DBG("\e[0;35mBool size(=%ld) unsupported\e[0m\r\n", tbl[index].size);
        return ERR_CJSONX_OVERFLOW;
    }

    pSrc = (void*)((char*)input + tbl[index].offset);

    if (tbl[index].size == sizeof(char)) {
        *obj = cjsonx_bool(*((char*)pSrc));
    } else if (tbl[index].size == sizeof(short)) {
        *obj = cjsonx_bool(*((short*)pSrc));
    } else if (tbl[index].size == sizeof(int)) {
        *obj = cjsonx_bool(*((int*)pSrc));
    } else {
        *obj = cjsonx_bool(*((long long*)pSrc));
    }

    return ERR_CJSONX_NONE;
}


int _cjsonx_serialize_arr_string(void* input, const cjsonx_reflect_t* tbl, int index,
                        const cjsonx_reflect_t* arr_reflect, cJSON** obj) {
    return _cjsonx_serialize_string(input, tbl, index, obj);
}

int _cjsonx_serialize_arr_integer(void* input, const cjsonx_reflect_t* tbl, int index,
                        const cjsonx_reflect_t* arr_reflect, cJSON** obj) {
    void* pSrc = NULL;
    long long val;
    if (arr_reflect->item_size != sizeof(char) && 
        arr_reflect->item_size != sizeof(short) &&
        arr_reflect->item_size != sizeof(int) &&
        arr_reflect->item_size != sizeof(long long)) {
        CJSONX_DBG("\e[0;35mInteger size(=%ld) unsupported\e[0m\r\n", arr_reflect->item_size);
        return ERR_CJSONX_OVERFLOW;
    }

    pSrc = (void*)((char*)input + tbl[index].offset);

    val = _cjsonx_get_int_form_ptr(pSrc, arr_reflect->item_size);

    *obj = cjsonx_integer(val);

    return ERR_CJSONX_NONE;
}

int _cjsonx_serialize_arr_real(void* input, const cjsonx_reflect_t* tbl, int index,
                        const cjsonx_reflect_t* arr_reflect, cJSON** obj) {
    double temp = 0;
    char convert_cache[50];
    char* convert_pend;
    void* pSrc = NULL;
    if (arr_reflect->item_size != sizeof(double) && arr_reflect->item_size != sizeof(float)) {
        CJSONX_DBG("\e[0;35mReal Number size(=%ld) unsupported\e[0m\r\n", arr_reflect->item_size);
        return ERR_CJSONX_OVERFLOW;
    }

    pSrc = (void*)((char*)input + tbl[index].offset);

    if (arr_reflect->item_size == sizeof(double)) {
        snprintf(convert_cache, sizeof(convert_cache), "%lf", *(double*)pSrc);
    } else {
        snprintf(convert_cache, sizeof(convert_cache), "%f", *(float*)pSrc);
    }
    temp = strtod(convert_cache, &convert_pend);
    *obj = cjsonx_real(temp);
    return ERR_CJSONX_NONE;
}

int _cjsonx_serialize_arr_bool(void* input, const cjsonx_reflect_t* tbl, int index,
                        const cjsonx_reflect_t* arr_reflect, cJSON** obj) {
    void* pSrc = NULL;
    if (arr_reflect->item_size != sizeof(char) && 
        arr_reflect->item_size != sizeof(short) &&
        arr_reflect->item_size != sizeof(int) &&
        arr_reflect->item_size != sizeof(long long)) {
        CJSONX_DBG("\e[0;35mBool size(=%ld) unsupported\e[0m\r\n", arr_reflect->item_size);
        return ERR_CJSONX_OVERFLOW;
    }

    pSrc = (void*)((char*)input + tbl[index].offset);

    if (arr_reflect->item_size == sizeof(char)) {
        *obj = cjsonx_bool(*((char*)pSrc));
    } else if (arr_reflect->item_size == sizeof(short)) {
        *obj = cjsonx_bool(*((short*)pSrc));
    } else if (arr_reflect->item_size == sizeof(int)) {
        *obj = cjsonx_bool(*((int*)pSrc));
    } else {
        *obj = cjsonx_bool(*((long long*)pSrc));
    }

    return ERR_CJSONX_NONE;
}

#endif


#ifdef __CJSONX_DESERIALIZE_INTERFACES_

int cjsonx_str2struct(const char* jstr, void* output,
                       const cjsonx_reflect_t* tbl) {
    int ret;
    cJSON* jo = cjsonx_parse(jstr, strlen(jstr));

    if (!jo) return ERR_CJSONX_FORMAT;

    ret = cjsonx_obj2struct(jo, output, tbl);
    cjsonx_delete(jo);

    return ret;
}

int cjsonx_nstr2struct(const char* jstr, size_t len, void* output, const cjsonx_reflect_t* tbl) {
    int ret;
    if (len < 0) return ERR_CJSONX_ARGS;

    cJSON* jo = cjsonx_parse(jstr, len);

    if (!jo) return ERR_CJSONX_FORMAT;

    ret = cjsonx_obj2struct(jo, output, tbl);
    cjsonx_delete(jo);

    return ret;
}

int cjsonx_obj2struct(cJSON* jo, void* output, const cjsonx_reflect_t* tbl) {
    int ret, i;
    cJSON* jo_tmp;
    int jsonType;

    if (!jo || !output || !tbl) return ERR_CJSONX_ARGS;

    for (i = 0;; i++) {
        ret = ERR_CJSONX_NONE;

        if (tbl[i].field == NULL) break;

        if (!(tbl[i].annotation.deserialized)) {
            continue;
        }

        jo_tmp = cjsonx_object_get(jo, tbl[i].annotation.serialized_name ? 
                tbl[i].annotation.serialized_name : tbl[i].field);

        if (jo_tmp == NULL) {
            ret = ERR_CJSONX_MISSING_FIELD;
        } else {
            jsonType = cjsonx_typeof(jo_tmp);

            if (jsonType == tbl[i].type ||
                (cjsonx_is_number(cjsonx_typeof(jo_tmp)) &&
                 cjsonx_is_number(tbl[i].type)) ||
                (cjsonx_is_bool(cjsonx_typeof(jo_tmp)) &&
                 cjsonx_is_bool(tbl[i].type))) {
                if (_json_deserializer_tbl[tbl[i].type] != NULL) {
                    ret = _json_deserializer_tbl[tbl[i].type](jo_tmp, output, tbl, i);
                }
            } else {
                ret = ERR_CJSONX_TYPE;
            }
        }

        if (ret != ERR_CJSONX_NONE) {
            CJSONX_DBG("\e[0;35mparse error on field:%s, cod=%d\e[0m\r\n", tbl[i].field,
                   ret);
            _json_deserializer_default_tbl[tbl[i].type](NULL, output, tbl, i);
            if (!(tbl[i].annotation.nullable)) return ret;
        }
    }

    return ERR_CJSONX_NONE;
}

int _cjsonx_deserialize_string(cJSON* jo_tmp, void* output, const cjsonx_reflect_t* tbl,
                    int index) {
    const char* tempstr = cjsonx_string_value(jo_tmp);
    char* pDst;
    CJSONX_DBG_FIELD(const cjsonx_reflect_t* t = &tbl[index]);

    if (NULL != tempstr) {
        if ((tbl + index)->constructed) {
            pDst = (char*)cJSON_malloc(strlen(tempstr) + 1);
            if (pDst == NULL) {
                return ERR_CJSONX_MEMORY;
            }
            strcpy(pDst, tempstr);
            _cjsonx_set_field_fast(output, &pDst, tbl + index);
        } else {
            // Bufferred
            memset(output + tbl[index].offset, 0, (tbl + index)->size);
            _cjsonx_set_field_buffered(output, tempstr, 
                strlen(tempstr) >= (tbl + index)->size ? (tbl + index)->size - 1 : strlen(tempstr), 
                tbl + index);
        }

        return ERR_CJSONX_NONE;
    }

    return ERR_CJSONX_MISSING_FIELD;
}

int _cjsonx_deserialize_integer(cJSON* jo_tmp, void* output, const cjsonx_reflect_t* tbl,
                     int index) {
    int ret;
    cjsonx_int_val_t value;
    ret = _cjsonx_get_int(jo_tmp, tbl[index].size, &value);

    if (ret != ERR_CJSONX_NONE) {
        CJSONX_DBG("\e[0;35mGet integer field[%s] failed:%d.\e[0m\r\n", tbl[index].field,
               ret);
    } else {
        _cjsonx_set_field_fast(output, &value, tbl + index);
    }

    return ret;
}

int _cjsonx_deserialize_object(cJSON* jo_tmp, void* output, const cjsonx_reflect_t* tbl,
                    int index) {
    int ret = ERR_CJSONX_NONE;
    void* temp = NULL;
    CJSONX_DBG_FIELD(const cjsonx_reflect_t* t = &tbl[index]);
    
    if (tbl[index].constructed) {
        temp = cJSON_malloc(tbl[index].item_size);
        if (!temp)
            return ERR_CJSONX_MEMORY;
    } else {
        temp = output + tbl[index].offset;
    }
    memset(temp, 0, tbl[index].item_size);

    ret = cjsonx_obj2struct(jo_tmp, (char*)temp, tbl[index].reflection);

    if (tbl[index].constructed) {
        if (ret != ERR_CJSONX_NONE) {
            cJSON_free(temp);
            temp = NULL;
        }
        _cjsonx_set_field_fast(output, &temp, tbl + index);
    }
    return ret;
}

int _cjsonx_deserialize_array(cJSON* jo_tmp, void* output, const cjsonx_reflect_t* tbl,
                   int index) {
    int countIndex = -1;
    void* pMem = NULL;
    size_t count;
    long long successCount = 0;
    cjsonx_int_val_t val;
    size_t arraySize = cjsonx_array_size(jo_tmp);
    size_t j;
    cJSON* item;

    if (arraySize == 0) {
        _cjsonx_set_field(output, tbl[index].arr_count_field, &arraySize, tbl);
        return ERR_CJSONX_NONE;
    }

    _cjsonx_get_field(output, tbl[index].arr_count_field, tbl, &countIndex);

    if (countIndex == -1) {
        return ERR_CJSONX_MISSING_FIELD;
    }

    if (tbl[index].constructed) {
        pMem = malloc(arraySize * tbl[index].item_size);
        if (pMem == NULL) return ERR_CJSONX_MEMORY;
    } else {
        // Bad performance
        pMem = (output + tbl[index].offset);
        count = tbl[index].size / tbl[index].item_size;
        arraySize = count > arraySize ? arraySize : count;
    }
    memset(pMem, 0, arraySize * tbl[index].item_size);

    for (j = 0; j < arraySize; j++) {
        item = cjsonx_array_get(jo_tmp, j);
        if (item != NULL) {
            int ret;

            if (tbl[index].reflection[0].field[0] ==
                '0') {
                ret = _json_arr_deserializer_tbl[tbl[index].reflection[0].type](
                    item, pMem + (successCount * tbl[index].item_size),
                    tbl[index].reflection, 0, tbl + index);
            } else {
                ret = cjsonx_obj2struct(
                    item, pMem + (successCount * tbl[index].item_size),
                    tbl[index].reflection);
            }

            if (ret == ERR_CJSONX_NONE) {
                successCount++;
            }
        }
    }

    if (_cjsonx_convert_int(successCount, tbl[countIndex].size, &val) != ERR_CJSONX_NONE) {
        successCount = 0;
    }

    if (successCount == 0) {
        _cjsonx_set_field_fast(output, &successCount, tbl + countIndex);
        if (tbl[index].constructed) {
            free(pMem);
            pMem = NULL;
        }
        _cjsonx_set_field_fast(output, &pMem, tbl + index);
        return ERR_CJSONX_MISSING_FIELD;
    } else {
        _cjsonx_set_field_fast(output, &val, tbl + countIndex);
        if (tbl[index].constructed) {
            _cjsonx_set_field_fast(output, &pMem, tbl + index);
        }
        return ERR_CJSONX_NONE;
    }
}

int _cjsonx_deserialize_real(cJSON* jo_tmp, void* output, const cjsonx_reflect_t* tbl,
                  int index) {
    double temp_d;
    float temp_f = 0;
    
    if (tbl[index].size != sizeof(double) && tbl[index].size != sizeof(float)) {
        CJSONX_DBG("\e[0;35mReal number size(=%ld) unsupported\e[0m\r\n", tbl[index].size);
        return ERR_CJSONX_OVERFLOW;
    }

    if (cjsonx_typeof(jo_tmp) == CJSONX_REAL) {
        temp_d = cjsonx_real_value(jo_tmp);
    } else {
        temp_d = cjsonx_integer_value(jo_tmp);
    }
    temp_f = (float)temp_d;


    _cjsonx_set_field_fast(output, tbl[index].size == sizeof(double) ? (void*)&temp_d : (void*)&temp_f, tbl + index);
    return ERR_CJSONX_NONE;
}

int _cjsonx_deserialize_bool(cJSON* jo_tmp, void* output, const cjsonx_reflect_t* tbl,
                  int index) {
    int ret;
    cjsonx_int_val_t value;
    ret = _cjsonx_get_int(jo_tmp, tbl[index].size, &value);
    if (ret != ERR_CJSONX_NONE) {
        CJSONX_DBG("\e[0;35mGet bool field[%s] failed:%d.\e[0m\r\n", tbl[index].field,
               ret);
    } else {
        _cjsonx_set_field_fast(output, &value, tbl + index);
    }
    return ret;
}

int _cjsonx_deserialize_arr_string(cJSON* jo_tmp, void* output, const cjsonx_reflect_t* tbl,
                        int index, const cjsonx_reflect_t* arr_reflect) {
    char* pDst;
    size_t max_size;
    const char* tempstr = cjsonx_string_value(jo_tmp);
    CJSONX_DBG_FIELD(const cjsonx_reflect_t* t = &tbl[index]);

    if (NULL != tempstr) {
        if ((tbl + index)->constructed) {
            pDst = (char*)cJSON_malloc(strlen(tempstr) + 1);
            if (pDst == NULL) {
                return ERR_CJSONX_MEMORY;
            }
            strcpy(pDst, tempstr);
            _cjsonx_set_field_fast(output, &pDst, tbl + index);
        } else {
            // Bufferred
            max_size = arr_reflect->item_size;
            memset(output + tbl[index].offset, 0, max_size);
            _cjsonx_set_field_buffered(output, tempstr, 
                strlen(tempstr) >= max_size ? max_size - 1 : strlen(tempstr), 
                tbl + index);
        }

        return ERR_CJSONX_NONE;
    }

    return ERR_CJSONX_MISSING_FIELD;
}

int _cjsonx_deserialize_arr_integer(cJSON* jo_tmp, void* output, const cjsonx_reflect_t* tbl,
                        int index, const cjsonx_reflect_t* arr_reflect) {
    int ret;
    cjsonx_int_val_t value;
    ret = _cjsonx_get_int(jo_tmp, arr_reflect->item_size, &value);

    if (ret != ERR_CJSONX_NONE) {
        CJSONX_DBG("\e[0;35mGet integer field[%s] failed:%d.\e[0m\r\n", tbl[index].field,
               ret);
    } else {
        _cjsonx_set_field_fast(output, &value, tbl + index);
    }

    return ret;
}

int _cjsonx_deserialize_arr_real(cJSON* jo_tmp, void* output, const cjsonx_reflect_t* tbl,
                        int index, const cjsonx_reflect_t* arr_reflect) {
    double temp_d;
    float temp_f = 0;
    
    if (arr_reflect->item_size != sizeof(double) && arr_reflect->item_size != sizeof(float)) {
        CJSONX_DBG("\e[0;35mReal number size(=%ld) unsupported\e[0m\r\n", arr_reflect->item_size);
        return ERR_CJSONX_OVERFLOW;
    }

    if (cjsonx_typeof(jo_tmp) == CJSONX_REAL) {
        temp_d = cjsonx_real_value(jo_tmp);
    } else {
        temp_d = cjsonx_integer_value(jo_tmp);
    }
    temp_f = (float)temp_d;
  
    if (arr_reflect->item_size == sizeof(double)) {
        _cjsonx_set_field_fast(output, (void*)&temp_d, 
                _cjsonx_reflect_double);
    } else {
        _cjsonx_set_field_fast(output, (void*)&temp_f, 
                _cjsonx_reflect_float);
    }
    return ERR_CJSONX_NONE;
}

int _cjsonx_deserialize_arr_bool(cJSON* jo_tmp, void* output, const cjsonx_reflect_t* tbl,
                        int index, const cjsonx_reflect_t* arr_reflect) {
    return _cjsonx_deserialize_arr_integer(jo_tmp, output, tbl, index, arr_reflect);
}

int _cjsonx_deserialize_object_default(cJSON* jo_tmp, void* output,
                           const cjsonx_reflect_t* tbl, int index) {
    int i = 0;
    void* temp = NULL;
    while (1) {
        if (tbl[i].reflection[i].field == NULL) break;

        if (!(tbl[i].annotation.deserialized)) {
            i++;
            continue;
        }

        _json_deserializer_default_tbl[tbl[index].reflection[i].type](
            NULL, output, tbl[index].reflection, i);
        i++;
    };
    // Constructed, set null
    if (tbl->constructed) {
        temp = NULL;
        _cjsonx_set_field_fast(output, &temp, tbl + index);
    }
    return ERR_CJSONX_NONE;
}

int _cjsonx_deserialize_array_default(cJSON* jo_tmp, void* output,
                          const cjsonx_reflect_t* tbl, int index) {             
    void* temp = NULL;       
    if (tbl->constructed) {
        _cjsonx_set_field_fast(output, &temp, tbl + index);
    }
    return ERR_CJSONX_NONE;
}

int _cjsonx_deserialize_string_default(cJSON* jo_tmp, void* output,
                           const cjsonx_reflect_t* tbl, int index) {
    char* temp = NULL;
    if (tbl->constructed)
        _cjsonx_set_field_fast(output, &temp, tbl + index);
    return ERR_CJSONX_NONE;
}

int _cjsonx_deserialize_integer_default(cJSON* jo_tmp, void* output,
                            const cjsonx_reflect_t* tbl, int index) {
    long long temp = 0;
    cjsonx_int_val_t ret;

    if (tbl[index].size != sizeof(char) && tbl[index].size != sizeof(short) &&
        tbl[index].size != sizeof(int) &&
        tbl[index].size != sizeof(long long)) {
        CJSONX_DBG("\e[0;35mInteger size(=%ld) unsupported\e[0m\r\n", tbl[index].size);
        return ERR_CJSONX_OVERFLOW;
    }

    _cjsonx_convert_int(temp, tbl[index].size, &ret);

    _cjsonx_set_field_fast(output, &ret, tbl + index);
    return ERR_CJSONX_NONE;
}

int _cjsonx_deserialize_real_default(cJSON* jo_tmp, void* output,
                         const cjsonx_reflect_t* tbl, int index) {
    double temp = 0.0;
    float tempf = 0.0F;
    if (tbl[index].size != sizeof(double) && tbl[index].size != sizeof(float)) {
        CJSONX_DBG("\e[0;35mReal number size(=%ld) unsupported\e[0m\r\n", tbl[index].size);
        return ERR_CJSONX_OVERFLOW;
    }

    _cjsonx_set_field_fast(output, tbl[index].size == sizeof(double) ? 
        (const void*)&temp : (const void*)&tempf, tbl + index);
    return ERR_CJSONX_NONE;
}

#endif

int _cjsonx_get_int(cJSON* jo_tmp, size_t size, cjsonx_int_val_t* i) {
    long long temp;
    double tempDouble;
    
    if (cjsonx_typeof(jo_tmp) == CJSONX_INTEGER) {
        temp = cjsonx_integer_value(jo_tmp);
    } else if (cjsonx_typeof(jo_tmp) == CJSONX_TRUE) {
        temp = 1;
    } else if (cjsonx_typeof(jo_tmp) == CJSONX_FALSE) {
        temp = 0;
    } else if (cjsonx_typeof(jo_tmp) == CJSONX_REAL) {
        tempDouble = cjsonx_real_value(jo_tmp);
        if (tempDouble > LLONG_MAX || tempDouble < LLONG_MIN) {
            return ERR_CJSONX_OVERFLOW;
        } else {
            temp = (long long)tempDouble;
        }
    } else {
        return ERR_CJSONX_ARGS;
    }

    return _cjsonx_convert_int(temp, size, i);
}

int _cjsonx_convert_int(long long val, int size, cjsonx_int_val_t* i) {
    int ret = _cjsonx_check_int(val, size);

    if (ret != ERR_CJSONX_NONE) return ret;

    if (size == sizeof(char)) {
        i->c = (char)val;
    } else if (size == sizeof(short)) {
        i->s = (short)val;
    } else if (size == sizeof(int)) {
        i->i = (int)val;
    } else {
        i->l = val;
    }

    return ERR_CJSONX_NONE;
}

int _cjsonx_check_int(long long val, int size) {
    if (size != sizeof(char) && size != sizeof(short) && size != sizeof(int) &&
        size != sizeof(long long)) {
        return ERR_CJSONX_OVERFLOW;
    }

    if (size == sizeof(char) && (val > CHAR_MAX || val < CHAR_MIN)) {
        return ERR_CJSONX_OVERFLOW;
    } else if (size == sizeof(short) && (val > SHRT_MAX || val < SHRT_MIN)) {
        return ERR_CJSONX_OVERFLOW;
    } else if (size == sizeof(int) && (val > INT_MAX || val < INT_MIN)) {
        return ERR_CJSONX_OVERFLOW;
    } else {
    }

    return ERR_CJSONX_NONE;
}

long long _cjsonx_get_int_form_ptr(void* ptr, size_t size) {
    long long ret = 0;

    if (!ptr) return 0;

    if (size == sizeof(char)) {
        ret = *((char*)ptr);
    } else if (size == sizeof(short)) {
        ret = *((short*)ptr);
    } else if (size == sizeof(int)) {
        ret = *((int*)ptr);
    } else if (size == sizeof(long long)) {
        ret = *((long long*)ptr);
    } else {
        CJSONX_DBG("\e[0;35mInteger size(=%ld) unsupported\e[0m\r\n", size);
    }

    return ret;
}

#ifndef __CJSONX_OTHOR_REFLECT_IMPLEMENTATION_
#define __CJSONX_OTHOR_REFLECT_IMPLEMENTATION_
/* reflect */
const cjsonx_reflect_t* _get_reflection(const char* field,
                                        const cjsonx_reflect_t* tbl,
                                        int* pIndex) {
    const cjsonx_reflect_t* ret = NULL;
    int i = 0;

    for (i = 0;; i++) {
        if (!(tbl[i].field)) break;
        if (strcmp(field, tbl[i].field) == 0) {
            ret = &(tbl[i]);

            if (pIndex) *pIndex = i;
            break;
        }
    }

    if (!ret) CJSONX_DBG("\e[0;35mField [%s] can not find\e[0m\r\n", field);

    return ret;
}

void* _cjsonx_get_field(void* obj, const char* field, const cjsonx_reflect_t* tbl,
                 int* pIndex) {
    const cjsonx_reflect_t* ret;
    if (!(obj && field && tbl)) return NULL;

    ret = _get_reflection(field, tbl, pIndex);

    if (!ret) return NULL;

    return (void*)((char*)obj + ret->offset);
}

void _cjsonx_set_field(void* obj, const char* field, void* data,
                     const cjsonx_reflect_t* tbl) {
    const cjsonx_reflect_t* ret;
    void* pDst;

    if (!(obj && field && data && tbl)) return;

    ret = _get_reflection(field, tbl, NULL);

    if (!ret) return;

    pDst = (void*)((char*)obj + ret->offset);
    memcpy(pDst, data, ret->size);
    return;
}

void _cjsonx_set_field_fast(void* obj, const void* data,
                         const cjsonx_reflect_t* tbl) {
    void* pDst;

    if (!(obj && data && tbl)) return;

    pDst = (void*)((char*)obj + tbl->offset);
    memcpy(pDst, data, tbl->size);
    return;
}

void _cjsonx_set_field_buffered(void* obj, const void* data, size_t data_size, const cjsonx_reflect_t* tbl) {
    void* pDst;
    if (data_size == 0) return;
    if (!(obj && data && tbl && (tbl->type == CJSONX_STRING || tbl->type == CJSONX_ARRAY))) return;

    CJSONX_DBG_FIELD(const char* c = data);
    pDst = (void*)((char*)obj + tbl->offset);
    memcpy(pDst, data, data_size);
    return;
}

#endif

#ifndef __CJSONX_OTHOR_INTERFACES_
#define __CJSONX_OTHOR_INTERFACES_

typedef void* (*cjsonx_loop_cb)(void* pData, const cjsonx_reflect_t* item, const cjsonx_reflect_t* arr);


const char* cjsonx_err_str(int code) {
    switch (code) {
        #define XX(name, code, desc) case code: return desc;
            CJSONX_ERR_CODE_MAP(XX)
        #undef XX
        default:
            return "unknown";
    }
}

static void cjsonx_reflection_loop(void* pData, const cjsonx_reflect_t* tbl, const cjsonx_reflect_t* arr, cjsonx_loop_cb func) {
    int i = 0;
    void* ptr = NULL;
    int countIndex = -1;
    char* field;
    long long size;
    long long unsigned j;

    while (1) {
        if (!tbl[i].field) break;
        
        // Field Data
        field = (char*)pData + tbl[i].offset;
        if (tbl[i].type == CJSONX_ARRAY) {
            ptr = _cjsonx_get_field(pData, tbl[i].arr_count_field, tbl, &countIndex);

            if (ptr == NULL || countIndex == -1) {
                continue;
            }
            
            size = _cjsonx_get_int_form_ptr(ptr, tbl[countIndex].size);

            for (j = 0; j < size; j++) {
                if (tbl[i].constructed) {
                    cjsonx_reflection_loop(*((char**)field) + j * tbl[i].item_size,
                        tbl[i].reflection, &tbl[i], func);
                } else {
                    cjsonx_reflection_loop(field + j * tbl[i].item_size,
                        tbl[i].reflection, &tbl[i], func);
                }
            }
        } else if (tbl[i].type == CJSONX_OBJECT) {
            if (tbl[i].constructed) {
                cjsonx_reflection_loop(*((char**)field), tbl[i].reflection, NULL, func);
            } else {
                cjsonx_reflection_loop(field, tbl[i].reflection, NULL, func);
            }
        }

        func(field, tbl + i, arr);

        i++;
    }
}

static void* _cjsonx_reflection_item_ptrint(void* pData, const cjsonx_reflect_t* item, const cjsonx_reflect_t* arr) {
    size_t size = 0;

    if (item->type == CJSONX_ARRAY || item->type == CJSONX_OBJECT) return NULL;

    if (item->type == CJSONX_INTEGER || item->type == CJSONX_TRUE ||
        item->type == CJSONX_FALSE) {
        if (arr) {
            size = arr->item_size;
        } else {
            size = item->size;
        }
        switch (size) {
            case sizeof(char):
                printf("%s:%d\n", item->field, *(char*)pData);
                break;
            case sizeof(short):
                printf("%s:%d\n", item->field, *(short*)pData);
                break;
            case sizeof(int):
                printf("%s:%d\n", item->field, *(int*)pData);
                break;
            case sizeof(long long):
                printf("%s:%lld\n", item->field, *(long long*)pData);
                break;
            default:
                printf("\e[0;35m%s:size error\e[0m\r\n", item->field);
        }
    }
    if (item->type == CJSONX_REAL) {
        if (arr) {
            size = arr->item_size;
        } else {
            size = item->size;
        }
        if (size == sizeof(float)) {
            printf("%s:%f\n", item->field, *(float*)pData);
        } else {
            printf("%s:%f\n", item->field, *(double*)pData);
        }
    }

    if (item->type == CJSONX_STRING) {
        if (item->constructed) {
            printf("%s:%s\n", item->field, *((char**)pData));
        } else {
            printf("%s:%s\n", item->field, (char*)pData);
        }
    }
    return NULL;
}

static void* _cjsonx_reflection_item_free(void* pData, const cjsonx_reflect_t* item, const cjsonx_reflect_t* arr) {
    if (item->constructed) {
        CJSONX_DBG("\e[0;32mField[%s] free\e[0m\r\n", item->field);
        free(*(void**)pData);
        *(void**)pData = NULL;
    }
    return NULL;
}

void cjsonx_reflection_print(void* pData, const cjsonx_reflect_t* tbl) {
    /* 调用loofield迭代结构体中的属性,完成迭代输出属性值 */
    cjsonx_reflection_loop(pData, tbl, NULL, _cjsonx_reflection_item_ptrint);
}

void cjsonx_reflection_ptr_free(void* list, const cjsonx_reflect_t* tbl) {
    /* 调用loofield迭代结构体中的属性,释放字符串和数组申请的内存空间 */
    cjsonx_reflection_loop(list, tbl, NULL, _cjsonx_reflection_item_free);
}

#endif

#ifndef __CJSONX_IMPLEMENTATIONS_
#define __CJSONX_IMPLEMENTATIONS_

cJSON* cjson_impl_object_get(const cJSON* object, const char* key) {
    return cJSON_GetObjectItemCaseSensitive(object, key);
}

cjsonx_type_e cjson_impl_typeof(cJSON* object) {
    switch ((object)->type) {
        case cJSON_Invalid:
        case cJSON_NULL:
            return CJSONX_NULL;
        case cJSON_False:
            return CJSONX_FALSE;
        case cJSON_True:
            return CJSONX_TRUE;
        case cJSON_Number:
            return CJSONX_REAL;
        case cJSON_String:
        case cJSON_Raw:
            return CJSONX_STRING;
        case cJSON_Array:
            return CJSONX_ARRAY;
        case cJSON_Object:
            return CJSONX_OBJECT;
        default:
            return CJSONX_NULL;
    }
}

cJSON* cjson_impl_parse(const char* buffer, size_t buflen) {
    cJSON* ret = NULL;
    ret = cJSON_ParseWithLength(buffer, buflen);
    if (!ret) {
        CJSONX_DBG("\e[0;35mJson string pasing error: %s\e[0m\r\n", cJSON_GetErrorPtr());
    }
    return ret;
}

void cjson_impl_delete(cJSON* object) { cJSON_Delete(object); }

const char* cjson_impl_string_value(const cJSON* object) {
    return cJSON_GetStringValue(object);
}

size_t cjson_impl_string_length(const cJSON* object) {
    return strlen(cJSON_GetStringValue(object));
}

long long cjson_impl_integer_value(const cJSON* object) {
    return (object)->valueint;
}

double cjson_impl_real_value(const cJSON* object) {
    return (object)->valuedouble;
}

char cjson_impl_bool_value(const cJSON* object) {
    return (object)->type == cJSON_True;
}

size_t cjson_impl_array_size(const cJSON* object) {
    return (size_t)cJSON_GetArraySize(object);
}

cJSON* cjson_impl_array_get(const cJSON* object, size_t index) {
    return cJSON_GetArrayItem(object, (int)index);
}

cJSON* cjson_impl_object(void) { return cJSON_CreateObject(); }

char* cjson_impl_to_string(cJSON* object) {
    return cJSON_PrintUnformatted(object);
}

int cjson_impl_to_string_bufferred(cJSON* object, char* buf, const int size) {
    return cJSON_PrintPreallocated(object, buf, size, 0);
}

cJSON* cjson_impl_integer(long long val) {
    cJSON* tmp = cJSON_CreateNumber(0);
    cJSON_SetNumberValue(tmp, val);
    return tmp;
}

cJSON* cjson_impl_string(const char* val) { return cJSON_CreateString(val); }

cJSON* cjson_impl_bool(char val) { return cJSON_CreateBool(val); }

cJSON* cjson_impl_real(double val) { return cJSON_CreateNumber(val); }

cJSON* cjson_impl_array(void) { return cJSON_CreateArray(); }

int cjson_impl_array_add(cJSON* array, cJSON* obj) {
    cJSON_AddItemToArray(array, obj);
    return 0;
}

int cjson_impl_object_set_new(cJSON* rootObj, const char* field, cJSON* obj) {
    cJSON_AddItemToObject(rootObj, field, obj);
    return 0;
}

#endif
