#include <cstdlib>
#include <filesystem>
#include <format>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "vdf_parser.hpp"

#include "bottles.hpp"
#include "output.hpp"
#include "steam.hpp"
#include "internal/steam.hpp"

using namespace tyti; // vdf

void cellar::steam::test_command(int argc, std::vector<std::string> argv) {
    for (std::string str_path_library : cellar::steam::find_steam_libraries()) {
        output::statement(str_path_library);
        
        std::filesystem::path pth_library(str_path_library);
        std::filesystem::path pth_steam_cellar = pth_library / "steamapps/compatdata";
        for (auto const& itm_appid : std::filesystem::directory_iterator(pth_steam_cellar)) {
            auto pth_appid = itm_appid.path();
            if (std::filesystem::is_directory(pth_appid / "pfx") && ! std::filesystem::is_empty(pth_appid / "pfx")) {
                //                            \/ string-to-unsigned-long
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

                output::warning("Steam App #" + str_appid + " (" + str_gamename + ")");
            }
        }
    }
}