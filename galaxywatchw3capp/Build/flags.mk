
DEBUG_OP = 
CPP_DEBUG_OP = 

OPTIMIZATION_OP = -O3 
CPP_OPTIMIZATION_OP = 

COMPILE_FLAGS = $(DEBUG_OP) $(OPTIMIZATION_OP) -Wall -c -fmessage-length=0 

CPP_COMPILE_FLAGS = $(CPP_DEBUG_OP) $(CPP_OPTIMIZATION_OP) 

LINK_FLAGS = -s 

ifeq ($(STRIP_INFO),off)
LINK_FLAGS = 
else
ifeq ($(STRIP_INFO),on)
LINK_FLAGS = -s
endif
endif

AR_FLAGS = 

EDC_COMPILE_FLAGS = 