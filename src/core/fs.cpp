#include <iostream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>

#include "cellar.hpp"
#include "fs.hpp"
#include "output.hpp"

using namespace std;
using namespace boost;
using namespace cellar;
using namespace fs;

// these are not necessarily the final function signatures for callbacks
// this only exists so i don't have to come back and do the ductwork for
// callback functionality later
void cbnull_copy(string src, string dst) { return; }
void cbnull_remove(string src) { return; }

vector<string> fs::listdir(string path) {
	vector<string> result;
    filesystem::path cwd(path);
    filesystem::directory_iterator iter_end;

    for (filesystem::directory_iterator iter_cwd(cwd); iter_cwd != iter_end; ++iter_cwd) {
        string item = iter_cwd->path().filename().native();
		
		result.push_back(item);
    }
	return result;
}

bool fs::recursive_copy(string src, string dst, CopyCallbackFunc callback) {
    if (!filesystem::exists(dst)) {
        if (dryrun) { output::statement("mkdir:   " + dst); }
        else {
            bool success = filesystem::create_directory(dst);
            if (!success) { return false; }
        }
    }

    for (string itemrel : listdir(src)) {
        string itemabs = src + "/" + itemrel;
        string targetabs = dst + "/" + itemrel;

        auto itemstat = filesystem::symlink_status(itemabs);

        if (filesystem::is_directory(itemstat)) { recursive_copy(itemabs, targetabs, callback); }
        else if (filesystem::is_symlink(itemstat)) { 
            auto symlinkpath = filesystem::read_symlink(itemabs);
            if (dryrun) { output::statement("symlink: " + symlinkpath.native() + " => " + targetabs); }
            else { filesystem::create_symlink(symlinkpath, targetabs); }
        }
        else { 
            if (dryrun) { output::statement("copy:    " + itemabs + " => " + targetabs); }
            else { filesystem::copy(itemabs, targetabs); }
        }

        callback(itemabs, targetabs);
    }

    return true;
}

bool fs::recursive_copy(string src, string dst) { return recursive_copy(src, dst, cbnull_copy); }

bool fs::recursive_remove(string target, RemoveCallbackFunc callback) {
    if (!filesystem::exists(target)) { return false; }

    for (string itemrel : listdir(target)) {
        string itemabs = target + "/" + itemrel;

        auto itemstat = filesystem::symlink_status(itemabs);

        if (filesystem::is_directory(itemstat)) { recursive_remove(itemabs, callback); }
        else {
            if (dryrun) { output::error("rm:      " + itemabs); } 
            else { filesystem::remove(itemabs); }
        }

        callback(itemabs);
    }

    if (dryrun) { output::error("rm:      " + target); }
    else { filesystem::remove(target); }

    return true;
}

bool fs::recursive_remove(string target) { return recursive_remove(target, cbnull_remove); }
