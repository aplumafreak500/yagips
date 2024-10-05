/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <json-c/json_object.h>
#include <json-c/json_tokener.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <string>
#include <map>
#include <unordered_map>
#include <stdexcept>
#include <vector>
#include "data.h"

// Needed to suppress "invalid use of incomplete type" errors
#include <json-c/linkhash.h>

const std::vector<excelTableEnt>* loadJsonExcelData(FILE* const fp) {
	if (fp == NULL) return NULL;
	static char fBuf[4096];
	size_t fLen;
	unsigned int done = 0;
	struct json_tokener* jtk = json_tokener_new();
	struct json_object* root;
	if (jtk == NULL) return NULL;
	while (!done) {
		memset(fBuf, 0, 4096);
		fLen = fread(fBuf, 1, 4096, fp);
		if (fLen < 4096) {
			if (feof(fp)) done = 1;
			if (ferror(fp)) {
				fprintf(stderr, "Error reading from file: errno %d (%s)\n", errno, strerror(errno));
				done = 1;
			}
		}
		root = json_tokener_parse_ex(jtk, fBuf, 4096);
		if (root == NULL) {
			json_tokener_error jerr = json_tokener_get_error(jtk);
			if (jerr != json_tokener_continue) {
				fprintf(stderr, "Error parsing JSON: %s\n", json_tokener_error_desc(jerr));
				json_tokener_free(jtk);
				return NULL;
			}
			else continue;
		}
		else break;
	}
	if (root == NULL) {
		fprintf(stderr, "Incomplete JSON object received\n");
		json_tokener_free(jtk);
		return NULL;
	}
	if (json_object_get_type(root) != json_type_array) {
		fprintf(stderr, "Received JSON data is not an array\n");
		json_tokener_free(jtk);
		return NULL;
	}
	json_tokener_free(jtk);
	unsigned int i;
	std::vector<excelTableEnt>* tbl = new std::vector<excelTableEnt>();
	excelTableEnt* ent = new excelTableEnt();
	struct json_object* data;
	for (i = 0; i < json_object_array_length(root); i++) {
		data = json_object_array_get_idx(root, i);
		// Reject non-objects.
		if (data == NULL) continue;
		if (json_object_get_type(root) != json_type_object) continue;
		struct json_object_iter j;
		json_object_object_foreachC(data, j) {
			(*ent)[j.key] = json_object_get_string(j.val);
		}
		tbl->push_back(*ent);
		ent->clear();
	}
	delete ent;
	json_object_put(root);
	return tbl;
}

const std::vector<excelTableEnt>* loadTsvExcelData(const std::vector<std::string>& field_list, FILE* const fp) {
	if (fp == NULL) return NULL;
	std::vector<excelTableEnt>* tbl = new std::vector<excelTableEnt>();
	excelTableEnt* ent = new excelTableEnt();
	unsigned int i;
	unsigned int l = 0;
	size_t fLen = 4096;
	ssize_t fLen_r;
	char* fBuf = (char*) malloc(fLen);
	char* tsv_ent;
	char* j;
	if (fBuf == NULL) return NULL;
	while (!feof(fp)) {
		l++;
		memset(fBuf, 0, fLen);
		fLen_r = getline(&fBuf, &fLen, fp);
		if (fLen_r < 0) {
			if (ferror(fp)) {
				fprintf(stderr, "Error reading from file: errno %d (%s)\n", errno, strerror(errno));
				break;
			}
		}
		if (l <= 1) continue; // Skip the first line, as it just contains the column headers, which are different from the field headers (and written in Chinese)
		j = fBuf;
		for (i = 0; i < field_list.size(); i++) {
			tsv_ent = strsep(&j, "\t\r\n");
			if (tsv_ent == NULL) continue;
			(*ent)[field_list[i]] = tsv_ent;
		}
		tbl->push_back(*ent);
		ent->clear();
	}
	free(fBuf);
	return tbl;
}

std::string tryGetKey(const excelTableEnt& v, const std::string& k) {
	std::string r;
	try {
		r = v.at(k);
	}
	catch(const std::out_of_range&) {}
	return r;
}

GameData::GameData() {
	avatar_data = new AvatarData();
	if (avatar_data->load()) {
		fprintf(stderr, "Warning: Failed to load avatar data\n");
	}
	openstate_data = new OpenStateData();
	if (openstate_data->load()) {
		fprintf(stderr, "Warning: Failed to load openstate data\n");
	}
}

GameData::~GameData() {
	if (avatar_data != NULL) delete avatar_data;
	if (openstate_data != NULL) delete openstate_data;
}

GameData* globalGameData;
