/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <vector>
#include <json-c/json_tokener.h>
#include <json-c/json_object.h>
#include <sys/param.h>
#include "runconfig.h"
#include "util.h"
#include "data.h"
#include "data/openstate_data.h"
#include "enum.h"
#include "enum/openstate.h"

OpenStateData::OpenStateData() {}
OpenStateData::~OpenStateData() {}

// Field names for TSV
static const std::vector<std::string> field_names = {
	"id",
	"defaultState",
	"allowClientOpen",
	"cond1Type",
	"cond1Param1",
	"cond1Param2",
	"cond2Type",
	"cond2Param1",
	"cond2Param2",
	"systemOpenUiId",
};

int OpenStateData::load() {
	const config_t* config = globalConfig->getConfig();
	static char path[4096];
	int ret;
	path[4095] = '\0';
	// Try first with Grasscutter's name scheme.
	snprintf(path, 4095, "%s/ExcelBinOutput/OpenStateConfigData.json", config->dataPath);
	ret = load(path);
	if (ret >= 0) return ret;
	snprintf(path, 4095, "%s/ExcelBinOutput/OpenStateConfigData.tsv", config->dataPath);
	ret = load(path);
	if (ret >= 0) return ret;
	snprintf(path, 4095, "%s/ExcelBinOutput/OpenStateConfigData.txt", config->dataPath);
	ret = load(path);
	if (ret >= 0) return ret;
	// Try again with GIO's name scheme.
	snprintf(path, 4095, "%s/txt/OpenStateData.json", config->dataPath);
	ret = load(path);
	if (ret >= 0) return ret;
	snprintf(path, 4095, "%s/txt/OpenStateData.tsv", config->dataPath);
	ret = load(path);
	if (ret >= 0) return ret;
	snprintf(path, 4095, "%s/txt/OpenStateData.txt", config->dataPath);
	ret = load(path);
	if (ret >= 0) return ret;
	// Bail
	return -1;
}

int OpenStateData::load(const char* path) {
	OpenStateDataEnt* tmpEnt = {};
	FILE* fp = fopen(path, "rb");
	if (fp == NULL) {
		return -1;
	}
	const std::vector<excelTableEnt>* tbl;
	const char* ext = strrchr(path, '.');
	unsigned int is_json;
	// Check for file name ending in .json
	if (ext != NULL && strncmp(ext, ".json", 5) == 0) {
		tbl = loadJsonExcelData(fp);
		is_json = 1;
	}
	else {
		tbl = loadTsvExcelData(field_names, fp);
		is_json = 0;
	}
	if (tbl == NULL) {
		fclose(fp);
		return -1;
	}
	unsigned int i;
	unsigned int j;
	unsigned int e;
	std::string v;
	struct json_tokener* jtk = NULL;
	struct json_object* jobj;
	struct json_object* jobj_idx;
	struct json_object* jobj_sub;
	const char* jsub;
	size_t jsub_sz;
	for (i = 0; i < tbl->size(); i++) {
		tmpEnt = new OpenStateDataEnt();
		const excelTableEnt& tblEnt = (*tbl)[i];
		tmpEnt->id = toInt(tryGetKey(tblEnt, "id"));
		tmpEnt->is_default = toInt(tryGetKey(tblEnt, "defaultState"));
		tmpEnt->allowClientOpen = toInt(tryGetKey(tblEnt, "allowClientOpen"));
		tmpEnt->uiOpenId = toInt(tryGetKey(tblEnt, "systemUiOpenId"));
		if (is_json) {
			// Set both conditions to "at least AR 0" as default in case parsing fails.
			tmpEnt->openCond[0].type = OPEN_STATE_COND_PLAYER_LEVEL;
			tmpEnt->openCond[0].param[0] = 0;
			tmpEnt->openCond[1].type = OPEN_STATE_COND_PLAYER_LEVEL;
			tmpEnt->openCond[1].param[0] = 0;
			v = tryGetKey(tblEnt, "cond");
			if (!v.empty()) {
				jsub = v.c_str();
				jsub_sz = v.size();
				jtk = json_tokener_new();
				if (jtk != NULL) {
					jobj = json_tokener_parse_ex(jtk, jsub, jsub_sz);
					if (jobj != NULL) {
						if (json_object_is_type(jobj, json_type_array)) {
							for (j = 0; j < MAX(2, json_object_array_length(jobj)); j++) {
								jobj_idx = json_object_array_get_idx(jobj, j);
								if (jobj_idx == NULL) continue;
								jobj_sub = json_object_object_get(jobj_idx, "condType");
								if (jobj_sub != NULL) {
									// First, try to get the value as an int.
									errno = 0;
									tmpEnt->openCond[j].type = json_object_get_int(jobj_sub);
									if (errno == EINVAL) {
										// If that failed, it may be an enum value.
										e = enumToInt(json_object_get_string(jobj_sub), OpenStateCondTbl);
										if ((int) e == -1) {
											tmpEnt->openCond[j].type = 0;
										}
										else {
											tmpEnt->openCond[j].type = e;
										}
									}
								}
								jobj_sub = json_object_object_get(jobj_idx, "param");
								if (jobj_sub != NULL) {
									tmpEnt->openCond[j].param[0] = json_object_get_int(jobj_sub);
								}
								else {
									tmpEnt->openCond[j].param[0] = 0;
								}
								jobj_sub = json_object_object_get(jobj_idx, "param2");
								if (jobj_sub != NULL) {
									tmpEnt->openCond[j].param[1] = json_object_get_int(jobj_sub);
								}
								else {
									tmpEnt->openCond[j].param[1] = 0;
								}
							}
						}
						json_object_put(jobj);
					}
				}
			}
			if (jtk != NULL) json_tokener_free(jtk);
		}
		else {
			tmpEnt->openCond[0].type = toInt(tryGetKey(tblEnt, "cond1Type"));
			tmpEnt->openCond[0].param[0] = toInt(tryGetKey(tblEnt, "cond1Param1"));
			tmpEnt->openCond[0].param[1] = toInt(tryGetKey(tblEnt, "cond1Param2"));
			tmpEnt->openCond[1].type = toInt(tryGetKey(tblEnt, "cond2Type"));
			tmpEnt->openCond[1].param[0] = toInt(tryGetKey(tblEnt, "cond2Param1"));
			tmpEnt->openCond[1].param[1] = toInt(tryGetKey(tblEnt, "cond2Param2"));
		}
		// TODO Config option to force these openstate IDs to be configured statically
		if (tmpEnt->id == 1) { // Paimon menu
			tmpEnt->is_default = 1;
			tmpEnt->openCond[0].type = OPEN_STATE_COND_PLAYER_LEVEL;
			tmpEnt->openCond[0].param[0] = 0;
			tmpEnt->openCond[1].type = OPEN_STATE_COND_PLAYER_LEVEL;
			tmpEnt->openCond[1].param[0] = 0;
		}
		if (tmpEnt->id == 47 || tmpEnt->id == 48) { // Paimon barriers
			tmpEnt->is_default = 0;
			tmpEnt->allowClientOpen = 0;
			tmpEnt->openCond[0].type = OPEN_STATE_COND_PLAYER_LEVEL;
			tmpEnt->openCond[0].param[0] = ~0;
			tmpEnt->openCond[1].type = OPEN_STATE_COND_PLAYER_LEVEL;
			tmpEnt->openCond[1].param[0] = ~0;
		}
		entries.push_back(*tmpEnt);
        delete tmpEnt;
	}
	delete tbl;
	return 0;
}

const OpenStateDataEnt* OpenStateData::operator[](const unsigned int idx) const {
	return &entries[idx];
}

const OpenStateDataEnt* OpenStateData::at(const unsigned int idx) const {
	if (idx >= entries.size()) return NULL;
	return &entries[idx];
}

size_t OpenStateData::size() const {
	return entries.size();
}
