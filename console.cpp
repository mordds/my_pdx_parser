#include "modifier.h"
#include "trigger.h"
#include "db_object.h"
#include "localization.h"
#include "utils/parser_util.h"
#include "national_idea.h"
#include <iostream>
#include <map>
#include <bitset>
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
	
	loadNationalIdea();

    std::cout << "#Load Completed!" << std::endl;
    handlers["print_modifier"] = printModifier;
    handlers["print_trigger"] = printTrigger;
    handlers["trade_good"] = [](std::vector<std::string> vec){
        if(vec.empty()){
            std::cout << "用法:trade_good <good_id>" << std::endl;
            return;
        }
        Good* good = getGood(vec[0]);
        if(good == nullptr){
            std::cout << "没有名为" << vec[0] << "的商品" << std::endl;
            return;
        }
        std::cout << good->localizedName << std::endl;
        good->provinceModifier->localize();
        std::cout << "基础价格:" << good->defaultPrice / 1000.0 << std::endl;
        std::cout << good->globalModifier->localize(); 
        std::cout << good->provinceModifier->localize();
    };
	handlers["goods_list"] = [](std::vector<std::string> vec){
		std::map<std::string,std::string> map1;
		listGoods(map1);
		for(auto it : map1){
			std::cout << it.first << " " << it.second << std::endl;
		}
	};
	handlers["tag_idea"] = [](std::vector<std::string> vec){
		if(vec.empty()){
			std::cout << "用法:tag_idea <tag>" << std::endl;
            return;
		}
		const NationalIdea* idea = getTagIdea(vec[0]);
		if(idea == nullptr){
			std::cout << "没有找到" << vec[0] << "的国家理念" << std::endl;
			return;
		}
		std::cout << idea->toString() << std::endl;
	};
	handlers["ideas"] = [](std::vector<std::string> vec){
		if(vec.empty()){
			std::cout << "用法:tag_idea <tag>" << std::endl;
            return;
		}
		const NationalIdea* idea = getFromName(vec[0]);
		if(idea == nullptr){
			std::cout << "没有找到" << vec[0] << "的国家理念" << std::endl;
			return;
		}
		std::cout << idea->toString() << std::endl;
	};
	handlers["tag_idea_text"] = [](std::vector<std::string> vec){
				if(vec.empty()){
			std::cout << "用法:tag_idea_text <tag>" << std::endl;
            return;
		}
		const NationalIdea* idea = getTagIdea(vec[0]);
		if(idea == nullptr){
			std::cout << "没有找到" << vec[0] << "的国家理念" << std::endl;
			return;
		}
		for(int i = 0;i < 7;i++){
			std::cout << getLocalization(*idea->modifiers[i]->name) << std::endl;
			std::cout << getLocalization(*idea->modifiers[i]->name + "_desc") << std::endl;		
			if(i != 6) std::cout << "=========" << std::endl;	
		}
	};
	handlers["reload_loc"] = [](std::vector<std::string> vec){
		readLocalizations();
	};
	/*
	handlers["extract_mission"] = [](std::vector<std::string> vec){
		if(vec.empty()){
			std::cout << "usage: extract_mission <mission_file_name>";
			return;
		}
		ParadoxTag* root = parseFile(vec[0]);
		for(std::string str : root->seq){
			ParadoxTag* tag = root->getAsTag(str);
			if(tag == nullptr) continue;
			for(std::string str1: tag->seq){
				if(tag->get(str1)->getType() != ParadoxType::TAG) continue;
				std::cout << str1 << std::endl;
			}
		}
	};
	
	handlers["read_head"] = [](std::vector<std::string> vec){
		if(vec.empty()){
			std::cout << "usage: extract_mission <mission_file_name>";
			return;
		}
		ParadoxTag* root = parseFile(vec[0]);
		for(std::string str : root->seq){
			std::cout << str << std::endl;
		}
	};
	*/
	/*
	handlers["freespace"] = [](std::vector<std::string> vec){
		std::bitset<2600> tag1;
		for(int i = 0;i < 2600;i++) tag1.set(i,true);
		ParadoxTag* tag = parseFile("./datas/tags.txt")->getAsTag("tags");
		for(std::string str :tag->seq){
			
			if(str[1] >= '0' && str[1] <= '9' && str[2] >= '0' && str[2] <= '9'){
				int index = (str[0] - 'A') * 100;
				index += (str[1] - '0') * 10;
				index += (str[2] - '0');

				tag1.set(index,false);
			}
		}
		for(int i = 200;i <= 275;i++) tag1.set(i,false);
		for(int i = 300;i <= 375;i++) tag1.set(i,false);
		for(int i = 400;i <= 450;i++) tag1.set(i,false);
		for(int i = 500;i <= 520;i++) tag1.set(i,false);
		for(int i = 1000;i <= 1099;i++) tag1.set(i,false);
		for(int i = 1400;i <= 1410;i++) tag1.set(i,false);
		for(int i = 1800;i <= 1810;i++) tag1.set(i,false);
		for(int i = 1900;i <= 1975;i++) tag1.set(i,false);
		int start = -1;
		for(int i = 0;i < 26;i++){
			char t = ('A'+ i);
			for(int j = 0;j < 100;j++){
				if(tag1[i * 100 + j] && start == -1){
					start = j;
				}
				else if(start != -1 && !tag1[i * 100 + j]){
					if(start == j - 1) std::cout << t << start << std::endl;
					else {
						std::cout << t << start << "-" << t << j - 1 << std::endl; 
					}
					start = -1;
				}

			}
			if(start != -1){
				if(start == 99) std::cout << t << start << std::endl;
				else {
					std::cout << t << start << "-" << t << 99 << std::endl; 
				}
				start = -1;
			}

		}
	};

	*/
	
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
		if(command1 == "\x0f\x03\x17") break;
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