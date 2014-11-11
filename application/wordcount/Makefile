include ../Makefile

CXXFLAGS += -DDATA_PREFETCH

TARGET=testprime wordcount old_wc mpwordcount

all:$(TARGET)

wordcount:$(DEPEND) $(LIB)

old_wc:$(DEPEND) $(LIB)

mpwordcount:$(DEPEND) $(LIB)

.PHONY:clean
clean:
	rm -rf $(TARGET)

