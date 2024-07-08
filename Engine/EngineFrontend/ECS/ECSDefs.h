#pragma once
#include <bitset>

typedef unsigned int Entity;
typedef unsigned int ComponentPoolID;

#define ComponentCounterInit static int fisoidfjiosdjfiosdjfdfhdhgudfhj = __COUNTER__;
#define Component(name, poolsize) constexpr ComponentPoolID gID_##name = __COUNTER__ ; struct name 
#define ComponentID(name) gID_##name
#define MaxTypesOfComponent 64
typedef std::bitset<MaxTypesOfComponent> EntityComponents;