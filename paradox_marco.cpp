#include "paradox_marco.h"
#include "utils/filesystem_util.h"
#include "utils/string_util.h"
#include "utils/parser_util.h"
#include <stack>
#include <fstream>
#include <algorithm>
#include <string_view>

extern std::set<std::string> simpleTriggers;
extern std::set<std::string> registeredTriggers;
extern std::map<std::string,TriggerItem*> items;
std::set<std::string> fixedSTs;
std::map<std::string,ScriptedTrigger*> loadedSTs;

Trigger* ComplicateScriptedTrigger::createInstance(std::map<std::string,ParadoxBase*> datas){
    std::vector<std::unique_ptr<MarcoHolder>>* current_holder = &this->marcoHolders;
    int pos = 0;
    std::stack<int> sPos;
    std::stack<std::vector<std::unique_ptr<MarcoHolder>>*> holders;
    std::string result;
    while(!holders.empty() || pos < current_holder->size()){
        MarcoHolder* holder = (*current_holder)[pos].get();
        if(holder->getType() == HolderType::STRING) result.append(static_cast<StringHolder*>(holder)->data);
        else if(holder->getType() == HolderType::PARAMETER) {
            std::string& name = this->parameterName[static_cast<ParameterHolder*>(holder)->parameterIndex];
            if(datas.find(name) == datas.end()) return nullptr;
            result.append(datas[name]->toString());
        }
        else{
            std::string& name = this->parameterName[static_cast<ConditionalHolder*>(holder)->parameterIndex];
            if(datas.find(name) != datas.end()){
                sPos.push(pos);
                pos = -1;
                holders.push(current_holder);
                current_holder = &static_cast<ConditionalHolder*>(holder)->subHolders;
            }
        }
        pos++;
        while(!holders.empty() && pos >= current_holder->size()){
            current_holder = holders.top();
            pos = sPos.top();
            holders.pop();
            sPos.pop();
            pos++;
        }
    }
    std::cout << result << std::endl;
    return nullptr;
}

enum class MarcoToken{
    IDENT,
    BRACKET_OPEN,
    BRACKET_CLOSE,
    EQUAL,
    DOLLAR,
    SQUARE_OPEN,
    SQUARE_CLOSE,
    END
};
struct TokenStruct {
    MarcoToken token;

};
struct MarcoTokenizer{
    std::string str;
    std::string current_info;
    MarcoTokenizer(const std::string && _str) : str(_str) {
        std::reverse(str.begin(),str.end());
    }
    MarcoToken nextToken(){
        std::string cache = "";
        while(!str.empty()){
            char c = str.back();
            if(cache.empty()){
                str.pop_back();
                if(c == '{') return MarcoToken::BRACKET_OPEN;
                else if(c == '}') return MarcoToken::BRACKET_CLOSE;
                else if(c == '[') return MarcoToken::SQUARE_OPEN;
                else if(c == ']') return MarcoToken::SQUARE_CLOSE;
                else if(c == '$') return MarcoToken::DOLLAR;
                else if(c == '=') return MarcoToken::EQUAL;
                else if(c == ' ') continue;
                else {
                    cache.push_back(c);
                    continue;
                }
            }
            else {
                if(c != '{' && c != '}'
                && c != '[' && c != ']'
                && c != '$' && c != '=')
                {
                    cache.push_back(c);
                }
                else{
                    current_info = cache;
                    return MarcoToken::IDENT;
                }
            }
            str.pop_back();
        }
        if(cache.empty()) {
            return MarcoToken::END;
        }
        else {
            current_info = cache;
            return MarcoToken::IDENT;
        }
    }
};

bool ParseScriptedTrigger(MarcoTokenizer &tokenizer,std::map<std::string,std::string>& fixedSTString);

void printAllScriptedTrigger(){
    for(auto [key,value] : loadedSTs){
        //std::cout << key << std::endl;
    }
}

void loadScriptedTrigger(){
    std::map<std::string,std::string> tempString;
    std::vector<std::string> paths;
    getAllFiles("./scripted_triggers",paths);
    std::ofstream fout("./out.txt");
    for(std::string path : paths){
        std::map<std::string,std::string> simpleMarco;
        std::string content = readAllLinesWithoutComment(path,'@',[&simpleMarco](std::string str){
            auto [key,value] = splitWith(str,"=");
            trim(key);
            trim(value);
            
            simpleMarco[key] = value;
            return false;
        });
        
        for(auto [key,value] : simpleMarco){
            
            replaceWith(content,key,value);
        }
        //std::ofstream fout("./out.txt");
        //fout << content << std::endl;
        MarcoTokenizer tokenizer(std::move(content));
        if(!ParseScriptedTrigger(tokenizer,tempString)) std::cout << "Failed in File" << path << std::endl;
    }
}
bool checkValidName(std::string& str2){
    for(size_t i = 0;i < str2.length();i++){
        char c = str2[i];
        if(c == '_' || (c > 'A' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <='9')) {}
        else return false;
    }
    return true;
}
bool ParseScriptedTrigger(MarcoTokenizer &tokenizer,std::map<std::string,std::string>& fixedSTString){
    int state = 0;
    int depth = 0;
    std::string name;
    std::string content = "";
    std::string content2 = "";
    MarcoToken nextToken;
    ComplicateScriptedTrigger* cst = nullptr;
    std::vector<std::unique_ptr<MarcoHolder>>* currentHolder = nullptr;
    std::stack<std::vector<std::unique_ptr<MarcoHolder>>*> holders;
    bool simple = true;
    while((nextToken = tokenizer.nextToken()) != MarcoToken::END){
        //if(state != 5) std::cout << state << ' ' << (int)nextToken << std::endl;
        switch(state){
            case 0: {
                if(nextToken != MarcoToken::IDENT) goto error;
                name = tokenizer.current_info;
                trim(name);
                state = 2;
                break;
            }
            case 2:{
                if(nextToken != MarcoToken::EQUAL) goto error;
                state = 4;
                break;
            }
            case 3:{
                if(nextToken != MarcoToken::DOLLAR) goto error;
                ParameterHolder* pHolder = new ParameterHolder(); 
                std::unique_ptr<MarcoHolder> holder(pHolder);
                auto it = std::find(cst->parameterName.begin(),cst->parameterName.end(),content2);
                if(it == cst->parameterName.end()) {
                    pHolder->parameterIndex = cst->parameterName.size();
                    cst->parameterName.push_back(content2);
                }
                else {
                    pHolder->parameterIndex = it - cst->parameterName.begin();
                }
                currentHolder->push_back(std::move(holder));
                content2.clear();
                state = 5;
                break;
            }
            case 4:{
                if(nextToken != MarcoToken::BRACKET_OPEN) goto error;
                depth++;
                state = 5;
                cst = new ComplicateScriptedTrigger();
                currentHolder = &cst->marcoHolders;
                break;
            }
            case 5:{
                if(nextToken == MarcoToken::IDENT){
                    content.append(tokenizer.current_info);
                }
                else if(nextToken == MarcoToken::EQUAL){
                    content.push_back('=');
                }
                else if(nextToken == MarcoToken::DOLLAR){
                    simple = false;
                    if(!content.empty()){
                        currentHolder->push_back(std::make_unique<StringHolder>(content));
                        content.clear();
                    }
                    state = 6;
                }
                else if(nextToken == MarcoToken::SQUARE_OPEN){
                    simple = false;
                    if(!content.empty()){
                        currentHolder->push_back(std::make_unique<StringHolder>(content));
                        content.clear();
                    }
                    state = 7;

                }
                else if(nextToken == MarcoToken::SQUARE_CLOSE){
                    if(holders.empty()) goto error;
                    if(!content.empty()){
                        currentHolder->push_back(std::make_unique<StringHolder>(content));
                        content.clear();
                    }
                    currentHolder = holders.top();
                    holders.pop();
                }
                else if(nextToken == MarcoToken::BRACKET_CLOSE){
                    if(depth == 1) {
                        state = 0;
                        if(simple){
                            delete cst;
                            FixedScriptedTrigger* fst = new FixedScriptedTrigger();
                            fixedSTString[name] = content;
                            loadedSTs[name] = fst;
                            content.clear();
                        }
                        else {
                            if(!content.empty()) currentHolder->push_back(std::make_unique<StringHolder>(content));
                            loadedSTs[name] = cst;
                            content.clear();
                        }
                        depth = 0;
                    }
                    else {
                        content.push_back('}');
                        depth--;
                    }
                }
                else if(nextToken == MarcoToken::BRACKET_OPEN){
                    content.push_back('{');
                    depth++;
                }
                else goto error;
                break;
            }
            case 6:{
                if(nextToken != MarcoToken::IDENT) goto error;
                content2 = tokenizer.current_info;
                state = 3;
                break;
            }
            case 7:{
                if(nextToken != MarcoToken::SQUARE_OPEN) goto error;
                state = 8;
                break;
            }
            case 8:{
                if(nextToken != MarcoToken::IDENT) goto error;
                content2 = tokenizer.current_info;
                state = 9;
                break;
            }
            case 9:{
                if(nextToken != MarcoToken::SQUARE_CLOSE) goto error;
                ConditionalHolder* cHolder = new ConditionalHolder();
                std::unique_ptr<MarcoHolder> holder(cHolder);
                auto it = std::find(cst->parameterName.begin(),cst->parameterName.end(),content2);
                if(it == cst->parameterName.end()) {
                    cHolder->parameterIndex = cst->parameterName.size();
                    cst->parameterName.push_back(content2);
                    
                }
                else {
                    cHolder->parameterIndex = it - cst->parameterName.begin();
                }
                content2.clear();
                currentHolder->push_back(std::move(holder));
                holders.push(currentHolder);
                currentHolder = &cHolder->subHolders;
                
                state = 5;
                break;
            }
        }
    }
    return true;
    error:
    return false;
}