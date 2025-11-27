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
ParadoxParameter* createParameter(){
	ParadoxParameter* parameter = new ParadoxParameter();
	parsedObject.push_back(parameter);
	return parameter;
}
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
ParadoxOptionalTag* createOptionalTag(){
	ParadoxOptionalTag* tag = new ParadoxOptionalTag();
	parsedObject.push_back(tag);
	return tag;
}
ParadoxComplicateTag* createComplicateTag(){
	ParadoxComplicateTag* tag = new ParadoxComplicateTag();
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
%token<name> T_SPECIAL

%type<base> GT
%type<base> T
%type<base> Ar
%type<base> A
%type<base> B
%type<base> C
%type<base> lVal
%type<base> rVal
%type<base> PC
%type<num> BLANK
%type<num> LBR
%type<base> OPT
%type<num> BLANK_OPT
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
	else{
		ParadoxComplicateTag* tag2 = createComplicateTag();
		tag2->op = '=';
		tag2->lVal = $1;
		tag2->rVal = $3;
		tag->add("#PARAMETER_TEMPLATE",tag2);
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
	else{
		ParadoxComplicateTag* tag2 = createComplicateTag();
		tag2->op = '=';
		tag2->lVal = $2;
		tag2->rVal = $4;
		tag->add("#PARAMETER_TEMPLATE",tag2);
	} 
	$$ = tag;
  }
  | A BLANK
  | OPT {
	std::string name = "#OPTIONAL_TAG";
	ParadoxTag* tag = createTag();
	tag->add(name,$1);
	$$=tag;
  }
  | A OPT {
	std::string name = "#OPTIONAL_TAG";
	ParadoxTag* tag = $1->getAsTag();
	tag->add(name,$2);
	$$=tag;
  }
  
  | BLANK_OPT {$$ = createTag();}
  | A BLANK_OPT
  | T_PARAMETER BLANK %prec LOWEST {
	std::string name = "#PARAMETER_BLOCK";
	ParadoxTag* tag = createTag();
	ParadoxParameter* par = createParameter();
	par->appendParameter($1);
	tag->add(name,par);
	$$=tag;
  }
  | A T_PARAMETER BLANK %prec LOWEST{
  	std::string name = "#PARAMETER_BLOCK";
	ParadoxTag* tag = $1->getAsTag();
	ParadoxParameter* par = createParameter();
	par->appendParameter($2);
	tag->add(name,par);
	$$=tag;
  }
  | lVal '>' rVal{
	ParadoxTag* tag = createTag();
	ParadoxType type = $1->getType();
	ParadoxComplicateTag* tag2 = createComplicateTag();
	tag2->op = '>';
	if(type == ParadoxType::STRING){
		tag2->lVal = nullptr;
		tag2->rVal = $3;
		tag->add($1->getAsString()->getStringContent(),tag2);
	}
	else if(type == ParadoxType::DATE){
		tag2->lVal = nullptr;
		tag2->rVal = $3;
		tag->add($1->getAsDate()->getDateContent().toString(),tag2);
	}
	else if(type == ParadoxType::INTEGER){
		tag2->lVal = nullptr;
		tag2->rVal = $3;
		tag->add(std::to_string($1->getAsInteger()->getIntegerContent() / 1000),tag2);
	}
	else{

		tag2->lVal = $1;
		tag2->rVal = $3;
		tag->add("#PARAMETER_TEMPLATE",tag2);
	} 
  }
  | A lVal '>' rVal {
	ParadoxType type = $2->getType();
	ParadoxTag* tag = $1->getAsTag();
	ParadoxComplicateTag* tag2 = createComplicateTag();
	tag2->op = '>';
	if(type == ParadoxType::STRING){
		tag2->lVal = nullptr;
		tag2->rVal = $4;
		tag->add($2->getAsString()->getStringContent(),tag2);
	}
	else if(type == ParadoxType::DATE){
		tag2->lVal = nullptr;
		tag2->rVal = $4;
		tag->add($2->getAsDate()->getDateContent().toString(),tag2);
	}
	else if(type == ParadoxType::INTEGER){
		tag2->lVal = nullptr;
		tag2->rVal = $4;
		tag->add(std::to_string($2->getAsInteger()->getIntegerContent() / 1000),tag2);
	}
	else{

		tag2->lVal = $2;
		tag2->rVal = $4;
		tag->add("#PARAMETER_TEMPLATE",tag2);
	} 
  }
  | lVal '<' rVal{
	ParadoxTag* tag = createTag();
	ParadoxType type = $1->getType();
	ParadoxComplicateTag* tag2 = createComplicateTag();
	tag2->op = '<';
	if(type == ParadoxType::STRING){
		tag2->lVal = nullptr;
		tag2->rVal = $3;
		tag->add($1->getAsString()->getStringContent(),tag2);
	}
	else if(type == ParadoxType::DATE){
		tag2->lVal = nullptr;
		tag2->rVal = $3;
		tag->add($1->getAsDate()->getDateContent().toString(),tag2);
	}
	else if(type == ParadoxType::INTEGER){
		tag2->lVal = nullptr;
		tag2->rVal = $3;
		tag->add(std::to_string($1->getAsInteger()->getIntegerContent() / 1000),tag2);
	}
	else{

		tag2->lVal = $1;
		tag2->rVal = $3;
		tag->add("#PARAMETER_TEMPLATE",tag2);
	} 
  }
  | A lVal '<' rVal{
	ParadoxType type = $2->getType();
	ParadoxTag* tag = $1->getAsTag();
	ParadoxComplicateTag* tag2 = createComplicateTag();
	tag2->op = '<';
	if(type == ParadoxType::STRING){
		tag2->lVal = nullptr;
		tag2->rVal = $4;
		tag->add($2->getAsString()->getStringContent(),tag2);
	}
	else if(type == ParadoxType::DATE){
		tag2->lVal = nullptr;
		tag2->rVal = $4;
		tag->add($2->getAsDate()->getDateContent().toString(),tag2);
	}
	else if(type == ParadoxType::INTEGER){
		tag2->lVal = nullptr;
		tag2->rVal = $4;
		tag->add(std::to_string($2->getAsInteger()->getIntegerContent() / 1000),tag2);
	}
	else{
		tag2->lVal = $2;
		tag2->rVal = $4;
		tag->add("#PARAMETER_TEMPLATE",tag2);
	} 
  }
OPT: '[' '[' T_IDENT ']' A ']' {
	ParadoxOptionalTag* tag = createOptionalTag();
	tag->condition = $3;
	tag->content = $5->getAsTag();
	$$ = tag;
}
	| '[' '[' T_IDENT ']' BLANK A ']' {
	ParadoxOptionalTag* tag = createOptionalTag();
	tag->condition = $3;
	tag->content = $6->getAsTag();
	$$ = tag;
}
	|'[' '[' T_NUM_CONSTANT ']' A ']' {
	ParadoxOptionalTag* tag = createOptionalTag();
	tag->condition = std::to_string($3 / 1000);
	tag->content = $5->getAsTag();
	$$ = tag;
}
	| '[' '[' T_NUM_CONSTANT ']' BLANK A ']' {
	ParadoxOptionalTag* tag = createOptionalTag();
	tag->condition = std::to_string($3 / 1000);
	tag->content = $6->getAsTag();
	$$ = tag;
}
	| '[' '[' T_NUM_CONSTANT ']' T_PARAMETER ']' {
		ParadoxOptionalTag* tag = createOptionalTag();
		tag->condition = std::to_string($3 / 1000);
		ParadoxTag* tag2 = createTag();
		std::string name = "#PARAMETER_BLOCK";
		ParadoxParameter* par = createParameter();
		par->appendParameter($5);
		tag2->add(name,par);
		tag->content = tag2;
		$$ = tag;
	
}
	| '[' '[' T_IDENT ']' T_PARAMETER ']' {
		ParadoxOptionalTag* tag = createOptionalTag();
		tag->condition = $3;
		ParadoxTag* tag2 = createTag();
		std::string name = "#PARAMETER_BLOCK";
		ParadoxParameter* par = createParameter();
		par->appendParameter($5);
		tag2->add(name,par);
		tag->content = tag2;
		$$ = tag;
}
BLANK_OPT: '[' '[' T_IDENT ']' ']' {$$ = 1;}
		 | '[' '[' T_IDENT ']' BLANK ']' {$$ = 1;}
		 | '[' '[' T_NUM_CONSTANT ']' ']' {$$ = 1;}
		 | '[' '[' T_NUM_CONSTANT ']' BLANK ']' {$$ = 1;}
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
  | PC {$$ = $1;}
  
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
	else $$ = $1;
  }
PC: T_SPECIAL T_PARAMETER {
	ParadoxParameter* parameter = createParameter();
	parameter->appendString($1);
	parameter->appendParameter($2);
	$$ = parameter;
  }
  | T_IDENT T_PARAMETER{
	ParadoxParameter* parameter = createParameter();
	parameter->appendString($1);
	parameter->appendParameter($2);
	$$ = parameter;
  }
  | T_PARAMETER {
	ParadoxParameter* parameter = createParameter();
	parameter->appendParameter($1);
	$$ = parameter;
  }
  | T_NUM_CONSTANT T_PARAMETER {
	ParadoxParameter* parameter = createParameter();
	parameter->appendString(std::to_string($1));
	parameter->appendParameter($2);
	$$ = parameter;

  }
  | T_IDENT '[' '[' T_IDENT ']' T_IDENT ']' {
	ParadoxParameter* parameter = createParameter();
	parameter->appendString($1);
	std::string opt_literal = "OPT_LITERAL^";
	opt_literal.append($4);
	opt_literal.append("^");
	opt_literal.append($6);
	parameter->appendString(opt_literal);
	$$ = parameter;
  } 
  | PC T_NUM_CONSTANT {
	$$->getAsParameter()->appendString(std::to_string($2));
  
  }
  | PC T_NUM_CONSTANT T_PARAMETER {
	$$->getAsParameter()->appendString(std::to_string($2));
	$$->getAsParameter()->appendParameter($3);
  }
  | PC T_IDENT T_PARAMETER {
	$$->getAsParameter()->appendString($2);
	$$->getAsParameter()->appendParameter($3);
  }
  | PC T_PARAMETER {
	$$->getAsParameter()->appendParameter($2);
  }
  | PC T_IDENT {
	$$->getAsParameter()->appendString($2);
  }
BLANK: ' ' { $$ = 1;}
	 | BLANK ' '{ $$ = 1;}
LBR: '{' {$$ = 1;}
   | '{' BLANK {$$ = 1;}	
%%


void yyerror(const char* s) {
    fprintf(stderr, "Error: %s\n", s);
}