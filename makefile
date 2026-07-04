CXX = g++
CXXFLAGS = -O2 -std=c++20 -Werror
CFLAGS = -O2 -std=c++20 -Werror

SRCS := $(wildcard *.cpp) $(wildcard utils/*.cpp)
SRCS := $(filter-out test.cpp console.cpp b.cpp, $(SRCS))
SRCHEADERS = $(SRCS:.cpp=.h)
OBJS = $(SRCS:.cpp=.o)
TARGET = console.exe

all: $(TARGET)

$(TARGET): $(OBJS) parser.o lexer.o console.cpp
	$(CXX) $(OBJS) parser.o lexer.o console.cpp -o $@ $(CXXFLAGS) -lstdc++fs
	
$(OBJS): %.o: %.cpp %.h
	$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	rm -f *.o
	rm -f utils/*.o

a.exe: lexer.o test.cpp paradox_type.o modifier.o parser.o utils/string_util.o scope.o localization.o
	g++ lexer.o parser.o test.cpp modifier.o paradox_type.o utils/string_util.o scope.o localization.o $(CXXFLAGS) -static -lstdc++fs -Werror 

b.exe: lexer.o b.cpp paradox_type.o modifier.o parser.o trigger.o scope.o pattern.o utils/string_util.o
	g++ lexer.o parser.o b.cpp modifier.o paradox_type.o trigger.o scope.o pattern.o utils/string_util.o -o b.exe $(CXXFLAGS) -lstdc++fs -Werror 

lex2.yy.c: pdx.l
	flex++ pdx.l
	sed 's/register//g' lex.yy.c > lex2.yy.c

lexer.o: lex2.yy.c 
	g++ -c lex2.yy.c -o lexer.o $(CFLAGS) -lstdc++fs -Werror
	
parser.o: y.tab.c
	g++ -c y.tab.c -o parser.o $(CFLAGS) -lstdc++fs -Werror
	
y.tab.c: test.y
	bison -vdty test.y
	sed -i 's/define YYSIZE_T size_t/define YYSIZE_T long long/g' y.tab.c
	sed -i 's/# ifdef __SIZE_TYPE__//g' y.tab.c
	sed -i 's/#  define YYSIZE_T __SIZE_TYPE__//g' y.tab.c
	sed -i 's/# elif defined size_t/# ifdef size_t/g' y.tab.c