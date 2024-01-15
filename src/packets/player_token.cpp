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
#include "packet_head.pb.h"
#include "player_token.pb.h"
#include <stdlib.h>
#include <errno.h>
#include "account.h"
#include "dbgate.h"
#include "crypt.h"
#include "keys.h"

int handleGetPlayerTokenReq(Session& session, std::string& header, std::string& data) {
	proto::PacketHead pkt_head;
	proto::GetPlayerTokenReq req;
	proto::GetPlayerTokenRsp rsp;
	// TODO Move to session class and also randomize it
	const unsigned long long encryptSeed = 0x9f26b217615fc8d0;
	Packet rsp_pkt(198);
	// TODO Set use dispatch key
	if (!pkt_head.ParseFromString(header)) {
		fprintf(stderr, "Error parsing packet header\n");
		return -1;
	}
	if (!req.ParseFromString(data)) {
		fprintf(stderr, "Error parsing packet data\n");
		// TODO should we send a response packet?
		return -1;
	}
	const char* aid_str = rsp.account_uid().c_str();
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
		// TODO Send a response packet first
		session.close(7);
		return -1;
	}
	// TODO aid-level bans are handled here (in addition to dispatch responses)
	// TODO Config entry to check whether the token matches the account. Disabling it would let external dispatch servers work without needing to sync account data between them. yagips and Grasscutter use different protocols for this, and extra work would need to be done for GIO's SDK server to exchange account data.
	// For now, we enforce it so everything can be in one place until the relevant protocols and storage structs are finalized.
#if 1
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
			// TODO Send a response packet first
			session.close(12);
			return -1;
		}
		player->setAccount(account);
		player->saveToDb();
	}
	session.setPlayer(player);
	// TODO Set use secret key
	// TODO Set session state: awaiting login
	rsp.set_retcode(0);
	rsp.set_msg("ok");
	rsp.set_uid(player->getUid());
	rsp.set_token(account->getToken());
	// TODO Check account if guest
	rsp.set_account_type(1);
	rsp.set_channel_id(1);
	// TODO Pull from session object?
	rsp.set_platform_type(3);
	// Unknown what this does.
	rsp.set_reg_platform(3);
	// TODO obtained from kcpSession->getClientIPAddress()
	// rsp.set_client_ip_str();
	// Unknown what this does.
	rsp.set_client_version_random_key("c25-314dd05b0b5f");
	// TODO either pull from account object or from GeoIP
	rsp.set_country_code("US");
	// TODO Grasscutter checks for avatar count (ie. whether you've created the Traveler's avatar object or not) to determine if this should be set. Figure out if we should do the same.
	rsp.set_is_proficient_player(0);
	rsp.set_secret_key_seed(encryptSeed);
	rsp.set_security_cmd_buffer((const char*) secretKeyBuf);
	if (req.key_id() > 0) {
/*
		std::string clientRandKeyEnc = b64dec(req.client_rand_key());
		char clientRandKeyBuf[4096];
		ssize_t clientRandKeySz = HyvCryptRsaDec(clientRandKeyEnc.c_str(), clientRandKeyEnc.size(), clientRandKeyBuf, 4096, 0);
		if (clientRandKeySz < 0) {
			// TODO modify and then send the response packet
			session.close();
			return -1;
		}
		// TODO assert size == sizeof(long long)
		// TODO figure out endianness
		unsigned long long clientRandKey = *(unsigned long long*) clientRandKeyBuf;
		unsigned long long serverRandKey = clientRandKey ^ encryptSeed;
		char serverRandKeyBuf[4096];
		ssize_t serverRandKeySz = HyvCryptRsaEnc((const char*) serverRandKey, sizeof(long long), serverRandKeyBuf, 4096, req.key_id());
		if (serverRandKeySz < 0) {
			// TODO modify and then send the response packet
			session.close();
			return -1;
		}
		char serverRandKeySignBuf[4096];
		ssize_t serverRandKeySignSz = HyvCryptRsaSign((const char*) serverRandKey, sizeof(long long), servetRandKeySignBuf, 4096);
		if (serverRandKeySignSz < 0) {
			// TODO modify and then send the response packet
			session.close();
			return -1;
		}
		std::string serverRandKeyS(serverRandKeyBuf, serverRandKeySz);
		std::string serverRandKeySignS(serverRandKeySignBuf, serverRandKeySignSz);
		std::string encryptSeedBuf(secretKeyBuf, 32);
		rsp.set_server_rand_key(b64enc(serverRandKeyS));
		rsp.set_sign(b64enc(serverRandKeySignS));
*/
	}
	if (!rsp.SerializeToString(&data)) {
		fprintf(stderr, "Error building packet data\n");
		return -1;
	}
	rsp_pkt.setHeader(header);
	rsp_pkt.setData(data);
	static unsigned char rsp_pkt_buf[1024];
	size_t rsp_pkt_sz = 1024;
	if (rsp_pkt.build(rsp_pkt_buf, &rsp_pkt_sz) < 0) {
		fprintf(stderr, "Error building packet\n");
		return -1;
	}
	size_t rawsz;
	const unsigned char* rawbuf = rsp_pkt.getBuffer(&rawsz);
	if (session.getKcpSession()->send(rawbuf, rawsz) < 0) {
		fprintf(stderr, "Error sending packet\n");
		return -1;
	}
	return 0;
}
