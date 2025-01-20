#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "vdf_parser.hpp"

#include "bottles.hpp"
#include "steam.hpp"
#include "internal/steam.hpp"

using namespace tyti;

std::vector<std::string> cellar::steam::find_steam_libraries() {
    std::stringstream sstr_steam_library_config;
    sstr_steam_library_config << std::getenv("HOME");
    sstr_steam_library_config << "/.steam/root/config/libraryfolders.vdf";
    std::string str_steam_library_config = sstr_steam_library_config.str();

    std::ifstream fd_steam_library_config(str_steam_library_config);
    auto hnd_steam_library_config = vdf::read(fd_steam_library_config);
}