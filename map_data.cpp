#include "map_data.h"
#include "localization.h"
#include "utils/parser_util.h"
#include <map>
#include <chrono>

std::vector<ProvinceGroup> continents;
std::map<const std::string*,ProvinceGroup> provinceGroups;
std::map<const std::string*,SuperRegion> superregions;
std::map<const std::string*,Region> regions;
std::map<const std::string*,Area> areas;


//TODO: Auto Scope Impl.
void loadMapDatas(){
    auto start = std::chrono::system_clock::now();
    ParadoxTag* root = parseFile("./map/continent.txt");
    //should not have duplicates in map files.
    for(int i = 0;i < root->size();i++){
        auto[key,value] = (*root)[i];
        ProvinceGroup group;
        ParadoxArray* array = value->getAsArray();
        if(array == nullptr) continue;
        if(array->getContentType() != ParadoxType::INTEGER) {
            log_error(current_location(),"Error In ",key," Continent Definitions: Continents should declare contained province id.");
            continue;
        }
        group.localize_key = getStringPtr(key);
        for(ParadoxBase *base : array->contents){
            ParadoxInteger* pInt = static_cast<ParadoxInteger*>(base);
            group.provinces.push_back(pInt->getIntegerContent() / 1000);
        }
        continents.push_back(group);
    }
    clearParserDatas();
	auto end = std::chrono::system_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    log_error(current_location(),"cost ", duration.count(), "us");
    root = parseFile("./map/provincegroup.txt");
    //should not have duplicates in map files.
    for(int i = 0;i < root->size();i++){
        auto[key,value] = (*root)[i];
        ProvinceGroup group;
        ParadoxArray* array = value->getAsArray();
        if(array == nullptr) continue;
        if(array->getContentType() != ParadoxType::INTEGER) {
            log_error(current_location(),"Error In ",key," Province Group Definitions: PG should declare contained province id.");
            continue;
        }
        auto ptr = getStringPtr(key);
        group.localize_key = ptr;
        for(ParadoxBase *base : array->contents){
            ParadoxInteger* pInt = static_cast<ParadoxInteger*>(base);
            group.provinces.push_back(pInt->getIntegerContent() / 1000);
        }
        provinceGroups[ptr] = group;
    }
    clearParserDatas();
    root = parseFile("./map/area.txt");
    //should not have duplicates in map files.
    for(int i = 0;i < root->size();i++){
        auto[key,value] = (*root)[i];
        Area area;
        ParadoxArray* array = value->getAsArray();
        if(array == nullptr) continue;
        if(array->getContentType() != ParadoxType::INTEGER) {
            log_error(current_location(),"Error In ",key," Area Definitions: Area should declare contained province id.");
            continue;
        }
        auto ptr = getStringPtr(key);
        area.localize_key = ptr;
        for(ParadoxBase *base : array->contents){
            ParadoxInteger* pInt = static_cast<ParadoxInteger*>(base);
            area.provinces.push_back(pInt->getIntegerContent() / 1000);
        }
        areas[ptr] = area;
    }
    clearParserDatas();   
    root = parseFile("./map/region.txt");
    //should not have duplicates in map files.

    for(int i = 0;i < root->size();i++){
        auto[key,value] = (*root)[i];
        Region region;
        auto tag = value->getAsTag()->get("areas",0);
        if(tag == nullptr) continue;
        ParadoxArray* array = tag->getAsArray();
        if(array == nullptr) continue;
        if(array->getContentType() != ParadoxType::STRING) {
            log_error(current_location(),"Error In ",key," Region Definitions: Regions should declare contained areas.");
            continue;
        }
        auto ptr = getStringPtr(key);
        region.localize_key = ptr;
        for(ParadoxBase *base : array->contents){
            ParadoxString* pStr = static_cast<ParadoxString*>(base);
            auto ptr2 = getStringPtr(pStr->getStringContent());
            auto it = areas.find(ptr2);
            if(it == areas.end()) continue;
            region.areas.push_back(&it->second);
        }
        regions[ptr] = region;
    }
    clearParserDatas();
    root = parseFile("./map/superregion.txt");
    if(root == nullptr) return;
    //should not have duplicates in map files.
    for(int i = 0;i < root->size();i++){
        auto[key,value] = (*root)[i];
        SuperRegion superregion;
        ParadoxArray* array = value->getAsArray();
        if(array == nullptr) continue;
        if(array->getContentType() != ParadoxType::STRING) {
            log_error(current_location(),"Error In ",key," SuperRegion Definitions: SuperRegions should declare contained regions.");
            continue;
        }
        auto ptr = getStringPtr(key);
        superregion.localize_key = ptr;
        for(ParadoxBase *base : array->contents){
            ParadoxString* pStr = static_cast<ParadoxString*>(base);
            auto ptr2 = getStringPtr(pStr->getStringContent());
            auto it = regions.find(ptr2);
            if(it == regions.end()) continue;
            superregion.regions.push_back(&it->second);
        }
        superregions[ptr] = superregion;
    }
    clearParserDatas();          
}