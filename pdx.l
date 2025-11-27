%{
#include "paradox_type.h"
#include "y.tab.h"
#include <iostream>


%}

%option yylineno

%option case-sensitive

%%

#.*\n {}
#.* {}
\"(\\.|[^\"])*\" {
		int f = strlen(yytext);
		yylval.str = new std::string(yytext);
		yylval.str->erase(0,1);
		yylval.str->erase(f-2,1);
		return T_LITERAL;
}

0[0-9]+ {
	strncpy(yylval.name,yytext,64);	
	return T_IDENT;
}

(\-)?[0-9]+ {
	yylval.num = atoi(yytext) * 1000;
	return T_NUM_CONSTANT;
}
(\-)?([0-9]+\.|[0-9]*\.[0-9]+)([eE][+-]?[0-9]+)? {
	yylval.num = strtof(yytext, NULL) * 1000;
    return T_NUM_CONSTANT;
}
\$[0-9a-zA-Z@\%^\*_\+\\\./\-?`~]+\$ {
	int f = strlen(yytext);
		yylval.name[f-2] = '\0';
		strncpy(yylval.name,yytext + 1,f - 2);
		return T_PARAMETER;
}


(variable:)(From::)?[0-9a-zA-Z@\%^\*_\+\\\./\-?`~]+ {
	strncpy(yylval.name,yytext,64);	
	return T_IDENT;
}
(variable:)(From::)? {
	strncpy(yylval.name,yytext,64);	
	return T_SPECIAL;
}
[0-9a-zA-Z@\%^\*_\+\\\./\-?`~]+ {

	strncpy(yylval.name,yytext,64);	
	return T_IDENT;
}

(event_target|modifier|trigger_value|new_variable):[0-9a-zA-Z@\%^\*_\+\\\./\-?`~]+ {
	strncpy(yylval.name,yytext,64);	
	return T_IDENT;
}

(event_target|modifier|trigger_value|new_variable): {
	strncpy(yylval.name,yytext,64);	
	return T_SPECIAL;
}
([\r\n\t\040])+=([\r\n\t\040])* {
	return '=';
}
[\{\}\[\]=]([\r\n\t\040])* {
	return yytext[0];
}
[\{\}=><\[\]] {
	return yytext[0];
}

\d{1,4}.\d{1,2}.\d{1,2} {
	strncpy(yylval.name,yytext,64);	
	return T_DATE;
}

[\t\040]+   { return ' ';  }
[\r\n]+     { return ' '; }

%%


int yywrap()
{
	return 1;
}
