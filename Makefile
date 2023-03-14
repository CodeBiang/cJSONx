CC = gcc
RM = rm -rf

OBJS := cJSONx.o cJSON.o

LIB_NAME = cjsonx
CJSONX_STATIC = lib$(LIB_NAME).a
CJSONX_SHARED = lib$(LIB_NAME).so


CFLAGS = -Wall -Werror -fPIC -lm

.PHONY: clean static shared

all: static shared

static: $(CJSONX_STATIC)

shared: $(CJSONX_SHARED)

$(CJSONX_STATIC): $(OBJS)
	@ $(AR) rcs $@ $^

$(CJSONX_SHARED): $(OBJS)
	@ $(CC) -shared -o $@ $(CFLAGS) $^

$(OBJS): %.o : %.c
	@ $(CC) $(CFLAGS)  -c $< -o $@

clean:
	@ $(RM) *.o *.a *.so
