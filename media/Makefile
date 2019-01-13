# Copyright (c) 2013-2014 ChengZhen(Anyou). All Rights Reserved.
# Author: ChengZhen (anyou@msn.com)

include ../Makefile.rules

# PROJECT_CFLAGS += -H


APP_NAME = media
LIB_NAME = lib$(APP_NAME).a

SRCXX	:= $(wildcard *.cpp)
SRCXX	+= $(wildcard core/*.cpp)

CXXOBJ  := $(SRCXX:.cpp=.o)
OBJS	:= $(COBJ) $(CXXOBJ)

all: $(APP_NAME)

$(APP_NAME): $(OBJS)
	$(TOOL_AR) -rc $(LIB_NAME) $(OBJS)
	$(CP) $(LIB_NAME) $(PROJECT_OUTPUT)/

clean:
	rm -f *.o core/*.o 
	rm -f $(LIB_NAME) $(PROJECT_OUTPUT)/$(LIB_NAME)

