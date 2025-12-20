#include "db_object.h"
#include <map>
#include <string>
#include <stdio.h>
#include "y.tab.h"
#include "utils/parser_util.h"
#include "modifier.h"
#include "localization.h"
#include <ratio>
#include <iostream>
std::map<std::string,Good> goodRegistry;


Good* getGood(std::string str){
    if(goodRegistry.find(str) == goodRegistry.end()) return nullptr;
    return &goodRegistry[str];
}

void registerGood(){
    ParadoxTag* root = parseFile("./datas/price.txt");
    for(std::string str : root->seq){
        Good good;
        
        ParadoxTag* tag = root->tags[str]->getAsTag();
        good.defaultPrice = tag->get("base_price",1)->getAsInteger()->getIntegerContent();
        std::string localizeKey = "good_";
        localizeKey.append(str);
        good.localizedName = getLocalization(localizeKey); 
        goodRegistry[str] = good;
    }
    clearParserDatas();
    root = parseFile("./datas/trade_good.txt");
    for(std::string str : root->seq){
        if(goodRegistry.find(str) == goodRegistry.end()) continue;
        goodRegistry[str].globalModifier = std::make_shared<Modifier>();
        goodRegistry[str].provinceModifier = std::make_shared<Modifier>();
        ParseModifier(root->tags[str]->getAsTag()->getAsTag("modifier",1), *(goodRegistry[str].globalModifier.get()));
        goodRegistry[str].globalModifier->name = "贸易优势奖励";
        ParseModifier(root->tags[str]->getAsTag()->getAsTag("province",1), *(goodRegistry[str].provinceModifier.get()));
         goodRegistry[str].provinceModifier->name = "生产该商品的省份效果";
    }
    clearParserDatas();
}



