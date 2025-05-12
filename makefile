CC = gcc
CFLAGS = -g -Wall -lm
LDFLAGS =
OBJFILES = webserver.o handle_calc.o handle_static.o
TARGET = webserver

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJFILES)

clean:
	rm -f $(OBJFILES) $(TARGET)