#include "parser_util.h"
#include <stdio.h>
extern YYSTYPE yylval;
extern FILE* yyin;
extern FILE* yyout;
extern int yyparse();
extern int yylex();
extern ParadoxTag* ROOT;
extern std::vector<ParadoxBase*> parsedObject;

ParadoxTag* parseFile(std::string str){
    yyin = fopen(str.c_str(),"r");
    yyparse();
    return ROOT;
}

void clearParserDatas(){
    for(ParadoxBase* ptr : parsedObject){
        delete ptr;
    }
    parsedObject.clear();
}

