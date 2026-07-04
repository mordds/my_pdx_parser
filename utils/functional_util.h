#ifndef PDX_FUNCTIONAL_UTIL
#define PDX_FUNCTIONAL_UTIL
#include "../paradox_type.h"
#include "../pattern.h"
#include "../scope.h"
#include "../effect.h"
#include "../localization.h"
#include <functional>
#include <tuple>
#include <iostream>

template<typename... types>
std::string applyPattern(std::string pattern,types... args){
	Pattern p(pattern);

	return applyPattern(p,args...);
}

template<typename... types>
std::string applyPattern(Pattern& pattern, types... args);

template<typename head,typename... tails>
std::string applyPattern(Pattern& pattern,head value,tails... args){
	if constexpr (std::is_integral_v<head> && !std::is_same_v<head,bool>){
		pattern.setNextInteger(value);
	}
	if constexpr (std::is_same_v<head,std::string>){
		pattern.setNextString(getLocalization(value));
	}
	if constexpr (std::is_same_v<head,Scope*>){
    
		pattern.setNextString(value->toString());
	}
	if constexpr (sizeof...(args) != 0) return applyPattern(pattern,args...);
	return pattern.getOutput();
}


template<int index,typename... types>
std::function<std::string(types...)> _signedPattern(std::string positve,std::string negative){
    static_assert(getParadoxType<typename std::tuple_element<index, _NativeCommonEffect<types...>>::type>() == ParadoxType::INTEGER, "the assigned arg must be INTEGER!");
    static_assert(index < sizeof...(types),"the index is overflowed!");
    return [positve,negative](types... args){
        void* arr[]{ &args... };
        long long value = *(long long*)arr[index];
        return applyPattern(value > 0 ? positve : negative, value);
    };
}




template<int index,typename... types>
std::function<std::string(types...)> _signedOrderPattern(std::string positive,std::string negative){
    static_assert(getParadoxType<typename std::tuple_element<index, _NativeCommonEffect<types...>>::type>() == ParadoxType::INTEGER, "the assigned arg must be INTEGER!");
    static_assert(index < sizeof...(types),"the index is overflowed!");

    return [positive,negative](types... args){
        void* arr[]{&args...};
        long long value = *(long long*)arr[index];
        
        return applyPattern(value > 0 ? positive : negative, args...);
    };
}

template<typename... types>
std::function<std::string(types...)> _orderedPattern(std::string pattern){
    return [pattern](types... args){
        return applyPattern(pattern, args...);
    };
}

template<int index,ParadoxType... types>
inline std::function<std::string(rawType<types>...)> signedPattern(std::string positive,std::string negative){
    return _signedPattern<index,rawType<types>...>(positive,negative);
}

template<int index,ParadoxType... types>
inline std::function<std::string(rawType<types>...)> signedOrderPattern(std::string positive,std::string negative){
    return _signedOrderPattern<index,rawType<types>...>(positive,negative);
}

template<ParadoxType... types>
inline std::function<std::string(rawType<types>...)> orderedPattern(std::string pattern){
    return _orderedPattern<rawType<types>...>(pattern);
}
/*
template<int index,ParadoxType... types>
using signedPattern = _signedPattern<index,rawType<types>...>;
template<int index,ParadoxType... types>
using signedOrderPattern = _signedOrderPattern<index,rawType<types>...>;
template<ParadoxType... types>
using orderedPattern = _orderedPattern<rawType<types>...>;
*/

std::function<std::string()> noArgsPattern(std::string pattern);



#endif