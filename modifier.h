#ifndef PDX_MODIFIER
#define PDX_MODIFIER
#include "paradox_type.h"
#include <vector>
#include <string>
enum ModifierType{
	NORMAL = 0,
	NEUTRAL = 1,
	MINUS = 2,
	PERCENTAGE = 3,
	NEUTRAL_PERCENTAGE = 4,
	MINUS_PERCENTAGE = 5,
	MARK = 6,
	MINUS_MARK = 7,
	NEUTRAL_MARK = 8,
	
	//special percentage
	PERCENTAGE2 = 9,
	NEUTRAL_PERCENTAGE2 = 10,
	MINUS_PERCENTAGE2 = 11,

	REVERSED_PERCENTAGE2 = 12
	
};
struct ModifierItem;
struct Modifier{
	
	std::vector<ModifierItem> items;
	std::string name;
	
	std::string localize();
	std::string localizeHtml();
};
struct ModifierItem{
	std::string modifierName;
	int value;	
};

struct ModifierObject{
	ModifierType type;
	std::string localizedName;
};

void registerModifier(std::string modifierName,ModifierType type,std::string localizedName);

void ParseModifier(ParadoxTag* tag,std::vector<Modifier>& modifiers);

void ParseModifier(ParadoxTag* tag,Modifier& modifier);

void loadInternalModifier();


#endif
