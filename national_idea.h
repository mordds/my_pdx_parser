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
    std::string toString() const;
    std::string toHtml() const;

    
};

void loadNationalIdea();

const NationalIdea* getTagIdea(std::string tag);

const NationalIdea* getFromName(std::string name);


#endif 