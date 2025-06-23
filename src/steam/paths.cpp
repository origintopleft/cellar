#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"
#include "vdf_parser.hpp"

#include "bottles.hpp"
#include "steam.hpp"
#include "internal/steam.hpp"

using namespace tyti;

/**
 * @brief Reads Steam library settings and returns a list of Steam library paths.
 * Returns an empty vector if it can't read ~/.steam/root/config/libraryfolders.vdf.
 * 
 * @return std::vector<std::string> Steam library paths.
 */
std::vector<std::string> cellar::steam::find_steam_libraries() {
    std::stringstream sstr_steam_library_config;
    sstr_steam_library_config << std::getenv("HOME");
    sstr_steam_library_config << "/.steam/root/config/libraryfolders.vdf";
    std::string str_steam_library_config = sstr_steam_library_config.str();
    std::vector<std::string> result = {};

    std::ifstream fd_steam_library_config(str_steam_library_config);
    if (fd_steam_library_config.fail()) { return result; } // return empty if something went wrong (should cover most problems)
    auto hnd_steam_library_config = vdf::read(fd_steam_library_config);

    for (auto hnd_library_def : hnd_steam_library_config.childs) {
        std::string str_index = hnd_library_def.first;
        auto hnd_library = hnd_library_def.second;
        result.push_back(hnd_library->attribs["path"]);
    }

    return result;
}

/**
 * @brief Gets available versions of Proton from Steam.
 * 
 * @return std::vector<std::string> Proton versions.
 */
std::map<std::string, std::string> cellar::steam::find_steam_protons() {
    std::map<std::string, std::string> result;

    for (std::string str_library_path : find_steam_libraries()) {
        std::filesystem::path pth_library(str_library_path);

        for ()
    }
}