#include "trigger.h"
#include "paradox_type.h"
#include "pattern.h"
#include<map>
#include<vector>
#include<string>
#include<iostream>
#include "y.tab.h"


extern YYSTYPE yylval;
extern FILE* yyin;
extern FILE* yyout;
extern int yyparse();
extern int yylex();
extern ParadoxTag* ROOT;
extern std::vector<ParadoxBase*> parsedObject;

int _main(){
	Pattern p("a:%d%%,b:%s%%abc%%def");
	p.setNextInteger(2120);
	std::cout << p.getOutput() << std::endl;
	p.setNextInteger(2000);
	std::cout << p.getOutput() << std::endl;
	p.setNextString("你好!");
	 std::cout << p.getOutput() << std::endl;
	return 0;
}

int main(int argc,char** argv){
	yyin = fopen("b0.txt","r");
	registerItems();
	yyparse();
	int token = yylex();
	if(token != 0){
		for(int i=0;i <parsedObject.size();i++){
			delete parsedObject[i];
		} 
		parsedObject.clear();
		while(token != 0){
			std::cout << token << ' ';
			if(token == T_IDENT || token == T_SPECIAL){
				std::cout << yylval.name << std::endl;
			}
			else if(token == T_LITERAL){
				std::cout << *(yylval.str) << std::endl;
			}
			else if(token == T_NUM_CONSTANT){
				std::cout << yylval.num << std::endl;
			}
			else if(token == T_PARAMETER){
				std::cout << '$' << yylval.name << '$' << std::endl;
			}
			else{
				std::cout << (char)token << std::endl;
			}
			token = yylex();
		}
	}
	else{
		bool multiTriggers = true; 
		// declared mode directly
		if(argc >= 2){
			std::string arg0(argv[1]);
			if(arg0 == "single"){
				multiTriggers = false;
			}
			else if(arg0 == "multi"){
				multiTriggers = true;
			} 
		}
		else{
			for(int i = 0;i < ROOT->seq.size();i++){
				if(ROOT->get(i)->getType() != ParadoxType::TAG){
					multiTriggers = false;
					break; 
				}
			}
		}
		if(multiTriggers){
	
			for(auto it = ROOT->tags.begin();it != ROOT->tags.end();it++){
				ComplexTrigger* ct = createBaseTrigger();
				parseTrigger(it->second->getAsTag(),ct);
				std::cout << it->first << std::endl;
				std::cout << ct->toString(false) << std::endl;
				delete ct;
			}
		}
		else{
			ComplexTrigger* ct = createBaseTrigger();
			ct->depth = 0;
			parseTrigger(ROOT,ct);
			std::cout << ct->toString(false) << std::endl;
			delete ct;
		}
		
	}
	return 0;
} 
