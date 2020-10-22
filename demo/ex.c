#include <stdio.h>
#include <string.h>
#include "cJSONx.h"

typedef struct {
    int a;
    int b;
    int c;
    int d;
    int e;

    int arrcnt;
    int arrb[10];
} cjx_ex_t;

cjsonx_reflect_t ex_reflect[] = {
    __cjsonx_int_ex(cjx_ex_t, a)
        __nullable
        __serialized
        __deserialized
        __cjsonx_ex_end,// 可空, 序列化, 反序列化
    __cjsonx_int_ex(cjx_ex_t, b)
        __serialized
        __deserialized
        __cjsonx_ex_end,// 不可空, 序列化, 反序列化
    __cjsonx_int_ex(cjx_ex_t, c)
        __nullable
        __deserialized
        __cjsonx_ex_end,// 可空, 不序列化, 反序列化
    __cjsonx_int_ex(cjx_ex_t, d)
        __nullable
        __deserialized
        __cjsonx_ex_end,// 可空, 不序列化, 反序列化
    __cjsonx_int_ex(cjx_ex_t, e)
        __nullable
        __serialized
        __cjsonx_ex_end,// 可空, 序列化, 不反序列化
    __cjsonx_array_int_ex(cjx_ex_t, arrb, arrcnt)
        __nullable
        __deserialized
        __serialized_name("AAAA")
        __serialized
        __cjsonx_ex_end,// 可空, 序列化, 反序列化, 序列化映射名称为“AAAA”
    __cjsonx_end()
};

int main(int argc, char* argv[]) {
    cjx_ex_t ex;
    const char* json_cannot_null = "{\"a\":1,\"c\":3,\"d\":4,\"e\":5,\"AAAA\":[1,2,3]}";
    printf("%s\r\n", cjsonx_err_str(cjsonx_str2struct(json_cannot_null, &ex, ex_reflect)));

    const char* json = "{\"a\":1, \"b\":2, \"c\":3,\"d\":4,\"e\":5,\"AAAA\":[1,2,3]}";
    memset(&ex, 0, sizeof(cjx_ex_t));
    cjsonx_str2struct(json, &ex, ex_reflect);
    cjsonx_reflection_print(&ex, ex_reflect);
    char p[1024] = {0};
    if (ERR_CJSONX_NONE == cjsonx_struct2str_bufferred(p, sizeof(p), &ex, ex_reflect)) {
        printf("%s\r\n", p);
    }
    return 0;
}
