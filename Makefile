CC=gcc
CFLAGS=-O2 -Wall -Wextra -Werror
RM=rm -f

# test flags
TFLAGS=-O0 -g3 -Wall -Wextra

TARGET=bs

# no need to change anything below this line

ifdef DEBUG
	CFLAGS=$(TFLAGS)
endif

OBJS=$(patsubst %.c,%.o,$(wildcard b*.c))

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

clean:
	$(RM) $(OBJS) $(TARGET)

.PHONY: clean

$(foreach target,$(target),$(shell $(CC) -MM b*.c))
