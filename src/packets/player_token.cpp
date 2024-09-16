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
#include "player.pb.h"
#include <stdlib.h>
#include <errno.h>
#include "account.h"
#include "dbgate.h"
#include "keys.h"

int handleGetPlayerTokenReq(Session& session, std::string&, std::string& data) {
	proto::GetPlayerTokenReq req;
	proto::GetPlayerTokenRsp rsp;
	unsigned long long encryptSeed;
	Packet rsp_pkt(102);
	if (session.getState() != Session::TOKEN_WAIT) {
		fprintf(stderr, "Session state is not TOKEN_WAIT\n");
		// TODO should we send a response packet?
		return -1;
	}
	if (!req.ParseFromString(data)) {
		fprintf(stderr, "Error parsing packet data\n");
		// TODO should we send a response packet?
		return -1;
	}
	const char* aid_str = req.account_uid().c_str();
	char* aid_str_tail;
	errno = 0;
	unsigned int aid = strtoul(aid_str, &aid_str_tail, 0);
	if (errno == ERANGE) {
		fprintf(stderr, "Warning: Parsing UID resulted in overflow\n");
	}
	if ((long) aid_str == (long) aid_str_tail) {
		fprintf(stderr, "Error parsing UID\n");
		// TODO Send a response packet first
		session.close(12);
		return -1;
	}
	if (*aid_str_tail != '\0') {
		fprintf(stderr, "Warning: Trailing data in UID string\n");
	}
	const char* token = req.account_token().c_str();
	Account* account = globalDbGate->getAccountByAid(aid);
	if (account == NULL) {
		fprintf(stderr, "Account with id %d does not exist\n", aid);
		// TODO Send a response packet first
		session.close(7);
		return -1;
	}
	// TODO aid-level bans are handled here (in addition to dispatch responses)
	// TODO Config entry to check whether the token matches the account. Disabling it would let external dispatch servers work without needing to sync account data between them. yagips and Grasscutter use different protocols for this, and extra work would need to be done for GIO's SDK server to exchange account data.
	// For now, we enforce it so everything can be in one place until the relevant protocols and storage structs are finalized.
	// TODO If that config entry is disabled, we need to sync the token to db so that PlayerLoginReq works properly
#if 1
	std::string ctoken = account->getToken();
	if (ctoken.empty()) {
		fprintf(stderr, "Stored account token is empty\n");
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
#endif
	Player* player = globalDbGate->getPlayerByAid(aid);
	if (player != NULL) {
		Session* oldSession = getSessionByUid(player->getUid());
		if (oldSession != NULL) {
			oldSession->close(4);
		}
	}
	// TODO enforce uid-level bans
	// TODO Enforce max player limit, distinct from max session limit
	// If player is still null, it means this account doesn't have player data at all, so we need to create some.
	if (player == NULL) {
		player = globalDbGate->newPlayer();
		if (player == NULL) {
			fprintf(stderr, "Failed to create new player data\n");
			// TODO Send a response packet first
			session.close(12);
			return -1;
		}
		player->setAccount(account);
		player->saveToDb();
	}
	session.setPlayer(player);
	session.generateSessionKey();
	encryptSeed = session.getSessionSeed();
	session.setUseSecretKey();
	session.setState(Session::LOGIN_WAIT);
	rsp.set_retcode(0);
	rsp.set_msg("ok");
	rsp.set_uid(player->getUid());
	rsp.set_token(account->getToken());
	// TODO Check account if guest
	rsp.set_account_type(1);
	rsp.set_channel_id(1);
	// TODO Pull from session object?
	rsp.set_platform_type(3);
	// TODO obtained from kcpSession->getClientIPAddress()
	// rsp.set_client_ip_str();
	// TODO either pull from account object or from GeoIP
	rsp.set_country_code("US");
	// TODO Grasscutter checks for avatar count (ie. whether you've created the Traveler's avatar object or not) to determine if this should be set. Figure out if we should do the same.
	rsp.set_is_proficient_player(0);
	rsp.set_secret_key_seed(encryptSeed);
	std::string encryptSeedBuf((const char*) secretKeyBuf, 32);
	rsp.set_security_cmd_buffer(encryptSeedBuf);
	if (!rsp.SerializeToString(&data)) {
		fprintf(stderr, "Error building packet data\n");
		return -1;
	}
	rsp_pkt.buildHeader(session.nextSeq());
	rsp_pkt.setData(data);
	rsp_pkt.setUseDispatchKey(1);
	return session.sendPacket(rsp_pkt);
}
