#include "localization.h"
#include "utils/string_util.h"
#include<vector>
#include<filesystem>
#include<fstream>
#include<map>
#include<iostream>
#include<set>
#include<queue>
#include<cstring>
#include<mutex>
#include<shared_mutex>
#include<thread>

std::thread* localization_thread = nullptr;

#ifndef PDX_USE_SIMPLE_LOCALIZATION_SYSTEM
struct LocalizationItem {
    union _Data{
        const std::string* str;
        struct file_struct {
            uint16_t file_id;
            uint16_t file_index;
            uint32_t str_len;
        } file_data;
    } data;
    bool is_short;
    LocalizationItem(const std::string*);
    LocalizationItem() = default;
};

LocalizationItem::LocalizationItem(const std::string* str){
    data.str = str;
    is_short = true;
}
std::map<int,std::string> longStringBuffer;
std::queue<int> longStringBufferIndex;
std::set<std::string> shortStringSet;
std::map<const std::string*,LocalizationItem> localizations;
std::shared_mutex mtx;
std::fstream activeFile;
int activeFileId = 0;
bool file_open = false;
char buffer[8192];
uint16_t file_id = 0,file_index = 0;
void createTempFiles(){
    //defaultly capacity is 256K string.
    if(!std::filesystem::exists("./temp")){
        std::filesystem::create_directory("./temp");
    }
    char* smallBuffer = new char[1024];
    memset(smallBuffer,0,1);
    for(int i = 0;i < 256;i++){
        std::string path("./temp/t_");
        path.append(std::to_string(i));
        path.append(".bin");
        std::ofstream fout(path,std::ios::out | std::ios::binary | std::ios::trunc);
        for(int i = 0;i < 4;i++) fout.write(smallBuffer,1024);
        fout.flush();
    }
}
std::fstream& getTempFile(int id){
    if(file_open){
        if(activeFileId == id) return activeFile;
        activeFile.flush();
        activeFile.close();
        std::string path("./temp/t_");
        path.append(std::to_string(id));
        path.append(".bin");
        activeFile.open(path,std::ios::out | std::ios::binary | std::ios::in);
        activeFileId = id;
        return activeFile;        
    }
    else{
        std::string path("./temp/t_");
        path.append(std::to_string(id));
        path.append(".bin");
        activeFile.open(path,std::ios::out | std::ios::binary | std::ios::in);
        activeFileId = id;        
        file_open = true;
        return activeFile;
    }
}
void closeActiveTempFile(){
    if(file_open){
        activeFile.flush();
        activeFile.close();
        file_open = false;
    }
}
LocalizationItem createLongStringItem(std::string& str){
    LocalizationItem item;
   
    item.data.file_data.file_id = file_id;
    item.data.file_data.file_index = file_index;
    item.data.file_data.str_len = str.length();
    item.is_short = false;
    int offset = 4096;      
    std::fstream& file = getTempFile(file_id);
    file.exceptions(std::ifstream::failbit);
    if(file_index != 0) {
        file.seekg((file_index - 1) * 4,std::ios::beg);
        try{
            file.read((char*)(&offset),sizeof(int));
        }
        catch(std::ios_base::failure& e){
            std::cout << e.what() << std::endl;
        }
    }    

    file.seekg(file_index * 4,std::ios::beg);
    int off1 = offset + str.length() + 1;
    
    file.write(((char*)(&off1)),sizeof(int));
    
    file.seekg(0,std::ios::end);
    file.write(str.c_str(),str.size() + 1);
    file_index++;
    if(file_index >= 1024){
        file_index = 0;
        file_id++;
    }
    return item;
}
#else

std::set<std::string> shortStringSet;
std::map<const std::string*,std::string*> localizations;

#endif






void getAllFiles(std::string path, std::vector<std::string>& files) 
{
	std::filesystem::path dir(path);
	for (const auto& entry : std::filesystem::directory_iterator(dir)) {
		files.push_back(entry.path().string());
	}
}

#ifndef PDX_USE_SIMPLE_LOCALIZATION_SYSTEM
void readFromFiles(std::string path){
  
    std::ifstream fin;
    fin.open(path);
    while(!fin.eof()){
        std::string line;
        std::getline(fin,line);
        if(line.length() == 0) continue; 
        if(line[0] == '#') continue;
        std::pair<std::string,std::string> pair = splitWith(line,":");
        replaceWith(pair.second,"\\n","\n");
        const std::string* r1 = getLocalizationKeyPtr(pair.first);
        if(pair.second.length() > 256){
            localizations[r1] = createLongStringItem(pair.second);
        }
        else {
            auto it = shortStringSet.insert(pair.second);
            localizations[r1] = LocalizationItem(&(*it.first));
        }  
    }
    fin.close();
}
#else
void readFromFiles(std::string path){
    std::ifstream fin;
    fin.open(path);
    while(!fin.eof()){
        std::string line;
        std::getline(fin,line);
        if(line.length() == 0) continue; 
        if(line[0] == '#') continue;
        std::pair<std::string,std::string> pair = splitWith(line,":");
        replaceWith(pair.second,"\\n","\n");
        const std::string* r1 = getLocalizationKeyPtr(pair.first);
        localizations[r1] = const_cast<std::string*>(getLocalizationKeyPtr(pair.second));
    }
    fin.close();
}
#endif




#ifndef PDX_USE_SIMPLE_LOCALIZATION_SYSTEM
const std::string& getLocalization(std::string key){
    //std::shared_lock<std::shared_mutex> lock(mtx);
    
    const std::string* ret;
    const std::string* key2 = getLocalizationKeyPtr(key);
    if(localizations.find(key2) == localizations.end()) ret = key2;
    else {
        LocalizationItem item = localizations[key2];
        if(item.is_short) return *item.data.str;
        else{
            int id = ((((int)item.data.file_data.file_id) << 16) + item.data.file_data.file_index);
            if(longStringBuffer.find(id) != longStringBuffer.end()) return longStringBuffer[id];
            std::fstream& file = getTempFile(item.data.file_data.file_id);
            int offset = 4096;
            if(item.data.file_data.file_index != 0){
                file.seekg((item.data.file_data.file_index - 1)*4,std::ios::beg);
                file.read((char*)&offset,4);
            }

            file.seekg(offset,std::ios::beg);
            memset(buffer,0,8192);
            file.read(buffer,item.data.file_data.str_len);
            longStringBufferIndex.push(id);
            if(longStringBufferIndex.size() > 256) {
                int pop_id = longStringBufferIndex.front();
                longStringBufferIndex.pop();
                longStringBuffer.erase(pop_id);
            }
            longStringBuffer[id] = std::string(buffer);
            return longStringBuffer[id];
        }
    } 
    return *ret;
}
#else
const std::string& getLocalization(std::string key){
    const std::string* key2 = getLocalizationKeyPtr(key);
    if(localizations.find(key2) != localizations.end()) return *localizations[key2];
    return *key2;
}
#endif
const std::string* getLocalizationKeyPtr(std::string key){

    auto it2 = shortStringSet.find(key);
    if(it2 == shortStringSet.end())  return &(*shortStringSet.insert(key).first);
    return &(*it2);
}
const std::string& registerShortString(std::string str){
    auto it2 = shortStringSet.find(str);
    if(it2 == shortStringSet.end())  return (*shortStringSet.insert(str).first);
    return (*it2);    
}
void _readLocalizations(){
    //std::unique_lock<std::shared_mutex> lock(mtx);
    #ifndef PDX_USE_SIMPLE_LOCALIZATION_SYSTEM
    file_id = 0;
    file_index = 0;
    #endif
    std::cout << "#loading locs..." << std::endl;
    std::vector<std::string> files;
    #ifndef PDX_USE_SIMPLE_LOCALIZATION_SYSTEM
    createTempFiles();
    #endif
    getAllFiles("./localization",files);
    for(std::string str : files){
        readFromFiles(str);
    }
    #ifndef PDX_USE_SIMPLE_LOCALIZATION_SYSTEM
    closeActiveTempFile();
    #endif
    std::cout << "#locs loaded!" << std::endl;
}

std::thread& readLocalizations(){
    if(localization_thread != nullptr) delete localization_thread;
    localization_thread = new std::thread(_readLocalizations);
    return *localization_thread;
}

void clearThreads(){
    if(localization_thread != nullptr) delete localization_thread;
}