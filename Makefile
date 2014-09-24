LIB = libCamiProdRead.a

SRCS = Arcs.cpp Node.cpp StoreNet.cpp IO.cpp Utilities.cpp  Expression.cpp Waiting.cpp
INCD = /opt/local/include

OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)
INCDIR = $(addprefix -I, $(INCD))

#CXX = g++
cXX = clang++
#CXXFLAGS= -O9  -Wall $(INCDIR) #-pg
#CXXFLAGS= -g3 -O0  -Wall $(INCDIR) -D_GLIBCXX_DEBUG
CXXFLAGS= -g3 -O0  -Wall $(INCDIR) --std=c++11

all: $(LIB)

$(LIB): $(OBJS)
	ar ru $(LIB) $(OBJS)
	ranlib $(LIB)
	doxygen Doxyfile

IO.o: IO.cpp IO.h StoreNet.h Node.h
Arcs.o: Arcs.cpp Arcs.h
Node.o: Node.cpp Node.h Arcs.h
StoreNet.o: StoreNet.cpp StoreNet.h Node.h Utilities.h
Parser.o: Parser.cpp Parser.h StoreNet.h Node.h Expression.h
Utilities.o: Utilities.h
Expression.o: Expression.cpp Expression.h StoreNet.h
Waiting.o: Waiting.cpp Waiting.h StoreNet.h

clean:
	rm -f $(LIB) *.o *.d *~ 

# implicit rules

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $<

%.d: %.cpp
	@$(SHELL) -ec '$(CXX) -M $(CXXFLAGS) $(INCLUDES) $< \
                | sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
                [ -s $@ ] || rm -f $@'

-include $(DEPS)        












