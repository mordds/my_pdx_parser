#ifndef PARADOX_TYPE
#define PARADOX_TYPE 2


#include<map>
#include<string>
#include<vector>
#include<stdio.h>
#include<cstdint>

enum class ParadoxType : uint8_t{
	BASE = 255,
	STRING = 0,
	INTEGER = 1,
	TAG = 2,
	ARRAY = 3,
	DATE = 4,
	SCOPE = 129,
	BOOLEAN = 130,
	GOOD = 131
};

struct Date{
	short year;
	unsigned char month;
	unsigned char day;
	std::string toString();
};

struct ParadoxBase;
struct ParadoxString;
struct ParadoxInteger;
struct ParadoxTag;
struct ParadoxArray;
struct ParadoxDate;


struct ParadoxBase{
	virtual void* getContent() = 0;
	virtual ParadoxType getType() = 0;
	ParadoxString* getAsString();
	ParadoxInteger* getAsInteger();
	ParadoxTag* getAsTag();
	ParadoxArray* getAsArray();
	ParadoxDate* getAsDate();
};

struct ParadoxString : public ParadoxBase{
	private:
		std::string content;
	public:
		
	ParadoxString(std::string str){ 
		content = str;
	}
	virtual void* getContent(){
		return (void*)&content;
	}
	virtual ParadoxType getType(){
		return ParadoxType::STRING;
	}
	std::string getStringContent(){
		
		return content;
	}
};
struct ParadoxInteger : public ParadoxBase{
	private:
		long long content;
	public:
		ParadoxInteger(long long val){
			content = val;
		}
		virtual void* getContent(){
			return (void*)&content;
		}
		virtual ParadoxType getType(){
			return ParadoxType::INTEGER;
		}
		long long getIntegerContent(){
			return content;
		}
};

struct ParadoxTag : public ParadoxBase{
	public:
		std::map<std::string,ParadoxBase*> tags;
		std::map<std::string,int> multiKeyCount;
		std::vector<std::string> seq;
		std::string assembleTagName(std::string name,int index){
			if(index <= 1) return name;
			else {
				name.append("@");
				name.append(std::to_string(index));
				return name;
				 
			}
		}
		
	public:
		virtual void* getContent(){
			return (void*)&tags;
		}
		virtual ParadoxType getType(){
			return ParadoxType::TAG;
		}
		ParadoxBase* get(std::string name);
		ParadoxBase* get(std::string name,int index);
		ParadoxBase* get(int index);
		ParadoxTag* getAsTag(std::string name);
		ParadoxTag* getAsTag(std::string name,int index);
		ParadoxTag* getAsTag(int index);
		
		void add(std::string name,ParadoxBase* base);
		void remove(std::string name,int index);
};

struct ParadoxArray : public ParadoxBase{
	std::vector<ParadoxBase*> contents;
	virtual ParadoxType getType(){
		return ParadoxType::ARRAY;
	}
	virtual void* getContent(){
		return (void*)&contents;
	}
	ParadoxBase* get(int index){
		if(index < 0 || contents.size() <= index) return nullptr;
		else return contents[index];
	}
	bool append(ParadoxBase* base){
		if(contents.empty()){
			contents.push_back(base);
			return true;
		}
		else if(contents[0]->getType() == base->getType()){
			contents.push_back(base);
			return true;
		}
		return false;
	}
};

struct ParadoxDate : public ParadoxBase{
	Date date;
	
	ParadoxDate(std::string dateString){
		sscanf(dateString.c_str(),"%d%*c%d%*c%d",&(date.year),&(date.month),&(date.day));
	}
	ParadoxDate(Date date){
		this->date = date;
	}
	virtual ParadoxType getType(){
		return ParadoxType::DATE;
	}
	virtual void* getContent(){
		return (void*)&date;
	}
	Date getDateContent(){
		return date;
	}
};

std::string stripTag(std::string original);
bool isCastable(ParadoxBase* base,ParadoxType type);
bool castToBool(ParadoxString* base);
bool Xor(bool a,bool b);
ParadoxBase* deep_copy(ParadoxBase*);
#endif
