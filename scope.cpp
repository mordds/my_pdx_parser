#include<string>
#include "scope.h"
#include "paradox_type.h"
#include "utils/string_util.h"
#include<sstream>
#include<string.h>
std::map<std::string,std::string> CustomScope::localizeMap = std::map<std::string,std::string>();
CountryScope* basicScopes[2600];
ProvinceScope* provinceScopes[8000];
std::map<std::string,Scope*> cachedScopes;
std::set<std::string> registeredCustomScopes;

ProvinceScope* Scope::getAsProvinceScope(){
	if(this->getType() == ScopeType::PROVINCE){
		return static_cast<ProvinceScope*>(this);
	}
	return nullptr;
}
CountryScope* Scope::getAsCountryScope(){
	if(this->getType() == ScopeType::COUNTRY){
		return static_cast<CountryScope*>(this);
	}
	return nullptr;
}
CustomScope* Scope::getAsCustomScope(){
	if(this->getType() == ScopeType::SPECIAL){
		return static_cast<CustomScope*>(this);
	}
	return nullptr;
}

ProvinceScope::ProvinceScope(int id){
	this->id = id;
}
CountryScope::CountryScope(std::string str){
	memset(this->tag,0,4);
	if(str.length() == 3){
		for(int i = 0;i < 3;i++) this->tag[i] = str[i];
	}
}
CustomScope::CustomScope(std::string str){
	this->data = str;
}
std::string ProvinceScope::toString(){
 	std::string ret = "";
 	ret.append("{{province_name|");
 	ret.append(std::to_string(this->id));
	ret.append("}}");
	return ret;
} 

std::string CountryScope::toString(){
	std::string ret = "";
	ret.append("{{tag_name_with_flag|");
	ret.append(this->tag);
	ret.append("}}");
	return ret;
}
std::string CustomScope::toString(){
	if(CustomScope::localizeMap.find(this->data) != CustomScope::localizeMap.end()){
		return CustomScope::localizeMap[this->data];
	}
	return "";
}
void CustomScope::registerLocalizeText(std::string key,std::string value){
	if(CustomScope::localizeMap.find(key) == CustomScope::localizeMap.end()) CustomScope::localizeMap[key] = value;
}
void initScope(){
	for(int i = 0;i < 2600;i++) basicScopes[i] = nullptr;
	for(int i = 0;i < 8000;i++) provinceScopes[i] = nullptr;
}


Scope* createScopeFromString(std::string str){
	//Check Number First
	if(isNumber(str)){
		std::stringstream sin(str);
		long long i;
		sin >> i;
		if(i < 8000){
			if(provinceScopes[i] == nullptr){
				provinceScopes[i] = new ProvinceScope(i);
			}
			return provinceScopes[i];
		}
	}
	else if(str.length() == 3){
		if(str[0] >= 'A' && str[0] <= 'Z'){
			if(str[1] >= '0' && str[1] <= '9' && str[2] >= '0' && str[2] <= '9'){
				int index = str[0] - 'A' * 100;
				index += str[1] - '0' * 10;
				index += str[2] - '0';
				if(basicScopes[index] == nullptr){
					basicScopes[index] = new CountryScope(str);
				}
				return basicScopes[index];
			}
			else if(str[1] >= 'A' && str[1] <= 'Z' && str[2] >= 'A' && str[2] <= 'Z'){
				if(cachedScopes.find(str) == cachedScopes.end()){
					cachedScopes[str] = new CountryScope(str);
				}
				return cachedScopes[str];
			}
		}
	}
	else if(startWith(str,"event_target:") && str.length() >= 13){
		if(cachedScopes.find(str) != cachedScopes.end()) return cachedScopes[str];
		CustomScope* scope = new CustomScope(str);
		CustomScope::registerLocalizeText(str,str);
		cachedScopes[str] = scope;
		return scope;
	}
	else{
		if(registeredCustomScopes.find(str) == registeredCustomScopes.end()) return nullptr;
		if(cachedScopes.find(str) == cachedScopes.end()){
			cachedScopes[str] = new CustomScope(str);
		}
		return cachedScopes[str];
	}
	return nullptr;
}
