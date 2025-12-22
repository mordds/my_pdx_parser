#include "national_idea.h"
#include "localization.h"
#include "utils/parser_util.h"
#include "paradox_type.h"
#include <map>


std::map<std::string,NationalIdea*> nationalIdeas;
std::map<std::string,NationalIdea*> tagIdeas;


void loadNationalIdea(){
    ParadoxTag* tag = parseFile("./datas/country_ideas.txt");
    for(std::string str : tag->seq){
        ParadoxTag* ideaTag = tag->getAsTag(str);
        if(ideaTag->get("free") == nullptr) continue;
        NationalIdea* idea = new NationalIdea();
        idea->start = std::make_shared<Modifier>();
        idea->bonus = std::make_shared<Modifier>();
        for(int i = 0;i < 7;i++){
            idea->modifiers[i] = std::make_shared<Modifier>();
        }
        idea->trigger = nullptr;
        int slot = 0;
        for(std::string entry : ideaTag->seq){
            if(entry == "start"){
                idea->start->name = "传统";
                ParseModifier(ideaTag->getAsTag(entry),*(idea->start.get()));
            }
            else if(entry == "bonus"){
                idea->bonus->name = "野心";
                ParseModifier(ideaTag->getAsTag(entry),*(idea->bonus.get()));
            }
            else if(entry == "free") continue;
            else if(entry == "trigger"){
                idea->trigger = std::shared_ptr<Trigger>((Trigger*)createBaseTrigger());
                parseTrigger(ideaTag->getAsTag(entry),idea->trigger->getAsComplexTrigger());
                idea->trigger->takeOverLifeCycle();
            }
            else{
                idea->modifiers[slot]->name = getLocalization(entry);
                ParseModifier(ideaTag->getAsTag(entry),*(idea->modifiers[slot]));
                slot++;
            }
        }
        nationalIdeas[str] = idea;
        if(idea->trigger != nullptr && idea->trigger->getAsComplexTrigger()->subTriggers.size() == 1){
            CommonTrigger* trigger = idea->trigger->getAsComplexTrigger()->subTriggers[0]->getAsCommonTrigger();
            if(trigger == nullptr) continue;
            if(trigger->item->name == "tag"){
                std::string tagName = trigger->base[0]->getAsString()->getStringContent();
                tagIdeas[tagName] = idea;
            }
        }
    }
    clearParserDatas();
}

std::string NationalIdea::toString(){
    std::string ret("");
    if(this->trigger != nullptr){
        ret.append("启用条件:\n");
        ret.append(this->trigger->toString(false));
    }
    ret.append(this->start->localize());
    for(int i = 0;i < 7;i++){
        ret.append(this->modifiers[i]->localize());
    }
    ret.append(this->bonus->localize());
    return ret;
}
std::string NationalIdea::toHtml(){
    std::string ret("");
    if(this->trigger != nullptr){
        ret.append("启用条件:\n");
        ret.append(this->trigger->toHtml(false));
    }
    ret.append(this->start->localizeHtml());
    for(int i = 0;i < 7;i++){
        ret.append(this->modifiers[i]->localizeHtml());
    }
    ret.append(this->bonus->localizeHtml());
    return ret;
}
NationalIdea* getFromName(std::string str){
    if(nationalIdeas.find(str) == nationalIdeas.end()) return nullptr;
    return nationalIdeas[str];
}
NationalIdea* getTagIdea(std::string name){
    if(tagIdeas.find(name) == tagIdeas.end()) return nullptr;
    return tagIdeas[name];
}
