#include "jsontogdx.h"
#include <iostream>
#include <fstream>

std::string slurp(const std::string & filename) {
    std::ifstream t(filename);
    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    return str;
}

int main(int argc, char **argv) {
    std::string _sysDir = "/home/andre/Downloads/gams24.9_linux_x64_64_sfx/";
    std::string _wd = "/home/andre/Desktop/";

    {
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

    {
        setGAMSDirectories(_sysDir.c_str(), _wd.c_str());
        std::string modelCode = slurp("knapsack.gms");
        std::string jss = slurp("knapsack_data.json");
        const char *resstr = solveModelWithDataJsonStr(modelCode.c_str(), jss.c_str());
        std::cout << resstr;
    }

    return 0;
}