# Libraries and Directories
LIBS = -lm 
INCS = -Iinc 
CFLAGS = -O3 -Wall -Wno-unused-variable -Werror -fno-exceptions            \
-fstack-protector -Wl,-z,relro -Wl,-z,now -Wformat-security -Wpointer-arith \
-Winit-self -Wl,-O1 -Wl,--no-undefined -g 

# Source and Object Files
SRCS = src/binom_heap.c src/fib_heap.c src/bin_heap.c  src/mm.c src/dijkstra.c main.c
OBJS = src/binom_heap.o src/fib_heap.o src/bin_heap.o  src/mm.o src/dijkstra.o main.o
TARGET = dijkstrahw1



# Compilers
CC = gcc

# Default rule
default: $(TARGET)

# Linking rule
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDIR) $(LIBS)
ifdef NO_OBJS
	rm -f src/*.o
endif

# Compilation rules
src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(INCS)

main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@ $(INCS)

# Dependency generation
depend: $(SRCS)
	makedepend -- $(SRCS)

# Clean rule
clean:
	rm -f *~ src/*.o main.o  $(TARGET)
