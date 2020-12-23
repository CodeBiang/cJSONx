#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cJSONx.h"

typedef struct {
    int id;
    char name[20];
    float temprature;
    unsigned long tick;
    
    unsigned int ip;
    unsigned int netmask;
    unsigned int gateway;
} device_t;

const cjsonx_reflect_t device_reflection[] = {
    __cjsonx_int(device_t, id),
    __cjsonx_str(device_t, name),
    __cjsonx_real(device_t, temprature),
    __cjsonx_int(device_t, tick),

    __cjsonx_int(device_t, ip),
    __cjsonx_int(device_t, netmask),
    __cjsonx_int_ex(device_t, gateway)
        __serialized
        __deserialized
        __serialized_name("gw")
        __nullable
        __cjsonx_ex_end,
    
    __cjsonx_end()
};


int main(int argc, char* argv[]) {
    device_t device = {0};
    const char* json = "{"
        "\"id\":12,"
        "\"ip\":13,"
        "\"name\":\"template\","
        "\"gw\":15"
        "}";
    printf("%s\r\n", json);
    cjsonx_nstr2struct(json, strlen(json), &device, device_reflection);

    char* p;
    cjsonx_struct2str(&p, &device, device_reflection);

    printf("%s\r\n", p);
    free(p);
}