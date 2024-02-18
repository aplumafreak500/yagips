/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#ifndef PACKET_H
#define PACKET_H
#include <string>

class Packet;

#include "session.h"

#define PACKET_MAGIC1 0x4567
#define PACKET_MAGIC2 0x89ab

class Packet {
public:
	Packet();
	Packet(unsigned short);
	~Packet();
	int parse(const unsigned char*, size_t);
	int build();
	int build(unsigned char*, size_t*);
	const std::string& getHeader() const;
	const std::string& buildHeader(unsigned int);
	void setHeader(const std::string&);
	const std::string& getData() const;
	void setData(const std::string&);
	unsigned short getOpcode() const;
	void setOpcode(unsigned short);
	unsigned char* getBuffer(size_t*) const;
	unsigned int useDispatchKey() const;
	void setUseDispatchKey();
	void setUseDispatchKey(unsigned int);
	void clearUseDispatchKey();
private:
	unsigned short opcode;
	std::string header;
	std::string data;
	unsigned char* rawpkt_buf;
	size_t rawpkt_sz;
	unsigned int use_dispatch_key;
};

int processPacket(Session&, Packet&);
#endif
