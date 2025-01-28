#include <cstdlib>
#include <string>
#include <vector>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <tclap/CmdLine.h>

#include "bottles.hpp"
#include "cellar.hpp"
#include "internal/bottles.hpp"
#include "internal/launch.hpp"
#include "fs.hpp"
#include "output.hpp"

using namespace std;
using namespace cellar;

void cellar::bottles::create_bottle(int argc, vector<string> argv) {
    TCLAP::CmdLine cmdparse("create a new WINE bottle", ' ', "", false);

    TCLAP::ValueArg<string> namearg("n", "name", "Friendly name for the bottle.", false, "", "name");
    cmdparse.add(namearg);

    TCLAP::ValueArg<string> descarg("d", "desc", "Short (or longer) description for the bottle.", false, "", "desc");
    cmdparse.add(descarg);

    TCLAP::UnlabeledValueArg<string> bottlearg("bottle", "Bottle name. Follows the rules of -b.", true, "", "bottle");
    cmdparse.add(bottlearg);

    cmdparse.parse(argv);

    // sanity check: make sure ~/.local/share/cellar/bottles exists
    string homepath = getenv("HOME");
    if (!boost::filesystem::exists(homepath + "/.local/share/cellar/bottles")) { boost::filesystem::create_directories(homepath + "/.local/share/cellar/bottles"); }

    string bottlechoice = bottlearg.getValue();
    string fullbottlepath;
    if (bottlechoice.substr(0,1) == "/" || bottlechoice.substr(0,1) == ".") { // absolute or relative path
        fullbottlepath = bottlechoice;
    } else if (bottlechoice.substr(0,1) == "~") { // "absolute" path in home directory, not expanded by the shell for some reason (i've seen some shit)
        // this is a naive replacement and will fail if the user tries something like ~nicole/.wine
        // i'm figuring at that point if you're doing that, you'll also recognize if your shell
        // isn't actually expanding your path...
        bottlechoice.replace(0,1,getenv("HOME"));
        // or at least you'll think to use verbose mode to make sure it's loading the right directory
        output::warning("your shell didn't expand your given path properly, doing a naive replacement", true);
        fullbottlepath = bottlechoice;
    } else {
        fullbottlepath = homepath + "/.local/share/cellar/bottles/" + bottlechoice;
    }

    if (boost::filesystem::exists(fullbottlepath)) {
        output::error(bottlechoice + " already exists");
        return;
    }

    if (fullbottlepath != homepath + "/.local/share/cellar/bottles/template") {
        output::statement("copying template bottle to " + fullbottlepath, true);

        // all this gets skipped if we're creating the template bottle
        if (boost::filesystem::exists(homepath + "/.local/share/cellar/bottles/template")) {
            if (!dryrun) {
                fs::recursive_copy(homepath + "/.local/share/cellar/bottles/template", fullbottlepath);
                return;
            }
        } else {
            output::error("no template bottle, create one using \"cellar create template\"");
            return;
        }
    } else {
        if (boost::filesystem::exists(homepath + "/.wine")) {
            output::statement("using existing bottle at ~/.wine as template bottle");
            if (!dryrun) { fs::recursive_copy(homepath + "/.wine", fullbottlepath); }
        } else {
            output::statement("creating template bottle from scratch");
            if (!dryrun) {
                setenv("WINEPREFIX", fullbottlepath.c_str(), 1);
                vector<string> wineboot;
                wineboot.push_back("wineboot"); wineboot.push_back("-u");
                launch::popen(wineboot);
            }
        }
    }
}
