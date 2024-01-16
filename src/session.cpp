/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2023 Alex Pensinger (ArcticLuma113)

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
#include "packet.h"
#include "ec2b.h"
#include "util.h"

Session::Session(Gameserver* gs, sock_t* sock, unsigned long long sid) {
	// TODO Null check on gs and sock
	kcpSession = new KcpSession(sid, sock, gs);
	// state = <some constant idk...>
}

Session::~Session() {
	close(12);
}

void Session::close(unsigned int reason) {
	kcp_handshake_t hs;
	unsigned long long sid;
	ssize_t pkt_size;
	// TODO Better state check
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
	state = 1; // <some constant idk...>
}

KcpSession* Session::getKcpSession() {
	return kcpSession;
}

unsigned int Session::getState() {
	return state;
}

void Session::setState(unsigned int i) {
	state = i;
}

unsigned long long Session::getSessionSeed() const {
	return sessionSeed;
}

const unsigned char* Session::getSessionKey() const {
	return sessionKey;
}

void Session::generateSessionKey() {
	getrandom(&sessionSeed, sizeof(long long), 0);
	genXorpadFromSeed(sessionSeed, sessionKey, 4096);
}

extern "C" {
	int SessionMain(Session* session) {
		__attribute__((aligned(256))) static unsigned char pkt_buf[16 * 1024];
		ssize_t pkt_size = 0;
		KcpSession* kcp = session->getKcpSession();
		fprintf(stderr, "Session 0x%08llx thread started\n", kcp->getSessionId());
		const struct timespec w = {0, 50000000}; // 50 ms
		// TODO null checks
		while(session->getState() != 1 /*TODO session close signal constant*/) {
			pkt_size = kcp->recv(pkt_buf, 16 * 1024);
			if (pkt_size >= 0) {
				fprintf(stderr, "Debug: Hexdump of Packet:\n");
				DbgHexdump(pkt_buf, pkt_size);
				// TODO Process the packet
			}
			// Else less than 0 - either no packets in queue, or an error occured
			kcp->update(50);
			nanosleep(&w, NULL);
		}
		session->close(3);
		return 0;
	}
}
