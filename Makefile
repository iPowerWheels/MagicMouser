CC = gcc
CFLAGS = -Wall -Wextra -ggdb -fno-omit-frame-pointer -O2 -std=gnu99
LIBS = -lsndfile -lm -lglfw -lpthread

SRCS = main.c core.c
OBJS = $(SRCS:.c=.o)
TARGET = magicmouser

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
