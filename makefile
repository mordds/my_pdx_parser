
a.exe: lexer.o test.cpp paradox_type.o modifier.o parser.o
	g++ lexer.o parser.o test.cpp modifier.o paradox_type.o -O2 -std=c++17 -lstdc++fs -Werror 

b.exe: lexer.o b.cpp paradox_type.o modifier.o parser.o trigger.o scope.o pattern.o
	g++ lexer.o parser.o b.cpp modifier.o paradox_type.o trigger.o scope.o pattern.o -o b.exe -O2 -std=c++17 -lstdc++fs -Werror 

lex2.yy.c: pdx.l
	flex++ pdx.l
	sed 's/register//g' lex.yy.c > lex2.yy.c

lexer.o: lex2.yy.c 
	g++ -c lex2.yy.c -o lexer.o -O2 -std=c++17 -lstdc++fs -Werror
	
parser.o:
	g++ -c y.tab.c -o parser.o -O2 -std=c++17 -lstdc++fs -Werror

y.tab.c: test.y
	bison -vdty test.y
	
paradox_type.o: paradox_type.h paradox_type.cpp
	g++ -c paradox_type.cpp -o paradox_type.o -O2 -std=c++17 -lstdc++fs -Werror
	
modifier.o: modifier.h modifier.cpp
	g++ -c modifier.cpp -o modifier.o -O2 -std=c++17 -lstdc++fs -Werror
	
scope.o: scope.h scope.cpp
	g++ -c scope.cpp -o scope.o -O2 -std=c++17 -lstdc++fs -Werror
	
trigger.o: trigger.h trigger.cpp
	g++ -c trigger.cpp -o trigger.o -O2 -std=c++17 -lstdc++fs -Werror

pattern.o: pattern.h pattern.cpp
	g++ -c pattern.cpp -o pattern.o -O2 -std=c++17 -lstdc++fs -Werror