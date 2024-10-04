/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <string>
#include "packet.h"
#include "session.h"
#include "runconfig.h"
#include "player.pb.h"

int handlePlayerLoginReq(Session& session, std::string& header, std::string& data) {
	proto::PlayerLoginReq req;
	proto::PlayerLoginRsp rsp;
	proto::ResVersionConfig* res;
	proto::ResVersionConfig* resNext;
	Packet rsp_pkt(104);
	if (session.getState() != Session::LOGIN_WAIT) {
		// TODO should we send a response packet?
		fprintf(stderr, "Session state is not LOGIN_WAIT\n");
		return -1;
	}
	if (!req.ParseFromString(data)) {
		// TODO should we send a response packet?
		fprintf(stderr, "Error parsing packet data\n");
		return -1;
	}
	// reject sessions that haven't sent GetPlayerTokenReq yet
	const Account* account = session.getAccount();
	if (account == NULL) {
		fprintf(stderr, "No account associated with current session\n");
		// TODO Send a response packet first
		session.close(7);
		return -1;
	}
	// TODO do we need to do ban checks here?
	// unlike with GetPlayerTokenReq, this check is completely unconditional
	const char* token = req.token().c_str();
	std::string ctoken = account->getToken();
	if (ctoken.empty()) {
		fprintf(stderr, "Stored token is empty\n");
		// TODO Send a response packet first
		session.close(8);
		return -1;
	}
	if (strncmp(ctoken.c_str(), token, ctoken.size()) != 0) {
		fprintf(stderr, "Packet token does not match stored token\n");
		// TODO Send a response packet first
		session.close(7);
		return -1;
	}
	Player* player = session.getPlayer();
	if (player == NULL) {
		fprintf(stderr, "No player data associated with session\n");
		// TODO Send a response packet first
		session.close(12);
		return -1;
	}
	// TODO once we implement proper avatar storage, send out DoSetPlayerBornDataNotify if avatar storage is empty. This triggers the opening cutscene in the client.
	// else...
	player->onLogin(session);
	rsp.set_retcode(0);
	// TODO hk4e_cn for Chinese clients
	rsp.set_game_biz("hk4e_global");
	// TODO either pull from account object or from GeoIP
	rsp.set_country_code("US");
	// Unknown exactly what these do.
	rsp.set_is_use_ability_hash(1);
	rsp.set_ability_hash_code(1844674);
	const config_t* config = NULL;
	if (globalConfig != NULL) {
		config = globalConfig->getConfig();
	}
	if (config != NULL && config->regionInfo != NULL) {
		if (config->regionInfo->res != NULL) {
			rsp.set_client_data_version(config->regionInfo->res->dataVersion);
			if (config->regionInfo->res->dataSuffix != NULL) {
				rsp.set_client_version_suffix(config->regionInfo->res->dataSuffix);
			}
			if (config->regionInfo->res->dataRes != NULL) {
				rsp.set_client_md5(config->regionInfo->res->dataRes);
			}
			rsp.set_client_silence_data_version(config->regionInfo->res->silenceVersion);
			if (config->regionInfo->res->silenceSuffix != NULL) {
				rsp.set_client_silence_version_suffix(config->regionInfo->res->silenceSuffix);
			}
			if (config->regionInfo->res->silenceRes != NULL) {
				rsp.set_client_silence_md5(config->regionInfo->res->silenceRes);
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
			rsp.set_is_relogin(config->regionInfo->res->relogin);
			rsp.set_allocated_res_version_config(res);
		}
		if (config->regionInfo->resNext != NULL) {
			if (config->regionInfo->resNext->resourceUrl != NULL) {
				rsp.set_next_resource_url(config->regionInfo->resNext->resourceUrl);
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
			rsp.set_allocated_next_res_version_config(resNext);
		}
	}
	if (!rsp.SerializeToString(&data)) {
		fprintf(stderr, "Error building packet data\n");
		return -1;
	}
	rsp_pkt.setHeader(header);
	rsp_pkt.setData(data);
	// for whatever reason, despite req using the session key, this packet uses the dispatch key... weird
	rsp_pkt.setUseDispatchKey(1);
	int ret = session.sendPacket(rsp_pkt);
	if (!ret) session.setState(Session::ACTIVE);
	return ret;
}
