CC := clang
CFLAGS := -std=gnu2x -O3 -Weverything -Wno-declaration-after-statement -Werror

SRCS := $(shell find . -type f -name '*.c')
OBJS := $(patsubst %.c,%.o,$(SRCS))
TARGET := slob

.PHONY: dev clean

dev: $(TARGET)
	./$(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJS)
