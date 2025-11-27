#ifndef PDX_SCOPE
#define PDX_SCOPE
#include<string>
#include<map>
#include<set>
enum ScopeType{
	PROVINCE = 1,
	COUNTRY = 2,
	SPECIAL = 3
};
struct ProvinceScope;
struct CountryScope;
struct CustomScope;
struct Scope{
	virtual std::string toString() = 0;
	virtual ScopeType getType() = 0;
	ProvinceScope* getAsProvinceScope();
	CountryScope* getAsCountryScope();
	CustomScope* getAsCustomScope();
};
struct ProvinceScope : Scope{
	virtual std::string toString();
	virtual ScopeType getType(){
		return ScopeType::PROVINCE;
	}
	ProvinceScope(int id);
	private:
	int id;
};
struct CountryScope : Scope{
	virtual std::string toString();
	virtual ScopeType getType(){
		return ScopeType::COUNTRY;
	}
	CountryScope(std::string str);
	private:
	char tag[4]; 
};
struct CustomScope : Scope{
	virtual std::string toString();
	virtual ScopeType getType(){
		return ScopeType::SPECIAL;
	}
	CustomScope(std::string str);
	std::string data;
	static void registerLocalizeText(std::string key,std::string value);
	static std::map<std::string,std::string> localizeMap;
};
Scope* createScopeFromString(std::string str);
#endif
