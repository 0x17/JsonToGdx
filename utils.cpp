//
// Created by andre on 15.09.17.
//

#include "utils.hpp"
#include <fstream>

std::string utils::slurp(const std::string & filename) {
    std::ifstream t(filename);
    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    return str;
}