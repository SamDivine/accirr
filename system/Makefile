AS=g++ -c

FLAGS= -O2

TARGET=stack.o

all:$(TARGET)

%.o: %.S
	$(AS) $*.S

.PHONY:clean 
clean:
	rm $(TARGET)


