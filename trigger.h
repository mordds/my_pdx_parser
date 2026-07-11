#ifndef PDX_TRIGGER
#define PDX_TRIGGER

#include<iostream>
#include<map>
#include "paradox_type.h"
#include "scope.h"
#include <functional>

enum class TriggerType{
	COMMON,LOGIC,CHANGE_SCOPE,CONDITIONAL,NUM,HIDDEN,CUSTOM_TT,SPECIAL
};
enum class LogicType{
	AND,OR,NOT
};
const int SINGLE_SCOPE_MERGABLE = 0x1;
struct ComplexTrigger;
struct LogicTrigger;
struct CommonTrigger;
struct ScriptedTrigger;

struct TriggerItem{
	std::string pattern;
	std::string reversePattern;
	const std::string& name;
	std::map<std::string,int> parameterName;
	std::vector<ParadoxType> parameterType;
	std::vector<int> usedParameter;
	ScopeType usable_scope;
	int attribue;
	std::string toString(std::vector<ParadoxBase*> base,bool reversed);
	std::string toHtml(std::vector<ParadoxBase*> base,bool reversed);
	TriggerItem(const std::string& _name) : name(_name){}
	TriggerItem(const std::string& _name,std::pair<std::string,std::string>&& patterns,std::vector<std::string>&& parameterName,std::vector<ParadoxType>&& parameterType,std::vector<int>&& usedParameter,ScopeType scope_type = ScopeType::COUNTRY);
};


struct Trigger{
	virtual TriggerType getType() = 0;
	virtual std::string toString(bool reversed,int depth = 1) = 0;
	virtual std::string toHtml(bool reversed,int depth = 1) = 0;
	virtual void takeOverLifeCycle() = 0;
	virtual bool hasAnyTrigger(bool (*predicate)(Trigger* trigger)) = 0;
	virtual bool foreach(std::function<bool(Trigger*)>) = 0;
	ComplexTrigger* getAsComplexTrigger();
	LogicTrigger* getAsLogicTrigger();
	CommonTrigger* getAsCommonTrigger();
	int depth;
	bool copied;
};
struct ComplexTrigger : Trigger{
	virtual void takeOverLifeCycle();
	virtual bool hasAnyTrigger(bool (*predicate)(Trigger* trigger));
	virtual bool foreach(std::function<bool(Trigger*)>);
	std::vector<Trigger*> subTriggers;
	bool ignored;
	bool omitted;
	~ComplexTrigger(){
		for(Trigger* trigger : subTriggers){
			delete trigger;
		}
	}
	void putTrigger(Trigger* trigger);

};
struct CommonTrigger : Trigger{
	virtual TriggerType getType(){
		return TriggerType::COMMON;
	}
	virtual std::string toString(bool reversed,int depth = 1);
	virtual std::string toHtml(bool reversed,int depth = 1);
	virtual bool foreach(std::function<bool(Trigger*)>);	
	virtual void takeOverLifeCycle();
	virtual bool hasAnyTrigger(bool (*predicate)(Trigger* trigger));
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

//sizeof(SpecialTrigger) = 56 
struct SpecialTrigger : Trigger {
	virtual TriggerType getType(){
		return TriggerType::SPECIAL;
	}
	const ScriptedTrigger * const st;
	const Trigger* instance;
	const std::vector<std::string> locKey;
};

struct LogicTrigger : ComplexTrigger {
	virtual TriggerType getType(){
		return TriggerType::LOGIC;
	}
	virtual std::string toHtml(bool reversed,int depth = 1){
		return this->toString(reversed);
	}
	LogicTrigger(LogicType type);
	virtual std::string toString(bool reversed,int depth = 1);
	LogicType type;
};
struct ChangeScopeTrigger : ComplexTrigger{
	virtual TriggerType getType(){
		return TriggerType::CHANGE_SCOPE;
	}
	virtual std::string toHtml(bool reversed,int depth = 1){
		return this->toString(reversed);
	}
	ChangeScopeTrigger(Scope* scope);
	virtual std::string toString(bool reversed,int depth = 1);
	Scope* changedScope;
	bool trigger_type;
	bool use_type;
};
struct ConditionalTrigger : ComplexTrigger{
	virtual TriggerType getType(){
		return TriggerType::CONDITIONAL;
	}
	virtual std::string toHtml(bool reversed,int depth = 1){
		return this->toString(reversed);
	}
	virtual std::string toString(bool reversed,int depth = 1);
	ComplexTrigger* condition;
	bool isElseTrigger;
	void putCondition(Trigger* trigger);
};
//for calc_true_if
struct NumberRequiredTrigger : ComplexTrigger{
	virtual TriggerType getType(){
		return TriggerType::NUM;
	}
	virtual std::string toHtml(bool reversed,int depth = 1){
		return this->toString(reversed,depth);
	}
	virtual std::string toString(bool reversed,int depth = 1);
	int amount;
	TriggerItem* item;
};
struct CustomTooltipTrigger : ComplexTrigger{
	virtual TriggerType getType(){
		return TriggerType::CUSTOM_TT;
	}
	virtual std::string toHtml(bool reversed,int depth = 1){
		return this->toString(reversed,depth);
	}
	virtual std::string toString(bool reversed,int depth = 1);
	std::string tooltip;
	bool show_origin;
};

struct HiddenTrigger : ComplexTrigger{
	virtual TriggerType getType(){
		return TriggerType::HIDDEN;
	}
	virtual std::string toHtml(bool reversed,int depth = 1){
		return this->toString(reversed,depth);
	}
	virtual std::string toString(bool reversed,int depth = 1);
	bool hidden_current;
};

void registerTriggerItems();
ComplexTrigger* createBaseTrigger(); 
void parseTrigger(ParadoxTag*,ComplexTrigger* trigger);

#endif
