include $(BUILDER_HOME)/builder.mk

CFLAGS += -Wall -O2
out := libplutil.a
inc := plsdk/log.h plsdk/plconfig.h

include $(BUILDER_HOME)/lib.mk
