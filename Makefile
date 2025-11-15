CC = gcc

DEBUG_FLAGS = -Wall -Wextra -Werror -ggdb -O2 -std=gnu99 \
	      -fno-omit-frame-pointer -D_FORTIFY_SOURCE=2 -fstack-protector-all \
	      -fPIE -pie -fsanitize=address,undefined
	      
RELEASE_FLAGS = -Wall -Wextra -Werror -ggdb -O3 -std=gnu99 \
		-march=native \
		-fstack-protector-strong \
		-fPIE \
		-D_FORTIFY_SOURCE=2

LIBS = -lsndfile -lm -lglfw -lpthread

SRCS = main.c core.c
OBJS = $(SRCS:.c=.o)
TARGET = magicmouser

all: release

release: CFLAGS = $(BASE_FLAGS) $(RELEASE_FLAGS)
release: $(TARGET)

debug: CFLAGS = $(BASE_FLAGS) $(DEBUG_FLAGS)
debug: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	
run: release
	./$(TARGET)
	
clean:
	rm -f $(OBJS) $(TARGET) core *.core *.out
	
.PHONY: all clean run debug release

help:
	@echo "Usage: make [target]"
	@echo "Targets:"
	@echo "  all      Compile the project"
	@echo "  debug    Compile in debugger the project "
	@echo "  run      Execute the program"
	@echo "  clean    Remove compiled files"
	@echo "Tip: Use 'make -j4' for faster parallel compilation"

