#ifndef PDX_TRIGGER
#define PDX_TRIGGER

#include<iostream>
#include<map>
#include "paradox_type.h"
#include "scope.h"

enum class TriggerType{
	COMMON,LOGIC,CHANGE_SCOPE,CONDITIONAL,NUM,HIDDEN,CUSTOM_TT
};
enum class LogicType{
	AND,OR,NOT
};
const int SINGLE_SCOPE_MERGABLE = 0x1;
struct ComplexTrigger;
struct LogicTrigger;
struct CommonTrigger;
struct TriggerItem{
	std::string pattern;
	std::string reversePattern;
	std::string name;
	std::map<std::string,int> parameterName;
	std::vector<ParadoxType> parameterType;
	std::vector<int> usedParameter;
	long long attribue;
	std::string toString(std::vector<ParadoxBase*> base,bool reversed);
	std::string toHtml(std::vector<ParadoxBase*> base,bool reversed);
	TriggerItem(){}
	TriggerItem(std::pair<std::string,std::string>&& patterns,std::vector<std::string>&& parameterName,std::vector<ParadoxType>&& parameterType,std::vector<int>&& usedParameter);
};

struct Trigger{
	virtual TriggerType getType() = 0;
	virtual std::string toString(bool reversed) = 0;
	virtual std::string toHtml(bool reversed) = 0;
	virtual void takeOverLifeCycle() = 0;
	ComplexTrigger* getAsComplexTrigger();
	LogicTrigger* getAsLogicTrigger();
	CommonTrigger* getAsCommonTrigger();
	int depth;
	bool copied;
};
struct ComplexTrigger : Trigger{
	std::vector<Trigger*> subTriggers;
	bool ignored;
	bool omitted;

	~ComplexTrigger(){
		for(Trigger* trigger : subTriggers){
			delete trigger;
		}
	}
	void putTrigger(Trigger* trigger);
	virtual void takeOverLifeCycle();
};
struct CommonTrigger : Trigger{
	virtual TriggerType getType(){
		return TriggerType::COMMON;
	}
	virtual std::string toString(bool reversed);
	virtual std::string toHtml(bool reversed);
	virtual void takeOverLifeCycle();
	CommonTrigger(TriggerItem* item);
	void pushObject(ParadoxBase* base);
	TriggerItem* item;
	bool reversed;
	std::vector<ParadoxBase*> base;
	~CommonTrigger(){
		if(this->copied){
			for(ParadoxBase* base1 : base) delete base1;
		}
	}
};

struct LogicTrigger : ComplexTrigger {
	virtual TriggerType getType(){
		return TriggerType::LOGIC;
	}
	virtual std::string toHtml(bool reversed){
		return this->toString(reversed);
	}
	LogicTrigger(LogicType type);
	virtual std::string toString(bool reversed);
	LogicType type;
};
struct ChangeScopeTrigger : ComplexTrigger{
	virtual TriggerType getType(){
		return TriggerType::CHANGE_SCOPE;
	}
	virtual std::string toHtml(bool reversed){
		return this->toString(reversed);
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
	virtual std::string toHtml(bool reversed){
		return this->toString(reversed);
	}
	virtual std::string toString(bool reversed);
	std::vector<Trigger*> condition;
	bool isElseTrigger;
	void putCondition(Trigger* trigger);
};
//for calc_true_if
struct NumberRequiredTrigger : ComplexTrigger{
	virtual TriggerType getType(){
		return TriggerType::NUM;
	}
	virtual std::string toHtml(bool reversed){
		return this->toString(reversed);
	}
	virtual std::string toString(bool reversed);
	int amount;
	TriggerItem* item;
};
struct CustomTooltipTrigger : ComplexTrigger{
	virtual TriggerType getType(){
		return TriggerType::CUSTOM_TT;
	}
	virtual std::string toHtml(bool reversed){
		return this->toString(reversed);
	}
	virtual std::string toString(bool reversed);
	std::string tooltip;
	bool show_origin;
};

struct HiddenTrigger : ComplexTrigger{
	virtual TriggerType getType(){
		return TriggerType::HIDDEN;
	}
	virtual std::string toHtml(bool reversed){
		return this->toString(reversed);
	}
	virtual std::string toString(bool reversed);
	bool hidden_current;
};

std::vector<Trigger*> parseTriggerList(ParadoxTag*,int root_depth);
void registerTriggerItems();
ComplexTrigger* createBaseTrigger(); 
void parseTrigger(ParadoxTag*,ComplexTrigger* trigger);

#endif
