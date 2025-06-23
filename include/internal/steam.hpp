#pragma once

#include <map>
#include <string>
#include <vector>

#include "commands.hpp"

namespace cellar {
    namespace steam {
        extern std::vector<std::string> find_steam_libraries();
        extern std::map<std::string, std::string> find_steam_protons();
    }
}