#ifndef PDX_PARSER_UTIL
#define PDX_PARSER_UTIL
#include <string>
#include "../paradox_type.h"
#include "../y.tab.h"



ParadoxTag* parseFile(std::string path);
ParadoxTag* parseString(std::string str);
void clearParserDatas();

#endif