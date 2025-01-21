#include <iostream>

#include "output.hpp"
#include "steam.hpp"
#include "internal/steam.hpp"

void cellar::steam::test_command(int argc, std::vector<std::string> argv) {
    for (std::string str_path : cellar::steam::find_steam_libraries()) {
        output::statement(str_path);
    }
}