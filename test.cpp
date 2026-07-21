#include "paradox_type.h"
#include "modifier.h"
#include "y.tab.h"
#include <iostream>
#include <stdio.h>
#include <set>
#include <unordered_map>
#include <filesystem>
#include <cstdlib> 
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif
extern YYSTYPE yylval;
extern FILE* yyin;
extern FILE* yyout;
extern int yyparse();
extern int yylex();
extern ParadoxTag* ROOT;
extern std::vector<ParadoxBase*> parsedObject;
extern std::map<std::string,ModifierObject> modifierObjects; 
//


int main(){
	//I HATE GBK ENCODING!!!
	#if defined(_WIN32) || defined(_WIN64)
	SetConsoleOutputCP(CP_UTF8);
	#endif
	std::cout << "请输入要解析的Modifier文件名称:" << std::endl;
	std::string name;
	getline(std::cin,name);
	yyin = fopen(name.c_str(),"r");
	loadInternalModifier();
	//std::cout << "Registered Modifier Number:" << modifierObjects.size() << std::endl;
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
	std::cin.get();
	std::cin.get();
	return 0;
} 


