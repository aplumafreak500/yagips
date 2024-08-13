/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2023 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include "toml.h"
#include "runconfig.h"

#define CONFIG_FILE_NAME "yagips.conf"

static char homePathBuf[4096];
static char tomlErrBuf[4096];

static const char* const pathList[11] = {
	NULL, // gets replaced by the path given in the constructer
	CONFIG_FILE_NAME,
	// next 5 entries are actually relative to the user's home directory.
	".yagips/config",
	".yagips/" CONFIG_FILE_NAME,
	".config/" CONFIG_FILE_NAME,
	"." CONFIG_FILE_NAME,
	CONFIG_FILE_NAME,
	// Next two entries are dynamic based on configure arguments
	CONFDIR "/config",
	CONFDIR "/" CONFIG_FILE_NAME,
	TOPCONFDIR "/" CONFIG_FILE_NAME,
	NULL, // End of list
};

static toml_table_t* rootConfig;

Config* globalConfig;

Config::Config(const char* path) {
	FILE* configFile;
	unsigned int i = 0;
	const char* _path;
	const char* homePath = getenv("HOME");
	struct passwd* pwd = getpwuid(getuid());
	homePathBuf[4095] = '\0';
	if (homePath == NULL) {
		if (pwd != NULL) {
			homePath = pwd->pw_dir;
		}
	}
	for (; i < 11; i++) {
		_path = pathList[i];
		if (i == 0) {
			if (path == NULL) continue;
			_path = path;
		}
		if (i >= 2 && i <= 6) {
			if (homePath == NULL) {
				i += 5;
				continue;
			}
			snprintf(homePathBuf, 4095, "%s/%s", homePath, pathList[i]);
			_path = homePathBuf;
		}
		if (_path == NULL) continue;
		// Open in binary mode, even on Windows systems, and despite that the config is (usually) text only.
		configFile = fopen(_path, "rb");
		if (configFile == NULL) continue;
		rootConfig = toml_parse_file(configFile, tomlErrBuf, 4096);
		if (rootConfig == NULL) {
			if (i == 0) {
				fprintf(stderr, "Warning: Cannot parse the file %s - %s\nA default config file will be loaded.\n", _path, tomlErrBuf);
			}
			fclose(configFile);
			configFile = NULL;
			continue;
		}
		break;
	}
	if (i >= 11 && configFile == NULL) {
		fprintf(stderr, "Warning: Cannot open any config files. Defaults will be used.\nConfig files are ususally loaded from a CLI argument, the current working directory, your home directory, or the directory %s.\n", CONFDIR);
		// TODO Drop a config file containing reasonable defaults into cwd and then reload it.
	}
	if (configFile != NULL) {
		// Close the config file, we don't need it anymore
		fclose(configFile);
		configFile = NULL;
	}
	if (rootConfig != NULL) {
		toml_value_t val;
		const toml_table_t* subConfig;
		// Section `core` - Core config and session params
		subConfig = toml_table_table(rootConfig, "core");
		if (subConfig != NULL) {
			// `core.maxSessions` - Session cap
			// Note: toml-c appears to do its own null checks, so no need to do it here.
			val = toml_table_int(subConfig, "maxSessions");
			config.maxSessions = val.ok ? val.u.i : 10;
			// `core.sessionTickRate` - Session tick rate
			val = toml_table_int(subConfig, "sessionTickRate");
			config.sessionTickRate = val.ok ? val.u.i : 500;
			// `core.dbPath` - Database path
			val = toml_table_string(subConfig, "dbPath");
			config.dbPath = val.ok ? val.u.s : VARDIR "/db.sqlite";
			// `core.ldbPath` - Database path (LevelDB)
			val = toml_table_string(subConfig, "ldbPath");
			config.ldbPath = val.ok ? val.u.s : VARDIR "/ldb";
			// `core.dataPath` - Server data path (keys, data tables, extra config)
			val = toml_table_string(subConfig, "dataPath");
			config.dataPath = val.ok ? val.u.s : DATADIR;
			// `core.logFile` - Log file path
			val = toml_table_string(subConfig, "logFile");
			config.logPath = val.ok ? val.u.s : LOGDIR "/yagips.log";
		}
		else {
			config.maxSessions = 10;
			config.sessionTickRate = 500;
			config.dbPath = VARDIR "/db.sqlite";
			config.ldbPath = VARDIR "/ldb";
			config.dataPath = DATADIR;
			config.logPath = LOGDIR "/yagips.log";
		}
		// Section `net` - Sockets and KCP
		subConfig = toml_table_table(rootConfig, "net");
		if (subConfig != NULL) {
			// `net.gameserverBindIp` - Bind address for the game server
			val = toml_table_string(subConfig, "gameserverBindIp");
			config.gameserver_bind_ip = val.ok ? val.u.s : "::";
			// `net.gameserverBindPort` - Bind port for the game server
			val = toml_table_int(subConfig, "gameserverBindPort");
			config.gameserver_bind_port = val.ok ? val.u.i : 22102;
			// `net.dispatchBindIp` - Bind address for the dispatch server
			val = toml_table_string(subConfig, "dispatchBindIp");
			config.dispatch_bind_ip = val.ok ? val.u.s : "::";
			// `net.dispatchPort` - Bind port for the dispatch server
			val = toml_table_int(subConfig, "dispatchBindPort");
			config.dispatch_bind_port = val.ok ? val.u.i : 22100;
			// `net.kcpInterval` - KCP interval
			val = toml_table_int(subConfig, "kcpInterval");
			config.kcpInterval = val.ok ? val.u.i : 100;
		}
		else {
			config.gameserver_bind_ip = "::";
			config.gameserver_bind_port = 22102;
			config.dispatch_bind_ip = "::";
			config.dispatch_bind_port = 22100;
			config.kcpInterval = 100;
		}
		// Section `game` - In-game TODO
		// `game.resinRefreshRate` - Time in seconds to replenish 1 Original Resin
		// `game.resinCap` - Max amount of Original Resin per player
		// `game.dailyResetTime` - UTC offset in seconds of daily server resets (influences domain availability, Welkin claims, mob and item respawns, and shop refreshes)
		// Section `dispatch` - Dispatch and auth
		subConfig = toml_table_table(rootConfig, "dispatch");
		if (subConfig != NULL) {
			config.regionInfo = (struct RegionInfo*) malloc(sizeof(struct RegionInfo));
			if (config.regionInfo != NULL) {
				// `dispatch.gateserverIp` - Address of the game server as reported by /query_region_list
				val = toml_table_string(subConfig, "gateserverIp");
				config.regionInfo->gateserverIp = val.ok ? val.u.s : "127.0.0.1";
				// `dispatch.gateserverPort` - Port of the game server as reported by /query_region_list (can be different from the local bind port due to NAT port forwarding, Docker containers, etc.)
				val = toml_table_int(subConfig, "gateserverPort");
				config.regionInfo->gateserverPort = val.ok ? val.u.i : 22102;
				// `dispatch.gateserverIpIsDomain` - Signals to the client that the gameserver IP is really a domain name.
				val = toml_table_bool(subConfig, "gateserverIpIsDomain");
				if (!val.ok) {
					// TODO Detect it ourselves by attempting to parse gateserverIp as an ipv4 or v6 address. If neither works, assume it's a domain.
				}
				else config.regionInfo->gateserverIpIsDomainName = val.u.b;
				// TODO If gateserverIpIsDomainName is false, run gateserverIp through inet_pton. If a valid ipv4 or ipv6 is detected, do no further checks. Else, run it through gethostbyname2. If there's a positive result, reset gateserverIp to the result of the name lookup, else, set gateserverIpIsDomainName to true anyways.
				// `dispatch.regionListSeed` - Ec2b initial seed for region list. Can be an integer or a path to a binary seed file. TODO
				// `dispatch.regionSeed` - Ec2b initial seed for region. Can be an integer or a path to a binary seed file. TODO
				// `dispatch.dispatchSeed` - Ec2b initial seed for dispatch. Can be an integer or a path to a binary seed file. TODO
				// All following `dispatch` fields are copied verbatim to the RegionInfo protobuf message.
				val = toml_table_string(subConfig, "feedbackUrl");
				config.regionInfo->feedbackUrl = val.ok ? val.u.s : NULL;
				val = toml_table_string(subConfig, "bulletinUrl");
				config.regionInfo->bulletinUrl = val.ok ? val.u.s : NULL;
				val = toml_table_string(subConfig, "handbookUrl");
				config.regionInfo->handbookUrl = val.ok ? val.u.s : NULL;
				val = toml_table_string(subConfig, "communityUrl");
				config.regionInfo->communityUrl = val.ok ? val.u.s : NULL;
				val = toml_table_string(subConfig, "userCenterUrl");
				config.regionInfo->userCenterUrl = val.ok ? val.u.s : NULL;
				val = toml_table_string(subConfig, "privacyPolicyUrl");
				config.regionInfo->privacyPolicyUrl = val.ok ? val.u.s : NULL;
				val = toml_table_string(subConfig, "accountBindUrl");
				config.regionInfo->accountBindUrl = val.ok ? val.u.s : NULL;
				val = toml_table_string(subConfig, "cdKeyUrl");
				config.regionInfo->cdKeyUrl = val.ok ? val.u.s : NULL;
				val = toml_table_string(subConfig, "payCbUrl");
				config.regionInfo->payCbUrl = val.ok ? val.u.s : NULL;
				val = toml_table_string(subConfig, "gameBiz");
				config.regionInfo->gameBiz = val.ok ? val.u.s : NULL;
				val = toml_table_string(subConfig, "areaType");
				config.regionInfo->areaType = val.ok ? val.u.s : NULL;
				val = toml_table_string(subConfig, "forceUpdateUrl");
				config.regionInfo->forceUpdateUrl = val.ok ? val.u.s : NULL;
				val = toml_table_string(subConfig, "regionCustomConfig");
				config.regionInfo->regionCustomConfig = val.ok ? val.u.s : NULL;
				val = toml_table_string(subConfig, "clientRegionCustomConfig");
				config.regionInfo->clientRegionCustomConfig = val.ok ? val.u.s : NULL;
				val = toml_table_int(subConfig, "sendStopServerOrForceUpdate");
				config.regionInfo->sendStopServerOrForceUpdate = val.ok ? val.u.i > 2u ? val.u.i : 0 : 0;
				// Section `dispatch.res` - ResVersionConfig
				const toml_table_t* dispatchSubConfig = toml_table_table(subConfig, "res");
				if (dispatchSubConfig != NULL) {
					config.regionInfo->res = (struct ResVersionConfig*) malloc(sizeof(struct ResVersionConfig));
					if (config.regionInfo->res != NULL) {
						val = toml_table_string(dispatchSubConfig, "resourceUrl");
						config.regionInfo->res->resourceUrl = val.ok ? val.u.s : NULL;
						val = toml_table_string(dispatchSubConfig, "dataUrl");
						config.regionInfo->res->dataUrl = val.ok ? val.u.s : NULL;
						val = toml_table_string(dispatchSubConfig, "resourceUrlBak");
						config.regionInfo->res->resourceUrlBak = val.ok ? val.u.s : NULL;
						val = toml_table_string(dispatchSubConfig, "dataUrlBak");
						config.regionInfo->res->dataUrlBak = val.ok ? val.u.s : NULL;
						val = toml_table_string(dispatchSubConfig, "resourceUrlNext");
						config.regionInfo->res->resourceUrlNext = val.ok ? val.u.s : NULL;
						val = toml_table_string(dispatchSubConfig, "resourceSuffix");
						config.regionInfo->res->resourceSuffix = val.ok ? val.u.s : NULL;
						val = toml_table_string(dispatchSubConfig, "dataSuffix");
						config.regionInfo->res->dataSuffix = val.ok ? val.u.s : NULL;
						val = toml_table_string(dispatchSubConfig, "silenceSuffix");
						config.regionInfo->res->silenceSuffix = val.ok ? val.u.s : NULL;
						val = toml_table_int(dispatchSubConfig, "resVersion");
						config.regionInfo->res->resVersion = val.ok ? val.u.i : 0;
						val = toml_table_int(dispatchSubConfig, "dataVersion");
						config.regionInfo->res->dataVersion = val.ok ? val.u.i : 0;
						val = toml_table_int(dispatchSubConfig, "silenceVersion");
						config.regionInfo->res->silenceVersion = val.ok ? val.u.i : 0;
						val = toml_table_string(dispatchSubConfig, "branch");
						config.regionInfo->res->branch = val.ok ? val.u.s : NULL;
						val = toml_table_string(dispatchSubConfig, "scriptVersion");
						config.regionInfo->res->scriptVersion = val.ok ? val.u.s : NULL;
						val = toml_table_string(dispatchSubConfig, "releaseTotalSize");
						config.regionInfo->res->releaseTotalSize = val.ok ? val.u.s : NULL;
						val = toml_table_int(dispatchSubConfig, "relogin");
						config.regionInfo->res->relogin = val.ok ? val.u.i : 0;
						// TODO These should be objects, not strings
						val = toml_table_string(dispatchSubConfig, "resourceRes");
						config.regionInfo->res->resourceRes = val.ok ? val.u.s : NULL;
						val = toml_table_string(dispatchSubConfig, "dataRes");
						config.regionInfo->res->dataRes = val.ok ? val.u.s : NULL;
						val = toml_table_string(dispatchSubConfig, "silenceRes");
						config.regionInfo->res->silenceRes = val.ok ? val.u.s : NULL;
					}
				}
				else config.regionInfo->res = NULL;
				// Section `dispatch.res.next` - NextResVersionConfig
				if (dispatchSubConfig != NULL) {
					dispatchSubConfig = toml_table_table(dispatchSubConfig, "next");
					if (dispatchSubConfig != NULL) {
						config.regionInfo->resNext = (struct ResVersionConfig*) malloc(sizeof(struct ResVersionConfig));
						if (config.regionInfo->resNext != NULL) {
							config.regionInfo->resNext->resourceUrl = config.regionInfo->res->resourceUrl;
							config.regionInfo->resNext->dataUrl = config.regionInfo->res->dataUrl;
							config.regionInfo->resNext->resourceUrlBak = config.regionInfo->res->resourceUrlBak;
							config.regionInfo->resNext->dataUrlBak = config.regionInfo->res->dataUrlBak;
							config.regionInfo->resNext->resourceUrlNext = config.regionInfo->res->resourceUrlNext;
							val = toml_table_string(dispatchSubConfig, "resourceSuffix");
							config.regionInfo->resNext->resourceSuffix = val.ok ? val.u.s : NULL;
							val = toml_table_string(dispatchSubConfig, "dataSuffix");
							config.regionInfo->resNext->dataSuffix = val.ok ? val.u.s : NULL;
							val = toml_table_string(dispatchSubConfig, "silenceSuffix");
							config.regionInfo->resNext->silenceSuffix = val.ok ? val.u.s : NULL;
							val = toml_table_int(dispatchSubConfig, "resVersion");
							config.regionInfo->resNext->resVersion = val.ok ? val.u.i : 0;
							val = toml_table_int(dispatchSubConfig, "dataVersion");
							config.regionInfo->resNext->dataVersion = val.ok ? val.u.i : 0;
							val = toml_table_int(dispatchSubConfig, "silenceVersion");
							config.regionInfo->resNext->silenceVersion = val.ok ? val.u.i : 0;
							val = toml_table_string(dispatchSubConfig, "branch");
							config.regionInfo->resNext->branch = val.ok ? val.u.s : NULL;
							val = toml_table_string(dispatchSubConfig, "scriptVersion");
							config.regionInfo->resNext->scriptVersion = val.ok ? val.u.s : NULL;
							val = toml_table_string(dispatchSubConfig, "releaseTotalSize");
							config.regionInfo->resNext->releaseTotalSize = val.ok ? val.u.s : NULL;
							val = toml_table_int(dispatchSubConfig, "relogin");
							config.regionInfo->resNext->relogin = val.ok ? val.u.i : 0;
							// TODO These should be objects, not strings
							val = toml_table_string(dispatchSubConfig, "resourceRes");
							config.regionInfo->resNext->resourceRes = val.ok ? val.u.s : NULL;
							val = toml_table_string(dispatchSubConfig, "dataRes");
							config.regionInfo->resNext->dataRes = val.ok ? val.u.s : NULL;
							val = toml_table_string(dispatchSubConfig, "silenceRes");
							config.regionInfo->resNext->silenceRes = val.ok ? val.u.s : NULL;
						}
					}
					else config.regionInfo->resNext = NULL;
				}
 				else config.regionInfo->resNext = NULL;
				// Section `dispatch.stop` - StopServer (maintenance period)
				dispatchSubConfig = toml_table_table(subConfig, "stop");
				if (dispatchSubConfig != NULL) {
					config.regionInfo->stopServer = (struct StopServer*) malloc(sizeof(struct StopServer));
					if (config.regionInfo->stopServer != NULL) {
						val = toml_table_int(dispatchSubConfig, "start");
						config.regionInfo->stopServer->start = val.ok ? val.u.i : 0;
						val = toml_table_int(dispatchSubConfig, "end");
						config.regionInfo->stopServer->end = val.ok ? val.u.i : 0;
						val = toml_table_string(dispatchSubConfig, "url");
						config.regionInfo->stopServer->url = val.ok ? val.u.s : NULL;
						// TODO: Also support an array of strings here for i18n purposes.
						val = toml_table_string(dispatchSubConfig, "msg");
						config.regionInfo->stopServer->msg = val.ok ? val.u.s : NULL;
					}
				}
				else config.regionInfo->stopServer = NULL;
				// Section array `dispatch.regions` - Region list
				const toml_array_t* dispatchRegions = toml_table_array(subConfig, "regions");
				if (dispatchRegions != NULL) {
					config.regionCnt = dispatchRegions->nitem;
					if (dispatchRegions->nitem > 0) {
						config.regions = (struct RegionListEntry**) malloc(sizeof(struct RegionListEntry*) * dispatchRegions->nitem);
						if (config.regions != NULL) {
							for (i = 0; i < (unsigned int) dispatchRegions->nitem; i++) {
								dispatchSubConfig = toml_array_table(dispatchRegions, i);
								if (dispatchSubConfig == NULL) {
									config.regions[i] = NULL;
									continue;
								}
								config.regions[i] = (struct RegionListEntry*) malloc(sizeof(struct RegionListEntry));
								if (config.regions[i] == NULL) continue;
								val = toml_table_string(dispatchSubConfig, "name");
								config.regions[i]->name = val.ok ? val.u.s : NULL;
								// TODO: Also support an array of strings here for i18n purposes.
								val = toml_table_string(dispatchSubConfig, "title");
								config.regions[i]->title = val.ok ? val.u.s : NULL;
								val = toml_table_string(dispatchSubConfig, "url");
								config.regions[i]->url = val.ok ? val.u.s : NULL;
								val = toml_table_string(dispatchSubConfig, "type");
								config.regions[i]->type = val.ok ? val.u.s : NULL;
								val = toml_table_string(dispatchSubConfig, "version");
								config.regions[i]->version = val.ok ? val.u.s : NULL;
							}
						}
					}
					else {
						config.regions = NULL;
					}
				}
				else {
					config.regions = NULL;
					config.regionCnt = 0;
				}
			}
		}
		else {
			config.regions = NULL;
			config.regionCnt = 0;
			config.regionInfo = NULL;
		}
		// Section `gacha` - Global gacha params TODO
		fprintf(stderr, "Sucessfully loaded config from %s\n", _path);
	}
	else {
		config.maxSessions = 10;
		config.sessionTickRate = 500;
		config.dbPath = VARDIR "/db.sqlite";
		config.ldbPath = VARDIR "/ldb";
		config.dataPath = DATADIR;
		config.logPath = LOGDIR "/yagips.log";
		config.gameserver_bind_ip = "::";
		config.gameserver_bind_port = 22102;
		config.dispatch_bind_ip = "::";
		config.dispatch_bind_port = 22100;
		config.kcpInterval = 100;
		config.regions = NULL;
		config.regionCnt = 0;
		config.regionInfo = NULL;
	}
}

Config::~Config() {
	unsigned int i;
	if (config.regionInfo != NULL) {
		if (config.regionInfo->res != NULL) free(config.regionInfo->res);
		if (config.regionInfo->resNext != NULL) free(config.regionInfo->resNext);
		if (config.regionInfo->stopServer != NULL) free(config.regionInfo->stopServer);
		free(config.regionInfo);
	}
	if (config.regions != NULL) {
		for (i = 0; i < config.regionCnt; i++) {
			if (config.regions[i] != NULL) free(config.regions[i]);
		}
		free(config.regions);
	}
	toml_free(rootConfig);
	memset(&config, 0, sizeof(config_t));
}

const config_t* Config::getConfig() const {
	return &config;
}
