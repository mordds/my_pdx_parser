#ifndef PDX_TRIGGER
#define PDX_TRIGGER

#include<iostream>
#include<map>
#include "paradox_type.h"
#include "scope.h"

enum TriggerType{
	COMMON,LOGIC,CHANGE_SCOPE,CONDITIONAL,NUM,HIDDEN,CUSTOM_TT
};
enum LogicType{
	AND,OR,NOT
};
const int SINGLE_SCOPE_MERGABLE = 0x1;
struct ComplexTrigger;
struct LogicTrigger;
struct TriggerItem{
	std::string pattern;
	std::string reversePattern;
	std::map<std::string,int> parameterName;
	std::vector<ParadoxType> parameterType;
	std::vector<int> usedParameter;
	std::string toString(std::vector<ParadoxBase*> base,bool reversed);
	long long attribute;
	TriggerItem(){}
	TriggerItem(std::pair<std::string,std::string>&& patterns,std::vector<std::string>&& parameterName,std::vector<ParadoxType>&& parameterType,std::vector<int>&& usedParameter);
};

struct Trigger{
	virtual TriggerType getType() = 0;
	virtual std::string toString(bool reversed) = 0;
	ComplexTrigger* getAsComplexTrigger();
	LogicTrigger* getAsLogicTrigger();
	int depth;
};
struct ComplexTrigger : Trigger{
	std::vector<Trigger*> subTriggers;
	bool ignored;
	bool omitted;
	void putTrigger(Trigger* trigger);
};
struct CommonTrigger : Trigger{
	virtual TriggerType getType(){
		return TriggerType::COMMON;
	}
	virtual std::string toString(bool reversed);
	CommonTrigger(TriggerItem* item);
	void pushObject(ParadoxBase* base);
	TriggerItem* item;
	bool reversed;
	std::vector<ParadoxBase*> base;
};

struct LogicTrigger : ComplexTrigger {
	virtual TriggerType getType(){
		return TriggerType::LOGIC;
	}
	LogicTrigger(LogicType type);
	virtual std::string toString(bool reversed);
	LogicType type;
};
struct ChangeScopeTrigger : ComplexTrigger{
	virtual TriggerType getType(){
		return TriggerType::CHANGE_SCOPE;
	}
	ChangeScopeTrigger(Scope* scope);
	virtual std::string toString(bool reversed);
	Scope* changedScope;
	std::vector<Trigger*> condition;
	bool trigger_type;
	bool use_type;
};
struct ConditionalTrigger : ComplexTrigger{
	virtual TriggerType getType(){
		return TriggerType::CONDITIONAL;
	}
	virtual std::string toString(bool reversed);
	std::vector<Trigger*> condition;
	void putCondition(Trigger* trigger);
};
//for calc_true_if
struct NumberRequiredTrigger : ComplexTrigger{
	virtual TriggerType getType(){
		return NUM;
	}
	virtual std::string toString(bool reversed);
	int amount;
	TriggerItem* item;
};
struct CustomTooltipTrigger : ComplexTrigger{
	virtual TriggerType getType(){
		return CUSTOM_TT;
	}
	virtual std::string toString(bool reversed);
	std::string tooltip;
	bool show_origin;
};

struct HiddenTrigger : ComplexTrigger{
	virtual TriggerType getType(){
		return HIDDEN;
	}
	virtual std::string toString(bool reversed);
	bool hidden_current;
};

std::vector<Trigger*> parseTriggerList(ParadoxTag*,int root_depth);
void registerItems();
ComplexTrigger* createBaseTrigger(); 
void parseTrigger(ParadoxTag*,ComplexTrigger* trigger);

#endif
