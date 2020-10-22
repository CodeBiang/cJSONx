#include <stdio.h>
#include "cJSONx.h"

typedef struct cjx_simp_t {
    int a;
} cjx_simp_t;

cjsonx_reflect_t simp_reflect[] = {
    __cjsonx_int(cjx_simp_t, a),
    __cjsonx_end()
};

typedef struct cjx_arrp_t {
    int ui16cnt;
    unsigned short* ui16;

    int icnt;
    int* i;

    int bcnt;
    bool* b;

    int dcnt;
    double* d;

    int fcnt;
    float* f;
    
    int strcnt;
    char* str[20];

    int strpcnt;
    char** strp;

    int objcnt;
    cjx_simp_t* obj;
} cjx_arrp_t;

cjsonx_reflect_t arrp_reflection[] = {
    __cjsonx_array_ptr_int(cjx_arrp_t, ui16, ui16cnt),
    __cjsonx_array_ptr_int(cjx_arrp_t, i, icnt),
    __cjsonx_array_ptr_bool(cjx_arrp_t, b, bcnt),
    __cjsonx_array_ptr_real(cjx_arrp_t, d, dcnt),
    __cjsonx_array_ptr_real(cjx_arrp_t, f, fcnt),
    __cjsonx_array_str_ptr(cjx_arrp_t, str, strcnt),
    __cjsonx_array_ptr_str_ptr(cjx_arrp_t, strp, strpcnt),
    __cjsonx_array_ptr_object(cjx_arrp_t, obj, objcnt, simp_reflect),
    __cjsonx_end()
};

typedef struct cjx_arrb_t {
    int ui16cnt;
    unsigned short ui16[10];

    int icnt;
    int i[10];

    int bcnt;
    bool b[10];

    int dcnt;
    double d[10];

    int fcnt;
    float f[10];
    
    int strcnt;
    char str[10][20];

    int strpcnt;
    char* strp[10];

    int objcnt;
    cjx_simp_t obj[10];
} cjx_arrb_t;

cjsonx_reflect_t arrb_reflection[] = {
    __cjsonx_array_int(cjx_arrb_t, ui16, ui16cnt),
    __cjsonx_array_int(cjx_arrb_t, i, icnt),
    __cjsonx_array_bool(cjx_arrb_t, b, bcnt),
    __cjsonx_array_real(cjx_arrb_t, f, fcnt),
    __cjsonx_array_real(cjx_arrb_t, d, dcnt),
    __cjsonx_array_str(cjx_arrb_t, str, strcnt),
    __cjsonx_array_str_ptr(cjx_arrb_t, strp, strpcnt),
    __cjsonx_array_object(cjx_arrb_t, obj, objcnt, simp_reflect),
    __cjsonx_end()
};



const char* arr_json = "{"
    "\"ui16\":[15, 120, 11, 16],"
    "\"i\":[-20, 21, 22, 23],"
    "\"b\":[true, false, true, false],"
    "\"d\":[2, 3, 4, 5.4],"
    "\"f\":[1, 2, 2, 3, 4.6],"
    "\"strp\":[\"hhhhh\", \"fjak\"],"
    "\"str\":[\"aaaaa\"],"
    "\"obj\":[{\"a\":1},{\"a\":1},{\"a\":1}]"
"}";

void arr_bufferred_test() {
    cjx_arrb_t arrb = {};
    int i;
    cjsonx_str2struct(arr_json, &arrb, arrb_reflection);
    printf("\r\ni:");
    for (i = 0; i < arrb.icnt; i++) {
        printf("%d ", arrb.i[i]);
    }
    printf("\r\nb:");
    for (i = 0; i < arrb.bcnt; i++) {
        printf("%d ", arrb.b[i]);
    }
    printf("\r\nd:");
    for (i = 0; i < arrb.dcnt; i++) {
        printf("%f ", arrb.d[i]);
    }
    printf("\r\nf:");
    for (i = 0; i < arrb.fcnt; i++) {
        printf("%f ", arrb.f[i]);
    }
    printf("\r\nstrp:");
    for (i = 0; i < arrb.strpcnt; i++) {
        printf("%s ", arrb.strp[i]);
    }
    printf("\r\nstrb:");
    for (i = 0; i < arrb.strcnt; i++) {
        printf("%s ", arrb.str[i]);
    }
    printf("\r\nobj:");
    for (i = 0; i < arrb.strcnt; i++) {
        printf("{a:%d} ", arrb.obj[i].a);
    }
    printf("\r\n");
}


void arr_ptr_test() {
    cjx_arrp_t arrp = {};
    int i;
    cjsonx_str2struct(arr_json, &arrp, arrp_reflection);
    printf("\r\ni:");
    for (i = 0; i < arrp.icnt; i++) {
        printf("%d ", arrp.i[i]);
    }
    printf("\r\nb:");
    for (i = 0; i < arrp.bcnt; i++) {
        printf("%d ", arrp.b[i]);
    }
    printf("\r\nd:");
    for (i = 0; i < arrp.dcnt; i++) {
        printf("%f ", arrp.d[i]);
    }
    printf("\r\nf:");
    for (i = 0; i < arrp.fcnt; i++) {
        printf("%f ", arrp.f[i]);
    }
    printf("\r\nstrp:");
    for (i = 0; i < arrp.strpcnt; i++) {
        printf("%s ", arrp.strp[i]);
    }
    printf("\r\nstrb:");
    for (i = 0; i < arrp.strcnt; i++) {
        printf("%s ", arrp.str[i]);
    }
    printf("\r\nobj:");
    for (i = 0; i < arrp.strcnt; i++) {
        printf("{a:%d} ", arrp.obj[i].a);
    }
    printf("\r\n");
}

int main(int argc, char* argv[]) {
    
    arr_ptr_test();
    
    arr_bufferred_test();
}

