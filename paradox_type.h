#ifndef PARADOX_TYPE
#define PARADOX_TYPE 2


#include<map>
#include<string>
#include<vector>
#include<stdio.h>
#include<cstdint>

enum ParadoxType : uint8_t{
	BASE = 255,
	STRING = 0,
	INTEGER = 1,
	TAG = 2,
	ARRAY = 3,
	DATE = 4,
	OPT_TAG = 5,
	PARAMETER = 6,
	COMP_TAG = 7,
	SCOPE = 129,
	BOOLEAN = 130
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
struct ParadoxParameter;
struct ParadoxOptionalTag;
struct ParadoxComplicateTag;

struct ParadoxBase{
	virtual void* getContent() = 0;
	virtual ParadoxType getType() = 0;
	ParadoxString* getAsString();
	ParadoxInteger* getAsInteger();
	ParadoxTag* getAsTag();
	ParadoxArray* getAsArray();
	ParadoxDate* getAsDate();
	ParadoxParameter* getAsParameter();
	ParadoxOptionalTag* getAsOptional();
	ParadoxComplicateTag* getAsComplicate();
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
		ParadoxBase* get(std::string name,int index);
		ParadoxBase* get(int index);
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

struct ParadoxOptionalTag : public ParadoxBase{
	std::string condition;
	ParadoxTag* content;
	virtual ParadoxType getType(){
		return ParadoxType::OPT_TAG;
	}
	virtual void* getContent(){
		return (void*)content;
	}
	ParadoxTag* getTag(){
		return content;
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
struct ParadoxParameter : public ParadoxBase{
	std::vector<std::string> parameterTemplate;
	std::map<int,std::string> parameterIndex;
	virtual void* getContent(){
		return nullptr;
	}
	virtual ParadoxType getType(){
		return ParadoxType::PARAMETER;
	}
	void appendParameter(std::string parameter);
	void appendString(std::string str);
	std::string assemble(std::map<std::string,std::string> parameters);
}; 
struct ParadoxComplicateTag : public ParadoxBase{
	ParadoxBase* lVal;
	ParadoxBase* rVal;
	char op;
	virtual void* getContent(){
		return nullptr;
	}
	virtual ParadoxType getType(){
		return ParadoxType::COMP_TAG;
	}
};

std::string stripTag(std::string original);
bool startWith(std::string from,std::string to);
bool isCastable(ParadoxBase* base,ParadoxType type);
bool castToBool(ParadoxString* base);
bool Xor(bool a,bool b);
namespace pdx{
	ParadoxString* createString(std::string str);
}

#endif
