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
#include "crypt.h"
#include "keys.h"
#include "login.pb.h"

int handlePlayerLoginReq(Session& session, std::string& header, std::string& data) {
	proto::PlayerLoginReq req;
	proto::PlayerLoginRsp rsp;
	Packet rsp_pkt(104);
	if (!req.ParseFromString(data)) {
		// TODO should we send a response packet?
		fprintf(stderr, "Error parsing packet data\n");
		return -1;
	}
	// reject sessions that haven't sent GetPlayerTokenReq yet
	const Account* account = session.getAccount();
	if (account == NULL) {
		// TODO Send a response packet first
		session.close(7);
		return -1;
	}
	// TODO do we need to do ban checks here?
	// unlike with GetPlayerTokenReq, this check is completely unconditional
	const char* token = req.token().c_str();
	std::string ctoken = account->getToken();
	if (ctoken.empty()) {
		// TODO Send a response packet first
		session.close(8);
		return -1;
	}
	if (strncmp(ctoken.c_str(), token, ctoken.size()) != 0) {
		// TODO Send a response packet first
		session.close(7);
		return -1;
	}
	Player* player = session.getPlayer();
	if (player == NULL) {
		// TODO Send a response packet first
		session.close(12);
		return -1;
	}
	// TODO once we implement proper avatar storage, send out DoSetPlayerBornDataNotify if avatar storage is empty. This triggers the opening cutscene in the client.
	// else, trigger player on login event, which sends various "notify" packets telling the client about the game state as of the last logout.
	rsp.set_retcode(0);
	// TODO Response packet contains ResVersionConfig info, hold off on sending it until we split the RegionInfo stuff from queryCurrRegionHttpRsp into a separate function
	// TODO hk4e_cn for Chinese clients
	rsp.set_game_biz("hk4e_global");
	// TODO either pull from account object or from GeoIP
	rsp.set_country_code("US");
	// Unknown exactly what these do.
	rsp.set_is_use_ability_hash(1);
	rsp.set_ability_hash_code(1844674);
	if (!rsp.SerializeToString(&data)) {
		fprintf(stderr, "Error building packet data\n");
		return -1;
	}
	rsp_pkt.setHeader(header);
	rsp_pkt.setData(data);
	static unsigned char rsp_buf[1024];
	size_t rsp_sz = 1024;
	if (rsp_pkt.build(rsp_buf, &rsp_sz) < 0) {
		fprintf(stderr, "Error building packet\n");
		return -1;
	}
	const unsigned char* key = NULL;
	// for whatever reason, despite req using the session key, this packet uses the dispatch key... weird
#if 0
	// TODO: verify that the key being used is in fact query_curr_region->client_secret_key before using this.
	else {
		if (hasDispatchKey) key = dispatchKey;
	}
#endif
	if (key != NULL) {
		HyvCryptXor(rsp_buf, rsp_sz, key, 4096);
	}
	if (session.getKcpSession()->send(rsp_buf, rsp_sz) < 0) {
		fprintf(stderr, "Error sending packet\n");
		return -1;
	}
	return 0;
}
