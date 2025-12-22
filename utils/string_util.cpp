#include "string_util.h"

bool isNumber(std::string str){
	for(int i = 0;i < str.length();i++){
		if(str[i] > '9' || str[i] < '0') return false;
	}
	return true;
}

bool startWith(std::string from,std::string to){
	if(from.length() < to.length()) return false;
	for(int i = 0;i < to.length();i++){
		if(from[i] != to[i]) return false;
	}
	return true;
}
std::pair<std::string,std::string> splitWith(std::string source,std::string split){
    size_t index = source.find(split);
    std::pair<std::string,std::string> ret;
    if(index == std::string::npos) {
        ret.first = source;
        ret.second = "";
    }
    else{
        ret.first = source.substr(0,index);
        ret.second = source.substr(index + split.length());
    }
    return ret;
}
std::string toUpperCase(std::string str){
    std::string ret = str;
    for(int i = 0;i < str.length();i++){
        if(ret[i] >= 'a' && ret[i] <= 'z') ret[i] -= 0x20;
    }
    return ret;
}

std::string toLowerCase(std::string str){
    std::string ret = str;
    for(int i = 0;i < str.length();i++){
        if(ret[i] >= 'a' && ret[i] <= 'z') ret[i] += 0x20;
    }
    return ret;
}