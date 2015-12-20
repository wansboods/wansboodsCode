CC := gcc

CFLAGS := -I. -I../build/include
LDFLAGS := -L../build/library -lcjason
LDFLAGS += -lm

#SRCDIRS := $(shell ls -l | grep "^d" | awk {'print $8'} )
SRCDIRS := $(shell ls | grep -v Makefile )

SRCS := $(wildcard *.c)
SRCS += $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))

OBJS := $(patsubst %.c, %.o, $(SRCS))

INCS := $(wildcard *.h)
INCS += $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.h))

#SLIB_OBJS += $(THIRDPARTY_TOPDIR)/library/libmopenssl.a
TARGET=tcjason
%.o: %.c
	@echo "Compile $@"
	${CC} ${CFLAGS} -c -o $@ $<
all: ${TARGET}
${TARGET}: ${OBJS}
	${CC} ${LDFLAGS} ${OBJS} -o $@
clean:	
	@rm -fr ${TARGET} ${OBJS}
