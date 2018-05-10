# Compiler
CC=g++

# Objects directory
ODIR=bin
# Inludes directory
IDIR=include
# Source directory
SDIR=source
# Dependecies directory
DDIR=.depends
# Tests directory
TDIR=test
# library name
MAINNAME=glc

# Compilation flags
CFLAGS=-g -std=c++11 -I$(IDIR)/ 

# Create dependencies directory
$(shell mkdir -p $(DDIR) > /dev/null)
# Create binary directory
$(shell mkdir -p $(ODIR) > /dev/null)
# Dependencies generation flags
DFLAGS=-MT $(foreach TMP,$(SDIR) $(TDIR),$(patsubst $(TMP)/%.cpp, $(ODIR)/%.o, $(filter $(TMP)/%.cpp,$<))) -MMD -MP -MF $(DDIR)/$*.d

# Libraries
LIBS=

# Object list (derived from source directory)
_OBJS=$(patsubst $(SDIR)/%.cpp, %.o, $(wildcard $(SDIR)/*.cpp))
OBJS=$(patsubst %,$(ODIR)/%,$(_OBJS))

# build objects and lib
all: build-main
Release: all
Debug: all

# build a test
$(TDIR)/%: $(TDIR)/%.cpp $(OBJS) 
	$(CC) $(DFLAGS) $(CFLAGS) $< -o $(patsubst $(TDIR)/%.cpp,$(ODIR)/%,$<) $(LIBS)

# build objects
$(ODIR)/%.o: $(SDIR)/%.cpp $(DDIR)/%.d
	$(CC) -c $(DFLAGS) $(CFLAGS) -o $@ $<

build-main: $(OBJS)
	$(CC) $(DFLAGS) $(CFLAGS) $(OBJS) -o $(ODIR)/$(MAINNAME) $(LIBS)

# empty rule, so make do not fail if dependency file do not exist (e.g firt make after make clean)
$(DDIR)/%.d: ;
.PRECIOUS: $(DDIR)/%.d

# include dependencies
-include $(DDIR)/*.d

# clean directory
clean:
	rm -f $(ODIR)/* ${DDIR}/* ./**/{*~,.*~,.*.sw?} ./{*~,.*~,.*.sw?}

.PHONY: clean build-lib all
