# Simple SAX-like parser for TDL data files
#
# Copyright (C) 2004 Aristid Breitkreuz
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

TEST     :=./testcase
LIBTARG  :=
CXXFLAGS :=-g -W -Wall
CXX      :=g++
INCLUDES :=-I../include
LIBS     :=
EXT      :=cpp

SOURCES  :=$(wildcard *.$(EXT))
OBJECTS  :=$(patsubst %.$(EXT),%.o,$(SOURCES))
DEPS     :=$(patsubst %.$(EXT),%.dep,$(SOURCES))
LIBOBJ   :=$(patsubst %.$(EXT),%.o,$(wildcard *[a-z].$(EXT)))
 
all: $(TEST)
 
$(TEST): $(OBJECTS) $(DEPS)
	$(CXX) $(CXXFLAGS) -o $(TEST) $(OBJECTS)

-include $(DEPS)
 
%.o: %.$(EXT)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@
 
%.dep: %.$(EXT)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MM $< -MT $(<:.$(EXT)=.o) > $@
 
.PHONY: clean
clean:
	rm -f $(LIBTARG) $(TEST) $(OBJECTS) $(DEPS) 

