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

/* MFC and unreal engine style verify */
#ifdef _DEBUG
#define EVerify(a) EAssert(a)
#else
#define EVerify(a) a
#endif


#define ENGINE_NULL_HANDLE 0

typedef size_t Handle_t;
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

using EBitVec = std::vector<bool>;

template<typename T>
using EVec = std::vector<T>;

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
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN,
	NUMDIRECTIONS
};

enum class MouseButtons
{
	LEFT,
	MIDDLE,
	RIGHT,
	MB_4,
	MB_5,
	NUM
};
