
CXX := g++
CC := gcc
LINK := g++

CFLAGS += -Wall
CFLAGS += -Werror	

ifeq ($(debug),1) 
 COMMONFLAGS += -g
endif

ifeq ($(release),1) 
 COMMONFLAGS += -O2
 COMMONFLAGS += -DNDEBUG
endif

ifeq ($(profile),1)
 CXXFLAGS += -pg
 CFLAGS += -pg
endif


BUILDDIR = build
BINDIR = bin
SRCDIR = src

# Common flags
#COMMONFLAGS += $(INCLUDES)
CXXFLAGS += $(COMMONFLAGS)
CFLAGS += -std=gnu99
CFLAGS += $(COMMONFLAGS)

EXEC=frostcoder
OBJS=frostcoder.c state.c


BUILD_OBJS =$(addprefix $(BUILDDIR)/, $(addsuffix .o, $(OBJS)))
LINKLINE = $(LINK) $(BUILD_OBJS) 

all: main

init:
	mkdir -p $(BUILDDIR)
	mkdir -p $(BINDIR)

.SUFFIXES: .c .cpp .o

$(BUILDDIR)/%.c.o: $(SRCDIR)/%.c $(SRCDIR)/%.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%.cpp.o: $(SRCDIR)/%.cpp $(SRCDIR)/%.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

main:  $(BUILD_OBJS) 
	$(LINKLINE) -o$(BINDIR)/$(EXEC)

clean:
	rm -f $(BUILDDIR)/* $(BINDIR)/$(EXEC) $(SRCDIR)/*~
