#include "paradox_type.h"
#include "utils/string_util.h"
#include "scope.h"
#include <iostream>
#include <algorithm>

extern ParadoxTag* createTag();
extern ParadoxArray* createArray();
extern ParadoxString* createString(std::string);
extern ParadoxInteger* createInteger(long long);
extern ParadoxDate* createDate(Date);
extern ParadoxScope* createScope(Scope*);


ParadoxBoolean* getBooleanInstance(bool value){
	static ParadoxBoolean BOOLEAN_YES(true);
	static ParadoxBoolean BOOLEAN_NO(false);
	return (value ? &BOOLEAN_YES : &BOOLEAN_NO);
}

ParadoxString* ParadoxBase::getAsString(){
	if(getType() != ParadoxType::STRING) return nullptr;
	return static_cast<ParadoxString*>(this); 
}
int ParadoxTag::size(){
	return seq.size();
}
ParadoxInteger* ParadoxBase::getAsInteger(){
	if(getType() != ParadoxType::INTEGER) return nullptr;
	return static_cast<ParadoxInteger*>(this); 
}
ParadoxTag* ParadoxBase::getAsTag(){
	if(getType() != ParadoxType::TAG) return nullptr;
	return static_cast<ParadoxTag*>(this); 
}
ParadoxArray* ParadoxBase::getAsArray(){
	if(getType() != ParadoxType::ARRAY) return nullptr;
	return static_cast<ParadoxArray*>(this); 
}
ParadoxDate* ParadoxBase::getAsDate(){
	if(getType() != ParadoxType::DATE) return nullptr;
	return static_cast<ParadoxDate*>(this); 
}
ParadoxBoolean* ParadoxBase::getAsBoolean(){
	if(getType() != ParadoxType::BOOLEAN) return nullptr;
	return static_cast<ParadoxBoolean*>(this);
}
ParadoxScope* ParadoxBase::getAsScope(){
	if(getType() != ParadoxType::SCOPE) return nullptr;
	return static_cast<ParadoxScope*>(this);
}

ParadoxBase* ParadoxTag::get(std::string name){
	if(tags.find(name) == tags.end()) return nullptr;
	return tags[name];
}

ParadoxBase* ParadoxTag::get(std::string name,int index){
	std::string aName = assembleTagName(name,index);
	if(tags.find(aName) == tags.end()) return nullptr;
	return tags[aName];
}
ParadoxBase* ParadoxTag::get(int index){
	if(index >= this->seq.size()) return nullptr; 
	std::string key = this->seq[index];
	return tags[key];
}
ParadoxTag* ParadoxTag::getAsTag(std::string name,int index){
	ParadoxBase* tag = get(name,index);
	return tag == nullptr ? nullptr : tag->getAsTag();
}
ParadoxTag* ParadoxTag::getAsTag(int index){
	ParadoxBase* tag = get(index);
	return tag == nullptr ? nullptr : tag->getAsTag();
}
ParadoxTag* ParadoxTag::getAsTag(std::string name){
	ParadoxBase* tag = get(name);
	return tag == nullptr ? nullptr : tag->getAsTag();
}
void ParadoxTag::add(std::string name,ParadoxBase* base){
	if(tags.find(name) == tags.end()){
		tags[name] = base;
		seq.push_back(name);
	}
	else if(multiKeyCount.find(name) == multiKeyCount.end()){
		multiKeyCount[name] = 2;
		std::string key1 = assembleTagName(name,2);	
		tags[key1] = base; 	
		seq.push_back(key1);
	}
	else {
		multiKeyCount[name]++;
		std::string key1 = assembleTagName(name,multiKeyCount[name]);	
		tags[key1] = base;	
		seq.push_back(key1);
	}
}
void ParadoxTag::remove(std::string name,int index){
	if(tags.find(name) == tags.end()) return;
	if(multiKeyCount.find(name) == multiKeyCount.end()){
		if(index >= 2) return;
		tags.erase(name);
		auto it = std::find(seq.begin(),seq.end(),name);
		if(it != seq.end()) seq.erase(it);
	}
	else{
		int cnt = multiKeyCount[name];
		if(index > cnt) return;
		if(index <= 1){
			std::string from = name;
			from.append("@2");
			tags[name] = tags[from];
			index++;
		}
		for(int i = index;i < cnt;i++){
			std::string from = name;
			from.append("@");
			from.append(std::to_string(i));
			std::string to = name;
			from.append("@");
			from.append(std::to_string(i + 1));
			tags[from] = tags[to];
		}
		int r1 = 2;
		std::string target = assembleTagName(name,r1);
		auto it2 = std::find(seq.begin(),seq.end(),name);
		auto to_remove = seq.begin();
		if(index == 1) to_remove = it2;
		while(r1 < cnt){
			if(*it2 == target){
				r1++;
				target = assembleTagName(name,r1);
				if(r1 == index + 1){
					to_remove = it2;
				}
				else if(r1 > index + 1){
					*it2 = assembleTagName(name,r1 - 2);
				}
			}
			it2++;
		}
		seq.erase(to_remove);
		std::string rem = name;
		rem.append("@");
		rem.append(std::to_string(cnt));
		tags.erase(rem);
		auto it = std::find(seq.begin(),seq.end(),rem);
		if(it != seq.end()) seq.erase(it);
		multiKeyCount[name]--;
		if(multiKeyCount[name] == 1) multiKeyCount.erase(name);
	}
}
std::string Date::toString() const {
	std::string str = "";
	str.append(std::to_string((int)this->year));
	str.append(".");
	str.append(std::to_string((int)this->month));
	str.append(".");
	str.append(std::to_string((int)this->day));
	return str;
}

std::string ParadoxScope::toString() const { return scope->toString(); }

std::string stripTag(std::string original){
	size_t pos = original.find_last_of('@');
	if(pos != std::string::npos){
		original.erase(pos);
	}
	return original;
}
bool Xor(bool a,bool b){
	return (a || b) && !(a && b);
} 

bool isCastable(ParadoxBase* base,ParadoxType type){
	if(base->getType() == type){
		return true;
	}
	else if(type == ParadoxType::SCOPE){
		if(base->getType() == ParadoxType::INTEGER){
			return base->getAsInteger()->getIntegerContent() <= 8000 * 1000L;
		}
		if(base->getType() == ParadoxType::STRING){
			return true;
		}
	}
	return false;
}
//!WARNING!
//the returned pointer is created by 'new' operator and the caller has the responsibility to manage memory
//this function is used to create a ParadoxBase Object which do not managed by global object manager.
ParadoxBase* deep_copy(ParadoxBase* base){
	if(base->getType() == ParadoxType::INTEGER){
		ParadoxInteger* pInt = base->getAsInteger();
		return new ParadoxInteger(pInt->getIntegerContent());
	}
	if(base->getType() == ParadoxType::STRING){
		ParadoxString* pStr = base->getAsString();
		return new ParadoxString(pStr->getStringContent());
	}
	if(base->getType() == ParadoxType::DATE){
		ParadoxDate* pDate = base->getAsDate();
		return new ParadoxDate(pDate->getDateContent());
	}
	if(base->getType() == ParadoxType::ARRAY){
		ParadoxArray* pArray = base->getAsArray();
		ParadoxArray* nArray = new ParadoxArray();
		for(ParadoxBase* base:pArray->contents){
			ParadoxBase* base_copy = deep_copy(base);
			nArray->append(base_copy);
		}
		return nArray;
	}
	if(base->getType() == ParadoxType::BOOLEAN){
		ParadoxBoolean* pBoolean = base->getAsBoolean();
		ParadoxBoolean* nBoolean = new ParadoxBoolean(pBoolean->getValue());
		return nBoolean;
	}
	if(base->getType() == ParadoxType::SCOPE){
		ParadoxScope* pScope = base->getAsScope();
		ParadoxScope* nScope = new ParadoxScope(pScope->getValue());
		return nScope;
	}
	else {
		ParadoxTag* pTag = base->getAsTag();
		ParadoxTag* nTag = new ParadoxTag();
		for(std::string entry : pTag->seq){
			ParadoxBase* base_copy = deep_copy(pTag->get(entry));
			nTag->add(entry,base_copy);
		}
		return nTag;
	}
}
//this function is used to deep copy a ParadoxBase Object safely.
ParadoxBase* deep_copy_safe(ParadoxBase* base){
	if(base->getType() == ParadoxType::INTEGER){
		ParadoxInteger* pInt = base->getAsInteger();
		return createInteger(pInt->getIntegerContent());
	}
	if(base->getType() == ParadoxType::STRING){
		ParadoxString* pStr = base->getAsString();
		return createString(pStr->getStringContent());
	}
	if(base->getType() == ParadoxType::DATE){
		ParadoxDate* pDate = base->getAsDate();
		return createDate(pDate->getDateContent());
	}
	if(base->getType() == ParadoxType::ARRAY){
		ParadoxArray* pArray = base->getAsArray();
		ParadoxArray* nArray = createArray();
		for(ParadoxBase* base:pArray->contents){
			ParadoxBase* base_copy = deep_copy_safe(base);
			nArray->append(base_copy);
		}
		return nArray;
	}
	if(base->getType() == ParadoxType::BOOLEAN){
		ParadoxBoolean* pBoolean = base->getAsBoolean();
		ParadoxBoolean* nBoolean = new ParadoxBoolean(pBoolean->getValue());
		extern std::vector<ParadoxBase*> parsedObject;
		parsedObject.push_back(nBoolean);
		return nBoolean;
	}
	if(base->getType() == ParadoxType::SCOPE){
		ParadoxScope* pScope = base->getAsScope();
		ParadoxScope* nScope = createScope(pScope->getValue());
		return nScope;
	}	
	else {
		ParadoxTag* pTag = base->getAsTag();
		ParadoxTag* nTag = createTag();
		for(std::string entry : pTag->seq){
			ParadoxBase* base_copy = deep_copy_safe(pTag->get(entry));
			nTag->add(entry,base_copy);
		}
	
		return nTag;
	}

}
template<ParadoxObject T>
ParadoxBase* castTo(T base,ParadoxType type){
	if(base == nullptr) return nullptr;
	ParadoxBase* aBase = base;
	if(aBase->getType() == type) return base;
	else return nullptr;
}

template<>
ParadoxBase* castTo(ParadoxBase* base,ParadoxType type){
	if(base == nullptr) return nullptr;
	if(base->getType() == ParadoxType::INTEGER){
		return castTo(static_cast<ParadoxInteger*>(base),type);
	}
	if(base->getType() == ParadoxType::STRING){
		return castTo(static_cast<ParadoxString*>(base),type);
	}
	if(base->getType() == type) return base;
	else return nullptr;
}

template<>
ParadoxBase* castTo(ParadoxString* string,ParadoxType type){
	if(string == nullptr) return nullptr;
	if(type == ParadoxType::STRING) return string;
	if(type == ParadoxType::SCOPE){
		Scope* scope = createScopeFromString(string->getStringContent());
		if(scope == nullptr) return nullptr;
		return createScope(scope);
	}
	return nullptr;
}
//
template<>
ParadoxBase* castTo(ParadoxInteger* number, ParadoxType type){
	if(number == nullptr) return nullptr;
	if(type == ParadoxType::INTEGER) return number;
	if(type == ParadoxType::SCOPE) {
		Scope* scope = getProvinceScope(number->getIntegerContent());
		if(scope == nullptr) return nullptr;
		return new ParadoxScope(scope);
	}
	return nullptr;
}

