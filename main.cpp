#include "jsontogdx.h"
#include <iostream>

int main(int argc, char **argv) {
    json11::Json obj = loadJsonFromFile("example.json");
    gams::GAMSWorkspaceInfo wsInfo;
    wsInfo.setSystemDirectory("/home/andre/Downloads/gams24.9_linux_x64_64_sfx/");
    wsInfo.setWorkingDirectory("/home/andre/Desktop/");
    gams::GAMSWorkspace ws(wsInfo);
    auto db = ws.addDatabase("MyDatabase");
    addSetsFromJson(db, obj["sets"]);
    //addSet(db, "j", 1, 10, "jobs");
    db.doExport("some.gdx");
    return 0;
}