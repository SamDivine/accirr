PREFETCH := -DDATA_PREFETCH

MODE := -DPREFECH_MODE=1

LOCALICY := -DPREFETCH_LOCALITY=1

CXXFLAGS += $(PREFETCH) $(MODE) $(LOCALICY) -pthread -lrt
TARGET=pointer_tracing old_pointer_tracing pttest mppttest pcm_pttest
include ../Makefile

