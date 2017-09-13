//
// Created by andre on 13.09.17.
//

#pragma once

#include <gams.h>
#include "json11.hpp"

void addDataFromJson(gams::GAMSDatabase &db, const json11::Json &obj);

void addSetsFromJson(gams::GAMSDatabase &db, const json11::Json &sets);
void addParametersFromJson(gams::GAMSDatabase &db, const json11::Json &params);
void addScalarsFromJson(gams::GAMSDatabase &db, const json11::Json &scalars);

void addSet(gams::GAMSDatabase &db, const std::string &name, int from, int to, const std::string &description = "");

json11::Json loadJsonFromFile(const std::string &filename);
json11::Json loadJsonFromString(const std::string &s);

extern "C" {
    void setGAMSDirectories(const char *systemDirectory, const char *workingDirectory);
    void writeJsonStrToGdxFile(const char *jsonStr, const char *gdxFilename);
		const char *solveModelWithDataJsonStr(const char *modelCode, const char *gdxFilename, const char *jsonStr);
}