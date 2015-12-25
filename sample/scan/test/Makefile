CC := gcc

CFLAGS := -I. -I../src/build/include/
CFLAGS += -DMOUNTED_HDIDSK
LDFLAGS := -lm -L../src/build/library -lscand

SRCDIRS := $(shell ls -l | grep "^d" | awk {'print $8'} )
#SRCDIRS := $(shell ls | grep -v Makefile )

SRCS := $(wildcard *.c)
#SRCS += $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))

OBJS := $(patsubst %.c, %.o, $(SRCS))

INCS := $(wildcard *.h)
INCS += $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.h))

#SLIB_OBJS += $(THIRDPARTY_TOPDIR)/library/libmopenssl.a
TARGET=tscan
%.o: %.c
	@echo "Compile $@"
	${CC} ${CFLAGS} -c -o $@ $<
all: ${TARGET}
${TARGET}: ${OBJS}
	${CC} ${OBJS} ${LDFLAGS} -o $@
clean:	
	@rm -fr ${TARGET} ${OBJS}
