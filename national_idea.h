#ifndef PDX_NATIONAL_IDEA
#define PDX_NATIONAL_IDEA

#include "trigger.h"
#include "modifier.h"
#include <string>
#include <memory>


struct NationalIdea{
    std::unique_ptr<Trigger> trigger;
    std::unique_ptr<Modifier> start;
    std::unique_ptr<Modifier> bonus;
    std::unique_ptr<Modifier> modifiers[7];  
    std::string toString() const;
    std::string toHtml() const;

    
};

void loadNationalIdea(std::string rootPath = ".");

const NationalIdea* getTagIdea(std::string tag);

const NationalIdea* getFromName(std::string name);
void reloadNationalIdeas(std::string rootPath = ".");

#endif 