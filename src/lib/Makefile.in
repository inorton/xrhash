ifeq ($(SRCROOT),)
		SRCROOT=${CURDIR}
endif

include ${SRCROOT}/platform.make

OBJS=xrhash.o xrhash_fast.o
LIBNAME=xrhash

STATIC_TARGET=$(LIBPREFIX)$(LIBNAME).$(STATIC_SUFFIX)
XRHASHLIB=${CURDIR}/$(STATIC_TARGET)

export XRHASHLIB
export SRCROOT
CFLAGS+=-I${CURDIR}
export CFLAGS

all: $(STATIC_TARGET)

clean:
	@rm -f $(OBJS) $(STATIC_TARGET)
	$(MAKE) -C tests clean

test: $(STATIC_TARGET) 
	$(MAKE) -e -C tests SRCROOT='${SRCROOT}' 
	$(MAKE) -e -C tests test


$(STATIC_TARGET): $(OBJS)
	ar ru $@ $^
	ranlib $@


