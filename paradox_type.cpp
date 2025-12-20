#include "paradox_type.h"
#include "utils/string_util.h"
#include <algorithm>


namespace pdx{
	std::vector<ParadoxBase*> tempObjects;
	ParadoxString* createString(std::string str){
		ParadoxString* pstr = new ParadoxString(str);
		pdx::tempObjects.push_back(pstr);
		return pstr;
	}
}
ParadoxString* ParadoxBase::getAsString(){
	if(getType() != ParadoxType::STRING) return nullptr;
	return dynamic_cast<ParadoxString*>(this); 
}
ParadoxInteger* ParadoxBase::getAsInteger(){
	if(getType() != ParadoxType::INTEGER) return nullptr;
	return dynamic_cast<ParadoxInteger*>(this); 
}
ParadoxTag* ParadoxBase::getAsTag(){
	if(getType() != ParadoxType::TAG) return nullptr;
	return dynamic_cast<ParadoxTag*>(this); 
}
ParadoxArray* ParadoxBase::getAsArray(){
	if(getType() != ParadoxType::ARRAY) return nullptr;
	return dynamic_cast<ParadoxArray*>(this); 
}
ParadoxDate* ParadoxBase::getAsDate(){
	if(getType() != ParadoxType::DATE) return nullptr;
	return dynamic_cast<ParadoxDate*>(this); 
}
ParadoxParameter* ParadoxBase::getAsParameter(){
	if(getType() != ParadoxType::PARAMETER) return nullptr;
	return dynamic_cast<ParadoxParameter*>(this); 
}
ParadoxOptionalTag* ParadoxBase::getAsOptional(){
	if(getType() != ParadoxType::OPT_TAG) return nullptr;
	return dynamic_cast<ParadoxOptionalTag*>(this);
}
ParadoxComplicateTag* ParadoxBase::getAsComplicate(){
	if(getType() != ParadoxType::COMP_TAG) return nullptr;
	return dynamic_cast<ParadoxComplicateTag*>(this);
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
//TODO Overwrite all Parser and ParadoxType)
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
std::string Date::toString(){
	std::string str = "";
	str.append(std::to_string((int)this->year));
	str.append(".");
	str.append(std::to_string((int)this->month));
	str.append(".");
	str.append(std::to_string((int)this->day));
	return str;
}

void ParadoxParameter::appendParameter(std::string parameter){
	int index = parameterTemplate.size();
	parameterTemplate.push_back(parameter);
	parameterIndex[index] = parameter;
} 

void ParadoxParameter::appendString(std::string str){
	parameterTemplate.push_back(str);
}

std::string ParadoxParameter::assemble(std::map<std::string,std::string> parameters){
	int pEnd = parameterIndex.size();
	int* index3 = new int [pEnd + 1];
	index3[pEnd] = parameterTemplate.size() + 1;
	int pos = 0;
	for(auto &p : parameterIndex) {
		index3[pos] = p.first;
		pos++;
	}
	pos = 0;
	std::string ret = "";
	for(int i = 0;i < parameterTemplate.size();i++){
		if(i == index3[pos]){
			ret.append(parameters[parameterIndex[pos]]);
			pos++;
		}
		else {
			ret.append(parameterTemplate[i]);
		}
	}
	return ret;
}
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
	if((int)type < 127){
		return base->getType() == type;
	}
	else{
		if(base->getType() == ParadoxType::INTEGER){
			return base->getAsInteger()->getIntegerContent() <= 32767L && type == ParadoxType::SCOPE;
		}
		if(base->getType() == ParadoxType::STRING){
			std::string content = base->getAsString()->getStringContent();
			if(type == ParadoxType::BOOLEAN){
				return content == "yes" || content == "no";
			}
			else if(type == ParadoxType::SCOPE){
				if(startWith(content,"event_target:") && content.length() > 13) return true;
				else if(content.length() == 3){
					if(content[0] < 'A' || content[0] > 'Z') return false;
					if((content[1] >= 'A' && content[1] <= 'Z') || (content[1] >= '0' && content[1] <= '9')){
						return (content[2] >= 'A' && content[2] <= 'Z') || (content[2] >= '0' && content[2] <= '9');
					}
				}
			}
		}
		return false;
	}
}
bool castToBool(ParadoxString* string){
	return string->getStringContent() == "yes";
}

