CC = gcc
RM = rm -rf

CJSONX_PATH = ../..

TARGET = complex

OBJS := main.o
CJSONX_OBJS := cJSON.o cJSONx.o

CFLAGS = -Wall -Werror -fPIC -lm

.PHONY: clean

all: $(TARGET)

$(TARGET): $(OBJS) $(CJSONX_OBJS)
	$(CC) $(CFLAGS) -I. -I$(CJSONX_PATH) $^ -o $@

$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) -I. -I$(CJSONX_PATH) -c $< -o $@

$(CJSONX_OBJS): %.o : $(CJSONX_PATH)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@ $(RM) *.o $(TARGET)

