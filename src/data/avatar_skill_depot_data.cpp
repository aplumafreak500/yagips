/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2025 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <string>
#include <vector>
#include <json-c/json_tokener.h>
#include <json-c/json_object.h>
#include "runconfig.h"
#include "util.h"
#include "data.h"
#include "data/avatar_skill_depot_data.h"

AvatarSkillDepotData::AvatarSkillDepotData() {}
AvatarSkillDepotData::~AvatarSkillDepotData() {}

// Field names for TSV
static const std::vector<std::string> field_names = {
	"id",
	"energySkill",
	"unk1",
	"skill1",
	"skill2",
	"skill3",
	"skill4",
	"subSkills",
	"attackModeSkill",
	"leaderTalent",
	"extraAbility1",
	"extraAbility2",
	"extraAbility3",
	"talent1",
	"talent2",
	"talent3",
	"talent4",
	"talent5",
	"talent6",
	"talentStarName",
	"proudSkillReqBreak",
	"proudSkillGroupId1",
	"needAvatarPromoteLevel1",
	"proudSkillGroupId2",
	"needAvatarPromoteLevel2",
	"proudSkillGroupId3",
	"needAvatarPromoteLevel3",
	"proudSkillGroupId4",
	"needAvatarPromoteLevel4",
	"proudSkillGroupId5",
	"needAvatarPromoteLevel5",
	"skillDepotAbilityGroup"
};

int AvatarSkillDepotData::load() {
	const config_t* config = globalConfig->getConfig();
	static char path[4096];
	int ret;
	path[4095] = '\0';
	// Try first with Grasscutter's name scheme.
	snprintf(path, 4095, "%s/ExcelBinOutput/AvatarSkillDepotExcelConfigData.json", config->dataPath);
	ret = load(path);
	if (ret >= 0) return ret;
	snprintf(path, 4095, "%s/ExcelBinOutput/AvatarSkillDepotExcelConfigData.tsv", config->dataPath);
	ret = load(path);
	if (ret >= 0) return ret;
	snprintf(path, 4095, "%s/ExcelBinOutput/AvatarSkillDepotExcelConfigData.txt", config->dataPath);
	ret = load(path);
	if (ret >= 0) return ret;
	// Try again with GIO's name scheme.
	snprintf(path, 4095, "%s/txt/AvatarSkillDepotData.json", config->dataPath);
	ret = load(path);
	if (ret >= 0) return ret;
	snprintf(path, 4095, "%s/txt/AvatarSkillDepotData.tsv", config->dataPath);
	ret = load(path);
	if (ret >= 0) return ret;
	snprintf(path, 4095, "%s/txt/AvatarSkillDepotData.txt", config->dataPath);
	ret = load(path);
	if (ret >= 0) return ret;
	// Bail
	return -1;
}

int AvatarSkillDepotData::load(const char* path) {
	AvatarSkillDepotDataEnt* tmpEnt = {};
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
	char* b = NULL;
	char* t = NULL;
	char* p;
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
		tmpEnt = new AvatarSkillDepotDataEnt();
		const excelTableEnt& tblEnt = (*tbl)[i];
		tmpEnt->id = toInt(tryGetKey(tblEnt, "id"));
		tmpEnt->burstSkill = toInt(tryGetKey(tblEnt, "energySkill"));
		tmpEnt->aimTalent = toInt(tryGetKey(tblEnt, "attackModeSkill"));
		tmpEnt->leaderTalent = toInt(tryGetKey(tblEnt, "leaderTalent"));
		tmpEnt->talentConfigFile = tryGetKey(tblEnt, "talentStarName");
		tmpEnt->abilityGroup = tryGetKey(tblEnt, "skillDepotAbilityGroup");
		if (is_json) {
			// TODO could be a bit cleaner
			v = tryGetKey(tblEnt, "skills");
			if (!v.empty()) {
				jtk = json_tokener_new();
				if (jtk != NULL) {
					jsub = v.c_str();
					jsub_sz = v.size();
					jobj = json_tokener_parse_ex(jtk, jsub, jsub_sz);
					if (jobj != NULL) {
						if (json_object_is_type(jobj, json_type_array)) {
							for (j = 0; j < MAX(json_object_array_length(jobj), 4); j++) {
								jobj_idx = json_object_array_get_idx(jobj, j);
								if (jobj_idx == NULL) continue;
								tmpEnt->skills[j] = json_object_get_int(jobj_idx);
							}
						}
						json_object_put(jobj);
					}
				}
			}
			v = tryGetKey(tblEnt, "subSkills");
			if (!v.empty()) {
				// Reset to parse subSkills
				if (jtk != NULL) {
					json_tokener_reset(jtk);
				}
				else {
					jtk = json_tokener_new();
				}
				if (jtk != NULL) {
					jsub = v.c_str();
					jsub_sz = v.size();
					jobj = json_tokener_parse_ex(jtk, jsub, jsub_sz);
					if (jobj != NULL) {
						if (json_object_is_type(jobj, json_type_array)) {
							for (j = 0; j < json_object_array_length(jobj); j++) {
								jobj_idx = json_object_array_get_idx(jobj, j);
								if (jobj_idx == NULL) continue;
								tmpEnt->secondarySkills[j] = json_object_get_int(jobj_idx);
							}
						}
						json_object_put(jobj);
					}
				}
			}
			v = tryGetKey(tblEnt, "extraAbilities");
			if (!v.empty()) {
				// Reset to parse extraAbilities
				if (jtk != NULL) {
					json_tokener_reset(jtk);
				}
				else {
					jtk = json_tokener_new();
				}
				if (jtk != NULL) {
					jsub = v.c_str();
					jsub_sz = v.size();
					jobj = json_tokener_parse_ex(jtk, jsub, jsub_sz);
					if (jobj != NULL) {
						if (json_object_is_type(jobj, json_type_array)) {
							for (j = 0; j < MAX(json_object_array_length(jobj), 3); j++) {
								jobj_idx = json_object_array_get_idx(jobj, j);
								if (jobj_idx == NULL) continue;
								tmpEnt->extraAbilities[j] = json_object_get_int(jobj_idx);
							}
						}
						json_object_put(jobj);
					}
				}
			}
			v = tryGetKey(tblEnt, "talents");
			if (!v.empty()) {
				// Reset to parse talents
				if (jtk != NULL) {
					json_tokener_reset(jtk);
				}
				else {
					jtk = json_tokener_new();
				}
				if (jtk != NULL) {
					jsub = v.c_str();
					jsub_sz = v.size();
					jobj = json_tokener_parse_ex(jtk, jsub, jsub_sz);
					if (jobj != NULL) {
						if (json_object_is_type(jobj, json_type_array)) {
							for (j = 0; j < MAX(json_object_array_length(jobj), 6); j++) {
								jobj_idx = json_object_array_get_idx(jobj, j);
								if (jobj_idx == NULL) continue;
								tmpEnt->constellationSkills[j] = json_object_get_int(jobj_idx);
							}
						}
						json_object_put(jobj);
					}
				}
			}
			v = tryGetKey(tblEnt, "inherentProudSkillOpens");
			if (!v.empty()) {
				jsub = v.c_str();
				jsub_sz = v.size();
				// Reset to parse inherentProudSkillOpens
				if (jtk != NULL) {
					json_tokener_reset(jtk);
				}
				else {
					jtk = json_tokener_new();
				}
				if (jtk != NULL) {
					jobj = json_tokener_parse_ex(jtk, jsub, jsub_sz);
					if (jobj != NULL) {
						if (json_object_is_type(jobj, json_type_array)) {
							for (j = 0; j < MAX(5, json_object_array_length(jobj)); j++) {
								jobj_idx = json_object_array_get_idx(jobj, j);
								if (jobj_idx == NULL) continue;
								jobj_sub = json_object_object_get(jobj_idx, "proudSkillGroupId");
								if (jobj_sub != NULL) {
									tmpEnt->ascensionPassiveSkills[j].id = json_object_get_int(jobj_sub);
								}
								jobj_sub = json_object_object_get(jobj_idx, "needAvatarPromoteLevel");
								if (jobj_sub != NULL) {
									tmpEnt->ascensionPassiveSkills[j].requiredAscensionLevel = json_object_get_int(jobj_sub);
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
			tmpEnt->skills[0] = toInt(tryGetKey(tblEnt, "skill1"));
			tmpEnt->skills[1] = toInt(tryGetKey(tblEnt, "skill2"));
			tmpEnt->skills[2] = toInt(tryGetKey(tblEnt, "skill3"));
			tmpEnt->skills[3] = toInt(tryGetKey(tblEnt, "skill4"));
			tmpEnt->extraAbilities[0] = toInt(tryGetKey(tblEnt, "extraAbility1"));
			tmpEnt->extraAbilities[1] = toInt(tryGetKey(tblEnt, "extraAbility2"));
			tmpEnt->extraAbilities[2] = toInt(tryGetKey(tblEnt, "extraAbility2"));
			tmpEnt->constellationSkills[0] = toInt(tryGetKey(tblEnt, "talent1"));
			tmpEnt->constellationSkills[1] = toInt(tryGetKey(tblEnt, "talent2"));
			tmpEnt->constellationSkills[2] = toInt(tryGetKey(tblEnt, "talent3"));
			tmpEnt->constellationSkills[3] = toInt(tryGetKey(tblEnt, "talent4"));
			tmpEnt->constellationSkills[4] = toInt(tryGetKey(tblEnt, "talent5"));
			tmpEnt->constellationSkills[5] = toInt(tryGetKey(tblEnt, "talent6"));
			tmpEnt->ascensionPassiveSkills[0].id = toInt(tryGetKey(tblEnt, "proudSkillGroupId1"));
			tmpEnt->ascensionPassiveSkills[0].requiredAscensionLevel = toInt(tryGetKey(tblEnt, "needAvatarPromoteLevel1"));
			tmpEnt->ascensionPassiveSkills[1].id = toInt(tryGetKey(tblEnt, "proudSkillGroupId2"));
			tmpEnt->ascensionPassiveSkills[1].requiredAscensionLevel = toInt(tryGetKey(tblEnt, "needAvatarPromoteLevel2"));
			tmpEnt->ascensionPassiveSkills[2].id = toInt(tryGetKey(tblEnt, "proudSkillGroupId3"));
			tmpEnt->ascensionPassiveSkills[2].requiredAscensionLevel = toInt(tryGetKey(tblEnt, "needAvatarPromoteLevel3"));
			tmpEnt->ascensionPassiveSkills[3].id = toInt(tryGetKey(tblEnt, "proudSkillGroupId4"));
			tmpEnt->ascensionPassiveSkills[3].requiredAscensionLevel = toInt(tryGetKey(tblEnt, "needAvatarPromoteLevel4"));
			tmpEnt->ascensionPassiveSkills[4].id = toInt(tryGetKey(tblEnt, "proudSkillGroupId5"));
			tmpEnt->ascensionPassiveSkills[4].requiredAscensionLevel = toInt(tryGetKey(tblEnt, "needAvatarPromoteLevel5"));
			// TODO Could be cleaner
			v = tryGetKey(tblEnt, "subSkills");
			if (!v.empty()) {
				b = (char*) malloc(v.size() + 1);
				if (b != NULL) {
					memcpy(b, v.c_str(), v.size() + 1);
					p = b;
				}
				j = 0;
				while (p != NULL) {
					t = strsep(&p, ",");
					if (t == NULL) break;
					e = strtoul(t, NULL, 0);
					tmpEnt->secondarySkills.push_back(e);
					j++;
				}
			}
			free(b);
			b = NULL;
		}
		entries.push_back(*tmpEnt);
		delete tmpEnt;
	}
	delete tbl;
	return 0;
}

const AvatarSkillDepotDataEnt* AvatarSkillDepotData::operator[](const unsigned int idx) const {
	unsigned i;
	for (i = 0; i < entries.size(); i++) {
		if (entries[i].id == idx) return &entries[i];
	}
	return NULL;
}

const AvatarSkillDepotDataEnt* AvatarSkillDepotData::at(const unsigned int idx) const {
	if (idx >= entries.size()) return NULL;
	return &entries[idx];
}

size_t AvatarSkillDepotData::size() const {
	return entries.size();
}
