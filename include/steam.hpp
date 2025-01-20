#pragma once

#include "bottles.hpp"

namespace cellar {
    namespace steam {
        extern cellar::bottles::Bottle app_bottle(unsigned appid);
    }
}