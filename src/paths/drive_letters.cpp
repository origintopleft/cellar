#include <exception>
#include <filesystem>
#include <regex>
#include <string>
#include <unistd.h>

#include <boost/algorithm/string.hpp>

#include "bottles.hpp"
#include "output.hpp"
#include "paths.hpp"

using namespace std;

string cellar::paths::resolve_drive_letter(string in_path) {
    filesystem::path lastwd = filesystem::current_path();
    bool windows_input;
    static regex drive_letter_rgx(R"([a-zA-Z]:\\)");

    windows_input = regex_match(in_path.substr(0, 3), drive_letter_rgx);

    string out_path = "";

    if (windows_input) {
        string drive_letter = boost::algorithm::to_lower_copy(in_path.substr(0, 2));

        string link_path = "";
        link_path.append(bottles::active_bottle.canonical_path);
        link_path.append("/dosdevices/");
        filesystem::current_path(filesystem::path(link_path));
        link_path.append(drive_letter);
        
        char stringbuffer[512];
        ssize_t bufflen = readlink(link_path.c_str(), stringbuffer, sizeof(stringbuffer) - 1);

        if (bufflen != -1) {
            stringbuffer[bufflen] = '\0';
            string str_absolutepath = filesystem::canonical(stringbuffer);
            out_path.append(str_absolutepath);
            out_path.append("/");
        } else {
            throw runtime_error("readlink isn't having it");
        }

        string rest_of_path = in_path.substr(3, in_path.size() - 2);
        size_t slashpos = rest_of_path.find("\\");
        while (slashpos != std::string::npos) {
            rest_of_path.replace(0, 1, "/");
            slashpos = rest_of_path.find("\\");
        }

        out_path.append(rest_of_path);
    }

    filesystem::current_path(lastwd);
    return out_path;
}
