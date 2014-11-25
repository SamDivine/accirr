include ../Makefile

PCM_DEPEND += cpucounters.cpp client_bw.cpp pci.cpp msr.cpp utils.cpp

PREFETCH := -DDATA_PREFETCH

MODE := -DPREFECH_MODE=0

LOCALICY := -DPREFETCH_LOCALITY=0

CXXFLAGS += $(PREFETCH) $(MODE) $(LOCALICY) -pthread -lrt -DUSING_MALLOC
TARGET=pointer_tracing old_pointer_tracing pttest mppttest pcm_pttest

all:$(TARGET)

pointer_tracing:$(DEPEND) $(LIB)

old_pointer_tracing:$(DEPEND) $(LIB)
	
pttest:$(DEPEND) $(LIB)

mppttest:$(DEPEND) $(LIB)

pcm_pttest:$(DEPEND) $(LIB) $(PCM_DEPEND)


.PHONY:clean
clean:
	rm -rf $(TARGET)

