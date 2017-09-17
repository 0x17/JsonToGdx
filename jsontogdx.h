//
// Created by andre on 13.09.17.
//

#pragma once

#include <gams.h>
#include "json11.hpp"

namespace jsontogdx {
	void addDataFromJson(gams::GAMSDatabase &db, const json11::Json &obj);
	void addSetsFromJson(gams::GAMSDatabase &db, const json11::Json &sets);
	void addParametersFromJson(gams::GAMSDatabase &db, const json11::Json &params);
	void addScalarsFromJson(gams::GAMSDatabase &db, const json11::Json &scalars);
	void addSet(gams::GAMSDatabase &db, const std::string &name, int from, int to, const std::string &description = "");

	gams::GAMSWorkspace writeJsonObjToGdxFile(const json11::Json &obj, const std::string &gdxFilename);
	json11::Json readJsonObjFromGdxFile(const std::string &gdxFilename);

	gams::GAMSWorkspace writeJsonStrToGdxFile(const std::string &jsonStr, const std::string &gdxFilename);
	std::string readJsonStrFromGdxFile(const std::string &gdxFilename);
}

extern "C" {
    void setGAMSDirectories(const char *systemDirectory, const char *workingDirectory);
	void setGAMSOptions(const char *jsonStr);

    void writeJsonStrToGdxFile(const char *jsonStr, const char *gdxFilename);
	const char *readJsonStrFromGdxFile(const char *gdxFilename);

    const char *solveModelWithDataJsonStr(const char *modelCode, const char *jsonStr);
	const char *solveModelWithDataJsonStrAndOptions(const char *modelCode, const char *jsonStr, const char *optionsJsonStr);
}
