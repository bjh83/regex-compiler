PACKAGE= regex-compiler
VERSION= 0.1
RELEASE_DIR= .
RELEASE_FILE= $(PACKAGE)-$(VERSION)
OBJS= regex-compiler.o regex-test.o
CC= gcc

.PHONY: all

all: $(PACKAGE)

$(PACKAGE): $(OBJS)
	$(CC) -o $(PACKAGE) $(OBJS)

regex-compiler.o: regex.h regex-compiler.c

regex-test.o: regex.h regex-test.c

clean:
	rm -rf $(PACKAGE) $(OBJS)

.PHONY: clean

