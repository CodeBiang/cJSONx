# cJSONx
Enhanced JSON converter based on cJSON


## Dependency & Reference

This libaray is depend on [cJSON](https://github.com/DaveGamble/cJSON)<br/>
Some implementation referred to [cson](https://github.com/sunchb/cson)

## Get cJSONx

### Download the source code
``` shell
git clone git@github.com:CodeBiang/cJSONx.git
cd cJSONx
```
### Building
You can incorporate cJSONx by several ways:
#### 1. Copying the source
Notice:<br/>
&emsp;&emsp;`cJSON` is according to `ANSI-C(c89)`.<br/>
&emsp;&emsp;But if you wanna embed `cJSONx` source code into your project, you can just use `c99` or above.<br/>
#### 2. make
&emsp;&emsp;You can compile libaries and demo by these commands:
``` shell
cd cJSONx
make
make demo
```
&emsp;&emsp;You can refer to [cJSON](https://github.com/DaveGamble/cJSON) for advanced building. <br/>

## Usage
&emsp;&emsp;Due to the nature of C language memory allocation, data structure convertion should consider how to map the memory.
&emsp;&emsp;cJSONx use reflection `cjsonx_reflection_t` to save struct memory information, so you can easily implement the conversation between json string and struct.

### Basic Usage
Define the data type:
```c
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
```
Reflect data fields:
```c
cjsonx_reflect_t simp_reflect[] = {
    __cjsonx_int(cjx_simp_t, a),
    __cjsonx_end()
};

const cjsonx_reflect_t basic_reflect[] = {
    __cjsonx_int(cjx_basic_t, i),
    __cjsonx_bool(cjx_basic_t, b),
    __cjsonx_real(cjx_basic_t, d),
    __cjsonx_real(cjx_basic_t, f),
    // dynamic allocated string(pointer)
    __cjsonx_str_ptr(cjx_basic_t, sptr),
    // preallocated string(char array)
    __cjsonx_str(cjx_basic_t, sbuf),
    // preallocated struct
    __cjsonx_object(cjx_basic_t, obj, simp_reflect),
    // dynamic allocated struct(pointer)
    __cjsonx_object_ptr(cjx_basic_t, objp, simp_reflect),
    __cjsonx_end()
};
```
Convertion:
```c
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

int main(int argc, char* argv[]) {
    cjx_basic_t basic;
    int ret;
    ret = cjsonx_str2struct(json, &basic, basic_reflect);
    if (ret == ERR_CJSONX_NONE) {
        cjsonx_reflection_print(&basic, basic_reflect);
        // Free the memory allocated
        cjsonx_reflection_ptr_free(&basic, basic_reflect);
    }
    return 0;
}
```
&emsp;&emsp;If your struct has a pointer field, remember use `cjsonx_reflection_ptr_free` to free them

### Extention Usage

&emsp;&emsp;You can better customize your data mapping relationships with cJSONx extention usages `__cjsonx_xx_ex`<br/>
&emsp;&emsp;This is a demo:
```c
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
        __cjsonx_ex_end,
    __cjsonx_int_ex(cjx_ex_t, b)
        __serialized
        __deserialized
        __cjsonx_ex_end,
    __cjsonx_int_ex(cjx_ex_t, c)
        __nullable
        __deserialized
        __cjsonx_ex_end,
    __cjsonx_int_ex(cjx_ex_t, d)
        __nullable
        __deserialized
        __cjsonx_ex_end,
    __cjsonx_int_ex(cjx_ex_t, e)
        __nullable
        __serialized
        __cjsonx_ex_end,
    __cjsonx_array_int_ex(cjx_ex_t, arrb, arrcnt)
        __nullable                  // Can be null
        __deserialized              // Will be deserialized
        __serialized_name("AAAA")   // Field name mapping
        __serialized                // Will be serialized
        __cjsonx_ex_end,
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
```

Result:
``` c
field not found
a:1
b:2
c:3
d:4
e:0
arrcnt:3
0integer:1
0integer:2
0integer:3
{"a":1,"b":2,"e":0,"AAAA":[1,2,3]}
```