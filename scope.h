#ifndef PDX_SCOPE
#define PDX_SCOPE
#include<string>
#include<map>
#include<set>
enum ScopeType{
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
	virtual std::string toHtml(){
		return toString();
	}
	ProvinceScope* getAsProvinceScope();
	CountryScope* getAsCountryScope();
	AnyScope* getAsAnyScope();
	UnitScope* getAsUnitScope();
};
struct ProvinceScope : Scope{
	virtual std::string toString();
	virtual std::string toHtml();
	virtual ScopeType getType() const{
		return ScopeType::PROVINCE;
	}
	ProvinceScope(int id);
	private:
	int id;
};
struct UnitScope : Scope {
	virtual std::string toString();
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
struct AnyScope : Scope{
	virtual std::string toString();
	virtual ScopeType getType() const{
		return ScopeType::ANY;
	}
	AnyScope(std::string str);
	std::string data;
	static void registerLocalizeText(std::string key,std::string value);
	static std::map<std::string,std::string> localizeMap;
};
Scope* createScopeFromString(std::string str);
#endif
