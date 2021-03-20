# cJSONx

<kbd>V1.0.1</kbd>

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
&emsp;&emsp;cJSONx use reflection `cjsonx_reflect_t` to save struct memory information, so you can easily implement the conversation between json string and struct.

### Simple Usage
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cJSONx.h"

struct device {
    int id;
    char name[20];
    float temprature;
    unsigned long tick;
    
    unsigned int ip;
    unsigned int netmask;
    unsigned int gateway;
};

const cjsonx_reflect_t device_reflection[] = {
    __cjsonx_int(struct device, id),
    __cjsonx_str(struct device, name),
    __cjsonx_real(struct device, temprature),
    __cjsonx_int(struct device, tick),
    __cjsonx_int(struct device, ip),
    __cjsonx_int(struct device, netmask),
    __cjsonx_int(struct device, gateway),
    
    __cjsonx_end()
};

static void simple_serialize();
static void simple_deserialize();

int main(int argc, char* argv[]) {
    simple_serialize();
    simple_deserialize();
}

void simple_serialize() {
    struct device d = {
        .id = 1,
        .ip = 0x0A01A8C0,
        .gateway = 0x0101A8C0,
        .netmask = 0x00FFFFFF,
        .name = "Hello World",
        .temprature = 36.2F,
        .tick = 123
    };
    char buf[300];
    int ret = cjsonx_struct2str_preallocated(buf, sizeof(buf), &d, device_reflection);
    if (ret == ERR_CJSONX_NONE) {
        printf("Serialized JSON string: %s\n", buf);
    }
}

void simple_deserialize() {
    const char* json = "{\"id\":1,\"name\":\"Hello World\",\"temprature\":36.2,\"tick\":123,\"ip\":167880896,\"netmask\":16777215,\"gateway\":16885952}";
    struct device d;
    int ret = cjsonx_str2struct(json, &d, device_reflection);
    if (ret == ERR_CJSONX_NONE) {
        printf("Deserialized struct: id[%d], name[%s], temprature[%f], tick[%ld], ip[%#08X], netmask[%#08X], gateway[%#08X]\n",
            d.id, d.name, d.temprature, d.tick, d.ip, d.netmask, d.gateway);
    }
}
```
&emsp;&emsp;If your struct has a pointer field, remember to use `cjsonx_reflection_ptr_free` to free them

### Extention Usage

&emsp;&emsp;You can better customize your data mapping relationships with cJSONx extention usages `__cjsonx_xx_ex`<br/>
&emsp;&emsp;This is a demo:
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cJSONx.h"

struct date {
    int year;
    int month;
    int day;
    int hour;
    int min;
    int sec;
};

enum booktype {
    BOOK_TECH = 0,
    BOOK_CARTOON
};

struct book {
    char* name;
    struct date time;
    enum booktype type;
    
    char isbn[11];

    char** authors;
    int author_cnt;
};


const cjsonx_reflect_t date_reflection[] = {
    __cjsonx_int(struct date, year),
    __cjsonx_int(struct date, month),
    __cjsonx_int(struct date, day),
    __cjsonx_int(struct date, min),
    __cjsonx_int(struct date, hour),
    __cjsonx_int(struct date, sec),
    __cjsonx_end()
};

const cjsonx_reflect_t book_reflection[] = {
    __cjsonx_str_ptr_ex(struct book, name),
    __cjsonx_object_ex(struct book, time, date_reflection, 
        __serialized(false)),
    __cjsonx_int(struct book, type),
    __cjsonx_str_ex(struct book, isbn, __serialized_name("ISBN")),
    __cjsonx_array_ptr_str_ptr(struct book, authors, author_cnt),
    __cjsonx_end()
};

int main(int argc, char* argv[]) {
    struct book b = {0};
    const char* json = "{\"name\":\"AAA\",\"ISBN\":\"1234567890\",\"type\":0"
        ",\"authors\":[\"a\",\"b\",\"c\"],\"time\":{\"year\":2020,\"month\":11"
        ",\"day\":10,\"hour\":12,\"min\":12,\"sec\":12}}";
    int ret = cjsonx_str2struct(json, &b, book_reflection);
    if (ret == ERR_CJSONX_NONE) {
        char* p;
        cjsonx_struct2str(&p, &b, book_reflection);
        printf("%s\n", p);
        cjsonx_reflection_ptr_free(&b, book_reflection);
        free(p);
    }
}
```
