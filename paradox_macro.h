#ifndef PDX_MARCO
#define PDX_MARCO

#include "trigger.h"
#include "paradox_type.h"
#include <memory>




struct ScriptedTrigger;

struct ScriptedTriggerItem : TriggerItem {
    ScriptedTrigger* st;
};

enum class HolderType{
    CONDITIONAL,
    STRING,
    PARAMETER,
};

struct MarcoHolder {
    virtual HolderType getType();
};

struct ConditionalHolder : MarcoHolder{
    virtual HolderType getType(){
        return HolderType::CONDITIONAL;
    }
    int parameterIndex;
    std::vector<std::unique_ptr<MarcoHolder>> subHolders;
};

struct StringHolder : MarcoHolder{
    virtual HolderType getType(){
        return HolderType::STRING;
    }
    std::string data;
    StringHolder(const std::string& _data) : data(_data){}
};

struct ParameterHolder : MarcoHolder{
    virtual HolderType getType(){
        return HolderType::PARAMETER;
    }
    int parameterIndex;
};


//
struct ScriptedTrigger {
    virtual Trigger* createInstance(std::map<std::string,ParadoxBase*>) = 0;
    virtual bool isFixed() const = 0;
    size_t suffix_index = -1;
    std::string name;
    std::string getLocalizationPattern(bool reversed);
    virtual ~ScriptedTrigger() noexcept = default;
};

struct FixedScriptedTrigger : ScriptedTrigger {
    Trigger* instance;
    virtual Trigger* createInstance(std::map<std::string,ParadoxBase*>);
    virtual bool isFixed() const{ return true; }
};

struct ComplicateScriptedTrigger : ScriptedTrigger {
    virtual Trigger* createInstance(std::map<std::string,ParadoxBase*>);
    std::string pattern;
    std::vector<std::string> parameterName;
    std::vector<std::unique_ptr<MarcoHolder>> marcoHolders;
    virtual bool isFixed() const { return false; }
};

void loadScriptedTrigger(std::string rootPath = ".");
void printAllScriptedTrigger();
void loadScriptedTrigger_POST();

#endif