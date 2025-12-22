#ifndef PDX_NATIONAL_IDEA
#define PDX_NATIONAL_IDEA

#include "trigger.h"
#include "modifier.h"
#include <string>
#include <memory>


struct NationalIdea{
    std::shared_ptr<Trigger> trigger;
    std::shared_ptr<Modifier> start;
    std::shared_ptr<Modifier> bonus;
    std::shared_ptr<Modifier> modifiers[7];    
    std::string toString();
    std::string toHtml();

    
};

void loadNationalIdea();

NationalIdea* getTagIdea(std::string tag);

NationalIdea* getFromName(std::string name);


#endif 