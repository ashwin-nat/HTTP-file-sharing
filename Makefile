# MIT License
# Copyright (c) [2020] [Ashwin Natarajan]
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

###############################################################
# General purpose variables. Set COMPILER = $(CXX) for C++,   #
#                                COMPILER = $(CC) for C       #
###############################################################
RM_RF		= rm -rf
MKDIR_P		= mkdir -p
SRCEXT		= cpp
HDREXT		= hpp
OBJEXT		= o
COMPILER	= $(CXX)
BINNAME		= http-server

###############################################################
# Variables for compile and link time flags                   #
# (Please don't pass -c CFLAG here)                           #
###############################################################
CFLAGS		= -Wall -std=c++14
CFLAGS_SAN	= -fsanitize=address -fsanitize=undefined
CFLAGS_DBG	= -O0 -ggdb3
CFLAGS_REL	= -O2 -Werror

LDFLAGS		=
LDFLAGS_DBG	=
LDFLAGS_SAN	= -lasan -lubsan
LDFLAGS_REL	=

###############################################################
# Variables for directories (Please don't suffix them with /) #
###############################################################
SRCDIR		= src
HDRDIR		= inc
OBJDIR		= obj
EXECDIR		= bin

###############################################################
# The final variable for the binary name                      #
###############################################################
BINARY		= $(EXECDIR)/$(BINNAME)

###############################################################
# List variables                                              #
###############################################################
OBJTREE_TMP	= $(addprefix $(OBJDIR)/,\
			$(subst $(SRCDIR)/,,$(SRCTREE)))

HDRLIST		= $(addprefix -I ,\
			$(shell find $(HDRDIR) -type d))
# HDRLIST		= -I $(HDRDIR)/
SRCLIST		= $(shell find $(SRCDIR) -name "*.$(SRCEXT)")
OBJLIST		= $(subst .$(SRCEXT),.$(OBJEXT),\
			$(subst $(SRCDIR)/,$(OBJDIR)/,\
			$(SRCLIST)))
SRCTREE		= $(shell find $(SRCDIR) -type d)
OBJTREE		= $(subst $(SRCDIR),,$(OBJTREE_TMP))

###############################################################
# Please don't modify anything below this line                #
###############################################################
.PHONY:		all release clean dir_check debug_print \
				cflags_rel debugsan debug
all: release

###############################################################
# In case you want to see the file lists generated            #
###############################################################
debug_print:
	@echo "HDRLIST = $(HDRLIST)"
	@echo "SRCLIST = $(SRCLIST)"
	@echo "OBJLIST = $(OBJLIST)"
	@echo "SRCTREE = $(SRCTREE)"
	@echo "OBJTREE = $(OBJTREE)"

###############################################################
# Create directory structure if required                      #
###############################################################
dir_check:
	@$(MKDIR_P) $(OBJDIR)
	@$(MKDIR_P) $(EXECDIR)
	@$(MKDIR_P) $(OBJTREE)

###############################################################
# Release build - add extra CFLAGS and LDFLAGS                #
###############################################################
release: CFLAGS+=$(CFLAGS_REL)
release: LDFLAGS+=$(LDFLAGS_REL)
release: dir_check $(BINARY)

###############################################################
# Debug build (without runtime sanitizers)                    #
###############################################################
debug: CFLAGS+=$(CFLAGS_DBG)
debug: LDFLAGS+=$(LDFLAGS_DBG)
debug: dir_check $(BINARY)

###############################################################
# Debug build (with Address and UB runtime sanitizers)        #
###############################################################
debugsan: CFLAGS+=$(CFLAGS_DBG)
debugsan: CFLAGS+=$(CFLAGS_SAN)
debugsan: LDFLAGS+=$(LDFLAGS_DBG)
debugsan: LDFLAGS+=$(LDFLAGS_SAN)
debugsan: dir_check $(BINARY)

###############################################################
# Compile all source files with CFLAGS                        #
###############################################################
$(OBJDIR)/%.$(OBJEXT):$(SRCDIR)/%.$(SRCEXT)
	$(COMPILER) -c $(CFLAGS) $(HDRLIST) -o $@ $<

###############################################################
# Link all object files with LDFLAGS                          #
###############################################################
$(BINARY): $(OBJLIST)
	$(COMPILER) $(OBJLIST) -o $(BINARY) $(LDFLAGS)
	@echo "The executable is at $(BINARY)"

###############################################################
# Remove all created directories and subdirectories           #
###############################################################
clean:
	$(RM_RF) $(OBJDIR)
	$(RM_RF) $(EXECDIR)

###############################################################
# Help output                                                 #
###############################################################
help:
	@echo "make               ----> Builds in release mode"
	@echo "make release       ----> Builds in release mode"
	@echo "make debug         ----> Builds in debug mode (without sanitizers)"
	@echo "make debugsan      ----> Builds in debug mode (with address and UB sanitizers)"
	@echo "make clean         ----> Deletes the $(HDRDIR)/ and $(OBJDIR)/ directories"
	@echo "make debug_print   ----> Prints the file lists that are generated by this Makefile"