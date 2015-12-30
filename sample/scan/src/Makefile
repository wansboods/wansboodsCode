CC := gcc
DFLAGS := -shared
CFLAGS := -Wall -fPIC
CFLAGS += -I. -Iclib -Isdir -Isfile -Ishd
CFLAGS += -DMOUNTED_HDIDSK

LDFLAGS :=

#SRCDIRS := $(shell ls -l | grep "^d" | awk {'print $8'} )
SRCDIRS := $(shell ls | grep -v Makefile )

SRCS := $(wildcard *.c)
SRCS += $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))

OBJS := $(patsubst %.c, %.o, $(SRCS))

INCS := $(wildcard *.h)
INCS += $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.h))

#SLIB_OBJS += $(THIRDPARTY_TOPDIR)/library/libmopenssl.a
TARGET_LIBRARY=build/library
TARGET_INCLUDE=build/include

OBJ_LIB_DIR=.libs
TARGET=${OBJ_LIB_DIR}/libscand.so

%.o: %.c
	@echo "Compile $@"
	${CC} ${CFLAGS} -c -o $@ $<

all: ${TARGET}
${TARGET}: ${OBJS}
	@echo "COMPILE $@"
	@mkdir -p $(OBJ_LIB_DIR)
	@${CC} ${DFLAGS} ${LDFLAGS} -o $@ ${OBJS}
	@echo "INSTALL ${TARGET}"
	@mkdir -p ${TARGET_LIBRARY};cp ${TARGET} ${TARGET_LIBRARY} -afr
	@echo "INSTALL ${INCS}"
	@mkdir -p ${TARGET_INCLUDE};cp ${INCS} ${TARGET_INCLUDE} -afr
clean:
	@rm -fr ${TARGET} ${OBJS}
