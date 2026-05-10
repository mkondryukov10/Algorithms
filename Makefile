CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = tankers

all: $(TARGET)

$(TARGET): main.c stb_image.h
	$(CC) $(CFLAGS) main.c -o $(TARGET) -lm

run: $(TARGET)
	./$(TARGET) image.png

clean:
	rm -f $(TARGET)