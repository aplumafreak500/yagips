/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2023 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <string.h>
#include <sys/types.h>
#include <time.h>
#include "packet.h"
#include "packet_head.pb.h"

Packet::Packet() {}
Packet::~Packet() {}

int Packet::parse(const unsigned char* buf, size_t sz) {
	if (buf == NULL) return -1;
	if (sz < 8) return -1;
	// TODO Decrypt
	if (((unsigned short*) buf)[0] != htobe16(PACKET_MAGIC1)) {
		return -1;
	}
	opcode = be16toh(((unsigned short*) buf)[1]);
	size_t hdr_sz = be16toh(((unsigned short*) buf)[2]);
	if (sz < hdr_sz + 8) {
		return -1;
	}
	// NASTY. TODO Find a better way to do this
	size_t data_sz = be32toh(*(unsigned int*)(&((unsigned short*) buf)[3]));
	if (sz < data_sz + hdr_sz + 10) {
		return -1;
	}
	if (((unsigned short*) buf)[data_sz + hdr_sz + 8] != htobe16(PACKET_MAGIC2)) {
		return -1;
	}
	header.assign((const char*) buf + 8, hdr_sz);
	data.assign((const char*) buf + hdr_sz + 8, data_sz);
	encrypted = 0;
	useDispatchKey = 0;
	return 0;
}

int Packet::build(unsigned char* buf, size_t* sz) const {
	if (buf == NULL) return -1;
	if (sz == NULL) return -1;
	if (header.empty()) return -1;
	*sz = 10 + header.size() + data.size();
	unsigned char* pos = buf;
	*(unsigned short*) pos = htobe16(PACKET_MAGIC1);
	pos += sizeof(short);
	*(unsigned short*) pos = htobe16(opcode);
	pos += sizeof(short);
	*(unsigned short*) pos = htobe16(header.size());
	pos += sizeof(short);
	*(unsigned int*) pos = htobe32(data.size());
	pos += sizeof(int);
	memcpy(pos, header.c_str(), header.size());
	pos += header.size();
	memcpy(pos, data.c_str(), data.size());
	pos += data.size();
	*(unsigned short*) pos = htobe16(PACKET_MAGIC2);
	// TODO Encrypt
	return 0;
}

const std::string& Packet::getHeader() const {
	return header;
}

void Packet::setHeader(const std::string& hdr) {
	header = hdr;
}

const std::string& Packet::buildHeader(unsigned int seq) {
	if (!header.empty() && seq == 0) {
		return header;
	}
	proto::PacketHead head_p;
	std::string ret;
	head_p.set_client_sequence_id(seq);
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	head_p.set_sent_ms((ts.tv_sec * 1000) + (ts.tv_nsec / 1000000));
	if (head_p.SerializeToString(&ret)) header = ret;
	return header;
}

const std::string& Packet::getData() const {
	return data;
}

void Packet::setData(const std::string& d) {
	data = d;
}

unsigned short Packet::getOpcode() const {
	return opcode;
}

void Packet::setOpcode(unsigned short opc) {
	opcode = opc;
}
