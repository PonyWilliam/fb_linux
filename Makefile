#
# Makefile
#
CC = arm-linux-gcc
LVGL_DIR_NAME ?= lvgl
LVGL_DIR ?= ${shell pwd}
CFLAGS ?= -O3 -g0 -I$(LVGL_DIR)/ -I/usr/local/arm/include -I./include/freetype -std=c99
BIN = myc


#Collect the files to compile
MAINSRC = ./main.c

include $(LVGL_DIR)/lvgl/lvgl.mk
include $(LVGL_DIR)/lv_drivers/lv_drivers.mk
include $(LVGL_DIR)/lv_examples/lv_examples.mk
include $(LVGL_DIR)/lv_freetype/lv_freetype.mk
OBJEXT ?= .o

AOBJS = $(ASRCS:.S=$(OBJEXT))
COBJS = $(CSRCS:.c=$(OBJEXT))

MAINOBJ = $(MAINSRC:.c=$(OBJEXT))

SRCS = $(ASRCS) $(CSRCS) $(MAINSRC)
OBJS = $(AOBJS) $(COBJS)

## MAINOBJ -> OBJFILES

%.o: %.c
	@$(CC)  $(CFLAGS) -c $< -o $@
	@echo "CC $<"
    
# default: $(AOBJS) $(COBJS) $(MAINOBJ)
# 	$(CC) -shared -fPIC -o lib$(BIN).so $(MAINOBJ) $(AOBJS) $(COBJS) $(LDFLAGS) -lpthread -lfreetype
bin: $(AOBJS) $(COBJS) $(MAINOBJ)
	$(CC) -o $(BIN) $(MAINOBJ) update_client.c led.c $(AOBJS) $(COBJS) $(LDFLAGS) -lpthread -lfreetype
lib: $(AOBJS) $(COBJS) $(MAINOBJ)
	$(CC) -shared -fPIC -o lib$(BIN).so update_client.c led.c $(MAINOBJ) $(AOBJS) $(COBJS) $(LDFLAGS) -lpthread -lfreetype
clean: 
	rm -f $(AOBJS) $(COBJS) $(MAINOBJ)
delbin:
	rm -f $(BIN)