#ifndef PDX_FS_UTILS
#include <filesystem>
#include <string>
#include <functional>


void getAllFiles(std::string path, std::vector<std::string>& files);
std::string readAllLines(std::string path);
std::string readAllLinesWithoutComment(std::string path);
std::string readAllLinesWithoutComment(std::string path,char prefix,std::function<bool(std::string)> handler);

#endif