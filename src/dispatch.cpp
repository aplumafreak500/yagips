/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2023 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/random.h>
#include <arpa/inet.h>
#include <errno.h>
#include <json-c/json_object.h>
#include <json-c/json_tokener.h>
#include <string>
#include "config.h"
#include "account.h"
#include "dbgate.h"
#include "gameserver.h"
#include "util.h"
#include "crypt.h"
#include "dispatch.h"
#include "dispatch.pb.h"
#include "keys.h"
#include "http.h"

enum {
	CLIENT_UNK = -1,
	CLIENT_PC = 0,
	CLIENT_ANDROID = 1,
	CLIENT_IOS = 2,
//	CLIENT_PS4 = 3,
//	CLIENT_PS5 = 4,
	CLIENT_CNT
};

enum {
	REGION_UNK = -1,
	REGION_OS = 0,
	REGION_CN = 1,
	REGION_CNT
};

#define LIVE_MAJOR 4
#define LIVE_MINOR 3
#define CUR_MAJOR 3
#define CUR_MINOR 2

std::string getQueryRegionListHttpRsp(const char* post) {
	proto::QueryRegionListHttpRsp ret;
	proto::RegionSimpleInfo* pregion;
	std::string ret_enc;
	std::string seed;
	std::string cconfig;
	size_t post_len;
	struct json_tokener* jtk;
	enum json_tokener_error jerr;
	struct json_object* jobj;
	const char* data;
	struct json_object* dobj;
	unsigned int major = 0;
	unsigned int minor = 0;
	unsigned int patch = 0;
	int client, region;
	char sclient[32];
	char sregion[3];
	int sret;
	char configBuf[1024];
	const char* config;
	size_t config_sz;
	const config_t* config_p;
	size_t i;
	unsigned int actualRegionCnt = 0;
	char tmpBuf[1024];
	if (post == NULL) {
		ret.set_retcode(-1);
		goto build_rsp;
	}
	post_len = strlen(post) + 1;
	jtk = json_tokener_new();
	if (jtk == NULL) {
		ret.set_retcode(-1);
		goto build_rsp;
	}
	jobj = json_tokener_parse_ex(jtk, post, post_len);
	if (jobj == NULL) {
		jerr = json_tokener_get_error(jtk);
		if (jerr == json_tokener_continue) {
			fprintf(stderr, "Error: JSON in POST data is incomplete\n");
		}
		else {
			fprintf(stderr, "Error parsing JSON POST data: %s\n", json_tokener_error_desc(jerr));
		}
		json_tokener_free(jtk);
		ret.set_retcode(-1);
		goto build_rsp;
	}
	if (json_tokener_get_parse_end(jtk) < post_len) {
		fprintf(stderr, "Warning: JSON in POST data has extra trailing data, it will be ignored\n");
	}
	json_tokener_free(jtk);
	if (!json_object_is_type(jobj, json_type_object)) {
		fprintf(stderr, "Error: JSON in POST data is not an object.\n");
		json_object_put(jobj);
		ret.set_retcode(-1);
		goto build_rsp;
	}
	if (!json_object_object_get_ex(jobj, "version", &dobj)) {
		// Version isn't even present
		json_object_put(jobj);
		ret.set_retcode(-1);
		goto build_rsp;
	}
	data = json_object_get_string(dobj);
	if (data == NULL) {
		// Version is present, but set to null
		json_object_put(jobj);
		ret.set_retcode(-1);
		goto build_rsp;
	}
	memset(sregion, '\0', 3);
	memset(sclient, '\0', 32);
	sret = sscanf(data, "%2cREL%31[^0-9.]%d.%d.%d", sregion, sclient, &major, &minor, &patch);
	if (sret == EOF || sret < 5) {
		// Version is present, but in an invalid format
		json_object_put(jobj);
		ret.set_retcode(-1);
		goto build_rsp;
	}
	if (strncmp(sregion, "OS", 3) == 0) region = REGION_OS;
	else if (strncmp(sregion, "CN", 3) == 0) region = REGION_CN;
	else region = REGION_UNK;
	if (strncmp(sclient, "Win", 31) == 0) client = CLIENT_PC;
	else if (strncmp(sclient, "Android", 31) == 0) client = CLIENT_ANDROID;
	else if (strncmp(sclient, "iOS", 31) == 0) client = CLIENT_IOS;
	// TODO PS4 and PS5 version ID's?
	// else if (strncmp(sclient, "PS4", 31) == 0) client = CLIENT_PS4;
	// else if (strncmp(sclient, "PS5", 31) == 0) client = CLIENT_PS5;
	else client = CLIENT_UNK;
	if (
		(client <= CLIENT_UNK || client >= CLIENT_CNT) ||
		(region <= REGION_UNK || region >= REGION_CNT) ||
		// Note: Official server accepts any version, as long as it's a valid, in range version, including both release and beta versions. (For example, reporting 1.0.0 or 4.2.51 is still allowed, but (as of 12/27/2023 when 4.3.0 was live) reporting 4.2.2 or 4.3.1 (not existing live client patch), 4.4.0 (future version), or 4.3.55 (not existing beta) causes dispatch to return -1.)
		// Also note that query_curr_region has much stricter version checks.
		// TODO: allow pre-1.0 betas, including CB3 (not CB1 or 2, they don't use KCP)
		(major < 1 || major > LIVE_MAJOR) ||
		(major == 1 && minor > 6) || // 2.0 was the first major version with a minor set to 7 or 8
		(major == LIVE_MAJOR && minor > LIVE_MINOR) ||
		minor > 8 ||
		// TODO: Does any beta version have a patch value above 51?
		patch > 51 ||
		// TODO: Does any live version have a patch value above 1?
		(patch > 1 && patch < 51)
	) {
		// Invalid version
		json_object_put(jobj);
		ret.set_retcode(-1);
		goto build_rsp;
	}
	// Note: Official servers check for the existence and correctness of the `channel_id` parameter. In my testing, no other parameters are needed (though they may be validated if present) to get a positive response. Here, we don't check that at all, nor any other parameters, though that may change in the future.
	// TODO Clients send a `language` parameter, this could be useful in case we end up supporting multilingual `title` parameters in the region list config.
	/* TODO Roadmap:
		1. Iterate through server-configured regions and check for its version.
			* If the config-level region list is empty, send -1 to the client and an error to the log.
			* If no version is set at the config level, treat it as allowing all versions.
		2. If there's a match, add it to the response here.
		3. If the proto-level region list is empty, send back -1.
		4. Grab "custom config", seed, and enable_login_pc from config, apply to the proto if present
			* If no seed is configured, send -1 to the client and an error to the log
	*/
	json_object_put(jobj);
	config_p = globalConfig->getConfig();
	if (config_p->regionCnt != 0 || config_p->regions == NULL) {
		// No regions configured, so supply our own
		actualRegionCnt = 1;
		pregion = ret.add_region_list();
		pregion->set_name("os_usa");
		pregion->set_title("yagips");
		pregion->set_type("DEV_PUBLIC");
		// TODO Grab external ip instead of hardcoding the domain
		pregion->set_dispatch_url("http://osusadispatch.yuanshen.com/query_curr_region");
	}
	else {
		// TODO Assert that at least one of these regions is us
		for (i = 0; i < config_p->regionCnt; i++) {
			if (config_p->regions[i] != NULL) {
				if (i == 0 && config_p->regions[i]->url == NULL) continue; // allow missing url for index 0 because usually that's actually us
				if (config_p->regions[i]->version != NULL) {
					snprintf(tmpBuf, 1024, "%d.%d", major, minor);
					if (strncmp(tmpBuf, config_p->regions[i]->version, 1024) != 0) continue;
				}
				actualRegionCnt++;
				pregion = ret.add_region_list();
				// If any of these are null, present some defaults.
				if (config_p->regions[i]->name != NULL) {
					pregion->set_name(config_p->regions[i]->name);
				}
				else {
					if (i == 0) pregion->set_name("os_usa");
					else {
						snprintf(tmpBuf, 1024, "os_usa.%ld", i);
						pregion->set_name(tmpBuf);
					}
				}
				if (config_p->regions[i]->title != NULL) {
					pregion->set_name(config_p->regions[i]->title);
				}
				else {
					if (i == 0) pregion->set_title("yagips");
					else {
						snprintf(tmpBuf, 1024, "yagips %ld", i);
						pregion->set_title(tmpBuf);
					}
				}
				if (config_p->regions[i]->type != NULL) {
					pregion->set_type(config_p->regions[i]->type);
				}
				else {
					pregion->set_type("DEV_PUBLIC");
				}
				if (config_p->regions[i]->url != NULL) {
					pregion->set_dispatch_url(config_p->regions[i]->url);
				}
				else {
					// TODO Grab external ip instead of hardcoding the domain
					pregion->set_dispatch_url("http://osusadispatch.yuanshen.com/query_curr_region");
				}
			}
		}
	}
	if (actualRegionCnt == 0) {
		fprintf(stderr, "No regions have been configured.\n");
		ret.set_retcode(-1);
		goto build_rsp;
	}
	// TODO Grab from config
	// TODO Unknown what these do
	// TODO CNREL sdkenv needs to be 0
	config = "{\"sdkenv\":\"2\",\"checkdevice\":false,\"loadPatch\":false,\"showexception\":false,\"regionConfig\":\"pm|fk|add\",\"downloadMode\":0,\"codeSwitch\":[0]}";
	config_sz = strlen(config);
	if (hasDispatchSeed) {
		for (i = 0; i < config_sz; i++) {
			configBuf[i] = config[i] ^ dispatchKey[i % 4096];
		}
		/* Dispatch seed (used to derive xor key) */
		seed.assign((const char*) &dispatchSeed, 2076);
		ret.set_client_secret_key(seed);
		cconfig.assign(configBuf, config_sz);
		ret.set_client_custom_config_encrypted(cconfig);
	}
	else {
		// TODO what does the client do? null key? internal default? or just ignore?
		ret.set_client_custom_config_encrypted(config);
	}
	// TODO Grab from config
	/* TODO Unknown what this does. */
	ret.set_enable_login_pc(1);
build_rsp:
	if (!ret.SerializeToString(&ret_enc)) {
		// Bypass Protobuf and encode a response ourselves. Note that this eventually gets base64 encoded, hence the raw hex string.
		ret_enc.assign("\x08\xff\xff\xff\xff\xff\xff\xff\xff\xff\x01", 11); // retcode = -1 and no other fields set
	}
	return ret_enc;
}

std::string getQueryCurrRegionHttpRsp(std::string& sign, const char* post) {
	int doResVersionConfig = 0;
	int doGateserver = 0;
	int doSign = 1;
	int doEnc = 0;
	proto::QueryCurrRegionHttpRsp ret;
	proto::RegionInfo* region;
	proto::ResVersionConfig* res;
	proto::ResVersionConfig* resNext;
	proto::StopServerInfo* stop;
	proto::ForceUpdateInfo* upd;
	std::string ret_enc;
	const config_t* config;
	size_t post_len;
	struct json_tokener* jtk;
	enum json_tokener_error jerr;
	struct json_object* jobj;
	const char* data;
	struct json_object* dobj;
	unsigned int major = 0;
	unsigned int minor = 0;
	unsigned int patch = 0;
	int client, iregion;
	char sclient[32];
	char sregion[3];
	int sret;
	config = globalConfig->getConfig();
	// These will get overwritten later if an error occurs.
	ret.set_retcode(0);
	ret.set_msg("ok");
	if (post == NULL) {
		ret.set_retcode(-1);
		ret.set_msg("Client is missing POST data");
		goto set_fields;
	}
	post_len = strlen(post) + 1;
	jtk = json_tokener_new();
	if (jtk == NULL) {
		ret.set_retcode(-1);
		ret.set_msg("JSON tokener alloc fail");
		goto set_fields;
	}
	jobj = json_tokener_parse_ex(jtk, post, post_len);
	if (jobj == NULL) {
		jerr = json_tokener_get_error(jtk);
		if (jerr == json_tokener_continue) {
			fprintf(stderr, "Error: JSON in POST data is incomplete\n");
		}
		else {
			fprintf(stderr, "Error parsing JSON POST data: %s\n", json_tokener_error_desc(jerr));
		}
		json_tokener_free(jtk);
		ret.set_retcode(-1);
		ret.set_msg("JSON parse error");
		goto set_fields;
	}
	if (json_tokener_get_parse_end(jtk) < post_len) {
		fprintf(stderr, "Warning: JSON in POST data has extra trailing data, it will be ignored\n");
	}
	json_tokener_free(jtk);
	if (!json_object_is_type(jobj, json_type_object)) {
		fprintf(stderr, "Error: JSON in POST data is not an object.\n");
		json_object_put(jobj);
		ret.set_retcode(-1);
		ret.set_msg("JSON POST data is not an object");
		goto set_fields;
	}
	if (!json_object_object_get_ex(jobj, "version", &dobj)) {
		// Version isn't even present
		json_object_put(jobj);
		ret.set_retcode(-1);
		ret.set_msg("Version is not set");
		goto set_fields;
	}
	data = json_object_get_string(dobj);
	if (data == NULL) {
		// Version is present, but set to null
		json_object_put(jobj);
		ret.set_retcode(-1);
		ret.set_msg("Version is null");
		goto set_fields;
	}
	memset(sregion, '\0', 3);
	memset(sclient, '\0', 32);
	sret = sscanf(data, "%2cREL%31[^0-9.]%d.%d.%d", sregion, sclient, &major, &minor, &patch);
	if (sret == EOF || sret < 5) {
		// Version is present, but in an invalid format
		json_object_put(jobj);
		ret.set_retcode(-1);
		ret.set_msg("Version format is invalid");
		goto set_fields;
	}
	if (strncmp(sregion, "OS", 3) == 0) iregion = REGION_OS;
	else if (strncmp(sregion, "CN", 3) == 0) iregion = REGION_CN;
	else iregion = REGION_UNK;
	if (strncmp(sclient, "Win", 31) == 0) client = CLIENT_PC;
	else if (strncmp(sclient, "Android", 31) == 0) client = CLIENT_ANDROID;
	else if (strncmp(sclient, "iOS", 31) == 0) client = CLIENT_IOS;
	// TODO PS4 and PS5 version ID's?
	// else if (strncmp(sclient, "PS4", 31) == 0) client = CLIENT_PS4;
	// else if (strncmp(sclient, "PS5", 31) == 0) client = CLIENT_PS5;
	else client = CLIENT_UNK;
	if (
		(client <= CLIENT_UNK || client >= CLIENT_CNT) ||
		(iregion <= REGION_UNK || iregion >= REGION_CNT) ||
		// TODO Pull from index 0 of RegionListEntry array
		major != CUR_MAJOR || minor != CUR_MINOR ||
		// TODO: Does any live version have a patch value above 1?
		// Deny beta clients (for now).
		patch > 1
	) {
		json_object_put(jobj);
		ret.set_retcode(-1);
		ret.set_msg("Version mismatch");
		// TODO Also send force_update_url and/or stop_server? GC does the latter in cases of version mismatch.
		goto set_fields;
	}
	doResVersionConfig = 1;
	doGateserver = 1;
	if (json_object_object_get_ex(jobj, "skipSign", &dobj)) {
		doSign = (~json_object_get_boolean(dobj)) & 1;
	}
	if (json_object_object_get_ex(jobj, "key_id", &dobj)) {
		doEnc = json_object_get_int(dobj);
	}
	if (doEnc == 1 || doEnc > 5) {
		ret.set_retcode(-1);
		ret.set_msg("Invalid key ID");
		doEnc = 0;
	}
	/* TODO Parse aid and check it. If negative, unset, or non-numeric, continue. Else, check that it exists in the db. If not, continue. Else, check for a ban. If there is one and it hasn't expired, send back a negative response with `msg` and `retcode` set appropriately. (Skip setting the gateserver and ResVersionConfig fields here.) If it has expired, delete it from the db and then continue. */
	// TODO: Figure out what to do with the dispatch_seed. Yuuki verifies it as part of determining the client version; Grasscutter merely checks for its existence and uses a hardcoded signature if not present, but doesn't appear to do anything else with it if it is set.
	// TODO What other parameters do we need to check?
	// TODO What other parameters does the official server check?
	json_object_put(jobj);
set_fields:
	if (config->regionInfo == NULL) {
		fprintf(stderr, "Error: Region info is null, check the config\n");
		ret.set_retcode(-1); // TODO more specific error code?
		ret.set_msg("Server is missing region data");
		goto build_rsp;
	}
	region = new proto::RegionInfo;
	if (doGateserver) {
		if (config->regionInfo->gateserverIp == NULL || config->regionInfo->gateserverPort == 0) {
			fprintf(stderr, "Error: Gateserver IP or port is null, check the config\n");
			ret.set_retcode(-1); // TODO more specific error code?
			ret.set_msg("Server is missing region data");
		}
		else {
			if (config->regionInfo->gateserverIpIsDomainName) {
				region->set_use_gateserver_domain_name(1);
				region->set_gateserver_domain_name(config->regionInfo->gateserverIp);
			}
			else {
				region->set_gateserver_ip(config->regionInfo->gateserverIp);
			}
			region->set_gateserver_port(config->regionInfo->gateserverPort);
		}
	}
	if (config->regionInfo->feedbackUrl != NULL) {
		region->set_feedback_url(config->regionInfo->feedbackUrl);
	}
	if (config->regionInfo->bulletinUrl != NULL) {
		region->set_bulletin_url(config->regionInfo->bulletinUrl);
	}
	if (config->regionInfo->handbookUrl != NULL) {
		region->set_handbook_url(config->regionInfo->handbookUrl);
	}
	if (config->regionInfo->communityUrl != NULL) {
		region->set_official_community_url(config->regionInfo->communityUrl);
	}
	if (config->regionInfo->userCenterUrl != NULL) {
		region->set_user_center_url(config->regionInfo->userCenterUrl);
	}
	if (config->regionInfo->privacyPolicyUrl != NULL) {
		region->set_privacy_policy_url(config->regionInfo->privacyPolicyUrl);
	}
	if (config->regionInfo->accountBindUrl != NULL) {
		region->set_account_bind_url(config->regionInfo->accountBindUrl);
	}
	if (config->regionInfo->cdKeyUrl != NULL) {
		region->set_cdkey_url(config->regionInfo->cdKeyUrl);
	}
	if (config->regionInfo->payCbUrl != NULL) {
		region->set_pay_callback_url(config->regionInfo->payCbUrl);
	}
	if (doResVersionConfig) {
		if (config->regionInfo->res != NULL) {
			if (config->regionInfo->res->resourceUrl != NULL) {
				region->set_resource_url(config->regionInfo->res->resourceUrl);
			}
			if (config->regionInfo->res->dataUrl != NULL) {
				region->set_data_url(config->regionInfo->res->dataUrl);
			}
			region->set_client_data_version(config->regionInfo->res->dataVersion);
			if (config->regionInfo->res->dataSuffix != NULL) {
				region->set_client_version_suffix(config->regionInfo->res->dataSuffix);
			}
			if (config->regionInfo->res->dataRes != NULL) {
				region->set_client_data_md5(config->regionInfo->res->dataRes);
			}
			region->set_client_silence_data_version(config->regionInfo->res->silenceVersion);
			if (config->regionInfo->res->silenceSuffix != NULL) {
				region->set_client_silence_version_suffix(config->regionInfo->res->silenceSuffix);
			}
			if (config->regionInfo->res->silenceRes != NULL) {
				region->set_client_silence_data_md5(config->regionInfo->res->silenceRes);
			}
			res = new proto::ResVersionConfig;
			res->set_version(config->regionInfo->res->resVersion);
			if (config->regionInfo->res->resourceSuffix != NULL) {
				res->set_version_suffix(config->regionInfo->res->resourceSuffix);
			}
			if (config->regionInfo->res->resourceRes != NULL) {
				res->set_md5(config->regionInfo->res->resourceRes);
			}
			if (config->regionInfo->res->branch != NULL) {
				res->set_branch(config->regionInfo->res->branch);
			}
			// TODO also read config->regionInfo->res->scriptVersion?
			if (config->regionInfo->res->releaseTotalSize != NULL) {
				res->set_release_total_size(config->regionInfo->res->releaseTotalSize);
			}
			res->set_relogin(config->regionInfo->res->relogin);
			region->set_allocated_res_version_config(res);
		}
		if (config->regionInfo->resNext != NULL) {
			if (config->regionInfo->resNext->resourceUrl != NULL) {
				region->set_next_resource_url(config->regionInfo->resNext->resourceUrl);
			}
			resNext = new proto::ResVersionConfig;
			resNext->set_version(config->regionInfo->resNext->resVersion);
			if (config->regionInfo->resNext->resourceSuffix != NULL) {
				resNext->set_version_suffix(config->regionInfo->resNext->resourceSuffix);
			}
			if (config->regionInfo->resNext->resourceRes != NULL) {
				resNext->set_md5(config->regionInfo->resNext->resourceRes);
			}
			if (config->regionInfo->resNext->branch != NULL) {
				resNext->set_branch(config->regionInfo->resNext->branch);
			}
			if (config->regionInfo->resNext->releaseTotalSize != NULL) {
				resNext->set_release_total_size(config->regionInfo->resNext->releaseTotalSize);
			}
			resNext->set_relogin(config->regionInfo->resNext->relogin);
			if (config->regionInfo->resNext->scriptVersion != NULL) {
				resNext->set_next_script_version(config->regionInfo->resNext->scriptVersion);
			}
			region->set_allocated_next_res_version_config(resNext);
		}
	}
	// TODO Load and set secret_key (on Yuuki, same as dispatch seed, but could be different). Unknown what this is actually used for.
	ret.set_allocated_region_info(region);
	if (config->regionInfo->sendStopServerOrForceUpdate == 1 && config->regionInfo->stopServer != NULL) {
		stop = new proto::StopServerInfo;
		stop->set_stop_begin_time(config->regionInfo->stopServer->start);
		stop->set_stop_end_time(config->regionInfo->stopServer->end);
		if (config->regionInfo->stopServer->url != NULL) {
			stop->set_url(config->regionInfo->stopServer->url);
		}
		if (config->regionInfo->stopServer->msg != NULL) {
			stop->set_content_msg(config->regionInfo->stopServer->msg);
		}
		ret.set_allocated_stop_server(stop);
	}
	if (config->regionInfo->sendStopServerOrForceUpdate == 2 && config->regionInfo->forceUpdateUrl != NULL) {
		upd = new proto::ForceUpdateInfo;
		upd->set_force_update_url(config->regionInfo->forceUpdateUrl);
		ret.set_allocated_force_udpate(upd);
	}
	/* Unknown Fields TODO
		* client_secret_key - ECB seed struct. At least on Yuuki, this is actually different from the one given in the region list. Unknown what this is actually used for, but it's likely that, once derived, it's the same as the secretKey we already have. (On the other hand, on Grasscutter, this *is* the same as the region list seed.) Notably, if unset, GetPlayerTokenReq/Rsp isn't encrypted (or is at least processed with a null key).
		* region_custom_config_encrypted - unknown JSON object (I think) encrypted with either the region list client_secret_key or the one from this message (idk which). Also unknown how/if it differs from the one below, or with the one from query_cur_region
		* client_region_custom_config_encrypted - unknown JSON object (I think) encrypted with either the region list client_secret_key or the one from this message (idk which). Also unknown how/if it differs from the one above, or with the one from query_cur_region
	*/
build_rsp:
	if (!ret.SerializeToString(&ret_enc)) {
		// Bypass Protobuf and encode a response ourselves. Note that this eventually gets base64 encoded, hence the raw hex string.
		ret_enc = ""; // TODO build a default response
	}
	size_t sz = ret_enc.size();
	size_t bufsz = ((sz / 256) + 1) * 256;
	ret_enc.resize(bufsz, '\0');
	size_t sign_sz = 4096;
	unsigned char sign_c[sign_sz];
	int j;
	if (doSign) {
		j = HyvCryptRsaSign((unsigned char*) ret_enc.c_str(), sz, sign_c, sign_sz);
		if (j < 0) {
			sign = "";
		}
		else {
			sign.assign((const char*) sign_c, (unsigned int) j);
		}
	}
	if (!doEnc) {
		ret_enc.resize(sz);
		return ret_enc;
	}
	// Default to twice the size of the buffer to account for some crazy small keys.
	unsigned char* tmpbuf = (unsigned char*) malloc(bufsz * 2);
	if (tmpbuf == NULL) {
		ret_enc.resize(sz);
		return ret_enc; // fall back to unencrypted buffer
	}
	j = HyvCryptRsaEnc((unsigned char*) ret_enc.c_str(), sz, tmpbuf, bufsz * 2, doEnc);
	if (j < 0) {
		free(tmpbuf);
		ret_enc.resize(sz);
		return ret_enc;
	}
	ret_enc.assign((const char*) tmpbuf, j);
	free(tmpbuf);
	return ret_enc;
}

// /session/new (/hk4e_{cn,global}/mdk/shield/api/login)
std::string handleLogin(const char* post) {
	if (post == NULL) {
		return "{\"retcode\":-103,\"message\":\"Login failure: `post` is NULL\"}";
	}
	size_t post_len = strlen(post) + 1;
	struct json_tokener* jtk = json_tokener_new();
	if (jtk == NULL) {
		return "{\"retcode\":-103,\"message\":\"Login failure: ubable to allocate JSON tokener\"}";
	}
	struct json_object* jobj = json_tokener_parse_ex(jtk, post, post_len);
	if (jobj == NULL) {
		enum json_tokener_error jerr = json_tokener_get_error(jtk);
		if (jerr != json_tokener_continue) {
			fprintf(stderr, "Error parsing JSON POST data: %s\n", json_tokener_error_desc(jerr));
		}
		json_tokener_free(jtk);
		return "{\"retcode\":-101,\"message\":\"Login failure: error parsing JSON data\"}";
	}
	if (json_tokener_get_parse_end(jtk) < post_len) {
		fprintf(stderr, "Warning: JSON in POST data has extra trailing data, it will be ignored\n");
	}
	json_tokener_free(jtk);
	if (!json_object_is_type(jobj, json_type_object)) {
		fprintf(stderr, "Error: JSON in POST data is not an object.\n");
		json_object_put(jobj);
		return "{\"retcode\":-101,\"message\":\"Login failure: JSON data is not an object\"}";
	}
	struct json_object* dobj;
	unsigned int isGuest = 0;
	if (json_object_object_get_ex(jobj, "is_guest", &dobj)) {
		isGuest = json_object_get_boolean(dobj);
	}
	const char* deviceId = NULL;
	if (json_object_object_get_ex(jobj, "device_id", &dobj)) {
		deviceId = json_object_get_string(dobj);
	}
	Account* account;
	if (isGuest) {
		if (deviceId == NULL) {
			json_object_put(jobj);
			return "{\"retcode\":-101,\"message\":\"Device ID is null\"}";
		}
		if (strlen(deviceId) == 0) {
			json_object_put(jobj);
			return "{\"retcode\":-101,\"message\":\"Device ID is empty\"}";
		}
		account = globalDbGate->getAccountByDeviceId(deviceId);
	}
	else {
		if (!json_object_object_get_ex(jobj, "account", &dobj)) {
			json_object_put(jobj);
			return "{\"retcode\":-101,\"message\":\"Username is not set\"}";
		}
		const char* username = json_object_get_string(dobj);
		if (username == NULL) {
			json_object_put(jobj);
			return "{\"retcode\":-101,\"message\":\"Username is null\"}";
		}
		if (strlen(username) == 0) {
		json_object_put(jobj);
			return "{\"retcode\":-101,\"message\":\"Username is empty\"}";
		}
		account = globalDbGate->getAccountByUsername(username);
	}
	if (account == NULL) {
		// TODO look up auto create an account in config
		json_object_put(jobj);
		return "{\"retcode\":-101,\"message\":\"Username does not exist\"}";
	}
	if (!json_object_object_get_ex(jobj, "password", &dobj)) {
		json_object_put(jobj);
		return "{\"retcode\":-101,\"message\":\"Password is not set\"}";
	}
	const char* password = json_object_get_string(dobj);
	if (password == NULL) {
		json_object_put(jobj);
		return "{\"retcode\":-101,\"message\":\"Password is null\"}";
	}
	unsigned int isCrypto = 0;
	if (json_object_object_get_ex(jobj, "is_crypto", &dobj)) {
		isCrypto = json_object_get_boolean(dobj);
	}
	// TODO look up verify password in config
#if 0
	char encPassword[1024];
	memset(encPassword, '\0', 1024);
	if (isCrypto) {
		// TODO HyvCryptRsaDec
	}
	// TODO look up encrypt password in config
	// TODO HMAC-SHA256
	if (strcmp(encPassword, account->getPasswordHash().c_str()) != 0) {
		json_object_put(jobj);
		return "{\"retcode\":-101,\"message\":\"Incorrect password\"}";
	}
#endif
	std::string sessionKey = account->getNewSessionKey();
	if (deviceId != NULL) {
		account->setDeviceId(deviceId);
	}
	globalDbGate->saveAccount(*account);
	std::string ret;
	json_object* njobj = json_object_new_object();
	if (njobj == NULL) {
		json_object_put(jobj);
		return "{\"retcode\":-103,\"message\":\"Login failure: ubable to allocate JSON object\"}";
	}
	json_object_object_add(njobj, "uid", json_object_new_uint64(account->getAccountId()));
	if (isGuest) {
		json_object_object_add(njobj, "account_type", json_object_new_uint64(0));
		ret = "{\"retcode\":0,\"message\":\"ok\",\"data\":";
		ret += json_object_to_json_string_ext(njobj, JSON_C_TO_STRING_PLAIN);
		ret += "}";
	}
	else {
		// TODO Official servers mask name and email for privacy reasons.
		json_object_object_add(njobj, "name", json_object_new_string(account->getUsername().c_str()));
		json_object_object_add(njobj, "email", json_object_new_string(account->getEmail().c_str()));
		json_object_object_add(njobj, "is_email_verify", json_object_new_boolean(0)); // TODO Unknown what this does.
		json_object_object_add(njobj, "token", json_object_new_string(sessionKey.c_str()));
		// Note: country and area_code might get rewritten by PHP, per the client IP address.
		json_object_object_add(njobj, "country", json_object_new_string("ZZ"));
		json_object_object_add(njobj, "area_code", NULL);
		ret = "{\"retcode\":0,\"message\":\"ok\",\"data\":{\"account\":";
		ret += json_object_to_json_string_ext(njobj, JSON_C_TO_STRING_PLAIN);
		// Not sure what any of these do.
		ret += ",\"device_grant_required\":false,\"realname_operation\":null,\"realperson_required\":false,\"safe_mobile_required\":false}}";
	}
	json_object_put(njobj);
	json_object_put(jobj);
	return ret;
}

// /session/verify (/hk4e_{cn,global}/mdk/shield/api/verify)
std::string handleVerify(const char* post) {
	if (post == NULL) {
		return "{\"retcode\":-103,\"message\":\"Login failure: `post` is NULL\"}";
	}
	size_t post_len = strlen(post) + 1;
	struct json_tokener* jtk = json_tokener_new();
	unsigned int aid = 0;
	if (jtk == NULL) {
		return "{\"retcode\":-103,\"message\":\"Login failure: ubable to allocate JSON tokener\"}";
	}
	struct json_object* jobj = json_tokener_parse_ex(jtk, post, post_len);
	if (jobj == NULL) {
		enum json_tokener_error jerr = json_tokener_get_error(jtk);
		if (jerr != json_tokener_continue) {
			fprintf(stderr, "Error parsing JSON POST data: %s\n", json_tokener_error_desc(jerr));
		}
		json_tokener_free(jtk);
		return "{\"retcode\":-101,\"message\":\"Login failure: error parsing JSON data\"}";
	}
	if (json_tokener_get_parse_end(jtk) < post_len) {
		fprintf(stderr, "Warning: JSON in POST data has extra trailing data, it will be ignored\n");
	}
	json_tokener_free(jtk);
	if (!json_object_is_type(jobj, json_type_object)) {
		fprintf(stderr, "Error: JSON in POST data is not an object.\n");
		json_object_put(jobj);
		return "{\"retcode\":-101,\"message\":\"Login failure: JSON data is not an object\"}";
	}
	struct json_object* dobj;
	const char* deviceId = NULL;
	if (json_object_object_get_ex(jobj, "device_id", &dobj)) {
		deviceId = json_object_get_string(dobj);
	}
	if (!json_object_object_get_ex(jobj, "uid", &dobj)) {
		json_object_put(jobj);
		return "{\"retcode\":-101,\"message\":\"Account ID is not set\"}";
	}
	aid = json_object_get_int(dobj);
	if (!json_object_object_get_ex(jobj, "token", &dobj)) {
		json_object_put(jobj);
		return "{\"retcode\":-101,\"message\":\"Token is not set\"}";
	}
	const char* token = json_object_get_string(dobj);
	if (token == NULL) {
		json_object_put(jobj);
		return "{\"retcode\":-101,\"message\":\"Token is null\"}";
	}
	if (strlen(token) == 0) {
		json_object_put(jobj);
		return "{\"retcode\":-101,\"message\":\"Token is empty\"}";
	}
	Account* account = globalDbGate->getAccountByAid(aid);
	if (account == NULL) {
		json_object_put(jobj);
		return "{\"retcode\":-101,\"message\":\"Account ID does not exist\"}";
	}
	if (strcmp(token, account->getSessionKey().c_str()) != 0) {
		json_object_put(jobj);
		return "{\"retcode\":-101,\"message\":\"Cached token does not match\"}";
	}
#if 0
	if (deviceId != NULL && strcmp(deviceId, account->getDeviceId().c_str()) != 0) {
		json_object_put(jobj);
		return "{\"retcode\":-101,\"message\":\"Token was created on another device\"}";
	}
#endif
	// TODO only do this if the config tells us to
#if 0
	// according to config...
	// regen the token
	token = account->getNewSessionKey().c_str();
	// or only regen the timestamp
	account->setSessionKeyTimestamp();
	globalDbGate->saveAccount(*account);
#endif
	std::string ret;
	json_object* njobj = json_object_new_object();
	if (njobj == NULL) {
		json_object_put(jobj);
		return "{\"retcode\":-103,\"message\":\"Login failure: ubable to allocate JSON object\"}";
	}
	json_object_object_add(njobj, "uid", json_object_new_uint64(aid));
	// TODO Official servers mask name and email for privacy reasons.
	json_object_object_add(njobj, "name", json_object_new_string(account->getUsername().c_str()));
	json_object_object_add(njobj, "email", json_object_new_string(account->getEmail().c_str()));
	json_object_object_add(njobj, "is_email_verify", json_object_new_boolean(0)); // TODO Unknown what this does.
	json_object_object_add(njobj, "token", json_object_new_string(token));
	// Note: country and area_code might get rewritten by PHP, per the client IP address.
	json_object_object_add(njobj, "country", json_object_new_string("ZZ"));
	json_object_object_add(njobj, "area_code", NULL);
	ret = "{\"retcode\":0,\"message\":\"ok\",\"data\":{\"account\":";
	ret += json_object_to_json_string_ext(njobj, JSON_C_TO_STRING_PLAIN);
	ret += "}}";
	json_object_put(njobj);
	json_object_put(jobj);
	return ret;
}

// /session/combo (/hk4e_{cn,global}/combo/granter/login/v2/login)
std::string handleCombo(const char* post) {
	if (post == NULL) {
		return "{\"retcode\":-103,\"message\":\"Login failure: `post` is NULL\"}";
	}
	size_t post_len = strlen(post) + 1;
	struct json_tokener* jtk = json_tokener_new();
	enum json_tokener_error jerr;
	unsigned int aid = 0;
	if (jtk == NULL) {
		return "{\"retcode\":-103,\"message\":\"Login failure: ubable to allocate JSON tokener\"}";
	}
	struct json_object* jobj = json_tokener_parse_ex(jtk, post, post_len);
	if (jobj == NULL) {
		jerr = json_tokener_get_error(jtk);
		if (jerr != json_tokener_continue) {
			fprintf(stderr, "Error parsing JSON POST data: %s\n", json_tokener_error_desc(jerr));
		}
		json_tokener_free(jtk);
		return "{\"retcode\":-101,\"message\":\"Login failure: error parsing JSON data\"}";
	}
	if (json_tokener_get_parse_end(jtk) < post_len) {
		fprintf(stderr, "Warning: JSON in POST data has extra trailing data, it will be ignored\n");
	}
	json_tokener_free(jtk);
	if (!json_object_is_type(jobj, json_type_object)) {
		fprintf(stderr, "Error: JSON in POST data is not an object.\n");
		json_object_put(jobj);
		return "{\"retcode\":-101,\"message\":\"Login failure: JSON data is not an object\"}";
	}
	struct json_object* dobj2;
	struct json_object* dobj;
	const char* data_str = NULL;
	size_t data_str_len;
	if (!json_object_object_get_ex(jobj, "data", &dobj2)) {
		json_object_put(jobj);
		return "{\"retcode\":-101,\"message\":\"Account data is not set\"}";
	}
	data_str = json_object_get_string(dobj2);
	data_str_len = json_object_get_string_len(dobj2);
	if (data_str == NULL) {
		json_object_put(jobj);
		return "{\"retcode\":-101,\"message\":\"Account data is not set\"}";
	}
	jtk = json_tokener_new();
	if (jtk == NULL) {
		return "{\"retcode\":-103,\"message\":\"Login failure: ubable to allocate JSON tokener\"}";
	}
	dobj2 = json_tokener_parse_ex(jtk, data_str, data_str_len);
	if (dobj2 == NULL) {
		jerr = json_tokener_get_error(jtk);
		if (jerr != json_tokener_continue) {
			fprintf(stderr, "Error parsing JSON POST data: %s\n", json_tokener_error_desc(jerr));
		}
		json_tokener_free(jtk);
		json_object_put(jobj);
		return "{\"retcode\":-101,\"message\":\"Login failure: error parsing JSON data\"}";
	}
	if (json_tokener_get_parse_end(jtk) < data_str_len) {
		fprintf(stderr, "Warning: JSON in POST data has extra trailing data, it will be ignored\n");
	}
	json_tokener_free(jtk);
	if (!json_object_is_type(dobj2, json_type_object)) {
		fprintf(stderr, "Error: JSON in POST data is not an object.\n");
		json_object_put(jobj);
		json_object_put(dobj2);
		return "{\"retcode\":-101,\"message\":\"Login failure: JSON data is not an object\"}";
	}
	if (!json_object_object_get_ex(dobj2, "uid", &dobj)) {
		json_object_put(jobj);
		json_object_put(dobj2);
		return "{\"retcode\":-101,\"message\":\"Account ID is not set\"}";
	}
	aid = json_object_get_int(dobj);
	unsigned int isGuest = 0;
	const char* deviceId = NULL;
	const char* token = NULL;
	if (json_object_object_get_ex(jobj, "device", &dobj)) {
		deviceId = json_object_get_string(dobj2);
	}
	if (json_object_object_get_ex(dobj2, "guest", &dobj)) {
		isGuest = json_object_get_boolean(dobj);
	}
	if (json_object_object_get_ex(dobj2, "token", &dobj)) {
		token = json_object_get_string(dobj);
	}
	if (isGuest) {
		if (deviceId == NULL) {
			json_object_put(jobj);
			json_object_put(dobj2);
			return "{\"retcode\":-101,\"message\":\"Device ID is null\"}";
		}
		if (strlen(deviceId) == 0) {
			json_object_put(jobj);
			json_object_put(dobj2);
			return "{\"retcode\":-101,\"message\":\"Device ID is empty\"}";
		}
	}
	else {
		if (token == NULL) {
			json_object_put(jobj);
			json_object_put(dobj2);
			return "{\"retcode\":-101,\"message\":\"Token is null\"}";
		}
		if (strlen(token) == 0) {
			json_object_put(jobj);
			json_object_put(dobj2);
			return "{\"retcode\":-101,\"message\":\"Token is empty\"}";
		}
	}
	Account* account = globalDbGate->getAccountByAid(aid);
	if (account == NULL) {
		json_object_put(jobj);
		json_object_put(dobj2);
		return "{\"retcode\":-101,\"message\":\"Account ID does not exist\"}";
	}
	if (!isGuest) {
		if (strcmp(token, account->getSessionKey().c_str()) != 0) {
			json_object_put(jobj);
			json_object_put(dobj2);
			return "{\"retcode\":-101,\"message\":\"Cached token does not match\"}";
		}
#if 0
		if (deviceId != NULL && strcmp(deviceId, account->getDeviceId().c_str()) != 0) {
			json_object_put(jobj);
			json_object_put(dobj2);
			return "{\"retcode\":-101,\"message\":\"Token was created on another device\"}";
		}
#endif
	}
	const char* comboToken = account->getNewToken().c_str();
	globalDbGate->saveAccount(*account);
	std::string ret;
	json_object* njobj = json_object_new_object();
	if (njobj == NULL) {
		json_object_put(jobj);
		json_object_put(dobj2);
		return "{\"retcode\":-103,\"message\":\"Login failure: ubable to allocate JSON object\"}";
	}
	json_object_object_add(njobj, "combo_token", json_object_new_string(comboToken));
	json_object_object_add(njobj, "open_id", json_object_new_uint64(aid));
	json_object_object_add(njobj, "account_type", json_object_new_int(isGuest ? 0 : 1));
	json_object_object_add(njobj, "data", json_object_new_string(isGuest ? "{\"guest\":true}" : "{\"guest\":false}"));
	json_object_object_add(njobj, "fatigue_remind", NULL); // CN only; shows in-game reminder if playing too long
	json_object_object_add(njobj, "heartbeat", json_object_new_boolean(0)); // CN only; forces game to send heartbeats so server can enforce maximum play time
	ret = "{\"retcode\":0,\"message\":\"ok\",\"data\":";
	ret += json_object_to_json_string_ext(njobj, JSON_C_TO_STRING_PLAIN);
	ret += "}";
	json_object_put(jobj);
	json_object_put(dobj2);
	return ret;
}

extern "C" {
	volatile int DispatchServerSignal = 0;

	enum {
		SDK_QUERY_REGION_LIST = 0,	// Get list of regions
		SDK_QUERY_CURR_REGION,		// Get region data
		SDK_GET_AUTH_TOKEN,			// Generate new auth token
		SDK_CHECK_AUTH_TOKEN,		// Verify auth token
		SDK_CHECK_COMBO_TOKEN,		// Verify auth token and send back combo token
		SDK_GACHA_CUR,				// Current banners
		SDK_GACHA_HIST,				// Gacha history
		SDK_REDEEM,					// Redeem gift codes and IAPs, also out-of-game rewards (hurichurl's treasure trove, daily check in, etc.)
		SDK_GM_CMD,					// Web-based GM command (Ayaka bot commands are handled as part of the in-game chat)
		SDK_STATS,					// Server-wide statistics
		SDK_GET_PLAYER_DATA,		// player data including in-game info (traveler's diary/battle chronicle) and also account details (useful for external dispatch servers)
		SDK_SET_PLAYER_DATA,		// set password/oath tokens or reset the account, also a low-level interface to directly set in-game data without going through gm
		SDK_ADMIN,					// Shut down or restart the server, manage the banlist, change abyss/gacha/event schedules
		SDK_ENDPOINT_CNT
	};

	const char* const sdkEndpoints[SDK_ENDPOINT_CNT] = {
		[SDK_QUERY_REGION_LIST] = "/query_region_list",
		[SDK_QUERY_CURR_REGION] = "/query_cur_region",
		[SDK_GET_AUTH_TOKEN] = "/session/new", // "short" for /hk4e_{cn,global}/mdk/shield/api/login
		[SDK_CHECK_AUTH_TOKEN] = "/session/verify", // "short" for /hk4e_{cn,global}/mdk/shield/api/verify
		[SDK_CHECK_COMBO_TOKEN] = "/session/combo", // "short" for /hk4e_{cn,global}/combo/granter/login/v2/login
		[SDK_GACHA_CUR] = "/gacha/details",
		[SDK_GACHA_HIST] = "/gacha/history",
		[SDK_REDEEM] = "/redeem",
		[SDK_GM_CMD] = "/gm",
		[SDK_STATS] = "/stat",
		[SDK_GET_PLAYER_DATA] = "/player/",
		[SDK_SET_PLAYER_DATA] = "/set-player/",
		[SDK_ADMIN] = "/admin",
	};

	// Backend for the dispatch server
	void* DispatchServerMain(__attribute__((unused)) void* __u) {
		const char* ip = "::";
		unsigned short port = 22100;
		int fd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
		int cfd = -1;
		int opt = 0;
		if (fd < 0) {
			fprintf(stderr, "Failed to create dispatch server socket, errno = %d (%s)\n", errno, strerror(errno));
			GameserverSignal = 1; // TODO enum constant
			return (void*) -1;
		}
		if (setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(int)) < 0) {
			fprintf(stderr, "Warning: Unable to unset option IPV6_V6ONLY (errno %d - %s).\nDispatch server might only be able to accept IPv6 clients.\n", errno, strerror(errno));
		}
		opt = 1;
		if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) < 0) {
			fprintf(stderr, "Warning: Unable to set option SO_REUSEADDR (errno %d - %s).\nIf this program crashes, you may need to wait to restart it until the kernel lets us use the port again.\n", errno, strerror(errno));
		}
		struct in_addr addr4 = {INADDR_ANY};
		struct in6_addr addr6 = IN6ADDR_ANY_INIT;
		sock_t srv, client;
		if (inet_pton(AF_INET6, ip, &addr6) == 0) {
			// v6 addr parse error. Try it with v4.
			if (inet_pton(AF_INET, ip, &addr4) != 1) {
				fprintf(stderr, "Failed to parse the bind address %s", ip);
				GameserverSignal = 1; // TODO enum constant
				return (void*) -1;
			}
			// Special logic for certain specific v4 addresses.
			// Check for an address in loopback range.
			if (ntohl(addr4.s_addr) >> 24 == 127) {
				// Set the last byte of addr6 to 1, indicating IPv6 loopback.
				addr6.s6_addr[15] = 1;
			}
			// Check if not INADDR_ANY
			else if (ntohl(addr4.s_addr) != INADDR_ANY) {
				// Create a v4-mapped v6 address. TODO does it actually work this way?
				memcpy(&addr6.s6_addr[12], &addr4, sizeof(struct in_addr));
				addr6.s6_addr[10] = 0xff;
				addr6.s6_addr[11] = 0xff;
			}
		}
		srv.sin6_family = AF_INET6;
		srv.sin6_port = htons(port);
		srv.sin6_addr = addr6;
		if (bind(fd, (struct sockaddr*) &srv, sizeof(sock_t)) < 0) {
			fprintf(stderr, "Failed to bind dispatch server socket to %s:%d, errno = %d (%s)\n", ip, port, errno, strerror(errno));
			close(fd);
			GameserverSignal = 1; // TODO enum constant
			return (void*) -1;
		}
		if (listen(fd, 8) < 0) {
			fprintf(stderr, "Failed to listen for connections on dispatch server socket, errno = %d (%s)\n", errno, strerror(errno));
			close(fd);
			GameserverSignal = 1; // TODO enum constant
			return (void*) -1;
		}
		fprintf(stderr, "Successfully bound dispatch backend server to %s:%d\n", ip, port);
		const size_t buf_len = 16 * 1024;
		const unsigned int _hdr_cnt = 128;
		size_t actual_len;
		size_t prev_len, meth_len, path_len;
		size_t hdr_cnt;
		ssize_t rd_len;
		int pret;
		unsigned int status = 200;
		static char pkt_buf[buf_len];
		const char* meth;
		const char* path;
		const char* body = NULL;
		struct phr_header headers[_hdr_cnt];
		int mver;
		unsigned int ssz = sizeof(sock_t);
		char cip[64];
		unsigned short cport;
		struct timeval tm;
		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(fd, &fds);
		while (!DispatchServerSignal) {
			// TODO Get from config (as ms)
			tm.tv_sec = 0;
			tm.tv_usec = 5000;
			FD_ZERO(&fds);
			FD_SET(fd, &fds);
			pret = 0;
			actual_len = 0;
			cfd = select(FD_SETSIZE, &fds, NULL, NULL, &tm);
			if (cfd <= 0) {
				if (cfd) fprintf(stderr, "Failed to check for a new connection, errno = %d (%s)\n", errno, strerror(errno));
				continue;
			}
			cfd = accept(fd, (struct sockaddr*) &client, &ssz);
			if (cfd < 0) {
				fprintf(stderr, "Failed to accept a new connection, errno = %d (%s)\n", errno, strerror(errno));
				continue;
			}
			inet_ntop(AF_INET6, client.sin6_addr.s6_addr, cip, 64);
			cport = ntohs(client.sin6_port);
			fprintf(stderr, "Accepted new connection from %s:%d\n", cip, cport);
			while (!DispatchServerSignal) {
				if (cfd < 0) break; // Quick safety check
				rd_len = recv(cfd, pkt_buf + actual_len, buf_len - actual_len, 0);
				if (rd_len == 0) {
					fprintf(stderr, "Client closed the connection before a response could be sent.\n");
					close(cfd);
					cfd = -1;
					break;
				}
				if (rd_len < 0) {
					fprintf(stderr, "Failed to read from the client, errno = %d (%s)\n", errno, strerror(errno));
					continue;
				}
				prev_len = actual_len;
				actual_len += rd_len;
				hdr_cnt = _hdr_cnt;
				pret = phr_parse_request(pkt_buf, buf_len, &meth, &meth_len, &path, &path_len, &mver, headers, &hdr_cnt, prev_len);
				if (pret >= -1) break; // -1 means parse error, this is tested for outside the read loop in order to send a 400 response to the client
				if (pret < -2) { // -2 means request hasn't finished yet
					fprintf(stderr, "HTTP parser returned unexpected error %d, **this should never happen!**\n", -pret);
					status = 500;
					goto write_rsp;
				}
				if (actual_len >= buf_len) {
					status = 413;
					goto write_rsp;
				}
			}
			if (cfd >= 0) {
				if (pret < 0) {
					status = 400;
					goto write_rsp;
				}
				if (strncmp(meth, "POST", 4) == 0) {
					body = pkt_buf + pret;
				}
				else {
					body = NULL;
				}
				if (strncmp(meth, "OPTIONS", 7) == 0) {
					status = 1204;
					goto write_rsp;
				}
				if (!(
					strncmp(meth, "GET", 3) == 0 ||
					strncmp(meth, "POST", 4) == 0 ||
					strncmp(meth, "HEAD", 4) == 0
				)) {
					status = 405;
					goto write_rsp;
				}
				// the extra space creates a distinction between the root directory and another path
				// the question mark, on the other hand, accounts for the possibility of GET parameters
				// there's gotta be a more graceful way to handle this
				if (strncmp(path, "/ ", 2) == 0 || strncmp(path, "/?", 2) == 0) {
					if (strncmp(meth, "POST", 4) == 0) status = 403;
					else status = 200;
					goto write_rsp;
				}
				// TODO are we really letting POST requests here?
				if (strncmp(path, "/generate_204", 13) == 0 || strncmp(path, "/gen_204", 8) == 0) {
					status = 204;
					goto write_rsp;
				}
				unsigned int i;
				for (i = 0; i < SDK_ENDPOINT_CNT; i++) {
					if (sdkEndpoints[i] == NULL) continue;
					if (strncmp(path, sdkEndpoints[i], strlen(sdkEndpoints[i])) == 0) {
						status = i;
						goto write_rsp;
					}
				}
				// everything else
				if (strncmp(path, "/", 1) == 0) {
					status = 404;
					goto write_rsp;
				}
				status = 400;
write_rsp:
				const char* rsp = "";
				const char* rsp_body = "";
				std::string rsp_str;
				std::string sign;
				size_t rsp_len = 0;
				const char* mime = "text/plain";
				const char* extra_hdrs = "";
				switch (status) {
				// Generic responses
				case 404:
				default:
					rsp = "Not Found";
					rsp_str = "{\"retcode\":-10,\"message\":\"No such endpoint\"}";
					mime = "application/json";
					rsp_body = rsp_str.c_str();
					rsp_len = rsp_str.size();
					break;
				case 403:
					rsp = "Forbidden";
					rsp_str = "{\"retcode\":-1,\"message\":\"Permission denied\"}";
					mime = "application/json";
					rsp_body = rsp_str.c_str();
					rsp_len = rsp_str.size();
					break;
				case 204:
					rsp = "No Content";
					mime = "text/plain";
					rsp_len = strlen(rsp_body);
					break;
				case 1204: // options method
					status = 204;
					rsp = "No Content";
					mime = "text/plain";
					rsp_len = strlen(rsp_body);
					extra_hdrs = "Allow: GET, HEAD, POST, OPTIONS\r\n";
					break;
				case 405:
					rsp = "Method Not Allowed";
					rsp_str = "{\"retcode\":-1,\"message\":\"HTTP method not allowed\"}";
					mime = "application/json";
					rsp_body = rsp_str.c_str();
					rsp_len = rsp_str.size();
					extra_hdrs = "Allow: GET, HEAD, POST, OPTIONS\r\n";
					break;
				case 500:
					rsp = "Internal Server Error";
					rsp_str = "{\"retcode\":-1,\"message\":\"Internal server error\"}";
					mime = "application/json";
					rsp_body = rsp_str.c_str();
					rsp_len = rsp_str.size();
					break;
				case 413:
					rsp = "Content Too Large";
					rsp_str = "{\"retcode\":-1,\"message\":\"Request too big\"}";
					mime = "application/json";
					rsp_body = rsp_str.c_str();
					rsp_len = rsp_str.size();
					break;
				case 200:
					rsp = "OK";
					rsp_str = "hi from yagips dispatch server\n";
					mime = "text/plain";
					rsp_body = rsp_str.c_str();
					rsp_len = rsp_str.size();
					break;
				// Endpoints - TODO implement them all
				case SDK_QUERY_REGION_LIST:
					// Notes: PHP frontend converts GET (or POST) parameters to JSON before sending it to us in a POST. This string is thus passed to getQueryRegionListHttpRsp.
					rsp_str = b64enc(getQueryRegionListHttpRsp(body));
					mime = "text/plain";
					rsp_body = rsp_str.c_str();
					rsp_len = rsp_str.size();
					break;
				case SDK_QUERY_CURR_REGION:
					// Notes: PHP frontend converts GET (or POST) parameters to JSON before sending it to us in a POST. This string is thus passed to getQueryCurrRegionHttpRsp.
					rsp_str = "{\"content\":\"" + b64enc(getQueryCurrRegionHttpRsp(sign, body)) + "\"";
					// TODO What to do if there's no signature?
					if (!sign.empty()) rsp_str += ",\"sign\":\"" + b64enc(sign) + "\"";
					rsp_str += "}";
					rsp_body = rsp_str.c_str();
					rsp_len = rsp_str.size();
					mime = "application/json";
					break;
				case SDK_GET_AUTH_TOKEN:
					rsp_str = handleLogin(body);
					mime = "application/json";
					rsp_body = rsp_str.c_str();
					rsp_len = rsp_str.size();
					break;
				case SDK_CHECK_AUTH_TOKEN:
					rsp_str = handleVerify(body);
					mime = "application/json";
					rsp_body = rsp_str.c_str();
					rsp_len = rsp_str.size();
					break;
				case SDK_CHECK_COMBO_TOKEN:
					rsp_str = handleCombo(body);
					mime = "application/json";
					rsp_body = rsp_str.c_str();
					rsp_len = rsp_str.size();
					break;
				// TODO
				case SDK_GACHA_HIST:
				case SDK_GET_PLAYER_DATA:
				// These additionally require out-of-game client authentication
				case SDK_SET_PLAYER_DATA:
				case SDK_REDEEM:
				case SDK_GM_CMD:
				case SDK_ADMIN:
				// TODO Wait to fully implement this until we can load config files
				case SDK_GACHA_CUR:
				// TODO Wait to fully implement this until we can gather and report statistics
				case SDK_STATS:
					rsp_str = "{\"retcode\":-1,\"message\":\"Endpoint not implemented yet, please be patient\"}";
					mime = "application/json";
					rsp_body = rsp_str.c_str();
					rsp_len = rsp_str.size();
					break;
				}
				if (status < SDK_ENDPOINT_CNT) {
					status = 200;
					rsp = "OK";
				}
				snprintf(pkt_buf, buf_len,
					"HTTP/1.%d %03d %s\r\n"
					"Content-Length: %ld\r\n"
					"Content-Type: %s\r\n"
					"Connection: close\r\n"
					// TODO Pull these from config.h
					"Server: yagips/0.0a\r\n%s"
					"\r\n%s",
				mver, status, rsp, rsp_len, mime, extra_hdrs, strncmp(meth, "HEAD", 4) == 0 ? "" : rsp_body);
				actual_len = 0;
				ssize_t wr_len = 0;
				prev_len = strnlen(pkt_buf, buf_len);
				while (prev_len > 0) {
					wr_len = send(cfd, pkt_buf + actual_len, prev_len, 0);
					if (wr_len < 0) {
						if (errno == EPIPE) {
							fprintf(stderr, "Client closed the connection before a response could be sent.\n");
						}
						else {
							fprintf(stderr, "Failed to send a response to the client, errno = %d (%s)\n", errno, strerror(errno));
						}
						close(cfd);
						cfd = -1;
						continue;
					}
					prev_len -= wr_len;
					actual_len += wr_len;
				}
				close(cfd);
				cfd = -1;
				continue;
			}
		}
		close(fd);
		GameserverSignal = 1; // TODO enum constant
		return (void*) 0;
	}
}
