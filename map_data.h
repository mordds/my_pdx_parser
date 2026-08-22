#include "scope.h"
#include <string>
#include <vector>

struct Region;
struct Area;

struct SuperRegion {
    const std::string* localize_key;
    std::vector<Region*> regions;
};

struct Region {
    const std::string* localize_key;
    std::vector<Area*> areas;
};

struct Area {
    const std::string* localize_key;
    std::vector<uint16_t> provinces;
};

struct ProvinceGroup {
    const std::string* localize_key;
    std::vector<uint16_t> provinces;
};

void loadMapDatas();