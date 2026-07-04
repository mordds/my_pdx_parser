
#include "parser_util.h"
#include <stdio.h>
#include <iostream>
extern YYSTYPE yylval;
extern YY_BUFFER_STATE yy_scan_string ( const char *yy_str );
extern FILE* yyin;
extern FILE* yyout;
extern int yyparse();
extern int yylex();
extern ParadoxTag* ROOT;
extern std::vector<ParadoxBase*> parsedObject;

/*
#define T_IDENT 258
#define T_LITERAL 259
#define T_DATE 260
#define T_NUM_CONSTANT 261
*/
ParadoxTag* parseFile(std::string str){
    yyin = fopen(str.c_str(),"r");
    yyparse();
    return ROOT;
}

ParadoxTag* parseString(std::string str){
    yy_scan_string(str.c_str());
    yyparse();
    return ROOT;
}
void clearParserDatas(){
    for(ParadoxBase* ptr : parsedObject){
        delete ptr;
    }
    parsedObject.clear();
    parsedObject.shrink_to_fit();
}
