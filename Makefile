CC=gcc
RM=rm -rf

CFLAGS=-O2 -Wall -Wextra
# debug flags
DFLAGS=-O0 -g3 -Wall -Wextra -DB_ASSERT
# test flags, added to the debug flags
TFLAGS=--coverage

BIN_DIR=bin
TARGET=bs

OBJS=bio.o bmem.o blex.o bparser.o
TESTS=bmem bio blex

# no need to change anything below this line
TEST:=$(filter tests bin/%.test, $(MAKECMDGOALS))

ifdef ASSERTIONS
CFLAGS=-DB_ASSERT $(CFLAGS)
endif

ifdef DEBUG
CFLAGS=$(DFLAGS)
endif

ifdef TEST
CFLAGS=$(DFLAGS) $(TFLAGS)
endif


$(BIN_DIR)/$(TARGET): $(OBJS) $(TARGET).o | $(BIN_DIR)
	$(CC) $(CFLAGS)      -o $@ $^

$(BIN_DIR)/%.test: $(OBJS) tests/%.test.o | $(BIN_DIR)
	$(CC) $(CFLAGS)      -o $@ $^

.PHONY: tests clean docs

tests: $(foreach test,$(TESTS),$(BIN_DIR)/$(test).test) | $(BIN_DIR)
	@:

docs:
	doxygen

clean:
	$(RM) $(BIN_DIR) docs \
		$(wildcard b*.o) $(wildcard tests/b*.o) \
		$(wildcard b*.gcno) $(wildcard tests/b*.gcno) \
		$(wildcard b*.gcda) $(wildcard tests/b*.gcda)

$(foreach target,$(target),$(shell $(CC) -MM b*.c))

.SECONDARY:
$(foreach test_target,$(test_target),$(shell $(CC) -MM tests/b*.c))

$(BIN_DIR): ; mkdir $@
