#include "pattern.h"
#include<string>
#include<string.h>
#include<iostream>
Pattern::Pattern(std::string str){
	this->patternString = str;
	pos = this->patternString.find("%");
	if(pos == std::string::npos){
		this->output = str;
		pos = str.length();
	}
	else{
		this->output = str.substr(0,pos);
	}
}
std::string Pattern::getOutput(){
	if(pos < this->patternString.length()){
		std::string ret = output;
		ret.append(this->patternString.substr(pos));
		return ret;
	}
	else return output;
}
bool Pattern::setNextString(std::string str){
	int pre = pos;
	int len = this->patternString.length();
	while(pos != std::string::npos){
		if(pos == len - 1) {
			pos++;
			return false;
		}
		if(this->patternString[pos+1] == '%') {
			this->output.append("%");
			pre = pos + 2;
		}
		else if(this->patternString[pos+1] == 'd') return false;
		else if(this->patternString[pos+1] == 'p') return false;
		else if(this->patternString[pos+1] == 's'){
			this->output.append(str);
			pre = pos + 2;
			pos = this->patternString.find("%",pre);
			this->output.append(this->patternString.substr(pre,pos-pre));
			while(pos != std::string::npos){
				if(pos == len - 1) {
					pos++;
					break;
				}
				if(this->patternString[pos+1] == '%') {
					this->output.append("%");
					pre = pos + 2;
				}
				else break;
				pos = this->patternString.find("%",pre);
				this->output.append(this->patternString.substr(pre,pos-pre));	
			}
			if(pos == std::string::npos) pos = len;
			return true;
		}
		pos = this->patternString.find("%",pre);
		this->output.append(this->patternString.substr(pre,pos-pre));	
	}
	this->output.append(this->patternString.substr(pre));
	pos = len;
	return false;

}
bool Pattern::setNextInteger(long long value){
	int pre = pos;
	int len = this->patternString.length();
	while(pos != std::string::npos){
		if(pos == len - 1) {
			pos++;
			return false;
		}
		if(this->patternString[pos+1] == '%') {
			this->output.append("%");
			pre = pos + 2;
		}
		else if(this->patternString[pos+1] == 'd') {
			int q = value % 1000;
			int p = value / 1000;
			while(q % 10 == 0 && q != 0) q /= 10;
			bool qminus = q < 0;
			if(qminus) q *= -1;
			if(q == 0){
				this->output.append(std::to_string(p));
			}
			else{
				char buffer[32];
				memset(buffer,0,32);
				if(p != 0 || !qminus)sprintf(buffer,"%d.%d",p,q);
				else sprintf(buffer,"-%d.%d",p,q);
				this->output.append(buffer);
			}
			pre = pos + 2;
			pos = this->patternString.find("%",pre);
			this->output.append(this->patternString.substr(pre,pos-pre));
			while(pos != std::string::npos){
				if(pos == len - 1) {
					pos++;
					break;
				}
				if(this->patternString[pos+1] == '%') {
					this->output.append("%");
					pre = pos + 2;
				}
				else break;
				pos = this->patternString.find("%",pre);
				this->output.append(this->patternString.substr(pre,pos-pre));	
			}
			if(pos == std::string::npos) pos = len;
			return true;
		}
		else if(this->patternString[pos+1] == 'p') {
			
			int q = value % 10;
			bool qminus = q < 0;
			if(qminus) q *= -1;		
			int p = value / 10;
			if(q == 0){
				this->output.append(std::to_string(p));
			}
			else{
				char buffer[32];
				memset(buffer,0,32);
				if(p != 0 || !qminus)sprintf(buffer,"%d.%d",p,q);
				else sprintf(buffer,"-%d.%d",p,q);
				this->output.append(buffer);
			}
			pre = pos + 2;
			pos = this->patternString.find("%",pre);
			this->output.append(this->patternString.substr(pre,pos-pre));
			while(pos != std::string::npos){
				if(pos == len - 1) {
					pos++;
					break;
				}
				if(this->patternString[pos+1] == '%') {
					this->output.append("%");
					pre = pos + 2;
				}
				else break;
				pos = this->patternString.find("%",pre);
				this->output.append(this->patternString.substr(pre,pos-pre));	
			}
			if(pos == std::string::npos) pos = len;
			return true;
		}
		else if(this->patternString[pos+1] == 's') return false;
		pos = this->patternString.find("%",pre);
		this->output.append(this->patternString.substr(pre,pos-pre));	
	}
	this->output.append(this->patternString.substr(pre));
	pos = len;
	return false;
}
