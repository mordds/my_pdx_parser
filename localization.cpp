#include "localization.h"
#include "utils/string_util.h"
#include<vector>
#include<filesystem>
#include<fstream>
#include<map>
#include<iostream>
#include<set>
std::map<std::string,std::string> localizations;
std::set<std::string> tempRef;


void getAllFiles(std::string path, std::vector<std::string>& files) 
{
	std::filesystem::path dir(path);
	for (const auto& entry : std::filesystem::directory_iterator(dir)) {
		files.push_back(entry.path().string());
	}
}

void readFromFiles(std::string path){
  
    std::ifstream fin;
    fin.open(path);
    while(!fin.eof()){
        std::string line;
        std::getline(fin,line);
        if(line.length() == 0) continue; 
        if(line[0] == '#') continue;
        std::pair<std::string,std::string> pair = splitWith(line,":");
        replaceWith(pair.second,"\\n","\n");
        localizations[pair.first] = pair.second;
    }
    fin.close();
}



std::string getLocalization(std::string key){
    std::string ret;
    if(localizations.find(key) == localizations.end()) ret = key;
    else ret = localizations[key];
    return ret;
}
const std::string* getLocalizationKeyPtr(std::string key){
    auto it = localizations.find(key);
    if(it != localizations.end()) return &(it->first);
    auto it2 = tempRef.insert(key).first;
    if(it2 == tempRef.end())  return &(*tempRef.insert(key).first);
    return &(*it2);
}
void readLocalizations(){
    std::cout << "#loading locs..." << std::endl;
    std::vector<std::string> files;
    getAllFiles("./localization",files);
    for(std::string str : files){
        readFromFiles(str);
    }
    std::cout << "#locs loaded!" << std::endl;
}