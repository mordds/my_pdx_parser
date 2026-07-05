#ifndef PDX_PATTERN
#define PDX_PATTERN
#include<string>
struct Scope;




struct Pattern{
	std::string patternString;


	int pos;
	Pattern(std::string str);
	bool setNextString(std::string str);
	bool setNextInteger(long long value);
	std::string getOutput();
};




#endif
