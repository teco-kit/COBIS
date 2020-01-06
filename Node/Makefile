PBS=../ParticleBaseSystem

CHEM?=AUTRAN

build:$(CHEM:%=node_%.hex)

include $(PBS)/Mk/ccsc.mk

CFLAGS+=I+=cobis

ifdef debug
CFLAGS+=+GCATEYE_DEBUG=1
endif


install: node_$(CHEM).hex

node_%.c:node.c
	printf '#define OWN_GOOD_TYPE %s\n#include "%s"\n' $(subst node_,,$(@:.c=)) $< >$(@)

all: 
	$(MAKE) $(MAKE_ARGS) CHEM=AUTRAN_MBX\ ENERGEAR_HYPO_90\ ENERGOL_GR_XP_150\ WATER\ EMPTY

clean:
	rm -f node_*.c

.PRECIOUS: node_%.c
