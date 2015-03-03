include ../Makefile

CXXFLAGS += -DDATA_PREFETCH -DCORO_DEBUG

TARGET=bfs naivebfs mpbfs threadbfs

all:$(TARGET)

bfs:$(DEPEND) $(LIB)

naivebfs:$(DEPEND) $(LIB)

mpbfs:$(DEPEND) $(LIB)

.PHONY:clean
clean:
	rm -rf $(TARGET)

