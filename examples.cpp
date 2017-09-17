#include <iostream>
#include "jsontogdx.h"
#include "utils.hpp"
#include "examples.hpp"

#ifdef __APPLE__
static std::string _sysDir = "/Applications/GAMS24.9/sysdir/";
static std::string _wd = "/Users/andreschnabel/Desktop";
#else
static std::string _sysDir = "/home/andre/Downloads/gams24.9_linux_x64_64_sfx/";
static std::string _wd = "/home/andre/Desktop/";
#endif

void examples::knapsackExample() {
    setGAMSDirectories(_sysDir.c_str(), _wd.c_str());
    std::string modelCode = utils::slurp("knapsack.gms");
    std::string jss = utils::slurp("knapsack_data.json");
    const char *resstr = solveModelWithDataJsonStr(modelCode.c_str(), jss.c_str());
    std::cout << resstr;
}

void examples::rcpspRocExample() {
    setGAMSDirectories(_sysDir.c_str(), _wd.c_str());
    std::string modelCode = utils::slurp("rcpsproc.gms");
    std::string jss = utils::slurp("rcpsproc_data.json");
    const char *resstr = solveModelWithDataJsonStr(modelCode.c_str(), jss.c_str());
    std::cout << resstr;
}

void examples::writeGdxExample() {
    setGAMSDirectories(_sysDir.c_str(), _wd.c_str());
    jsontogdx::writeJsonStrToGdxFile(utils::slurp("example.json"), "some.gdx");
}

void examples::readGdxExample() {
    setGAMSDirectories(_sysDir.c_str(), _wd.c_str());
    jsontogdx::writeJsonStrToGdxFile(utils::slurp("example.json"), "some.gdx");
    std::cout << jsontogdx::readJsonStrFromGdxFile("some.gdx") << std::endl;
}