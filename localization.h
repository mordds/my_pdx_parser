#ifndef PDX_LOCALIZATION
#define PDX_LOCALIZATION
#include <string>
#include <thread>


std::thread& readLocalizations(std::string path = ".");



const std::string* getLocalizationKeyPtr(std::string key);

const std::string& registerShortString(std::string str);

bool hasLocalization(const std::string& key);

#ifndef PDX_USE_SIMPLE_LOCALIZATION_SYSTEM
std::string getLocalization(std::string key);
#else
const std::string& getLocalization(std::string key);
#endif

void clearThreads();
#endif