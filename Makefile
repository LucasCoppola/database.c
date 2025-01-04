CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = simple-db

all: $(TARGET)

$(TARGET): main.o db.o
    $(CC) $(CFLAGS) -o $(TARGET) main.o db.o

main.o: main.c db.h
db.o: db.c db.h

clean:
    rm -f *.o $(TARGET)
