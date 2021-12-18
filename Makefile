XENO_CONFIG := /usr/xenomai/bin/xeno-config
CFLAGS := $(shell $(XENO_CONFIG) --SI_PVC_CNC_Controller --cflags)
LDFLAGS := $(shell $(XENO_CONFIG) --SI_PVC_CNC_Controller --ldflags)
CC := $(shell $(XENO_CONFIG) --cc)

EXECUTABLE := SI_PVC_CNC_Controller

all: $(EXECUTABLE)

%: %.c
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)
