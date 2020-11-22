CC = gcc
RM = rm -rf

OBJS := cJSONx.o cJSON.o

CJSONX_DEMOS = deserialize_basic.demo deserialize_arr.demo ex.demo serialize.demo simple_demo.demo

LIB_NAME = cjsonx
CJSONX_STATIC = lib$(LIB_NAME).a
CJSONX_SHARED = lib$(LIB_NAME).so


CFLAGS = -Wall -Werror -fPIC -lm

.PHONY: clean static shared demo

all: static shared

static: $(CJSONX_STATIC)

shared: $(CJSONX_SHARED)

demo: $(CJSONX_DEMOS)
	

$(CJSONX_DEMOS): %.demo : demo/%.c
	$(CC) $(CFLAGS) $< cJSONx.c cJSON.c -o $@ -I.

$(CJSONX_STATIC): $(OBJS)
	@ $(AR) rcs $@ $^

$(CJSONX_SHARED): $(OBJS)
	@ $(CC) -shared -o $@ $(CFLAGS) $^

$(OBJS): %.o : %.c
	@ $(CC) $(CFLAGS)  -c $< -o $@

clean:
	@ $(RM) *.o *.a *.so *.demo
