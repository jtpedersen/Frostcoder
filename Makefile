
CXX := g++
CC := gcc
LINK := g++

CFLAGS += -Wall
CFLAGS += -Werror	
CFLAGS += -pedantic
CFLAGS += -Wextra
CFLAGS += -Wmissing-prototypes
CFLAGS += -Wstrict-prototypes
CFLAGS += -Wswitch-enum  #require that all enums are used in switc
CFLAGS += -lm
CFLAGS += -std=gnu99
CFLAGS += -DGNUSOURCE  		#asprintf


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
CFLAGS += $(COMMONFLAGS)

OBJS=state.c parser.c
EXECS=frostcoder.c eggcoder.c

BUILD_OBJS=$(addprefix $(BUILDDIR)/, $(addsuffix .o, $(OBJS)))
LINKLINE = $(LINK) $(BUILD_OBJS) 

all: frost egg

init:
	mkdir -p $(BUILDDIR)
	mkdir -p $(BINDIR)

.SUFFIXES: .c .cpp .o

$(BUILDDIR)/%.c.o: $(SRCDIR)/%.c $(SRCDIR)/%.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%.cpp.o: $(SRCDIR)/%.cpp $(SRCDIR)/%.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

frost:  $(BUILD_OBJS) $(BUILDDIR)/frostcoder.c.o
	$(LINKLINE) $(BUILDDIR)/frostcoder.c.o -o$(BINDIR)/frostcoder

egg:  $(BUILD_OBJS) $(BUILDDIR)/eggcoder.c.o
	$(LINKLINE) $(BUILDDIR)/eggcoder.c.o -o$(BINDIR)/eggcoder

clean:
	rm -f $(BUILDDIR)/* $(BINDIR)/eggcoder $(BINDIR)/frostcoder $(SRCDIR)/*~
