#include "parser_util.h"
#include <stdio.h>
#include <iostream>
extern YYSTYPE yylval;
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
    int error = yylex();
    while(error != 0){
        if(error == '}') std::cout << '}' <<std::endl;
        else if(error <= 0xFF) std::cout << (char)error;
        else if(error == 258) std::cout << yylval.name;
        else if(error == 259) std::cout << "T_LITERAL" ;
        else if(error == 261) std::cout << "T_NUM_CONSTANT" ;
        else if(error == 260) std::cout << "T_DATE" ;
        error = yylex();
    }
    return ROOT;
}

void clearParserDatas(){
    for(ParadoxBase* ptr : parsedObject){
        delete ptr;
    }
    parsedObject.clear();
    parsedObject.shrink_to_fit();
}

