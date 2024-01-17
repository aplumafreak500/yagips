/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2023 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#ifndef CONFIG_H
#define CONFIG_H
#include "buildconf.h"
#include <stddef.h>
#include <string>

// these structs are used as an intermediary between Protobuf and TOML
struct RegionListEntry {
	const char* name;
	const char* type;
	const char* title; // TODO: Also support an array of strings here for i18n purposes.
	const char* url;
	const char* version; // Not in the proto. Instead, it's used to build query_region_list based on reported client version
};

struct ResMd5 {
	const char* remote_name;
	const char* md5;
	size_t fileSize;
};

struct ResVersionConfig {
	const char* resourceUrl;
	const char* dataUrl;
	const char* resourceUrlBak;
	const char* resourceUrlNext;
	const char* dataUrlBak;
	const char* resourceSuffix;
	const char* dataSuffix;
	const char* silenceSuffix;
	const char* branch;
	const char* scriptVersion;
	unsigned int resVersion;
	unsigned int dataVersion;
	unsigned int silenceVersion;
	const char* resourceRes;
	const char* dataRes;
	const char* silenceRes;
	// TODO Below 4 should be used instead of above 3
	// struct ResMd5** resourceRes;
	// size_t resourceResCnt;
	// struct ResMd5* dataRes;
	// struct ResMd5* silenceRes;
	unsigned int relogin;
	const char* releaseTotalSize;
};

struct StopServer {
	unsigned long long start;
	unsigned long long end;
	const char* url;
	const char* msg; // TODO: Also support an array of strings here for i18n purposes.
};

struct RegionInfo {
	const char* gateserverIp;
	unsigned short gateserverPort;
	unsigned int gateserverIpIsDomainName;
	const char* feedbackUrl;
	const char* bulletinUrl;
	const char* handbookUrl;
	const char* communityUrl;
	const char* userCenterUrl;
	const char* privacyPolicyUrl;
	const char* accountBindUrl;
	const char* cdKeyUrl;
	const char* payCbUrl;
	struct ResVersionConfig* res;
	struct ResVersionConfig* resNext;
	const char* gameBiz;
	const char* areaType;
	const char* forceUpdateUrl;
	const char* regionCustomConfig;
	const char* clientRegionCustomConfig;
	unsigned int sendStopServerOrForceUpdate;
	struct StopServer* stopServer;
};

// Root config layer
typedef struct {
	const char* gameserver_bind_ip;
	const char* dispatch_bind_ip;
	unsigned short gameserver_bind_port;
	unsigned short dispatch_bind_port;
	unsigned int maxSessions;
	unsigned int kcpInterval;
	unsigned int sessionTickRate;
	const char* dbPath;
	const char* dataPath;
	const char* logPath;
	size_t regionCnt;
	struct RegionListEntry** regions;
	struct RegionInfo* regionInfo;
} config_t;

class Config {
public:
	Config(const char* path = NULL);
	~Config();
	const config_t* getConfig() const;
private:
	config_t config;
};

extern Config* globalConfig;
#endif
