#include <cstdlib>
#include <map>
#include <string>
#include <vector>

#include "bottles.hpp"
#include "internal/bottles.hpp"
#include "output.hpp"

using namespace std;
using namespace cellar::bottles;

void cellar::bottles::print_active_bottle(int argc, vector<string> argv) {
    map<string, Bottle> bottlemap = get_bottles();

    Bottle active_bottle = Bottle(getenv("HOME") + string("/.wine"));
    string bottlepath = active_bottle.canonical_path;
    stringstream outstr;
    bool cellar_managed = true;
    bool external_managed = false;
    if (active_bottle.type == bottle_symlink) {
        outstr << "symlink to ";
        string homedir = getenv("HOME");
        string bottlerack = homedir + "/.local/share/cellar/bottles";
        if (active_bottle.canonical_path.substr(0, bottlerack.length()) == bottlerack) {
            bottlepath.replace(0, bottlerack.length() + 1, ""); // should convert "/home/someone/.wine.example" to ".wine.example"
            active_bottle = bottlemap[bottlepath];
            active_bottle.set_config("manager", "cellar");
            active_bottle.save_config();
        } else {
            outstr << active_bottle.canonical_path;
            cellar_managed = false;
        }
    }

    if (cellar_managed) {
        switch (active_bottle.type) {
            case bottle_anonymous:
                outstr << "anonymous wine bottle at " << active_bottle.canonical_path;
                break;
            case bottle_labelled:
                outstr << active_bottle.config["name"] << " (" << bottlepath << ")";
                if (active_bottle.config.find("desc") != active_bottle.config.end()) {
                    outstr << " - " << active_bottle.config["desc"];
                }
                break;
            default:
                outstr << "broken or unsupported wine bottle";
                break;
        }
    }

    output::statement(outstr.str());
}
