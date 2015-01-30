CXX=g++

ACCIRR_HOME := /home/localhost/accirr

PCM_DIR := $(ACCIRR_HOME)/third_party/intelpcm
PCM_DEPEND += cpucounters.cpp client_bw.cpp pci.cpp msr.cpp utils.cpp

vpath %.cpp $(ACCIRR_HOME)/third_party/intelpcm

CXXFLAGS += -O2 -I$(PCM_DIR) -fopenmp -pthread -lrt -DUSING_MALLOC

TARGET=pointer_tracing old_pointer_tracing pttest pcm_pttest multiref_compiler multiref_source pcm_multiref_compiler pcm_multiref_source mpmultiref_source mppttest repeatpttest loadspttest jmppttest neworgnode newnodejmppttest

all: $(TARGET)

pcm_pttest:$(PCM_DEPEND)

newpcm_pttest:$(PCM_DEPEND)

pcm_multiref_compiler:$(PCM_DEPEND)

pcm_multiref_source:$(PCM_DEPEND)

.PHONY:clean
clean:
	rm -f $(TARGET)

