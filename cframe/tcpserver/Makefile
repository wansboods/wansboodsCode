CC := gcc
CFLAGS := -I.
CFLAGS += -Ithirdparty/zlog
CFLAGS += -Ithirdparty/basic/include
CFLAGS += -Ithirdparty/libxml2/include/
#CFLAGS += -Ithirdparty/pkmysql/include
#CFLAGS += -Ithirdparty/mysql/include
CFLAGS += -Ithirdparty/cJSON/include
CFLAGS += -Ithirdparty/analyzeXML/include
#CFLAGS += -Ithirdparty/trlv
#CFLAGS += -Ithirdparty/module
#CFLAGS += -Ithirdparty/mbarsys
#CFLAGS += -Ithirdparty/mbarmid

LDFLAGS := -lpthread -ldl -Lthirdparty/basic/library -lhdbs
#LDFLAGS += -Lthirdparty/pkmysql/library -lpkmysql
#LDFLAGS += -Lthirdparty/mysql/library -lmysqlclient
LDFLAGS += -Lthirdparty/libxml2/library -lxml2
LDFLAGS += -Lthirdparty/cJSON/library -lcjson
LDFLAGS += -Lthirdparty/analyzeXML/library -lcxml2
LDFLAGS += -Lthirdparty/zlog -lzlog
#LDFLAGS += -Lthirdparty/trlv/ -ltp_trlv
#LDFLAGS += -Lthirdparty/mbarsys -lmbarsys
#LDFLAGS += -Lthirdparty/module -lhdmod
#LDFLAGS += -Lthirdparty/mbarmid -lmbarmid

#SRCDIRS := $(shell ls -l | grep "^d" | awk {'print $8'} )
SRCDIRS := $(shell ls | grep -v Makefile )

SRCS := $(wildcard *.c)
SRCS += $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))

OBJS := $(patsubst %.c, %.o, $(SRCS))

INCS := $(wildcard *.h)
INCS += $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.h))

#SLIB_OBJS += $(THIRDPARTY_TOPDIR)/library/libmopenssl.a
TARGET=charingmanage
%.o: %.c
	@echo "Compile $@"
	${CC} ${CFLAGS} -c -o $@ $<
all: ${TARGET}
${TARGET}: ${OBJS}
	${CC} $(OBJS) ${LDFLAGS} -o $@
clean:	
	@rm -fr ${TARGET} ${OBJS}
