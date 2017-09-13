//
// Created by andre on 13.09.17.
//

#include "jsontogdx.h"

#include <iostream>
#include <fstream>
#include <gams.h>
#include "json11.hpp"

using namespace std;
using namespace json11;
using namespace gams;

json11::Json loadJsonFromFile(const std::string &filename) {
    ifstream ifs(filename);
    string content((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
    string error;
    json11::Json obj = json11::Json::parse(content, error);
    if(!error.empty()) {
        cout << "Parse error: " << error << "!" << endl;
    }
    return obj;
}

void addSet(GAMSDatabase &db, const string &name, int from, int to, const string &description) {
    auto s = db.addSet(name, description.empty() ? name : description);
    for(int j=from; j<=to; j++)
        s.addRecord(name + to_string(j));
}

void addSetsFromJson(GAMSDatabase &db, const Json &sets) {
    for(const auto &s : sets.array_items()) {
        if(s["from"].is_number() && s["to"].is_number()) {
            string descr = s["description"].is_string() ? s["description"].string_value() : "";
            addSet(db, s["name"].string_value(), s["from"].int_value(), s["to"].int_value(), descr);
        }
    }
}

static string sysDir, workDir;

void writeJsonStrToGdxFile(const char *jsonStr, const char *gdxFilename) {
    string jss(jsonStr), gdxfn(gdxFilename);
    Json obj = loadJsonFromString(jss);
    gams::GAMSWorkspaceInfo wsInfo;
    wsInfo.setSystemDirectory(sysDir);
    wsInfo.setWorkingDirectory(workDir);
    gams::GAMSWorkspace ws(wsInfo);
    auto db = ws.addDatabase("MyDatabase");
    addDataFromJson(db, obj);
    db.doExport(gdxfn);
}

void addDataFromJson(GAMSDatabase &db, const Json &obj) {
    addSetsFromJson(db, obj["sets"]);
}

void setGAMSDirectories(const char *systemDirectory, const char *workingDirectory) {
    sysDir = systemDirectory;
    workDir = workingDirectory;
}

json11::Json loadJsonFromString(const std::string &s) {
    string err;
    json11::Json obj = json11::Json::parse(s, err);
    if(!err.empty()) {
        cout << "Parse error: " << err << "!" << endl;
    }
    return obj;
}




