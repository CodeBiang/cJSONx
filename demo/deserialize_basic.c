#include <stdio.h>
#include "cJSONx.h"

typedef struct cjx_simp_t {
    int a;
} cjx_simp_t;

typedef struct cjx_basic_t {
    int i;
    bool b;
    double d;
    float f;
    char* sptr;
    char sbuf[20];
    struct cjx_simp_t obj;
    struct cjx_simp_t* objp;
} cjx_basic_t;

const char* json = "{"
    "\"i\":1,"
    "\"b\":true,"
    "\"d\":1.222,"
    "\"f\":2.333,"
    "\"sbuf\":\"Hello World\","
    " \"sptr\":\"Hello C\","
    "\"obj\":{\"a\":2},"
    "\"objp\":{\"a\":3}"
"}";


cjsonx_reflect_t simp_reflect[] = {
    __cjsonx_int(cjx_simp_t, a),
    __cjsonx_end()
};

const cjsonx_reflect_t basic_reflect[] = {
    __cjsonx_int(cjx_basic_t, i),
    __cjsonx_bool(cjx_basic_t, b),
    __cjsonx_real(cjx_basic_t, d),
    __cjsonx_real(cjx_basic_t, f),
    __cjsonx_str_ptr(cjx_basic_t, sptr),
    __cjsonx_str(cjx_basic_t, sbuf),
    __cjsonx_object(cjx_basic_t, obj, simp_reflect),
    __cjsonx_object_ptr(cjx_basic_t, objp, simp_reflect),
    __cjsonx_end()
};

int main(int argc, char* argv[]) {
    cjx_basic_t basic;
    cjsonx_str2struct(json, &basic, basic_reflect);
    cjsonx_reflection_print(&basic, basic_reflect);
    cjsonx_reflection_ptr_free(&basic, basic_reflect);
    return 0;
}