#include "pattern.h"
#include "utils/string_util.h"
#include "utils/parser_util.h"
#include "effect.h"
#include "trigger.h"
#include<string>
#include<string.h>
#include<iostream>
#include<string_view>


Pattern::Pattern(std::string str){
	this->patternString = str; 
	pos = 0;
	replaceWith(this->patternString,"%%","\x06");
}
std::string Pattern::getOutput(){
	replaceWith(this->patternString,"\x06","%");
	return this->patternString;
}
bool Pattern::setNextString(std::string str){
	size_t index = this->patternString.find('%',pos);
	if(index == std::string::npos || index == this->patternString.length() - 1) return false;
	char c = this->patternString[index + 1];
	if(c != 's') return false;
	else this->patternString.replace(index,2,str);
	this->pos = index + str.length();
	return true;
}
bool Pattern::setNextInteger(long long value){
	size_t index = this->patternString.find('%');
	if(index == std::string::npos || index == this->patternString.length() - 1) return false;
	size_t len = 2;
	char c = this->patternString[index + 1];
	std::string target;
	if(index != 0 && this->patternString[index - 1] == '-') { 
		value *= -1;
		len++;
		index--;
	}
	if(c == 'd'){
		long u = value / 1000;
		long v = value % 1000;	
		if(u < 0 || v < 0) {
			target.push_back('-');
			u *= -1;
			v *= -1;
		}
		target.append(std::to_string(u));
		if(v != 0){
			target.push_back('.');
			if(v < 100) target.push_back('0');
			if(v < 10) target.push_back('0');
			target.append(std::to_string(v));
		} 
		this->patternString.replace(index,len,target);
		pos = index + target.length();
	}
	else if(c == 'p'){
		long u = value / 10;
		long v = value % 10;	
		if(u < 0 || v < 0) {
			target.push_back('-');
			u *= -1;
			v *= -1;
		}
		target.append(std::to_string(u));
		if(v != 0){
			target.push_back('.');
			target.append(std::to_string(v));
		} 
		this->patternString.replace(index,len,target);
		pos = index + target.length();
	}
	else return false;
	return true;
}

NamedPattern::NamedPattern(std::string str){
	this->patternString = str; 
	replaceWith(this->patternString,"%%","\x06");
}

std::string NamedPattern::getOutput(){
	replaceWith(this->patternString,"\x06","%");
	return this->patternString;	
}

bool NamedPattern::fillName(const std::string& name,std::string content,int depth){
	std::string target = "%{";
	target.append(name);
	size_t index = this->patternString.find(target);
	if(index == std::string::npos) return false;
	size_t uIndex = index + target.length() ;
	if(this->patternString[uIndex] == '}'){
		this->patternString.replace(index,target.length() + 1,content);
	}
	else if(this->patternString[uIndex] == ':'){
		size_t pIndex = this->patternString.find('}',uIndex);
		std::string_view view(this->patternString);
		view = view.substr(uIndex + 1,pIndex - uIndex - 1);
		if(view.ends_with("literal")){
			ParadoxTag* root = parseString(content);
			if(root == nullptr) { clearParserDatas(); return false; }
			if(view.starts_with("effect")){
				std::unique_ptr<ComplexEffect> ce = createBaseEffect(depth);
				parseEffect(root,ce.get());
				//from there on view have invalidated.
				this->patternString.replace(index,pIndex - index + 1,ce->toString());
			}
			else if(view.starts_with("trigger")){
				std::unique_ptr<ComplexTrigger> ct = std::unique_ptr<ComplexTrigger>(createBaseTrigger());
				parseTrigger(root,ct.get());
				//from there on view have invalidated.
				this->patternString.replace(index,pIndex - index + 1,ct->toString(false,depth));				
			}
		}
		else if(view == "scope") {
			Scope* scope = createScopeFromString(content);
			if(scope == nullptr) return false;
			this->patternString.replace(index,pIndex - index + 1,scope->toString());
		}
		else if(view == "percent") return false;
	}
	return false;
}
bool NamedPattern::fillName(const std::string& name,long long content){
	std::string target = "%{";
	target.append(name);
	size_t index = this->patternString.find(target);
	if(index == std::string::npos) return false;
	size_t uIndex = index + target.length() ;
	if(index != 0 && name[index - 1] == '-') content *= -1;
	if(this->patternString[uIndex] == '}'){
		long long high = content / 1000;
		long long low = content % 1000;
		std::string rep = "";
		if(high < 0) rep.push_back('-');
		rep.append(std::to_string(high));
		rep.push_back('.');
		if(low < 100) rep.push_back('0');
		if(low < 10) rep.push_back('0');
		rep.append(std::to_string(low));
		this->patternString.replace(index,target.length() + 1,rep);
	}
	else if(this->patternString[uIndex] == ':'){
		size_t pIndex = this->patternString.find('}',uIndex);
		std::string_view view(this->patternString);
		view = view.substr(uIndex + 1,pIndex - uIndex - 1);
		if(view.ends_with("literal")) return false;
		else if(view == "percent") {
			long long high = content / 10;
			long long low = content % 10;
			std::string rep = "";
			if(high < 0) rep.push_back('-');
			rep.append(std::to_string(high));
			rep.push_back('.');
			rep.append(std::to_string(low));
			this->patternString.replace(index,pIndex - index + 1,rep);			
		}
		else if(view == "scope") {
			Scope* scope = getProvinceScope(content);
			if(scope == nullptr) return false;
			this->patternString.replace(index,pIndex - index + 1,scope->toString());
		}
	}
	return false;
}
