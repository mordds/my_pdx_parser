%{


#include<stdio.h>
#include "paradox_type.h"
#include<cstdlib>
#include<string.h>
#include<map>
#include<string>
#include<vector>
#include<queue>

extern int yylex();
void yyerror(const char* s);

std::vector<ParadoxBase*> parsedObject;
std::vector<ParadoxBase*> tempObject;
ParadoxTag* ROOT = nullptr;
ParadoxString* createString(std::string str){
	ParadoxString* string = new ParadoxString(str);
	parsedObject.push_back(string);
	return string;
}

ParadoxString* createTempString(std::string str){
	ParadoxString* string = new ParadoxString(str);
	tempObject.push_back(string);
	return string;
}

ParadoxArray* createArray(){
	ParadoxArray* array = new ParadoxArray();
	parsedObject.push_back(array);
	return array;
}


ParadoxInteger* createInteger(long long val){
	ParadoxInteger* integer = new ParadoxInteger(val);
	parsedObject.push_back(integer);
	return integer;
}

ParadoxInteger* createTempInteger(long long val){
	ParadoxInteger* integer = new ParadoxInteger(val);
	tempObject.push_back(integer);
	return integer;
}
ParadoxDate* createDate(std::string str){
	ParadoxDate* date = new ParadoxDate(str);
	parsedObject.push_back(date);
	return date;
}
ParadoxDate* createDate(Date date1){
	ParadoxDate* date = new ParadoxDate(date1);
	parsedObject.push_back(date);
	return date;
}
ParadoxDate* createTempDate(std::string str){
	ParadoxDate* string = new ParadoxDate(str);
	tempObject.push_back(string);
	return string;
}
ParadoxTag* createTag(){
	ParadoxTag* tag = new ParadoxTag();
	parsedObject.push_back(tag);
	return tag;
}
%}


%union{
	char name[64];
	int num;
	ParadoxBase* base;
	std::string* str;
}

%token<name> T_IDENT
%token<str> T_LITERAL
%token<name> T_DATE
%token<num> T_NUM_CONSTANT
%token<name> T_PARAMETER

%type<base> GT
%type<base> T
%type<base> Ar
%type<base> A
%type<base> B
%type<base> C
%type<base> lVal
%type<base> rVal
%type<num> BLANK
%type<num> LBR
%error-verbose

%nonassoc LOWEST
%left ' '
%left T_IDENT
%left T_PARAMETER



%%

input: GT {ROOT = $1->getAsTag();}
	| A {ROOT = $1->getAsTag();}
	| BLANK A {ROOT = $2->getAsTag();}
	| BLANK GT {ROOT = $2->getAsTag();}
	| /*empty*/

GT: T {$$ = $1;}
  | Ar {$$ = $1;}
  | LBR '}' {$$ = createTag();}
T: LBR A '}' {$$ = $2;}
Ar: LBR B '}' {$$ = $2;}
  | LBR C '}' {$$ = $2;}
A: lVal '=' rVal { 
	ParadoxTag* tag = createTag();
	ParadoxType type = $1->getType();
	if(type == ParadoxType::STRING){
		tag->add($1->getAsString()->getStringContent(),$3);
	}
	else if(type == ParadoxType::DATE){
		tag->add($1->getAsDate()->getDateContent().toString(),$3);
	}
	else if(type == ParadoxType::INTEGER){
		tag->add(std::to_string($1->getAsInteger()->getIntegerContent() / 1000),$3);
	}
	$$ = tag;
}
  | A lVal '=' rVal {
	ParadoxType type = $2->getType();
	ParadoxTag* tag = $1->getAsTag();
	if(type == ParadoxType::STRING){
		tag->add($2->getAsString()->getStringContent(),$4);
	}
	else if(type == ParadoxType::DATE){
		tag->add($2->getAsDate()->getDateContent().toString(),$4);
	}
	else if(type == ParadoxType::INTEGER){
		tag->add(std::to_string($2->getAsInteger()->getIntegerContent() / 1000),$4);
	}
	$$ = tag;
  }
  | A BLANK

B: B T_IDENT{
	$1->getAsArray()->append(createString($2));
  } 
  | T_IDENT {  
	ParadoxArray* array = createArray();
	array->append(createString($1));
	$$ = array;
  }
  | B BLANK
C: C T_NUM_CONSTANT {
	$1->getAsArray()->append(createInteger($2));
	$$ = $1;
  }
  | T_NUM_CONSTANT {  
	ParadoxArray* array = createArray();
	array->append(createInteger($1));
	$$ = array;
  }
  | C BLANK
lVal: T_IDENT {$$ = createTempString($1);}
  | T_DATE {$$ = createTempDate($1);}
  | T_NUM_CONSTANT {$$ = createTempInteger($1);}
  
rVal: GT {$$ = $1;}
  | T_LITERAL {
	$$ = createString(*$1);
  } 
  | lVal { 
	ParadoxType type = $1->getType();
	if(type == ParadoxType::STRING){
		$$ = createString($1->getAsString()->getStringContent());
		tempObject.pop_back();
		delete $1;
	}
	else if(type == ParadoxType::INTEGER){
		$$ = createInteger($1->getAsInteger()->getIntegerContent());
		tempObject.pop_back();
		delete $1;
	}
	else if(type == ParadoxType::DATE){
		$$ = createDate($1->getAsDate()->getDateContent());
		tempObject.pop_back();
		delete $1;
	}
  }
BLANK: ' ' { $$ = 1;}
	 | BLANK ' '{ $$ = 1;}
LBR: '{' {$$ = 1;}
   | '{' BLANK {$$ = 1;}	
%%


void yyerror(const char* s) {
    fprintf(stderr, "Error: %s\n", s);
}