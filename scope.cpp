#include<string>
#include "scope.h"
#include "paradox_type.h"
#include "utils/string_util.h"
#include "localization.h"
#include<sstream>
#include<iostream>
#include<string.h>
#include<charconv>
std::map<std::string,std::string> AnyScope::localizeMap = std::map<std::string,std::string>();
std::map<std::string,std::string> UnitScope::localizeMap = std::map<std::string,std::string>();
std::map<std::string,std::string> MultiCountryScope::localizeMap = std::map<std::string,std::string>();
std::map<std::string,std::string> MultiProvinceScope::localizeMap = std::map<std::string,std::string>();
CountryScope* basicScopes[2600];
ProvinceScope* provinceScopes[8000];
std::map<std::string,Scope*> cachedScopes;
std::set<std::string> registeredCustomScopes;

bool hasRegistered(std::string str){
	if(isNumber(str)) return true; // Numbers should not be registered as AnyScope or MultiScope
	if(isTagString(str)) return true; //Tag Strings are reserved for tags.
	if(AnyScope::localizeMap.find(str) != AnyScope::localizeMap.end()) return true;
	if(MultiCountryScope::localizeMap.find(str) != MultiCountryScope::localizeMap.end()) return true;
	if(MultiProvinceScope::localizeMap.find(str) != MultiProvinceScope::localizeMap.end()) return true;
	return false;
}

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
AnyScope* Scope::getAsAnyScope(){
	if(this->getType() == ScopeType::ANY){
		return static_cast<AnyScope*>(this);
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
AnyScope::AnyScope(std::string str){
	this->data = str;
}
std::string ProvinceScope::toString(){
 	std::string ret = "PROV";
 	ret.append(std::to_string(this->id));
	ret = getLocalization(ret);
	return ret;
} 
std::string ProvinceScope::toHtml(){
 	std::string ret = "";
 	ret.append("{{province_name|");
 	ret.append(std::to_string(this->id));
	ret.append("}}");
	return ret;
} 
std::string CountryScope::toString(){
	return std::string(getLocalization(this->tag));
}
std::string CountryScope::toHtml(){
	std::string ret = "";
	ret.append("{{tag_name_with_flag|");
	ret.append(this->tag);
	ret.append("}}");
	return ret;
}
std::string AnyScope::toString(){
	if(AnyScope::localizeMap.find(this->data) != AnyScope::localizeMap.end()){
		return AnyScope::localizeMap[this->data];
	}
	return "<AnyScope>";
}

std::string UnitScope::toString(){
	if(UnitScope::localizeMap.find(this->data) != UnitScope::localizeMap.end()){
		return UnitScope::localizeMap[this->data];
	}
	return "<UnitScope>";
}


std::string MultiCountryScope::toString(){
	if(MultiCountryScope::localizeMap.find(this->data) != MultiCountryScope::localizeMap.end()){
		return MultiCountryScope::localizeMap[this->data];
	}
	return "<MultiCountryScope>";
}

std::string MultiProvinceScope::toString(){
	if(MultiProvinceScope::localizeMap.find(this->data) != MultiProvinceScope::localizeMap.end()){
		return MultiProvinceScope::localizeMap[this->data];
	}
	return "<MultiProvinceScope>";
}

void AnyScope::registerLocalizeText(std::string key,std::string value){
	if(hasRegistered(key)){
		std::cerr << "[ERROR][Scope.cpp:86 AnyScope::registerLocalizeText]: " <<
		 key << "have been registered or is not a valid Scope Name.";
		return;
	}
	AnyScope::localizeMap[key] = value;
}

void MultiProvinceScope::registerMultiProvinceScope(std::string key,std::string value){
	if(hasRegistered(key)){
		std::cerr << "[ERROR][Scope.cpp:94 MultiProvinceScope::registerMultiProvinceScope]: " << 
		key << "have been registered or is not a valid Scope Name.";
		return;
	}
	MultiProvinceScope::localizeMap[key] = value;	
}

void MultiCountryScope::registerMultiCountryScope(std::string key,std::string value){
	if(hasRegistered(key)){
		std::cerr << "[ERROR][Scope.cpp:104 MultiCountryScope::registerMultiCountryScope]: " << 
		key << "have been registered or is not a valid Scope Name.";
		return;
	}
	MultiCountryScope::localizeMap[key] = value;	
}



Scope* getCustomScope(std::string str){
	if(auto it = AnyScope::localizeMap.find(str); it != AnyScope::localizeMap.end()) return new AnyScope(it->first);
	if(auto it = MultiCountryScope::localizeMap.find(str); it != MultiCountryScope::localizeMap.end()) return new MultiCountryScope(it->first);
	if(auto it = MultiCountryScope::localizeMap.find(str); it != MultiCountryScope::localizeMap.end()) return new MultiCountryScope(it->first);
	return nullptr;
}

void initScope(){
	for(int i = 0;i < 2600;i++) basicScopes[i] = nullptr;
	for(int i = 0;i < 8000;i++) provinceScopes[i] = nullptr;
}

Scope* findScopeByName(std::string name,ScopeType type){
	bool exec_country = (type == ScopeType::ANY || type == ScopeType::COUNTRY);
	bool exec_province = (type == ScopeType::ANY || type == ScopeType::COUNTRY);
	if(exec_country){
		//check without createInstance.
		std::string str = "A00";
		for(str[0] = 'A';str[0] <= 'Z';str[0]++){
			for(str[1] = '0';str[1] <= '9';str[1]++){
				for(str[2] = '0';str[2] <= '9';str[2]++){
					if(!hasLocalization(str)) continue;
					if(getLocalization(str) == name){
						return createScopeFromString(str);
					}
				}			
			}
		}
	}
	if(exec_province){
		for(int i = 0;i < 8000;i++){
			std::string str = "PROV";
			str.append(std::to_string(i));
			if(!hasLocalization(str)) continue;
			if(getLocalization(str) == name){
				return getProvinceScope(i);
			}
		}
	}
	return nullptr;
}

ProvinceScope* getProvinceScope(int id){
	if(id < 8000){
		if(provinceScopes[id] == nullptr){
			provinceScopes[id] = new ProvinceScope(id);
		}
		return provinceScopes[id];		
	}
	return nullptr;
}

Scope* createScopeFromString(std::string str){
	
	if(isNumber(str)){
		long long i = -1;
		std::from_chars(str.data(),str.data()+str.size(),i);
		if(i < 0) return nullptr;
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
				int index = (str[0] - 'A') * 100;
				index += (str[1] - '0') * 10;
				index += (str[2] - '0');
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
		AnyScope* scope = new AnyScope(str);
		AnyScope::registerLocalizeText(str,str);
		cachedScopes[str] = scope;
		return scope;
	}
	else{
		if(cachedScopes.find(str) == cachedScopes.end()){
			Scope* scope = getCustomScope(str);
			if(scope != nullptr) cachedScopes[str] = scope;
		}
		return cachedScopes[str];
	}
	return nullptr;
}


void registerInternalScopes(){
	AnyScope::localizeMap["ROOT"] = "(ROOT)";
}