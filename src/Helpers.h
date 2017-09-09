#pragma once

#include <cstdlib>
#include <iostream>
#include <vector>
//#include <llvm/IR/LLVMContext.h>

#define ERROR(x) {std::cerr<< x << " in " << __FILE__ << ":" << __LINE__ << std::endl; abort();}

//static llvm::LLVMContext TheContext;

std::string repeat(const std::string &str, int times);

template<typename T>
bool equal_content_by_pointer(const std::vector<T*> &v1, const std::vector<T*> &v2) {
    return std::equal(v1.begin(), v1.end(), v2.begin(), v2.end(), [](const T *lhs, const T *rhs) { return *lhs == *rhs; });
}

template<typename E>
constexpr auto to_underlying(E e) -> typename std::underlying_type<E>::type
{
    return static_cast<typename std::underlying_type<E>::type>(e);
}