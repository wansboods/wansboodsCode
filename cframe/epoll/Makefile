CC := gcc
CFLAGS := -I.
CFLAGS += -Ithirdparty/mysql
CFLAGS += -Ithirdparty/basic/include
CFLAGS += -Ithirdparty/libxml2/include/libxml2
CFLAGS += -Ithirdparty/pkmysql/include
CFLAGS += -Ithirdparty/mysql/include
CFLAGS += -Ithirdparty/cJSON/include


LDFLAGS := -Lthirdparty/basic/library -lhdbs
LDFLAGS += -Lthirdparty/pkmysql/library -lpkmysql
LDFLAGS += -Lthirdparty/mysql/library -lmysqlclient
LDFLAGS += -Lthirdparty/libxml2/library -lxml2
LDFLAGS += -Lthirdparty/cJSON/library -lcjson

#SRCDIRS := $(shell ls -l | grep "^d" | awk {'print $8'} )
SRCDIRS := $(shell ls | grep -v Makefile )

SRCS := $(wildcard *.c)
SRCS += $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))

OBJS := $(patsubst %.c, %.o, $(SRCS))

INCS := $(wildcard *.h)
INCS += $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.h))

#SLIB_OBJS += $(THIRDPARTY_TOPDIR)/library/libmopenssl.a
TARGET=tepoll
%.o: %.c
	@echo "Compile $@"
	${CC} ${CFLAGS} -c -o $@ $<
all: ${TARGET}
${TARGET}: ${OBJS}
	${CC} $(OBJS) ${LDFLAGS} -o $@
clean:	
	@rm -fr ${TARGET} ${OBJS}
