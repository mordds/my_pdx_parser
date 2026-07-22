#include "localization.h"
#include "utils/string_util.h"
#include "utils/filesystem_util.h"
#include "paradox_type.h"
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

int activeFileId = 0;
int activeFilePage = 0;
bool file_open = false;
//1M + 4k buffer
char head_buffer[4 * 1024];
char string_buffer[1024 * 1024];
uint16_t file_id = 0,file_index = 0;
size_t current_offset = 0;
std::string rootPath = ".";
void setRootPath(std::string root_path){
    if(root_path.empty()) rootPath = ".";
    else rootPath = root_path;
}

void createTempFiles(){
    //defaultly capacity is 256K string.
    std::string path1 = rootPath;
    path1.append("/temp");
    if(!std::filesystem::exists(path1)){
        std::filesystem::create_directory(path1);
    }
    char* smallBuffer = new char[1024];
    memset(smallBuffer,0,1);
    for(int i = 0;i < 256;i++){
        std::string path = path1;
        path.append("/t_");
        path.append(std::to_string(i));
        path.append(".bin");
        std::ofstream fout(path,std::ios::out | std::ios::binary | std::ios::trunc);
        for(int i = 0;i < 4;i++) fout.write(smallBuffer,1024);
        fout.flush();
    }
    delete [] smallBuffer;
}
void loadTempFileHead(int index){
    if(activeFileId == index) return;
    std::string path = rootPath;
    path.append("/temp/t_");
    path.append(std::to_string(index));
    path.append(".bin");
    std::fstream activeFile;
    activeFile.open(path,std::ios::out | std::ios::binary | std::ios::in);
    activeFile.read(head_buffer,4096);
}

void loadTempFileData(int index,int page = 0){
    if(activeFileId == index && activeFilePage == page) return;
    else {
        std::string path = rootPath;
        path.append("/temp/t_");
        path.append(std::to_string(index));
        path.append(".bin");
        size_t fileSize = std::filesystem::file_size(path);
        if(fileSize < 4096 + page * 1048576) return;
        std::fstream activeFile;
        activeFile.open(path,std::ios::out | std::ios::binary | std::ios::in);
        activeFile.seekg(4096 + page * 1048576,std::ios::beg);
        activeFile.read(string_buffer,1048576);
        activeFile.close();
        activeFileId = index;
        activeFilePage = page;
    }
}
void storeTempFileData(int index,int page = 0){
    std::string path = rootPath;
    path.append("/temp/t_");
    path.append(std::to_string(index));
    path.append(".bin");
    size_t fileSize = std::filesystem::file_size(path);
    while(fileSize < 4096 + page * 1048576) page--;
    std::fstream activeFile;
    activeFile.open(path,std::ios::out | std::ios::binary | std::ios::in);
    activeFile.write(head_buffer,4096);
    activeFile.seekg(4096 + page * 1048576,std::ios::beg);
    activeFile.write(string_buffer,1048576);
    activeFile.close();
}

LocalizationItem createLongStringItem(const std::string& str){
    
    LocalizationItem item;
   
    item.data.file_data.file_id = file_id;
    item.data.file_data.file_index = file_index;
    item.data.file_data.str_len = str.length();
    item.is_short = false;

    size_t page = current_offset / 1048576;
          
    size_t mOffset = current_offset + str.length() + 1;
    if(mOffset / 1048576 > page) {
        current_offset = (page + 1) * 1048576;
        storeTempFileData(file_id,page);
        mOffset = current_offset + str.length() + 1;
    }
    unsigned int* target = (unsigned int*)(head_buffer + (file_index * 4));
    *target = current_offset;
    memcpy(string_buffer + (current_offset % 1048576),str.c_str(),str.length() + 1);
    current_offset = mOffset;
    file_index++;
    if(file_index >= 1024){
        file_index = 0;
        storeTempFileData(file_id,current_offset / 1048576);
        memset(head_buffer,0,4096);
        file_id++;
        current_offset = 0;
    }
    return item;
}
#else

std::set<std::string> shortStringSet;
std::map<const std::string*,std::string*> localizations;
std::string rootPath = ".";

void setRootPath(std::string root_path){
    if(root_path.empty()) rootPath = ".";
    else rootPath = root_path;
}

#endif







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
        //if(startWith(pair.first,"is_or_was_tag")) std::cout << pair.first << std::endl;
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
std::string getLocalization(std::string key){
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

            if(key == "I26_memories_of_the_remnant_fleet_desc") log_error(current_location(),item.data.file_data.file_id,",",item.data.file_data.file_index);
            loadTempFileHead(item.data.file_data.file_id);
            unsigned int* uOffset = (unsigned int*)head_buffer;
            int offset = uOffset[item.data.file_data.file_index];
            int page = offset / 1048576;
             if(key == "I26_memories_of_the_remnant_fleet_desc") log_error(current_location(),offset," ",page);
            loadTempFileData(item.data.file_data.file_id,page);
            
            longStringBufferIndex.push(id);
            if(longStringBufferIndex.size() > 256) {
                int pop_id = longStringBufferIndex.front();
                longStringBufferIndex.pop();
                longStringBuffer.erase(pop_id);
            }
            longStringBuffer[id] = std::string(string_buffer + (offset % 1048576),item.data.file_data.str_len);
            ret = &longStringBuffer[id];
        }
    } 
    return *ret;
}
#else
const std::string& getLocalization(std::string key){
    const std::string* key2 = getLocalizationKeyPtr(key);
    //std::cout << *key2 << std::endl;
    if(localizations.find(key2) != localizations.end()) return *localizations[key2];
    return *key2;
}
#endif
const std::string* getLocalizationKeyPtr(std::string key){
    auto it2 = shortStringSet.find(key);
    if(it2 == shortStringSet.end())  return &(*shortStringSet.insert(key).first);
    return &(*it2);
}
const std::string& registerShortString(const std::string str){
    auto it2 = shortStringSet.find(str);
    if(it2 == shortStringSet.end()) {
        return *shortStringSet.insert(str).first;
    }
    return (*it2);    
}
bool hasLocalization(const std::string& key){
    if(shortStringSet.find(key) == shortStringSet.end()) return false;
    return localizations.find(getLocalizationKeyPtr(key)) != localizations.end();
}
void _readLocalizations(std::string root_path){
    setRootPath(root_path);
    root_path.append("/localization");
    #ifndef PDX_USE_SIMPLE_LOCALIZATION_SYSTEM
    file_id = 0;
    file_index = 0;
    #endif
    std::cout << "#loading locs..." << std::endl;
    std::vector<std::string> files;
    #ifndef PDX_USE_SIMPLE_LOCALIZATION_SYSTEM
    createTempFiles();
    #endif
    getAllFiles(root_path,files);
    for(std::string str : files){
        readFromFiles(str);
    }
    #ifndef PDX_USE_SIMPLE_LOCALIZATION_SYSTEM
    storeTempFileData(file_id,current_offset / 1048576);
    activeFileId = -1;
    activeFilePage = -1;
    #endif

    std::cout << "#locs loaded!" << std::endl;
}

std::thread& readLocalizations(std::string path){
    if(localization_thread != nullptr) {
        delete localization_thread;
        localization_thread = nullptr;
    }
    localization_thread = new std::thread(_readLocalizations,path);
    return *localization_thread;
}

void clearThreads(){
    if(localization_thread != nullptr) {
        delete localization_thread;
        localization_thread = nullptr;
    }
}