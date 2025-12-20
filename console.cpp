#include "modifier.h"
#include "trigger.h"
#include "db_object.h"
#include "localization.h"
#include "db_object.h"
#include "utils/parser_util.h"
#include <iostream>
#include <map>
#include <sstream>

typedef void(*CommandHandler)(std::vector<std::string>);

std::map<std::string,CommandHandler> handlers;

void printModifier(std::vector<std::string> vec){
    ParadoxTag* root = parseFile(vec[0]);
	std::vector<Modifier> modifiers;
	ParseModifier(root,modifiers);
	for(int i = 0;i < modifiers.size();i++){
		std::cout << modifiers[i].localize();
		std::cout << std::endl; 
		std::cout << std::endl; 
	}	
    clearParserDatas();
}

void printTrigger(std::vector<std::string> vec){
	bool multiTriggers = true; 
    ParadoxTag* root = parseFile(vec[0]);
	if(vec.size() >= 2){
		std::string arg0(vec[1]);
		if(arg0 == "single"){
			multiTriggers = false;
		}
		else if(arg0 == "multi"){
			multiTriggers = true;
		} 
	}
	else{
		for(int i = 0;i < root->seq.size();i++){
			if(root->get(i)->getType() != ParadoxType::TAG){
				multiTriggers = false;
				break; 
			}
		}
	}
	if(multiTriggers){
		for(auto it = root->tags.begin();it != root->tags.end();it++){
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
		parseTrigger(root,ct);
		std::cout << ct->toString(false) << std::endl;
		delete ct;
	}
    clearParserDatas();
}

int main(){
    using namespace std;

    readLocalizations();

    loadInternalModifier();

    registerGood();

    registerTriggerItems();
    std::cout << "#Load Completed!" << std::endl;
    handlers["help"] = [](std::vector<std::string> vec){
        std::cout << "help -------- 查看帮助" << std::endl; 
    };
    handlers["print_modifier"] = printModifier;
    handlers["print_trigger"] = printTrigger;
    handlers["trade_good"] = [](std::vector<std::string> vec){
        Good* good = getGood(vec[0]);
        if(good == nullptr) return;
        std::cout << good->localizedName << std::endl;
        good->provinceModifier->localize();
        std::cout << "基础价格:" << good->defaultPrice / 1000.0 << std::endl;
        std::cout << good->globalModifier->localize(); 
        std::cout << good->provinceModifier->localize();
    };
    string command,command1;
	vector<string> args;
	while(true){
		
		getline(cin,command);
		if(command.length() == 0) continue;
		stringstream ss;
		ss << command;
		string command1;
		string command2 = "";
		ss >> command >> command1;
		bool shouldMerge = command1[0] == '\"';
		bool broke = false;
		while(command1.length() != 0){
			if(!shouldMerge){
				args.push_back(command1);
				command1 = "";
				ss >> command1;
			} 
			else {
				ss >> command2;

				if(command1[command1.length() - 1] == '\"') {
					shouldMerge = false;
					command1 = command1.substr(1,command1.length() - 2);
					command2 = "";
					continue;
				}
				command1.append(" ");
				command1.append(command2);
				if(command2.length() == 0) {
					cout << "ERROR:Incomplete Quotation!" << endl; 
					broke = true;
					break;
				}
				
				command2 = "";
			}
			
		}
		if(broke) {
			args.clear();
			cout << endl;	
			continue;
		}	
		if(handlers.find(command) == handlers.end()) cout << "Command Not Found." << endl;
		else handlers[command](args);
		args.clear();
		cout << endl;
	}

}