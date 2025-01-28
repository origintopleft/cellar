#pragma once
#include <map>

#include "bottles.hpp"

namespace cellar {
    namespace steam {
        extern cellar::bottles::Bottle app_bottle(unsigned appid);
        extern std::map<std::string, cellar::bottles::Bottle> get_app_bottles();
    }
}