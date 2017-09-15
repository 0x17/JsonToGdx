//
// Created by andre on 15.09.17.
//

#include "utils.hpp"
#include "json11.hpp"
#include <fstream>
#include <iostream>

using namespace std;

std::string utils::slurp(const std::string & filename) {
    std::ifstream t(filename);
    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    return str;
}

json11::Json utils::loadJsonFromString(const std::string &s) {
    std::string err;
    json11::Json obj = json11::Json::parse(s, err);
    if(!err.empty()) {
        std::cout << "Parse error: " << err << "!" << std::endl;
        return json11::Json::object {};
    }
    return obj;
}

json11::Json utils::loadJsonFromFile(const std::string &filename) {
    ifstream ifs(filename);
    string content((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
    string error;
    json11::Json obj = json11::Json::parse(content, error);
    if(!error.empty()) {
        cout << "Parse error: " << error << "!" << endl;
        return json11::Json::object {};
    }
    return obj;
}
