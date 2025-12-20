#ifndef PDX_STRING_UTIL
#define PDX_STRING_UTIL
#include<string>
#include<map>
bool isNumber(std::string str);
bool startWith(std::string from,std::string to);
std::pair<std::string,std::string> splitWith(std::string source,std::string split);
std::string toUpperCase(std::string src);
std::string toLowerCase(std::string src);


#endif