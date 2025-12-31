CXX = g++
CXXFLAGS = -O2 -std=c++17 -Werror

SRCS = utils/string_util.cpp utils/parser_util.cpp pattern.cpp trigger.cpp scope.cpp modifier.cpp paradox_type.cpp localization.cpp db_object.cpp national_idea.cpp
SRCHEADERS = $(SRCS:.cpp=.h)
OBJS = $(SRCS:.cpp=.o)
TARGET = console.exe

all: $(TARGET)

$(TARGET): $(OBJS) parser.o lexer.o console.cpp
	$(CXX) $(OBJS) parser.o lexer.o console.cpp -o $@ $(CXXFLAGS) -lstdc++fs
	
$(OBJS): %.o: %.cpp %.h
	$(CXX) -c $(CXXFLAGS) $< -o $@

a.exe: lexer.o test.cpp paradox_type.o modifier.o parser.o utils/string_util.o 
	g++ lexer.o parser.o test.cpp modifier.o paradox_type.o utils/string_util.o -O2 -std=c++17 -lstdc++fs -Werror 

b.exe: lexer.o b.cpp paradox_type.o modifier.o parser.o trigger.o scope.o pattern.o utils/string_util.o
	g++ lexer.o parser.o b.cpp modifier.o paradox_type.o trigger.o scope.o pattern.o utils/string_util.o -o b.exe -O2 -std=c++17 -lstdc++fs -Werror 

lex2.yy.c: pdx.l
	flex++ pdx.l
	sed 's/register//g' lex.yy.c > lex2.yy.c

lexer.o: lex2.yy.c 
	g++ -c lex2.yy.c -o lexer.o -O2 -std=c++17 -lstdc++fs -Werror
	
parser.o: y.tab.c
	g++ -c y.tab.c -o parser.o -O2 -std=c++17 -lstdc++fs -Werror
	
y.tab.c: test.y
	bison -vdty test.y
	sed -i 's/define YYSIZE_T size_t/define YYSIZE_T long long/g' y.tab.c
	sed -i 's/# ifdef __SIZE_TYPE__//g' y.tab.c
	sed -i 's/#  define YYSIZE_T __SIZE_TYPE__//g' y.tab.c
	sed -i 's/# elif defined size_t/# ifdef size_t/g' y.tab.c