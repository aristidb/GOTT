BASE := ../../..

include ../build/Var.mk

all: commonheaders.hpp.gch

commonheaders.hpp.gch: commonheaders.hpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(VIS) -x c++-header -c commonheaders.hpp
