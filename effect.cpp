#include "effect.h"
#include "trigger.h"
#include "localization.h"
#include "pattern.h"
#include "utils/functional_util.h"
#include "paradox_type.h"
#include "paradox_macro.h"
#include <array>
#include <string_view>
#include <iostream>
#include <source_location>


using OverrideHandler = bool(*)(std::map<std::string,ParadoxBase*>&);

extern std::map<const std::string*,ScriptedEffect*> loadedSEs;

std::map<const std::string*,EffectItem*> effectItems;
std::set<const std::string*> simpleEffectItems;
std::map<const EffectItem*,OverrideHandler> effectHandlers;

inline void preInit(std::string& str,int depth){
    for(int i = 0;i < depth;i++) str.push_back(' ');
}

bool isSimpleItem(std::string key){
    return simpleEffectItems.find(getStringPtr(key)) != simpleEffectItems.end();
}
EffectItem* getEffectItem(std::string key){
    return effectItems[getStringPtr(key)];
}   
template<ParadoxType types>
void registerSingleArgEffect(std::string name,ScopeType usable_scope, std::function<std::string(rawType<types>)> localize){
    const std::string* name_ptr = getStringPtr(name.append(std::to_string(static_cast<int>(types))));
    EffectItem* u = new NativeEffectItem<rawType<types>>(*name_ptr,usable_scope,localize);
    
    effectItems[name_ptr] = u;
}

template<ParadoxType... types>
void registerSimpleEffect(std::string name,ScopeType usable_scope, std::function<std::string(rawType<types>...)> localize){
    const std::string* name_ptr = getStringPtr(name);
    EffectItem* u = new NativeEffectItem<rawType<types>...>(*name_ptr,usable_scope,localize);
    simpleEffectItems.insert(name_ptr);
    effectItems[name_ptr] = u;
}

template<ParadoxType...types>
void regiserSimpleClausedEffect(std::string name,ScopeType usable_scope,std::function<std::string(rawType<types>...)> localize,std::array<std::string,sizeof...(types)> names){
    const std::string* name_ptr = getStringPtr(name);
    if(!localize) log_error(current_location(),"An empty localize function was provided for effect ",name);
    EffectItem* u = new ParameteredNativeEffectItem<rawType<types>...>(*name_ptr,usable_scope,localize,names);
    simpleEffectItems.insert(name_ptr);
    effectItems[name_ptr] = u;
}

template<ParadoxType...types>
void regiserArrayEffect(std::string name,ScopeType usable_scope,std::function<std::string(rawType<types>...)> localize,std::array<std::string,sizeof...(types)> names){
    name.append("_array");
    const std::string* name_ptr = getStringPtr(name);
    EffectItem* u = new NativeEffectItem<rawType<types>...>(*name_ptr,usable_scope,localize,names);
    simpleEffectItems.insert(name_ptr);
    effectItems[name_ptr] = u;
}

template<ParadoxType...types>
void regiserClausedEffect(std::string name,ScopeType usable_scope,std::function<std::string(rawType<types>...)> localize,std::array<std::string,sizeof...(types)> names,OverrideHandler handler){
    const std::string* name_ptr = getStringPtr(name);
    const EffectItem* u = new ParameteredNativeEffectItem<rawType<types>...>(*name_ptr,usable_scope,localize,names);
    simpleEffectItems.insert(name_ptr);
    effectItems[name_ptr] = u;    
    effectHandlers[u] = handler;
}//

void registerEffectItems(){
    using namespace std::literals;
    registerSimpleEffect<ParadoxType::INTEGER>("add_treasury"s,ScopeType::COUNTRY,
        signedPattern<0,ParadoxType::INTEGER>("获得%d克朗"s,"失去-%d克朗"s));
    regiserSimpleClausedEffect<ParadoxType::SCOPE,ParadoxType::INTEGER>("add_trust"s,ScopeType::COUNTRY,
        signedOrderPattern<1,ParadoxType::SCOPE,ParadoxType::INTEGER>("对%s的信任提高了%d"s,"对%s的信任降低了-%d"s),{"who","value"});
    registerSimpleEffect<ParadoxType::STRING>("set_global_flag"s,ScopeType::ANY,
        orderedPattern<ParadoxType::STRING>("设置全局标签'%s'"s));
    registerSimpleEffect<ParadoxType::STRING>("clr_global_flag"s,ScopeType::ANY,
        orderedPattern<ParadoxType::STRING>("清除全局标签'%s'"s));
    registerSimpleEffect<ParadoxType::STRING>("custom_tooltip"s,ScopeType::ANY,
    [](std::string str){return getLocalization(str);});
    registerSimpleEffect<ParadoxType::STRING>("save_event_target_as",ScopeType::ANY,
        orderedPattern<ParadoxType::STRING>("将当前作用域保存为事件目标%s"s));
    registerSimpleEffect<ParadoxType::STRING>("save_global_event_target_as",ScopeType::ANY,
        orderedPattern<ParadoxType::STRING>("将当前作用域保存为全局事件目标%s"s));
    registerSimpleEffect<ParadoxType::STRING>("clear_global_event_target",ScopeType::ANY,
        orderedPattern<ParadoxType::STRING>("清除全局事件目标%s"s));
    registerSimpleEffect<ParadoxType::BOOLEAN>("clear_global_event_target",ScopeType::ANY,
        [](bool b){return "清除所有全局事件目标";});    
}

std::string ChangeScopeEffect::toString(int depth){
    std::string str("");
    if(this->target != nullptr){
        preInit(str,depth);
        if(this->subEffects.empty()) return str;
        str.append(this->target->toString());
        str.append(":\n");
        if(this->condition != nullptr){
            preInit(str,depth + 1);
            str.append("若满足以下条件:");
            str.append(this->condition->toString(false,depth + 2));
            preInit(str,depth + 1);
            str.append("则:\n");
        }
    }
    for(Effect* effect : this->subEffects){
        str.append(effect->toString(depth + 2));
    }
    return str;
}

std::string HiddenEffect::toString(int depth){
    if(this->isHidden()) return "";
    else{
        std::string str("");
        preInit(str,depth);
        str.append("隐藏效果:\n");
        for(Effect* effect : this->subEffects){
            str.append(effect->toString(depth + 1));
        } 
        return str;
    }
}

std::string ConditionalEffect::toString(int depth){
    std::string str("");
    if(this->condition == nullptr && !this->isElse()){
        log_warning(current_location(),"unexcepted nullptr trigger in ConditionalEffect.");
        depth--;
    }
    else if(this->isElse()){
        preInit(str,depth);
        str.append("否则:\n");
    }
    else if(this->isElseIf()){
        preInit(str,depth);
        str.append("否则若满足以下条件:\n");
        preInit(str,depth + 1);
        str.append(this->condition->toString(false));
        preInit(str,depth);
        str.append("则:\n");
    }
    else {
        preInit(str,depth);
        str.append("若满足以下条件:\n");
        preInit(str,depth + 1);
        str.append(this->condition->toString(false));
        preInit(str,depth);
        str.append("则:\n");
    }

    for(Effect* effect : this->subEffects){
        str.append(effect->toString(depth + 1));
    }
    return str;
}
std::string RandomEffect::toString(int depth){
    Pattern p("%p%%概率发生以下效果:\n");
    p.setNextInteger(this->getChance());
    std::string str("");
    preInit(str,depth);
    str.append(p.getOutput());
    for(Effect* effect : this->subEffects){
        str.append(effect->toString(depth + 1));
    }
    return str;
}

std::string SpecialEffect::toString(int depth){
    if(this->instance == nullptr && this->prototype->isFixed()){
 		std::map<std::string,ParadoxBase*> data;
		this->instance = this->prototype->createInstance(data);       
    }
    return this->instance->toString(depth);
}

std::unique_ptr<ComplexEffect> createBaseEffect(){
    auto ptr = std::unique_ptr<ComplexEffect>(static_cast<ComplexEffect*>(new ChangeScopeEffect(nullptr)));
    return ptr;
}
//Final Part of the Paradox Parser Base!
void parseEffect(ParadoxTag* root,ComplexEffect* from){
    for(int i = 0;i < root->size();i++){
        std::string name = stripTag(root->seq[i]);
        ParadoxBase* base = root->get(i);
        ParadoxTag* tag = base->getAsTag();
        
        if(tag != nullptr){
            if(name == "if"){
                if(ParadoxTag* subTag = tag->getAsTag("limit");subTag != nullptr){
                    ConditionalEffect* effect = new ConditionalEffect();
                    from->addEffect(effect);
                    ComplexTrigger* trigger = createBaseTrigger();
                    trigger->depth = 0;
                    parseTrigger(subTag,trigger);
                    tag->remove("limit",0);
                    effect->condition = trigger;
                    
                    parseEffect(tag,effect);
                }
                else {
                    
                    log_error(current_location(),"cannot create a Conditional Effect without \"limit\" block.");

                }
            }
            else if(name == "else_if"){
                if(from->subEffects.empty()) continue;
                if (Effect* back = from->subEffects.back();back->getType() == EffectType::CONDITIONAL && back->extra_data[0] <= 1){
                    if(ParadoxTag* subTag = tag->getAsTag("limit");subTag != nullptr){
                        ConditionalEffect* effect = new ConditionalEffect();
                        from->addEffect(effect);
                        ComplexTrigger* trigger = createBaseTrigger();
                        trigger->depth = 0;
                        parseTrigger(subTag,trigger);
                        tag->remove("limit",0);
                        effect->condition = trigger;
                        effect->setElseIfState();
                        parseEffect(tag,effect);
                    }
                    else {
                        log_error(current_location(),"cannot create a Conditional Effect without \"limit\" block.");
                    }
                }
                else log_error(current_location(),"cannot create \"else_if\" block before a \"if\" block.");
            }
            else if(name == "else"){
                if(from->subEffects.empty()) continue;
                if (Effect* back = from->subEffects.back();back->getType() == EffectType::CONDITIONAL && back->extra_data[0] <= 1){
                        ConditionalEffect* effect = new ConditionalEffect();
                        from->addEffect(effect);
                        effect->setElseState();
                        parseEffect(tag,effect);
                }
                else{
                    auto location = current_location();
                    log_error(current_location(),"cannot create \"else\" block before a \"if\" block.");
                    log_error(location,"Context:");
                    for(auto k : tag->seq){
                        auto v = tag->tags[k];
                        log_error(location,k,":",v->toString());
                    }
                    log_error(location,"effects:");
                    for(auto k : from->subEffects){
                        log_error(location,"effect:",(int)k->getType()," ",(int)k->extra_data[0]);
                    }
                } 
            }
            else if(name == "hidden_effect"){
                HiddenEffect* effect = new HiddenEffect();
                from->addEffect(effect);
                parseEffect(tag,effect);
            }
            else if(name == "random"){
                ParadoxBase* base = tag->get("chance");
                if(base == nullptr) log_error(current_location(),"cannot create \"random\" block without \"chance\"");
                if(ParadoxInteger* chance = base->getAsInteger();chance != nullptr){
                    RandomEffect* effect = new RandomEffect();
                    effect->setChance(chance->getIntegerContent());
                    tag->remove("chance",0);
                    from->addEffect(from);
                    parseEffect(tag,effect);
                }
                else {
                    log_error(current_location(),"cannot create \"random\" block with a non-number \"chance\"");
                }
            }
            else if(Scope* scope = createScopeFromString(name);scope != nullptr){
                ParadoxTag* subTag = tag->getAsTag("limit");
                
                if(subTag != nullptr && !scope->isMultiScope()) log_error(current_location(),"\"limit\" block for single scope is Invalid.");
                else {
                    ChangeScopeEffect* effect = nullptr;
                    if(subTag != nullptr){
                        ComplexTrigger* trigger = createBaseTrigger();
                        parseTrigger(subTag,trigger);
                        effect = new ChangeScopeEffect(scope,trigger);   
                        from->addEffect(effect);
                        tag->remove("limit",0);
                        parseEffect(tag,effect);
                    }
                    else {
                        effect = new ChangeScopeEffect(scope);
                        from->addEffect(effect);
                        parseEffect(tag,effect);
                    }
                }
            }
            else {
                auto ptr = getStringPtr(name);
                if(loadedSEs.contains(ptr)){
                    SpecialEffect* se = new SpecialEffect();
                    se->prototype = loadedSEs[ptr];
                    se->instance = se->prototype->createInstance(tag->tags);       
                    if(se->instance == nullptr){
                        delete se;
                        continue;
                    }
                    for(auto[k,v] : tag->tags){
                        se->items[k] = deep_copy(v);
                    }
                    from->addEffect(se);
                }
                else {
                    EffectItem* item = effectItems[ptr];
                    if(item == nullptr) continue;
                    if(effectHandlers.contains(item)){
                        bool success = effectHandlers[item](tag->tags);
                        if(!success) continue;
                    }
                    Effect* effect = item->createInstance(tag->tags).release();
                    if(effect != nullptr) from->addEffect(effect);
                }

            }
        }
        else if(base->getType() == ParadoxType::ARRAY){
            //arrays should not be 1 args...
            name.append("_array");
            EffectItem* item = getEffectItem(name);
            if(item == nullptr) continue;
            Effect* effect = item->createInstance(base->getAsArray()->contents).release();
            if(effect != nullptr) from->addEffect(effect);
        }
        else {
            auto ptr = getStringPtr(name);
            if(loadedSEs.contains(ptr)){
                SpecialEffect* se = new SpecialEffect();
                se->prototype = loadedSEs[ptr];
                std::map<std::string,ParadoxBase*> placeholder;
                se->instance = se->prototype->createInstance(placeholder);
            }
            else if(simpleEffectItems.contains(ptr)){
                EffectItem* item = effectItems[ptr];
                Effect* effect = item->createInstance(std::vector<ParadoxBase*>{base}).release();
                if(effect != nullptr) from->addEffect(effect);
            }
            else{
                if(base->getType() == ParadoxType::INTEGER){
                    if(EffectItem* item = getEffectItem(name + std::to_string(static_cast<uint8_t>(ParadoxType::INTEGER)));item != nullptr){
                        Effect* effect = item->createInstance(std::vector<ParadoxBase*>{base}).release();
                        if(effect != nullptr) from->addEffect(effect);    
                    }
                    else if(EffectItem* item = getEffectItem(name + std::to_string(static_cast<uint8_t>(ParadoxType::SCOPE)));item != nullptr){
                        Effect* effect = item->createInstance(std::vector<ParadoxBase*>{castTo(base,ParadoxType::SCOPE)}).release();
                        if(effect != nullptr) from->addEffect(effect);    
                    }
                }
                else if(base->getType() == ParadoxType::STRING){
                    if(EffectItem* item = getEffectItem(name + std::to_string(static_cast<uint8_t>(ParadoxType::SCOPE)));item != nullptr){
                        Effect* effect = item->createInstance(std::vector<ParadoxBase*>{castTo(base,ParadoxType::SCOPE)}).release();
                        if(effect != nullptr) from->addEffect(effect);    
                    }
                    else if(EffectItem* item = getEffectItem(name + std::to_string(static_cast<uint8_t>(ParadoxType::STRING)));item != nullptr){
                        Effect* effect = item->createInstance(std::vector<ParadoxBase*>{base}).release();
                        if(effect != nullptr) from->addEffect(effect);    
                    }
                }
                else if(EffectItem* item = getEffectItem(name + std::to_string(static_cast<uint8_t>(base->getType())));item != nullptr){
                    Effect* effect = item->createInstance(std::vector<ParadoxBase*>{base}).release();
                    if(effect != nullptr) from->addEffect(effect);    
                }
            }

        }
    }
}