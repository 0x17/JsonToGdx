//
// Created by andre on 13.09.17.
//

#include "jsontogdx.h"

#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include "utils.hpp"
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace json11;
using namespace gams;

void jsontogdx::addSet(GAMSDatabase &db, const string &name, int from, int to, const string &description) {
    auto s = db.addSet(name, description.empty() ? name : description);
    for(int j=from; j<=to; j++)
        s.addRecord(name + to_string(j));
}

void jsontogdx::addSetsFromJson(GAMSDatabase &db, const Json &sets) {
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
        } else if(s["elements"].is_array()) {
            int dim = s["dim"].is_number() ? (int)s["dim"].number_value() : 1;
            auto myset = db.addSet(s["name"].string_value(), dim);
            switch(dim) {
                case 1:
                    for(const auto &element : s["elements"].array_items()) {
                        myset.addRecord(element.string_value()).setText("yes");
                    }
                    break;
                case 2:
                    for(const auto &pairstr : s["elements"].array_items()) {
						vector<string> parts;
						boost::split(parts, pairstr.string_value(), boost::is_any_of(","));
                        myset.addRecord(parts[0], parts[1]).setText("yes");
                    }
                    break;
                default:
                    break;
            }
        }
    }
}

void jsontogdx::addParametersFromJson(gams::GAMSDatabase &db, const json11::Json &params) {
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

void jsontogdx::addScalarsFromJson(gams::GAMSDatabase &db, const json11::Json &scalars) {
    for(const auto &scalar : scalars.array_items()) {
        auto param = db.addParameter(scalar["name"].string_value(), 0);
        param.addRecord().setValue(scalar["value"].number_value());
    }
}

void jsontogdx::addDataFromJson(GAMSDatabase &db, const Json &obj) {
	addSetsFromJson(db, obj["sets"]);
	addParametersFromJson(db, obj["parameters"]);
	addScalarsFromJson(db, obj["scalars"]);
}

// hidden
static string _sysDir, _workDir, _optionsJsonStr = "";

static gams::GAMSWorkspace _writeJsonStrToGdxFile(const std::string &jss, const std::string &gdxfn) {
	Json obj = utils::loadJsonFromString(jss);
	gams::GAMSWorkspaceInfo wsInfo;
	wsInfo.setSystemDirectory(_sysDir);
	wsInfo.setWorkingDirectory(_workDir);
	gams::GAMSWorkspace ws(wsInfo);
	auto db = ws.addDatabase("MyDatabase");
	jsontogdx::addDataFromJson(db, obj);
	db.doExport(gdxfn);
	return ws;
}

static string fixComma(double v) {
	return boost::replace_first_copy(to_string(v), ",", ".");
}

// FFI accessible
void writeJsonStrToGdxFile(const char *jsonStr, const char *gdxFilename) {
    string jss(jsonStr), gdxfn(gdxFilename);
    _writeJsonStrToGdxFile(jss, gdxfn);
}

void setGAMSDirectories(const char *systemDirectory, const char *workingDirectory) {
    _sysDir = systemDirectory;
    _workDir = workingDirectory;
}

void setGAMSOptions(const char *jsonStr) {
	_optionsJsonStr = jsonStr;
}

json11::Json gamsVarToJsonObject(GAMSVariable &v) {
	if(v.dim() == 0) {
		return json11::Json::object { {"name", v.name() }, {"level", fixComma(v.firstRecord().level()) } };
	} else if(v.dim() == 1) {
		vector<json11::Json> levels;
		for(auto rec : v) {
			levels.push_back(Json::object { {rec.key(0), fixComma(rec.level()) } });
		}
		return json11::Json::object { {"name", v.name() }, {"levels", levels } };
	} else if(v.dim() == 2) {
		vector<json11::Json> levels;
		for(auto rec : v) {
			levels.push_back(Json::object { {rec.key(0)+","+rec.key(1), fixComma(rec.level()) } });
		}
		return json11::Json::object { {"name", v.name() }, {"levels", levels } };
	}
	return json11::Json::object {};
}

json11::Json extractResultsFromOutDatabase(GAMSDatabase &outDB) {
    vector<json11::Json> jsvars;
    for(auto sym : outDB) {
        if(sym.type() == GAMSEnum::SymTypeVar) {
			auto v = (GAMSVariable)sym;
            jsvars.push_back(gamsVarToJsonObject(v));
        }
    }
    json11::Json oobj = json11::Json::object{{"results", jsvars}};
    return oobj;
}

void transferOptionsFromJson(GAMSOptions &options, const Json &obj) {
	map<string, void (GAMSOptions::*)(double)> dblSetters = {
			{"OptCR", &GAMSOptions::setOptCR},
			{"OptCA", &GAMSOptions::setOptCA},
			{"ResLim", &GAMSOptions::setResLim},
	};

	for(auto pair : dblSetters) {
		if(obj[pair.first].is_number()) {
			(options.*pair.second)(obj[pair.first].number_value());
		}
	}

	if(obj["Solver"].is_array()) {
		string modelType = obj["Solver"].array_items()[0].string_value();
		string solverName = obj["Solver"].array_items()[1].string_value();
		options.setSolver(modelType, solverName);
	}
}

const char *solveModelWithDataJsonStr(const char *modelCode, const char *jsonStr) {
	if(_sysDir.empty() || _workDir.empty()) return "";
	const string GDX_FILENAME = "data";
	string mc(modelCode), jss(jsonStr);
	auto ws = _writeJsonStrToGdxFile(jss, GDX_FILENAME+".gdx");
	auto job = ws.addJobFromString(mc);
	auto options = ws.addOptions();
	options.setDefine("gdxincname", GDX_FILENAME);
	options.setOptCR(0.0);
	if(!_optionsJsonStr.empty()) {
		transferOptionsFromJson(options, utils::loadJsonFromString(_optionsJsonStr));
	}
	job.run(options);
	auto outDB = job.outDB();
	string ostr = extractResultsFromOutDatabase(outDB).dump();
    return strdup(ostr.c_str());
}

Json gamsSetToJsonObject(GAMSSet &set) {
	// FIXME: Implement me!
	return Json::object {};
}

Json gamsParameterToJsonObject(GAMSParameter &param) {
	// FIXME: Implement me!
	return Json::object {};
}

string _readJsonStrFromGdxFile(const string &gdxFilename) {
	gams::GAMSWorkspaceInfo wsInfo;
	wsInfo.setSystemDirectory(_sysDir);
	wsInfo.setWorkingDirectory(_workDir);
	gams::GAMSWorkspace ws(wsInfo);
	auto db = ws.addDatabaseFromGDX(gdxFilename);

	GAMSSet myset;
	GAMSParameter myparam;
	GAMSVariable myvar;

	vector<Json> sets, params, scalars, vars;
	vector<Json> *oarr = nullptr;

	for(auto sym : db) {
		switch(sym.type()) {
			case GAMSEnum::SymTypeSet:
				myset = (GAMSSet)sym;
				sets.push_back(gamsSetToJsonObject(myset));
				break;
			case GAMSEnum::SymTypePar:
				myparam = (GAMSParameter)sym;
				oarr = myparam.dim() == 0 ? &scalars : &params;
				oarr->push_back(gamsParameterToJsonObject(myparam));
				break;
			case GAMSEnum::SymTypeVar:
				myvar = (GAMSVariable)sym;
				vars.push_back(gamsVarToJsonObject(myvar));
				break;
			default:
				break;
		}
	}

	Json obj = Json::object { {"sets", sets}, {"parameters", params}, {"scalars", scalars}, {"variables", vars} };
	return obj.dump();
}

const char *readJsonStrFromGdxFile(const char *gdxFilename) {
	string ostr = _readJsonStrFromGdxFile(gdxFilename);
	return strdup(ostr.c_str());
}

const char *solveModelWithDataJsonStrAndOptions(const char *modelCode, const char *jsonStr, const char *optionsJsonStr) {
	setGAMSOptions(optionsJsonStr);
	return solveModelWithDataJsonStr(modelCode, jsonStr);
}



