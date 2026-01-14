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

void replaceWith(std::string& source,std::string target,std::string placer){
    size_t index = source.find(target);
    size_t size = placer.length();
    while(index != std::string::npos){
        source.replace(index,target.length(),placer);
        index = source.find(target,index + size);
    }
}

void eraseFrom(std::string& source,std::string target){
    size_t index = source.find(target);
    size_t size = target.length();
    while(index != std::string::npos){
        source.erase(index,size);
        index = source.find(target,index);
    }
}
void trim(std::string& source){
    size_t pos;
    for(pos = 0;pos < source.length();pos++){
        if(source[pos] != ' ') break;
    }
    source.erase(0,pos);
    for(pos = source.length() - 1;pos > 0;pos--){
        if(source[pos] != ' ') break; 
    }
    source.erase(pos + 1);
}