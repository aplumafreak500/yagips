/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <arpa/inet.h>
#include <stdexcept>
#include "gameserver.h"
#include "kcpsession.h"
#include "kcp.h"

extern "C" {
	static int sendFromKcp(const unsigned char*, int, ikcpcb*, KcpSession*);
}

KcpSession::KcpSession(unsigned long long id, sock_t* _client, Gameserver* _gs) {
	if (_gs == NULL) {
		throw std::invalid_argument("Gameserver reference missing.");
	}
	gs = _gs;
	if (_client == NULL) {
		throw std::invalid_argument("Client socket not provided.");
	}
	client = _client;
	kcp = ikcp_create(id, this);
	if (kcp == NULL) {
		throw std::runtime_error("KCP object couldn't be created.");
	}
	// Just ignore the nasty cast, please please please...
	ikcp_setoutput(kcp, (int(*)(const char*, int, ikcpcb*, void*)) sendFromKcp);
	ikcp_nodelay(kcp, 1, gs->getKcpInterval(), 2, 1);
	ikcp_setmtu(kcp, 1400);
	ikcp_wndsize(kcp, 256, 256);
}

KcpSession::~KcpSession() {
	if (kcp != NULL) ikcp_release(kcp);
	kcp = NULL;
}

ssize_t KcpSession::send(const unsigned char* buf, size_t len) {
	if (buf == NULL && len != 0) return -1;
	if (kcp == NULL) return sendRaw(buf, len);
	return ikcp_send(kcp, (char*) buf, len);
}

ssize_t KcpSession::sendRaw(const unsigned char* buf, size_t len) {
	if (client == NULL) return -1;
	if (buf == NULL && len != 0) return -1;
	if (gs == NULL) return -1;
	return gs->send(client, buf, len);
}

ssize_t KcpSession::recv(unsigned char* buf, size_t len) {
	if (buf == NULL && len != 0) return -1;
	if (kcp == NULL) return recvRaw(buf, len);
	return ikcp_recv(kcp, (char*) buf, len);
}

ssize_t KcpSession::recvRaw(unsigned char* buf, size_t len) {
	if (buf == NULL && len != 0) return -1;
	if (gs == NULL) return -1;
	return gs->recv(buf, len);
}

ssize_t KcpSession::pushToKcp(const unsigned char* buf, size_t len) {
	if (buf == NULL && len != 0) return -1;
	if (kcp == NULL) return -1;
	return ikcp_input(kcp, (const char*) buf, len);
}

unsigned long long KcpSession::getSessionId() const {
	if (kcp == NULL) return 0;
	return kcp->conv;
}

struct in6_addr* KcpSession::getClientIPAddress() const {
	return &(client->sin6_addr);
}

void KcpSession::update() {
	unsigned int ms = 100;
	if (gs != NULL) ms = gs->getKcpInterval();
	update(ms);
}

void KcpSession::update(unsigned int ms) {
	if (kcp == NULL) return;
	static unsigned int now = 0;
	unsigned int next = ikcp_check(kcp, now);
	if (next <= now) ikcp_update(kcp, now);
	now += ms;
}

extern "C" {
	static int sendFromKcp(const unsigned char* buf, int len, __attribute__((unused)) ikcpcb* kcp, KcpSession* session) {
		if (session == NULL) return -1;
		if (buf == NULL && len != 0) return -1;
		// TODO: what about the `kcp` arg? Do we need to do anything with it?
		return session->sendRaw(buf, len);
	}
}
