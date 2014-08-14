TOP_DIR ?= /home/eason_weng/Qualcomm_LTE/code
include $(TOP_DIR)/include/host-build.mk

CFLAGS+=-I$(HOST_BUILD_DIR)/include -I ./include -I./cgi -I/home/eason_weng/Qualcomm_LTE/code/packages/build_package/gemtek-ui/cgi-bin/

#LDFLAGS+=-L$(HOST_BUILD_DIR)/lib -lm2m -lmosquitto -Wl,-rpath,$(HOST_BUILD_DIR)/lib -lkarmapb -lmysqlclient
#LDFLAGS += -ljansson -lssl -lz -lcrypto -L$(HOST_BUILD_DIR)/lib 

LDFLAGS+=-L$(HOST_BUILD_DIR)/lib -Wl,-rpath,$(HOST_BUILD_DIR)/lib -lmysqlclient
LDFLAGS += -ljansson -L$(HOST_BUILD_DIR)/lib 


# OUT_DIR=server 
CGIEXT = cgi
CGIFILE = ./www/cgi
OBJS = utils.o cgilib.o bill.o
CGI = $(CGIFILE)/api
# BIN_DIR = ./cgi/

.PHONY: all romfs clean

all: clean $(OBJS) $(CGI)
#	cp $(patsubst %,%.$(CGIEXT), $(CGI)) $(BIN_DIR)

$(OBJS):
	$(CC) $(CFLAGS) -c $(CGIFILE)/$(patsubst %.o,%.c, $@)

$(CGI):
	$(CC) $(CFLAGS) -o $@.$(CGIEXT) $@.c $(OBJS) $(LDFLAGS)
	mv ./utils.o $(CGIFILE)/
	mv ./cgilib.o $(CGIFILE)/
	mv ./bill.o $(CGIFILE)/

clean:
	rm -f *.o 
	rm -f $(CGIFILE)/*.o $(CGIFILE)/*.$(CGIEXT)
