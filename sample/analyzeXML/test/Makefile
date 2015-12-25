CC := gcc

CFLAGS := -I. -I../build/include -I../lib/thirdparty/include/
LDFLAGS := -L../build/library -lcxml2
LDFLAGS += -L../lib/thirdparty/lib -lxml2
#SLIB_OBJS := ../lib/thirdparty/lib/libxml2.a

#SRCDIRS := $(shell ls -l | grep "^d" | awk {'print $8'} )
SRCDIRS := $(shell ls | grep -v Makefile )

SRCS := $(wildcard *.c)
SRCS += $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))

OBJS := $(patsubst %.c, %.o, $(SRCS))

INCS := $(wildcard *.h)
INCS += $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.h))

TARGET=txml
%.o: %.c
	@echo "Compile $@"
	${CC} ${CFLAGS} -c -o $@ $<
all: ${TARGET}
${TARGET}: ${OBJS}
	${CC} ${OBJS} ${SLIB_OBJS} ${LDFLAGS} -o $@
clean:	
	@rm -fr ${TARGET} ${OBJS}
