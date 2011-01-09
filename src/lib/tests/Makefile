include ${SRCROOT}/platform.make

OBJS=testutils.o xrhash-test.o
TARGET=xrhash-test
LIBS=$(XRHASHLIB)

all: $(TARGET)

clean:
	@rm -f $(OBJS) $(TARGET)

test: $(TARGET)
	./$(TARGET)

$(TARGET): $(OBJS) $(XRHASHLIB)
	$(CC) -o $@ $^
