#include "pattern.h"
#include "utils/string_util.h"
#include<string>
#include<string.h>
#include<iostream>



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

