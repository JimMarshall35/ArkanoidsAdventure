#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <map>
#include  <cstdlib> 
typedef std::string EString;
typedef std::wstring EWString;

template<typename T>
using EAllocator = std::allocator<T>;

template<typename T>
using EVec = std::vector<T, EAllocator<T>>;

template<typename T>
using EMap = std::unordered_map<T, EAllocator<T>>;

template<typename T>
using EOrderedMap = std::map<T, EAllocator<T>>;

template<typename T>
using EUPtr = std::unique_ptr<T, EAllocator<T>>;
//
//template<typename T>
//using ESPtr = std::shared_ptr<T, EAllocator<T>>;