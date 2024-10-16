/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <endian.h>
#include "util.h"
#include "packet.h"
#include "packet_head.pb.h"

Packet::Packet() {
	rawpkt_buf = NULL;
	rawpkt_sz = 0;
	opcode = 0;
}

Packet::Packet(unsigned short opc) {
	rawpkt_buf = NULL;
	rawpkt_sz = 0;
	opcode = opc;
}

Packet::~Packet() {}

int Packet::parse(const unsigned char* buf, size_t sz) {
	if (buf == NULL) {
		fprintf(stderr, "Buffer is null\n");
		return -1;
	}
	if (sz < 10) {
		fprintf(stderr, "Invalid packet size 0x%08lx\n", sz);
		return -1;
	}
	unsigned short magic = be16toh(((unsigned short*) buf)[0]);
	if (magic != PACKET_MAGIC1) {
		fprintf(stderr, "Invalid magic constant 0x%04x (expected 0x%04x)\n", magic, PACKET_MAGIC1);
		return -1;
	}
	opcode = be16toh(((unsigned short*) buf)[1]);
	size_t hdr_sz = be16toh(((unsigned short*) buf)[2]);
	if (sz < hdr_sz + 10) {
		fprintf(stderr, "Invalid packet size 0x%08lx\n", sz);
		return -1;
	}
	// we do it this way to prevent unaligned memory access
	unsigned short data_sz_hi = be16toh(((unsigned short*) buf)[3]);
	unsigned short data_sz_lo = be16toh(((unsigned short*) buf)[4]);
	size_t data_sz = (data_sz_hi << 16) | data_sz_lo;
	if (sz < data_sz + hdr_sz + 12) {
		fprintf(stderr, "Invalid packet size 0x%08lx\n", sz);
		return -1;
	}
	// magic2 can be unaligned in some cases, so read it byte by byte
	unsigned char magic2_hi = ((unsigned char*) buf)[data_sz + hdr_sz + 10];
	unsigned char magic2_lo = ((unsigned char*) buf)[data_sz + hdr_sz + 11];
	magic = (magic2_hi << 8) | magic2_lo;
	if (magic != PACKET_MAGIC2) {
		fprintf(stderr, "Invalid magic constant 0x%04x (expected 0x%04x)\n", magic, PACKET_MAGIC2);
		return -1;
	}
	header.assign((const char*) buf + 10, hdr_sz);
	data.assign((const char*) buf + hdr_sz + 10, data_sz);
	rawpkt_buf = (unsigned char*) buf;
	rawpkt_sz = sz;
	return 0;
}

int Packet::build() {
	if (rawpkt_buf == NULL) return -1;
	return build(rawpkt_buf, &rawpkt_sz);
}

int Packet::build(unsigned char* buf, size_t* sz) {
	if (buf == NULL) return -1;
	if (sz == NULL) return -1;
	*sz = 12 + header.size() + data.size();
	unsigned char* pos = buf;
	*(unsigned short*) pos = htobe16(PACKET_MAGIC1);
	pos += sizeof(short);
	*(unsigned short*) pos = htobe16(opcode);
	pos += sizeof(short);
	*(unsigned short*) pos = htobe16(header.size());
	pos += sizeof(short);
	// we do it this way to prevent unaligned memory access
	*(unsigned short*) pos = htobe16(data.size() >> 16);
	pos += sizeof(short);
	*(unsigned short*) pos = htobe16(data.size() & 0xffff);
	pos += sizeof(short);
	memcpy(pos, header.c_str(), header.size());
	pos += header.size();
	memcpy(pos, data.c_str(), data.size());
	pos += data.size();
	// magic2 can be unaligned in some cases, so write it byte by byte
	*(unsigned char*) pos = PACKET_MAGIC2 >> 8;
	pos += sizeof(char);
	*(unsigned char*) pos = PACKET_MAGIC2 & 0xff;
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
	head_p.set_sent_ms(curTimeMs());
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

unsigned char* Packet::getBuffer(size_t* sz) const {
	if (sz != NULL) {
		*sz = rawpkt_sz;
	}
	return rawpkt_buf;
}

unsigned int Packet::useDispatchKey() const {
	return use_dispatch_key ? 1 : 0;
}

void Packet::setUseDispatchKey() {
	use_dispatch_key = 1;
}

void Packet::setUseDispatchKey(unsigned int i) {
	use_dispatch_key = i ? 1 : 0;
}

void Packet::clearUseDispatchKey() {
	use_dispatch_key = 0;
}
