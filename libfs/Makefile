# Target library
lib := libfs.a

CC=gcc
CFLAGS=-g -O0

all: $(lib)

objs := disk.o fs.o

libfs.a: $(objs)
	@echo "AR	$@"
	ar rcs $@ $^

%.o: %.c
	@echo "CC	$@"
	@$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $< $(DEPFLAGS)

clean:
	@echo "CLEAN"
	@rm -f $(objs) $(lib)

