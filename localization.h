#ifndef PDX_LOCALIZATION
#define PDX_LOCALIZATION
#include <string>
#include <thread>

#define PDX_USE_SIMPLE_LOCALIZATION_SYSTEM

std::thread& readLocalizations();

const std::string& getLocalization(std::string key);

const std::string* getLocalizationKeyPtr(std::string key);

const std::string& registerShortString(std::string str);

#ifndef PDX_USE_SIMPLE_LOCALIZATION_SYSTEM
void closeActiveTempFile();
#endif

void clearThreads();
#endif