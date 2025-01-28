#include <cstdlib>
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

Bottle::Bottle() {
    // define a null bottle
    // strings handle themselves
    config = json({});
    type = bottle_anonymous;
}

Bottle::Bottle(string patharg) {
    output::statement("loading bottle from " + patharg, true);
    config = json({});
    path = patharg;
    
    boost::filesystem::file_status path_status = boost::filesystem::symlink_status(path);
    bool symlink = boost::filesystem::is_symlink(path_status);

    if (symlink) {
        boost::filesystem::path realpath = boost::filesystem::canonical(path);
        canonical_path = realpath.string();
        type = bottle_symlink;
    } else {
        canonical_path = path;
		try {
            if (load_config()) {
                type = bottle_labelled;
            } else {
                type = bottle_anonymous;
            }
    	}
		catch (const exception &exc) {
            type = bottle_error;
		}
    }
}

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

string cellar::bottles::resolve_bottle(string bottlechoice) {
    string result;
    if (bottlechoice.substr(0,1) == "/" || bottlechoice.substr(0,1) == ".") { // absolute or relative path
        result = bottlechoice;
    } else if (bottlechoice.substr(0,1) == "~") { // "absolute" path in home directory, not expanded by the shell for some reason (i've seen some shit)
        // this is a naive replacement and will fail if the user tries something like ~nick/.wine
        // i'm figuring at that point if you're doing that, you'll also recognize if your shell
        // isn't actually expanding your path...
        bottlechoice.replace(0,1,getenv("HOME"));
        // or at least you'll think to use verbose mode to make sure it's loading the right directory
        output::warning("your shell didn't expand your given path properly, doing a naive replacement", true);
        result = bottlechoice;
    } else {
        string homepath = getenv("HOME");
        string fullbottlepath = homepath + "/.local/share/cellar/bottles" + bottlechoice;
        result = fullbottlepath;
    }
    return result;
}

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
            default:
                outstr << "broken or unsupported wine bottle";
        }
        output::statement(outstr.str());
        outstr.str("");
    }
}
