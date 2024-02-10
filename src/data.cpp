#include <json-c/json_object.h>
#include <json-c/json_tokener.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <string>
#include <map>
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
	unsigned int i, j;
	unsigned int l = 0;
	size_t fLen = 4096;
	ssize_t fLen_r;
	char* fBuf = (char*) malloc(fLen);
	char* tsv_ent;
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
		j = 0;
		for (i = 0; i < field_list.size(); i++) {
			tsv_ent = strtok(j ? NULL : fBuf, "\t\r\n");
			if (!j) j = 1;
			if (tsv_ent == NULL) continue;
			(*ent)[field_list[i]] = tsv_ent;
		}
		tbl->push_back(*ent);
		ent->clear();
	}
	free(fBuf);
	return tbl;
}
