#ifndef PDX_SCOPE
#define PDX_SCOPE
#include<string>
#include<map>
#include<set>
#include<cstdint>
#include "utils/string_util.h"
enum class ScopeType : uint8_t{
	PROVINCE = 1,
	COUNTRY = 2,
	UNIT = 3,
	ANY = 4
};
struct ProvinceScope;
struct CountryScope;
struct AnyScope;
struct UnitScope;
struct Scope{
	virtual std::string toString() = 0;
	virtual ScopeType getType() const = 0;
	virtual bool isMultiScope() const = 0;
	virtual std::string toHtml(){
		return toString();
	}
	
	ProvinceScope* getAsProvinceScope();
	CountryScope* getAsCountryScope();
	AnyScope* getAsAnyScope();
	UnitScope* getAsUnitScope();
};
struct ProvinceScope : Scope {
	virtual std::string toString();
	virtual std::string toHtml();
	virtual bool isMultiScope() const { return false; }
	virtual ScopeType getType() const{
		return ScopeType::PROVINCE;
	}
	int getId();
	ProvinceScope(int id);
	private:
	int id;
};
struct UnitScope : Scope {
	virtual std::string toString();
	virtual bool isMultiScope() const { return false; };
	virtual ScopeType getType() const {
		return ScopeType::UNIT;
	}
	static void registerLocalizeText(std::string key,std::string value);
	static std::map<std::string,std::string> localizeMap;
	UnitScope(std::string data) : data(data) {};
	private:
		std::string data;
};
struct CountryScope : Scope{
	virtual std::string toString();
	virtual std::string toHtml();
	virtual bool isMultiScope() const { return false; };
	virtual ScopeType getType() const{
		return ScopeType::COUNTRY;
	}
	CountryScope(std::string str);
	std::string getTag() {
		return tag;
	}
	private:
	char tag[4]; 
};
struct MultiCountryScope : Scope {
	virtual std::string toString();
	virtual std::string toHtml(){ return toString(); }
	virtual bool isMultiScope() const { return true; };
	virtual ScopeType getType() const {
		return ScopeType::COUNTRY;
	}
	MultiCountryScope(std::string name) : data(name){};
	std::string data;
	static void registerMultiCountryScope(std::string name,std::string value);	
	static std::map<std::string,std::string> localizeMap;
};
//

struct MultiProvinceScope : Scope {
	virtual std::string toString();
	virtual std::string toHtml(){ return toString(); }
	virtual bool isMultiScope() const { return true; };
	virtual ScopeType getType() const {
		return ScopeType::COUNTRY;
	}
	MultiProvinceScope(std::string name) : data(name){};
	std::string data;
	static void registerMultiProvinceScope(std::string name,std::string value);	
	static std::map<std::string,std::string> localizeMap;
};



struct AnyScope : Scope{
	virtual std::string toString();
	virtual ScopeType getType() const{
		return ScopeType::ANY;
	}
	virtual bool isMultiScope() const { return false; };
	AnyScope(std::string str);
	std::string data;
	static void registerLocalizeText(std::string key,std::string value);
	static std::map<std::string,std::string> localizeMap;
};
Scope* createScopeFromString(std::string str);
ProvinceScope* getProvinceScope(int id);

void registerInternalScopes();
Scope* findScopeByName(std::string name,ScopeType type);
#endif
