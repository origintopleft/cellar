#ifndef __BOTTLES_HPP
#define __BOTTLES_HPP
#pragma once

#include <map>
#include <string>

#include "commands.hpp"
#include "nlohmann/json.hpp"

using namespace std;

using json = nlohmann::json;

namespace cellar {
	namespace bottles {
        enum bottle_type {
            bottle_error,
            bottle_anonymous,
            bottle_labelled,
            bottle_symlink,
            bottle_steam
        };

        /**
         * Bottles are the internal name for WINE prefixes. In addition to standard WINE contents,
         * bottles contain a configuration file managed by cellar, which labels the bottle as well
         * as providing configuration settings.
         */
        class Bottle {
            public:
                // public members
                bottle_type type;
                json config;
                string path;
                string canonical_path;
                
                // constructors
                Bottle();
                Bottle(string);
                
                // methods
                bool load_config();
                bool save_config();
                string get_config(string);
                bool set_config(string, string);
        };
        
        extern Bottle active_bottle;
    
		extern map<string, Bottle> get_bottles();
        extern string resolve_bottle(string);

        extern void cork(string, bool);
        extern void press(string, vector<string>, bool);
        extern void uncork(string);
	}
}

#endif // __BOTTLES_HPP
