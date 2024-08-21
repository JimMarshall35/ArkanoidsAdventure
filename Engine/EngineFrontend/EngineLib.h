#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <map>
#include <set>
#include  <cstdlib> 
#include <cassert>
#include <entt/entt.hpp>

#define EAssert(a) assert(a)

typedef std::int64_t i64;
typedef std::uint64_t u64;
typedef std::int32_t i32;
typedef std::uint32_t u32;
typedef std::int16_t i16;
typedef std::uint16_t u16;
typedef std::uint8_t u8;
typedef std::int8_t i8;

typedef entt::entity Entity;
typedef entt::registry EntityReg;


typedef std::string EString;
typedef std::wstring EWString;

template<typename T>
using EAllocator = std::allocator<T>;

template<typename T>
using EVec = std::vector<T, EAllocator<T>>;

template<typename Tkey, typename Tval>
using EMap = std::unordered_map<Tkey, Tval>;

template<typename Tkey, typename Tval>
using EOrderedMap = std::map<Tkey, Tval>;

template<typename T>
using EUPtr = std::unique_ptr<T>;

template<typename T, typename T2>
using EPair = std::pair<T, T2>;

template<typename T>
using ESet = std::set<T>;

//template<typename T>
//using ESPtr = std::shared_ptr<T, EAllocator<T>>;

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;

typedef float f32;
typedef double f64;

enum class Directions {
	NONE = 0,
	UP = 1,
	DOWN = 2,
	LEFT = 4,
	RIGHT = 8,

};
inline Directions operator|(Directions a, Directions b)
{
	return static_cast<Directions>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}

inline void operator|=(Directions& a, Directions b)
{
	a = static_cast<Directions>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}

inline unsigned int operator&(Directions a, Directions b)
{
	return static_cast<unsigned int>(a) & static_cast<unsigned int>(b);
}

#define NUMDIRECTIONS 4