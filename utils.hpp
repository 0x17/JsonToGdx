//
// Created by andre on 15.09.17.
//

#pragma once

#include <string>
#include "json11.hpp"

namespace utils {
    std::string slurp(const std::string & filename);
    json11::Json loadJsonFromString(const std::string &s);
    json11::Json loadJsonFromFile(const std::string &filename);
};
