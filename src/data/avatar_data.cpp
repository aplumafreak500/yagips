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
#include "data/avatar_data.h"
#include "enum.h"
#include "enum/item.h"
#include "enum/fight_prop.h"
#include "enum/grow_curve.h"

AvatarData::AvatarData() {}
AvatarData::~AvatarData() {}

// Field names for TSV
static const std::vector<std::string> field_names = {
	"id",
	"unk1", // 默认阵营 (default camp?)
	"hpBase",
	"attackBase",
	"defenseBase",
	"critical",
	"criticalRes",
	"criticalHurt",
	"pyroRes",
	"dendroRes",
	"hydroRes",
	"electroRes",
	"anemoRes",
	"cryoRes",
	"geoRes",
	"pyroBonus",
	"dendroBonus",
	"hydroBonus",
	"electroBonus",
	"anemoBonus",
	"cryoBonus",
	"geoBonus",
	"propGrowCurveType1",
	"propGrowCurve1",
	"propGrowCurveType2",
	"propGrowCurve2",
	"propGrowCurveType3",
	"propGrowCurve3",
	"elementalMastery",
	"physicalRes",
	"physicalBonus",
	"useType",
	"qualityType",
	"chargeEfficiency",
	"unk2", // 治疗效果 (treatment effect?)
	"unk3", // 受治疗效果 (therapeutic effect?)
	"combatConfig",
	"isRangeAttack",
	"initialWeapon",
	"weaponType",
	"skillDepotId",
	"staminaRecoverSpeed",
	"candSkillDepotIds",
	"avatarIdentityType",
	"avatarPromoteId",
	"avatarPromoteRewardLevelList",
	"avatarPromoteRewardIdList",
	"featureTagGroupID",
	"name",
	"desc"
};

int AvatarData::load() {
	const config_t* config = globalConfig->getConfig();
	static char path[4096];
	int ret;
	path[4095] = '\0';
	// Try first with Grasscutter's name scheme.
	snprintf(path, 4095, "%s/ExcelBinOutput/AvatarExcelConfigData.json", config->dataPath);
	ret = load(path);
	if (ret >= 0) return ret;
	snprintf(path, 4095, "%s/ExcelBinOutput/AvatarExcelConfigData.tsv", config->dataPath);
	ret = load(path);
	if (ret >= 0) return ret;
	snprintf(path, 4095, "%s/ExcelBinOutput/AvatarExcelConfigData.txt", config->dataPath);
	ret = load(path);
	if (ret >= 0) return ret;
	// Try again with GIO's name scheme.
	snprintf(path, 4095, "%s/txt/AvatarData.json", config->dataPath);
	ret = load(path);
	if (ret >= 0) return ret;
	snprintf(path, 4095, "%s/txt/AvatarData.tsv", config->dataPath);
	ret = load(path);
	if (ret >= 0) return ret;
	snprintf(path, 4095, "%s/txt/AvatarData.txt", config->dataPath);
	ret = load(path);
	if (ret >= 0) return ret;
	// Bail
	return -1;
}

int AvatarData::load(const char* path) {
	AvatarDataEnt* tmpEnt = {};
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
	std::string v;
	char* b = NULL;
	char* t = NULL;
	char* p;
	std::string rkey;
	std::string rval;
	unsigned int rkey_i;
	unsigned int rval_i;
	char* rkey_b = NULL;
	char* rval_b = NULL;
	char* rkey_t = NULL;
	char* rval_t = NULL;
	char* rkey_p;
	char* rval_p;
	struct json_tokener* jtk = NULL;
	struct json_object* jobj;
	struct json_object* jobj_idx;
	struct json_object* jobj_sub;
	const char* jsub;
	size_t jsub_sz;
	unsigned int j;
	unsigned int e;
	for (i = 0; i < tbl->size(); i++) {
		tmpEnt = new AvatarDataEnt();
		const excelTableEnt& tblEnt = (*tbl)[i];
		tmpEnt->id = toInt(tryGetKey(tblEnt, "id"));
		tmpEnt->base_hp = toFlt(tryGetKey(tblEnt, "hpBase"));
		tmpEnt->base_atk = toFlt(tryGetKey(tblEnt, "attackBase"));
		tmpEnt->base_def = toFlt(tryGetKey(tblEnt, "defenseBase"));
		tmpEnt->crit_rate = toFlt(tryGetKey(tblEnt, "critical"));
		tmpEnt->crit_dmg = toFlt(tryGetKey(tblEnt, "criticalHurt"));
		tmpEnt->crit_res = toFlt(tryGetKey(tblEnt, "criticalRes"));
		tmpEnt->element_res[0] = toFlt(tryGetKey(tblEnt, "pyroRes"));
		tmpEnt->element_res[1] = toFlt(tryGetKey(tblEnt, "dendroRes"));
		tmpEnt->element_res[2] = toFlt(tryGetKey(tblEnt, "hydroRes"));
		tmpEnt->element_res[3] = toFlt(tryGetKey(tblEnt, "electroRes"));
		tmpEnt->element_res[4] = toFlt(tryGetKey(tblEnt, "anemoRes"));
		tmpEnt->element_res[5] = toFlt(tryGetKey(tblEnt, "geoRes"));
		tmpEnt->element_res[6] = toFlt(tryGetKey(tblEnt, "cryoRes"));
		tmpEnt->element_res[7] = toFlt(tryGetKey(tblEnt, "physicalRes"));
		tmpEnt->element_bonus[0] = toFlt(tryGetKey(tblEnt, "pyroBonus"));
		tmpEnt->element_bonus[1] = toFlt(tryGetKey(tblEnt, "dendroBonus"));
		tmpEnt->element_bonus[2] = toFlt(tryGetKey(tblEnt, "hydroBonus"));
		tmpEnt->element_bonus[3] = toFlt(tryGetKey(tblEnt, "electroBonus"));
		tmpEnt->element_bonus[4] = toFlt(tryGetKey(tblEnt, "anemoBonus"));
		tmpEnt->element_bonus[5] = toFlt(tryGetKey(tblEnt, "geoBonus"));
		tmpEnt->element_bonus[6] = toFlt(tryGetKey(tblEnt, "cryoBonus"));
		tmpEnt->element_bonus[7] = toFlt(tryGetKey(tblEnt, "physicalBonus"));
		tmpEnt->em = toFlt(tryGetKey(tblEnt, "elementalMastery"));
		tmpEnt->charge_efficiency = toFlt(tryGetKey(tblEnt, "chargeEfficiency"));
		tmpEnt->has_range = toInt(tryGetKey(tblEnt, "isRangeAttack"));
		tmpEnt->initial_weapon = toInt(tryGetKey(tblEnt, "initialWeapon"));
		tmpEnt->skill_depot_id = toInt(tryGetKey(tblEnt, "skillDepotId"));
		tmpEnt->stanima_recover_speed = toInt(tryGetKey(tblEnt, "stanimaRecoverSpeed"));
		tmpEnt->promote_id = toInt(tryGetKey(tblEnt, "avatarPromoteId"));
		tmpEnt->prop_group_id = toInt(tryGetKey(tblEnt, "featureTagGroupID"));
		if (is_json) {
			e = enumToInt(tryGetKey(tblEnt, "qualityType").c_str(), ItemQualityTbl);
			if ((int) e == -1) {
				tmpEnt->quality = 0;
			}
			else {
				tmpEnt->quality = e;
			}
			e = enumToInt(tryGetKey(tblEnt, "weaponType").c_str(), WeaponTypeTbl);
			if ((int) e == -1) {
				tmpEnt->weapon_type = 0;
			}
			else {
				tmpEnt->weapon_type = e;
			}
			// TODO useType and avatarIdentityType enum mappings
			// TODO battle_config_file string is stored as a hash in client files.
			// TODO could be a bit cleaner
			v = tryGetKey(tblEnt, "propGrowCurves");
			if (!v.empty()) {
				jsub = v.c_str();
				jsub_sz = v.size();
				jtk = json_tokener_new();
				if (jtk != NULL) {
					jobj = json_tokener_parse_ex(jtk, jsub, jsub_sz);
					if (jobj != NULL) {
						if (json_object_is_type(jobj, json_type_array)) {
							for (j = 0; j < MAX(3, json_object_array_length(jobj)); j++) {
								jobj_idx = json_object_array_get_idx(jobj, j);
								if (jobj_idx == NULL) continue;
								jobj_sub = json_object_object_get(jobj_idx, "type");
								if (jobj_sub != NULL) {
									// First, try to get the value as an int.
									errno = 0;
									tmpEnt->prop_curves[j].prop = json_object_get_int(jobj_sub);
									if (errno == EINVAL) {
										// If that failed, it may be an enum value.
										e = enumToInt(json_object_get_string(jobj_sub), FightPropTbl);
										if ((int) e == -1) {
											tmpEnt->prop_curves[j].prop = 0;
										}
										else {
											tmpEnt->prop_curves[j].prop = e;
										}
									}
								}
								jobj_sub = json_object_object_get(jobj_idx, "growCurve");
								if (jobj_sub != NULL) {
									errno = 0;
									tmpEnt->prop_curves[j].type = json_object_get_int(jobj_sub);
									if (errno == EINVAL) {
										e = enumToInt(json_object_get_string(jobj_sub), GrowCurveTbl);
										if ((int) e == -1) {
											tmpEnt->prop_curves[j].type = 0;
										}
										else {
											tmpEnt->prop_curves[j].type = e;
										}
									}
								}
							}
						}
						json_object_put(jobj);
					}
				}
			}
			v = tryGetKey(tblEnt, "candSkillDepotIds");
			if (!v.empty()) {
				// Reset to parse candSkillDepotIds
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
								tmpEnt->candidate_skill_depot_ids[j] = json_object_get_int(jobj_idx);
							}
						}
						json_object_put(jobj);
					}
				}
			}
/* TODO
			rkey = tryGetKey(tblEnt, "avatarPromoteRewardLevelList");
			rval = tryGetKey(tblEnt, "avatarPromoteRewardIdList");
			if (!rkey.empty()) {
				// Reset to parse avatarPromoteReward{Level,Id}List
				if (jtk != NULL) {
					json_tokener_reset(jtk);
				}
				else {
					jtk = json_tokener_new();
				}
				if (jtk != NULL) {
					// TODO
				}
			}*/
			if (jtk != NULL) json_tokener_free(jtk);
		}
		else {
			tmpEnt->prop_curves[0].prop = toInt(tryGetKey(tblEnt, "propGrowCurveType1"));
			tmpEnt->prop_curves[0].type = toInt(tryGetKey(tblEnt, "propGrowCurve1"));
			tmpEnt->prop_curves[1].prop = toInt(tryGetKey(tblEnt, "propGrowCurveType2"));
			tmpEnt->prop_curves[1].type = toInt(tryGetKey(tblEnt, "propGrowCurve2"));
			tmpEnt->prop_curves[2].prop = toInt(tryGetKey(tblEnt, "propGrowCurveType3"));
			tmpEnt->prop_curves[2].type = toInt(tryGetKey(tblEnt, "propGrowCurve3"));
			tmpEnt->battle_config_file = tryGetKey(tblEnt, "combatConfig");
			tmpEnt->useType = toInt(tryGetKey(tblEnt, "useType"));
			tmpEnt->quality = toInt(tryGetKey(tblEnt, "qualityType"));
			tmpEnt->weapon_type = toInt(tryGetKey(tblEnt, "weaponType"));
			tmpEnt->identity = toInt(tryGetKey(tblEnt, "avatarIdentityType"));
			// TODO Could be cleaner
			rkey = tryGetKey(tblEnt, "avatarPromoteRewardLevelList");
			rval = tryGetKey(tblEnt, "avatarPromoteRewardIdList");
			if (!rkey.empty()) {
				rkey_b = (char*) malloc(rkey.size() + 1);
				if (!rval.empty()) {
					rval_b = (char*) malloc(rval.size() + 1);
				}
			}
			if (rkey_b != NULL) {
				memcpy(rkey_b, rkey.c_str(), rkey.size() + 1);
				rkey_p = rkey_b;
			}
			if (rval_b != NULL) {
				memcpy(rval_b, rval.c_str(), rval.size() + 1);
				rval_p = rval_b;
			}
			while (rkey_p != NULL) {
				rkey_t = strsep(&rkey_p, ";");
				if (rkey_t == NULL) break;
				rval_t = strsep(&rval_p, ";");
				rkey_i = strtoul(rkey_t, NULL, 0);
				rval_i = rval_p == NULL ? 0 : strtoul(rval_t, NULL, 0);
				tmpEnt->promote_rewards[rkey_i] = rval_i;
			}
			free(rkey_b);
			free(rval_b);
			rkey_b = NULL;
			rval_b = NULL;
			v = tryGetKey(tblEnt, "candSkillDepotIds");
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
					tmpEnt->candidate_skill_depot_ids.push_back(e);
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

const AvatarDataEnt* AvatarData::operator[](const unsigned int idx) const {
	unsigned i;
	for (i = 0; i < entries.size(); i++) {
		if (entries[i].id == idx) return &entries[i];
	}
	return NULL;
}

const AvatarDataEnt* AvatarData::at(const unsigned int idx) const {
	if (idx >= entries.size()) return NULL;
	return &entries[idx];
}

size_t AvatarData::size() const {
	return entries.size();
}
