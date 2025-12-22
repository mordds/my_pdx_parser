#include "localization.h"
#include "utils/string_util.h"
#include<vector>
#include<filesystem>
#include<fstream>
#include<map>
#include<iostream>
std::map<std::string,std::string> localizations;



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

void readLocalizations(){
    std::vector<std::string> files;
    getAllFiles("./localization",files);
    for(std::string str : files){
        
        readFromFiles(str);
    }
}