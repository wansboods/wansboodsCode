#CC=arm-marvell-linux-gnueabi-gcc
#CFLAGS=-Wall -O3 -I. -I../include -I$(GALOIS_LINUX_ROOT)/usr/include -fPIC
CC := gcc -Wall -O2
CFLAGS := -I. -Ireadconfig -I../src/build/include/
LDFLAGS := -L../src/build/library -lscand
LDFLAGS += -ldl

#SRCDIRS := $(shell ls -l | grep "^d" | awk {'print $8'} )
SRCDIRS := $(shell ls | grep -v Makefile )

SRCS := $(wildcard *.c)
SRCS += $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))

OBJS := $(patsubst %.c, %.o, $(SRCS))

INCS := $(wildcard *.h)
INCS += $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.h))

TARGET=tscan
%.o: %.c
	@echo "Compile $@"
	${CC} ${CFLAGS} -c -o $@ $<
all: ${TARGET}
${TARGET}: ${OBJS}
	@echo "Compile $@"
	${CC} ${OBJS} ${LDFLAGS} -o $@
	@echo "Install ${TARGET}"
clean:
	@echo "Remove ${TARGET} ${OBJS}"
	@rm -fr ${TARGET}  ${OBJS}
