/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/random.h>
#include "gameserver.h"
#include "kcpsession.h"
#include "session.h"
#include "player.h"
#include "packet.h"
#include "ec2b.h"
#include "crypt.h"
#include "keys.h"
#include "util.h"

Session::Session(Gameserver* gs, sock_t* sock, unsigned long long sid) {
	// TODO Null check on gs and sock
	// TODO enforce ip-level bans (send the close packet ourselves instead of calling close() since the kcp object won't have been created yet) (in addition to disapatch responses)
	kcpSession = new KcpSession(sid, sock, gs);
	player = NULL;
	use_secret_key = 0;
	sequence = 10;
	state = Session::TOKEN_WAIT;
	lastPingTime = curTimeMs();
}

Session::~Session() {
	close(12);
}

void Session::close(unsigned int reason) {
	kcp_handshake_t hs;
	unsigned long long sid;
	ssize_t pkt_size;
	if (kcpSession != NULL) {
		sid = kcpSession->getSessionId();
		fprintf(stderr, "Closing session 0x%08llx\n", sid);
		hs.magic1 = htobe32(0x194);
		hs.sid1 = htobe32(sid & 0xffffffff);
		hs.sid2 = htobe32(sid >> 32);
		hs.cmd = htobe32(reason);
		hs.magic2 = htobe32(0x19419494);
		pkt_size = kcpSession->sendRaw((const unsigned char*) &hs, sizeof(hs));
		if (pkt_size != (ssize_t) sizeof(hs)) {
			fprintf(stderr, "Warning: Failed to send close packet to client\n");
		}
		delete kcpSession;
		kcpSession = NULL;
	}
	if (player != NULL) {
		delete player;
		player = NULL;
	}
	state = Session::NOT_CONNECTED;
}

KcpSession* Session::getKcpSession() const {
	return kcpSession;
}

int Session::getState() const {
	return state;
}

void Session::setState(int i) {
	state = i;
}

Player* Session::getPlayer() const {
	return player;
}

const Account* Session::getAccount() const {
	if (player == NULL) return NULL;
	return player->getAccount();
}

void Session::setPlayer(Player* p) {
	player = p;
}

unsigned long long Session::getSessionSeed() const {
	return sessionSeed;
}

const unsigned char* Session::getSessionKey() const {
	return sessionKey;
}

void Session::generateSessionKey() {
	getrandom(&sessionSeed, sizeof(long long), 0);
	genXorpadFromSeed2(sessionSeed, sessionKey, 4096);
}

unsigned int Session::useSecretKey() const {
	return use_secret_key ? 1 : 0;
}

void Session::setUseSecretKey() {
	use_secret_key = 1;
}

void Session::setUseSecretKey(unsigned int i) {
	use_secret_key = i ? 1 : 0;
}

void Session::clearUseSecretKey() {
	use_secret_key = 0;
}

unsigned long long Session::getLastPingTime() const {
	return lastPingTime;
}

void Session::setLastPingTime(unsigned long long ping) {
	lastPingTime = ping;
}

void Session::updateLastPingTime() {
	lastPingTime = curTimeMs();
}

unsigned int Session::getSeq() const {
	return sequence;
}

void Session::setSeq(unsigned int seq) {
	sequence = seq;
}

unsigned int Session::nextSeq() {
	sequence++;
	return sequence;
}

int Session::sendPacket(Packet& packet) {
	static unsigned char buf[4096];
	size_t sz = 4096;
	if (packet.build(buf, &sz) < 0) {
		fprintf(stderr, "Error building packet\n");
		return -1;
	}
	const unsigned char* key = NULL;
	if (!packet.useDispatchKey() && use_secret_key) {
		key = sessionKey;
	}
#if 0
	// TODO: verify that the key being used is in fact query_curr_region->client_secret_key before using this.
	else {
		if (hasDispatchKey) key = dispatchKey;
	}
#endif
	if (key != NULL) {
		HyvCryptXor(buf, sz, key, 4096);
	}
	if (kcpSession->send(buf, sz) < 0) {
		fprintf(stderr, "Error sending packet\n");
		return -1;
	}
	return 0;
}

extern "C" {
	int SessionMain(Session* session) {
		__attribute__((aligned(256))) static unsigned char pkt_buf[16 * 1024];
		ssize_t pkt_size = 0;
		int packet_ret;
		KcpSession* kcp = session->getKcpSession();
		unsigned long long sessionId = kcp->getSessionId();
		Packet packet;
		std::string pkt_data;
		unsigned const char* key = NULL;
		fprintf(stderr, "Session 0x%08llx thread started\n", sessionId);
		const struct timespec w = {0, 50000000}; // 50 ms
		// TODO null checks
		while(session->getState() > Session::NOT_CONNECTED) {
			pkt_size = kcp->recv(pkt_buf, 16 * 1024);
			if (pkt_size >= 0) {
				if (session->useSecretKey()) {
					key = session->getSessionKey();
				}
#if 0
				// TODO: verify that the key being used is in fact query_curr_region->client_secret_key before using this.
				else {
					if (hasDispatchKey) key = dispatchKey;
				}
#endif
				if (key != NULL) {
					HyvCryptXor(pkt_buf, pkt_size, key, 4096);
				}
				if (packet.parse(pkt_buf, pkt_size) < 0) {
					fprintf(stderr, "Warning: Invalid packet received.\n");
					fprintf(stderr, "Hexdump of Packet Payload:\n");
					DbgHexdump((const unsigned char*) pkt_buf, pkt_size);
				}
				else {
					int isValid;
					packet_ret = processPacket(*session, packet, &isValid);
					if (!isValid) {
						fprintf(stderr, "Warning: Unhandled packet with ID %d\n", packet.getOpcode());
						pkt_data = packet.getHeader();
						fprintf(stderr, "Hexdump of Packet Header:\n");
						DbgHexdump((const unsigned char*) pkt_data.c_str(), pkt_data.size());
						pkt_data = packet.getData();
						fprintf(stderr, "Hexdump of Packet Payload:\n");
						DbgHexdump((const unsigned char*) pkt_data.c_str(), pkt_data.size());
					}
					else if (packet_ret < 0) {
						fprintf(stderr, "Warning: Handler for packet %d returned error %d\n", packet.getOpcode(), -packet_ret);
						if (session->getState() <= Session::NOT_CONNECTED) {
							break;
						}
					}
				}
			}
			// Else less than 0 - either no packets in queue, or an error occured
			kcp->update(50);
			// TODO Get ping timeout from config. for now, use a value of 30 seconds
			if (session->getLastPingTime() + (30 * 1000) < curTimeMs()) {
				fprintf(stderr, "Session 0x%08llx ping timeout\n", sessionId);
				// TODO Set state to TIMEOUT instead of closing right off the bat
				session->close(10);
				return 0;
			}
			nanosleep(&w, NULL);
		}
		session->close(3);
		return 0;
	}
}
