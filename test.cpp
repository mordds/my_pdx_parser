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

int mem = 0;
int rmem = 0;


void getAllFiles(std::string path, std::vector<std::string>& files) 
{
	std::filesystem::path dir(path);
	for (const auto& entry : std::filesystem::directory_iterator(dir)) {
		files.push_back(entry.path().string());
	}
}



void print(ParadoxBase* base){
	if(base==nullptr) return;
	if(base->getType() == ParadoxType::INTEGER){
		mem += sizeof(ParadoxInteger);
		rmem += sizeof(int);
		std::cout << " :int " << base->getAsInteger()->getIntegerContent();
	}
	else if(base->getType() == ParadoxType::STRING){
		mem += sizeof(ParadoxString);
		rmem += sizeof(std::string);
		int len = base->getAsString()->getStringContent().length();
		rmem += len >= 16 ? 0 : len + 1;
		std::cout << " :string " << base->getAsString()->getStringContent();
		
	}
	else if(base->getType() == ParadoxType::TAG){
		mem += sizeof(ParadoxTag);
		std::cout << " :tag" << std::endl;
		for(auto &c : base->getAsTag()->seq){
			rmem += sizeof(std::string) * 2;
			int len = c.length();
			rmem += (len >= 16 ? 0 : len + 1) * 2;
			std::cout << c;
			print(base->getAsTag()->tags[c]);
			std::cout << std::endl;
		}
	}
	else if(base->getType() == ParadoxType::PARAMETER){
		std::cout << " :parameter ";
		mem += sizeof(ParadoxParameter);
		ParadoxParameter* par = base->getAsParameter();
		for(int i = 0;i < par->parameterTemplate.size();i++){
			rmem += sizeof(std::string);
			int len = par->parameterTemplate[i].length();
			rmem += (len >= 16 ? 0 : len + 1) * 2;
			if(par->parameterIndex.find(i) != par->parameterIndex.end()) {
				std::cout << "$" << par->parameterTemplate[i] << "$";
			}
			else {
				std::cout << par->parameterTemplate[i];
			}
		} 
	}
	else if(base->getType() == ParadoxType::ARRAY){
		
	}
	else if(base->getType() == ParadoxType::OPT_TAG){
		mem += 56;
		std::cout << " :optional tag " << std::endl;
		std::cout << "#CONTENT#";
		print(base->getAsOptional()->content);
		std::cout << std::endl;
	}
	else if(base->getType() == ParadoxType::COMP_TAG){
		mem += 32;
		std::cout << " :complicated tag " << std::endl;
		std::cout << "#CONTENT#";
		print(base->getAsComplicate()->lVal);
		std::cout << base->getAsComplicate()->op;
		print(base->getAsComplicate()->rVal);
		std::cout << std::endl;
	} 
}
 
bool scriptedMode = false; 
int _main(){
   	std::vector<std::string> files;
   	getAllFiles("./scripted_effect/",files);
	for(int i = 0;i < files.size();i++){
		std::string fileName = files[i];
		std::string outName = "./result_se"+files[i].substr(16);
		//std::cout << outName.c_str() << std::endl;
		yyin = fopen(fileName.c_str(),"r");
		freopen(outName.c_str(),"w+",stdout);
		
	   	yyparse();
		int token = yylex();
		if(token != 0){
			for(int i=0;i <parsedObject.size();i++){
				delete parsedObject[i];
			} 
			parsedObject.clear();
			freopen("CON", "w", stdout);
			std::cout << "Error at " << fileName << std::endl;
			freopen(outName.c_str(),"w+",stdout);
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
			std::cout << "ROOT";
		   	print(ROOT);
		   	std::cout << "memory used in nodes:" << mem << std::endl;
		   	std::cout << "memory used in datas:" << rmem << std::endl;
		   	for(int i=0;i <parsedObject.size();i++){
				delete parsedObject[i];
			} 
			parsedObject.clear();
		    
		}
	}
	return 0;


}

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
		std::vector<Modifier> modifiers;
		ParseModifier(ROOT,modifiers);
		for(int i = 0;i < modifiers.size();i++){
			std::cout << modifiers[i].localizeHtml();
			std::cout << std::endl; 
		}	
		for(int i=0;i <parsedObject.size();i++){
			delete parsedObject[i];
		} 
	}
	return 0;
} 

int __main(){
	yyin = fopen("a0.txt","r");
	int token = yylex();
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
	token = fgetc(yyin);
	while(token != -1){
		std::cout << (char)token;
		token = fgetc(yyin);
	}
	return 0;
} 

