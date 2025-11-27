#ifndef PDX_PATTERN
#define PDX_PATTERN
#include<string>
struct Pattern{
	std::string patternString;
	std::string output;
	int pos;
	Pattern(std::string str);
	bool setNextString(std::string str);
	bool setNextInteger(long long value);
	std::string getOutput();
};


#endif
