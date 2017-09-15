#include <iostream>
#include "jsontogdx.h"
#include "utils.hpp"
#include "examples.hpp"

static std::string _sysDir = "/home/andre/Downloads/gams24.9_linux_x64_64_sfx/";
static std::string _wd = "/home/andre/Desktop/";

void examples::knapsackExample() {
    setGAMSDirectories(_sysDir.c_str(), _wd.c_str());
    std::string modelCode = utils::slurp("knapsack.gms");
    std::string jss = utils::slurp("knapsack_data.json");
    const char *resstr = solveModelWithDataJsonStr(modelCode.c_str(), jss.c_str());
    std::cout << resstr;
}

void examples::rcpspRocExample() {

}

void examples::writeGdxExample() {
    json11::Json obj = loadJsonFromFile("example.json");
    gams::GAMSWorkspaceInfo wsInfo;
    wsInfo.setSystemDirectory(_sysDir);
    wsInfo.setWorkingDirectory(_wd);
    gams::GAMSWorkspace ws(wsInfo);
    auto db = ws.addDatabase("MyDatabase");
    addDataFromJson(db, obj);
    //addSetsFromJson(db, obj["sets"]);
    //addSet(db, "j", 1, 10, "jobs");
    db.doExport("some.gdx");
}