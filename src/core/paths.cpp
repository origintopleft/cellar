#include <exception>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <regex>

#include <boost/algorithm/string.hpp>
#include "tclap/CmdLine.h"

#include "bottles.hpp"
#include "output.hpp"
#include "paths.hpp"
#include "version.hpp"
#include "internal/core.hpp"

using namespace std;
using namespace cellar;

string cellar::paths::translate(std::string in_path, bool lazy) {
    bool windows_input;

    static regex drive_letter_rgx(R"([a-zA-Z]:\\)");

    windows_input = regex_match(in_path.substr(0, 3), drive_letter_rgx);

    if (windows_input) {
        if (lazy) {
            if (boost::algorithm::to_lower_copy(in_path.substr(0,1)) != "z") {
                throw invalid_argument("lazy path translation isn't possible for drive letters other than Z:");
            }
            string out_path = in_path.substr(2, in_path.size() - 2);
            
            size_t slashpos = out_path.find("\\");
            while (slashpos != std::string::npos) {
                out_path.replace(slashpos, 1, "/");
                slashpos = out_path.find("\\");
            }

            return out_path;
        } else {
            return paths::resolve_drive_letter(in_path);
        }
    } else {
        string out_path;
        string str_absolutepath = filesystem::canonical(in_path);

        if (lazy) {
            out_path = "Z:";
            out_path.append(str_absolutepath);

            size_t slashpos = out_path.find("/");
            while (slashpos != std::string::npos) {
                out_path.replace(slashpos, 1, "\\");
                slashpos = out_path.find("/");
            }
        } else {
            map<string, string> dct_drives;
            for (auto hnd_curitem : filesystem::directory_iterator(filesystem::path(bottles::active_bottle.canonical_path) / "dosdevices"))  {
                auto pth_curitem = hnd_curitem.path();
                dct_drives.insert_or_assign(pth_curitem.filename().string(), filesystem::canonical(pth_curitem));
            }

            for (auto hnd_curdrive : dct_drives) {
                size_t sz_drivelen = hnd_curdrive.second.length();
                size_t sz_findpos = in_path.rfind(hnd_curdrive.second, 0);
                if (sz_findpos == 0) {
                    out_path.append(hnd_curdrive.first);
                    out_path.append(in_path.substr(sz_drivelen));

                    size_t slashpos = out_path.find("/");
                    while (slashpos != std::string::npos) {
                        out_path.replace(slashpos, 1, "\\");
                        slashpos = out_path.find("/");
                    }

                    break;
                }
            }

            // if we're here, it's not on a drive letter. use Z:
            out_path = "Z:";
            out_path.append(str_absolutepath);

            size_t slashpos = out_path.find("/");
            while (slashpos != std::string::npos) {
                out_path.replace(slashpos, 1, "\\");
                slashpos = out_path.find("/");
            }
        }

        return out_path;
    }
}

void cellar::core::translate_command(int argc, vector<string> argv) {
    TCLAP::CmdLine cmdparse("Translate a path from Windows to UNIX, or vice versa", ' ', version::short_version(), false);
    
    TCLAP::SwitchArg lazyarg("l", "lazy", "Lazy path translating - don't read symlinks, just assume / is mounted on Z:");
    cmdparse.add(lazyarg);

    TCLAP::UnlabeledMultiArg<string> pathargcollector("path", "Paths to translate.", true, "PATH");
    cmdparse.add(pathargcollector);

    cmdparse.parse(argv);

    bool lazy = lazyarg.getValue();
    vector<string> pathargs = pathargcollector.getValue();

    for (auto arg : pathargs) {
        cout << paths::translate(arg, lazy) << endl;
    }
}
