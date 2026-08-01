
#include "parser_util.h"
#include <stdio.h>
#include <iostream>
struct yy_buffer_state;
typedef yy_buffer_state* YY_BUFFER_STATE;

extern YYSTYPE yylval;
extern YY_BUFFER_STATE yy_scan_string ( const char *yy_str );
extern void yy_delete_buffer (YY_BUFFER_STATE  b );
extern FILE* yyin;
extern FILE* yyout;
extern int yyparse();
extern int yylex();
extern ParadoxTag* ROOT;
extern std::vector<ParadoxBase*> parsedObject;
extern void yyrestart ( FILE *input_file );
extern 
/*
#define T_IDENT 258
#define T_LITERAL 259
#define T_DATE 260
#define T_NUM_CONSTANT 261
*/
ParadoxTag* parseFile(std::string str){
    ROOT = nullptr;
    yyrestart(fopen(str.c_str(),"r"));
    yyparse();
    return ROOT;
}
//
ParadoxTag* parseString(std::string str){
    ROOT = nullptr;

    YY_BUFFER_STATE state = yy_scan_string(str.c_str());
    yyparse();
    yy_delete_buffer(state);
    return ROOT;
}
void clearParserDatas(){
    for(ParadoxBase* ptr : parsedObject){
        delete ptr;
    }
    parsedObject.clear();
    parsedObject.shrink_to_fit();
}
