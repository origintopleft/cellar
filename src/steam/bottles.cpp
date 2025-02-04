#include <filesystem>
#include <fstream>
#include <map>
#include <string>

#include "vdf_parser.hpp"

#include "bottles.hpp"
#include "steam.hpp"
#include "internal/steam.hpp"

using namespace tyti; //vdf

/**
 * @brief Returns all app bottles managed by Steam.
 * 
 * @return std::map<std::string, cellar::bottles::Bottle> Steam managed bottles. Keys are "steam:<appid>".
 */
std::map<std::string, cellar::bottles::Bottle> cellar::steam::get_app_bottles() {
    std::map<std::string, cellar::bottles::Bottle> result;

    for (std::string str_path_library : cellar::steam::find_steam_libraries()) {
        std::filesystem::path pth_library(str_path_library);
        std::filesystem::path pth_steam_cellar = pth_library / "steamapps/compatdata";

        for (auto const& itm_appid : std::filesystem::directory_iterator(pth_steam_cellar)) {
            auto pth_appid = itm_appid.path();
            if (std::filesystem::is_directory(pth_appid / "pfx") && ! std::filesystem::is_empty(pth_appid / "pfx")) {
                std::string str_appid = pth_appid.filename().string(); // should become, e.g. 1124300
                auto pth_appmanifest = pth_library / ("steamapps/appmanifest_" + str_appid + ".acf");
                if (! std::filesystem::exists(pth_appmanifest) ) { continue; }
                std::string str_gamename = "";

                std::ifstream fd_appmanifest(pth_appmanifest);
                auto hnd_appmanifest = vdf::read(fd_appmanifest);
                for (auto hnd_appmanifest_def : hnd_appmanifest.attribs) {
                    std::string str_index = hnd_appmanifest_def.first;
                    std::string str_value = hnd_appmanifest_def.second;

                    if (str_index == "name") {
                        str_gamename = str_value;
                        break;
                    }
                }

                auto curbottle = cellar::bottles::Bottle((pth_appid / "pfx").string());
                curbottle.set_config("name", str_gamename);
                curbottle.save_config();
                result[std::string("steam:" + pth_appid.filename().string())] = curbottle;
            }
        }
    }

    return result;
}

cellar::bottles::Bottle cellar::steam::app_bottle(unsigned appid) {
    string str_appid = std::to_string(appid);
    string str_prefix = std::string("steam:") + str_appid;
    auto steambottles = get_app_bottles();
    if (steambottles.find(str_prefix) == steambottles.end()) {
        throw std::range_error("steam is not currently managing a valid prefix for " + std::to_string(appid));
    }

    return steambottles.at(str_prefix);
}