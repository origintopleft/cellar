#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include <sstream>
#include <vector>

#include <boost/filesystem.hpp>
#include "nlohmann/json.hpp"

#include "bottles.hpp"
#include "cmake.hpp"
#include "internal/bottles.hpp"
#include "fs.hpp"
#include "output.hpp"
#ifdef ENABLE_STEAM
#include "steam.hpp"
#endif

using namespace std;
using namespace cellar;
using namespace cellar::bottles;

using CommandFunction = cellar::commands::CommandFunction;
using json = nlohmann::json;

/**
 * @brief Construct an empty bottle object.
 * This empty bottle object can then be used to help create new ones.
 */
Bottle::Bottle() {
    // define a null bottle
    // strings handle themselves
    config = json({});
    type = bottle_anonymous;
    manager = manager_error;
}

std::string cellar::bottles::bottle_home;

/**
 * @brief Sets up bottle home.
 * Called once from early mainloop. 
 * @todo Have this not be an effectively hardcoded path. Respect xdg paths.
 */
void cellar::bottles::setup_bottle_home() {
    stringstream sstr_bottle_home;
    sstr_bottle_home << std::getenv("HOME");
    sstr_bottle_home << "/.local/share/cellar/bottles";
    bottle_home = sstr_bottle_home.str();
}

/**
 * @brief Construct a bottle object from at a given path.
 * 
 * @param patharg The path to load a bottle from.
 */
Bottle::Bottle(string patharg) {
    output::statement("loading bottle from " + patharg, true);
    config = json({});
    path = patharg;
    
    //boost::filesystem::file_status path_status = boost::filesystem::symlink_status(path);
    //bool symlink = boost::filesystem::is_symlink(path_status);
    auto path_status = std::filesystem::path(path);
    auto path_canon = std::filesystem::canonical(path_status);
    canonical_path = path_canon.string();

    if (std::filesystem::is_symlink(path_canon)) {
        type = bottle_symlink;
    } else {
		try {
            load_config();
            auto cur_manager = get_config("manager");
            if (cur_manager == "cellar") {
                manager = manager_cellar;

                if (get_config("name") != "") {
                    type = bottle_labelled;
                } else {
                    type = bottle_anonymous;
                }
            } else if (path_canon.parent_path() == bottle_home) {
                manager = manager_cellar;
                set_config("manager", "cellar"); // migrate from older cellar (or correct for something weird happening)
                save_config();

                if (get_config("name") != "") {
                    type = bottle_labelled;
                } else {
                    type = bottle_anonymous;
                }
            } else if (cur_manager == "steam") {
                type = bottle_steam;
                manager = manager_steam;
            }
    	}
		catch (const exception &exc) {
            type = bottle_error;
		}
    }
}

/**
 * @brief Lists all bottles cellar manages or is otherwise aware of.
 * This includes bottles managed by other tools like Steam or Lutris,
 * assuming support for those tools is compiled into cellar. (Steam
 * support is present; Lutris support is planned for the future.)
 * 
 * @return map<string, Bottle> All bottles. Bottles managed by other tools have prefixed keys, e.g. Steam bottles use "steam:"
 */
map<string, Bottle> cellar::bottles::get_bottles() {
	map<string, Bottle> result;

	string homepath = getenv("HOME");
	vector<string> homedir = fs::listdir(homepath + "/.local/share/cellar/bottles");
	for (string item : homedir) {
        string fullitem = homepath + "/.local/share/cellar/bottles/" + item;
        Bottle output(fullitem);

	    result[item] = output;
	}

#ifdef ENABLE_STEAM
    map<string, Bottle> bottles_proton = cellar::steam::get_app_bottles();
    for (auto item : bottles_proton) {
        result.insert_or_assign(item.first, item.second);
    }
#endif

	return result;
}

/**
 * @brief Takes an input that refers to a bottle and returns a path to that bottle.
 * This input can be any of the following:
 *  * A bottle name as managed by cellar
 *  * A prefixed bottle name, in the format `<tool>:<identifier>`. (e.g. `steam:78000`)
 *  * An absolute or relative path, in which case it is returned with no modification.
 *  * A path relative to (and starting with ~). cellar will throw a warning in verbose mode,
 *    since usually the shell expands this for us, but will do a naive replacement on its own
 *    and try to prevent confused users.
 * 
 * @param bottlechoice Input, usually referring to a specific bottle known to cellar.
 * @return string Path to referenced bottle.
 */
string cellar::bottles::resolve_bottle(string bottlechoice) {
    string result;
    if (bottlechoice.substr(0,1) == "/" || bottlechoice.substr(0,1) == ".") { // absolute or relative path
        result = bottlechoice;
    } else if (bottlechoice.substr(0,1) == "~") { // "absolute" path in home directory, not expanded by the shell for some reason (i've seen some shit)
        // this is a naive replacement and will fail if the user tries something like ~nicole/.wine
        // i'm figuring at that point if you're doing that, you'll also recognize if your shell
        // isn't actually expanding your path...
        bottlechoice.replace(0,1,getenv("HOME"));
        // or at least you'll think to use verbose mode to make sure it's loading the right directory
        output::warning("your shell didn't expand your given path properly, doing a naive replacement", true);
        result = bottlechoice;
#ifdef ENABLE_STEAM
    } else if (bottlechoice.substr(0,6) == "steam:") { // steam bottles
        string str_appid = bottlechoice.substr(6);
        unsigned long uint_appid = std::stoul(str_appid);
        auto steambottle = cellar::steam::app_bottle(uint_appid);
        result = steambottle.path;
#endif
    } else {
        string homepath = getenv("HOME");
        string fullbottlepath = homepath + "/.local/share/cellar/bottles/" + bottlechoice;
        result = fullbottlepath;
    }
    return result;
}

/**
 * @brief Prints bottles. Used as a command in CLI.
 */
void cellar::bottles::print_bottles(int argc, vector<string> argv) {
    map<string, Bottle> bottles = get_bottles();

    stringstream outstr;

    for (auto item : bottles) {
        if (item.first == ".wine" || item.first == ".local/share/cellar/bottles/template") {
            // .wine is considered to be "active", and .local/share/cellar/bottles/template is used as a template
            // and therefore treated specially
            continue;
        }
        Bottle bottle = item.second;
        outstr << item.first << " - ";
        switch (bottle.type) {
            case bottle_anonymous:
                outstr << "anonymous wine bottle";
                break;
            case bottle_symlink:
                outstr << "symlink to " << bottle.canonical_path;
                break;
            case bottle_labelled:
                outstr << bottle.config["name"];
                break;
            case bottle_steam:
                outstr << "Steam managed bottle for " << bottle.config["name"];
                break;
            default:
                outstr << "broken or unsupported wine bottle";
        }
        output::statement(outstr.str());
        outstr.str("");
    }
}
