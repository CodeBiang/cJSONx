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