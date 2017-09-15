//
// Created by andre on 13.09.17.
//

#include "jsontogdx.h"

#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include "utils.hpp"

using namespace std;
using namespace json11;
using namespace gams;

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
        } else if(s["indices"].is_string()) {
            auto myset = db.addSet(s["name"].string_value(), 1);
            for(const auto &entry : s["values"].array_items()) {
                myset.addRecord(entry.string_value()).setText("yes");
            }
        } else if(s["indices"].is_array()) {
            if(s["indices"].array_items().size() == 2) {
                auto myset = db.addSet(s["name"].string_value(), 2);
                for(const auto &row : s["values"].array_items()) {
                    auto pair = row.array_items();
                    myset.addRecord(pair[0].string_value(), pair[1].string_value()).setText("yes");
                }
            }
        }
    }
}

void addParametersFromJson(gams::GAMSDatabase &db, const json11::Json &params) {
    for(const auto &param : params.array_items()) {
		auto name = param["name"].string_value();
		vector<string> indices;
		if(param["indices"].is_string()) indices.push_back(param["indices"].string_value());
		else if(param["indices"].is_array()) {
			for(const auto &index : param["indices"].array_items()) {
				indices.push_back(index.string_value());
			}
		} else continue;
        auto dim = static_cast<int>(indices.size());
		auto p = db.addParameter(name, dim);
        int rctr = 0, cctr = 0;
		switch(dim) {
            default:
            case 1:
                for(const auto &varrentry : param["values"].array_items()) {
                    p.addRecord(indices[0]+to_string(cctr+1)).setValue(varrentry.number_value());
                    cctr++;
                }
                break;
            case 2:
                for(const auto &row : param["values"].array_items()) {
                    cctr = 0;
                    for(const auto &entry : row.array_items()) {
                        p.addRecord(indices[0]+to_string(rctr+1), indices[1]+to_string(cctr+1)).setValue(entry.number_value());
                        cctr++;
                    }
                    rctr++;
                }
                break;
        }
    }
}

void addScalarsFromJson(gams::GAMSDatabase &db, const json11::Json &scalars) {
    for(const auto &scalar : scalars.array_items()) {
        auto param = db.addParameter(scalar["name"].string_value(), 0);
        param.addRecord().setValue(scalar["value"].number_value());
    }
}

static string sysDir, workDir;

gams::GAMSWorkspace _writeJsonStrToGdxFile(const std::string &jss, const std::string &gdxfn) {
    Json obj = utils::loadJsonFromString(jss);
    gams::GAMSWorkspaceInfo wsInfo;
    wsInfo.setSystemDirectory(sysDir);
    wsInfo.setWorkingDirectory(workDir);
    gams::GAMSWorkspace ws(wsInfo);
    auto db = ws.addDatabase("MyDatabase");
    addDataFromJson(db, obj);
    db.doExport(gdxfn);
    return ws;
}

void writeJsonStrToGdxFile(const char *jsonStr, const char *gdxFilename) {
    string jss(jsonStr), gdxfn(gdxFilename);
    _writeJsonStrToGdxFile(jss, gdxfn);
}

void addDataFromJson(GAMSDatabase &db, const Json &obj) {
    addSetsFromJson(db, obj["sets"]);
    addParametersFromJson(db, obj["parameters"]);
    addScalarsFromJson(db, obj["scalars"]);
}

void setGAMSDirectories(const char *systemDirectory, const char *workingDirectory) {
    sysDir = systemDirectory;
    workDir = workingDirectory;
}

string fixComma(double v) {
    string s = to_string(v);
    std::replace(s.begin(), s.end(), ',', '.');
    return s;
}

json11::Json extractResultsFromOutDatabase(GAMSDatabase &outDB) {
    vector<json11::Json> jsvars;
    for(auto sym : outDB) {
        GAMSVariable v;
        if(sym.type() == GAMSEnum::SymTypeVar) {
            v = (GAMSVariable)sym;
            if(v.dim() == 0) {
                jsvars.push_back(json11::Json::object { {"name", v.name() }, {"level", fixComma(v.firstRecord().level()) } });
            } else if(v.dim() == 1) {
                vector<json11::Json> levels;
                for(auto rec : v) {
                    levels.push_back(Json::object { {rec.key(0), fixComma(rec.level()) } });
                }
                jsvars.push_back(json11::Json::object { {"name", v.name() }, {"levels", levels } });
            } else if(v.dim() == 2) {
                vector<json11::Json> levels;
                for(auto rec : v) {
                    levels.push_back(Json::object { {rec.key(0)+","+rec.key(1), fixComma(rec.level()) } });
                }
                jsvars.push_back(json11::Json::object { {"name", v.name() }, {"levels", levels } });
            }
        }
    }
    json11::Json oobj = json11::Json::object{{"results", jsvars}};
    return oobj;
}

const char *solveModelWithDataJsonStr(const char *modelCode, const char *jsonStr) {
	const string GDX_FILENAME = "data";
	string mc(modelCode), jss(jsonStr);
	auto ws = _writeJsonStrToGdxFile(jss, GDX_FILENAME+".gdx");
	auto job = ws.addJobFromString(mc);
	auto options = ws.addOptions();
	options.setDefine("gdxincname", GDX_FILENAME);
	options.setOptCR(0.0);
	job.run(options);
	auto outDB = job.outDB();
	string ostr = extractResultsFromOutDatabase(outDB).dump();
    return strdup(ostr.c_str());
}





