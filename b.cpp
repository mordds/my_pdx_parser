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
extern std::map<std::string,TriggerItem*> registeredTriggers;

int main(int argc,char** argv){
	yyin = fopen("b0.txt","r");
	registerTriggerItems();
	yyparse();
		std::cout << "REGISTERD TRIGGER COUNT:" <<registeredTriggers.size() << std::endl;
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
	return 0;
} 
