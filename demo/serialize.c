#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../cJSONx.h"

struct cjx_serial_obj_t {
    int a;
};

struct cjx_serial_t {
    int i;
    float f;
    double d;
    char* strp;
    char strb[50];

    int iarrpcnt;
    int* iarrp;
    
    struct cjx_serial_obj_t obj;
    struct cjx_serial_obj_t* objp;

    int farrbcnt;
    float farrb[50];

    int apsp_cnt;
    char** apsp;
    
    int absp_cnt;
    char* absp[10];

    int absb_cnt;
    char absb[10][50];
};

const cjsonx_reflect_t serial_obj_reflect[] = {
    __cjsonx_int(struct cjx_serial_obj_t, a),
    __cjsonx_end()
};

const cjsonx_reflect_t serial_reflect[] = {
    /* Integer((unsigned)char/short/int/long) 整型 */
    __cjsonx_int(struct cjx_serial_t, i),
    /* Real Number(double/float) 实数 */
    __cjsonx_real(struct cjx_serial_t, f),
    __cjsonx_real(struct cjx_serial_t, d),
    /* String Pointer(malloc) 字符串指针(动态内存分配) */
    __cjsonx_str_ptr(struct cjx_serial_t, strp),
    /* String(Preallocated) 字符串(预先分配内存) */
    __cjsonx_str(struct cjx_serial_t, strb),
    /* Array Pointer(malloc)  动态内存分配的int数组 */
    __cjsonx_array_ptr_int(struct cjx_serial_t, iarrp, iarrpcnt),
    /* Array Preallocated 预分配实数数组 */
    __cjsonx_array_real(struct cjx_serial_t, farrb, farrbcnt),
    /* Object 其他结构体成员 */
    __cjsonx_object(struct cjx_serial_t, obj, serial_obj_reflect),
    /* Object Pointer 其他结构体成员指针 */
    __cjsonx_object_ptr(struct cjx_serial_t, objp, serial_obj_reflect),
    /* Pointer to a string pointer that dynamically allocates memory */
    /* 指向动态分配内存的字符串指针的指针 */
    __cjsonx_array_ptr_str_ptr(struct cjx_serial_t, apsp, apsp_cnt),
    /* 动态分配内存的字符串指针数组 */
    __cjsonx_array_str_ptr(struct cjx_serial_t, absp, absp_cnt),
    /* 预分配内存的字符串数组(char[m][n]) */
    __cjsonx_array_str(struct cjx_serial_t, absb, absb_cnt),
    __cjsonx_end()
};

int main(int argc, char* argv[]) {
    char* p = strdup("hhhh");
    int* iarr = malloc(sizeof(int) * 10);
    struct cjx_serial_obj_t* o = malloc(sizeof(struct cjx_serial_obj_t));
    int apsp_cnt = 2;
    char** apsp = malloc(sizeof(apsp) * apsp_cnt);
    
    iarr[0] = 1;
    iarr[1] = 2;
    o->a = 2;

    for (int i = 0; i < apsp_cnt; i++) {
        apsp[i] = strdup("test");
    }

    struct cjx_serial_t s = {
        .i = 1,
        .f = 1.8888f,
        .d = 1.8888,
        .strp = p,
        .strb = "3451", 
        .iarrpcnt = 2,
        .iarrp = iarr,
        .obj = {1},
        .objp = o,
        .farrbcnt = 3,
        .farrb = {1.4f, 3.0f, 5.5f},
        .apsp_cnt = apsp_cnt,
        .apsp = apsp,
        .absb_cnt = 3,
        .absb = {
            "hh", "hh", "hh2"
        },
        .absp_cnt = 2,
        .absp = {
            strdup("12"),
            strdup("23")
        }
    };

    if (cjsonx_struct2str(&p, &s, serial_reflect) == ERR_CJSONX_NONE) {
        printf("%s\r\n", p);
        cJSON_free(p);
    }

    // Free the memory allocated in heap
    // Pointer item (Not Null) should be able to be free, or it will lead to an error
    cjsonx_reflection_ptr_free(&s, serial_reflect);
    return 0;
}