BUILD = build
STATICLIB = $(BUILD)/libmatrix.a
# SHAREDLIB = $(BUILD)/libmatrix.so

BINSRCS = \

TSTSRCS = \
	  MatrixTest.cpp \
	  EquationTest.cpp \

EMPSRCS = \
	  EquationExample.cpp \

INCS = $(shell ls *.h)
SRCS = $(shell ls *.cpp)
DEPS = $(SRCS:%.cpp=$(BUILD)/%.d)
OBJS = $(SRCS:%.cpp=$(BUILD)/%.o)
LIBS = $(STATICLIB) $(SHAREDLIB)
BINS = $(BINSRCS:%.cpp=$(BUILD)/%)
TSTS = $(TSTSRCS:%.cpp=$(BUILD)/%)
EMPS = $(EMPSRCS:%.cpp=$(BUILD)/%)
BINOBJS = $(BINSRCS:%.cpp=$(BUILD)/%.o)
TSTOBJS = $(TSTSRCS:%.cpp=$(BUILD)/%.o)
EMPOBJS = $(EMPSRCS:%.cpp=$(BUILD)/%.o)
NOLIBOBJS = $(BINOBJS) $(TSTOBJS) $(EMPOBJS)
LIBOBJS=$(filter-out $(NOLIBOBJS),$(OBJS))
TSTLABS = $(TSTSRCS:%Test.cpp=%.TEST)

PREFIX=install
INCPREFIX=$(PREFIX)/include
LIBPREFIX=$(PREFIX)/lib
BINPREFIX=$(PREFIX)/bin
TSTPREFIX=$(PREFIX)/share/Matrix/test
EMPPREFIX=$(PREFIX)/share/Matrix/examples

CXX = g++
CXXFLAGS = -g -O3 -Wall -Wshadow -Wextra
LDFLAGS = -L$(BUILD) -lmatrix \
	  -Wl,--rpath=$(abspath $(LIBPREFIX)) \
	  -Wl,--rpath=$(BUILD) -Wl,--rpath=.
AR = ar
ARFLAGS = -crs

all: $(LIBS) $(BINOBJS) $(BINS)

clean:
	rm -rf $(BUILD)

test: $(TSTOBJS) $(TSTS) $(TSTLABS)
	@echo "Success: All tests passed."

examples: $(EMPOBJS) $(EMPS)

install: \
	$(INCS:%=%.INCINSTALL) \
	$(LIBS:%=%.LIBINSTALL) \
	$(BINS:%=%.BININSTALL) \
	$(TSTS:%=%.TSTINSTALL) \
	$(EMPS:%=%.EMPINSTALL)

.PHONY: all clean test examples install

$(BUILD)/%.d: %.cpp
	@mkdir -p $(BUILD)
	@echo "Building dependency: $@"
	@$(CXX) $(CXXFLAGS) -M -MF $@ -MT $@ $<

$(BUILD)/%.o: %.cpp $(BUILD)/%.d
	@mkdir -p $(BUILD)
	@echo "Compiling: $<"
	@$(CXX) $(CXXFLAGS) -c -fPIC $< -o $@

$(BUILD)/%: $(BUILD)/%.o $(LIBS)
	@echo "Building executable: $@"
	@$(CXX) $< -o $@ $(LDFLAGS)

$(BUILD)/libmatrix.a: $(LIBOBJS)
	@echo "Building static library: $@"
	@$(AR) $(ARFLAGS) $@ $(LIBOBJS)

$(BUILD)/libmatrix.so: $(LIBOBJS)
	@echo "Building shared library: $@"
	@$(CXX) -shared $(LIBOBJS) -o $@

%.TEST: $(BUILD)/%Test
	@echo "Testing: $<"
	@objdump -CS $< > $<.dmp
	@valgrind --quiet --error-exitcode=1 \
		--leak-check=full --show-error-list=yes $<

%.INCINSTALL: %
	@mkdir -p $(INCPREFIX)
	@echo "Copying header: $(INCPREFIX)/$$(basename $<)"
	@cp -p $< $(INCPREFIX)

%.LIBINSTALL: %
	@mkdir -p $(LIBPREFIX)
	@echo "Copying library: $(LIBPREFIX)/$$(basename $<)"
	@cp -p $< $(LIBPREFIX)

%.BININSTALL: %
	@mkdir -p $(BINPREFIX)
	@echo "Copying executable: $(BINPREFIX)/$$(basename $<)"
	@cp -p $< $(BINPREFIX)

%.TSTINSTALL: %
	@mkdir -p $(TSTPREFIX)
	@echo "Copying test: $(TSTPREFIX)/$$(basename $<)"
	@cp -p $< $(TSTPREFIX)

%.EMPINSTALL: %
	@mkdir -p $(EMPPREFIX)
	@echo "Copying example: $(EMPPREFIX)/$$(basename $<)"
	@cp -p $< $(EMPPREFIX)

-include $(DEPS)
