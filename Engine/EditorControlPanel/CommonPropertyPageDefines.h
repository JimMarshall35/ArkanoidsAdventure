#pragma once
#include <functional>
#include "pugixml.hpp"
typedef std::function<pugi::xml_node(void)> GetXMLSceneFn;
