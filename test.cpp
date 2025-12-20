#include "paradox_type.h"
#include "modifier.h"
#include "y.tab.h"
#include <iostream>
#include <stdio.h>
#include <set>
#include <unordered_map>
#include <filesystem>
extern YYSTYPE yylval;
extern FILE* yyin;
extern FILE* yyout;
extern int yyparse();
extern int yylex();
extern ParadoxTag* ROOT;
extern std::vector<ParadoxBase*> parsedObject;
extern std::map<std::string,ModifierObject> modifierObjects; 



int main(){
	yyin = fopen("a0.txt","r");
	loadInternalModifier();
	std::cout << "Registered Modifier Number:" << modifierObjects.size() << std::endl;
	yyparse();
	int token = yylex();
	if(token != 0){
		for(int i=0;i <parsedObject.size();i++){
			delete parsedObject[i];
		} 
		parsedObject.clear();
	}
	else{
		std::vector<Modifier> modifiers;
		ParseModifier(ROOT,modifiers);
		for(int i = 0;i < modifiers.size();i++){
			std::cout << modifiers[i].localize();
			std::cout << std::endl; 
			std::cout << std::endl; 
		}	
		for(int i=0;i <parsedObject.size();i++){
			delete parsedObject[i];
		} 
	}
	return 0;
} 


