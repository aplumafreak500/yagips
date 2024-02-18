/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2023 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#ifndef KCPSESSION_H
#define KCPSESSION_H
#include <arpa/inet.h>
#include "gameserver.h"
#include "kcp.h"

class KcpSession {
public:
	KcpSession(unsigned long long, sock_t*, Gameserver*);
	~KcpSession();
	ssize_t send(const unsigned char*, size_t);
	ssize_t recv(unsigned char*, size_t);
	ssize_t sendRaw(const unsigned char*, size_t);
	ssize_t recvRaw(unsigned char*, size_t);
	ssize_t pushToKcp(const unsigned char*, size_t);
	unsigned long long getSessionId() const;
	struct in6_addr* getClientIPAddress() const;
	void update();
	void update(unsigned int);
private:
	Gameserver* gs;
	ikcpcb* kcp;
	sock_t* client;
};
#endif
