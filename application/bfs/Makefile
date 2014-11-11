include ../Makefile

CXXFLAGS += -DDATA_PREFETCH

TARGET=bfs naivebfs mpbfs

all:$(TARGET)

bfs:$(DEPEND) $(LIB)

naivebfs:$(DEPEND) $(LIB)

mpbfs:$(DEPEND) $(LIB)

.PHONY:clean
clean:
	rm -rf $(TARGET)

