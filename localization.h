#ifndef PDX_LOCALIZATION
#define PDX_LOCALIZATION
#include <string>

void getAllFiles(std::string path);

void readLocalizations();

std::string getLocalization(std::string key);

#endif