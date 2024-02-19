/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/random.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <stdexcept>
#include <thread>
#include "gameserver.h"
#include "dispatch.h"
#include "session.h"
#include "kcpsession.h"
#include "util.h"

Gameserver::Gameserver(const char* _ip, unsigned short _port) {
	ip = _ip;
	port = _port;
	fd = -1;
	struct in_addr addr4 = {INADDR_ANY};
	struct in6_addr addr6 = IN6ADDR_ANY_INIT;
	if (inet_pton(AF_INET6, ip, &addr6) == 0) {
		// v6 addr parse error. Try it with v4.
		if (inet_pton(AF_INET, ip, &addr4) != 1) {
			__attribute__((aligned(16))) char b[1024];
			snprintf(b, 1024, "Failed to parse the bind address %s", ip);
			throw std::invalid_argument(b);
		}
		// Special logic for certain specific v4 addresses.
		// Check for an address in loopback range.
		if (ntohl(addr4.s_addr) >> 24 == 127) {
			// Set the last byte of addr6 to 1, indicating IPv6 loopback.
			addr6.s6_addr[15] = 1;
		}
		// Check if not INADDR_ANY
		else if (ntohl(addr4.s_addr) != INADDR_ANY) {
			// Create a v4-mapped v6 address. TODO does it actually work this way?
			memcpy(&addr6.s6_addr[12], &addr4, sizeof(struct in_addr));
			addr6.s6_addr[10] = 0xff;
			addr6.s6_addr[11] = 0xff;
		}
	}
	addr = addr6;
	srv.sin6_family = AF_INET6;
	srv.sin6_port = htons(port);
	srv.sin6_addr = addr6;
	sessionList = NULL;
}

Gameserver::~Gameserver() {
	stop();
}

int Gameserver::start() {
	int opt = 0;
	fd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
	if (fd < 0) {
		fprintf(stderr, "Failed to create gameserver socket, errno = %d (%s)\n", errno, strerror(errno));
		return -1;
	}
	if (setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(int)) < 0) {
		fprintf(stderr, "Warning: Unable to unset option IPV6_V6ONLY (errno %d - %s).\nGameserver might only be able to accept IPv6 clients.\n", errno, strerror(errno));
	}
	if (bind(fd, (struct sockaddr*) &srv, sizeof(sock_t)) < 0) {
		fprintf(stderr, "Failed to bind gameserver socket to %s:%d, errno = %d (%s)\n", ip, port, errno, strerror(errno));
		close(fd);
		fd = -1;
		return -1;
	}
	fprintf(stderr, "Successfully bound gameserver to %s:%d\n", ip, port);
	sessionList = (Session**) malloc(sizeof(Session*) * maxSessions);
	if (sessionList == NULL) {
		fprintf(stderr, "Session array can't be allocated.\n");
		close(fd);
		fd = -1;
		return -1;
	}
	memset(sessionList, 0, sizeof(Session*) * maxSessions);
	return 0;
}

void Gameserver::stop() {
	unsigned int i;
	for (i = 0; i < maxSessions; i++) {
		if (sessionList == NULL) break;
		if (sessionList[i] == NULL) continue;
		sessionList[i]->close(6);
		delete sessionList[i];
	}
	// TODO: Wait for a timeout or until all sessions have shut down
	if (fd != -1) {
		close(fd);
		fd = -1;
	}
	if (sessionList != NULL) {
		free(sessionList);
		sessionList = NULL;
	}
}

ssize_t Gameserver::recv(unsigned char* msg, size_t sz) {
	return recv(NULL, msg, sz);
}

ssize_t Gameserver::recv(sock_t* client, unsigned char* msg, size_t sz) {
	// Note: This function doesn't do its own client tracking. That's up to the higher-level interfaces like KCP and the Session class.
	ssize_t ret;
	socklen_t ssz = 0;
	if (client != NULL) ssz = sizeof(sock_t);
	if (msg == NULL && sz != 0) return 0;
	if (fd < 0) {
		return 0;
	}
	ret = recvfrom(fd, msg, sz, 0, (struct sockaddr*) client, &ssz);
	if (ret < 0) {
		fprintf(stderr, "Warning: Attempt to receive data from a client returned error %d (%s)\n", errno, strerror(errno));
		return 0;
	}
	return ret;
}

ssize_t Gameserver::send(sock_t* client, const unsigned char* msg, size_t sz) {
	ssize_t ret;
	if (client == NULL) return 0;
	// We allow a null message to be sent as long as size is also zero.
	if (msg == NULL && sz != 0) return 0;
	if (fd < 0) return 0;
	ret = sendto(fd, msg, sz, 0, (struct sockaddr*) client, sizeof(sock_t));
	if (ret < 0) {
		fprintf(stderr, "Warning: Attempt to send data to client returned error %d (%s)\n", errno, strerror(errno));
		return -1;
	}
	if ((size_t) ret != sz) {
		fprintf(stderr, "Warning: Only able to send %ld bytes of data to the client instead of the expected %ld\n", ret, sz);
	}
	return ret;
}

int Gameserver::canRecv(unsigned long mills) {
	if (fd < 0) return 0;
	struct timeval tm;
	tm.tv_sec = mills / 1000;
	tm.tv_usec = (mills % 1000) * 1000;
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(fd, &fds);
	int ret = select(FD_SETSIZE, &fds, NULL, NULL, &tm);
	if (ret < 0) {
		fprintf(stderr, "Warning: Attempt to check for input on the socket returned error %d (%s)\n", errno, strerror(errno));
		return 0;
	}
	return ret;
}

unsigned long Gameserver::getKcpInterval() const {
	return kcpInterval;
}

void Gameserver::setKcpInterval(unsigned long i) {
	kcpInterval = i;
}

unsigned int Gameserver::getMaxSessions() const {
	return maxSessions;
}

void Gameserver::setMaxSessions(unsigned int i) {
	if (sessionList != NULL) {
		fprintf(stderr, "Warning: Not currently able to reallocate the session list while the server is running. Stop the server first and then try again.\n");
		return;
	}
	maxSessions = i;
}

Session** Gameserver::getSessions() const {
	return sessionList;
}

Session* Gameserver::getSession(unsigned int i) const {
	if (sessionList == NULL) return NULL;
	return sessionList[i];
}

Session* Gameserver::getSessionById(unsigned long long id) const {
	if (sessionList == NULL) return NULL;
	Session* session = NULL;
	KcpSession* kcpSession;
	unsigned int i;
	for (i = 0; i < maxSessions; i++) {
		kcpSession = sessionList[i]->getKcpSession();
		if (kcpSession == NULL) continue;
		if (kcpSession->getSessionId() == id) {
			session = sessionList[i];
			break;
		}
	}
	return session;
}

Session* Gameserver::getSessionByUid(unsigned int id) const {
	if (sessionList == NULL) return NULL;
	Session* session = NULL;
	Player* player;
	unsigned int i;
	for (i = 0; i < maxSessions; i++) {
		player = sessionList[i]->getPlayer();
		if (player == NULL) continue;
		if (player->getUid() == id) {
			session = sessionList[i];
			break;
		}
	}
	return session;
}

extern "C" {
	volatile int GameserverSignal = 0;
	static Gameserver* gs = NULL;

	Session* getSessionById(unsigned long long id) {
		if (gs == NULL) return NULL;
		return gs->getSessionById(id);
	}

	Session* getSessionByUid(unsigned int id) {
		if (gs == NULL) return NULL;
		return gs->getSessionByUid(id);
	}

	void* GameserverMain(__attribute__((unused)) void* __u) {
		const char* ip = "::";
		unsigned short port = 22102;
		unsigned int maxSessions = 1; // TODO Grab from config
		// TODO Swap out for pthreads
		std::thread** sessionThreads = (std::thread**) malloc(sizeof(std::thread*) * maxSessions);
		if (sessionThreads == NULL) {
			fprintf(stderr, "Session thread list can't be allocated.\n");
			DispatchServerSignal = 1; // TODO enum constant
			return (void*) -1;
		}
		memset(sessionThreads, 0, sizeof(std::thread*) * maxSessions);
		__attribute__((aligned(256))) static unsigned char pkt_buf[16 * 1024];
		__attribute__((aligned(16))) static unsigned char addr_buf[128];
		const unsigned int pkt_buf_size = 16 * 1024;
		ssize_t pkt_size = pkt_buf_size;
		try {
			gs = new Gameserver(ip, port);
		}
		catch (const std::exception& e) {
			fprintf(stderr, "Gameserver failed to init due to exception: %s\n", e.what());
			DispatchServerSignal = 1; // TODO enum constant
			return (void*) -1;
		}
		catch (...) {
			fprintf(stderr, "Gameserver failed to init due to some sort of exception\n");
			DispatchServerSignal = 1; // TODO enum constant
			return (void*) -1;
		}
		// TODO Grab KCP interval from the config and call the object's setter
		gs->setMaxSessions(maxSessions);
		if (gs->start() < 0) {
			fprintf(stderr, "Gameserver failed to start\n");
			delete gs;
			DispatchServerSignal = 1; // TODO enum constant
			return (void*) -1;
		}
		fprintf(stderr, "Gameserver started at %s:%d\n", ip, port);
		Session** sessionList = gs->getSessions();
		static sock_t client;
		while(!GameserverSignal) {
			// TODO Get from config
			if (!gs->canRecv(5)) continue;
			pkt_size = gs->recv(&client, pkt_buf, pkt_buf_size);
			if (pkt_size < 0) {
				fprintf(stderr, "Warning: Gameserver couldn't receive a packet.\n");
			}
			else if (pkt_size > 0) {
				if (pkt_size >= (ssize_t) sizeof(kcp_handshake_t)) {
					// Peek into the KCP header to see what session it belongs to. Also sets up the handshake parser.
					unsigned long long sid = ((unsigned long long*) pkt_buf)[0];
					kcp_handshake_t* hs = (kcp_handshake_t*) pkt_buf;
					KcpSession* kcpSession;
					unsigned int i;
					// Iterate through each session
					for (i = 0; i < maxSessions; i++) {
						// Does the session object even exist?
						if (sessionList[i] == NULL) continue;
						// Is the session active and connected?
						//if (sessionList[i].getState() == <some constant idk yet...>) continue;
						// Is there a corresponding KCP object?
						kcpSession = sessionList[i]->getKcpSession();
						if (kcpSession == NULL) continue;
						// Check the session ID.
						if (kcpSession->getSessionId() == sid) {
							// There's a match. At this point, forward the packet to the KCP session. The session thread will (hopefully) see the packet in full the next time it calls ikcp_recv.
							kcpSession->pushToKcp(pkt_buf, pkt_size);
							break;
						}
						// Check 4 bytes ahead in case the client sent a request to close the session, which uses the "handshake" format.
						sid = (unsigned long long) be32toh(hs->sid2) << 32;
						sid |= be32toh(hs->sid1);
						if (kcpSession->getSessionId() != sid) {
							// Client sometimes sends shutdown requests with the highest 32 bits of the session ID unset.
							if ((sid & 0xffffffff) != be32toh(hs->sid1) && be32toh(hs->sid2) != 0) {
								continue;
							}
						}
						// Check for a shutdown request
						if (hs->magic1 == htobe32(0x194) && hs->magic2 == htobe32(0x19419494)) {
							sessionList[i]->close(be32toh(hs->cmd));
							// TODO Once we check state constants, don't do this anymore
							delete sessionList[i];
							sessionList[i] = NULL;
							break;
						}
						// Unknown "handshake format" packet
						fprintf(stderr, "Debug: Hexdump of Packet:\n");
						DbgHexdump(pkt_buf, pkt_size);
						continue;
					}
					if (i >= maxSessions) {
						// No existing sessions matched. Try to see if a client is trying to open a new session. (Note that it's entirely possible that a client can send an oversized but still valid handshake packet.)
						if (hs->magic1 == htobe32(0xff) && hs->cmd == htobe32(1234567890) && hs->magic2 == htobe32(0xffffffff)) {
							getrandom(&sid, sizeof(unsigned long long), 0);
							// Open a new session.
							hs->magic1 = htobe32(0x145);
							hs->sid1 = htobe32(sid & 0xffffffff);
							hs->sid2 = htobe32(sid >> 32);
							hs->cmd = htobe32(1234567890);
							hs->magic2 = htobe32(0x14514545);
							pkt_size = gs->send(&client, pkt_buf, sizeof(kcp_handshake_t));
							if (pkt_size < (ssize_t) sizeof(kcp_handshake_t)) {
								inet_ntop(AF_INET6, client.sin6_addr.s6_addr, (char*) addr_buf, 128);
								fprintf(stderr, "Failed to send handshake back to client at %s:%d.\n", addr_buf, client.sin6_port);
							}
							else {
								for (i = 0; i < maxSessions; i++) {
									if (sessionList[i] == NULL) break;
									//if (sessionList[i].getState() == <some constant idk yet...>) break;
								}
								if (i >= maxSessions) {
									fprintf(stderr, "Reached the max amount of sessions, cannot accept a new one.\n");
									hs->magic1 = htobe32(0x194);
									hs->sid1 = htobe32(sid & 0xffffffff);
									hs->sid2 = htobe32(sid >> 32);
									hs->cmd = htobe32(5);
									hs->magic2 = htobe32(0x19419494);
									pkt_size = gs->send(&client, pkt_buf, sizeof(kcp_handshake_t));
									if (pkt_size < (ssize_t) sizeof(kcp_handshake_t)) {
										inet_ntop(AF_INET6, client.sin6_addr.s6_addr, (char*) addr_buf, 128);
										fprintf(stderr, "Failed to send close packet to client at %s:%d.\n", addr_buf, client.sin6_port);
									}
								}
								else {
									sessionList[i] = new Session(gs, &client, sid);
									inet_ntop(AF_INET6, client.sin6_addr.s6_addr, (char*) addr_buf, 128);
									fprintf(stderr, "Accepted new client from %s:%d\n", addr_buf, client.sin6_port);
									sessionThreads[i] = new std::thread(SessionMain, sessionList[i]);
								}
							}
						}
						else if (pkt_size >= 28) { // Unfortunately, if a packet is sized bigger than KCP's header, there's no viable way to check for valid KCP headers that don't belong to a connected client, compared to completely invalid packets altogether.
							fprintf(stderr, "Warning: Invalid packet received (or session id is not registered)\n");
							//fprintf(stderr, "Hexdump of Packet:\n");
							//DbgHexdump(pkt_buf, pkt_size);
						}
						else { // We know for sure the packet is bogus if it's not a valid handshake and its size is smaller than KCP's header.
							fprintf(stderr, "Warning: Invalid packet received.\n");
							//fprintf(stderr, "Hexdump of Packet:\n");
							//DbgHexdump(pkt_buf, pkt_size);
						}
					}
				}
				else { // Smaller than even a handshake packet, so it's definitely bogus
					fprintf(stderr, "Warning: Invalid packet received.\n");
					//fprintf(stderr, "Hexdump of Packet:\n");
					//DbgHexdump(pkt_buf, pkt_size);
				}
			}
			// else pkt_size == 0 (no-op)
		}
		// TODO Wait for either a timeout or a specific signal to allow session threads to clean themselves up gracefully
		gs->stop();
		delete gs;
		gs = NULL;
		DispatchServerSignal = 1; // TODO enum constant
		return (void*) 0;
	}
}
