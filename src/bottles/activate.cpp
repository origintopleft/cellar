#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>

#include "bottles.hpp"
#include "cellar.hpp"
#include "internal/bottles.hpp"
#include "output.hpp"

using namespace std;
using namespace boost::filesystem;

void cellar::bottles::switch_active_bottle(int argc, vector<string> argv) {
    if (argc == 1) {
        output::error("forgot to specify a bottle to activate");
        return;
    }

    string homepath = getenv("HOME");
    string bottlepath = homepath + "/.wine";
    string targetpath = cellar::bottles::resolve_bottle(argv[1]);

    file_status targetstatus = symlink_status(targetpath);
    if (!exists(targetstatus)) {
        output::error("target " + targetpath + " does not exist");
        return;
    }

    file_status bottlestatus = symlink_status(bottlepath);
    if (exists(bottlestatus)) {
        bool bottlesymlink = is_symlink(bottlestatus);
        if (!bottlesymlink) {
            output::error("refusing to clobber " + bottlepath + ": not a symlink");
            return;
        }
        output::statement("removing existing symlink at " + bottlepath, true);
        if (!cellar::dryrun) { remove(bottlepath); }
    }

    output::statement("creating symlink to " + targetpath + " at " + bottlepath, true);
    if (cellar::dryrun) { return; }
    // TODO: not blindly assume this will magically work
    try {
        create_symlink(targetpath, bottlepath);
    } catch (filesystem_error &exc) {
        output::error((string) "Error creating symlink at ~/.wine: " + exc.what());
        return;
    }
}
