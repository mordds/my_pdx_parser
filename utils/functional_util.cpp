#include "functional_util.h"

std::function<std::string()> noArgsPattern(std::string pattern){
    return [pattern](){
        return pattern;
    };
}
